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
#include "ast.h"

#include "llvm/Analysis/Verifier.h"

#include <iostream>

#include "codegenerator.h"
#include "parser.h"

Parser::Parser(Lexer& lex) :
    lexer(lex)
{}

ExprAST::ExprAST(AstType ast) :
    astType(ast)
{}

template<typename T>
std::nullptr_t ExprAST::CodeGenError(const char* const msg, T t) {
    std::cerr << "[CODEGEN ERROR] " << msg << t << "\n";

    return nullptr;
}

std::nullptr_t ExprAST::CodeGenError(const char* const msg) {
    std::cerr << "[CODEGEN ERROR] " << msg << "\n";

    return nullptr;
}

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

llvm::Value* FloatExpAST::codeGen(CodeGenerator* codeGenerator) {
    return llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(value));;
}

ProtoTypeAST::ProtoTypeAST(const std::string& theName, const std::vector<std::string>& theArgs) :
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

llvm::Function* ProtoTypeAST::codeGen(CodeGenerator* codeGenerator) {
    // TODO remove this as soon as we support real function in the language
    if (name != "") {
        return CodeGenError("Generation of code of named function not yet supported : name=", name);
    }
    if (not args.empty()) {
        return CodeGenError("generation of code of function with args not yet supported : nb args=", args.size());
    }

    // We need this to create types
    llvm::LLVMContext& globalContext = llvm::getGlobalContext();

    // The signature of the arguments
    // WARNING For the moment MeeMaw does not support function declaration that
    //         why we can only create empty args vector
    std::vector<llvm::Type*> argsType = std::vector<llvm::Type*>(
        args.size(), // == 0
        llvm::Type::getFloatTy(globalContext));

    // The return type of the function
    // WARNING For the moment MeeMaw only support floats that's why we can only
    //         return float
    llvm::Type* returnType = llvm::Type::getFloatTy(globalContext);

    // The complete function type of the function
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        returnType, // return type of the function
        argsType,   // types of the arguments
        false);     // not a vararg function
    if (funcType == nullptr) {
        return CodeGenError("Prototype AST node failed to get a function type.");
    }

    // Creates the function that the prototype will correspond to
    llvm::Function* function = llvm::Function::Create(
        funcType,                           // type of the function
        llvm::Function::ExternalLinkage,    // the function may be defined outside the current module and/or that it is callable by functions outside the module
        name,                               // name of the function
        codeGenerator->getModule());        // the module to insert the function into
    if (function == nullptr) {
        return CodeGenError("Prototype AST node failed to create the function object.");
    }

    // WARNING as MeeMaw language currently only support anonymous function we
    //         do not handle name conflict.
    //         If we had to it would be here (cf tutorial).

    // WARNING as MeeMaw language currently only support arg less function we
    //         do not arg names.
    //         If we had to it would be here (cf tutorial).

    return function;
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

llvm::Function* FunctionAST::codeGen(CodeGenerator* codeGenerator) {
    // check if the function node is valid
    if (prototype == nullptr) {
        return CodeGenError("Function AST node without prototype.");
    }

    // Clear the symbol table to make sure that there isn’t anything in it from
    // the last function we compiled
    codeGenerator->getSymbolTable().clear();

    // Generate the code of the prototype
    llvm::Function* func = prototype->codeGen(codeGenerator);
    if (func == nullptr) {
        return CodeGenError("Function AST node can not generate prototype code.");
    }

    // Create a new basic block to insert the body into.
    llvm::BasicBlock* basicBlk = llvm::BasicBlock::Create(
        llvm::getGlobalContext(),
        "function basic block", //name
        func);                  //parent
    if (basicBlk == nullptr) {
        return CodeGenError("Function AST node can not create basic block to put the code in.");
    }
    // From now on we insert into the new block
    codeGenerator->getBuilder().SetInsertPoint(basicBlk);

    // Generate the code of the body
    llvm::Value* returnValue = body->codeGen(codeGenerator);
    if (returnValue == nullptr) {
        // Error reading body, remove function
        func->eraseFromParent();

        return CodeGenError("Function AST node can not generate body code.");
    }

    // Finish off the function.
    codeGenerator->getBuilder().CreateRet(returnValue);

    // Validate the generated code, checking for consistency.
    bool verifyError = llvm::verifyFunction(*func, llvm::PrintMessageAction);
    if (verifyError) {
        return CodeGenError("Function AST node has generated code but failed verification.");
    }

    return func;
}

FloatConstantVariableDeclarationExprAST::FloatConstantVariableDeclarationExprAST(std::string theName, FloatExpAST* theRhsExpr) :
    ExprAST(AstType::FLOAT_CONSTANT_VARIABLE_DECLARATION),
    name(theName),
    rhsExpr(theRhsExpr)
{}

const std::string FloatConstantVariableDeclarationExprAST::getName() const {
    return name;
}

FloatExpAST* FloatConstantVariableDeclarationExprAST::getRhsExpr() const {
    return rhsExpr;
}


llvm::Value* FloatConstantVariableDeclarationExprAST::codeGen(CodeGenerator* codeGenerator) {
    // Check the validity of the name
    if (name == "") {
        return CodeGenError("Float Constant has no name.");
    }

    // Generate the right hand side expression code
    if (rhsExpr == nullptr) {
        return CodeGenError("Float constant has no right hand side expression.");
    }
    llvm::Value* rhsValue = rhsExpr->codeGen(codeGenerator);
    if (rhsValue == nullptr) {
        return CodeGenError("Can't generate code for right hand side expression of the float constant.");
    }

    // Insert the value in the symbol table
    auto symbolTable = codeGenerator->getSymbolTable();
    if (symbolTable.count(name) > 0) {
        return CodeGenError("Float constant name already in use, MeeMaw refuse symbol shadowing. Name=", name);
    }
    symbolTable[name] = rhsValue;

    // Finally we return the value
    return rhsValue;
}
