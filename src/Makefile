CPPFLAGS=-I.
CXXFLAGS=-Wno-deprecated -Wno-deprecated-declarations --std=c++11 -fPIC
CXXFLAGS_DEBUG=-Wno-deprecated -Wno-deprecated-declarations --std=c++11 -fPIC -g -O0

CPP_FILES=$(wildcard *.cpp)
HPP_FILES=$(wildcard *.hpp)
OBJ_FILES=$(patsubst %.cpp,release/%.o,$(CPP_FILES))
OBJ_FILES_DEBUG=$(patsubst %.cpp,debug/%.o,$(CPP_FILES))

SOURCES=$(CPP_FILES) $(HPP_FILES)

PROGRAMS=$(shell find examples/ -name '*.cpp' | sed -e 's/\.cpp//' | grep -v 'feedback\|schedp')
LIBRARIES=librtsim.so

PROGRAMS_DEBUG=$(patsubst %,%_debug,$(PROGRAMS))
LIBRARIES_DEBUG=$(patsubst %.so,%_debug.so,$(LIBRARIES))

all: $(LIBRARIES) $(LIBRARIES_DEBUG) $(PROGRAMS)

release/%.o: %.cpp
	$(CXX) -o $@ $(CXXFLAGS) $(CPPFLAGS) -c $<

debug/%.o: %.cpp
	$(CXX) -o $@ $(CXXFLAGS_DEBUG) $(CPPFLAGS) -c $<

librtsim.so: $(OBJ_FILES)
	$(CXX) -shared -o $@ $^

librtsim_debug.so: $(OBJ_FILES_DEBUG)
	$(CXX) -shared -o $@ $^

examples/%: examples/%.cpp
	$(CXX) -o $@ $(CXXFLAGS) $(CPPFLAGS) $< -L. -lrtsim

clean:
	rm -f $(PROGRAMS) $(PROGRAMS_DEBUG) $(LIBRARIES) $(LIBRARIES_DEBUG) $(OBJ_FILES) $(OBJ_FILES_DEBUG) *.o capacities.txt debug.txt *~ *.bak

dep:
	makedepend -prelease/ -- -I. -Y -- *.cpp
	makedepend -pdebug/ -a -- -I. -Y -- *.cpp

run: 
	./example > t.txt

release:
	mkdir -p release

debug:
	mkdir -p debug

# DO NOT DELETE

release/AVRTask.o: ./AVRTask.hpp ./task.hpp ./entity.hpp ./baseexc.hpp
release/AVRTask.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./gevent.hpp
release/AVRTask.o: ./event.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
release/AVRTask.o: ./particle.hpp ./plist.hpp ./trace.hpp ./randomvar.hpp
release/AVRTask.o: ./cloneable.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
release/AVRTask.o: ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
release/AVRTask.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
release/AVRTask.o: ./regvar.hpp ./factory.hpp ./instr.hpp
release/basestat.o: ./baseexc.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
release/basestat.o: ./strtoken.hpp ./simul.hpp ./debugstream.hpp ./entity.hpp
release/basestat.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
release/bwi.o: bwi.hpp resmanager.hpp ./entity.hpp ./baseexc.hpp
release/bwi.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./scheduler.hpp
release/bwi.o: ./plist.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
release/bwi.o: ./event.hpp ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp
release/bwi.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp server.hpp
release/bwi.o: ./abskernel.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
release/bwi.o: ./kernel.hpp ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
release/bwi.o: ./taskexc.hpp resource.hpp
release/capacitytimer.o: ./capacitytimer.hpp ./simul.hpp ./basestat.hpp
release/capacitytimer.o: ./basetype.hpp ./tick.hpp ./baseexc.hpp
release/capacitytimer.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp
release/capacitytimer.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
release/cbserver.o: cbserver.hpp server.hpp ./baseexc.hpp ./entity.hpp
release/cbserver.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
release/cbserver.o: ./basestat.hpp ./debugstream.hpp ./event.hpp
release/cbserver.o: ./particle.hpp ./plist.hpp ./trace.hpp ./gevent.hpp
release/cbserver.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
release/cbserver.o: ./powermodel.hpp ./task.hpp ./randomvar.hpp
release/cbserver.o: ./cloneable.hpp ./kernel.hpp ./kernevt.hpp ./taskevt.hpp
release/cbserver.o: ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
release/cbserver.o: resmanager.hpp ./capacitytimer.hpp rttask.hpp
release/cbserver.o: ./regvar.hpp energyMRTKernel.hpp mrtkernel.hpp
release/cbserver.o: multi_cores_scheds.hpp ./exeinstr.hpp ./instr.hpp
release/cbserver.o: utils.hpp ./rrsched.hpp ./edfsched.hpp
release/cpu.o: ./cpu.hpp ./trace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
release/cpu.o: ./strtoken.hpp ./timer.hpp ./simul.hpp ./basestat.hpp
release/cpu.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp
release/cpu.o: ./plist.hpp ./gevent.hpp ./powermodel.hpp ./abstask.hpp
release/cpu.o: energyMRTKernel.hpp mrtkernel.hpp ./kernel.hpp ./abskernel.hpp
release/cpu.o: ./kernevt.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
release/cpu.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp rttask.hpp
release/cpu.o: ./regvar.hpp multi_cores_scheds.hpp ./scheduler.hpp
release/cpu.o: ./exeinstr.hpp ./instr.hpp utils.hpp cbserver.hpp server.hpp
release/cpu.o: resmanager.hpp ./capacitytimer.hpp ./rrsched.hpp
release/cpu.o: ./edfsched.hpp
release/debugstream.o: ./debugstream.hpp ./basetype.hpp ./tick.hpp
release/debugstream.o: ./baseexc.hpp ./strtoken.hpp ./simul.hpp
release/debugstream.o: ./basestat.hpp ./entity.hpp ./event.hpp ./particle.hpp
release/debugstream.o: ./plist.hpp ./trace.hpp
release/edfsched.o: ./edfsched.hpp ./scheduler.hpp ./baseexc.hpp ./plist.hpp
release/edfsched.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
release/edfsched.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
release/edfsched.o: ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp
release/edfsched.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./kernel.hpp
release/edfsched.o: ./abskernel.hpp ./kernevt.hpp rttask.hpp ./regvar.hpp
release/edfsched.o: ./randomvar.hpp ./cloneable.hpp ./task.hpp ./taskevt.hpp
release/edfsched.o: ./feedback.hpp ./taskexc.hpp
release/energyMRTKernel.o: energyMRTKernel.hpp mrtkernel.hpp ./kernel.hpp
release/energyMRTKernel.o: ./baseexc.hpp ./entity.hpp ./basetype.hpp
release/energyMRTKernel.o: ./tick.hpp ./strtoken.hpp ./abskernel.hpp
release/energyMRTKernel.o: ./abstask.hpp ./cpu.hpp ./trace.hpp ./timer.hpp
release/energyMRTKernel.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp
release/energyMRTKernel.o: ./event.hpp ./particle.hpp ./plist.hpp
release/energyMRTKernel.o: ./gevent.hpp ./powermodel.hpp ./kernevt.hpp
release/energyMRTKernel.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp
release/energyMRTKernel.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
release/energyMRTKernel.o: rttask.hpp ./regvar.hpp multi_cores_scheds.hpp
release/energyMRTKernel.o: ./scheduler.hpp ./exeinstr.hpp ./instr.hpp
release/energyMRTKernel.o: utils.hpp cbserver.hpp server.hpp resmanager.hpp
release/energyMRTKernel.o: ./capacitytimer.hpp ./rrsched.hpp ./edfsched.hpp
release/entity.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
release/entity.o: ./strtoken.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
release/entity.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
release/event.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
release/event.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
release/event.o: ./debugstream.hpp ./particle.hpp ./plist.hpp ./trace.hpp
release/exeinstr.o: ./factory.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
release/exeinstr.o: ./tick.hpp ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
release/exeinstr.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
release/exeinstr.o: ./trace.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
release/exeinstr.o: ./powermodel.hpp ./abstask.hpp ./exeinstr.hpp
release/exeinstr.o: ./randomvar.hpp ./cloneable.hpp ./instr.hpp ./taskevt.hpp
release/exeinstr.o: ./task.hpp ./kernel.hpp ./abskernel.hpp ./kernevt.hpp
release/exeinstr.o: ./feedback.hpp ./taskexc.hpp
release/fcfsresmanager.o: ./abskernel.hpp ./abstask.hpp ./basetype.hpp
release/fcfsresmanager.o: ./tick.hpp ./baseexc.hpp ./strtoken.hpp ./cpu.hpp
release/fcfsresmanager.o: ./trace.hpp ./timer.hpp ./simul.hpp ./basestat.hpp
release/fcfsresmanager.o: ./debugstream.hpp ./entity.hpp ./event.hpp
release/fcfsresmanager.o: ./particle.hpp ./plist.hpp ./gevent.hpp
release/fcfsresmanager.o: ./powermodel.hpp ./fcfsresmanager.hpp
release/fcfsresmanager.o: resmanager.hpp ./scheduler.hpp resource.hpp
release/fcfsresmanager.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp
release/fcfsresmanager.o: ./kernel.hpp ./kernevt.hpp ./taskevt.hpp
release/fcfsresmanager.o: ./feedback.hpp ./taskexc.hpp
release/feedback.o: ./feedback.hpp ./entity.hpp ./baseexc.hpp ./basetype.hpp
release/feedback.o: ./tick.hpp ./strtoken.hpp ./task.hpp ./gevent.hpp
release/feedback.o: ./event.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
release/feedback.o: ./particle.hpp ./plist.hpp ./trace.hpp ./randomvar.hpp
release/feedback.o: ./cloneable.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
release/feedback.o: ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
release/feedback.o: ./kernevt.hpp ./taskevt.hpp ./taskexc.hpp
release/feedbacktest.o: ./feedbacktest.hpp ./feedback.hpp ./entity.hpp
release/feedbacktest.o: ./baseexc.hpp ./basetype.hpp ./tick.hpp
release/feedbacktest.o: ./strtoken.hpp ./sparepot.hpp ./sporadicserver.hpp
release/feedbacktest.o: server.hpp ./simul.hpp ./basestat.hpp
release/feedbacktest.o: ./debugstream.hpp ./event.hpp ./particle.hpp
release/feedbacktest.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
release/feedbacktest.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
release/feedbacktest.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp
release/feedbacktest.o: ./kernel.hpp ./kernevt.hpp ./taskevt.hpp
release/feedbacktest.o: ./taskexc.hpp ./scheduler.hpp resmanager.hpp
release/feedbacktest.o: ./capacitytimer.hpp ./supervisor.hpp
release/fifosched.o: ./fifosched.hpp ./scheduler.hpp ./baseexc.hpp
release/fifosched.o: ./plist.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
release/fifosched.o: ./tick.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
release/fifosched.o: ./event.hpp ./particle.hpp ./trace.hpp ./abstask.hpp
release/fifosched.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
release/fifosched.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./factory.hpp
release/fpsched.o: ./fpsched.hpp ./scheduler.hpp ./baseexc.hpp ./plist.hpp
release/fpsched.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
release/fpsched.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
release/fpsched.o: ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp
release/fpsched.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./kernel.hpp
release/fpsched.o: ./abskernel.hpp ./kernevt.hpp
release/genericvar.o: ./genericvar.hpp ./randomvar.hpp ./baseexc.hpp
release/genericvar.o: ./cloneable.hpp ./strtoken.hpp
release/grubserver.o: ./grubserver.hpp server.hpp ./baseexc.hpp ./entity.hpp
release/grubserver.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
release/grubserver.o: ./basestat.hpp ./debugstream.hpp ./event.hpp
release/grubserver.o: ./particle.hpp ./plist.hpp ./trace.hpp ./gevent.hpp
release/grubserver.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
release/grubserver.o: ./powermodel.hpp ./task.hpp ./randomvar.hpp
release/grubserver.o: ./cloneable.hpp ./kernel.hpp ./kernevt.hpp
release/grubserver.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
release/grubserver.o: ./scheduler.hpp resmanager.hpp ./capacitytimer.hpp
release/instr.o: ./instr.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
release/instr.o: ./tick.hpp ./strtoken.hpp ./event.hpp ./simul.hpp
release/instr.o: ./basestat.hpp ./debugstream.hpp ./particle.hpp ./plist.hpp
release/instr.o: ./trace.hpp ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp
release/instr.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
release/instr.o: ./powermodel.hpp
release/interrupt.o: ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp
release/interrupt.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
release/interrupt.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp
release/interrupt.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
release/interrupt.o: ./interrupt.hpp ./gevent.hpp ./task.hpp ./abstask.hpp
release/interrupt.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./kernel.hpp
release/interrupt.o: ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp
release/interrupt.o: ./feedback.hpp ./taskexc.hpp
release/json_trace.o: ./json_trace.hpp ./baseexc.hpp ./basetype.hpp
release/json_trace.o: ./tick.hpp ./strtoken.hpp ./event.hpp ./simul.hpp
release/json_trace.o: ./basestat.hpp ./debugstream.hpp ./entity.hpp
release/json_trace.o: ./particle.hpp ./plist.hpp ./trace.hpp rttask.hpp
release/json_trace.o: ./regvar.hpp ./randomvar.hpp ./cloneable.hpp ./task.hpp
release/json_trace.o: ./gevent.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
release/json_trace.o: ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
release/json_trace.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
release/json_trace.o: ./taskexc.hpp
release/jtrace.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
release/jtrace.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
release/jtrace.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
release/jtrace.o: server.hpp ./gevent.hpp ./abskernel.hpp ./abstask.hpp
release/jtrace.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./task.hpp
release/jtrace.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp ./kernevt.hpp
release/jtrace.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
release/jtrace.o: resmanager.hpp ./jtrace.hpp ./traceevent.hpp
release/jtrace.o: ./waitinstr.hpp ./factory.hpp ./instr.hpp
release/kernel.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
release/kernel.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
release/kernel.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
release/kernel.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
release/kernel.o: ./abstask.hpp ./kernel.hpp ./abskernel.hpp ./kernevt.hpp
release/kernel.o: resmanager.hpp ./scheduler.hpp ./task.hpp ./randomvar.hpp
release/kernel.o: ./cloneable.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
release/kernel.o: ./reginstr.hpp
release/kernevt.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
release/kernevt.o: ./tick.hpp ./strtoken.hpp ./abskernel.hpp ./abstask.hpp
release/kernevt.o: ./cpu.hpp ./trace.hpp ./timer.hpp ./simul.hpp
release/kernevt.o: ./basestat.hpp ./debugstream.hpp ./event.hpp
release/kernevt.o: ./particle.hpp ./plist.hpp ./gevent.hpp ./powermodel.hpp
release/kernevt.o: ./kernevt.hpp
release/load.o: ./exeinstr.hpp ./debugstream.hpp ./basetype.hpp ./tick.hpp
release/load.o: ./baseexc.hpp ./strtoken.hpp ./event.hpp ./simul.hpp
release/load.o: ./basestat.hpp ./entity.hpp ./particle.hpp ./plist.hpp
release/load.o: ./trace.hpp ./randomvar.hpp ./cloneable.hpp ./instr.hpp
release/load.o: ./taskevt.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
release/load.o: ./gevent.hpp ./powermodel.hpp ./load.hpp ./task.hpp
release/load.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./feedback.hpp
release/load.o: ./taskexc.hpp
release/mrtkernel.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
release/mrtkernel.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
release/mrtkernel.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
release/mrtkernel.o: ./trace.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
release/mrtkernel.o: ./powermodel.hpp ./abstask.hpp mrtkernel.hpp
release/mrtkernel.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./task.hpp
release/mrtkernel.o: ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp
release/mrtkernel.o: ./feedback.hpp ./taskexc.hpp resmanager.hpp
release/mrtkernel.o: ./scheduler.hpp cbserver.hpp server.hpp
release/mrtkernel.o: ./capacitytimer.hpp rttask.hpp ./regvar.hpp
release/multi_cores_scheds.o: multi_cores_scheds.hpp ./entity.hpp
release/multi_cores_scheds.o: ./baseexc.hpp ./basetype.hpp ./tick.hpp
release/multi_cores_scheds.o: ./strtoken.hpp ./scheduler.hpp ./plist.hpp
release/multi_cores_scheds.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp
release/multi_cores_scheds.o: ./event.hpp ./particle.hpp ./trace.hpp
release/multi_cores_scheds.o: ./abstask.hpp ./cpu.hpp ./timer.hpp
release/multi_cores_scheds.o: ./gevent.hpp ./powermodel.hpp ./taskevt.hpp
release/multi_cores_scheds.o: mrtkernel.hpp ./kernel.hpp ./abskernel.hpp
release/multi_cores_scheds.o: ./kernevt.hpp ./task.hpp ./randomvar.hpp
release/multi_cores_scheds.o: ./cloneable.hpp ./feedback.hpp ./taskexc.hpp
release/multi_cores_scheds.o: ./exeinstr.hpp ./instr.hpp utils.hpp
release/multi_cores_scheds.o: cbserver.hpp server.hpp resmanager.hpp
release/multi_cores_scheds.o: ./capacitytimer.hpp rttask.hpp ./regvar.hpp
release/multi_cores_scheds.o: ./rrsched.hpp ./edfsched.hpp
release/multi_cores_scheds.o: energyMRTKernel.hpp
release/piresman.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
release/piresman.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
release/piresman.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
release/piresman.o: ./trace.hpp ./abskernel.hpp ./abstask.hpp ./cpu.hpp
release/piresman.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./piresman.hpp
release/piresman.o: resmanager.hpp ./scheduler.hpp resource.hpp
release/pollingserver.o: pollingserver.hpp server.hpp ./baseexc.hpp
release/pollingserver.o: ./entity.hpp ./basetype.hpp ./tick.hpp
release/pollingserver.o: ./strtoken.hpp ./simul.hpp ./basestat.hpp
release/pollingserver.o: ./debugstream.hpp ./event.hpp ./particle.hpp
release/pollingserver.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
release/pollingserver.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
release/pollingserver.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp
release/pollingserver.o: ./kernel.hpp ./kernevt.hpp ./taskevt.hpp
release/pollingserver.o: ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
release/pollingserver.o: resmanager.hpp
release/powermodel.o: ./powermodel.hpp ./cpu.hpp ./trace.hpp ./baseexc.hpp
release/powermodel.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./timer.hpp
release/powermodel.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp
release/powermodel.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
release/powermodel.o: ./gevent.hpp ./abstask.hpp
release/ps_trace.o: ./ps_trace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
release/ps_trace.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
release/ps_trace.o: ./debugstream.hpp ./entity.hpp ./particle.hpp ./plist.hpp
release/ps_trace.o: ./trace.hpp rttask.hpp ./regvar.hpp ./randomvar.hpp
release/ps_trace.o: ./cloneable.hpp ./task.hpp ./gevent.hpp ./abstask.hpp
release/ps_trace.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./kernel.hpp
release/ps_trace.o: ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp
release/ps_trace.o: ./feedback.hpp ./taskexc.hpp
release/randomvar.o: ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp
release/randomvar.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
release/randomvar.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp
release/randomvar.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
release/randomvar.o: ./factory.hpp ./regvar.hpp
release/reginstr.o: ./strtoken.hpp ./baseexc.hpp ./exeinstr.hpp
release/reginstr.o: ./debugstream.hpp ./basetype.hpp ./tick.hpp ./event.hpp
release/reginstr.o: ./simul.hpp ./basestat.hpp ./entity.hpp ./particle.hpp
release/reginstr.o: ./plist.hpp ./trace.hpp ./randomvar.hpp ./cloneable.hpp
release/reginstr.o: ./instr.hpp ./taskevt.hpp ./abstask.hpp ./cpu.hpp
release/reginstr.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./threinstr.hpp
release/reginstr.o: ./factory.hpp ./schedinstr.hpp ./waitinstr.hpp
release/reginstr.o: ./suspend_instr.hpp ./reginstr.hpp
release/regsched.o: ./strtoken.hpp ./baseexc.hpp ./factory.hpp
release/regsched.o: ./fifosched.hpp ./scheduler.hpp ./plist.hpp ./simul.hpp
release/regsched.o: ./basestat.hpp ./basetype.hpp ./tick.hpp
release/regsched.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp
release/regsched.o: ./trace.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
release/regsched.o: ./gevent.hpp ./powermodel.hpp ./fpsched.hpp
release/regsched.o: ./edfsched.hpp ./rrsched.hpp
release/regtask.o: ./factory.hpp ./AVRTask.hpp ./task.hpp ./entity.hpp
release/regtask.o: ./baseexc.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
release/regtask.o: ./gevent.hpp ./event.hpp ./simul.hpp ./basestat.hpp
release/regtask.o: ./debugstream.hpp ./particle.hpp ./plist.hpp ./trace.hpp
release/regtask.o: ./randomvar.hpp ./cloneable.hpp ./abstask.hpp ./cpu.hpp
release/regtask.o: ./timer.hpp ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
release/regtask.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
release/regtask.o: ./regvar.hpp ./instr.hpp rttask.hpp ./reginstr.hpp
release/regvar.o: ./regvar.hpp ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp
release/regvar.o: ./genericvar.hpp ./factory.hpp
release/resmanager.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
release/resmanager.o: ./strtoken.hpp ./abskernel.hpp ./abstask.hpp ./cpu.hpp
release/resmanager.o: ./trace.hpp ./timer.hpp ./simul.hpp ./basestat.hpp
release/resmanager.o: ./debugstream.hpp ./event.hpp ./particle.hpp
release/resmanager.o: ./plist.hpp ./gevent.hpp ./powermodel.hpp
release/resmanager.o: resmanager.hpp ./scheduler.hpp resource.hpp
release/resource.o: ./abstask.hpp ./basetype.hpp ./tick.hpp ./baseexc.hpp
release/resource.o: ./strtoken.hpp ./cpu.hpp ./trace.hpp ./timer.hpp
release/resource.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./entity.hpp
release/resource.o: ./event.hpp ./particle.hpp ./plist.hpp ./gevent.hpp
release/resource.o: ./powermodel.hpp resource.hpp
release/rmsched.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
release/rmsched.o: ./tick.hpp ./strtoken.hpp ./abskernel.hpp ./abstask.hpp
release/rmsched.o: ./cpu.hpp ./trace.hpp ./timer.hpp ./simul.hpp
release/rmsched.o: ./basestat.hpp ./debugstream.hpp ./event.hpp
release/rmsched.o: ./particle.hpp ./plist.hpp ./gevent.hpp ./powermodel.hpp
release/rmsched.o: ./kernevt.hpp ./rmsched.hpp ./fpsched.hpp ./scheduler.hpp
release/rrsched.o: ./rrsched.hpp ./baseexc.hpp ./plist.hpp ./simul.hpp
release/rrsched.o: ./basestat.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
release/rrsched.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp
release/rrsched.o: ./trace.hpp ./gevent.hpp ./scheduler.hpp ./abstask.hpp
release/rrsched.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./task.hpp
release/rrsched.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
release/rrsched.o: ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
release/rrsched.o: ./taskexc.hpp energyMRTKernel.hpp mrtkernel.hpp rttask.hpp
release/rrsched.o: ./regvar.hpp multi_cores_scheds.hpp ./exeinstr.hpp
release/rrsched.o: ./instr.hpp utils.hpp cbserver.hpp server.hpp
release/rrsched.o: resmanager.hpp ./capacitytimer.hpp ./edfsched.hpp
release/rttask.o: rttask.hpp ./regvar.hpp ./randomvar.hpp ./baseexc.hpp
release/rttask.o: ./cloneable.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
release/rttask.o: ./tick.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
release/rttask.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
release/rttask.o: ./task.hpp ./gevent.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
release/rttask.o: ./powermodel.hpp ./kernel.hpp ./abskernel.hpp ./kernevt.hpp
release/rttask.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
release/schedinstr.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
release/schedinstr.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
release/schedinstr.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
release/schedinstr.o: ./trace.hpp ./kernel.hpp ./abskernel.hpp ./abstask.hpp
release/schedinstr.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
release/schedinstr.o: ./kernevt.hpp ./task.hpp ./randomvar.hpp
release/schedinstr.o: ./cloneable.hpp ./taskevt.hpp ./feedback.hpp
release/schedinstr.o: ./taskexc.hpp ./schedinstr.hpp ./factory.hpp
release/schedinstr.o: ./instr.hpp
release/schedpoints.o: ./schedpoints.hpp ./supervisor.hpp server.hpp
release/schedpoints.o: ./baseexc.hpp ./entity.hpp ./basetype.hpp ./tick.hpp
release/schedpoints.o: ./strtoken.hpp ./simul.hpp ./basestat.hpp
release/schedpoints.o: ./debugstream.hpp ./event.hpp ./particle.hpp
release/schedpoints.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
release/schedpoints.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
release/schedpoints.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp
release/schedpoints.o: ./kernel.hpp ./kernevt.hpp ./taskevt.hpp
release/schedpoints.o: ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
release/schedpoints.o: resmanager.hpp ./sporadicserver.hpp
release/schedpoints.o: ./capacitytimer.hpp
release/schedrta.o: ./schedrta.hpp ./supervisor.hpp server.hpp ./baseexc.hpp
release/schedrta.o: ./entity.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
release/schedrta.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./event.hpp
release/schedrta.o: ./particle.hpp ./plist.hpp ./trace.hpp ./gevent.hpp
release/schedrta.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
release/schedrta.o: ./powermodel.hpp ./task.hpp ./randomvar.hpp
release/schedrta.o: ./cloneable.hpp ./kernel.hpp ./kernevt.hpp ./taskevt.hpp
release/schedrta.o: ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
release/schedrta.o: resmanager.hpp ./sporadicserver.hpp ./capacitytimer.hpp
release/scheduler.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
release/scheduler.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
release/scheduler.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
release/scheduler.o: ./trace.hpp ./scheduler.hpp ./abstask.hpp ./cpu.hpp
release/scheduler.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./task.hpp
release/scheduler.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
release/scheduler.o: ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp
release/scheduler.o: ./feedback.hpp ./taskexc.hpp
release/server.o: ./factory.hpp server.hpp ./baseexc.hpp ./entity.hpp
release/server.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
release/server.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
release/server.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
release/server.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
release/server.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
release/server.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
release/server.o: ./scheduler.hpp resmanager.hpp
release/simul.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
release/simul.o: ./strtoken.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
release/simul.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
release/sparepot.o: ./sparepot.hpp ./sporadicserver.hpp server.hpp
release/sparepot.o: ./baseexc.hpp ./entity.hpp ./basetype.hpp ./tick.hpp
release/sparepot.o: ./strtoken.hpp ./simul.hpp ./basestat.hpp
release/sparepot.o: ./debugstream.hpp ./event.hpp ./particle.hpp ./plist.hpp
release/sparepot.o: ./trace.hpp ./gevent.hpp ./abskernel.hpp ./abstask.hpp
release/sparepot.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./task.hpp
release/sparepot.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
release/sparepot.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
release/sparepot.o: ./scheduler.hpp resmanager.hpp ./capacitytimer.hpp
release/sparepot.o: ./supervisor.hpp
release/sporadicserver.o: ./sporadicserver.hpp server.hpp ./baseexc.hpp
release/sporadicserver.o: ./entity.hpp ./basetype.hpp ./tick.hpp
release/sporadicserver.o: ./strtoken.hpp ./simul.hpp ./basestat.hpp
release/sporadicserver.o: ./debugstream.hpp ./event.hpp ./particle.hpp
release/sporadicserver.o: ./plist.hpp ./trace.hpp ./gevent.hpp
release/sporadicserver.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
release/sporadicserver.o: ./powermodel.hpp ./task.hpp ./randomvar.hpp
release/sporadicserver.o: ./cloneable.hpp ./kernel.hpp ./kernevt.hpp
release/sporadicserver.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
release/sporadicserver.o: ./scheduler.hpp resmanager.hpp ./capacitytimer.hpp
release/strtoken.o: ./strtoken.hpp ./baseexc.hpp
release/supercbs.o: ./supercbs.hpp ./supervisor.hpp server.hpp ./baseexc.hpp
release/supercbs.o: ./entity.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
release/supercbs.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./event.hpp
release/supercbs.o: ./particle.hpp ./plist.hpp ./trace.hpp ./gevent.hpp
release/supercbs.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
release/supercbs.o: ./powermodel.hpp ./task.hpp ./randomvar.hpp
release/supercbs.o: ./cloneable.hpp ./kernel.hpp ./kernevt.hpp ./taskevt.hpp
release/supercbs.o: ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
release/supercbs.o: resmanager.hpp ./sporadicserver.hpp ./capacitytimer.hpp
release/supercbs.o: cbserver.hpp rttask.hpp ./regvar.hpp
release/suspend_instr.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp
release/suspend_instr.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp
release/suspend_instr.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./trace.hpp
release/suspend_instr.o: ./timer.hpp ./simul.hpp ./basestat.hpp
release/suspend_instr.o: ./debugstream.hpp ./event.hpp ./particle.hpp
release/suspend_instr.o: ./plist.hpp ./gevent.hpp ./powermodel.hpp
release/suspend_instr.o: ./kernevt.hpp ./task.hpp ./randomvar.hpp
release/suspend_instr.o: ./cloneable.hpp ./taskevt.hpp ./feedback.hpp
release/suspend_instr.o: ./taskexc.hpp ./suspend_instr.hpp ./instr.hpp
release/task.o: ./regvar.hpp ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp
release/task.o: ./factory.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
release/task.o: ./tick.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
release/task.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
release/task.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
release/task.o: ./gevent.hpp ./powermodel.hpp ./instr.hpp ./taskevt.hpp
release/task.o: ./task.hpp ./kernel.hpp ./kernevt.hpp ./feedback.hpp
release/task.o: ./taskexc.hpp
release/taskevt.o: ./task.hpp ./entity.hpp ./baseexc.hpp ./basetype.hpp
release/taskevt.o: ./tick.hpp ./strtoken.hpp ./gevent.hpp ./event.hpp
release/taskevt.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp
release/taskevt.o: ./particle.hpp ./plist.hpp ./trace.hpp ./randomvar.hpp
release/taskevt.o: ./cloneable.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
release/taskevt.o: ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
release/taskevt.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
release/texttrace.o: ./texttrace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
release/texttrace.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
release/texttrace.o: ./debugstream.hpp ./entity.hpp ./particle.hpp
release/texttrace.o: ./plist.hpp ./trace.hpp rttask.hpp ./regvar.hpp
release/texttrace.o: ./randomvar.hpp ./cloneable.hpp ./task.hpp ./gevent.hpp
release/texttrace.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
release/texttrace.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp
release/texttrace.o: ./feedback.hpp ./taskexc.hpp
release/threinstr.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
release/threinstr.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
release/threinstr.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
release/threinstr.o: ./trace.hpp ./kernel.hpp ./abskernel.hpp ./abstask.hpp
release/threinstr.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
release/threinstr.o: ./kernevt.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
release/threinstr.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
release/threinstr.o: ./threinstr.hpp ./factory.hpp ./instr.hpp
release/tick.o: ./strtoken.hpp ./baseexc.hpp ./tick.hpp
release/timer.o: ./timer.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
release/timer.o: ./tick.hpp ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
release/timer.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
release/timer.o: ./trace.hpp ./gevent.hpp
release/trace.o: ./event.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
release/trace.o: ./tick.hpp ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
release/trace.o: ./entity.hpp ./particle.hpp ./plist.hpp ./trace.hpp
release/traceevent.o: ./jtrace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
release/traceevent.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
release/traceevent.o: ./debugstream.hpp ./entity.hpp ./particle.hpp
release/traceevent.o: ./plist.hpp ./trace.hpp ./traceevent.hpp
release/tracepower.o: tracepower.hpp ./cpu.hpp ./trace.hpp ./baseexc.hpp
release/tracepower.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./timer.hpp
release/tracepower.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp
release/tracepower.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
release/tracepower.o: ./gevent.hpp ./powermodel.hpp ./abstask.hpp
release/waitinstr.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
release/waitinstr.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
release/waitinstr.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
release/waitinstr.o: ./trace.hpp ./kernel.hpp ./abskernel.hpp ./abstask.hpp
release/waitinstr.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
release/waitinstr.o: ./kernevt.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
release/waitinstr.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
release/waitinstr.o: ./waitinstr.hpp ./factory.hpp ./instr.hpp

debug/AVRTask.o: ./AVRTask.hpp ./task.hpp ./entity.hpp ./baseexc.hpp
debug/AVRTask.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./gevent.hpp
debug/AVRTask.o: ./event.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
debug/AVRTask.o: ./particle.hpp ./plist.hpp ./trace.hpp ./randomvar.hpp
debug/AVRTask.o: ./cloneable.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
debug/AVRTask.o: ./powermodel.hpp ./kernel.hpp ./abskernel.hpp ./kernevt.hpp
debug/AVRTask.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./regvar.hpp
debug/AVRTask.o: ./factory.hpp ./instr.hpp
debug/basestat.o: ./baseexc.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
debug/basestat.o: ./strtoken.hpp ./simul.hpp ./debugstream.hpp ./entity.hpp
debug/basestat.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
debug/bwi.o: bwi.hpp resmanager.hpp ./entity.hpp ./baseexc.hpp ./basetype.hpp
debug/bwi.o: ./tick.hpp ./strtoken.hpp ./scheduler.hpp ./plist.hpp
debug/bwi.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./event.hpp
debug/bwi.o: ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
debug/bwi.o: ./gevent.hpp ./powermodel.hpp server.hpp ./abskernel.hpp
debug/bwi.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
debug/bwi.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
debug/bwi.o: resource.hpp
debug/capacitytimer.o: ./capacitytimer.hpp ./simul.hpp ./basestat.hpp
debug/capacitytimer.o: ./basetype.hpp ./tick.hpp ./baseexc.hpp ./strtoken.hpp
debug/capacitytimer.o: ./debugstream.hpp ./entity.hpp ./event.hpp
debug/capacitytimer.o: ./particle.hpp ./plist.hpp ./trace.hpp
debug/cbserver.o: cbserver.hpp server.hpp ./baseexc.hpp ./entity.hpp
debug/cbserver.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
debug/cbserver.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
debug/cbserver.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
debug/cbserver.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
debug/cbserver.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
debug/cbserver.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
debug/cbserver.o: ./scheduler.hpp resmanager.hpp ./capacitytimer.hpp
debug/cbserver.o: rttask.hpp ./regvar.hpp energyMRTKernel.hpp mrtkernel.hpp
debug/cbserver.o: multi_cores_scheds.hpp ./exeinstr.hpp ./instr.hpp utils.hpp
debug/cbserver.o: ./rrsched.hpp ./edfsched.hpp
debug/cpu.o: ./cpu.hpp ./trace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
debug/cpu.o: ./strtoken.hpp ./timer.hpp ./simul.hpp ./basestat.hpp
debug/cpu.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp
debug/cpu.o: ./plist.hpp ./gevent.hpp ./powermodel.hpp ./abstask.hpp
debug/cpu.o: energyMRTKernel.hpp mrtkernel.hpp ./kernel.hpp ./abskernel.hpp
debug/cpu.o: ./kernevt.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
debug/cpu.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp rttask.hpp
debug/cpu.o: ./regvar.hpp multi_cores_scheds.hpp ./scheduler.hpp
debug/cpu.o: ./exeinstr.hpp ./instr.hpp utils.hpp cbserver.hpp server.hpp
debug/cpu.o: resmanager.hpp ./capacitytimer.hpp ./rrsched.hpp ./edfsched.hpp
debug/debugstream.o: ./debugstream.hpp ./basetype.hpp ./tick.hpp
debug/debugstream.o: ./baseexc.hpp ./strtoken.hpp ./simul.hpp ./basestat.hpp
debug/debugstream.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
debug/debugstream.o: ./trace.hpp
debug/edfsched.o: ./edfsched.hpp ./scheduler.hpp ./baseexc.hpp ./plist.hpp
debug/edfsched.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
debug/edfsched.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
debug/edfsched.o: ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp
debug/edfsched.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./kernel.hpp
debug/edfsched.o: ./abskernel.hpp ./kernevt.hpp rttask.hpp ./regvar.hpp
debug/edfsched.o: ./randomvar.hpp ./cloneable.hpp ./task.hpp ./taskevt.hpp
debug/edfsched.o: ./feedback.hpp ./taskexc.hpp
debug/energyMRTKernel.o: energyMRTKernel.hpp mrtkernel.hpp ./kernel.hpp
debug/energyMRTKernel.o: ./baseexc.hpp ./entity.hpp ./basetype.hpp ./tick.hpp
debug/energyMRTKernel.o: ./strtoken.hpp ./abskernel.hpp ./abstask.hpp
debug/energyMRTKernel.o: ./cpu.hpp ./trace.hpp ./timer.hpp ./simul.hpp
debug/energyMRTKernel.o: ./basestat.hpp ./debugstream.hpp ./event.hpp
debug/energyMRTKernel.o: ./particle.hpp ./plist.hpp ./gevent.hpp
debug/energyMRTKernel.o: ./powermodel.hpp ./kernevt.hpp ./task.hpp
debug/energyMRTKernel.o: ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp
debug/energyMRTKernel.o: ./feedback.hpp ./taskexc.hpp rttask.hpp ./regvar.hpp
debug/energyMRTKernel.o: multi_cores_scheds.hpp ./scheduler.hpp
debug/energyMRTKernel.o: ./exeinstr.hpp ./instr.hpp utils.hpp cbserver.hpp
debug/energyMRTKernel.o: server.hpp resmanager.hpp ./capacitytimer.hpp
debug/energyMRTKernel.o: ./rrsched.hpp ./edfsched.hpp
debug/entity.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
debug/entity.o: ./strtoken.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
debug/entity.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
debug/event.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
debug/event.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
debug/event.o: ./debugstream.hpp ./particle.hpp ./plist.hpp ./trace.hpp
debug/exeinstr.o: ./factory.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
debug/exeinstr.o: ./tick.hpp ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
debug/exeinstr.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
debug/exeinstr.o: ./trace.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
debug/exeinstr.o: ./powermodel.hpp ./abstask.hpp ./exeinstr.hpp
debug/exeinstr.o: ./randomvar.hpp ./cloneable.hpp ./instr.hpp ./taskevt.hpp
debug/exeinstr.o: ./task.hpp ./kernel.hpp ./abskernel.hpp ./kernevt.hpp
debug/exeinstr.o: ./feedback.hpp ./taskexc.hpp
debug/fcfsresmanager.o: ./abskernel.hpp ./abstask.hpp ./basetype.hpp
debug/fcfsresmanager.o: ./tick.hpp ./baseexc.hpp ./strtoken.hpp ./cpu.hpp
debug/fcfsresmanager.o: ./trace.hpp ./timer.hpp ./simul.hpp ./basestat.hpp
debug/fcfsresmanager.o: ./debugstream.hpp ./entity.hpp ./event.hpp
debug/fcfsresmanager.o: ./particle.hpp ./plist.hpp ./gevent.hpp
debug/fcfsresmanager.o: ./powermodel.hpp ./fcfsresmanager.hpp resmanager.hpp
debug/fcfsresmanager.o: ./scheduler.hpp resource.hpp ./task.hpp
debug/fcfsresmanager.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
debug/fcfsresmanager.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
debug/fcfsresmanager.o: ./taskexc.hpp
debug/feedback.o: ./feedback.hpp ./entity.hpp ./baseexc.hpp ./basetype.hpp
debug/feedback.o: ./tick.hpp ./strtoken.hpp ./task.hpp ./gevent.hpp
debug/feedback.o: ./event.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
debug/feedback.o: ./particle.hpp ./plist.hpp ./trace.hpp ./randomvar.hpp
debug/feedback.o: ./cloneable.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
debug/feedback.o: ./powermodel.hpp ./kernel.hpp ./abskernel.hpp ./kernevt.hpp
debug/feedback.o: ./taskevt.hpp ./taskexc.hpp
debug/feedbacktest.o: ./feedbacktest.hpp ./feedback.hpp ./entity.hpp
debug/feedbacktest.o: ./baseexc.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
debug/feedbacktest.o: ./sparepot.hpp ./sporadicserver.hpp server.hpp
debug/feedbacktest.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp
debug/feedbacktest.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
debug/feedbacktest.o: ./gevent.hpp ./abskernel.hpp ./abstask.hpp ./cpu.hpp
debug/feedbacktest.o: ./timer.hpp ./powermodel.hpp ./task.hpp ./randomvar.hpp
debug/feedbacktest.o: ./cloneable.hpp ./kernel.hpp ./kernevt.hpp
debug/feedbacktest.o: ./taskevt.hpp ./taskexc.hpp ./scheduler.hpp
debug/feedbacktest.o: resmanager.hpp ./capacitytimer.hpp ./supervisor.hpp
debug/fifosched.o: ./fifosched.hpp ./scheduler.hpp ./baseexc.hpp ./plist.hpp
debug/fifosched.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
debug/fifosched.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
debug/fifosched.o: ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp
debug/fifosched.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./kernel.hpp
debug/fifosched.o: ./abskernel.hpp ./kernevt.hpp ./factory.hpp
debug/fpsched.o: ./fpsched.hpp ./scheduler.hpp ./baseexc.hpp ./plist.hpp
debug/fpsched.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
debug/fpsched.o: ./strtoken.hpp ./debugstream.hpp ./entity.hpp ./event.hpp
debug/fpsched.o: ./particle.hpp ./trace.hpp ./abstask.hpp ./cpu.hpp
debug/fpsched.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./kernel.hpp
debug/fpsched.o: ./abskernel.hpp ./kernevt.hpp
debug/genericvar.o: ./genericvar.hpp ./randomvar.hpp ./baseexc.hpp
debug/genericvar.o: ./cloneable.hpp ./strtoken.hpp
debug/grubserver.o: ./grubserver.hpp server.hpp ./baseexc.hpp ./entity.hpp
debug/grubserver.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
debug/grubserver.o: ./basestat.hpp ./debugstream.hpp ./event.hpp
debug/grubserver.o: ./particle.hpp ./plist.hpp ./trace.hpp ./gevent.hpp
debug/grubserver.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
debug/grubserver.o: ./powermodel.hpp ./task.hpp ./randomvar.hpp
debug/grubserver.o: ./cloneable.hpp ./kernel.hpp ./kernevt.hpp ./taskevt.hpp
debug/grubserver.o: ./feedback.hpp ./taskexc.hpp ./scheduler.hpp
debug/grubserver.o: resmanager.hpp ./capacitytimer.hpp
debug/instr.o: ./instr.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
debug/instr.o: ./tick.hpp ./strtoken.hpp ./event.hpp ./simul.hpp
debug/instr.o: ./basestat.hpp ./debugstream.hpp ./particle.hpp ./plist.hpp
debug/instr.o: ./trace.hpp ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp
debug/instr.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
debug/instr.o: ./powermodel.hpp
debug/interrupt.o: ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp ./simul.hpp
debug/interrupt.o: ./basestat.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
debug/interrupt.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp
debug/interrupt.o: ./plist.hpp ./trace.hpp ./interrupt.hpp ./gevent.hpp
debug/interrupt.o: ./task.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
debug/interrupt.o: ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
debug/interrupt.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
debug/json_trace.o: ./json_trace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
debug/json_trace.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
debug/json_trace.o: ./debugstream.hpp ./entity.hpp ./particle.hpp ./plist.hpp
debug/json_trace.o: ./trace.hpp rttask.hpp ./regvar.hpp ./randomvar.hpp
debug/json_trace.o: ./cloneable.hpp ./task.hpp ./gevent.hpp ./abstask.hpp
debug/json_trace.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./kernel.hpp
debug/json_trace.o: ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp
debug/json_trace.o: ./feedback.hpp ./taskexc.hpp
debug/jtrace.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
debug/jtrace.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
debug/jtrace.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp server.hpp
debug/jtrace.o: ./gevent.hpp ./abskernel.hpp ./abstask.hpp ./cpu.hpp
debug/jtrace.o: ./timer.hpp ./powermodel.hpp ./task.hpp ./randomvar.hpp
debug/jtrace.o: ./cloneable.hpp ./kernel.hpp ./kernevt.hpp ./taskevt.hpp
debug/jtrace.o: ./feedback.hpp ./taskexc.hpp ./scheduler.hpp resmanager.hpp
debug/jtrace.o: ./jtrace.hpp ./traceevent.hpp ./waitinstr.hpp ./factory.hpp
debug/jtrace.o: ./instr.hpp
debug/kernel.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
debug/kernel.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
debug/kernel.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./cpu.hpp
debug/kernel.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./abstask.hpp
debug/kernel.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp resmanager.hpp
debug/kernel.o: ./scheduler.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
debug/kernel.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./reginstr.hpp
debug/kernevt.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
debug/kernevt.o: ./tick.hpp ./strtoken.hpp ./abskernel.hpp ./abstask.hpp
debug/kernevt.o: ./cpu.hpp ./trace.hpp ./timer.hpp ./simul.hpp ./basestat.hpp
debug/kernevt.o: ./debugstream.hpp ./event.hpp ./particle.hpp ./plist.hpp
debug/kernevt.o: ./gevent.hpp ./powermodel.hpp ./kernevt.hpp
debug/load.o: ./exeinstr.hpp ./debugstream.hpp ./basetype.hpp ./tick.hpp
debug/load.o: ./baseexc.hpp ./strtoken.hpp ./event.hpp ./simul.hpp
debug/load.o: ./basestat.hpp ./entity.hpp ./particle.hpp ./plist.hpp
debug/load.o: ./trace.hpp ./randomvar.hpp ./cloneable.hpp ./instr.hpp
debug/load.o: ./taskevt.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
debug/load.o: ./powermodel.hpp ./load.hpp ./task.hpp ./kernel.hpp
debug/load.o: ./abskernel.hpp ./kernevt.hpp ./feedback.hpp ./taskexc.hpp
debug/mrtkernel.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
debug/mrtkernel.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
debug/mrtkernel.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
debug/mrtkernel.o: ./trace.hpp ./cpu.hpp ./timer.hpp ./gevent.hpp
debug/mrtkernel.o: ./powermodel.hpp ./abstask.hpp mrtkernel.hpp ./kernel.hpp
debug/mrtkernel.o: ./abskernel.hpp ./kernevt.hpp ./task.hpp ./randomvar.hpp
debug/mrtkernel.o: ./cloneable.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
debug/mrtkernel.o: resmanager.hpp ./scheduler.hpp cbserver.hpp server.hpp
debug/mrtkernel.o: ./capacitytimer.hpp rttask.hpp ./regvar.hpp
debug/multi_cores_scheds.o: multi_cores_scheds.hpp ./entity.hpp ./baseexc.hpp
debug/multi_cores_scheds.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp
debug/multi_cores_scheds.o: ./scheduler.hpp ./plist.hpp ./simul.hpp
debug/multi_cores_scheds.o: ./basestat.hpp ./debugstream.hpp ./event.hpp
debug/multi_cores_scheds.o: ./particle.hpp ./trace.hpp ./abstask.hpp
debug/multi_cores_scheds.o: ./cpu.hpp ./timer.hpp ./gevent.hpp
debug/multi_cores_scheds.o: ./powermodel.hpp ./taskevt.hpp mrtkernel.hpp
debug/multi_cores_scheds.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp
debug/multi_cores_scheds.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp
debug/multi_cores_scheds.o: ./feedback.hpp ./taskexc.hpp ./exeinstr.hpp
debug/multi_cores_scheds.o: ./instr.hpp utils.hpp cbserver.hpp server.hpp
debug/multi_cores_scheds.o: resmanager.hpp ./capacitytimer.hpp rttask.hpp
debug/multi_cores_scheds.o: ./regvar.hpp ./rrsched.hpp ./edfsched.hpp
debug/multi_cores_scheds.o: energyMRTKernel.hpp
debug/piresman.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
debug/piresman.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
debug/piresman.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
debug/piresman.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
debug/piresman.o: ./gevent.hpp ./powermodel.hpp ./piresman.hpp resmanager.hpp
debug/piresman.o: ./scheduler.hpp resource.hpp
debug/pollingserver.o: pollingserver.hpp server.hpp ./baseexc.hpp
debug/pollingserver.o: ./entity.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
debug/pollingserver.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp
debug/pollingserver.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
debug/pollingserver.o: ./gevent.hpp ./abskernel.hpp ./abstask.hpp ./cpu.hpp
debug/pollingserver.o: ./timer.hpp ./powermodel.hpp ./task.hpp
debug/pollingserver.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
debug/pollingserver.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
debug/pollingserver.o: ./taskexc.hpp ./scheduler.hpp resmanager.hpp
debug/powermodel.o: ./powermodel.hpp ./cpu.hpp ./trace.hpp ./baseexc.hpp
debug/powermodel.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./timer.hpp
debug/powermodel.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./entity.hpp
debug/powermodel.o: ./event.hpp ./particle.hpp ./plist.hpp ./gevent.hpp
debug/powermodel.o: ./abstask.hpp
debug/ps_trace.o: ./ps_trace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
debug/ps_trace.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
debug/ps_trace.o: ./debugstream.hpp ./entity.hpp ./particle.hpp ./plist.hpp
debug/ps_trace.o: ./trace.hpp rttask.hpp ./regvar.hpp ./randomvar.hpp
debug/ps_trace.o: ./cloneable.hpp ./task.hpp ./gevent.hpp ./abstask.hpp
debug/ps_trace.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./kernel.hpp
debug/ps_trace.o: ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
debug/ps_trace.o: ./taskexc.hpp
debug/randomvar.o: ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp ./simul.hpp
debug/randomvar.o: ./basestat.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
debug/randomvar.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp
debug/randomvar.o: ./plist.hpp ./trace.hpp ./factory.hpp ./regvar.hpp
debug/reginstr.o: ./strtoken.hpp ./baseexc.hpp ./exeinstr.hpp
debug/reginstr.o: ./debugstream.hpp ./basetype.hpp ./tick.hpp ./event.hpp
debug/reginstr.o: ./simul.hpp ./basestat.hpp ./entity.hpp ./particle.hpp
debug/reginstr.o: ./plist.hpp ./trace.hpp ./randomvar.hpp ./cloneable.hpp
debug/reginstr.o: ./instr.hpp ./taskevt.hpp ./abstask.hpp ./cpu.hpp
debug/reginstr.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./threinstr.hpp
debug/reginstr.o: ./factory.hpp ./schedinstr.hpp ./waitinstr.hpp
debug/reginstr.o: ./suspend_instr.hpp ./reginstr.hpp
debug/regsched.o: ./strtoken.hpp ./baseexc.hpp ./factory.hpp ./fifosched.hpp
debug/regsched.o: ./scheduler.hpp ./plist.hpp ./simul.hpp ./basestat.hpp
debug/regsched.o: ./basetype.hpp ./tick.hpp ./debugstream.hpp ./entity.hpp
debug/regsched.o: ./event.hpp ./particle.hpp ./trace.hpp ./abstask.hpp
debug/regsched.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
debug/regsched.o: ./fpsched.hpp ./edfsched.hpp ./rrsched.hpp
debug/regtask.o: ./factory.hpp ./AVRTask.hpp ./task.hpp ./entity.hpp
debug/regtask.o: ./baseexc.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
debug/regtask.o: ./gevent.hpp ./event.hpp ./simul.hpp ./basestat.hpp
debug/regtask.o: ./debugstream.hpp ./particle.hpp ./plist.hpp ./trace.hpp
debug/regtask.o: ./randomvar.hpp ./cloneable.hpp ./abstask.hpp ./cpu.hpp
debug/regtask.o: ./timer.hpp ./powermodel.hpp ./kernel.hpp ./abskernel.hpp
debug/regtask.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
debug/regtask.o: ./regvar.hpp ./instr.hpp rttask.hpp ./reginstr.hpp
debug/regvar.o: ./regvar.hpp ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp
debug/regvar.o: ./genericvar.hpp ./factory.hpp
debug/resmanager.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
debug/resmanager.o: ./strtoken.hpp ./abskernel.hpp ./abstask.hpp ./cpu.hpp
debug/resmanager.o: ./trace.hpp ./timer.hpp ./simul.hpp ./basestat.hpp
debug/resmanager.o: ./debugstream.hpp ./event.hpp ./particle.hpp ./plist.hpp
debug/resmanager.o: ./gevent.hpp ./powermodel.hpp resmanager.hpp
debug/resmanager.o: ./scheduler.hpp resource.hpp
debug/resource.o: ./abstask.hpp ./basetype.hpp ./tick.hpp ./baseexc.hpp
debug/resource.o: ./strtoken.hpp ./cpu.hpp ./trace.hpp ./timer.hpp
debug/resource.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./entity.hpp
debug/resource.o: ./event.hpp ./particle.hpp ./plist.hpp ./gevent.hpp
debug/resource.o: ./powermodel.hpp resource.hpp
debug/rmsched.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
debug/rmsched.o: ./tick.hpp ./strtoken.hpp ./abskernel.hpp ./abstask.hpp
debug/rmsched.o: ./cpu.hpp ./trace.hpp ./timer.hpp ./simul.hpp ./basestat.hpp
debug/rmsched.o: ./debugstream.hpp ./event.hpp ./particle.hpp ./plist.hpp
debug/rmsched.o: ./gevent.hpp ./powermodel.hpp ./kernevt.hpp ./rmsched.hpp
debug/rmsched.o: ./fpsched.hpp ./scheduler.hpp
debug/rrsched.o: ./rrsched.hpp ./baseexc.hpp ./plist.hpp ./simul.hpp
debug/rrsched.o: ./basestat.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
debug/rrsched.o: ./debugstream.hpp ./entity.hpp ./event.hpp ./particle.hpp
debug/rrsched.o: ./trace.hpp ./gevent.hpp ./scheduler.hpp ./abstask.hpp
debug/rrsched.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./task.hpp
debug/rrsched.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp ./abskernel.hpp
debug/rrsched.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
debug/rrsched.o: energyMRTKernel.hpp mrtkernel.hpp rttask.hpp ./regvar.hpp
debug/rrsched.o: multi_cores_scheds.hpp ./exeinstr.hpp ./instr.hpp utils.hpp
debug/rrsched.o: cbserver.hpp server.hpp resmanager.hpp ./capacitytimer.hpp
debug/rrsched.o: ./edfsched.hpp
debug/rttask.o: rttask.hpp ./regvar.hpp ./randomvar.hpp ./baseexc.hpp
debug/rttask.o: ./cloneable.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
debug/rttask.o: ./tick.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
debug/rttask.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp ./task.hpp
debug/rttask.o: ./gevent.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
debug/rttask.o: ./powermodel.hpp ./kernel.hpp ./abskernel.hpp ./kernevt.hpp
debug/rttask.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
debug/schedinstr.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
debug/schedinstr.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
debug/schedinstr.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
debug/schedinstr.o: ./trace.hpp ./kernel.hpp ./abskernel.hpp ./abstask.hpp
debug/schedinstr.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
debug/schedinstr.o: ./kernevt.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
debug/schedinstr.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
debug/schedinstr.o: ./schedinstr.hpp ./factory.hpp ./instr.hpp
debug/schedpoints.o: ./schedpoints.hpp ./supervisor.hpp server.hpp
debug/schedpoints.o: ./baseexc.hpp ./entity.hpp ./basetype.hpp ./tick.hpp
debug/schedpoints.o: ./strtoken.hpp ./simul.hpp ./basestat.hpp
debug/schedpoints.o: ./debugstream.hpp ./event.hpp ./particle.hpp ./plist.hpp
debug/schedpoints.o: ./trace.hpp ./gevent.hpp ./abskernel.hpp ./abstask.hpp
debug/schedpoints.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./task.hpp
debug/schedpoints.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
debug/schedpoints.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
debug/schedpoints.o: ./scheduler.hpp resmanager.hpp ./sporadicserver.hpp
debug/schedpoints.o: ./capacitytimer.hpp
debug/schedrta.o: ./schedrta.hpp ./supervisor.hpp server.hpp ./baseexc.hpp
debug/schedrta.o: ./entity.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
debug/schedrta.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./event.hpp
debug/schedrta.o: ./particle.hpp ./plist.hpp ./trace.hpp ./gevent.hpp
debug/schedrta.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
debug/schedrta.o: ./powermodel.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
debug/schedrta.o: ./kernel.hpp ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
debug/schedrta.o: ./taskexc.hpp ./scheduler.hpp resmanager.hpp
debug/schedrta.o: ./sporadicserver.hpp ./capacitytimer.hpp
debug/scheduler.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
debug/scheduler.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
debug/scheduler.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
debug/scheduler.o: ./trace.hpp ./scheduler.hpp ./abstask.hpp ./cpu.hpp
debug/scheduler.o: ./timer.hpp ./gevent.hpp ./powermodel.hpp ./task.hpp
debug/scheduler.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
debug/scheduler.o: ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
debug/scheduler.o: ./taskexc.hpp
debug/server.o: ./factory.hpp server.hpp ./baseexc.hpp ./entity.hpp
debug/server.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./simul.hpp
debug/server.o: ./basestat.hpp ./debugstream.hpp ./event.hpp ./particle.hpp
debug/server.o: ./plist.hpp ./trace.hpp ./gevent.hpp ./abskernel.hpp
debug/server.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
debug/server.o: ./task.hpp ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
debug/server.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp ./taskexc.hpp
debug/server.o: ./scheduler.hpp resmanager.hpp
debug/simul.o: ./entity.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
debug/simul.o: ./strtoken.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
debug/simul.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
debug/sparepot.o: ./sparepot.hpp ./sporadicserver.hpp server.hpp
debug/sparepot.o: ./baseexc.hpp ./entity.hpp ./basetype.hpp ./tick.hpp
debug/sparepot.o: ./strtoken.hpp ./simul.hpp ./basestat.hpp ./debugstream.hpp
debug/sparepot.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
debug/sparepot.o: ./gevent.hpp ./abskernel.hpp ./abstask.hpp ./cpu.hpp
debug/sparepot.o: ./timer.hpp ./powermodel.hpp ./task.hpp ./randomvar.hpp
debug/sparepot.o: ./cloneable.hpp ./kernel.hpp ./kernevt.hpp ./taskevt.hpp
debug/sparepot.o: ./feedback.hpp ./taskexc.hpp ./scheduler.hpp resmanager.hpp
debug/sparepot.o: ./capacitytimer.hpp ./supervisor.hpp
debug/sporadicserver.o: ./sporadicserver.hpp server.hpp ./baseexc.hpp
debug/sporadicserver.o: ./entity.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
debug/sporadicserver.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp
debug/sporadicserver.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
debug/sporadicserver.o: ./gevent.hpp ./abskernel.hpp ./abstask.hpp ./cpu.hpp
debug/sporadicserver.o: ./timer.hpp ./powermodel.hpp ./task.hpp
debug/sporadicserver.o: ./randomvar.hpp ./cloneable.hpp ./kernel.hpp
debug/sporadicserver.o: ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
debug/sporadicserver.o: ./taskexc.hpp ./scheduler.hpp resmanager.hpp
debug/sporadicserver.o: ./capacitytimer.hpp
debug/strtoken.o: ./strtoken.hpp ./baseexc.hpp
debug/supercbs.o: ./supercbs.hpp ./supervisor.hpp server.hpp ./baseexc.hpp
debug/supercbs.o: ./entity.hpp ./basetype.hpp ./tick.hpp ./strtoken.hpp
debug/supercbs.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./event.hpp
debug/supercbs.o: ./particle.hpp ./plist.hpp ./trace.hpp ./gevent.hpp
debug/supercbs.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
debug/supercbs.o: ./powermodel.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
debug/supercbs.o: ./kernel.hpp ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
debug/supercbs.o: ./taskexc.hpp ./scheduler.hpp resmanager.hpp
debug/supercbs.o: ./sporadicserver.hpp ./capacitytimer.hpp cbserver.hpp
debug/supercbs.o: rttask.hpp ./regvar.hpp
debug/suspend_instr.o: ./kernel.hpp ./baseexc.hpp ./entity.hpp ./basetype.hpp
debug/suspend_instr.o: ./tick.hpp ./strtoken.hpp ./abskernel.hpp
debug/suspend_instr.o: ./abstask.hpp ./cpu.hpp ./trace.hpp ./timer.hpp
debug/suspend_instr.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp
debug/suspend_instr.o: ./event.hpp ./particle.hpp ./plist.hpp ./gevent.hpp
debug/suspend_instr.o: ./powermodel.hpp ./kernevt.hpp ./task.hpp
debug/suspend_instr.o: ./randomvar.hpp ./cloneable.hpp ./taskevt.hpp
debug/suspend_instr.o: ./feedback.hpp ./taskexc.hpp ./suspend_instr.hpp
debug/suspend_instr.o: ./instr.hpp
debug/task.o: ./regvar.hpp ./randomvar.hpp ./baseexc.hpp ./cloneable.hpp
debug/task.o: ./factory.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
debug/task.o: ./tick.hpp ./strtoken.hpp ./debugstream.hpp ./entity.hpp
debug/task.o: ./event.hpp ./particle.hpp ./plist.hpp ./trace.hpp
debug/task.o: ./abskernel.hpp ./abstask.hpp ./cpu.hpp ./timer.hpp
debug/task.o: ./gevent.hpp ./powermodel.hpp ./instr.hpp ./taskevt.hpp
debug/task.o: ./task.hpp ./kernel.hpp ./kernevt.hpp ./feedback.hpp
debug/task.o: ./taskexc.hpp
debug/taskevt.o: ./task.hpp ./entity.hpp ./baseexc.hpp ./basetype.hpp
debug/taskevt.o: ./tick.hpp ./strtoken.hpp ./gevent.hpp ./event.hpp
debug/taskevt.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./particle.hpp
debug/taskevt.o: ./plist.hpp ./trace.hpp ./randomvar.hpp ./cloneable.hpp
debug/taskevt.o: ./abstask.hpp ./cpu.hpp ./timer.hpp ./powermodel.hpp
debug/taskevt.o: ./kernel.hpp ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp
debug/taskevt.o: ./feedback.hpp ./taskexc.hpp
debug/texttrace.o: ./texttrace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
debug/texttrace.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
debug/texttrace.o: ./debugstream.hpp ./entity.hpp ./particle.hpp ./plist.hpp
debug/texttrace.o: ./trace.hpp rttask.hpp ./regvar.hpp ./randomvar.hpp
debug/texttrace.o: ./cloneable.hpp ./task.hpp ./gevent.hpp ./abstask.hpp
debug/texttrace.o: ./cpu.hpp ./timer.hpp ./powermodel.hpp ./kernel.hpp
debug/texttrace.o: ./abskernel.hpp ./kernevt.hpp ./taskevt.hpp ./feedback.hpp
debug/texttrace.o: ./taskexc.hpp
debug/threinstr.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
debug/threinstr.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
debug/threinstr.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
debug/threinstr.o: ./trace.hpp ./kernel.hpp ./abskernel.hpp ./abstask.hpp
debug/threinstr.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
debug/threinstr.o: ./kernevt.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
debug/threinstr.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./threinstr.hpp
debug/threinstr.o: ./factory.hpp ./instr.hpp
debug/tick.o: ./strtoken.hpp ./baseexc.hpp ./tick.hpp
debug/timer.o: ./timer.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
debug/timer.o: ./tick.hpp ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
debug/timer.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
debug/timer.o: ./trace.hpp ./gevent.hpp
debug/trace.o: ./event.hpp ./simul.hpp ./basestat.hpp ./basetype.hpp
debug/trace.o: ./tick.hpp ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
debug/trace.o: ./entity.hpp ./particle.hpp ./plist.hpp ./trace.hpp
debug/traceevent.o: ./jtrace.hpp ./baseexc.hpp ./basetype.hpp ./tick.hpp
debug/traceevent.o: ./strtoken.hpp ./event.hpp ./simul.hpp ./basestat.hpp
debug/traceevent.o: ./debugstream.hpp ./entity.hpp ./particle.hpp ./plist.hpp
debug/traceevent.o: ./trace.hpp ./traceevent.hpp
debug/tracepower.o: tracepower.hpp ./cpu.hpp ./trace.hpp ./baseexc.hpp
debug/tracepower.o: ./basetype.hpp ./tick.hpp ./strtoken.hpp ./timer.hpp
debug/tracepower.o: ./simul.hpp ./basestat.hpp ./debugstream.hpp ./entity.hpp
debug/tracepower.o: ./event.hpp ./particle.hpp ./plist.hpp ./gevent.hpp
debug/tracepower.o: ./powermodel.hpp ./abstask.hpp
debug/waitinstr.o: ./simul.hpp ./basestat.hpp ./basetype.hpp ./tick.hpp
debug/waitinstr.o: ./baseexc.hpp ./strtoken.hpp ./debugstream.hpp
debug/waitinstr.o: ./entity.hpp ./event.hpp ./particle.hpp ./plist.hpp
debug/waitinstr.o: ./trace.hpp ./kernel.hpp ./abskernel.hpp ./abstask.hpp
debug/waitinstr.o: ./cpu.hpp ./timer.hpp ./gevent.hpp ./powermodel.hpp
debug/waitinstr.o: ./kernevt.hpp ./task.hpp ./randomvar.hpp ./cloneable.hpp
debug/waitinstr.o: ./taskevt.hpp ./feedback.hpp ./taskexc.hpp ./waitinstr.hpp
debug/waitinstr.o: ./factory.hpp ./instr.hpp
