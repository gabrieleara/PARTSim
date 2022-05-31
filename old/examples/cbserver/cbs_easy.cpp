#include <rtsim/kernel.hpp>
#include <rtsim/scheduler/edfsched.hpp>
#include <rtsim/scheduler/fifosched.hpp>
//#include <rtsim/jtrace.hpp>
#include <rtsim/cbserver.hpp>
#include <rtsim/rttask.hpp>
#include <rtsim/supercbs.hpp>
#include <rtsim/texttrace.hpp>

using namespace MetaSim;
using namespace RTSim;

int main() {
    try {
        //        JavaTrace jtrace("trace.trc");
        TextTrace ttrace("trace.txt");

        // create the scheduler and the kernel
        EDFScheduler sched;
        RTKernel kern(&sched);

        PeriodicTask t2(15, 15, 0, "TaskB");
        t2.insertCode("fixed(4);");
        t2.setAbort(false);

        // t11.setTrace(&jtrace);
        // t12.setTrace(&jtrace);
        // t2.setTrace(&jtrace);
        // t3.setTrace(&jtrace);

        ttrace.attachToTask(t2);
        // ttrace.attachToTask(t11);
        // ttrace.attachToTask(t12);
        // ttrace.attachToTask(t2);
        // ttrace.attachToTask(t3);

        CBServer serv(4, 15, 15, "hard", "server1",
                      "FIFOSched"); //"RRSched(2);");
        // serv.addTask(t11);
        // serv.addTask(t12);
        serv.addTask(t2);
        kern.addTask(serv, "");

        // kern.addTask(t12, "");
        // kern.addTask(t11, "");
        //    kern.addTask(t2, "");
        //    kern.addTask(t3, "");

        // run the simulation for 500 units of time
        SIMUL.dbg.enable(_TASK_DBG_LEV);
        SIMUL.dbg.enable(_KERNEL_DBG_LEV);
        SIMUL.dbg.enable(_SERVER_DBG_LEV);
        // SIMUL.dbg.enable(_FIFO_SCHED_DBG_LEV);
        SIMUL.run(50);
    } catch (BaseExc &e) {
        std::cout << e.what() << std::endl;
    } catch (parse_util::ParseExc &e2) {
        std::cout << e2.what() << std::endl;
    }
}
