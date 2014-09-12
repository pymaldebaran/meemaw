#include "lexer.h"

#include <cstdlib>
#include <stdexcept>

int Lexer::gettok()
{
    lastChar = stream.get();

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
{
}

float Lexer::getFloatValue()
{
    if (currentToken != TOK_FLOAT) {
        char buffer[100];
        //TODO write a helper function to represent token const
        snprintf(buffer, 100, "Lexer error: accessing float value but current token is %d not TOK_FLOAT(%d).\n", currentToken, TOK_FLOAT);
        throw std::logic_error(buffer);
    }
    return floatValue;
}

int Lexer::getNextToken()
{
    return currentToken = gettok();
}
