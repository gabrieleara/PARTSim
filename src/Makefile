CXXFLAGS=-Wno-deprecated -Wno-deprecated-declarations --std=c++11 -I.

all: example

example: $(patsubst %.cpp,%.o,$(wildcard *.cpp))
	g++ -o $@ $^

clean:
	rm -f eaxmple *.o capacities.txt debug.txt *~ *.bak
