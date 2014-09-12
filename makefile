CXX=clang++
FLAGS=-std=c++11 -Wall

all: test

clean:
	@echo === cleaning...
	rm -fr build/*

format:
	@echo === formating files...
	uncrustify -c uncrustify.cfg test/*.cpp --replace --no-backup
	uncrustify -c uncrustify.cfg src/*.cpp --replace --no-backup
	uncrustify -c uncrustify.cfg src/*.h --replace --no-backup

lint:
	@echo === checking quality of files...
	cppcheck test/test.cpp

test: parser.o lexer.o
	@echo === compiling tests...
	$(CXX) $(FLAGS) test/test.cpp -o bin/test -Iinclude/ build/parser.o build/lexer.o

parser.o: src/parser.cpp
	@echo === compiling parser
	$(CXX) $(FLAGS) -c src/parser.cpp -o build/parser.o -Iinclude/

lexer.o: src/lexer.cpp
	@echo === compiling lexer
	$(CXX) $(FLAGS) -c src/lexer.cpp -o build/lexer.o -Iinclude/

runtest: test
	@echo === running tests...
	bin/test

.PHONY: clean test runtest