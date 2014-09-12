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

ExprAST* Parser::parseTopLevelExpr() {
    return nullptr;
}