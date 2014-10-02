CXX=clang++
FLAGS=-std=c++11 -Wall -g

all: test

clean:
	@echo === cleaning...
	rm -fr build/* bin/*

format:
	@echo === formating modified files...
	git --no-pager diff --name-only | grep '\.h$$\|\.cpp$$' > build/to_format_list.txt || true
	uncrustify -c uncrustify.cfg --replace --no-backup -F build/to_format_list.txt

lint:
	@echo === checking quality of files...
	cppcheck test/test.cpp

test: test/test.cpp test/lexer_test.cpp test/parser_test.cpp test/codegenerator_test.cpp lexer.o parser.o codegenerator.o ast.o
	@echo === compiling tests...
	$(CXX) $(FLAGS) test/test.cpp test/lexer_test.cpp test/parser_test.cpp test/codegenerator_test.cpp -o bin/test -Iinclude/ build/lexer.o build/parser.o build/codegenerator.o build/ast.o `llvm-config --cppflags --ldflags --libs core engine`

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
