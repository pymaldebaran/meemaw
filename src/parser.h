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

#include <string>
#include <vector>

// Forward declarations
class Lexer;
class CodeGenerator;
class ProtoTypeAST;
class FunctionAST;
class ExprAST;
class FloatExpAST;
class FloatConstantVariableDeclarationExprAST;


// Parser for the MeeMaw language.
// Generate the Abstract Syntax Tree for each token parsed.
class Parser {
private:
    Lexer& lexer;

    // Simple error display helper for use in parse* methods
    //
    // Always returns nullptr in order to be used like this :
    //     return ParseError("blahblah");
    //
    // No newline needed at the end of the message
    static std::nullptr_t ParserError(const char* const msg);

    // Error display helper when handling with unexpected token but can't
    // specify which token was expected
    //
    // Always returns nullptr in order to be used like this :
    //     return ParseError("blahblah", t1);
    //
    // No newline needed at the end of the message
    static std::nullptr_t ParserErrorUnexpectedToken(const char* const when, const int actualToken);

    // Error display helper when handling with unexpected token
    //
    // Always returns nullptr in order to be used like this :
    //     return ParseError("blahblah", t1, t2);
    //
    // No newline needed at the end of the message
    static std::nullptr_t ParserErrorUnexpectedToken(const char* const when, const int actualToken, const int expectedToken);
public:
    explicit Parser(Lexer& lexer);

    // Parse top level expression wrapping them in an anonymous function
    // returning the appropriate type.
    //
    // A top level expression is an expression that can be used outside of any
    // other program structure. Only expression that could be used
    // at module level (or directly in the interpreter) is a top level
    // expression.
    //
    // A top level expression behave like an anonymous function that returns the
    // value of the expression.
    //
    // top ::= primaryexpr
    FunctionAST* parseTopLevelExpr();

    // Parse primary expression by selecting the correct parse* method according
    // according to the current token.
    //
    // A primary expression is an expression that can be used inside any other
    // program structure. Since MeeMaw is an expression based language any
    // expression is a primary expression.
    //
    // primaryexpr
    //      ::= floatlitexp
    //      ::= floatconstdeclexp
    ExprAST* parsePrimaryExpr();

    // Parse float litteral expression
    //
    // floatlitexp ::= float
    FloatExpAST* parseFloatLitteralExpr();

    // Parse floa constant declaration expression
    //
    // floatconstdeclexp ::= let identifier = floatlitexp
    FloatConstantVariableDeclarationExprAST* parseFloatConstantVariableDeclarationExpr();
};

#endif // PARSER_H