#ifndef LEXER_H
#define LEXER_H

#include <istream>

// The lexer returns tokens [0-255] if it is an unknown character, otherwise
// one of these for known things.
const int TOK_FLOAT = -1;
const int TOK_EOF   = -2;

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
    float getFloatValue();

    // Reads another token from the lexer and updates CurTok with its results
    int getNextToken();
};

#endif // LEXER_H