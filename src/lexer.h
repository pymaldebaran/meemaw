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

// The lexer returns tokens [0-255] if it is an unknown character, otherwise
// one of these for known things.
static const int TOK_FLOAT = -1;
static const int TOK_EOF   = -2;
static const int TOK_LET   = -3;
static const int TOK_NONE  = -255;

// Lexer for the MeeMaw language
class Lexer {
private:
    std::istream& stream;   // input stream to parse
    float floatValue;       // Filled in if curTok == TOK_FLOAT
    int currentToken;       // Current token i.e. the last returned by getNextToken()
    int lastChar;           // Store the last char read by gettok()

    // gettok - Return the next token from standard input.
    int gettok();

public:
    // Constructor
    explicit Lexer(std::istream& strm);

    // floatValue getter
    float getFloatValue() const;

    // Reads another token from the lexer and updates CurTok with its results
    int getNextToken();
};

#endif // LEXER_H