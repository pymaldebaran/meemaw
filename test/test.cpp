// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <chrono>

// The lexer returns tokens [0-255] if it is an unknown character, otherwise
// one of these for known things.
const int TOK_FLOAT = -1;
const int TOK_EOF   = -2;

// Lexer for the MeeMaw language
class Lexer {
private:
    std::istream& stream;   // input stream to parse
    float floatValue;       // Filled in if curTok == TOK_FLOAT
    int currentToken;       // Current token i.e. the last returned by getNextToken()
    int lastChar;           // Store the last char read by gettok()

    // gettok - Return the next token from standard input.
    int gettok();

public:
    // Constructor
    explicit Lexer(std::istream& strm);

    // floatValue getter
    float getFloatValue();

    // Reads another token from the lexer and updates CurTok with its results
    int getNextToken();
};

int Lexer::gettok()
{
    lastChar = stream.get();

    // Float: [0-9.]+
    if (isdigit(lastChar) || lastChar == '.') {
        std::string floatStr;

        do {
            floatStr += lastChar;
            lastChar = stream.get();
        } while (isdigit(lastChar) || lastChar == '.');

        floatValue = strtof(floatStr.c_str(), nullptr);
        return TOK_FLOAT;
    }

    // Check for end of file.  Don't eat the EOF.
    if (lastChar == EOF)
        return TOK_EOF;

    // Otherwise, just return the character as its ascii value.
    int thisChar = lastChar;
    lastChar = stream.get();
    return thisChar;
}

Lexer::Lexer(std::istream& strm) :
    stream(strm),
    floatValue(0.0),
    currentToken(0),
    lastChar(' ')
{
}

float Lexer::getFloatValue()
{
    //TODO check if currentToken == TOK_FLOAT
    return floatValue;
}

int Lexer::getNextToken()
{
    return currentToken = gettok();
}


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