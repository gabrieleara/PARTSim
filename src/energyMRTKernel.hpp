/***************************************************************************
    begin                : Thu Apr 24 15:54:58 CEST 2003
    copyright            : (C) 2003 by Giuseppe Lipari
    email                : lipari@sssup.it
 ***************************************************************************/
 *                                                                         *
 /***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RTLIB2_0_ENERGYMRTKERNEL_H
#define RTLIB2_0_ENERGYMRTKERNEL_H

#include "mrtkernel.hpp"
#include "task.hpp"
#include "rttask.hpp"
#include "multi_sched.hpp"

#define _ENERGYMRTKERNEL_DBG_LEV "EnergyMRTKernel"
#define LEAVE_LITTLE3_ENABLED 0

namespace RTSim {

    /**
        \ingroup sched

        Extension of MultiScheduler for Big-Little.
        Tasks are also bound to the desired OPP.

        Notice this is not a scheduler, its a class to manage cores queues
        of ready and running tasks.
     */
    class EnergyMultiScheduler : public MultiScheduler {
    private:
        /// OPP needed by tasks
        map<AbsRTTask*, unsigned int> _opps;

    public:
        EnergyMultiScheduler(MRTKernel* kernel, vector<CPU*> &cpus, vector<Scheduler*> &s, const string& name);

        /// Add a task to the queue of a core
        virtual void addTask(AbsRTTask* t, CPU_BL* c, const string& params, int opp) {
            assert(opp >= 0 && opp < c->getIsland()->getOPPsize());
            MultiScheduler::addTask(t, c, params);
            _opps[t] = opp;
        }

        /**
         * Add a task to the queue of a core. Use instead
         * of addTask if you don't need specific task parameters
         */
        virtual void insertTask(AbsRTTask* t, CPU_BL* c, int opp) {
            assert(opp >= 0 && opp < c->getIsland()->getOPPsize());
            MultiScheduler::insertTask(t, c);
            _opps[t] = opp;
        }

        /// Remove the first task of a core queue
        virtual void removeFirstFromQueue(CPU_BL* c) {
            AbsRTTask *t = getFirst(c);
            removeFromQueue(c, t);
        }

        /// Remove a specific task from a core queue
        virtual void removeFromQueue(CPU_BL* c, AbsRTTask* t) {
            MultiScheduler::removeFirstFromQueue(c);
            _opps.erase(t);
            assert(_opps.find(t) == _opps.end());
        }

        /// Empties a core queue
        virtual void empty(CPU_BL* c) {
            vector<AbsRTTask*> tasks = getAllTasksInQueue(c);
            MultiScheduler::empty(c);
            for (AbsRTTask* t : tasks)
                _opps.erase(t);
        }

        virtual void makeRunning(AbsRTTask* t, CPU* c) {
            MultiScheduler::makeRunning(t, c);
            c->setOPP(_opps[t]);
        }

        virtual void endRun() {
            MultiScheduler::endRun();
            _opps.clear();
        }

        virtual unsigned int getOPP(AbsRTTask* t) {
            return _opps[t];
        }

        string toString(CPU_BL* c) {
            vector<AbsRTTask*> tasks = getAllTasksInQueue(c);
            stringstream ss;
            int i = 1;
            for (AbsRTTask* t : tasks)
                ss << "\t" << i++ << ") " << t->toString() << endl;
            return ss.str();
        }

        virtual string toString() {
            stringstream ss;
            ss << "EnergyMultiScheduler:" << endl;
            for (const auto& q : _queues) {
                string qs = toString(dynamic_cast<CPU_BL *>(q.first));
                if (qs == "")
                    ss << "\tEmpty queue for " << q.first->getName() << endl;
                else
                    ss << "\t" << q.first->getName() << ":" << endl << qs << endl;
            }
            return ss.str();
        }
    };

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
        the processor on which it's scheduled.

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

        /**
         * List of tasks ready on a CPU_BL with a given frequency.
         * Please use this instead of MRTKernel::_m_dispatched because you need to remember CPU_BL OPP.
         * In fact, the dispatch() could choose to schedule a task on a big CPU_BL with freq 200 and
         * another on another big with freq 1900. But in Big Little all CPU_BLs have same freq/OPP.
         */
        //map<const AbsRTTask *, pair<CPU_BL*, int>> _m_dispatching;

        /// cores queues, containing ready and running tasks for each core
        EnergyMultiScheduler *_queues;

        /// for debug, if you want to force a certain choice of cores and frequencies
        map<Task*, pair<CPU_BL*, int>> _m_forcedDispatch;

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

        vector<CPU_BL*> getProcessors(Island island) const {
            return getIsland(island)->getProcessors();
        }

        /// Tries to schedule a task on a CPU_BL, for all valid OPPs,
        /// remembering power consumption
        void tryTaskOnCPU_BL(AbsRTTask *t, CPU_BL *c, vector<struct ConsumptionTable> &iDeltaPows);

        /// needed for onOPPChanged()
        void setTryingTaskOnCPU_BL(bool b) { _tryingTaskOnCPU_BL = b; }
        bool isTryngTaskOnCPU_BL() { return _tryingTaskOnCPU_BL; }

    public:

        /**
          * Kernel with scheduler s and CPU_BLs CPU_BLs.
          * qs are the schedulers you want for MultiScheduler, the queues of cores.
          *
          * @see MultiScheduler
          */
        EnergyMRTKernel(vector<Scheduler*> &qs, Scheduler *s, Island_BL* big, Island_BL* little, const string &name = "");

        virtual ~EnergyMRTKernel() {
          cout << "~EnergyMRTKernel" << endl;
          delete _islands[0];
          delete _islands[1];

          delete _queues;
        }

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
        CPU_BL* getDispatchingProcessor(const AbsRTTask* t);

        vector<CPU_BL*> getProcessors() const { 
            vector<CPU_BL*> CPU_BLs;
            for (CPU_BL* c : getIslandLittle()->getProcessors())
                CPU_BLs.push_back(c);
            for (CPU_BL* c : getIslandBig()->getProcessors())
                CPU_BLs.push_back(c);
            return CPU_BLs;
        }

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
        virtual vector<AbsRTTask*> getTasksReady(CPU_BL* c) const;

        virtual void newRun() {
            MRTKernel::newRun();
            _queues->newRun();
        }

        virtual void endRun() {
            MRTKernel::endRun();
            _queues->endRun();
        }

        /// to debug internal functions...
        void test();

        double time();

        void printMap();

        void printBool(bool b);

        virtual void printState();

        bool manageForcedDispatch(Task*);

        void addForcedDispatch(RTSim::PeriodicTask *t, CPU_BL *c, int opp);
    };
}

#endif //RTLIB2_0_ENERGYMRTKERNEL_H
