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
#include "llvm/ExecutionEngine/ExecutionEngine.h"

class ExprAST;
class AbstractSyntaxTree;

class Code {
public:
    // Constructor
    explicit Code();

    // Set the codeIR
    bool setCodeIR(llvm::Value* ir);

    // Create a Just In Time engine to run the code
    //
    // Must be call only once and before any call to run() method.
    bool createJIT(llvm::Module* module);

    // Run the code
    //
    // Returns true and the result of the last expression in the float reference
    //        if code execution worked.
    // Returns flase otherwise.
    bool run(float& result);

private:
    llvm::Value* codeIR;                    // LLVM value to put the IR code after generation
    llvm::ExecutionEngine* execEngineJIT;   // LLVM JIT execution engine to execute the code

    // Error function helper : display the error message and returns false.
    static bool Error(const std::string msg);
};

class CodeGenerator {
public:
    // Constructor
    explicit CodeGenerator(AbstractSyntaxTree& theAst, Code& theCode);

    // Initialise the LLVM internals needed to generate code
    //
    // Returns true if the initialisation when well
    //         false elsewise.
    bool init();

    // Generate code from the AST.
    // Use a recursive call to AST node codeGen() method.
    //
    // Returns true if the generation succeed
    //         false in al other case
    bool codegen();

    // LLVM module getter
    llvm::Module* getModule() const;

    // LLVM IR builder getter
    llvm::IRBuilder<>& getBuilder();

    // symbol table getter
    std::map<std::string, llvm::Value*>& getSymbolTable();

private:
    AbstractSyntaxTree& ast;                            // Abstract syntax tree to convert in code
    Code& code;                                         // Code to produce from AST
    std::map<std::string, llvm::Value*> symbolTable;    // Symbol table

    llvm::Module* module;                               // Module used for code generation
    llvm::IRBuilder<> builder;                          // Helper taht makes it easy to generate LLVM instructions

    // Error function helper : display the error message and returns false.
    bool Error(const std::string msg);
};

#endif // CODEGENERATOR_H