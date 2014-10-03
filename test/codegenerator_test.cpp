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

#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/codegenerator.h"
#include "../src/ast.h"

// TODO add test for code generation from an empty input
TEST_CASE("Code generator produce code for float litteral expression from ast") {
    std::stringstream in;                           // stream to parse by lexer
    TokenQueue tokens;                              // token container
    NewLexer lex = NewLexer(in, tokens);            // the lexer
    AbstractSyntaxTree ast;                         // the Abstract Syntax Tree
    NewParser parser = NewParser(tokens, ast);      // the parser
    Code code;                                      // code to generate
    CodeGenerator gen = CodeGenerator(ast, code);   // the code generator

    in << "1.0";

    REQUIRE(lex.tokenize() == 1);
    REQUIRE(parser.parse() == 1);

    REQUIRE(gen.init() == true);
    REQUIRE(gen.codegen() == true);
    //TODO add some test about the module content
}

TEST_CASE("Code generator produce code for float litteral constant declaration expression from ast") {
    std::stringstream in;                           // stream to parse by lexer
    TokenQueue tokens;                              // token container
    NewLexer lex = NewLexer(in, tokens);            // the lexer
    AbstractSyntaxTree ast;                         // the Abstract Syntax Tree
    NewParser parser = NewParser(tokens, ast);      // the parser
    Code code;                                      // code to generate
    CodeGenerator gen = CodeGenerator(ast, code);   // the code generator

    in << "let aaa = 3.0";

    REQUIRE(lex.tokenize() == 4);
    REQUIRE(parser.parse() == 1);

    REQUIRE(gen.init() == true);
    REQUIRE(gen.codegen() == true);
    //TODO add some test about the module content
}

// TODO add test for code running from an empty input
TEST_CASE("Running code works for float litteral expression") {
    std::stringstream in;                           // stream to parse by lexer
    TokenQueue tokens;                              // token container
    NewLexer lex = NewLexer(in, tokens);            // the lexer
    AbstractSyntaxTree ast;                         // the Abstract Syntax Tree
    NewParser parser = NewParser(tokens, ast);      // the parser
    Code code;                                      // code to generate and run
    CodeGenerator gen = CodeGenerator(ast, code);   // the code generator

    in << "1.0";

    REQUIRE(lex.tokenize() == 1);
    REQUIRE(parser.parse() == 1);
    REQUIRE(gen.init() == true);
    REQUIRE(gen.codegen() == true);

    float result = -100.0;
    REQUIRE(code.run(result) == true);
    REQUIRE(result == 1.0);
}

TEST_CASE("Running code works for float litteral constant declaration expression") {
    std::stringstream in;                           // stream to parse by lexer
    TokenQueue tokens;                              // token container
    NewLexer lex = NewLexer(in, tokens);            // the lexer
    AbstractSyntaxTree ast;                         // the Abstract Syntax Tree
    NewParser parser = NewParser(tokens, ast);      // the parser
    Code code;                                      // code to generate and run
    CodeGenerator gen = CodeGenerator(ast, code);   // the code generator

    in << "let aaa = 3.0";

    REQUIRE(lex.tokenize() == 4);
    REQUIRE(parser.parse() == 1);
    REQUIRE(gen.init() == true);
    REQUIRE(gen.codegen() == true);

    float result = -100.0;
    REQUIRE(code.run(result) == true);
    REQUIRE(result == 3.0);
}
// TODO add a test with usage of the created constant
