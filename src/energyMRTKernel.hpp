//
// Created by agost on 20/02/19.
//

#ifndef RTLIB2_0_ENERGYMRTKERNEL_H
#define RTLIB2_0_ENERGYMRTKERNEL_H

#include "mrtkernel.hpp"
#include "task.hpp"
#include "rttask.hpp"

#define _ENERGYMRTKERNEL_DBG_LEV "EnergyMRTKernel"

namespace RTSim {

    /**
        \ingroup kernel

        An implementation of a real-time multi processor kernel with
        global scheduling and a policy to smartly select CPU_BLs on big-LITTLE
        architectures. It contains:

        - a pointer to a list of CPU_BLs managed by the kernel;

        - a pointer to a Scheduler, which implements the scheduling
          policy;

        - a pointer to a Resource Manager, which is responsible for
          resource access related operations and thus implements a
          resource allocation policy;

        - a map of pointers to CPU_BL and task, which keeps the
          information about current task assignment to CPU_BLs;

        - the set of tasks handled by this kernel.

        This implementation is quite general: it lets the user of this
        class the freedom to adopt any scheduler derived form
        Scheduler and a resource manager derived from ResManager or no
        resource manager at all.  The kernel for a multiprocessor
        system with different CPU_BLs can be also be simulated. It is up
        to the instruction class to implement the correct duration of
        its execution by asking the kernel of its task the speed of
        the processor on whitch it's scheduled.

        We will probably have to derive from this class to implement
        static partition and mixed task allocation to CPU_BL.

        @see absCPU_BLFactory, Scheduler, ResManager, AbsRTTask
    */

    class EnergyMRTKernel : public MRTKernel {

    private:
        struct ConsumptionTable {
            double cons;
            CPU_BL* cpu;
            int opp;
        };

        // little, big (order matters for speed)
        Island_BL* _islands[2];

        double totalPowerCosumption;

        bool _tryingTaskOnCPU_BL;

        /// cores have a queue of ready=dispatching tasks
        map<CPU_BL*, vector<BeginDispatchMultiEvt*>> _beginEvts;
        map<CPU_BL*, vector<EndDispatchMultiEvt*>> _endEvts;

        /**
         * Needed by migration mechanism. What OPP do running tasks need to finish on time on their core?
         */
        map<AbsRTTask*, int> _m_currExe_OPP;

        /**
         * List of tasks ready on a CPU_BL with a given frequency.
         * Please use this instead of MRTKernel::_m_dispatched because you need to remember CPU_BL OPP.
         * In fact, the dispatch() could choose to schedule a task on a big CPU_BL with freq 200 and
         * another on another big with freq 1900. But in Big Little all CPU_BLs have same freq/OPP.
         */
        map<const AbsRTTask *, pair<CPU_BL*, int>> _m_dispatching;

        /// for debug, if you want to force a certain choice of cores and frequencies
        map<Task*, pair<CPU_BL*, int>> _m_forcedDispatch;

        /// list of tasks that you are trying to migrate (i.e., they already had a core assigned but you are
        /// trying to move it to a better one). Migration happens when a task ends
        vector<AbsRTTask*> _m_migrating;

        /// island cores load balancing policy: if possible, make all island cores work
        void balanceLoad(CPU_BL **chosenCPU, unsigned int &chosenOPP, bool &chosenCPUchanged, vector<struct ConsumptionTable> iDeltaPows);

        /**
        * CPU_BL choice from the table of consumptions (not sorted).
        * It tries to spread tasks on CPU_BLs if they have the same energy consumption
        */
        void chooseCPU_BL(AbsRTTask* t, vector<ConsumptionTable> iDeltaPows);

        /**
         * Implements the policy of leaving little 3 free, just in case a task with high WCET arrives,
         * risking to be forced to schedule it on big cores, increasing power consumption.
         */
        void leaveLittle3(AbsRTTask *t, std::vector<ConsumptionTable> iDeltaPows, CPU_BL*& chosenCPU_BL);

        /// Implements migration mechanism on task end
        void migrate(CPU_BL* endingCPU_BL);

        Island_BL* getIsland(Island island) const { return _islands[island]; }

        vector<CPU_BL*> getProcessors() const { 
            static vector<CPU_BL*> CPU_BLs;
            if (CPU_BLs.size() == 0) {
                for (CPU_BL* c : getIslandLittle()->getProcessors())
                    CPU_BLs.push_back(c);
                for (CPU_BL* c : getIslandBig()->getProcessors())
                    CPU_BLs.push_back(c);
            }
            return CPU_BLs;
        }

        vector<CPU_BL*> getProcessors(Island island) const {
            return getIsland(island)->getProcessors();
        }

        /// Tries to schedule a task on a CPU_BL, for all valid OPPs,
        /// remembering power consumption
        void tryTaskOnCPU_BL(AbsRTTask *t, CPU_BL *c, vector<struct ConsumptionTable> &iDeltaPows);

        /// needed for onOPPChanged()
        void setTryingTaskOnCPU_BL(bool b) { _tryingTaskOnCPU_BL = b; }
        bool isTryngTaskOnCPU_BL() { return _tryingTaskOnCPU_BL; }

        /// decides when to make context switch (i.e. call onBeginDispatchMulti) for t on p
        Tick decideBeginCtxSwitch(CPU_BL* p, AbsRTTask* t);

        /// commodity function for posting an event for context switching
        void postEvt(CPU* c, AbsRTTask* t, Tick when, bool endevt);

        /// drop event of context switch to t on c, whenever in future
        void dropEvt(CPU_BL* c, AbsRTTask* t);

        /// Update when the context switches of tasks of c will happen
        void updateDispatchingOrder(CPU_BL* c);

    public:

        /**
          * Kernel with scheduler s and CPU_BLs CPU_BLs
          */
        EnergyMRTKernel(Scheduler *s, Island_BL* big, Island_BL* little, const string &name = "");


        Island_BL* getIslandLittle() const { return _islands[0]; }
        Island_BL* getIslandBig() const { return _islands[1]; }
        void       setIslandLittle(Island_BL* island) { _islands[0] = island; }
        void       setIslandBig(Island_BL* island) { _islands[1] = island; }

        /**
           This is different from the version we have in MRTKernel: here you decide a
           processor for arrived tasks. See also @chooseCPU_BL() and @dispatch(CPU_BL*)

           This function is called by the onArrival and by the
            activate function. When we call this, we first select a
            processor, chosen smartly, for arrived tasks, then we call the other dispatch,
            confirming the dispatch process.
         */
        virtual void dispatch();

        /**
           This function only calls dispatch(CPU_BL*) and assigns a task to a CPU_BL,
           which should be actually done by dispatch(CPU_BL*) itself or onEndDispatchMulti(),
           but the last one is only called after dispatch(), and I need the assignment
           CPU_BL - task to be done before for getTask(CPU_BL*) to work
         */
        void dispatch(CPU *p, AbsRTTask *t, int opp);

        /**
            Dispatching a task on a given CPU_BL.

            This is different from the version we have on RTKernel,
            since we may need to specify on which CPU_BL we have to
            select a new task (for example, in the onEnd() and
            suspend() functions). In the onArrival() function,
            instead, we still do not know which processor is
            free!

            This is not used at all here because MRTkernel decides here
            to be task context switch in the current simulation or after a while.
            But that's because dispatch() just selects a free CPU and puts the
            new task there. EnergyMRTKernel, instead, allows CPUs to have a queue
            of tasks. Info about task is needed
         */
        virtual void dispatch(CPU* c) {}

        /// decides when task context switch begins on the CPU
        void dispatch(CPU* c, AbsRTTask* t);

        /// Tells where a task has been dispatched (when it's in the limbo
        /// between onBeginDispatchMulti and onEndDispatchMulti). Similar to getProcessor()
        CPU_BL* getDispatchingProcessor(const AbsRTTask* t) const;

        /// Tells what task has been dispatched to a CPU_BL (when it's in the limbo
        /// between onBeginDispatchMulti and onEndDispatchMulti). Similar to getProcessor()
        AbsRTTask* getDispatchingTask(const CPU_BL* CPU_BL) const;

        /**
           Returns island utilization given a capacity to scale up/down tasks WCET.
           It also returns the number of tasks being scheduled in the island
        */
        double getIslandUtilization(double capacity, Island island, int *nTaskIsland);

        /// Returns utilization of task t on CPU_BL c. This method could be defined for tasks, but this way I can make this implementation private
        double getUtilization(AbsRTTask* t, CPU_BL* c, double capacity) const;

        /// Returns utilization of tasks on CPU_BL c, supposing it runs with given freq and capacity. This method could be defined for tasks, but this way I can make this implementation private
        double getUtilization(CPU_BL* c, double freq, double capacity) const;

        /// Returns power consumption after SIMUL.getTime() == 1
        double getTotalPowerConsumption();

        /**
         * Begins the dispatch process (context switch). The task is dispatched, but not
         * executed yet. Its execution on its CPU_BL starts with onEndDispatchMulti()
         */
        virtual void onBeginDispatchMulti(BeginDispatchMultiEvt* e);

        /**
         *  First a task is dispatched, but not executed yet, in the
         *  onBeginDispatchMulti. Then, in the onEndDispatchMulti, its execution starts
         *  on the processor.
         */
        virtual void onEndDispatchMulti(EndDispatchMultiEvt* e);

        /// called when OPP changes on an island
        void onOppChanged(unsigned int curropp, Island_BL* island);

        /**
         * Invoked when a task ends
         */
        virtual void onEnd(AbsRTTask* t);

        /// returns true if we have already decided t's processor (valid before onEndMultiDispatch() completes)
        bool isDispatching(AbsRTTask*);

        /// is any task dispatched on CPU_BL p?
        bool isDispatching(CPU_BL* p);

        /**
         *  Returns a pointer to the task which is executing on given
         *  CPU_BL (NULL if given CPU_BL is idle)
         */
        virtual AbsRTTask* getTaskRunning(CPU* c);

         /// Returns the set of tasks in the runqueue of CPU_BL c, but the runnning one, ordered by DL (300, 400, ...)
        virtual vector<AbsRTTask*> getTasksDispatching(CPU_BL* c) const;

        virtual void newRun() {
            MRTKernel::newRun();

            for (auto& elem : _m_dispatching) {
                elem.second.first = NULL;
                elem.second.second = -1;
            }
        }

        /// to debug internal functions...
        void test();

        double time();

        void printMap();

        void printBool(bool b);

        bool manageForcedDispatch(Task*);

        void addForcedDispatch(RTSim::PeriodicTask *t, CPU_BL *c, int opp);
    };
}

#endif //RTLIB2_0_ENERGYMRTKERNEL_H
