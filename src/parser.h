#ifndef PARSER_H
#define PARSER_H

#include <string>

class Lexer;

// Base class for all expression nodes.
class ExprAST {
public:
    // Gets a textual representation of the node
    virtual std::string dump() = 0;
};


// Parser for the MeeMaw language.
// Generate the Abstract Syntax Tree for each token parsed.
class Parser {
private:
    Lexer& lexer;
public:
    explicit Parser(Lexer& lexer);

    // Parse top level expression
    // top ::= expression
    ExprAST* parseTopLevelExpr();
};

#endif // PARSER_H