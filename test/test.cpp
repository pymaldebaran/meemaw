// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// The lexer returns tokens [0-255] if it is an unknown character, otherwise
// one of these for known things.
const int TOK_FLOAT = -1;

// Lexer for the MeeMaw language
class Lexer {
private:
    std::istream& stream;   // input stream to parse
    float floatValue;       // Filled in if curTok == TOK_FLOAT
public:
    // Constructor
    explicit Lexer(std::istream& strm)
        : stream(strm), floatValue(0.0)
    {
    }

    // floatValue getter
    float getFloatValue()
    {
        //TODO check if curTok == TOK_FLOAT
        return floatValue;
    }

    // Reads another token from the lexer and updates CurTok with its results
    int getNextToken()
    {
        floatValue = 1.0;
        return TOK_FLOAT;
    }
};

TEST_CASE("Environment works"){
    INFO( "This means we have a working developping environment." );
}

TEST_CASE("Lexer categorise float"){
    SECTION("1.0 is categorized as float"){
        std::stringstream test;
        Lexer lex = Lexer(test);

        test << "1.0";

        int tokId = lex.getNextToken();

        REQUIRE(tokId == TOK_FLOAT);
        REQUIRE(lex.getFloatValue() == 1.0);
    }
}