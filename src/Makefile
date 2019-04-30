CXXFLAGS=-Wno-deprecated -Wno-deprecated-declarations --std=c++11 -I. -g

CPP_FILES=$(wildcard *.cpp)
HPP_FILES=$(wildcard *.hpp)
OBJ_FILES=$(patsubst %.cpp,%.o,$(CPP_FILES))

SOURCES=$(CPP_FILES) $(HPP_FILES)

all: example

example: $(OBJ_FILES)
	g++ -o $@ $^

clean:
	rm -f example *.o capacities.txt debug.txt *~ *.bak

dep:
	makedepend -- -I. -Y -- *.cpp

run: 
	./example > t.txt

# DO NOT DELETE

AVRTask.o: ./AVRTask.hpp ./task.hpp ./entity.hpp ./baseexc.hpp ./basetype.hpp
AVRTask.o: ./tick.hpp ./strtoken.hpp ./gevent.hpp ./event.hpp ./simul.hpp
AVRTask.o: ./basestat.hpp ./debugstream.hpp ./particle.hpp ./plist.hpp
AVRTask.o: ./trace.hpp ./randomvar.hpp ./cloneable.hpp ./abstask.hpp
AVRTask.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./kernel.hpp
AVRTask.o: ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
AVRTask.o: ./taskexc.hpp ./regvar.hpp ./factory.hpp ./instr.hpp
basestat.o: ./baseexc.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
basestat.o: ./strtoken.hpp ./simul.hpp ./debugstream.hpp ./entity.hpp
basestat.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
bwi.o: bwi.hpp resmanager.hpp ./entity.hpp ./baseexc.hpp ./basetype.hpp
bwi.o: ./tick.hpp ./strtoken.hpp ./scheduler.hpp ./plist.hpp ./simul.hpp
bwi.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
bwi.o: ./trace.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
bwi.o: ./powermodel.hpp server.hpp ./abskernel.hpp ./task.hpp ./randomvar.hpp
bwi.o: ./cloneable.hpp ./kernel.hpp ./kernevt.hpp ./taskevt.hpp
bwi.o: ./feedback.hpp ./taskexc.hpp resource.hpp
capacitytimer.o: ./capacitytimer.hpp ./simul.hpp ./basestat.hpp
capacitytimer.o: ./basetype.hpp ./tick.hpp ./baseexc.hpp ./strtoken.hpp
capacitytimer.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp
capacitytimer.o: ./plist.hpp ./trace.hpp
cbserver.o: cbserver.hpp server.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
cbserver.o: ./tick.hpp ./strtoken.hpp ./simul.hpp ./basestat.hpp
cbserver.o: ./debugstream.hpp ./event.hpp ./particle.hpp ./plist.hpp
cbserver.o: ./trace.hpp ./gevent.hpp ./abskernel.hpp ./abstask.hpp ./cpu.hpp
cbserver.o: ./timer.hpp ./powermodel.hpp ./task.hpp ./randomvar.hpp
cbserver.o: ./cloneable.hpp ./kernel.hpp ./kernevt.hpp ./taskevt.hpp
cbserver.o: ./feedback.hpp ./taskexc.hpp ./scheduler.hpp resmanager.hpp
cbserver.o: ./capacitytimer.hpp
cpu.o: ./cpu.hpp ./trace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
cpu.o: ./strtoken.hpp ./timer.hpp ./simul.hpp ./basestat.hpp
cpu.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
cpu.o: ./gevent.hpp ./powermodel.hpp ./abstask.hpp ./energyMRTKernel.hpp
cpu.o: mrtkernel.hpp ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./task.hpp
cpu.o: ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp ./feedback.hpp
cpu.o: ./taskexc.hpp rttask.hpp ./regvar.hpp multi_sched.hpp ./scheduler.hpp
debugstream.o: ./debugstream.hpp ./basetype.hpp ./tick.hpp ./baseexc.hpp
debugstream.o: ./strtoken.hpp ./simul.hpp ./basestat.hpp ./entity.hpp
debugstream.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
edfsched.o: ./edfsched.hpp ./scheduler.hpp ./baseexc.hpp ./plist.hpp
edfsched.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
edfsched.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
edfsched.o: ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
edfsched.o: ./gevent.hpp ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
edfsched.o: ./kernevt.hpp
energyMRTKernel.o: ./energyMRTKernel.hpp mrtkernel.hpp ./kernel.hpp
energyMRTKernel.o: ./baseexc.hpp ./entity.hpp ./basetype.hpp ./tick.hpp
energyMRTKernel.o: ./strtoken.hpp ./abskernel.hpp ./abstask.hpp ./cpu.hpp
energyMRTKernel.o: ./trace.hpp ./timer.hpp ./simul.hpp ./basestat.hpp
energyMRTKernel.o: ./debugstream.hpp ./event.hpp ./particle.hpp ./plist.hpp
energyMRTKernel.o: ./gevent.hpp ./powermodel.hpp ./kernevt.hpp ./task.hpp
energyMRTKernel.o: ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp
energyMRTKernel.o: ./feedback.hpp ./taskexc.hpp rttask.hpp ./regvar.hpp
energyMRTKernel.o: multi_sched.hpp ./scheduler.hpp exeinstr.hpp ./instr.hpp
energyMRTKernel_example.o: mrtkernel.hpp ./kernel.hpp ./baseexc.hpp
energyMRTKernel_example.o: ./entity.hpp ./basetype.hpp ./tick.hpp
energyMRTKernel_example.o: ./strtoken.hpp ./abskernel.hpp ./abstask.hpp
energyMRTKernel_example.o: ./cpu.hpp ./trace.hpp ./timer.hpp ./simul.hpp
energyMRTKernel_example.o: ./basestat.hpp ./debugstream.hpp ./event.hpp
energyMRTKernel_example.o: ./particle.hpp ./plist.hpp ./gevent.hpp
energyMRTKernel_example.o: ./powermodel.hpp ./kernevt.hpp ./task.hpp
energyMRTKernel_example.o: ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp
energyMRTKernel_example.o: ./feedback.hpp ./taskexc.hpp ./edfsched.hpp
energyMRTKernel_example.o: ./scheduler.hpp ./jtrace.hpp ./traceevent.hpp
energyMRTKernel_example.o: ./texttrace.hpp rttask.hpp ./regvar.hpp
energyMRTKernel_example.o: ./json_trace.hpp ./tracepower.hpp ./instr.hpp
energyMRTKernel_example.o: ./energyMRTKernel.hpp multi_sched.hpp rrsched.hpp
entity.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
entity.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./event.hpp
entity.o: ./particle.hpp ./plist.hpp ./trace.hpp
event.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
event.o: ./event.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
event.o: ./particle.hpp ./plist.hpp ./trace.hpp
exeinstr.o: ./factory.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
exeinstr.o: ./tick.hpp ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
exeinstr.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
exeinstr.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp ./abstask.hpp
exeinstr.o: exeinstr.hpp ./randomvar.hpp ./cloneable.hpp ./instr.hpp
exeinstr.o: ./taskevt.hpp ./task.hpp ./kernel.hpp ./abskernel.hpp
exeinstr.o: ./kernevt.hpp ./feedback.hpp ./taskexc.hpp
fcfsresmanager.o: ./abskernel.hpp ./abstask.hpp ./basetype.hpp ./tick.hpp
fcfsresmanager.o: ./baseexc.hpp ./strtoken.hpp ./cpu.hpp ./trace.hpp
fcfsresmanager.o: ./timer.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
fcfsresmanager.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
fcfsresmanager.o: ./gevent.hpp ./powermodel.hpp ./fcfsresmanager.hpp
fcfsresmanager.o: resmanager.hpp ./scheduler.hpp resource.hpp ./task.hpp
fcfsresmanager.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp ./kernevt.hpp
fcfsresmanager.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
feedback.o: ./feedback.hpp ./entity.hpp ./baseexc.hpp ./basetype.hpp
feedback.o: ./tick.hpp ./strtoken.hpp ./task.hpp ./gevent.hpp ./event.hpp
feedback.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./particle.hpp
feedback.o: ./plist.hpp ./trace.hpp ./randomvar.hpp ./cloneable.hpp
feedback.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp ./kernel.hpp
feedback.o: ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp ./taskexc.hpp
feedbacktest.o: ./feedbacktest.hpp ./feedback.hpp ./entity.hpp ./baseexc.hpp
feedbacktest.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./sparepot.hpp
feedbacktest.o: ./sporadicserver.hpp server.hpp ./simul.hpp ./basestat.hpp
feedbacktest.o: ./debugstream.hpp ./event.hpp ./particle.hpp ./plist.hpp
feedbacktest.o: ./trace.hpp ./gevent.hpp ./abskernel.hpp ./abstask.hpp
feedbacktest.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./task.hpp
feedbacktest.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp ./kernevt.hpp
feedbacktest.o: ./taskevt.hpp ./taskexc.hpp ./scheduler.hpp resmanager.hpp
feedbacktest.o: ./capacitytimer.hpp ./supervisor.hpp
fifosched.o: ./fifosched.hpp ./scheduler.hpp ./baseexc.hpp ./plist.hpp
fifosched.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
fifosched.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
fifosched.o: ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
fifosched.o: ./gevent.hpp ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
fifosched.o: ./kernevt.hpp ./factory.hpp
fpsched.o: ./fpsched.hpp ./scheduler.hpp ./baseexc.hpp ./plist.hpp
fpsched.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
fpsched.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
fpsched.o: ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
fpsched.o: ./gevent.hpp ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
fpsched.o: ./kernevt.hpp
genericvar.o: ./genericvar.hpp ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp
genericvar.o: ./strtoken.hpp
grubserver.o: ./grubserver.hpp server.hpp ./baseexc.hpp ./entity.hpp
grubserver.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
grubserver.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
grubserver.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
grubserver.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp ./task.hpp
grubserver.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp ./kernevt.hpp
grubserver.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
grubserver.o: resmanager.hpp ./capacitytimer.hpp
instr.o: ./instr.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp ./tick.hpp
instr.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
instr.o: ./debugstream.hpp ./particle.hpp ./plist.hpp ./trace.hpp
instr.o: ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp ./abstask.hpp
instr.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
interrupt.o: ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp ./simul.hpp
interrupt.o: ./basestat.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
interrupt.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp
interrupt.o: ./plist.hpp ./trace.hpp ./interrupt.hpp ./gevent.hpp ./task.hpp
interrupt.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
interrupt.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp
interrupt.o: ./feedback.hpp ./taskexc.hpp
json_trace.o: ./json_trace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
json_trace.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
json_trace.o: ./debugstream.hpp ./entity.hpp ./particle.hpp ./plist.hpp
json_trace.o: ./trace.hpp rttask.hpp ./regvar.hpp ./randomvar.hpp
json_trace.o: ./cloneable.hpp ./task.hpp ./gevent.hpp ./abstask.hpp ./cpu.hpp
json_trace.o: ./timer.hpp ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
json_trace.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
jtrace.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp ./baseexc.hpp
jtrace.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
jtrace.o: ./particle.hpp ./plist.hpp ./trace.hpp server.hpp ./gevent.hpp
jtrace.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
jtrace.o: ./powermodel.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
jtrace.o: ./kernel.hpp ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
jtrace.o: ./taskexc.hpp ./scheduler.hpp resmanager.hpp ./jtrace.hpp
jtrace.o: ./traceevent.hpp ./waitinstr.hpp ./factory.hpp ./instr.hpp
kernel.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp ./baseexc.hpp
kernel.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
kernel.o: ./particle.hpp ./plist.hpp ./trace.hpp ./cpu.hpp ./timer.hpp
kernel.o: ./gevent.hpp ./powermodel.hpp ./abstask.hpp ./kernel.hpp
kernel.o: ./abskernel.hpp ./kernevt.hpp resmanager.hpp ./scheduler.hpp
kernel.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp
kernel.o: ./feedback.hpp ./taskexc.hpp ./reginstr.hpp
kernevt.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp ./tick.hpp
kernevt.o: ./strtoken.hpp ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./trace.hpp
kernevt.o: ./timer.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
kernevt.o: ./event.hpp ./particle.hpp ./plist.hpp ./gevent.hpp
kernevt.o: ./powermodel.hpp ./kernevt.hpp
load.o: exeinstr.hpp ./debugstream.hpp ./basetype.hpp ./tick.hpp
load.o: ./baseexc.hpp ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
load.o: ./entity.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./randomvar.hpp
load.o: ./cloneable.hpp ./instr.hpp ./taskevt.hpp ./abstask.hpp ./cpu.hpp
load.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./load.hpp ./task.hpp
load.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./feedback.hpp
load.o: ./taskexc.hpp
mrtkernel.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
mrtkernel.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
mrtkernel.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./cpu.hpp
mrtkernel.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./abstask.hpp
mrtkernel.o: mrtkernel.hpp ./kernel.hpp ./abskernel.hpp ./kernevt.hpp
mrtkernel.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp
mrtkernel.o: ./feedback.hpp ./taskexc.hpp resmanager.hpp ./scheduler.hpp
multi_sched.o: multi_sched.hpp ./entity.hpp ./baseexc.hpp ./basetype.hpp
multi_sched.o: ./tick.hpp ./strtoken.hpp ./scheduler.hpp ./plist.hpp
multi_sched.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./event.hpp
multi_sched.o: ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
multi_sched.o: ./gevent.hpp ./powermodel.hpp ./taskevt.hpp mrtkernel.hpp
multi_sched.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./task.hpp
multi_sched.o: ./randomvar.hpp ./cloneable.hpp ./feedback.hpp ./taskexc.hpp
piresman.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
piresman.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
piresman.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
piresman.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
piresman.o: ./powermodel.hpp ./piresman.hpp resmanager.hpp ./scheduler.hpp
piresman.o: resource.hpp
pollingserver.o: pollingserver.hpp server.hpp ./baseexc.hpp ./entity.hpp
pollingserver.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
pollingserver.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
pollingserver.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
pollingserver.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
pollingserver.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
pollingserver.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
pollingserver.o: ./scheduler.hpp resmanager.hpp
powermodel.o: ./powermodel.hpp ./cpu.hpp ./trace.hpp ./baseexc.hpp
powermodel.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./timer.hpp
powermodel.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./entity.hpp
powermodel.o: ./event.hpp ./particle.hpp ./plist.hpp ./gevent.hpp
powermodel.o: ./abstask.hpp
randomvar.o: ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp ./simul.hpp
randomvar.o: ./basestat.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
randomvar.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp
randomvar.o: ./plist.hpp ./trace.hpp ./factory.hpp ./regvar.hpp
reginstr.o: ./strtoken.hpp ./baseexc.hpp exeinstr.hpp ./debugstream.hpp
reginstr.o: ./basetype.hpp ./tick.hpp ./event.hpp ./simul.hpp ./basestat.hpp
reginstr.o: ./entity.hpp ./particle.hpp ./plist.hpp ./trace.hpp
reginstr.o: ./randomvar.hpp ./cloneable.hpp ./instr.hpp ./taskevt.hpp
reginstr.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
reginstr.o: ./threinstr.hpp ./factory.hpp ./schedinstr.hpp ./waitinstr.hpp
reginstr.o: ./suspend_instr.hpp ./reginstr.hpp
regsched.o: ./strtoken.hpp ./baseexc.hpp ./factory.hpp ./fifosched.hpp
regsched.o: ./scheduler.hpp ./plist.hpp ./simul.hpp ./basestat.hpp
regsched.o: ./basetype.hpp ./tick.hpp ./debugstream.hpp ./entity.hpp
regsched.o: ./event.hpp ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp
regsched.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./fpsched.hpp
regsched.o: ./edfsched.hpp rrsched.hpp
regtask.o: ./factory.hpp ./AVRTask.hpp ./task.hpp ./entity.hpp ./baseexc.hpp
regtask.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./gevent.hpp ./event.hpp
regtask.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./particle.hpp
regtask.o: ./plist.hpp ./trace.hpp ./randomvar.hpp ./cloneable.hpp
regtask.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp ./kernel.hpp
regtask.o: ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
regtask.o: ./taskexc.hpp ./regvar.hpp ./instr.hpp rttask.hpp ./reginstr.hpp
regvar.o: ./regvar.hpp ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp
regvar.o: ./genericvar.hpp ./factory.hpp
resmanager.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
resmanager.o: ./strtoken.hpp ./abskernel.hpp ./abstask.hpp ./cpu.hpp
resmanager.o: ./trace.hpp ./timer.hpp ./simul.hpp ./basestat.hpp
resmanager.o: ./debugstream.hpp ./event.hpp ./particle.hpp ./plist.hpp
resmanager.o: ./gevent.hpp ./powermodel.hpp resmanager.hpp ./scheduler.hpp
resmanager.o: resource.hpp
resource.o: ./abstask.hpp ./basetype.hpp ./tick.hpp ./baseexc.hpp
resource.o: ./strtoken.hpp ./cpu.hpp ./trace.hpp ./timer.hpp ./simul.hpp
resource.o: ./basestat.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
resource.o: ./particle.hpp ./plist.hpp ./gevent.hpp ./powermodel.hpp
resource.o: resource.hpp
rmsched.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp ./tick.hpp
rmsched.o: ./strtoken.hpp ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./trace.hpp
rmsched.o: ./timer.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
rmsched.o: ./event.hpp ./particle.hpp ./plist.hpp ./gevent.hpp
rmsched.o: ./powermodel.hpp ./kernevt.hpp ./rmsched.hpp ./fpsched.hpp
rmsched.o: ./scheduler.hpp
rrsched.o: rrsched.hpp ./baseexc.hpp ./plist.hpp ./simul.hpp ./basestat.hpp
rrsched.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./debugstream.hpp
rrsched.o: ./entity.hpp ./event.hpp ./particle.hpp ./trace.hpp ./gevent.hpp
rrsched.o: ./scheduler.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
rrsched.o: ./powermodel.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
rrsched.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp
rrsched.o: ./feedback.hpp ./taskexc.hpp
rttask.o: rttask.hpp ./regvar.hpp ./randomvar.hpp ./baseexc.hpp
rttask.o: ./cloneable.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
rttask.o: ./tick.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
rttask.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./task.hpp
rttask.o: ./gevent.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
rttask.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp
rttask.o: ./feedback.hpp ./taskexc.hpp
schedinstr.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
schedinstr.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
schedinstr.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./kernel.hpp
schedinstr.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
schedinstr.o: ./gevent.hpp ./powermodel.hpp ./kernevt.hpp ./task.hpp
schedinstr.o: ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp ./feedback.hpp
schedinstr.o: ./taskexc.hpp ./schedinstr.hpp ./factory.hpp ./instr.hpp
schedpoints.o: ./schedpoints.hpp ./supervisor.hpp server.hpp ./baseexc.hpp
schedpoints.o: ./entity.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
schedpoints.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./event.hpp
schedpoints.o: ./particle.hpp ./plist.hpp ./trace.hpp ./gevent.hpp
schedpoints.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
schedpoints.o: ./powermodel.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
schedpoints.o: ./kernel.hpp ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
schedpoints.o: ./taskexc.hpp ./scheduler.hpp resmanager.hpp
schedpoints.o: ./sporadicserver.hpp ./capacitytimer.hpp
schedrta.o: ./schedrta.hpp ./supervisor.hpp server.hpp ./baseexc.hpp
schedrta.o: ./entity.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
schedrta.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
schedrta.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
schedrta.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp ./task.hpp
schedrta.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp ./kernevt.hpp
schedrta.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
schedrta.o: resmanager.hpp ./sporadicserver.hpp ./capacitytimer.hpp
scheduler.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
scheduler.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
scheduler.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
scheduler.o: ./scheduler.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
scheduler.o: ./powermodel.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
scheduler.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp
scheduler.o: ./feedback.hpp ./taskexc.hpp
server.o: ./factory.hpp server.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
server.o: ./tick.hpp ./strtoken.hpp ./simul.hpp ./basestat.hpp
server.o: ./debugstream.hpp ./event.hpp ./particle.hpp ./plist.hpp
server.o: ./trace.hpp ./gevent.hpp ./abskernel.hpp ./abstask.hpp ./cpu.hpp
server.o: ./timer.hpp ./powermodel.hpp ./task.hpp ./randomvar.hpp
server.o: ./cloneable.hpp ./kernel.hpp ./kernevt.hpp ./taskevt.hpp
server.o: ./feedback.hpp ./taskexc.hpp ./scheduler.hpp resmanager.hpp
simul.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
simul.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./event.hpp
simul.o: ./particle.hpp ./plist.hpp ./trace.hpp
sparepot.o: ./sparepot.hpp ./sporadicserver.hpp server.hpp ./baseexc.hpp
sparepot.o: ./entity.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
sparepot.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
sparepot.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
sparepot.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp ./task.hpp
sparepot.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp ./kernevt.hpp
sparepot.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
sparepot.o: resmanager.hpp ./capacitytimer.hpp ./supervisor.hpp
sporadicserver.o: ./sporadicserver.hpp server.hpp ./baseexc.hpp ./entity.hpp
sporadicserver.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
sporadicserver.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
sporadicserver.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
sporadicserver.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
sporadicserver.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
sporadicserver.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
sporadicserver.o: ./scheduler.hpp resmanager.hpp ./capacitytimer.hpp
strtoken.o: ./strtoken.hpp ./baseexc.hpp
supercbs.o: ./supercbs.hpp ./supervisor.hpp server.hpp ./baseexc.hpp
supercbs.o: ./entity.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
supercbs.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
supercbs.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
supercbs.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp ./task.hpp
supercbs.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp ./kernevt.hpp
supercbs.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
supercbs.o: resmanager.hpp ./sporadicserver.hpp ./capacitytimer.hpp
supercbs.o: cbserver.hpp
suspend_instr.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
suspend_instr.o: ./tick.hpp ./strtoken.hpp ./abskernel.hpp ./abstask.hpp
suspend_instr.o: ./cpu.hpp ./trace.hpp ./timer.hpp ./simul.hpp ./basestat.hpp
suspend_instr.o: ./debugstream.hpp ./event.hpp ./particle.hpp ./plist.hpp
suspend_instr.o: ./gevent.hpp ./powermodel.hpp ./kernevt.hpp ./task.hpp
suspend_instr.o: ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp ./feedback.hpp
suspend_instr.o: ./taskexc.hpp ./suspend_instr.hpp ./instr.hpp
task.o: ./regvar.hpp ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp
task.o: ./factory.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
task.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
task.o: ./particle.hpp ./plist.hpp ./trace.hpp ./abskernel.hpp ./abstask.hpp
task.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp ./instr.hpp
task.o: ./taskevt.hpp ./task.hpp ./kernel.hpp ./kernevt.hpp ./feedback.hpp
task.o: ./taskexc.hpp
taskevt.o: ./task.hpp ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
taskevt.o: ./strtoken.hpp ./gevent.hpp ./event.hpp ./simul.hpp ./basestat.hpp
taskevt.o: ./debugstream.hpp ./particle.hpp ./plist.hpp ./trace.hpp
taskevt.o: ./randomvar.hpp ./cloneable.hpp ./abstask.hpp ./cpu.hpp
taskevt.o: ./timer.hpp ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
taskevt.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
texttrace.o: ./texttrace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
texttrace.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
texttrace.o: ./debugstream.hpp ./entity.hpp ./particle.hpp ./plist.hpp
texttrace.o: ./trace.hpp rttask.hpp ./regvar.hpp ./randomvar.hpp
texttrace.o: ./cloneable.hpp ./task.hpp ./gevent.hpp ./abstask.hpp ./cpu.hpp
texttrace.o: ./timer.hpp ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
texttrace.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
threinstr.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
threinstr.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
threinstr.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./kernel.hpp
threinstr.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
threinstr.o: ./powermodel.hpp ./kernevt.hpp ./task.hpp ./randomvar.hpp
threinstr.o: ./cloneable.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
threinstr.o: ./threinstr.hpp ./factory.hpp ./instr.hpp
tick.o: ./strtoken.hpp ./baseexc.hpp ./tick.hpp
timer.o: ./timer.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
timer.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
timer.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./gevent.hpp
trace.o: ./event.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
trace.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
trace.o: ./particle.hpp ./plist.hpp ./trace.hpp
traceevent.o: ./jtrace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
traceevent.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
traceevent.o: ./debugstream.hpp ./entity.hpp ./particle.hpp ./plist.hpp
traceevent.o: ./trace.hpp ./traceevent.hpp
tracepower.o: ./tracepower.hpp ./cpu.hpp ./trace.hpp ./baseexc.hpp
tracepower.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./timer.hpp
tracepower.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./entity.hpp
tracepower.o: ./event.hpp ./particle.hpp ./plist.hpp ./gevent.hpp
tracepower.o: ./powermodel.hpp ./abstask.hpp
waitinstr.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
waitinstr.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
waitinstr.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./kernel.hpp
waitinstr.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
waitinstr.o: ./powermodel.hpp ./kernevt.hpp ./task.hpp ./randomvar.hpp
waitinstr.o: ./cloneable.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
waitinstr.o: ./waitinstr.hpp ./factory.hpp ./instr.hpp
