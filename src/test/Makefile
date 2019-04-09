CXXFLAGS=-Wno-deprecated -Wno-deprecated-declarations --std=c++11 -I. -g

CPP_FILES=$(wildcard ../*.cpp) $(wildcard *.cpp)
HPP_FILES=$(wildcard ../*.hpp)
OBJ_FILES=$(patsubst %.cpp,%.o,$(CPP_FILES))

SOURCES=$(CPP_FILES) $(HPP_FILES)

.PHONY: all build test clean execute open

all: test

test: clean build execute open

build:
	g++ -Wno-deprecated -Wno-deprecated-declarations  --std=c++11 -I.. -I./CUnit/Headers -lcunit -g   -c -o test.o test_energy_mrt.cpp
	g++ -o test test.o ../sporadicserver.o ../tick.o ../grubserver.o ../rrsched.o ../load.o ../regtask.o ../trace.o ../debugstream.o ../regvar.o ../resmanager.o ../resource.o ../bwi.o ../fcfsresmanager.o ../pollingserver.o ../reginstr.o ../taskevt.o ../waitinstr.o ../task.o ../edfsched.o ../piresman.o ../entity.o ../regsched.o ../scheduler.o ../fpsched.o ../schedpoints.o ../suspend_instr.o ../server.o ../texttrace.o ../powermodel.o ../strtoken.o ../jtrace.o ../cpu.o ../kernel.o ../event.o ../genericvar.o ../rttask.o ../fifosched.o ../feedback.o ../simul.o ../supercbs.o ../traceevent.o ../exeinstr.o ../feedbacktest.o ../sparepot.o ../mrtkernel.o ../schedrta.o ../basestat.o ../json_trace.o ../tracepower.o ../randomvar.o ../interrupt.o ../instr.o ../AVRTask.o ../cbserver.o ../capacitytimer.o ../timer.o ../energyMRTKernel.o ../schedinstr.o ../kernevt.o ../threinstr.o ../rmsched.o -lcunit

execute:
	./test &> t.txt

open:
	nano results-Results.xml

clean:
	rm -f test *.o *~ *.bak *.txt *.xml
