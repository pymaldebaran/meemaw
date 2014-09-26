CXX=clang++
FLAGS=-std=c++11 -Wall -g

all: test

clean:
	@echo === cleaning...
	rm -fr build/* bin/*

format:
	@echo === formating files...
	uncrustify -c uncrustify.cfg test/*.cpp --replace --no-backup
	uncrustify -c uncrustify.cfg src/*.cpp --replace --no-backup
	uncrustify -c uncrustify.cfg src/*.h --replace --no-backup

lint:
	@echo === checking quality of files...
	cppcheck test/test.cpp

test: test/test.cpp parser.o lexer.o codegenerator.o ast.o
	@echo === compiling tests...
	$(CXX) $(FLAGS) test/test.cpp -o bin/test -Iinclude/ build/parser.o build/lexer.o build/codegenerator.o build/ast.o `llvm-config --cppflags --ldflags --libs core engine`

parser.o: src/parser.cpp
	@echo === compiling parser
	$(CXX) $(FLAGS) -c src/parser.cpp -o build/parser.o -Iinclude/ `llvm-config --cppflags`

lexer.o: src/lexer.cpp
	@echo === compiling lexer
	$(CXX) $(FLAGS) -c src/lexer.cpp -o build/lexer.o -Iinclude/

codegenerator.o: src/codegenerator.cpp
	@echo === compiling codegenerator
	$(CXX) $(FLAGS) -c src/codegenerator.cpp -o build/codegenerator.o -Iinclude/ `llvm-config --cppflags`

ast.o: src/ast.cpp
	@echo === compiling ast
	$(CXX) $(FLAGS) -c src/ast.cpp -o build/ast.o -Iinclude/ `llvm-config --cppflags`

runtest: test
	@echo === running tests...
	bin/test

.PHONY: clean test runtest
