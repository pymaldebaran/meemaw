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

int Lexer::gettok() {
    lastChar = stream.get();

    // identifier and keywords: [a-zA-Z][a-zA-Z0-9]*
    if (isalpha(lastChar)) {
        identifierString = lastChar;
        while (isalnum((lastChar = stream.get())))
            identifierString += lastChar;

        if (identifierString == "let")
            return TOK_KEYWORD_LET;

        fprintf(stderr, "Lexer error : identifier not yet supported\n");
        return TOK_LEXER_ERROR;
        //return TOK_IDENTIFIER;
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

    // Check for end of file.  Don't eat the EOF.
    if (lastChar == EOF)
        return TOK_EOF;

    // Otherwise, just return the character as its ascii value.
    int thisChar = lastChar;
    lastChar = stream.get();
    return thisChar;
}

Lexer::Lexer(std::istream& strm) :
    stream(strm),
    floatValue(0.0),
    currentToken(TOK_NONE),
    lastChar(' ')
{}

float Lexer::getFloatValue() const {
    if (currentToken != TOK_FLOAT) {
        char buffer[100];
        //TODO write a helper function to represent token const
        snprintf(buffer, 100, "Lexer error: accessing float value but current token is %d not TOK_FLOAT(%d).\n", currentToken, TOK_FLOAT);
        throw std::logic_error(buffer);
    }
    return floatValue;
}

int Lexer::getNextToken() {
    return currentToken = gettok();
}
