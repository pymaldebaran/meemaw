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

#include "catch.hpp"

#include "llvm/Support/TargetSelect.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h" // needed to prevent "Interpreter has not been linked in" when creating JIT engine
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include <string>
#include <memory>

#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/codegenerator.h"
#include "../src/ast.h"

TEST_CASE("Code generated for float litteral expression is correct") {
    // LLVM stuff
    // TODO: put all this in a class... but witch one
    llvm::InitializeNativeTarget();
    llvm::LLVMContext& context = llvm::getGlobalContext();
    llvm::Module* module = new llvm::Module("Test module", context); // Make the module, which holds all the code.

    if (module == nullptr) {
        fprintf(stderr, "no module\n");
    }

    // non LLVM stuff
    std::stringstream test;                     // stream to parse by lexer
    Lexer lex = Lexer(test);                    // the lexer
    Parser parser = Parser(lex);                // the parser
    CodeGenerator gen = CodeGenerator(module);  // the code generator

    // create a JIT Engine (This takes ownership of the module).
    std::string errStr;
    llvm::ExecutionEngine* execEngine = llvm::EngineBuilder(module).setErrorStr(&errStr).setEngineKind(llvm::EngineKind::JIT).create();
    if (execEngine == nullptr) {
        fprintf(stderr, "no exec engine: %s\n", errStr.c_str());
    }

    test << "1.0";

    lex.getNextToken();
    FunctionAST* ast = parser.parseTopLevelExpr();
    llvm::Value* code = gen.codeGen(ast);

    REQUIRE(code != nullptr);
    //TODO add some test about the module content

    llvm::Function* funcCode = static_cast<llvm::Function*>(code);
    const std::vector<llvm::GenericValue> noArgs = std::vector<llvm::GenericValue>();

    float result = execEngine->runFunction(funcCode, noArgs).FloatVal;
    REQUIRE(result == 1.0);
}

TEST_CASE("Code generated for float litteral constant declaration expression is correct") {
    // LLVM stuff
    // TODO: put all this in a class... but witch one
    llvm::InitializeNativeTarget();
    llvm::LLVMContext& context = llvm::getGlobalContext();
    llvm::Module* module = new llvm::Module("Test module", context); // Make the module, which holds all the code.

    if (module == nullptr) {
        fprintf(stderr, "no module\n");
    }

    // non LLVM stuff
    std::stringstream test;                     // stream to parse by lexer
    Lexer lex = Lexer(test);                    // the lexer
    Parser parser = Parser(lex);                // the parser
    CodeGenerator gen = CodeGenerator(module);  // the code generator

    // create a JIT Engine (This takes ownership of the module).
    std::string errStr;
    llvm::ExecutionEngine* execEngine = llvm::EngineBuilder(module).setErrorStr(&errStr).setEngineKind(llvm::EngineKind::JIT).create();
    if (execEngine == nullptr) {
        fprintf(stderr, "no exec engine: %s\n", errStr.c_str());
    }

    test << "let aaa = 1.0";

    lex.getNextToken();
    FunctionAST* ast = parser.parseTopLevelExpr();
    llvm::Value* code = gen.codeGen(ast);

    REQUIRE(code != nullptr);
    //TODO add some test about the module content
    //     symbol table content ?

    llvm::Function* funcCode = static_cast<llvm::Function*>(code);
    const std::vector<llvm::GenericValue> noArgs = std::vector<llvm::GenericValue>();

    float result = execEngine->runFunction(funcCode, noArgs).FloatVal;
    REQUIRE(result == 1.0);
}

// TODO add a test with usage of the created constant
