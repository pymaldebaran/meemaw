CXX=clang++
FLAGS=-std=c++03 -Wall

clean:
	@echo === cleaning...
	rm -fr build/*

format:
	@echo === formating files...
	uncrustify -c uncrustify.cfg test/test.cpp --replace --no-backup

lint:
	@echo === checking quality of files...
	cppcheck test/test.cpp

test:
	@echo === compiling tests...
	$(CXX) $(FLAGS) test/test.cpp -o bin/test -Iinclude/

runtest: test
	@echo === running tests...
	bin/test --success

all: clean format lint test runtest

.PHONY: all clean test