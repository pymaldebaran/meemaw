#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

class Lexer;

// Specify ExprAST type without using RTTI.
// This is only usefull for tesing purpose and should not be used in actual
// code.
enum class AstType {
    NONE = 0, //should never happen, only the base class use it
    FLOAT_LITTERAL = 1,
    PROTOTYPE = 2,
    FUNCTION = 3,
};

// Base class for all expression nodes.
class ExprAST {
private:
    const AstType astType; // used to determine type at runtime (tests only)
protected:
    // Constructor for member initialisation in derived classes
    explicit ExprAST(AstType ast);
public:
    // Constructor
    explicit ExprAST();

    // astType getter
    const AstType getAstType() const;
};

class FloatExpAST : public ExprAST {
private:
    const float value; // value of the float litteral
public:
    // Constructor
    explicit FloatExpAST(const float val);

    // value getter
    const float getValue() const;
};

class ProtoTypeAST : public ExprAST {
private:
    const std::string name;                 // name of the function
    const std::vector<std::string> args;    // arguments name of the function
public:
    // Constructor
    explicit ProtoTypeAST(const std::string theName, const std::vector<std::string> theArgs);

    // name getter
    const std::string getName() const;

    // args getter
    const std::vector<std::string> getArgs() const;
};

class FunctionAST : public ExprAST {
private:
    ProtoTypeAST* prototype;    // prototype of the function
    ExprAST* body;              // body of the function
public:
    // constructor
    explicit FunctionAST(ProtoTypeAST* proto, ExprAST* theBody);

    // prototype getter
    ProtoTypeAST* getPrototype() const;

    // body getter
    ExprAST* getBody() const;
};


// Parser for the MeeMaw language.
// Generate the Abstract Syntax Tree for each token parsed.
class Parser {
private:
    Lexer& lexer;
public:
    explicit Parser(Lexer& lexer);

    // Parse top level expression
    // top ::= floatlitexp
    ExprAST* parseTopLevelExpr();

    // Parse float litteral expression
    // floatlitexp ::= float
    FloatExpAST* parseFloatLitteralExpr();
};

#endif // PARSER_H