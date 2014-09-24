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
#ifndef PARSER_H
#define PARSER_H

#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"

#include <string>
#include <vector>

// Forward declarations
class Lexer;
class CodeGenerator;

// Specify ExprAST type without using RTTI.
// This is only usefull for tesing purpose and should not be used in actual
// code.
enum class AstType {
    NONE = 0, //should never happen, only the base class use it
    FLOAT_LITTERAL = 1,
    PROTOTYPE = 2,
    FUNCTION = 3,
    FLOAT_CONSTANT_VARIABLE_DECLARATION = 4,
};

// Base class for all expression nodes.
class ExprAST {
private:
    const AstType astType; // used to determine type at runtime (tests only)

protected:
    // Constructor for member initialisation in derived classes
    explicit ExprAST(AstType ast);

    // Simple error display helper for use in codeGen() method with one param.s
    // Always returns nullptr in order to be used like this :
    //     return CodeGenError("blahblah");
    // No newline needed at the end of the message
    template<typename T>
    static std::nullptr_t CodeGenError(const char* const msg, T t);

    // Simple error display helper for use in codeGen() method
    // Always returns nullptr in order to be used like this :
    //     return CodeGenError("blahblah");
    // No newline needed at the end of the message
    static std::nullptr_t CodeGenError(const char* const msg);

public:
    // Constructor
    explicit ExprAST();

    // Virtual destructor since we have a virtual function
    virtual ~ExprAST() {}

    // astType getter
    const AstType getAstType() const;

    // Generate LLVM Intermediary Representation for the node.
    // Call recursively codeGen() method on enclosing nodes if any.
    virtual llvm::Value* codeGen(CodeGenerator* codeGenerator) = 0;
};

// TODO correct the name of this class to FloatLitteralExprAST
class FloatExpAST : public ExprAST {
private:
    const float value; // value of the float litteral
public:
    // Constructor
    explicit FloatExpAST(const float val);

    // value getter
    const float getValue() const;

    virtual llvm::Value* codeGen(CodeGenerator* codeGenerator);
};

class ProtoTypeAST : public ExprAST {
private:
    const std::string name;                 // name of the function
    const std::vector<std::string> args;    // arguments name of the function
public:
    // Constructor
    explicit ProtoTypeAST(const std::string& theName, const std::vector<std::string>& theArgs);

    // name getter
    const std::string getName() const;

    // args getter
    const std::vector<std::string> getArgs() const;

    virtual llvm::Function* codeGen(CodeGenerator* codeGenerator);
};

class FunctionAST : public ExprAST {
private:
    ProtoTypeAST* prototype;    // prototype of the function
    ExprAST* body;              // body of the function
public:
    // constructor
    explicit FunctionAST(ProtoTypeAST* proto, ExprAST* theBody);

    // prototype getter
    ProtoTypeAST* getPrototype() const;

    // body getter
    ExprAST* getBody() const;

    virtual llvm::Function* codeGen(CodeGenerator* codeGenerator);
};

class FloatConstantVariableDeclarationExprAST : public ExprAST {
public:
    // Constructor
    explicit FloatConstantVariableDeclarationExprAST(std::string theName, FloatExpAST* theRhsExpr);

    // value name
    const std::string getName() const;

    // value getter
    FloatExpAST* getRhsExpr() const;

    virtual llvm::Value* codeGen(CodeGenerator* codeGenerator);

private:
    const std::string name; // Name of the constant
    FloatExpAST* rhsExpr;   // float litteral expression to be named
};


// Parser for the MeeMaw language.
// Generate the Abstract Syntax Tree for each token parsed.
class Parser {
private:
    Lexer& lexer;

    // Simple error display helper for use in parse* methods
    // Always returns nullptr in order to be used like this :
    //     return ParseError("blahblah");
    // No newline needed at the end of the message
    static std::nullptr_t ParserError(const char* const msg);

    // Error display helper when handling with unexpected token
    // Always returns nullptr in order to be used like this :
    //     return ParseError("blahblah", t1, t2);
    // No newline needed at the end of the message
    static std::nullptr_t ParserErrorUnexpectedToken(const char* const when, const int actualToken, const int expectedToken);
public:
    explicit Parser(Lexer& lexer);

    // Parse top level expression
    // top ::= floatlitexp
    ExprAST* parseTopLevelExpr();

    // Parse float litteral expression
    // floatlitexp ::= float
    FloatExpAST* parseFloatLitteralExpr();

    // Parse floa constant declaration expression
    // floatconstdecexp ::= let identifier = floatlitexp
    FloatConstantVariableDeclarationExprAST* parseFloatConstantVariableDeclarationExpr();
};

#endif // PARSER_H