CXXFLAGS=-Wno-deprecated -Wno-deprecated-declarations --std=c++11 -I. -g -fPIC

CPP_FILES=$(filter-out energyMRTKernel_example.cpp,$(wildcard *.cpp))
HPP_FILES=$(filter-out energyMRTKernel_example.hpp,$(wildcard *.hpp))
OBJ_FILES=$(patsubst %.cpp,%.o,$(CPP_FILES))
LIB_FILE=librtsim.so

PROG_CPP_FILES=energyMRTKernel_example.cpp $(wildcard examples/*/*.cpp)
PROG_OBJ_FILES=$(patsubst %.cpp,%.o,$(PROG_CPP_FILES))
PROG_EXE_FILES=$(patsubst %.cpp,%,$(PROG_CPP_FILES))

SOURCES=$(CPP_FILES) $(HPP_FILES)

all: $(LIB_FILE) energyMRT_example

$(LIB_FILE): $(OBJ_FILES)
	g++ -shared -o $@ $^

examples: $(PROG_EXE_FILES)

%: %.o $(LIB_FILE)
	g++ -o $@ $< -I. -L. -lrtsim

clean:
	rm -f $(LIB_FILE) $(PROG_EXE_FILES) $(OBJ_FILES) $(PROG_OBJ_FILES) capacities.txt debug.txt *~ *.bak

dep:
	makedepend -- -I. -Y -- $(CPP_FILES) $(PROG_CPP_FILES)

# DO NOT DELETE

sporadicserver.o: sporadicserver.hpp ./server.hpp ./baseexc.hpp ./entity.hpp
sporadicserver.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
sporadicserver.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
sporadicserver.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
sporadicserver.o: ./abstask.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
sporadicserver.o: ./kernel.hpp ./kernevt.hpp ./cpu.hpp ./timer.hpp
sporadicserver.o: ./powermodel.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
sporadicserver.o: ./scheduler.hpp ./resmanager.hpp ./capacitytimer.hpp
capacitytimer.o: ./capacitytimer.hpp ./simul.hpp ./basestat.hpp
capacitytimer.o: ./basetype.hpp ./tick.hpp ./baseexc.hpp ./strtoken.hpp
capacitytimer.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp
capacitytimer.o: ./plist.hpp ./trace.hpp
tick.o: ./strtoken.hpp ./baseexc.hpp ./tick.hpp
grubserver.o: ./grubserver.hpp ./server.hpp ./baseexc.hpp ./entity.hpp
grubserver.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
grubserver.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
grubserver.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
grubserver.o: ./abstask.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
grubserver.o: ./kernel.hpp ./kernevt.hpp ./cpu.hpp ./timer.hpp
grubserver.o: ./powermodel.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
grubserver.o: ./scheduler.hpp ./resmanager.hpp ./capacitytimer.hpp
rrsched.o: ./rrsched.hpp ./baseexc.hpp ./plist.hpp ./simul.hpp ./basestat.hpp
rrsched.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./debugstream.hpp
rrsched.o: ./entity.hpp ./event.hpp ./particle.hpp ./trace.hpp ./gevent.hpp
rrsched.o: ./scheduler.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
rrsched.o: ./powermodel.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
rrsched.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp
rrsched.o: ./feedback.hpp ./taskexc.hpp
debugstream.o: ./debugstream.hpp ./basetype.hpp ./tick.hpp ./baseexc.hpp
debugstream.o: ./strtoken.hpp ./simul.hpp ./basestat.hpp ./entity.hpp
debugstream.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
timer.o: ./timer.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
timer.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
timer.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./gevent.hpp
mrtkernel.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
mrtkernel.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
mrtkernel.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./cpu.hpp
mrtkernel.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./abstask.hpp
mrtkernel.o: ./mrtkernel.hpp ./kernel.hpp ./abskernel.hpp ./kernevt.hpp
mrtkernel.o: ./resmanager.hpp ./scheduler.hpp ./task.hpp ./randomvar.hpp
mrtkernel.o: ./cloneable.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
reginstr.o: ./strtoken.hpp ./baseexc.hpp ./exeinstr.hpp ./debugstream.hpp
reginstr.o: ./basetype.hpp ./tick.hpp ./event.hpp ./simul.hpp ./basestat.hpp
reginstr.o: ./entity.hpp ./particle.hpp ./plist.hpp ./trace.hpp
reginstr.o: ./randomvar.hpp ./cloneable.hpp ./instr.hpp ./taskevt.hpp
reginstr.o: ./abstask.hpp ./threinstr.hpp ./factory.hpp ./schedinstr.hpp
reginstr.o: ./waitinstr.hpp ./suspend_instr.hpp ./gevent.hpp ./reginstr.hpp
regtask.o: ./factory.hpp ./AVRTask.hpp ./task.hpp ./entity.hpp ./baseexc.hpp
regtask.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./gevent.hpp ./event.hpp
regtask.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./particle.hpp
regtask.o: ./plist.hpp ./trace.hpp ./randomvar.hpp ./cloneable.hpp
regtask.o: ./abstask.hpp ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./cpu.hpp
regtask.o: ./timer.hpp ./powermodel.hpp ./taskevt.hpp ./feedback.hpp
regtask.o: ./taskexc.hpp ./regvar.hpp ./instr.hpp ./rttask.hpp ./reginstr.hpp
tracepower.o: tracepower.hpp ./cpu.hpp ./trace.hpp ./baseexc.hpp
tracepower.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./timer.hpp
tracepower.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./entity.hpp
tracepower.o: ./event.hpp ./particle.hpp ./plist.hpp ./gevent.hpp
tracepower.o: ./powermodel.hpp ./abstask.hpp
instr.o: ./instr.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp ./tick.hpp
instr.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
instr.o: ./debugstream.hpp ./particle.hpp ./plist.hpp ./trace.hpp
instr.o: ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp ./abstask.hpp
regvar.o: ./regvar.hpp ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp
regvar.o: ./genericvar.hpp ./factory.hpp
texttrace.o: ./texttrace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
texttrace.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
texttrace.o: ./debugstream.hpp ./entity.hpp ./particle.hpp ./plist.hpp
texttrace.o: ./trace.hpp ./rttask.hpp ./regvar.hpp ./randomvar.hpp
texttrace.o: ./cloneable.hpp ./task.hpp ./gevent.hpp ./abstask.hpp
texttrace.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./cpu.hpp ./timer.hpp
texttrace.o: ./powermodel.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
feedback.o: ./feedback.hpp ./entity.hpp ./baseexc.hpp ./basetype.hpp
feedback.o: ./tick.hpp ./strtoken.hpp ./task.hpp ./gevent.hpp ./event.hpp
feedback.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./particle.hpp
feedback.o: ./plist.hpp ./trace.hpp ./randomvar.hpp ./cloneable.hpp
feedback.o: ./abstask.hpp ./kernel.hpp ./abskernel.hpp ./kernevt.hpp
feedback.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./taskevt.hpp
feedback.o: ./taskexc.hpp
suspend_instr.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
suspend_instr.o: ./tick.hpp ./strtoken.hpp ./abskernel.hpp ./abstask.hpp
suspend_instr.o: ./kernevt.hpp ./event.hpp ./simul.hpp ./basestat.hpp
suspend_instr.o: ./debugstream.hpp ./particle.hpp ./plist.hpp ./trace.hpp
suspend_instr.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
suspend_instr.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp
suspend_instr.o: ./feedback.hpp ./taskexc.hpp ./suspend_instr.hpp ./instr.hpp
pollingserver.o: pollingserver.hpp ./server.hpp ./baseexc.hpp ./entity.hpp
pollingserver.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
pollingserver.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
pollingserver.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
pollingserver.o: ./abstask.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
pollingserver.o: ./kernel.hpp ./kernevt.hpp ./cpu.hpp ./timer.hpp
pollingserver.o: ./powermodel.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
pollingserver.o: ./scheduler.hpp ./resmanager.hpp
fcfsresmanager.o: ./abskernel.hpp ./abstask.hpp ./basetype.hpp ./tick.hpp
fcfsresmanager.o: ./baseexc.hpp ./strtoken.hpp ./fcfsresmanager.hpp
fcfsresmanager.o: ./resmanager.hpp ./entity.hpp ./scheduler.hpp ./plist.hpp
fcfsresmanager.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./event.hpp
fcfsresmanager.o: ./particle.hpp ./trace.hpp ./cpu.hpp ./timer.hpp
fcfsresmanager.o: ./gevent.hpp ./powermodel.hpp ./resource.hpp ./task.hpp
fcfsresmanager.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp ./kernevt.hpp
fcfsresmanager.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
cbserver.o: cbserver.hpp ./server.hpp ./baseexc.hpp ./entity.hpp
cbserver.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
cbserver.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
cbserver.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
cbserver.o: ./abstask.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
cbserver.o: ./kernel.hpp ./kernevt.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
cbserver.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
cbserver.o: ./resmanager.hpp ./capacitytimer.hpp
rmsched.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp ./tick.hpp
rmsched.o: ./strtoken.hpp ./abskernel.hpp ./abstask.hpp ./kernevt.hpp
rmsched.o: ./event.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
rmsched.o: ./particle.hpp ./plist.hpp ./trace.hpp ./cpu.hpp ./timer.hpp
rmsched.o: ./gevent.hpp ./powermodel.hpp ./rmsched.hpp ./fpsched.hpp
rmsched.o: ./scheduler.hpp
feedbacktest.o: ./feedbacktest.hpp ./feedback.hpp ./entity.hpp ./baseexc.hpp
feedbacktest.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./sparepot.hpp
feedbacktest.o: sporadicserver.hpp ./server.hpp ./simul.hpp ./basestat.hpp
feedbacktest.o: ./debugstream.hpp ./event.hpp ./particle.hpp ./plist.hpp
feedbacktest.o: ./trace.hpp ./gevent.hpp ./abskernel.hpp ./abstask.hpp
feedbacktest.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
feedbacktest.o: ./kernevt.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
feedbacktest.o: ./taskevt.hpp ./taskexc.hpp ./scheduler.hpp ./resmanager.hpp
feedbacktest.o: ./capacitytimer.hpp ./supervisor.hpp
task.o: ./regvar.hpp ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp
task.o: ./factory.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
task.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
task.o: ./particle.hpp ./plist.hpp ./trace.hpp ./abskernel.hpp ./abstask.hpp
task.o: ./instr.hpp ./taskevt.hpp ./task.hpp ./gevent.hpp ./kernel.hpp
task.o: ./kernevt.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp ./feedback.hpp
task.o: ./taskexc.hpp
waitinstr.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
waitinstr.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
waitinstr.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./kernel.hpp
waitinstr.o: ./abskernel.hpp ./abstask.hpp ./kernevt.hpp ./cpu.hpp
waitinstr.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./task.hpp
waitinstr.o: ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp ./feedback.hpp
waitinstr.o: ./taskexc.hpp ./waitinstr.hpp ./factory.hpp ./instr.hpp
strtoken.o: ./strtoken.hpp ./baseexc.hpp
simul.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
simul.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./event.hpp
simul.o: ./particle.hpp ./plist.hpp ./trace.hpp
piresman.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
piresman.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
piresman.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
piresman.o: ./abskernel.hpp ./abstask.hpp ./piresman.hpp ./resmanager.hpp
piresman.o: ./scheduler.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
piresman.o: ./powermodel.hpp ./resource.hpp
entity.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
entity.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./event.hpp
entity.o: ./particle.hpp ./plist.hpp ./trace.hpp
bwi.o: bwi.hpp ./resmanager.hpp ./entity.hpp ./baseexc.hpp ./basetype.hpp
bwi.o: ./tick.hpp ./strtoken.hpp ./scheduler.hpp ./plist.hpp ./simul.hpp
bwi.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
bwi.o: ./trace.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
bwi.o: ./powermodel.hpp ./server.hpp ./abskernel.hpp ./task.hpp
bwi.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp ./kernevt.hpp
bwi.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./resource.hpp
regsched.o: ./strtoken.hpp ./baseexc.hpp ./factory.hpp ./fifosched.hpp
regsched.o: ./scheduler.hpp ./plist.hpp ./simul.hpp ./basestat.hpp
regsched.o: ./basetype.hpp ./tick.hpp ./debugstream.hpp ./entity.hpp
regsched.o: ./event.hpp ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp
regsched.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./fpsched.hpp
regsched.o: ./edfsched.hpp ./rrsched.hpp
AVRTask.o: ./AVRTask.hpp ./task.hpp ./entity.hpp ./baseexc.hpp ./basetype.hpp
AVRTask.o: ./tick.hpp ./strtoken.hpp ./gevent.hpp ./event.hpp ./simul.hpp
AVRTask.o: ./basestat.hpp ./debugstream.hpp ./particle.hpp ./plist.hpp
AVRTask.o: ./trace.hpp ./randomvar.hpp ./cloneable.hpp ./abstask.hpp
AVRTask.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./cpu.hpp ./timer.hpp
AVRTask.o: ./powermodel.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
AVRTask.o: ./regvar.hpp ./factory.hpp ./instr.hpp
basestat.o: ./baseexc.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
basestat.o: ./strtoken.hpp ./simul.hpp ./debugstream.hpp ./entity.hpp
basestat.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
load.o: ./exeinstr.hpp ./debugstream.hpp ./basetype.hpp ./tick.hpp
load.o: ./baseexc.hpp ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
load.o: ./entity.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./randomvar.hpp
load.o: ./cloneable.hpp ./instr.hpp ./taskevt.hpp ./abstask.hpp ./load.hpp
load.o: ./task.hpp ./gevent.hpp ./kernel.hpp ./abskernel.hpp ./kernevt.hpp
load.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./feedback.hpp ./taskexc.hpp
fpsched.o: ./fpsched.hpp ./scheduler.hpp ./baseexc.hpp ./plist.hpp
fpsched.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
fpsched.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
fpsched.o: ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
fpsched.o: ./gevent.hpp ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
fpsched.o: ./kernevt.hpp
schedpoints.o: ./schedpoints.hpp ./supervisor.hpp ./server.hpp ./baseexc.hpp
schedpoints.o: ./entity.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
schedpoints.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./event.hpp
schedpoints.o: ./particle.hpp ./plist.hpp ./trace.hpp ./gevent.hpp
schedpoints.o: ./abskernel.hpp ./abstask.hpp ./task.hpp ./randomvar.hpp
schedpoints.o: ./cloneable.hpp ./kernel.hpp ./kernevt.hpp ./cpu.hpp
schedpoints.o: ./timer.hpp ./powermodel.hpp ./taskevt.hpp ./feedback.hpp
schedpoints.o: ./taskexc.hpp ./scheduler.hpp ./resmanager.hpp
schedpoints.o: sporadicserver.hpp ./capacitytimer.hpp
server.o: ./factory.hpp ./server.hpp ./baseexc.hpp ./entity.hpp
server.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp ./basestat.hpp
server.o: ./debugstream.hpp ./event.hpp ./particle.hpp ./plist.hpp
server.o: ./trace.hpp ./gevent.hpp ./abskernel.hpp ./abstask.hpp ./task.hpp
server.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp ./kernevt.hpp
server.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./taskevt.hpp ./feedback.hpp
server.o: ./taskexc.hpp ./scheduler.hpp ./resmanager.hpp
energyMRTKernel.o: energyMRTKernel.hpp ./mrtkernel.hpp ./kernel.hpp
energyMRTKernel.o: ./baseexc.hpp ./entity.hpp ./basetype.hpp ./tick.hpp
energyMRTKernel.o: ./strtoken.hpp ./abskernel.hpp ./abstask.hpp ./kernevt.hpp
energyMRTKernel.o: ./event.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
energyMRTKernel.o: ./particle.hpp ./plist.hpp ./trace.hpp ./cpu.hpp
energyMRTKernel.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./task.hpp
energyMRTKernel.o: ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp
energyMRTKernel.o: ./feedback.hpp ./taskexc.hpp ./scheduler.hpp ./rttask.hpp
energyMRTKernel.o: ./regvar.hpp ./exeinstr.hpp ./instr.hpp
kernel.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp ./baseexc.hpp
kernel.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
kernel.o: ./particle.hpp ./plist.hpp ./trace.hpp ./cpu.hpp ./timer.hpp
kernel.o: ./gevent.hpp ./powermodel.hpp ./abstask.hpp ./kernel.hpp
kernel.o: ./abskernel.hpp ./kernevt.hpp ./resmanager.hpp ./scheduler.hpp
kernel.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp
kernel.o: ./feedback.hpp ./taskexc.hpp ./reginstr.hpp
genericvar.o: ./genericvar.hpp ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp
genericvar.o: ./strtoken.hpp
rttask.o: ./rttask.hpp ./regvar.hpp ./randomvar.hpp ./baseexc.hpp
rttask.o: ./cloneable.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
rttask.o: ./tick.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
rttask.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./task.hpp
rttask.o: ./gevent.hpp ./abstask.hpp ./kernel.hpp ./abskernel.hpp
rttask.o: ./kernevt.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp ./taskevt.hpp
rttask.o: ./feedback.hpp ./taskexc.hpp
fifosched.o: ./fifosched.hpp ./scheduler.hpp ./baseexc.hpp ./plist.hpp
fifosched.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
fifosched.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
fifosched.o: ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
fifosched.o: ./gevent.hpp ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
fifosched.o: ./kernevt.hpp ./factory.hpp
supercbs.o: ./supercbs.hpp ./supervisor.hpp ./server.hpp ./baseexc.hpp
supercbs.o: ./entity.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
supercbs.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
supercbs.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
supercbs.o: ./abstask.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
supercbs.o: ./kernel.hpp ./kernevt.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
supercbs.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
supercbs.o: ./resmanager.hpp sporadicserver.hpp ./capacitytimer.hpp
supercbs.o: cbserver.hpp
exeinstr.o: ./factory.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
exeinstr.o: ./tick.hpp ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
exeinstr.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
exeinstr.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp ./abstask.hpp
exeinstr.o: ./exeinstr.hpp ./randomvar.hpp ./cloneable.hpp ./instr.hpp
exeinstr.o: ./taskevt.hpp ./task.hpp ./kernel.hpp ./abskernel.hpp
exeinstr.o: ./kernevt.hpp ./feedback.hpp ./taskexc.hpp
resource.o: ./abstask.hpp ./basetype.hpp ./tick.hpp ./baseexc.hpp
resource.o: ./strtoken.hpp ./resource.hpp ./simul.hpp ./basestat.hpp
resource.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp
resource.o: ./plist.hpp ./trace.hpp
event.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
event.o: ./event.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
event.o: ./particle.hpp ./plist.hpp ./trace.hpp
schedrta.o: ./schedrta.hpp ./supervisor.hpp ./server.hpp ./baseexc.hpp
schedrta.o: ./entity.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
schedrta.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
schedrta.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
schedrta.o: ./abstask.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
schedrta.o: ./kernel.hpp ./kernevt.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
schedrta.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
schedrta.o: ./resmanager.hpp sporadicserver.hpp ./capacitytimer.hpp
resmanager.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
resmanager.o: ./strtoken.hpp ./abskernel.hpp ./abstask.hpp ./resmanager.hpp
resmanager.o: ./scheduler.hpp ./plist.hpp ./simul.hpp ./basestat.hpp
resmanager.o: ./debugstream.hpp ./event.hpp ./particle.hpp ./trace.hpp
resmanager.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
resmanager.o: ./resource.hpp
scheduler.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
scheduler.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
scheduler.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
scheduler.o: ./scheduler.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
scheduler.o: ./powermodel.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
scheduler.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp
scheduler.o: ./feedback.hpp ./taskexc.hpp
trace.o: ./event.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
trace.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
trace.o: ./particle.hpp ./plist.hpp ./trace.hpp
randomvar.o: ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp ./simul.hpp
randomvar.o: ./basestat.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
randomvar.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp
randomvar.o: ./plist.hpp ./trace.hpp ./factory.hpp ./regvar.hpp
interrupt.o: ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp ./simul.hpp
interrupt.o: ./basestat.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
interrupt.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp
interrupt.o: ./plist.hpp ./trace.hpp ./interrupt.hpp ./gevent.hpp ./task.hpp
interrupt.o: ./abstask.hpp ./kernel.hpp ./abskernel.hpp ./kernevt.hpp
interrupt.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./taskevt.hpp
interrupt.o: ./feedback.hpp ./taskexc.hpp
sparepot.o: ./sparepot.hpp sporadicserver.hpp ./server.hpp ./baseexc.hpp
sparepot.o: ./entity.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
sparepot.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
sparepot.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
sparepot.o: ./abstask.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
sparepot.o: ./kernel.hpp ./kernevt.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
sparepot.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
sparepot.o: ./resmanager.hpp ./capacitytimer.hpp ./supervisor.hpp
jtrace.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp ./baseexc.hpp
jtrace.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
jtrace.o: ./particle.hpp ./plist.hpp ./trace.hpp ./server.hpp ./gevent.hpp
jtrace.o: ./abskernel.hpp ./abstask.hpp ./task.hpp ./randomvar.hpp
jtrace.o: ./cloneable.hpp ./kernel.hpp ./kernevt.hpp ./cpu.hpp ./timer.hpp
jtrace.o: ./powermodel.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
jtrace.o: ./scheduler.hpp ./resmanager.hpp ./jtrace.hpp ./traceevent.hpp
jtrace.o: ./waitinstr.hpp ./factory.hpp ./instr.hpp
edfsched.o: ./edfsched.hpp ./scheduler.hpp ./baseexc.hpp ./plist.hpp
edfsched.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
edfsched.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
edfsched.o: ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
edfsched.o: ./gevent.hpp ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
edfsched.o: ./kernevt.hpp
traceevent.o: ./jtrace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
traceevent.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
traceevent.o: ./debugstream.hpp ./entity.hpp ./particle.hpp ./plist.hpp
traceevent.o: ./trace.hpp ./traceevent.hpp
taskevt.o: ./task.hpp ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
taskevt.o: ./strtoken.hpp ./gevent.hpp ./event.hpp ./simul.hpp ./basestat.hpp
taskevt.o: ./debugstream.hpp ./particle.hpp ./plist.hpp ./trace.hpp
taskevt.o: ./randomvar.hpp ./cloneable.hpp ./abstask.hpp ./kernel.hpp
taskevt.o: ./abskernel.hpp ./kernevt.hpp ./cpu.hpp ./timer.hpp
taskevt.o: ./powermodel.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
powermodel.o: ./powermodel.hpp ./cpu.hpp ./trace.hpp ./baseexc.hpp
powermodel.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./timer.hpp
powermodel.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./entity.hpp
powermodel.o: ./event.hpp ./particle.hpp ./plist.hpp ./gevent.hpp
powermodel.o: ./abstask.hpp
cpu.o: ./cpu.hpp ./trace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
cpu.o: ./strtoken.hpp ./timer.hpp ./simul.hpp ./basestat.hpp
cpu.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
cpu.o: ./gevent.hpp ./powermodel.hpp ./abstask.hpp
kernevt.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp ./tick.hpp
kernevt.o: ./strtoken.hpp ./abskernel.hpp ./abstask.hpp ./kernevt.hpp
kernevt.o: ./event.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
kernevt.o: ./particle.hpp ./plist.hpp ./trace.hpp ./cpu.hpp ./timer.hpp
kernevt.o: ./gevent.hpp ./powermodel.hpp
json_trace.o: ./json_trace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
json_trace.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
json_trace.o: ./debugstream.hpp ./entity.hpp ./particle.hpp ./plist.hpp
json_trace.o: ./trace.hpp ./rttask.hpp ./regvar.hpp ./randomvar.hpp
json_trace.o: ./cloneable.hpp ./task.hpp ./gevent.hpp ./abstask.hpp
json_trace.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./cpu.hpp
json_trace.o: ./timer.hpp ./powermodel.hpp ./taskevt.hpp ./feedback.hpp
json_trace.o: ./taskexc.hpp
schedinstr.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
schedinstr.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
schedinstr.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./kernel.hpp
schedinstr.o: ./abskernel.hpp ./abstask.hpp ./kernevt.hpp ./cpu.hpp
schedinstr.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./task.hpp
schedinstr.o: ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp ./feedback.hpp
schedinstr.o: ./taskexc.hpp ./schedinstr.hpp ./factory.hpp ./instr.hpp
threinstr.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
threinstr.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
threinstr.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./kernel.hpp
threinstr.o: ./abskernel.hpp ./abstask.hpp ./kernevt.hpp ./cpu.hpp
threinstr.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./task.hpp
threinstr.o: ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp ./feedback.hpp
threinstr.o: ./taskexc.hpp ./threinstr.hpp ./factory.hpp ./instr.hpp
energyMRTKernel_example.o: ./mrtkernel.hpp ./kernel.hpp ./baseexc.hpp
energyMRTKernel_example.o: ./entity.hpp ./basetype.hpp ./tick.hpp
energyMRTKernel_example.o: ./strtoken.hpp ./abskernel.hpp ./abstask.hpp
energyMRTKernel_example.o: ./kernevt.hpp ./event.hpp ./simul.hpp
energyMRTKernel_example.o: ./basestat.hpp ./debugstream.hpp ./particle.hpp
energyMRTKernel_example.o: ./plist.hpp ./trace.hpp ./cpu.hpp ./timer.hpp
energyMRTKernel_example.o: ./gevent.hpp ./powermodel.hpp ./edfsched.hpp
energyMRTKernel_example.o: ./scheduler.hpp ./jtrace.hpp ./traceevent.hpp
energyMRTKernel_example.o: ./texttrace.hpp ./rttask.hpp ./regvar.hpp
energyMRTKernel_example.o: ./randomvar.hpp ./cloneable.hpp ./task.hpp
energyMRTKernel_example.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
energyMRTKernel_example.o: ./json_trace.hpp tracepower.hpp ./instr.hpp
energyMRTKernel_example.o: energyMRTKernel.hpp
examples/energy_my_example/energy_my_example.o: ./mrtkernel.hpp ./kernel.hpp
examples/energy_my_example/energy_my_example.o: ./baseexc.hpp ./entity.hpp
examples/energy_my_example/energy_my_example.o: ./basetype.hpp ./tick.hpp
examples/energy_my_example/energy_my_example.o: ./strtoken.hpp
examples/energy_my_example/energy_my_example.o: ./abskernel.hpp ./abstask.hpp
examples/energy_my_example/energy_my_example.o: ./kernevt.hpp ./event.hpp
examples/energy_my_example/energy_my_example.o: ./simul.hpp ./basestat.hpp
examples/energy_my_example/energy_my_example.o: ./debugstream.hpp
examples/energy_my_example/energy_my_example.o: ./particle.hpp ./plist.hpp
examples/energy_my_example/energy_my_example.o: ./trace.hpp ./cpu.hpp
examples/energy_my_example/energy_my_example.o: ./timer.hpp ./gevent.hpp
examples/energy_my_example/energy_my_example.o: ./powermodel.hpp
examples/energy_my_example/energy_my_example.o: ./edfsched.hpp
examples/energy_my_example/energy_my_example.o: ./scheduler.hpp ./jtrace.hpp
examples/energy_my_example/energy_my_example.o: ./traceevent.hpp
examples/energy_my_example/energy_my_example.o: ./texttrace.hpp ./rttask.hpp
examples/energy_my_example/energy_my_example.o: ./regvar.hpp ./randomvar.hpp
examples/energy_my_example/energy_my_example.o: ./cloneable.hpp ./task.hpp
examples/energy_my_example/energy_my_example.o: ./taskevt.hpp ./feedback.hpp
examples/energy_my_example/energy_my_example.o: ./taskexc.hpp
examples/energy_my_example/energy_my_example.o: ./json_trace.hpp
examples/energy_my_example/energy_my_example.o: tracepower.hpp ./instr.hpp
examples/rm/rm.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
examples/rm/rm.o: ./tick.hpp ./strtoken.hpp ./abskernel.hpp ./abstask.hpp
examples/rm/rm.o: ./kernevt.hpp ./event.hpp ./simul.hpp ./basestat.hpp
examples/rm/rm.o: ./debugstream.hpp ./particle.hpp ./plist.hpp ./trace.hpp
examples/rm/rm.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
examples/rm/rm.o: ./fpsched.hpp ./scheduler.hpp ./jtrace.hpp ./traceevent.hpp
examples/rm/rm.o: ./texttrace.hpp ./rttask.hpp ./regvar.hpp ./randomvar.hpp
examples/rm/rm.o: ./cloneable.hpp ./task.hpp ./taskevt.hpp ./feedback.hpp
examples/rm/rm.o: ./taskexc.hpp
examples/prob/edf.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
examples/prob/edf.o: ./tick.hpp ./strtoken.hpp ./abskernel.hpp ./abstask.hpp
examples/prob/edf.o: ./kernevt.hpp ./event.hpp ./simul.hpp ./basestat.hpp
examples/prob/edf.o: ./debugstream.hpp ./particle.hpp ./plist.hpp ./trace.hpp
examples/prob/edf.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
examples/prob/edf.o: ./edfsched.hpp ./scheduler.hpp ./jtrace.hpp
examples/prob/edf.o: ./traceevent.hpp ./texttrace.hpp ./rttask.hpp
examples/prob/edf.o: ./regvar.hpp ./randomvar.hpp ./cloneable.hpp ./task.hpp
examples/prob/edf.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./instr.hpp
examples/prob/edf.o: ./taskstat.hpp
examples/cbserver/schedp.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp
examples/cbserver/schedp.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp
examples/cbserver/schedp.o: ./abskernel.hpp ./abstask.hpp ./kernevt.hpp
examples/cbserver/schedp.o: ./event.hpp ./simul.hpp ./basestat.hpp
examples/cbserver/schedp.o: ./debugstream.hpp ./particle.hpp ./plist.hpp
examples/cbserver/schedp.o: ./trace.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
examples/cbserver/schedp.o: ./powermodel.hpp ./rmsched.hpp ./fpsched.hpp
examples/cbserver/schedp.o: ./scheduler.hpp ./rrsched.hpp ./jtrace.hpp
examples/cbserver/schedp.o: ./traceevent.hpp ./texttrace.hpp ./rttask.hpp
examples/cbserver/schedp.o: ./regvar.hpp ./randomvar.hpp ./cloneable.hpp
examples/cbserver/schedp.o: ./task.hpp ./taskevt.hpp ./feedback.hpp
examples/cbserver/schedp.o: ./taskexc.hpp sporadicserver.hpp ./server.hpp
examples/cbserver/schedp.o: ./resmanager.hpp ./capacitytimer.hpp
examples/cbserver/schedp.o: ./feedbacktest.hpp ./sparepot.hpp
examples/cbserver/schedp.o: ./supervisor.hpp ./schedpoints.hpp ./taskstat.hpp
examples/cbserver/pserver.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp
examples/cbserver/pserver.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp
examples/cbserver/pserver.o: ./abskernel.hpp ./abstask.hpp ./kernevt.hpp
examples/cbserver/pserver.o: ./event.hpp ./simul.hpp ./basestat.hpp
examples/cbserver/pserver.o: ./debugstream.hpp ./particle.hpp ./plist.hpp
examples/cbserver/pserver.o: ./trace.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
examples/cbserver/pserver.o: ./powermodel.hpp ./rmsched.hpp ./fpsched.hpp
examples/cbserver/pserver.o: ./scheduler.hpp ./rrsched.hpp ./jtrace.hpp
examples/cbserver/pserver.o: ./traceevent.hpp ./texttrace.hpp ./rttask.hpp
examples/cbserver/pserver.o: ./regvar.hpp ./randomvar.hpp ./cloneable.hpp
examples/cbserver/pserver.o: ./task.hpp ./taskevt.hpp ./feedback.hpp
examples/cbserver/pserver.o: ./taskexc.hpp pollingserver.hpp ./server.hpp
examples/cbserver/pserver.o: ./resmanager.hpp
examples/cbserver/pservermulti.o: ./mrtkernel.hpp ./kernel.hpp ./baseexc.hpp
examples/cbserver/pservermulti.o: ./entity.hpp ./basetype.hpp ./tick.hpp
examples/cbserver/pservermulti.o: ./strtoken.hpp ./abskernel.hpp
examples/cbserver/pservermulti.o: ./abstask.hpp ./kernevt.hpp ./event.hpp
examples/cbserver/pservermulti.o: ./simul.hpp ./basestat.hpp
examples/cbserver/pservermulti.o: ./debugstream.hpp ./particle.hpp
examples/cbserver/pservermulti.o: ./plist.hpp ./trace.hpp ./cpu.hpp
examples/cbserver/pservermulti.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp
examples/cbserver/pservermulti.o: ./rmsched.hpp ./fpsched.hpp ./scheduler.hpp
examples/cbserver/pservermulti.o: ./rrsched.hpp ./jtrace.hpp ./traceevent.hpp
examples/cbserver/pservermulti.o: ./texttrace.hpp ./rttask.hpp ./regvar.hpp
examples/cbserver/pservermulti.o: ./randomvar.hpp ./cloneable.hpp ./task.hpp
examples/cbserver/pservermulti.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
examples/cbserver/pservermulti.o: pollingserver.hpp ./server.hpp
examples/cbserver/pservermulti.o: ./resmanager.hpp
examples/cbserver/cbs.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp
examples/cbserver/cbs.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp
examples/cbserver/cbs.o: ./abskernel.hpp ./abstask.hpp ./kernevt.hpp
examples/cbserver/cbs.o: ./event.hpp ./simul.hpp ./basestat.hpp
examples/cbserver/cbs.o: ./debugstream.hpp ./particle.hpp ./plist.hpp
examples/cbserver/cbs.o: ./trace.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
examples/cbserver/cbs.o: ./powermodel.hpp ./edfsched.hpp ./scheduler.hpp
examples/cbserver/cbs.o: ./fifosched.hpp ./texttrace.hpp ./rttask.hpp
examples/cbserver/cbs.o: ./regvar.hpp ./randomvar.hpp ./cloneable.hpp
examples/cbserver/cbs.o: ./task.hpp ./taskevt.hpp ./feedback.hpp
examples/cbserver/cbs.o: ./taskexc.hpp cbserver.hpp ./server.hpp
examples/cbserver/cbs.o: ./resmanager.hpp ./capacitytimer.hpp ./supercbs.hpp
examples/cbserver/cbs.o: ./supervisor.hpp sporadicserver.hpp
examples/cbserver/feedback.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp
examples/cbserver/feedback.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp
examples/cbserver/feedback.o: ./abskernel.hpp ./abstask.hpp ./kernevt.hpp
examples/cbserver/feedback.o: ./event.hpp ./simul.hpp ./basestat.hpp
examples/cbserver/feedback.o: ./debugstream.hpp ./particle.hpp ./plist.hpp
examples/cbserver/feedback.o: ./trace.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
examples/cbserver/feedback.o: ./powermodel.hpp ./edfsched.hpp ./scheduler.hpp
examples/cbserver/feedback.o: ./rrsched.hpp ./jtrace.hpp ./traceevent.hpp
examples/cbserver/feedback.o: ./texttrace.hpp ./rttask.hpp ./regvar.hpp
examples/cbserver/feedback.o: ./randomvar.hpp ./cloneable.hpp ./task.hpp
examples/cbserver/feedback.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
examples/cbserver/feedback.o: ./supercbs.hpp ./supervisor.hpp ./server.hpp
examples/cbserver/feedback.o: ./resmanager.hpp sporadicserver.hpp
examples/cbserver/feedback.o: ./capacitytimer.hpp cbserver.hpp
examples/cbserver/feedback.o: ./feedbacktest.hpp ./sparepot.hpp
examples/cbserver/feedback.o: ./taskstat.hpp
examples/cbserver/cbsfeedback.o: cbserver.hpp ./server.hpp ./baseexc.hpp
examples/cbserver/cbsfeedback.o: ./entity.hpp ./basetype.hpp ./tick.hpp
examples/cbserver/cbsfeedback.o: ./strtoken.hpp ./simul.hpp ./basestat.hpp
examples/cbserver/cbsfeedback.o: ./debugstream.hpp ./event.hpp ./particle.hpp
examples/cbserver/cbsfeedback.o: ./plist.hpp ./trace.hpp ./gevent.hpp
examples/cbserver/cbsfeedback.o: ./abskernel.hpp ./abstask.hpp ./task.hpp
examples/cbserver/cbsfeedback.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
examples/cbserver/cbsfeedback.o: ./kernevt.hpp ./cpu.hpp ./timer.hpp
examples/cbserver/cbsfeedback.o: ./powermodel.hpp ./taskevt.hpp
examples/cbserver/cbsfeedback.o: ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
examples/cbserver/cbsfeedback.o: ./resmanager.hpp ./capacitytimer.hpp
examples/cbserver/cbsfeedback.o: ./rmsched.hpp ./fpsched.hpp ./edfsched.hpp
examples/cbserver/cbsfeedback.o: ./rrsched.hpp ./jtrace.hpp ./traceevent.hpp
examples/cbserver/cbsfeedback.o: ./texttrace.hpp ./rttask.hpp ./regvar.hpp
examples/cbserver/cbsfeedback.o: sporadicserver.hpp ./feedbacktest.hpp
examples/cbserver/cbsfeedback.o: ./sparepot.hpp ./supervisor.hpp
examples/cbserver/cbsfeedback.o: ./schedpoints.hpp ./taskstat.hpp
examples/cbserver/cbsfeedback.o: ./supercbs.hpp
examples/sporadic/spsl.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp
examples/sporadic/spsl.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp
examples/sporadic/spsl.o: ./abskernel.hpp ./abstask.hpp ./kernevt.hpp
examples/sporadic/spsl.o: ./event.hpp ./simul.hpp ./basestat.hpp
examples/sporadic/spsl.o: ./debugstream.hpp ./particle.hpp ./plist.hpp
examples/sporadic/spsl.o: ./trace.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
examples/sporadic/spsl.o: ./powermodel.hpp ./rmsched.hpp ./fpsched.hpp
examples/sporadic/spsl.o: ./scheduler.hpp ./texttrace.hpp ./rttask.hpp
examples/sporadic/spsl.o: ./regvar.hpp ./randomvar.hpp ./cloneable.hpp
examples/sporadic/spsl.o: ./task.hpp ./taskevt.hpp ./feedback.hpp
examples/sporadic/spsl.o: ./taskexc.hpp sporadicserver.hpp ./server.hpp
examples/sporadic/spsl.o: ./resmanager.hpp ./capacitytimer.hpp
examples/sporadic/sprunt_example1.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp
examples/sporadic/sprunt_example1.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp
examples/sporadic/sprunt_example1.o: ./abskernel.hpp ./abstask.hpp
examples/sporadic/sprunt_example1.o: ./kernevt.hpp ./event.hpp ./simul.hpp
examples/sporadic/sprunt_example1.o: ./basestat.hpp ./debugstream.hpp
examples/sporadic/sprunt_example1.o: ./particle.hpp ./plist.hpp ./trace.hpp
examples/sporadic/sprunt_example1.o: ./cpu.hpp ./timer.hpp ./gevent.hpp
examples/sporadic/sprunt_example1.o: ./powermodel.hpp ./rmsched.hpp
examples/sporadic/sprunt_example1.o: ./fpsched.hpp ./scheduler.hpp
examples/sporadic/sprunt_example1.o: ./rrsched.hpp ./texttrace.hpp
examples/sporadic/sprunt_example1.o: ./rttask.hpp ./regvar.hpp
examples/sporadic/sprunt_example1.o: ./randomvar.hpp ./cloneable.hpp
examples/sporadic/sprunt_example1.o: ./task.hpp ./taskevt.hpp ./feedback.hpp
examples/sporadic/sprunt_example1.o: ./taskexc.hpp sporadicserver.hpp
examples/sporadic/sprunt_example1.o: ./server.hpp ./resmanager.hpp
examples/sporadic/sprunt_example1.o: ./capacitytimer.hpp
examples/energy/energy.o: ./mrtkernel.hpp ./kernel.hpp ./baseexc.hpp
examples/energy/energy.o: ./entity.hpp ./basetype.hpp ./tick.hpp
examples/energy/energy.o: ./strtoken.hpp ./abskernel.hpp ./abstask.hpp
examples/energy/energy.o: ./kernevt.hpp ./event.hpp ./simul.hpp
examples/energy/energy.o: ./basestat.hpp ./debugstream.hpp ./particle.hpp
examples/energy/energy.o: ./plist.hpp ./trace.hpp ./cpu.hpp ./timer.hpp
examples/energy/energy.o: ./gevent.hpp ./powermodel.hpp ./edfsched.hpp
examples/energy/energy.o: ./scheduler.hpp ./jtrace.hpp ./traceevent.hpp
examples/energy/energy.o: ./texttrace.hpp ./rttask.hpp ./regvar.hpp
examples/energy/energy.o: ./randomvar.hpp ./cloneable.hpp ./task.hpp
examples/energy/energy.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
examples/energy/energy.o: ./json_trace.hpp tracepower.hpp ./instr.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./mrtkernel.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./kernel.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./baseexc.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./entity.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./basetype.hpp ./tick.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./strtoken.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./abskernel.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./abstask.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./kernevt.hpp ./event.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./simul.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./basestat.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./debugstream.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./particle.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./plist.hpp ./trace.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./cpu.hpp ./timer.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./gevent.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./powermodel.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./edfsched.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./scheduler.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./jtrace.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./traceevent.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./texttrace.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./rttask.hpp ./regvar.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./randomvar.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./cloneable.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./task.hpp ./taskevt.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./feedback.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./taskexc.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./json_trace.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: tracepower.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: ./instr.hpp
examples/energyMRTKernel/energyMRTKernel_example.o: energyMRTKernel.hpp
examples/edf/edf.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
examples/edf/edf.o: ./tick.hpp ./strtoken.hpp ./abskernel.hpp ./abstask.hpp
examples/edf/edf.o: ./kernevt.hpp ./event.hpp ./simul.hpp ./basestat.hpp
examples/edf/edf.o: ./debugstream.hpp ./particle.hpp ./plist.hpp ./trace.hpp
examples/edf/edf.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
examples/edf/edf.o: ./edfsched.hpp ./scheduler.hpp ./jtrace.hpp
examples/edf/edf.o: ./traceevent.hpp ./texttrace.hpp ./rttask.hpp
examples/edf/edf.o: ./regvar.hpp ./randomvar.hpp ./cloneable.hpp ./task.hpp
examples/edf/edf.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
examples/edf/edf.o: ./json_trace.hpp ./instr.hpp
examples/grub/grub.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
examples/grub/grub.o: ./tick.hpp ./strtoken.hpp ./abskernel.hpp ./abstask.hpp
examples/grub/grub.o: ./kernevt.hpp ./event.hpp ./simul.hpp ./basestat.hpp
examples/grub/grub.o: ./debugstream.hpp ./particle.hpp ./plist.hpp
examples/grub/grub.o: ./trace.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
examples/grub/grub.o: ./powermodel.hpp ./edfsched.hpp ./scheduler.hpp
examples/grub/grub.o: ./fifosched.hpp ./jtrace.hpp ./traceevent.hpp
examples/grub/grub.o: ./texttrace.hpp ./rttask.hpp ./regvar.hpp
examples/grub/grub.o: ./randomvar.hpp ./cloneable.hpp ./task.hpp
examples/grub/grub.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
examples/grub/grub.o: ./grubserver.hpp ./server.hpp ./resmanager.hpp
examples/grub/grub.o: ./capacitytimer.hpp
examples/grub/gruball.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp
examples/grub/gruball.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp
examples/grub/gruball.o: ./abskernel.hpp ./abstask.hpp ./kernevt.hpp
examples/grub/gruball.o: ./event.hpp ./simul.hpp ./basestat.hpp
examples/grub/gruball.o: ./debugstream.hpp ./particle.hpp ./plist.hpp
examples/grub/gruball.o: ./trace.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
examples/grub/gruball.o: ./powermodel.hpp ./edfsched.hpp ./scheduler.hpp
examples/grub/gruball.o: ./fifosched.hpp ./jtrace.hpp ./traceevent.hpp
examples/grub/gruball.o: ./texttrace.hpp ./rttask.hpp ./regvar.hpp
examples/grub/gruball.o: ./randomvar.hpp ./cloneable.hpp ./task.hpp
examples/grub/gruball.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
examples/grub/gruball.o: ./grubserver.hpp ./server.hpp ./resmanager.hpp
examples/grub/gruball.o: ./capacitytimer.hpp ./load.hpp ./taskstat.hpp
examples/grub/gruball.o: examples/grub/sim_structs.hpp
