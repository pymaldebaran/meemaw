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
#ifndef LEXER_H
#define LEXER_H

#include <istream>

// Lexer for the MeeMaw language
class Lexer {
public:
    //TODO test the usage of TOK_LEXER_ERROR and TOK_NONE
    // The lexer returns tokens [0-255] if it is an unknown character, otherwise
    // one of these for known things.
    enum {
        TOK_FLOAT = -1,
        TOK_EOF   = -2,
        TOK_KEYWORD_LET = -3,
        TOK_IDENTIFIER = -4,
        TOK_LEXER_ERROR = -254,     // returned by lexer in case of error
        TOK_NONE  = -255            // initial value of the currentToken attribute
    };


    // Constructor
    explicit Lexer(std::istream& strm);

    // floatValue getter
    float getFloatValue() const;

    // identifierString getter
    std::string getIdentifierString() const;

    // Reads another token from the lexer and updates CurTok with its results
    int getNextToken();

private:
    std::istream& stream;           // input stream to parse
    std::string identifierString;   // Filled in if currentToken == TOK_IDENTIFIER or TOK_KEYWORD_*
    float floatValue;               // Filled in if currentToken == TOK_FLOAT
    int currentToken;               // Current token i.e. the last returned by getNextToken()
    int lastChar;                   // Store the last char read by gettok()

    // gettok - Return the next token from standard input.
    int gettok();
};

#endif // LEXER_H