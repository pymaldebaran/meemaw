#ifndef PARSER_H
#define PARSER_H

#include <string>

class Lexer;

// Specify ExprAST type without using RTTI.
// This is only usefull for tesing purpose and should not be used in actual 
// code.
enum class AstType {
    NONE = 0, //should never happen, only the base class use it
    FLOAT_LITTERAL = 1,
};

// Base class for all expression nodes.
class ExprAST {
private:
    AstType astType; // used to determine type at runtime (tests only)
protected:
    // Constructor for member initialisation in derived classes
    explicit ExprAST(AstType ast);
public:
    // Constructor
    explicit ExprAST();

    // astType getter
    AstType getAstType();
};

class FloatExpAST : public ExprAST {
private:
    float value; // value of the float litteral
public:
    // Constructor
    explicit FloatExpAST();

    // value getter
    float getValue();
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