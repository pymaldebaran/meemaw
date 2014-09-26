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
#include <map>
#include <deque>

// Lexer for the MeeMaw language
class Lexer {
public:
    //TODO test the usage of TOK_LEXER_ERROR and TOK_NONE
    // The lexer returns tokens [0-255] if it is an unknown character, otherwise
    // one of these for known things.
    enum {
        TOK_FLOAT = -1, // TODO change nae to TOK_LITTERAL_FLOAT
        TOK_EOF   = -2,
        TOK_KEYWORD_LET = -3,
        TOK_IDENTIFIER = -4,
        TOK_OPERATOR_AFFECTATION = -5,
        TOK_LEXER_ERROR = -254,     // returned by lexer in case of error
        TOK_NONE  = -255            // initial value of the currentToken attribute
    };

    // Store the names of the enums for easy printing
    static const std::map<int, const char* const> TOKEN_NAMES;

    // Constructor
    explicit Lexer(std::istream& strm);

    // floatValue getter
    // Returns true if the current token is a float litteral and put its value
    // in the result parameter.
    // Returns false otherwise and the result parameter stay unmodified.
    bool getFloatValue(float& result) const;

    // identifierString getter
    // Returns true if the current token is an identifier and put the identifier
    // string in the result parameter.
    // Returns false otherwise and the result parameter stay unmodified.
    bool getIdentifierString(std::string& result) const;

    // TODO replace this method with nextToken() because it looks like a getter
    // TODO add a "token safe" method that takes a token parameter and "eat" it
    //      only if the next token is of the correct type
    // TODO add a eat() method just to be more clear when using it ;)
    // Reads another token from the lexer and updates CurTok with its results
    int getNextToken();

    // currentToken getter
    int getCurrentToken();

private:
    std::istream& stream;           // input stream to parse
    std::string identifierString;   // Filled in if currentToken == TOK_IDENTIFIER or TOK_KEYWORD_*
    float floatValue;               // Filled in if currentToken == TOK_FLOAT
    int currentToken;               // Current token i.e. the last returned by getNextToken()
    int lastChar;                   // Store the last char read by gettok()

    // Return the next token from standard input.
    int gettok();

    // Simple error display helper
    static void PrintError(const char* const msg);

    // Error display helper when handling with unexpected token
    static void PrintUnexpectedTokenError(const char* const when, const int actualToken, const int expectedToken);
};

//TODO test the usage of TOK_LEXER_ERROR and TOK_NONE
// The lexer returns tokens [0-255] if it is an unknown character, otherwise
// one of these for known things.
enum class TokenType {
    TOK_LITTERAL_FLOAT = -1,
    TOK_EOF   = -2,
    TOK_KEYWORD_LET = -3,
    TOK_IDENTIFIER = -4,
    TOK_OPERATOR_AFFECTATION = -5,
    TOK_LEXER_ERROR = -254,     // returned by lexer in case of error
    TOK_NONE  = -255            // initial value of the currentToken attribute
};

class Token {
public:
    // Constructor
    explicit Token();

    // Get the type of token
    TokenType getTokenType();

    // Get the identifier string (available only if getTokenType() is
    // TOK_IDENTIFIER)
    std::string getIdentifierString();

    // Get the value of a float litteral token (available only if getTokenType()
    // is TOK_LITTERAL_FLOAT
    float getFloatValue();

private:
    TokenType tokenType;            // Type of the token
    std::string identifierString;   // Filled in if currentToken == TOK_IDENTIFIER
    float floatValue;               // Filled in if currentToken == TOK_LITTERAL_FLOAT
};

class NewLexer {
public:
    // Constructor
    explicit NewLexer(std::istream& stream);

    // Accessor to the token container.
    // The container is updated everytime a nextToken() call is made.
    std::deque<Token>& getTokens();

    // Tokenize all the stream and put all the produced tokens in the token
    // container.
    //
    // Returns the number of produced tokens.
    //
    // The input stream is consumed till the end.
    //
    // The token container is filled FILO :
    // - The last processed token will always be in back position
    // - The first processed token will always be in front position
    unsigned int tokenize();

private:
    std::istream& input;        // input stream to parse
    std::deque<Token> tokens;   // where to put all the tokens generated when colling nextToken()

    // Tokenize just one token.
    //
    // Returns true if one token has been produced
    //         false if no token has been produced
    //
    // The input stream is consumed according to the legth of the token.
    //
    // The produced token (if any) is appended in front position.
    bool tokenizeOne();
};

#endif // LEXER_H