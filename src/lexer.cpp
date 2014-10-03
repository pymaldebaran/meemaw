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
#include "lexer.h"

#include <cstdlib>
#include <stdexcept>
#include <map>
#include <iostream>

std::ostream & operator<<(std::ostream& os, const TokenType tok) {
    std::string str = "";

    switch (tok) {
    case TokenType::TOK_LITTERAL_FLOAT:
        str = "TOK_LITTERAL_FLOAT";
        break;
    case TokenType::TOK_KEYWORD_LET:
        str = "TOK_KEYWORD_LET";
        break;
    case TokenType::TOK_IDENTIFIER:
        str = "TOK_IDENTIFIER";
        break;
    case TokenType::TOK_OPERATOR_AFFECTATION:
        str = "TOK_OPERATOR_AFFECTATION";
        break;
    case TokenType::TOK_LEXER_ERROR:
        str = "TOK_LEXER_ERROR";
        break;
    case TokenType::TOK_NONE:
        str = "TOK_NONE";
        break;
    default:
        str = "!!!UNREFERENCED_TOKEN!!!";
        break;
    }
    return os << str;
}

Token Token::CreateLitteralFloat(const float f) {
    return Token(TokenType::TOK_LITTERAL_FLOAT, "", f);
}

Token Token::CreateKeywordLet() {
    return Token(TokenType::TOK_KEYWORD_LET);
}

Token Token::CreateIdentifier(const std::string& idStr) {
    return Token(TokenType::TOK_IDENTIFIER, idStr);
}

Token Token::CreateOperatorAffectation() {
    return Token(TokenType::TOK_OPERATOR_AFFECTATION);
}

Token Token::CreateLexerError(const int ch) {
    return Token(TokenType::TOK_LEXER_ERROR, std::string(ch, 1));
}
Token Token::CreateNone() {
    return Token(TokenType::TOK_NONE);
}

Token::Token(const TokenType typ, const std::string identifierstr, const float floatVal) :
    tokenType(typ),
    identifierString(identifierstr),
    floatValue(floatVal)
{}

bool Token::UnexpectedTokenError(const char* const when, const Token& actualToken, const TokenType expectedTokenType) {
    std::cerr   << "[TOKEN ERROR] " << when
                << " but current token is " << actualToken
                << "while expecting " << expectedTokenType
                << std::endl;

    return false;
}

const TokenType Token::getTokenType() const {
    return tokenType;
}

bool Token::getIdentifierString(std::string& idStr) const {
    if (getTokenType() != TokenType::TOK_IDENTIFIER) {
        return UnexpectedTokenError("Accessing identifier string", *this, TokenType::TOK_IDENTIFIER);
    }

    idStr = identifierString;
    return true;
}

bool Token::getFloatValue(float& fVal) const {
    if (getTokenType() != TokenType::TOK_LITTERAL_FLOAT) {
        return UnexpectedTokenError("Accessing float value", *this, TokenType::TOK_LITTERAL_FLOAT);
    }

    fVal = floatValue;
    return true;
}

std::ostream& operator<<(std::ostream& os, const Token& tok) {
    std::string desc = "";

    switch (tok.tokenType) {
    case TokenType::TOK_LITTERAL_FLOAT:
        desc = "floatValue=" + std::to_string(tok.floatValue);
        break;
    case TokenType::TOK_IDENTIFIER:
        desc = "identifierString=" + tok.identifierString;
        break;
    case TokenType::TOK_KEYWORD_LET:
    case TokenType::TOK_OPERATOR_AFFECTATION:
    case TokenType::TOK_LEXER_ERROR:
    case TokenType::TOK_NONE:
        desc = "";
        break;
    default:  // unknown token
        desc = "tokenType=" + std::to_string(static_cast<unsigned int>(tok.tokenType));
        break;
    }

    return os << tok.tokenType << "(" << desc << ")";
}

TokenQueue::TokenQueue() :
    tokens(std::deque<Token>())
{}

bool TokenQueue::empty() const {
    return tokens.empty();
}

unsigned int TokenQueue::size() const {
    return tokens.size();
}

const Token& TokenQueue::at(const unsigned int pos) const {
    return tokens.at(pos);
}

const Token& TokenQueue::front() const {
    return tokens.front();
}

const Token& TokenQueue::back() const {
    return tokens.back();
}

bool TokenQueue::pop() {
    if (tokens.empty()) {
        return false;
    }

    tokens.pop_front();
    return true;
}

bool TokenQueue::pop(const TokenType typ) {
    if (tokens.empty()) {
        return false;
    }

    if (tokens.front().getTokenType() != typ) {
        return false;
    }

    tokens.pop_front();
    return true;
}

void TokenQueue::push(const Token tok) {
    tokens.push_back(tok);
}

std::ostream& operator<<(std::ostream& os, const TokenQueue& tokQ) {
    os << "TokenQueue[";

    for (auto tok : tokQ.tokens) {
        os << tok  << " ";
    }

    return os << "]";
}

NewLexer::NewLexer(std::istream& stream, TokenQueue& tokenQ) :
    input(stream),
    tokens(tokenQ)
{}

unsigned int NewLexer::tokenize() {
    unsigned int productedTokens = 0;

    while (tokenizeOne()) {
        ++productedTokens;
    }

    return productedTokens;
}

bool NewLexer::tokenizeOne() {
    int lastChar = input.get();

    // Skip any whitespace.
    while (isspace(lastChar)) {
        lastChar = input.get();
    }

    // identifier and keywords: [_a-zA-Z][_a-zA-Z0-9]*
    if (isalpha(lastChar) or lastChar == '_') {
        std::string identifierBuffer;
        identifierBuffer = lastChar;

        lastChar = input.get();
        while (isalnum(lastChar) or lastChar == '_') {
            identifierBuffer += lastChar;
            lastChar = input.get();
        }

        if (identifierBuffer == "let") {
            tokens.push(Token::CreateKeywordLet());
            return true;
        }

        tokens.push(Token::CreateIdentifier(identifierBuffer));
        return true;
    }

    // Float: [0-9.]+
    if (isdigit(lastChar) || lastChar == '.') {
        std::string floatStr;

        do {
            floatStr += lastChar;
            lastChar = input.get();
        } while (isdigit(lastChar) || lastChar == '.');

        float floatValue = strtof(floatStr.c_str(), nullptr);

        tokens.push(Token::CreateLitteralFloat(floatValue));
        return true;
    }

    // Affectation operator: =
    if (lastChar == '=') {
        lastChar = input.get(); // eat the affectation operator

        tokens.push(Token::CreateOperatorAffectation());
        return true;
    }

    // Check for end of file.  Don't eat the EOF.
    if (lastChar == EOF) {
        // we add no token
        return false;
    }

    // Otherwise it's an unrecognized sequence.
    int thisChar = lastChar;    // save the char to return it later
    lastChar = input.get();     // eat the char
    // TODO replace this with a proper error function
    std::cerr << "[NEW LEXER ERROR] Unrecognized character.\n";
    tokens.push(Token::CreateLexerError(thisChar));
    return true;
}
