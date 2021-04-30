/*
  In this example, a simple system is simulated, consisting of two
  real-time tasks scheduled by EDF on a single processor.
*/
#include <kernel.hpp>
#include <edfsched.hpp>
#include <jtrace.hpp>
#include <texttrace.hpp>
#include <json_trace.hpp>
#include <rttask.hpp>
#include <instr.hpp>

using namespace MetaSim;
using namespace RTSim;

int main()
{
    try {

        SIMUL.dbg.enable("All");
        SIMUL.dbg.setStream("debug.txt");
        // set the trace file. This can be visualized by the
        // rttracer tool

        TextTrace ttrace("trace.txt");

        cout << "Creating Scheduler and kernel" << endl;
        EDFScheduler edfsched;
        RTKernel kern(&edfsched);

        cout << "Creating the first task" << endl;
        PeriodicTask t1(4, 4, 0, "Task0");
        t1.insertCode("fixed(2);");

        cout << "Creating the second task" << endl;
        PeriodicTask t2(5, 5, 0, "Task1");

        cout << "Inserting code" << endl;
        t2.insertCode("fixed(2);");

        cout << "Setting up traces" << endl;

        // new way
        ttrace.attachToTask(t1);
        ttrace.attachToTask(t2);

        cout << "Adding tasks to schedulers" << endl;

        kern.addTask(t1, "");
        kern.addTask(t2, "");

        cout << "Ready to run!" << endl;
        // run the simulation for 500 units of time
        SIMUL.initSingleRun();
        SIMUL.run_to(14);
        cout << "sim paused at: " << SIMUL.getTime() << endl;
        t1.killInstance();
        SIMUL.run_to(30);
        SIMUL.endSingleRun();
    } catch (BaseExc &e) {
        cout << e.what() << endl;
    }
}
