#include "codegenerator.h"

#include "parser.h"

CodeGenerator::CodeGenerator(llvm::Module* mod) :
    module(mod),
    builder(llvm::getGlobalContext()),
    namedValues()
{}

llvm::Module* CodeGenerator::getModule() const {
    return module;
}

llvm::IRBuilder<>& CodeGenerator::getBuilder() {
    return builder;
}

std::map<std::string, llvm::Value*>& CodeGenerator::getNamedValues() {
    return namedValues;
}

llvm::Value* CodeGenerator::codeGen(ExprAST* ast) {
    return ast->codeGen(this);
}