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

FloatExpAST::FloatExpAST(float val) :
    ExprAST(AstType::FLOAT_LITTERAL),
    value(val)
{}

float FloatExpAST::getValue() {
    return value;
}

ProtoTypeAST::ProtoTypeAST(std::string theName, std::vector<std::string> theArgs) :
    ExprAST(AstType::PROTOTYPE),
    name(theName),
    args(theArgs)
{}

std::string ProtoTypeAST::getName() {
    return name;
}

std::vector<std::string> ProtoTypeAST::getArgs() {
    return args;
}

FunctionAST::FunctionAST(ProtoTypeAST* proto, ExprAST* theBody) :
    ExprAST(AstType::FUNCTION),
    prototype(proto),
    body(theBody)
{}

ProtoTypeAST* FunctionAST::getPrototype() {
    return prototype;
}

ExprAST* FunctionAST::getBody() {
    return body;
}

ExprAST* Parser::parseTopLevelExpr() {
    if (ExprAST* expr = parseFloatLitteralExpr()) {
        // Make an anonymous proto.
        ProtoTypeAST* proto = new ProtoTypeAST("", std::vector<std::string>());
        return new FunctionAST(proto, expr);
    }

    return nullptr;
}

FloatExpAST* Parser::parseFloatLitteralExpr() {
    FloatExpAST* result = new FloatExpAST(lexer.getFloatValue());
    lexer.getNextToken(); // consume the float
    return result;
}