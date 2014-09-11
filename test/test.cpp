// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <chrono>
#include <string>

#include "../src/lexer.h"
#include "../src/parser.h"

// TESTS /////////////////////////////////////
//maximum difference between 2 floats to be considered equal
const float F_EPSILON = std::numeric_limits<float>::min() * 10.0;

TEST_CASE("Environment works"){
    INFO( "This means we have a working developping environment." );
}

TEST_CASE("Lexer categorise float"){
    // stream to parse by lexer
    std::stringstream test;

    // the lexer
    Lexer lex = Lexer(test);

    // random generator initialisation
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine engine(seed);

    // float generator
    std::uniform_real_distribution<float> floatDistribution(0.0, std::numeric_limits<float>::max());
    auto randFloatGenerator = std::bind(floatDistribution, engine);

    // unsigned char generator
    std::uniform_int_distribution<unsigned char> ucharDistribution(0, std::numeric_limits<unsigned char>::max());
    auto randUCharGenerator = std::bind(ucharDistribution, engine);

    SECTION("1.0 is categorized as float"){
        test << "1.0";

        int tokId = lex.getNextToken();

        REQUIRE(tokId == TOK_FLOAT);
        REQUIRE(lex.getFloatValue() == 1.0);
    }

    SECTION("Any float between 0.0 and max float is categorized as float"){
        float f = randFloatGenerator();

        test << std::to_string(f);

        int tokId = lex.getNextToken();

        REQUIRE(tokId == TOK_FLOAT);
        REQUIRE(abs(lex.getFloatValue() - f) < F_EPSILON);
    }

    SECTION("Random char is not categorized as float"){
        unsigned char c = randUCharGenerator();

        test << c;

        int tokId = lex.getNextToken();

        REQUIRE(tokId == static_cast<int>(c));
    }
}

TEST_CASE("Parser generate AST for float"){
    std::stringstream test;         // stream to parse by lexer
    Lexer lex = Lexer(test);        // the lexer
    Parser parser = Parser(lex);    // the parser

    test << "1.0";

    ExprAST* ast = parser.parseFloat();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->dump() == "FloatExprAST(value=1.0)");
}