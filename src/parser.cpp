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

std::nullptr_t ParserError(const char* const msg) {
    std::cerr << "[PARSE ERROR] " << msg << "\n";

    return nullptr;
}

std::nullptr_t ParserErrorUnexpectedToken(const char* const msg, const int actualToken) {
    std::cerr   << "[PARSE ERROR] " << msg << Lexer::TOKEN_NAMES.at(actualToken) << "(" << actualToken << ")"
                << "\n";

    return nullptr;
}

std::nullptr_t ParserErrorUnexpectedToken(const char* const msg, const Token& actualToken) {
    std::cerr << "[PARSE ERROR] " << msg << " " << actualToken << "\n";

    return nullptr;
}

std::nullptr_t ParserErrorUnexpectedToken(const char* const when, const int actualToken, const int expectedToken) {
    std::cerr   << "[PARSE ERROR] " << when
                << "but current token is " << Lexer::TOKEN_NAMES.at(actualToken) << "(" << actualToken << ")"
                << " instead of " << Lexer::TOKEN_NAMES.at(expectedToken) << "(" << expectedToken << ")"
                << "\n";

    return nullptr;
}

std::nullptr_t ParserErrorUnexpectedToken(const char* const when, const Token& actualToken, const TokenType expectedTokenType) {
    std::cerr << "[PARSE ERROR] " << when << " but current token is " << actualToken << " instead of " << expectedTokenType << "\n";

    return nullptr;
}

AbstractSyntaxTree::AbstractSyntaxTree() :
    topLevel()
{}

std::deque<ExprAST*>& AbstractSyntaxTree::getTopLevel() {
    return topLevel;
}

llvm::Value* AbstractSyntaxTree::codegen(CodeGenerator* codeGenerator) {
    if (topLevel.empty()) {
        return Error("No top level AST node to generate code from.");
    }

    if (topLevel.size() > 1) {
        return Error("Multiple top level AST node not yet supported.");
    }

    llvm::Value* codeIR = topLevel.front()->codeGen(codeGenerator);
    if (codeIR == nullptr) {
        return Error("No code generated from top level AST node.");
    }

    return codeIR;
}

std::nullptr_t AbstractSyntaxTree::Error(const std::string msg) {
    std::cerr << "[AST ERROR] " << msg << std::endl;

    return nullptr;
}

NewParser::NewParser(TokenQueue& tokenQ, AbstractSyntaxTree& astree) :
    tokens(tokenQ),
    ast(astree)
{}

unsigned int NewParser::parse() {
    unsigned int productedTopLevelNodes = 0;

    while (not tokens.empty()) {
        parseTopLevelExpr();
        ++productedTopLevelNodes;
    }

    return productedTopLevelNodes;
}

FunctionAST* NewParser::parseTopLevelExpr() {
    // For the moment any primary expression is a top level expression
    ExprAST* primExpr = parsePrimaryExpr();

    if (primExpr == nullptr) {
        return ParserError("Can't parse top level expression.");
    }

    // Make an anonymous proto.
    ProtoTypeAST* proto = new ProtoTypeAST("", std::vector<std::string>());

    // Add the anonymous function to the AST
    FunctionAST* anonymousFunc = new FunctionAST(proto, primExpr);
    ast.getTopLevel().push_back(anonymousFunc);

    // return the anonymous function
    return anonymousFunc;
}

ExprAST* NewParser::parsePrimaryExpr() {
    ExprAST* expr;

    // Just reading the first token and dispatching
    switch (tokens.front().getTokenType()) {
    case TokenType::TOK_LITTERAL_FLOAT:
        expr = parseFloatLitteralExpr();
        break;
    case TokenType::TOK_KEYWORD_LET:
        expr =  parseFloatConstantVariableDeclarationExpr();
        break;
    default:
        return ParserErrorUnexpectedToken("Can't parse primary expression, unexpected token ", tokens.front());
    }

    if (expr == nullptr) {
        return ParserError("Can't parse primary expression.");
    }

    return expr;
}

FloatLitteralExprAST* NewParser::parseFloatLitteralExpr() {
    float fVal;

    if (not tokens.front().getFloatValue(fVal)) {
        return ParserError("Can't get a float value from the lexer.");
    }

    FloatLitteralExprAST* result = new FloatLitteralExprAST(fVal);

    tokens.pop(); // eat the float
    return result;
}

FloatConstantVariableDeclarationExprAST* NewParser::parseFloatConstantVariableDeclarationExpr() {
    // Consume "let" keyword
    if (not tokens.pop(TokenType::TOK_KEYWORD_LET)) {
        return ParserErrorUnexpectedToken("Parsing float constant variable declaration", tokens.front(), TokenType::TOK_KEYWORD_LET);
    }

    // Read and consume the constant name
    std::string name; // variable to store the name to use it later during AST node construction
    if (not tokens.front().getIdentifierString(name)) {
        return ParserErrorUnexpectedToken("Parsing float constant variable declaration", tokens.front(), TokenType::TOK_IDENTIFIER);
    }

    tokens.pop(TokenType::TOK_IDENTIFIER); // consume identifier

    // Consume the affectation operator "="
    if (not tokens.pop(TokenType::TOK_OPERATOR_AFFECTATION)) {
        return ParserErrorUnexpectedToken("Parsing float constant variable declaration", tokens.front(), TokenType::TOK_OPERATOR_AFFECTATION);
    }

    // Read the right hand side of the affectation
    FloatLitteralExprAST* rhsExpr = parseFloatLitteralExpr();
    if (rhsExpr == nullptr) {
        return ParserError("Failed to parse Right Hand Side expression of the affectation.");
    }

    // everything went right... we can do AST node construction
    return new FloatConstantVariableDeclarationExprAST(name, rhsExpr);
}
