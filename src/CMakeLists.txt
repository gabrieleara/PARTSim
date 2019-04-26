cmake_minimum_required(VERSION 3.6)   # CMake version check
project(simple_example)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 11)            # Enable c++11 standard

set(SOURCE_FILES 
AVRTask.cpp
basestat.cpp
bwi.cpp
capacitytimer.cpp
cbserver.cpp
cpu.cpp
debugstream.cpp
edfsched.cpp
energyMRTKernel.cpp
energyMRTKernel_example.cpp
entity.cpp
event.cpp
exeinstr.cpp
fcfsresmanager.cpp
feedback.cpp
feedbacktest.cpp
fifosched.cpp
fpsched.cpp
genericvar.cpp
grubserver.cpp
instr.cpp
interrupt.cpp
json_trace.cpp
jtrace.cpp
kernel.cpp
kernevt.cpp
load.cpp
mrtkernel.cpp
piresman.cpp
pollingserver.cpp
powermodel.cpp
randomvar.cpp
reginstr.cpp
regsched.cpp
regtask.cpp
regvar.cpp
resmanager.cpp
resource.cpp
rmsched.cpp
rrsched.cpp
rttask.cpp
schedinstr.cpp
schedpoints.cpp
schedrta.cpp
scheduler.cpp
server.cpp
simul.cpp
sparepot.cpp
sporadicserver.cpp
strtoken.cpp
supercbs.cpp
suspend_instr.cpp
task.cpp
taskevt.cpp
texttrace.cpp
threinstr.cpp
tick.cpp
timer.cpp
trace.cpp
traceevent.cpp
tracepower.cpp
waitinstr.cpp multi_sched.hpp multi_sched.cpp)            # Add main.cpp file of project root directory as source file

add_executable(simple_example ${SOURCE_FILES})       # Add executable target with source files listed in SOURCE_FILES variable

