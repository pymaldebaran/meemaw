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

const std::map<int, const char* const> Lexer::TOKEN_NAMES = {
    { Lexer::TOK_FLOAT,                "TOK_FLOAT"                                                  },
    { Lexer::TOK_EOF,                  "TOK_EOF"                                                    },
    { Lexer::TOK_KEYWORD_LET,          "TOK_KEYWORD_LET"                                            },
    { Lexer::TOK_IDENTIFIER,           "TOK_IDENTIFIER"                                             },
    { Lexer::TOK_OPERATOR_AFFECTATION, "TOK_OPERATOR_AFFECTATION"                                   },
    { Lexer::TOK_LEXER_ERROR,          "TOK_LEXER_ERROR"                                            },
    { Lexer::TOK_NONE,                 "TOK_NONE"                                                   },
};

int Lexer::gettok() {
    // Skip any whitespace.
    while (isspace(lastChar)) {
        lastChar = stream.get();
    }

    // identifier and keywords: [_a-zA-Z][_a-zA-Z0-9]*
    if (isalpha(lastChar) or lastChar == '_') {
        identifierString = lastChar;

        lastChar = stream.get();
        while (isalnum(lastChar) or lastChar == '_') {
            identifierString += lastChar;
            lastChar = stream.get();
        }

        if (identifierString == "let") {
            return TOK_KEYWORD_LET;
        }

        return TOK_IDENTIFIER;
    }

    // Float: [0-9.]+
    if (isdigit(lastChar) || lastChar == '.') {
        std::string floatStr;

        do {
            floatStr += lastChar;
            lastChar = stream.get();
        } while (isdigit(lastChar) || lastChar == '.');

        floatValue = strtof(floatStr.c_str(), nullptr);
        return TOK_FLOAT;
    }

    // Affectation operator: =
    if (lastChar == '=') {
        lastChar = stream.get(); // eat the affectation operator
        return TOK_OPERATOR_AFFECTATION;
    }

    // Check for end of file.  Don't eat the EOF.
    if (lastChar == EOF) {
        return TOK_EOF;
    }

    // Otherwise, just return the character as its ascii value.
    int thisChar = lastChar;    // save the char to return it later
    lastChar = stream.get();    // eat the char
    return thisChar;
}

Lexer::Lexer(std::istream& strm) :
    stream(strm),
    floatValue(0.0),
    currentToken(TOK_NONE),
    lastChar(' ')
{}

bool Lexer::getFloatValue(float& result) const {
    if (currentToken != TOK_FLOAT) {
        PrintUnexpectedTokenError("Accessing float value", currentToken, TOK_FLOAT);
        return false;
    }

    result = floatValue;
    return true;
}

bool Lexer::getIdentifierString(std::string& result) const {
    if (currentToken != TOK_IDENTIFIER) {
        PrintUnexpectedTokenError("Accessing identifier string", currentToken, TOK_IDENTIFIER);
        return false;
    }
    result = identifierString;
    return true;
}

int Lexer::getNextToken() {
    return currentToken = gettok();
}

int Lexer::getCurrentToken() {
    return currentToken;
}

void Lexer::PrintError(const char* const msg) {
    fprintf(stderr, "[LEXER ERROR] %s\n", msg);
}

void Lexer::PrintUnexpectedTokenError(const char* const when, const int actualToken, const int expectedToken) {
    const unsigned int BUF_SIZE = 255;
    char buffer[BUF_SIZE];

    snprintf(buffer, BUF_SIZE, "%s but current token is %s(%d) instead of %s(%d).",
             when,
             TOKEN_NAMES.at(actualToken),
             actualToken,
             TOKEN_NAMES.at(expectedToken),
             expectedToken);
    PrintError(buffer);
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

const TokenType Token::getTokenType() const {
    return tokenType;
}

const std::string Token::getIdentifierString() const {
    return identifierString;
}

float Token::getFloatValue() const {
    return floatValue;
}

TokenQueue::TokenQueue() :
    dummyToken(Token::CreateNone())
{}

bool TokenQueue::empty() const {
    std::cerr << "TokenQueue::empty() Not implemented.\n";

    return false;
}

unsigned int TokenQueue::size() const {
    std::cerr << "TokenQueue::size() Not implemented.\n";

    return 255;
}

const Token& TokenQueue::at(const unsigned int pos) {
    std::cerr << "TokenQueue::at() Not implemented.\n";

    return dummyToken;
}

const Token& TokenQueue::at(const unsigned int pos) const {
    std::cerr << "TokenQueue::at() const Not implemented.\n";

    return dummyToken;
}

const Token& TokenQueue::front() {
    std::cerr << "TokenQueue::front() Not implemented.\n";

    return dummyToken;
}

const Token& TokenQueue::front() const {
    std::cerr << "TokenQueue::front() const Not implemented.\n";

    return dummyToken;
}

bool TokenQueue::pop() {
    std::cerr << "TokenQueue::pop() Not implemented.\n";

    return false;
}

bool TokenQueue::pop(const TokenType typ) {
    std::cerr << "TokenQueue::pop() const Not implemented.\n";

    return false;
}

void TokenQueue::push(const Token tok) {
    std::cerr << "TokenQueue::push() Not implemented.\n";
}

NewLexer::NewLexer(std::istream& stream, TokenQueue& tokenQ) :
    input(stream)
{}

std::deque<Token>& NewLexer::getTokens() {
    return tokens;
}

unsigned int NewLexer::tokenize() {
    unsigned int productedTokens = 0;

    while (tokenizeOne()) {
        ++productedTokens;
    }

    return productedTokens;
}

bool NewLexer::eatToken() {
    if (tokens.empty()) {
        return false;
    }

    tokens.pop_front();
    return true;
}

bool NewLexer::eatToken(TokenType typ) {
    if (tokens.empty()) {
        return false;
    }

    if (tokens.front().getTokenType() != typ) {
        return false;
    }

    tokens.pop_front();
    return true;
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
            pushToken(Token::CreateKeywordLet());
            return true;
        }

        pushToken(Token::CreateIdentifier(identifierBuffer));
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

        pushToken(Token::CreateLitteralFloat(floatValue));
        return true;
    }

    // Affectation operator: =
    if (lastChar == '=') {
        lastChar = input.get(); // eat the affectation operator

        pushToken(Token::CreateOperatorAffectation());
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
    pushToken(Token::CreateLexerError(thisChar));
    return true;
}

void NewLexer::pushToken(Token tok) {
    tokens.push_back(tok);
}