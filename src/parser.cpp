#include "parser.h"

#include "lexer.h"

Parser::Parser(Lexer& lex) :
    lexer(lex)
{}

ExprAST* Parser::parseTopLevelExpr() {
    return nullptr;
}