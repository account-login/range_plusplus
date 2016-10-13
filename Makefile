
CXXFLAGS += -Wall -g -std=c++11

.PHONY : all clean clean-all test examples update_readme

all: test

clean:
	rm -rf ./build/test*

clean-all:
	rm -rf ./build/*

build/catch_main.o: catch.hpp
	$(CXX) $(CXXFLAGS) -DCATCH_CONFIG_MAIN -xc++ -c -o build/catch_main.o $^

build/test.o: test.cpp range.hpp
	$(CXX) $(CXXFLAGS) -xc++ -c test.cpp -o build/test.o

build/test: build/test.o build/catch_main.o
	$(CXX) $(CXXFLAGS) $^ -o build/test

test: build/test
	./build/test -s -d yes

examples: build/examples
	./build/examples

build/examples: examples.cpp range.hpp
	$(CXX) $(CXXFLAGS) -o build/examples examples.cpp

update_readme: README.template.md
	cat README.template.md examples.cpp - <<<'```' >README.md
