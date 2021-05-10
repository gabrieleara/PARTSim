#ifndef _RTSIM_UTILS_HPP
#define _RTSIM_UTILS_HPP

#include <abstask.hpp>
#include <cbserver.hpp>
#include <exeinstr.hpp>
#include <task.hpp>

namespace RTSim {

    /**
        Created in order not to repeat functions in files that don't share a
       common parent. It contains some static utility functions (i.e.,
       Utils::...() ).
      */
    class Utils {
    public:
        /// Returns true if the element is in the vector
        template <typename T>
        static bool exists(T toBeFound, vector<T> objs) {
            bool found = false;
            for (T o : objs) // usare il for e l'if. T deve fare overriding di
                             // operator==() per il confronto tra oggetti
                if (o == toBeFound) {
                    found = true;
                    break;
                }
            return found;
        }

        /// Wrapper for exists()
        template <typename T>
        static bool exists(T toBeFound, vector<T> *objs) {
            if (objs == NULL)
                return false;
            return exists(toBeFound, *objs);
        }

        /// Returns the workload type of the first instrution of the task t
        static string getTaskWorkload(AbsRTTask *t, bool initInstr = true) {
            string wl = "";
            unsigned int i = 0;

            if (!initInstr) { /* add code if needed */
            }

            if (dynamic_cast<Task *>(t))
                wl = dynamic_cast<ExecInstr *>(
                         dynamic_cast<Task *>(t)->getInstrQueue().at(i).get())
                         ->getWorkload();
            else if (dynamic_cast<CBServer *>(t)) {
                // get first task of server and thus its WL
                Task *task = dynamic_cast<Task *>(
                    dynamic_cast<CBServerCallingEMRTKernel *>(t)
                        ->getFirstTask());
                if (task == NULL) { // server with no task
                    std::cout << "\tutils.hpp. cbs server with no task. "
                                 "returning wl idle"
                              << std::endl;
                    return "bzip2";
                }
                // std::cout << task->toString() << std::endl;
                wl =
                    dynamic_cast<ExecInstr *>(task->getInstrQueue().at(i).get())
                        ->getWorkload(); // this is for sure wrong. See comment
                                         // below
            }

            /**
                What if a task has instructions
               fixed(2,bzip2);fixed(3,hash);fixed(1,bzip2); ? Shouldn't workload
               be "mixed"? However, there is no such workload type in
               CPUModel::getSpeed(). A solution is to split the task into many
               and add a dependency (DAG), so to have task with the same
               workload.

                However, what about servers, having a task with WL bzip2 and
               another with WL hash or encrypt? I need the workload even before
               tasks begin executing.
              */

            assert(wl != "");
            return wl;
        }

        /// Vector to string
        static string toString(const vector<double> vec) {
            string s = "[";
            for (double d : vec)
                s += std::to_string(d) + ", ";
            return s + "]";
        }
    };

} // namespace RTSim

#endif