CXXFLAGS=-Wno-deprecated -Wno-deprecated-declarations --std=c++11 -I. -I../../ -g

CPP_FILES=$(filter-out ../../energyMRTKernel_example.cpp, $(wildcard ../../*.cpp) $(wildcard cbs_easy.cpp))
HPP_FILES=$(wildcard ../*.hpp)
OBJ_FILES=$(patsubst %.cpp,%.o,$(CPP_FILES))

SOURCES=$(CPP_FILES) $(HPP_FILES)

.PHONY: all build test clean execute build_cache

all: test

test:  build execute

build: $(OBJ_FILES)
	g++ -o $@ $^

execute:
	./build &> t.txt

clean:
	rm -f test *.o *~ *.bak *.txt *.xml

build_cache:
	g++ -c -DDEBUG TestMain.cpp -o TestMain.o
	#ar rvs catch.a catch.o
# DO NOT DELETE

cbs_easy.o: ../../kernel.hpp ../../baseexc.hpp ../../entity.hpp
cbs_easy.o: ../../basetype.hpp ../../tick.hpp /usr/include/limits.h
cbs_easy.o: ../../strtoken.hpp ../../abskernel.hpp ../../abstask.hpp
cbs_easy.o: ../../cpu.hpp ../../trace.hpp ../../timer.hpp ../../simul.hpp
cbs_easy.o: ../../basestat.hpp ../../debugstream.hpp ../../event.hpp
cbs_easy.o: ../../particle.hpp ../../plist.hpp ../../gevent.hpp
cbs_easy.o: ../../powermodel.hpp /usr/include/assert.h
cbs_easy.o: /usr/include/features.h /usr/include/stdc-predef.h
cbs_easy.o: ../../kernevt.hpp ../../edfsched.hpp ../../scheduler.hpp
cbs_easy.o: ../../fifosched.hpp ../../texttrace.hpp ../../rttask.hpp
cbs_easy.o: ../../regvar.hpp ../../randomvar.hpp ../../cloneable.hpp
cbs_easy.o: ../../task.hpp ../../taskevt.hpp ../../feedback.hpp
cbs_easy.o: ../../taskexc.hpp ../../cbserver.hpp ../../server.hpp
cbs_easy.o: ../../resmanager.hpp ../../capacitytimer.hpp ../../supercbs.hpp
cbs_easy.o: ../../supervisor.hpp ../../sporadicserver.hpp
