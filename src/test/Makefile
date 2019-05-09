CXXFLAGS=-Wno-deprecated -Wno-deprecated-declarations --std=c++11 -I. -g

CPP_FILES=$(wildcard ../*.cpp) $(wildcard *.cpp)
HPP_FILES=$(wildcard ../*.hpp)
OBJ_FILES=$(patsubst %.cpp,%.o,$(CPP_FILES))

SOURCES=$(CPP_FILES) $(HPP_FILES)

.PHONY: all build test clean execute build_cache

all: test

test:  build execute

build:
	g++ -o test -Wno-deprecated -I.. TestMain.o test_energy_mrt.cpp ../multi_cores_scheds.o ../sporadicserver.o ../tick.o ../grubserver.o ../rrsched.o ../load.o ../regtask.o ../trace.o ../debugstream.o ../regvar.o ../resmanager.o ../resource.o ../bwi.o ../fcfsresmanager.o ../pollingserver.o ../reginstr.o ../taskevt.o ../waitinstr.o ../task.o ../edfsched.o ../piresman.o ../entity.o ../regsched.o ../scheduler.o ../fpsched.o ../schedpoints.o ../suspend_instr.o ../server.o ../texttrace.o ../powermodel.o ../strtoken.o ../jtrace.o ../cpu.o ../kernel.o ../event.o ../genericvar.o ../rttask.o ../fifosched.o ../feedback.o ../simul.o ../supercbs.o ../traceevent.o ../exeinstr.o ../feedbacktest.o ../sparepot.o ../mrtkernel.o ../schedrta.o ../basestat.o ../json_trace.o ../tracepower.o ../randomvar.o ../interrupt.o ../instr.o ../AVRTask.o ../cbserver.o ../capacitytimer.o ../timer.o ../energyMRTKernel.o ../schedinstr.o ../kernevt.o ../threinstr.o ../rmsched.o

execute:
	./test &> t.txt

clean:
	rm -f test *.o *~ *.bak *.txt *.xml

build_cache:
	g++ -c -DDEBUG TestMain.cpp -o TestMain.o
	#ar rvs catch.a catch.o
