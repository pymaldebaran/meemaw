#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"

class ExprAST;

class CodeGenerator {
private:
    llvm::Module* module;       // module used for code generation
    llvm::IRBuilder<> builder;  // helper taht makes it easy to generate LLVM instructions
    std::map<std::string, llvm::Value*> namedValues;
public:
    // Constuctor
    CodeGenerator(llvm::Module* mod);

    // module getter
    llvm::Module* getModule() const;

    // builder getter
    llvm::IRBuilder<>& getBuilder();

    // namedValue getter
    std::map<std::string, llvm::Value*>& getNamedValues();

    // Generate LLVM Intermediary Representation for the given AST.
    // Use a recursive call to AST node codeGen() method.
    llvm::Value* codeGen(ExprAST* ast);
};

#endif // CODEGENERATOR_H