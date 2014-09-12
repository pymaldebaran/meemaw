#include "parser.h"

#include "lexer.h"

Parser::Parser(Lexer& lex) :
    lexer(lex)
{}

ExprAST::ExprAST(AstType ast) :
    astType(ast)
{}

ExprAST::ExprAST() :
    astType(AstType::NONE)
{}

AstType ExprAST::getAstType() {
    return astType;
}

FloatExpAST::FloatExpAST() :
    ExprAST(AstType::FLOAT_LITTERAL)
{}

float FloatExpAST::getValue() {
    return value;
}

ProtoTypeAST::ProtoTypeAST() :
    ExprAST(AstType::PROTOTYPE)
{}

std::string ProtoTypeAST::getName() {
    return name;
}

std::vector<std::string> ProtoTypeAST::getArgs() {
    return args;
}

FunctionAST::FunctionAST() :
    ExprAST(AstType::FUNCTION)
{}

ProtoTypeAST* FunctionAST::getPrototype() {
    return prototype;
}

ExprAST* FunctionAST::getBody() {
    return body;
}

ExprAST* Parser::parseTopLevelExpr() {
    return nullptr;
}

FloatExpAST* Parser::parseFloatLitteralExpr() {
    return nullptr;
}