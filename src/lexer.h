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
    // The lexer returns tokens [0-255] if it is an unknown character, otherwise
    // one of these for known things.
    enum {
        TOK_FLOAT = -1,
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
enum class TokenType : unsigned int {
    TOK_LITTERAL_FLOAT = 1,
    TOK_KEYWORD_LET = 2,
    TOK_IDENTIFIER = 3,
    TOK_OPERATOR_AFFECTATION = 4,
    TOK_LEXER_ERROR = 255,        // returned by lexer in case of error
    TOK_NONE = 256,               // initial value of the currentToken attribute
};

// TODO make this an enum class and remove all the int<->enum crap from the
//      lexer mecanic

std::ostream& operator<<(std::ostream& os, const TokenType tok);

class Token {
public:
    // A bunch of helpers to create valid tokens
    static Token CreateLitteralFloat(const float f);
    static Token CreateKeywordLet();
    static Token CreateIdentifier(const std::string& idStr);
    static Token CreateOperatorAffectation();
    static Token CreateLexerError(const int ch);
    static Token CreateNone();

    // Get the type of token
    const TokenType getTokenType() const;

    // Get the identifier string and put it in the idStr reference.
    //
    // Returns true and set the idStr reference if getTokenType() is
    // TOK_IDENTIFIER.
    // Returns false and do not set the fVal reference if getTokenType() is
    // anything but TOK_IDENTIFIER.
    bool getIdentifierString(std::string& idStr) const;

    // Get the value of a float litteral token and put it in the fVal reference.
    //
    // Returns true and set the fVal reference if getTokenType() is
    // TOK_LITTERAL_FLOAT.
    // Returns false and do not set the fVal reference if getTokenType() is
    // anything but TOK_LITTERAL_FLOAT.
    bool getFloatValue(float& fVal) const;

    // Stream operator for easy printing and debugging
    friend std::ostream& operator<<(std::ostream& os, const Token& tok);

private:
    const TokenType tokenType;              // Type of the token
    const std::string identifierString;     // Filled in if currentToken == TOK_IDENTIFIER
    const float floatValue;                 // Filled in if currentToken == TOK_LITTERAL_FLOAT

    // Private Constructor
    // You should use public static function to create valid tokens
    explicit Token(const TokenType typ, const std::string identifierstr = "", const float floatVal = 0.0);

    static bool UnexpectedTokenError(const char* const when, const Token& actualToken, const TokenType expectedTokenType);
};

class TokenQueue {
public:
    // Constructor
    TokenQueue();

    // Returns whether the queue is empty (i.e. whether its size is 0).
    bool empty() const;

    // Returns the number of elements in the queue
    unsigned int size() const;

    // Returns a reference to the element at position pos in the queue.
    const Token& at(const unsigned int pos) const;

    // Returns a reference to the first element in the queue.
    const Token& front() const;

    // Returns a reference to the last element in the queue.
    const Token& back() const;

    // Remove the older token from the token container.
    //
    // Returns true only if a token has been effectively removed
    //         false otherwise (especialy if the token container was empty)
    //
    // The token is always removed from the front position in order to ensure
    // FIFO comportement.
    bool pop();

    // Remove the older token from the token container only if it is from the
    // specified type.
    //
    // Returns true only if a token has been effectively removed
    //         false otherwise (especialy if the token container was empty)
    //
    // The token is always removed from the front position in order to ensure
    // FIFO comportement.
    bool pop(const TokenType typ);

    // Add a new token to the token container.
    //
    // The token is always added in back position in order to ensure FIFO
    // comportement.
    //
    // This should be used instead of direct access to the tokens attribute.
    void push(const Token tok);

    // Stream operator for easy printing and debugging
    friend std::ostream& operator<<(std::ostream& os, const TokenQueue& tokQ);

private:
    std::deque<Token> tokens;   // where to put all the tokens generated when colling nextToken()
};

class NewLexer {
public:
    // Constructor
    explicit NewLexer(std::istream& stream, TokenQueue& tokenQ);

    // Tokenize all the stream and put all the produced tokens in the token
    // container.
    //
    // Returns the number of produced tokens.
    //
    // The input stream is consumed till the end.
    //
    // The token container is filled FIFO :
    // - The last processed token will always be in back position
    // - The first processed token will always be in front position
    unsigned int tokenize();

private:
    std::istream& input;    // input stream to parse
    TokenQueue& tokens;     // where to put all the tokens generated when colling nextToken()

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