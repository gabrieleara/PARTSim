#ifndef _RTSIM_UTILS_HPP
#define _RTSIM_UTILS_HPP

#include "cbserver.hpp"
#include "task.hpp"
#include "exeinstr.hpp"
#include "abstask.hpp"

namespace RTSim {

    /**
        Created in order not to repeat functions in files that don't share a commong parent.
        It contains some static utility functions (i.e., Utils::...() ).
      */
    class Utils {
    public:
        static string getTaskWorkload(AbsRTTask *t) {
            string wl = "";

            if (dynamic_cast<Task*>(t))
                wl = dynamic_cast<ExecInstr*>(dynamic_cast<Task*>(t)->getInstrQueue().at(0).get())->getWorkload();
            else if (dynamic_cast<CBServer*>(t)) {
                // get first task of server and thus its WL
                Task *task = dynamic_cast<Task*>(dynamic_cast<CBServer*>(t)->getFirstTask());
                cout << task->toString() << endl;
                wl = dynamic_cast<ExecInstr*>(task->getInstrQueue().at(0).get())->getWorkload(); // this is for sure wrong. See /**
            }

            /**
                What if a task has instructions fixed(2,bzip2);fixed(3,hash);fixed(1,bzip2); ?
                Shouldn't workload be "mixed". However, there is no such workload type in CPUModel::getSpeed().
                A solution is to split the task into many and add a dependency (DAG), so to have task with
                the same workload.

                However, what about servers, having a task with WL bzip2 and another with WL hash or encrypt?
                I need the workload even before tasks begin executing.
              */

            assert (wl != "");
            return wl; 
        }
    };

} // namespace RTSim

#endif