#include "lexer.h"

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
    currentToken(0),
    lastChar(' ')
{
}

float Lexer::getFloatValue()
{
    //TODO check if currentToken == TOK_FLOAT
    return floatValue;
}

int Lexer::getNextToken()
{
    return currentToken = gettok();
}
