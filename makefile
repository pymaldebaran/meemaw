CXX=clang++
FLAGS=-std=c++03 -Wall
all:

clean:
	@echo === cleaning...
	rm -fr build/*

test:
	@echo === compiling tests...
	$(CXX) $(FLAGS) test/test.cpp -o bin/test -Iinclude/
	@echo === running tests...
	bin/test

format:
	@echo === formating files...
	uncrustify -c uncrustify.cfg test/test.cpp --replace --no-backup

lint:
	@echo === checking quality of files...
	cppcheck test/test.cpp

.PHONY: all clean test