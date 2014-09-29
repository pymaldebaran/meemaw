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
#include "parser.h"

#include <iostream>

#include "ast.h"
#include "lexer.h"

std::nullptr_t Parser::ParserError(const char* const msg) {
    std::cerr << "[PARSE ERROR] " << msg << "\n";

    return nullptr;
}

std::nullptr_t Parser::ParserErrorUnexpectedToken(const char* const msg, const int actualToken) {
    std::cerr   << "[PARSE ERROR] " << msg << Lexer::TOKEN_NAMES.at(actualToken) << "(" << actualToken << ")"
                << "\n";

    return nullptr;
}

std::nullptr_t Parser::ParserErrorUnexpectedToken(const char* const when, const int actualToken, const int expectedToken) {
    std::cerr   << "[PARSE ERROR] " << when
                << "but current token is " << Lexer::TOKEN_NAMES.at(actualToken) << "(" << actualToken << ")"
                << " instead of" << Lexer::TOKEN_NAMES.at(expectedToken) << "(" << expectedToken << ")"
                << "\n";

    return nullptr;
}

FunctionAST* Parser::parseTopLevelExpr() {
    // For the moment any primary expression is a top level expression
    ExprAST * primExpr = parsePrimaryExpr();

    if (primExpr == nullptr) {
        return ParserError("Can't parse top level expression.");
    }

    // Make an anonymous proto.
    ProtoTypeAST* proto = new ProtoTypeAST("", std::vector<std::string>());

    // return the anonymous function
    return new FunctionAST(proto, primExpr);
}

ExprAST* Parser::parsePrimaryExpr() {
    ExprAST* expr;

    switch (lexer.getCurrentToken()) {
    default:
        return ParserErrorUnexpectedToken("Can't parse primary expression, unexpected token ", lexer.getCurrentToken());
    case Lexer::TOK_FLOAT:
        expr = parseFloatLitteralExpr();
        break;
    case Lexer::TOK_KEYWORD_LET:
        expr =  parseFloatConstantVariableDeclarationExpr();
        break;
    }

    if (expr == nullptr) {
        return ParserError("Can't parse primary expression.");
    }

    return expr;
}

FloatLitteralExprAST* Parser::parseFloatLitteralExpr() {
    float fVal;

    if (not lexer.getFloatValue(fVal)) {
        return ParserError("Can't get a float value from the lexer.");
    }

    FloatLitteralExprAST* result = new FloatLitteralExprAST(fVal);

    lexer.getNextToken(); // consume the float
    return result;
}

FloatConstantVariableDeclarationExprAST* Parser::parseFloatConstantVariableDeclarationExpr() {
    // let keyword
    if (lexer.getCurrentToken() != Lexer::TOK_KEYWORD_LET) {
        return ParserErrorUnexpectedToken("Parsing float constant variable declaration", lexer.getCurrentToken(), Lexer::TOK_KEYWORD_LET);
    }

    lexer.getNextToken(); // consume "let"

    // constant name
    std::string name; // variable to store the name to use it later during AST node construction
    if (not lexer.getIdentifierString(name)) {
        return ParserErrorUnexpectedToken("Parsing float constant variable declaration", lexer.getCurrentToken(), Lexer::TOK_IDENTIFIER);
    }

    lexer.getNextToken(); // consume identifier

    // affectation operator "="
    if (lexer.getCurrentToken() != Lexer::TOK_OPERATOR_AFFECTATION) {
        return ParserErrorUnexpectedToken("Parsing float constant variable declaration", lexer.getCurrentToken(), Lexer::TOK_OPERATOR_AFFECTATION);
    }

    lexer.getNextToken(); // consume affectation operator "="

    FloatLitteralExprAST* rhsExpr = parseFloatLitteralExpr();
    if (rhsExpr == nullptr) {
        return ParserError("Failed to parse Right Hand Side expression of the affectation.");
    }

    // everything went right... we can do AST node construction
    return new FloatConstantVariableDeclarationExprAST(name, rhsExpr);
}

AbstractSyntaxTree::AbstractSyntaxTree() :
    topLevel()
{}

std::deque<ExprAST*>& AbstractSyntaxTree::getTopLevel() {
    return topLevel;
}


NewParser::NewParser(TokenQueue& tokenQ, AbstractSyntaxTree& astree) :
    tokens(tokenQ),
    ast(astree)
{}

unsigned int NewParser::parse() {
    unsigned int productedTopLevelNodes = 0;

    while (parseTopLevelExpr()) {
        ++productedTopLevelNodes;
    }

    return productedTopLevelNodes;
}

bool NewParser::parseTopLevelExpr() {
    return false;
}
