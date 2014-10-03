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

#include "catch.hpp"

#include "../src/parser.h"

#include "../src/lexer.h"
#include "../src/ast.h"

TEST_CASE("Parser generate AST for float litteral") {
    SECTION("Parsing a float litteral expression generate a float litteral expression AST node") {
        std::stringstream in;                   // stream to parse by lexer
        TokenQueue tokens;                      // token container
        NewLexer lex = NewLexer(in, tokens);    // the lexer
        AbstractSyntaxTree ast;                 // the Abstract Syntax Tree
        Parser parser = Parser(tokens, ast);    // the parser

        in << "1.0";

        lex.tokenize();

        FloatLitteralExprAST* floatLittAst = parser.parseFloatLitteralExpr();

        // TODO add test for the presence in the AST container object
        REQUIRE(floatLittAst != nullptr);
        REQUIRE(floatLittAst->getAstType() == AstType::FLOAT_LITTERAL);
        REQUIRE(floatLittAst->getValue() == 1.0);
    }

    SECTION("Parser recognise float litteral expression as a top level expression") {
        std::stringstream in;                   // stream to parse by lexer
        TokenQueue tokens;                      // token container
        NewLexer lex = NewLexer(in, tokens);    // the lexer
        AbstractSyntaxTree ast;                 // the Abstract Syntax Tree
        Parser parser = Parser(tokens, ast);    // the parser

        in << "1.0";

        lex.tokenize();

        FunctionAST* anonFuncAst = parser.parseTopLevelExpr();

        // Do we have an float expression ?
        // i.e. an anonymous function returning a float
        REQUIRE(anonFuncAst != nullptr);

        REQUIRE(anonFuncAst->getPrototype() != nullptr);
        REQUIRE(anonFuncAst->getPrototype()->getAstType() == AstType::PROTOTYPE);
        ProtoTypeAST* protoAst = static_cast<ProtoTypeAST*>(anonFuncAst->getPrototype());
        REQUIRE(protoAst->getName() == "");
        REQUIRE(protoAst->getArgs().empty());

        REQUIRE(anonFuncAst->getBody() != nullptr);
        REQUIRE(anonFuncAst->getBody()->getAstType() == AstType::FLOAT_LITTERAL);
        FloatLitteralExprAST* bodyAst = static_cast<FloatLitteralExprAST*>(anonFuncAst->getBody());
        REQUIRE(bodyAst->getValue() == 1.0);
    }
}

TEST_CASE("Parser generate AST for litteral constant declaration") {
    std::stringstream in;                   // stream to parse by lexer
    TokenQueue tokens;                      // token container
    NewLexer lex = NewLexer(in, tokens);    // the lexer
    AbstractSyntaxTree ast;                 // the Abstract Syntax Tree
    Parser parser = Parser(tokens, ast);    // the parser

    in << "let aaa = 1.0";

    lex.tokenize();

    SECTION("Parsing a litteral constant declaration generate a float expression AST node") {
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
        ExprAST* topLevelAst = parser.parseTopLevelExpr();

        // Do we have an float expression ?
        // i.e. an anonymous function returning a float
        REQUIRE(topLevelAst != nullptr);
        REQUIRE(topLevelAst->getAstType() == AstType::FUNCTION);

        FunctionAST* funcAst = static_cast<FunctionAST*>(topLevelAst);

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

TEST_CASE("New parser consume tokens and produce abstract syntax tree depending on the input") {
    std::stringstream in;                   // stream to parse by lexer
    TokenQueue tokens;                      // token container
    NewLexer lex = NewLexer(in, tokens);    // the lexer
    AbstractSyntaxTree ast;                 // the Abstract Syntax Tree
    Parser parser = Parser(tokens, ast);    // the parser

    CHECK(ast.getTopLevel().empty());

    SECTION("Parsing top level expression consume the tokens of that primary expression and produce abstract syntax tree for it") {
        in << "let aaa = 1.0";

        lex.tokenize();

        REQUIRE(not tokens.empty());
        REQUIRE(tokens.size() == 4);

        // let's do the actual parsing
        ExprAST* astTopLevel = parser.parseTopLevelExpr();

        REQUIRE(astTopLevel != nullptr);

        // all tokens should have been consumed
        CHECK(tokens.empty());

        // and one expression should have been produced
        CHECK(not ast.getTopLevel().empty());
        CHECK(ast.getTopLevel().size() == 1);
    }

    SECTION("Parsing an empty input do not produce abstract Syntax Tree") {
        lex.tokenize();

        REQUIRE(tokens.empty());

        CHECK(parser.parse() == 0);

        CHECK(ast.getTopLevel().empty());
        CHECK(tokens.empty());
    }

    SECTION("Parsing consume all tokens and produce abstract syntax tree") {
        in << "let aaa = 1.0";

        lex.tokenize();

        REQUIRE(not tokens.empty());

        // let's do the actual parsing
        CHECK(parser.parse() == 1);

        // all tokens should have been consumed
        CHECK(tokens.empty());

        // and one expression should have been produced
        CHECK(not ast.getTopLevel().empty());
        CHECK(ast.getTopLevel().size() == 1);
    }

    // TODO test with multiple expressions (handling \n)
    // TODO test parseTopLevelExpr
}
