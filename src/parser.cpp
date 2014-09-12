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

const AstType ExprAST::getAstType() const {
    return astType;
}

FloatExpAST::FloatExpAST(float val) :
    ExprAST(AstType::FLOAT_LITTERAL),
    value(val)
{}

const float FloatExpAST::getValue() const {
    return value;
}

ProtoTypeAST::ProtoTypeAST(const std::string theName, const std::vector<std::string> theArgs) :
    ExprAST(AstType::PROTOTYPE),
    name(theName),
    args(theArgs)
{}

const std::string ProtoTypeAST::getName() const {
    return name;
}

const std::vector<std::string> ProtoTypeAST::getArgs() const {
    return args;
}

FunctionAST::FunctionAST(ProtoTypeAST* proto, ExprAST* theBody) :
    ExprAST(AstType::FUNCTION),
    prototype(proto),
    body(theBody)
{}

ProtoTypeAST* FunctionAST::getPrototype() const {
    return prototype;
}

ExprAST* FunctionAST::getBody() const {
    return body;
}

ExprAST* Parser::parseTopLevelExpr() {
    if (ExprAST * expr = parseFloatLitteralExpr()) {
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