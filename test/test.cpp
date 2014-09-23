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

// TESTS /////////////////////////////////////
//maximum difference between 2 floats to be considered equal
const float F_EPSILON = std::numeric_limits<float>::min() * 10.0;

TEST_CASE("Environment works") {
    INFO( "This means we have a working developping environment." );
}

TEST_CASE("Lexer categorise float") {
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

    SECTION("1.0 is categorized as float") {
        test << "1.0";

        int tokId = lex.getNextToken();

        REQUIRE(tokId == Lexer::TOK_FLOAT);
        REQUIRE(lex.getFloatValue() == 1.0);
    }

    SECTION("Any float between 0.0 and max float is categorized as float") {
        float f = randFloatGenerator();

        test << std::to_string(f);

        int tokId = lex.getNextToken();

        REQUIRE(abs(lex.getFloatValue() - f) < F_EPSILON);
    }
}

TEST_CASE("Parser generate AST for float") {
    SECTION("Parsing a float expression generate a float expression AST node") {
        std::stringstream test;         // stream to parse by lexer
        Lexer lex = Lexer(test);        // the lexer
        Parser parser = Parser(lex);    // the parser

        test << "1.0";

        lex.getNextToken();
        FloatExpAST* ast = parser.parseFloatLitteralExpr();

        REQUIRE(ast != nullptr);
        // TODO replace REQUIRE by CHECK here for easy diagnostic
        REQUIRE(ast->getAstType() == AstType::FLOAT_LITTERAL);
        REQUIRE(ast->getValue() == 1.0);
    }

    SECTION("Parser recognise float expression as a top level expression") {
        std::stringstream test;         // stream to parse by lexer
        Lexer lex = Lexer(test);        // the lexer
        Parser parser = Parser(lex);    // the parser

        test << "1.0";

        lex.getNextToken();
        ExprAST* ast = parser.parseTopLevelExpr();

        REQUIRE(ast != nullptr);
        REQUIRE(ast->getAstType() == AstType::FUNCTION);

        FunctionAST* funcAst = static_cast<FunctionAST*>(ast);

        REQUIRE(funcAst->getPrototype() != nullptr);
        REQUIRE(funcAst->getPrototype()->getAstType() == AstType::PROTOTYPE);
        ProtoTypeAST* protoAst = static_cast<ProtoTypeAST*>(funcAst->getPrototype());
        REQUIRE(protoAst->getName() == "");
        REQUIRE(protoAst->getArgs().empty());

        REQUIRE(funcAst->getBody() != nullptr);
        REQUIRE(funcAst->getBody()->getAstType() == AstType::FLOAT_LITTERAL);
        FloatExpAST* bodyAst = static_cast<FloatExpAST*>(funcAst->getBody());
        REQUIRE(bodyAst->getValue() == 1.0);
    }
}

TEST_CASE("Code generated for float expression is correct", "[problem]") {
    // LLVM stuff
    // TODO: put all this in a class... but witch one
    llvm::InitializeNativeTarget();
    llvm::LLVMContext& context = llvm::getGlobalContext();
    llvm::Module* module = new llvm::Module("Test module", context); // Make the module, which holds all the code.

    if (module == nullptr)
        fprintf(stderr, "no module\n");

    // non LLVM stuff
    std::stringstream test;                     // stream to parse by lexer
    Lexer lex = Lexer(test);                    // the lexer
    Parser parser = Parser(lex);                // the parser
    CodeGenerator gen = CodeGenerator(module);  // the code generator

    // create a JIT Engine (This takes ownership of the module).
    std::string errStr;
    llvm::ExecutionEngine* execEngine = llvm::EngineBuilder(module).setErrorStr(&errStr).setEngineKind(llvm::EngineKind::JIT).create();
    if (execEngine == nullptr)
        fprintf(stderr, "no exec engine: %s\n", errStr.c_str());

    test << "1.0";

    lex.getNextToken();
    ExprAST* ast = parser.parseTopLevelExpr();
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
        CHECK(lex.getIdentifierString() == identifier);
    }

    SECTION("Lexer categorise an uppercase identifier") {
        std::string identifier = "ABC";

        test << identifier;

        int tokId = lex.getNextToken();

        CHECK(tokId == Lexer::TOK_IDENTIFIER);
        CHECK(lex.getIdentifierString() == identifier);
    }

    SECTION("Lexer categorise identifier with leading underscore") {
        std::string identifier = "_ab";

        test << identifier;

        int tokId = lex.getNextToken();

        CHECK(tokId == Lexer::TOK_IDENTIFIER);
        CHECK(lex.getIdentifierString() == identifier);
    }

    SECTION("Lexer categorise identifier with underscore") {
        std::string identifier = "a_b";

        test << identifier;

        int tokId = lex.getNextToken();

        CHECK(tokId == Lexer::TOK_IDENTIFIER);
        CHECK(lex.getIdentifierString() == identifier);
    }

    SECTION("Lexer categorise identifier with numbers") {
        std::string identifier = "a2b";

        test << identifier;

        int tokId = lex.getNextToken();

        CHECK(tokId == Lexer::TOK_IDENTIFIER);
        CHECK(lex.getIdentifierString() == identifier);
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

TEST_CASE("Lexer does not skip EOF after a token") {
    // stream to parse by lexer
    std::stringstream test;

    // the lexer
    Lexer lex = Lexer(test);

    test << "1.0";

    int tokId = lex.getNextToken();

    CHECK(tokId == Lexer::TOK_FLOAT);
    CHECK(lex.getFloatValue() == 1.0);

    tokId = lex.getNextToken();

    CHECK(tokId == Lexer::TOK_EOF);
}

TEST_CASE("Lexer only returns EOF once") {
    // stream to parse by lexer
    std::stringstream test;

    // the lexer
    Lexer lex = Lexer(test);

    test << "1.0";

    int tokId = lex.getNextToken();

    CHECK(tokId == Lexer::TOK_FLOAT);
    CHECK(lex.getFloatValue() == 1.0);

    tokId = lex.getNextToken();

    CHECK(tokId == Lexer::TOK_EOF);

    tokId = lex.getNextToken();

    CHECK(tokId == Lexer::TOK_LEXER_ERROR);
}

TEST_CASE("Lexer skip one whitespace between tokens") {
    // stream to parse by lexer
    std::stringstream test;

    // the lexer
    Lexer lex = Lexer(test);

    test << "1.0 2.0";

    int tokId = lex.getNextToken();

    CHECK(tokId == Lexer::TOK_FLOAT);
    CHECK(lex.getFloatValue() == 1.0);

    tokId = lex.getNextToken();

    CHECK(tokId == Lexer::TOK_FLOAT);
    CHECK(lex.getFloatValue() == 2.0);

    tokId = lex.getNextToken();

    CHECK(tokId == Lexer::TOK_EOF);
}

TEST_CASE("Parser generate AST for litteral constant declaration") {
    SECTION("Parsing a litteral constant declaration generate a float expression AST node") {
        std::stringstream test;         // stream to parse by lexer
        Lexer lex = Lexer(test);        // the lexer
        Parser parser = Parser(lex);    // the parser

        test << "let aaa = 1.0";

        lex.getNextToken();
        FloatConstantVariableDeclarationExprAST* ast = parser.parseFloatConstantVariableDeclarationExpr();

        REQUIRE(ast != nullptr);
        CHECK(ast->getAstType() == AstType::FLOAT_CONSTANT_VARIABLE_DECLARATION);
        CHECK(ast->getName() == "aaa");
        CHECK(ast->getValue() == 1.0);
    }

    //TODO test constant declaration with an expr as value
}