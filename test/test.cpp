/*
 * Copyright (c) 2014, Pierre-Yves Martin
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "llvm/Support/TargetSelect.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h" // needed to prevent "Interpreter has not been linked in" when creating JIT engine
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include <chrono>
#include <string>
#include <memory>

#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/codegenerator.h"
#include "../src/ast.h"

// TESTS /////////////////////////////////////
//maximum difference between 2 floats to be considered equal
const float F_EPSILON = std::numeric_limits<float>::min() * 10.0;

TEST_CASE("Environment works") {
    INFO( "This means we have a working developping environment." );
}

TEST_CASE("Lexer categorise float litteral") {
    // stream to parse by lexer
    std::stringstream test;

    // the lexer
    Lexer lex = Lexer(test);

    // random generator initialisation
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine engine(seed);

    // float generator
    std::uniform_real_distribution<float> floatDistribution(0.0, std::numeric_limits<float>::max());
    auto randFloatGenerator = std::bind(floatDistribution, engine);

    // unsigned char generator
    std::uniform_int_distribution<unsigned char> ucharDistribution(0, std::numeric_limits<unsigned char>::max());
    auto randUCharGenerator = std::bind(ucharDistribution, engine);

    SECTION("1.0 is categorized as float litteral") {
        test << "1.0";

        int tokId = lex.getNextToken();

        REQUIRE(tokId == Lexer::TOK_FLOAT);
        float fVal;
        REQUIRE(lex.getFloatValue(fVal));
        REQUIRE(fVal == 1.0);
    }

    SECTION("Any float between 0.0 and max float is categorized as float litteral") {
        float f = randFloatGenerator();

        test << std::to_string(f);

        int tokId = lex.getNextToken();

        float fVal;
        REQUIRE(lex.getFloatValue(fVal));
        REQUIRE(abs(fVal - f) < F_EPSILON);
    }
}

TEST_CASE("Parser generate AST for float litteral") {
    SECTION("Parsing a float litteral expression generate a float litteral expression AST node") {
        std::stringstream test;         // stream to parse by lexer
        Lexer lex = Lexer(test);        // the lexer
        Parser parser = Parser(lex);    // the parser

        test << "1.0";

        lex.getNextToken();
        FloatLitteralExprAST* ast = parser.parseFloatLitteralExpr();

        REQUIRE(ast != nullptr);
        REQUIRE(ast->getAstType() == AstType::FLOAT_LITTERAL);
        REQUIRE(ast->getValue() == 1.0);
    }

    SECTION("Parser recognise float litteral expression as a top level expression") {
        std::stringstream test;         // stream to parse by lexer
        Lexer lex = Lexer(test);        // the lexer
        Parser parser = Parser(lex);    // the parser

        test << "1.0";

        lex.getNextToken();
        FunctionAST* ast = parser.parseTopLevelExpr();

        // Do we have an float expression ?
        // i.e. an anonymous function returning a float
        REQUIRE(ast != nullptr);

        REQUIRE(ast->getPrototype() != nullptr);
        REQUIRE(ast->getPrototype()->getAstType() == AstType::PROTOTYPE);
        ProtoTypeAST* protoAst = static_cast<ProtoTypeAST*>(ast->getPrototype());
        REQUIRE(protoAst->getName() == "");
        REQUIRE(protoAst->getArgs().empty());

        REQUIRE(ast->getBody() != nullptr);
        REQUIRE(ast->getBody()->getAstType() == AstType::FLOAT_LITTERAL);
        FloatLitteralExprAST* bodyAst = static_cast<FloatLitteralExprAST*>(ast->getBody());
        REQUIRE(bodyAst->getValue() == 1.0);
    }
}

TEST_CASE("Code generated for float litteral expression is correct") {
    // LLVM stuff
    // TODO: put all this in a class... but witch one
    llvm::InitializeNativeTarget();
    llvm::LLVMContext& context = llvm::getGlobalContext();
    llvm::Module* module = new llvm::Module("Test module", context); // Make the module, which holds all the code.

    if (module == nullptr) {
        fprintf(stderr, "no module\n");
    }

    // non LLVM stuff
    std::stringstream test;                     // stream to parse by lexer
    Lexer lex = Lexer(test);                    // the lexer
    Parser parser = Parser(lex);                // the parser
    CodeGenerator gen = CodeGenerator(module);  // the code generator

    // create a JIT Engine (This takes ownership of the module).
    std::string errStr;
    llvm::ExecutionEngine* execEngine = llvm::EngineBuilder(module).setErrorStr(&errStr).setEngineKind(llvm::EngineKind::JIT).create();
    if (execEngine == nullptr) {
        fprintf(stderr, "no exec engine: %s\n", errStr.c_str());
    }

    test << "1.0";

    lex.getNextToken();
    FunctionAST* ast = parser.parseTopLevelExpr();
    llvm::Value* code = gen.codeGen(ast);

    REQUIRE(code != nullptr);
    //TODO add some test about the module content

    llvm::Function* funcCode = static_cast<llvm::Function*>(code);
    const std::vector<llvm::GenericValue> noArgs = std::vector<llvm::GenericValue>();

    float result = execEngine->runFunction(funcCode, noArgs).FloatVal;
    REQUIRE(result == 1.0);
}

TEST_CASE("Lexer categorise keyword let") {
    // stream to parse by lexer
    std::stringstream test;

    // the lexer
    Lexer lex = Lexer(test);

    test << "let";

    int tokId = lex.getNextToken();

    REQUIRE(tokId == Lexer::TOK_KEYWORD_LET);
}

TEST_CASE("Lexer categorise identifier") {
    // stream to parse by lexer
    std::stringstream test;

    // the lexer
    Lexer lex = Lexer(test);

    SECTION("Lexer categorise a lowercaser identifier") {
        std::string identifier = "abc";

        test << identifier;

        int tokId = lex.getNextToken();

        CHECK(tokId == Lexer::TOK_IDENTIFIER);
        std::string idStr;
        CHECK(lex.getIdentifierString(idStr));
        CHECK(idStr == identifier);
    }

    SECTION("Lexer categorise an uppercase identifier") {
        std::string identifier = "ABC";

        test << identifier;

        int tokId = lex.getNextToken();

        CHECK(tokId == Lexer::TOK_IDENTIFIER);
        std::string idStr;
        CHECK(lex.getIdentifierString(idStr));
        CHECK(idStr == identifier);
    }

    SECTION("Lexer categorise identifier with leading underscore") {
        std::string identifier = "_ab";

        test << identifier;

        int tokId = lex.getNextToken();

        CHECK(tokId == Lexer::TOK_IDENTIFIER);
        std::string idStr;
        CHECK(lex.getIdentifierString(idStr));
        CHECK(idStr == identifier);
    }

    SECTION("Lexer categorise identifier with underscore") {
        std::string identifier = "a_b";

        test << identifier;

        int tokId = lex.getNextToken();

        CHECK(tokId == Lexer::TOK_IDENTIFIER);
        std::string idStr;
        CHECK(lex.getIdentifierString(idStr));
        CHECK(idStr == identifier);
    }

    SECTION("Lexer categorise identifier with numbers") {
        std::string identifier = "a2b";

        test << identifier;

        int tokId = lex.getNextToken();

        CHECK(tokId == Lexer::TOK_IDENTIFIER);
        std::string idStr;
        CHECK(lex.getIdentifierString(idStr));
        CHECK(idStr == identifier);
    }
}

TEST_CASE("Lexer categorise affectation operator") {
    // stream to parse by lexer
    std::stringstream test;

    // the lexer
    Lexer lex = Lexer(test);

    test << "=";

    int tokId = lex.getNextToken();

    CHECK(tokId == Lexer::TOK_OPERATOR_AFFECTATION);
}

TEST_CASE("Lexer handles whitespaces") {
    // stream to parse by lexer
    std::stringstream test;

    // the lexer
    Lexer lex = Lexer(test);

    SECTION("Lexer does not skip EOF after a token") {
        test << "1.0";

        int tokId = lex.getNextToken();

        CHECK(tokId == Lexer::TOK_FLOAT);
        float fVal;
        REQUIRE(lex.getFloatValue(fVal));
        CHECK(fVal == 1.0);

        tokId = lex.getNextToken();

        CHECK(tokId == Lexer::TOK_EOF);
    }

    SECTION("Lexer skip one whitespace between tokens") {
        test << "1.0 2.0";

        int tokId = lex.getNextToken();
        float fVal;

        CHECK(tokId == Lexer::TOK_FLOAT);
        REQUIRE(lex.getFloatValue(fVal));
        CHECK(fVal == 1.0);

        tokId = lex.getNextToken();

        CHECK(tokId == Lexer::TOK_FLOAT);
        REQUIRE(lex.getFloatValue(fVal));
        CHECK(fVal == 2.0);

        tokId = lex.getNextToken();

        CHECK(tokId == Lexer::TOK_EOF);
    }

    // TODO test multiple whitespace between tokens
}

TEST_CASE("Parser generate AST for litteral constant declaration") {
    std::stringstream test;         // stream to parse by lexer
    Lexer lex = Lexer(test);        // the lexer
    Parser parser = Parser(lex);    // the parser

    test << "let aaa = 1.0";

    SECTION("Parsing a litteral constant declaration generate a float expression AST node") {
        lex.getNextToken();
        FloatConstantVariableDeclarationExprAST* declarationAst = parser.parseFloatConstantVariableDeclarationExpr();

        REQUIRE(declarationAst != nullptr);
        CHECK(declarationAst->getAstType() == AstType::FLOAT_CONSTANT_VARIABLE_DECLARATION);
        CHECK(declarationAst->getName() == "aaa");
        REQUIRE(declarationAst->getRhsExpr() != nullptr);
        REQUIRE(declarationAst->getRhsExpr()->getAstType() == AstType::FLOAT_LITTERAL);
        FloatLitteralExprAST* rhsAst = static_cast<FloatLitteralExprAST*>(declarationAst->getRhsExpr());
        CHECK(rhsAst->getValue() == 1.0);
    }

    SECTION("Parser recognise litteral constant declaration as a top level expression") {
        lex.getNextToken();
        ExprAST* ast = parser.parseTopLevelExpr();

        // Do we have an float expression ?
        // i.e. an anonymous function returning a float
        REQUIRE(ast != nullptr);
        REQUIRE(ast->getAstType() == AstType::FUNCTION);

        FunctionAST* funcAst = static_cast<FunctionAST*>(ast);

        REQUIRE(funcAst->getPrototype() != nullptr);
        REQUIRE(funcAst->getPrototype()->getAstType() == AstType::PROTOTYPE);
        ProtoTypeAST* protoAst = static_cast<ProtoTypeAST*>(funcAst->getPrototype());
        CHECK(protoAst->getName() == "");
        CHECK(protoAst->getArgs().empty());

        REQUIRE(funcAst->getBody() != nullptr);
        REQUIRE(funcAst->getBody()->getAstType() == AstType::FLOAT_CONSTANT_VARIABLE_DECLARATION);

        // now check the content of the body
        FloatConstantVariableDeclarationExprAST* bodyAst = static_cast<FloatConstantVariableDeclarationExprAST*>(funcAst->getBody());
        CHECK(bodyAst->getName() == "aaa");
        REQUIRE(bodyAst->getRhsExpr() != nullptr);
        REQUIRE(bodyAst->getRhsExpr()->getAstType() == AstType::FLOAT_LITTERAL);
        FloatLitteralExprAST* rhsAst = static_cast<FloatLitteralExprAST*>(bodyAst->getRhsExpr());
        CHECK(rhsAst->getValue() == 1.0);
    }

    //TODO test constant declaration with an expr as value
}

TEST_CASE("Code generated for float litteral constant declaration expression is correct") {
    // LLVM stuff
    // TODO: put all this in a class... but witch one
    llvm::InitializeNativeTarget();
    llvm::LLVMContext& context = llvm::getGlobalContext();
    llvm::Module* module = new llvm::Module("Test module", context); // Make the module, which holds all the code.

    if (module == nullptr) {
        fprintf(stderr, "no module\n");
    }

    // non LLVM stuff
    std::stringstream test;                     // stream to parse by lexer
    Lexer lex = Lexer(test);                    // the lexer
    Parser parser = Parser(lex);                // the parser
    CodeGenerator gen = CodeGenerator(module);  // the code generator

    // create a JIT Engine (This takes ownership of the module).
    std::string errStr;
    llvm::ExecutionEngine* execEngine = llvm::EngineBuilder(module).setErrorStr(&errStr).setEngineKind(llvm::EngineKind::JIT).create();
    if (execEngine == nullptr) {
        fprintf(stderr, "no exec engine: %s\n", errStr.c_str());
    }

    test << "let aaa = 1.0";

    lex.getNextToken();
    FunctionAST* ast = parser.parseTopLevelExpr();
    llvm::Value* code = gen.codeGen(ast);

    REQUIRE(code != nullptr);
    //TODO add some test about the module content
    //     symbol table content ?

    llvm::Function* funcCode = static_cast<llvm::Function*>(code);
    const std::vector<llvm::GenericValue> noArgs = std::vector<llvm::GenericValue>();

    float result = execEngine->runFunction(funcCode, noArgs).FloatVal;
    REQUIRE(result == 1.0);
}

// TODO add a test with usage of the created constant

TEST_CASE("New lexer initially has no token") {
    std::stringstream in;               // stream to parse by lexer
    TokenQueue out;                     // token conatainer
    NewLexer lex = NewLexer(in, out);   // the lexer

    REQUIRE(out.empty());
}

TEST_CASE("New lexer do not fill tokens when tokenise is called on empty input") {
    std::stringstream in;               // stream to parse by lexer
    TokenQueue out;                     // token conatainer
    NewLexer lex = NewLexer(in, out);   // the lexer

    CHECK(lex.tokenize() == 0);
    CHECK(out.size() == 0);
}

TEST_CASE("New lexer fills tokens when tokenise is called") {
    std::stringstream in;               // stream to parse by lexer
    TokenQueue out;                     // token conatainer
    NewLexer lex = NewLexer(in, out);   // the lexer

    in << "let aaa = 1.0";

    CHECK(lex.tokenize() == 4);
    CHECK(out.size() == 4);

    CHECK(out.at(0).getTokenType() == TokenType::TOK_KEYWORD_LET);

    CHECK(out.at(1).getTokenType() == TokenType::TOK_IDENTIFIER);
    CHECK(out.at(1).getIdentifierString() == "aaa");

    CHECK(out.at(2).getTokenType() == TokenType::TOK_OPERATOR_AFFECTATION);

    CHECK(out.at(3).getTokenType() == TokenType::TOK_LITTERAL_FLOAT);
    CHECK(out.at(3).getFloatValue() == 1.0);
}

TEST_CASE("New lexer eats token FIFO until there are no more token") {
    std::stringstream in;               // stream to parse by lexer
    TokenQueue out;                     // token conatainer
    NewLexer lex = NewLexer(in, out);   // the lexer

    in << "let aaa = 1.0";

    CHECK(lex.tokenize() == 4);
    CHECK(out.size() == 4);
    CHECK(out.front().getTokenType() == TokenType::TOK_KEYWORD_LET);

    CHECK(out.pop() == true);
    CHECK(out.size() == 3);
    CHECK(out.front().getTokenType() == TokenType::TOK_IDENTIFIER);
    CHECK(out.front().getIdentifierString() == "aaa");

    CHECK(out.pop() == true);
    CHECK(out.size() == 2);
    CHECK(out.front().getTokenType() == TokenType::TOK_OPERATOR_AFFECTATION);

    CHECK(out.pop() == true);
    CHECK(out.size() == 1);
    CHECK(out.front().getTokenType() == TokenType::TOK_LITTERAL_FLOAT);
    CHECK(out.front().getFloatValue() == 1.0);

    CHECK(out.pop() == true);
    CHECK(out.size() == 0);
    CHECK(out.empty() == true);

    CHECK(out.pop() == false);
    CHECK(out.size() == 0);
    CHECK(out.empty() == true);
}

TEST_CASE("New lexer can eats token with type verification") {
    std::stringstream in;               // stream to parse by lexer
    TokenQueue out;                     // token conatainer
    NewLexer lex = NewLexer(in, out);   // the lexer

    in << "let aaa = 1.0";

    // Before eating
    CHECK(lex.tokenize() == 4);
    CHECK(out.size() == 4);
    CHECK(out.front().getTokenType() == TokenType::TOK_KEYWORD_LET);

    // Trying to eat the wrong type of token
    CHECK(out.pop(TokenType::TOK_LITTERAL_FLOAT) == false);
    CHECK(out.size() == 4);
    CHECK(out.front().getTokenType() == TokenType::TOK_KEYWORD_LET);

    // Trying to eat the correct type of token
    CHECK(out.pop(TokenType::TOK_KEYWORD_LET) == true);
    CHECK(out.size() == 3);
    CHECK(out.front().getTokenType() == TokenType::TOK_IDENTIFIER);
    CHECK(out.front().getIdentifierString() == "aaa");
}

TEST_CASE("TokenQueue can be pushed and poped", "[tokenQ]") {
    TokenQueue tokenQ;

    CHECK(tokenQ.empty());
    CHECK(tokenQ.size() == 0);
    CHECK(tokenQ.pop() == false);

    SECTION("Pushing a token actualy add a token to the queue") {
        tokenQ.push(Token::CreateLitteralFloat(1.0));

        CHECK(not tokenQ.empty());
        CHECK(tokenQ.size() == 1);
        CHECK(tokenQ.front().getTokenType() == TokenType::TOK_LITTERAL_FLOAT);
        CHECK(tokenQ.front().getFloatValue() == 1.0);
    }

    SECTION("Poping a token actualy remove a token from the queue") {
        tokenQ.push(Token::CreateLitteralFloat(1.0));

        CHECK(tokenQ.pop() == true);

        CHECK(tokenQ.empty());
        CHECK(tokenQ.size() == 0);
    }

    SECTION("Tokens are push to the back of the queue") {
        tokenQ.push(Token::CreateLitteralFloat(2.0));

        CHECK(tokenQ.back().getTokenType() == TokenType::TOK_LITTERAL_FLOAT);
        CHECK(tokenQ.back().getFloatValue() == 2.0);

        tokenQ.push(Token::CreateLitteralFloat(3.0));

        CHECK(tokenQ.back().getTokenType() == TokenType::TOK_LITTERAL_FLOAT);
        CHECK(tokenQ.back().getFloatValue() == 3.0);

        tokenQ.push(Token::CreateLitteralFloat(4.0));

        CHECK(tokenQ.back().getTokenType() == TokenType::TOK_LITTERAL_FLOAT);
        CHECK(tokenQ.back().getFloatValue() == 4.0);

        // check the final result
        REQUIRE(tokenQ.size() == 3);
        CHECK(tokenQ.at(0).getTokenType() == TokenType::TOK_LITTERAL_FLOAT);
        CHECK(tokenQ.at(0).getFloatValue() == 2.0);
        CHECK(tokenQ.at(1).getTokenType() == TokenType::TOK_LITTERAL_FLOAT);
        CHECK(tokenQ.at(1).getFloatValue() == 3.0);
        CHECK(tokenQ.at(2).getTokenType() == TokenType::TOK_LITTERAL_FLOAT);
        CHECK(tokenQ.at(2).getFloatValue() == 4.0);
    }

    SECTION("Tokens are poped from the front of the queue") {
        // put some few things in the queue
        tokenQ.push(Token::CreateLitteralFloat(2.0));
        tokenQ.push(Token::CreateLitteralFloat(3.0));
        tokenQ.push(Token::CreateLitteralFloat(4.0));

        // now let's pop them one by one
        REQUIRE(tokenQ.size() == 3);
        CHECK(tokenQ.front().getTokenType() == TokenType::TOK_LITTERAL_FLOAT);
        CHECK(tokenQ.front().getFloatValue() == 2.0);

        CHECK(tokenQ.pop() == true);

        CHECK(tokenQ.size() == 2);
        CHECK(tokenQ.front().getTokenType() == TokenType::TOK_LITTERAL_FLOAT);
        CHECK(tokenQ.front().getFloatValue() == 3.0);

        CHECK(tokenQ.pop() == true);

        CHECK(tokenQ.size() == 1);
        CHECK(tokenQ.front().getTokenType() == TokenType::TOK_LITTERAL_FLOAT);
        CHECK(tokenQ.front().getFloatValue() == 4.0);

        CHECK(tokenQ.pop() == true);

        CHECK(tokenQ.size() == 0);
        CHECK(tokenQ.empty());
    }

    SECTION("Poping from an empty queue fails and has no effect on it") {
        REQUIRE(tokenQ.size() == 0);
        REQUIRE(tokenQ.empty());

        CHECK(tokenQ.pop() == false);

        CHECK(tokenQ.size() == 0);
        CHECK(tokenQ.empty());
    }
}
