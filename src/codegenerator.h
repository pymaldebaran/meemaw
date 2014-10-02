/*
 * Copyright (c) 2014, Pierre-Yves Martin
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"

class ExprAST;
class AbstractSyntaxTree;

class CodeGenerator {
private:
    llvm::Module* module;                               // module used for code generation
    llvm::IRBuilder<> builder;                          // helper taht makes it easy to generate LLVM instructions

    std::map<std::string, llvm::Value*> symbolTable;    // symbol table
public:
    // Constuctor
    CodeGenerator(llvm::Module* mod);

    // module getter
    llvm::Module* getModule() const;

    // builder getter
    llvm::IRBuilder<>& getBuilder();

    // namedValue getter
    std::map<std::string, llvm::Value*>& getSymbolTable();

    // Generate LLVM Intermediary Representation for the given AST.
    // Use a recursive call to AST node codeGen() method.
    llvm::Value* codeGen(ExprAST* ast);
};

class Code {
public:
    // Run the code
    //
    // Return the result of the last expression
    float run();
};

class NewCodeGenerator {
public:
    // Constructor
    explicit NewCodeGenerator(AbstractSyntaxTree& theAst, Code& theCode);

    // Generate code from the AST.
    // Use a recursive call to AST node codeGen() method.
    //
    // Returns true if the generation succeed
    //         false in al other case
    bool codegen();
};

#endif // CODEGENERATOR_H