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

const std::map<int, const char* const> Lexer::TOKEN_NAMES = {
    { Lexer::TOK_FLOAT,                "TOK_FLOAT"                                                                                                                                                                                                                                                              },
    { Lexer::TOK_EOF,                  "TOK_EOF"                                                                                                                                                                                                                                                                },
    { Lexer::TOK_KEYWORD_LET,          "TOK_KEYWORD_LET"                                                                                                                                                                                                                                                        },
    { Lexer::TOK_IDENTIFIER,           "TOK_IDENTIFIER"                                                                                                                                                                                                                                                         },
    { Lexer::TOK_OPERATOR_AFFECTATION, "TOK_OPERATOR_AFFECTATION"                                                                                                                                                                                                                                               },
    { Lexer::TOK_LEXER_ERROR,          "TOK_LEXER_ERROR"                                                                                                                                                                                                                                                        },
    { Lexer::TOK_NONE,                 "TOK_NONE"                                                                                                                                                                                                                                                               },
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

Token::Token() :
    tokenType(TokenType::TOK_NONE),
    identifierString(""),
    floatValue(0.0)
{}

TokenType Token::getTokenType() {
    return tokenType;
}

std::string Token::getIdentifierString() {
    return identifierString;
}

float Token::getFloatValue() {
    return floatValue;
}

NewLexer::NewLexer(std::istream& stream) :
    input(stream)
{}

std::deque<Token>& NewLexer::getTokens() {
    return tokens;
}

unsigned int NewLexer::tokenize() {
    return 0;
}

bool NewLexer::tokenizeOne() {
    return false;
}