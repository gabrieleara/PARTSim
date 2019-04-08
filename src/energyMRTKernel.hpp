//
// Created by agost on 20/02/19.
//

#ifndef RTLIB2_0_ENERGYMRTKERNEL_H
#define RTLIB2_0_ENERGYMRTKERNEL_H

#include "mrtkernel.hpp"

#define _ENERGYMRTKERNEL_DBG_LEV "EnergyMRTKernel"

namespace RTSim {
    /**
        \ingroup kernel

        An implementation of a real-time multi processor kernel with
        global scheduling and a policy to smartly select CPUs on big-LITTLE
        architectures. It contains:

        - a pointer to a list of CPUs managed by the kernel;

        - a pointer to a Scheduler, which implements the scheduling
          policy;

        - a pointer to a Resource Manager, which is responsible for
          resource access related operations and thus implements a
          resource allocation policy;

        - a map of pointers to CPU and task, which keeps the
          information about current task assignment to CPUs;

        - the set of tasks handled by this kernel.

        This implementation is quite general: it lets the user of this
        class the freedom to adopt any scheduler derived form
        Scheduler and a resource manager derived from ResManager or no
        resource manager at all.  The kernel for a multiprocessor
        system with different CPUs can be also be simulated. It is up
        to the instruction class to implement the correct duration of
        its execution by asking the kernel of its task the speed of
        the processor on whitch it's scheduled.

        We will probably have to derive from this class to implement
        static partition and mixed task allocation to CPU.

        @see absCPUFactory, Scheduler, ResManager, AbsRTTask
    */

    class EnergyMRTKernel : public MRTKernel {

    private:
        struct ConsumptionTable {
            double cons;
            CPU* cpu;
            int opp;
        };

        /**
         * CPU choice from the table of consumptions (not sorted).
         * It tries to spread tasks on CPUs if they have the same energy consumption
         */
        void chooseCPU(AbsRTTask* t, vector<ConsumptionTable> iDeltaPows);
    protected:

        std::vector<CPU*> CPUs;

      /**
       * List of tasks ready on a CPU with a given frequency.
       * Use this variable only before the scheduling finishes ( onBeginDispatchMulti() ), at which point you can use MRTKernel::_m_dispatched.
       */
        std::map<const AbsRTTask *, pair<CPU*, int>> _m_dispatching;

        inline std::vector<CPU*> getProcessors() const { return CPUs; }

        /// Reweigh task instructions WCET after island clock up
        void reweighInstr(Task *t, double oldSpeed, double newSpeed);

        /// in big-little all CPUs in a island have the same freq. Set it to max CPU freq
        void setIslandFrequency(CPU::Island island);

    public:

        /**
          * Kernel with scheduler s and CPUs cpus
          */
        EnergyMRTKernel(Scheduler *s, std::vector<CPU*> cpus , const std::string &name = "");

        /**
           This function is called by the onArrival and by the
            activate function. When we call this, we first select a
            free processor, chosen smartly, then we call the other dispatch,
            in the superclass, specifying on which processor we need to schedule.
         */
        virtual void dispatch();

        /**
            Dispatching on a given CPU.

            This is different from the version we have on RTKernel,
            since we may need to specify on which CPU we have to
            select a new task (for example, in the onEnd() and
            suspend() functions). In the onArrival() function,
            instead, we stil do not know which processor is
            free!
         */
        virtual void dispatch(CPU* c);

        /**
           This function only calls dispatch(CPU*) and assigns a task to a CPU,
           which should be actually done by dispatch(CPU*) itself or onEndDispatchMulti(),
           but the last one is only called after dispatch(), and I need the assignment
           CPU - task to be done before for getTask(CPU*) to work
         */
        void dispatch(CPU *p, AbsRTTask *t, int opp);

        /**
           Returns island utilization given a capacity to scale up/down tasks WCET.
           It also returns the number of tasks being scheduled in the island
        */
        double getIslandUtilization(double capacity, CPU::Island island, int *nTaskIsland);

        /// Returns utilization of task t on CPU c. todo They should be defined for tasks, but this way I can make this implementation private
        double getUtilization(AbsRTTask* t, CPU* c, double capacity) const;

        /// Returns utilization of tasks on CPU c, supposing it runs with given freq and capacity. todo They should be defined for tasks, but this way I can make this implementation private
        double getUtilization(CPU* c, double freq, double capacity) const;

      	virtual void onBeginDispatchMulti(BeginDispatchMultiEvt* e);

        /**
         *  First a task is dispatched, but not executed yet, in the
         *  onBeginDispatchMulti. Then, in the onEndDispatchMulti, its execution starts
         *  on the processor.
         */
        virtual void onEndDispatchMulti(EndDispatchMultiEvt* e);

        /**
         * Invoked when a task ends
         */
        virtual void onEnd(AbsRTTask* t);

        /**
         * Tells whether there exists a task to be scheduled on CPU p
         */
        bool existDispatchingTask(CPU* p);

        /// returns true if we have already decided t's processor (valid before onEndMultiDispatch() completes)
        bool isDispatching(AbsRTTask*);

        /**
         *  Returns a pointer to the task which is executing on given
         *  CPU (NULL if given CPU is idle)
         */
        virtual AbsRTTask* getTask(CPU* c);

        /**
         *  Returns the set of tasks in the runqueu of CPU c
         */
        virtual std::vector<AbsRTTask*> getTasks(CPU* c) const;

        virtual CPU *getProcessor(const AbsRTTask *t) const;

        /// to debug internal functions...
        void test();
    };
}

#endif //RTLIB2_0_ENERGYMRTKERNEL_H
