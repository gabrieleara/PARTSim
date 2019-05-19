/***************************************************************************
    begin                : Thu Apr 24 15:54:58 CEST 2003
    copyright            : (C) 2003 by Giuseppe Lipari
    email                : lipari@sssup.it
 ***************************************************************************/
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
#include "multi_cores_scheds.hpp"
#include "rrsched.hpp"

#define _ENERGYMRTKERNEL_DBG_LEV    "EnergyMRTKernel"
#define EMRTK_LEAVE_LITTLE3_ENABLED 0
#define EMRTK_MIGRATE_ENABLED       0

namespace RTSim {

  class CBServer;

    /**
       Manages tasks migrations among cores and how islands frequency
       has changed over time.

       Expected scenarios - task migration history:
       t. tick evt   core      wl
       t1  0  sched  little_0  bzip
       t1 10  susp
       t1 15  sched  little_3  bzip
       t1 25  cg_wl            encrypt
       t1 50  desch
       t1 80  sched  little2   encrypt

       island frequency history:
       island       tick OPP
       little_island   0   0
       little_island 240  10
       little_island 500  11
    */
    class EnergyMigrationManager : public MigrationManager {
    private:
      typedef struct MigrationCPURow { Island_BL* island; Tick tick; unsigned int opp; } MigrationCPURow;

      /// Remeber Island frequencies over time
      vector<MigrationCPURow> _islands_history;

    protected:
      /// Returns the opp the island had at time tick. And you can infere CPU frequency
      unsigned int getOPPAtTime(Tick tick, Island_BL* island) const {
        unsigned int opp = 0;
        for (const auto& elem : _islands_history)
          if (elem.island == island && elem.tick <= tick) {
            opp = elem.opp;
          }
        return opp;
      }

    public:
      EnergyMigrationManager(vector<Island_BL*> islands) {
        // At the beginning, islands frequency is supposed to be the minimum
        //        for (Island_BL* i : islands)
        //addFrequencyChangeEvent(i, Tick(0), 0);
      }
      ~EnergyMigrationManager() { cout << __func__ << endl; _islands_history.clear(); }

      /// Add an island frequency change event
      void addFrequencyChangeEvent(Island_BL* island, Tick when, unsigned int opp) {
        assert(opp >= 0 && opp < island->getOPPsize());
        if (getOPPAtTime(when, island) == opp) // frequency already recorded
          return;

        MigrationCPURow r = { island, when, opp };
        _islands_history.push_back(r);
      }

      /**
	 Prints island frequencies over time and optinally tasks migrations (if alsoConsumption = true) into a file.

 	 If you need tasks migrations, then pass also them.
	*/
      void dumpToFile(const bool alsoConsumptions = true, vector<AbsRTTask*> tasks = {}, const string filename = "migrationManager.txt") {
        assert (alsoConsumptions && !tasks.empty());

        ofstream stream;
        stream.open(filename);
        stream << toString();

        stream << endl << endl;

        if (alsoConsumptions) {
    double totalConsumption = 0.0;
	  for ( AbsRTTask *t : tasks ) {
            double cons = getConsumption(t, stream);
            totalConsumption += cons;
    }
    stream << endl << endl << "Total tasks consumption = " << totalConsumption << endl;
	}
        
        stream.close();
      }

      /// Returns island frequencies over time and tasks migrations
      string toString() const {
        stringstream ss;
        ss << MigrationManager::toString();

        ss << endl << "Island frequencies over time:" << endl;
        ss << "Island\t\tTick\tFrequency" << endl;
        for (const auto& elem : _islands_history) {
          ss << elem.island->toString() << "\t" << double(elem.tick) << "\t" << elem.island->getFrequency(elem.opp) << endl;
        }
        return ss.str();
      }

      /**
         Gets the total power consumption for a task tt and
         outputs to a stream, if it is != NULL
      */
      double getConsumption(AbsRTTask* tt, ostream& os = cout) const {
        double cons = 0.0;
        os << __func__ << "(" << taskname(tt) << "):" << endl << "\tcons = ";

        // if there is no event event, I suppose there is an error somewhere
        for (int i = 1; i < _tasks_history.size(); i++) {
          int j = i - 1;
          while (j >= 0 && _tasks_history.at(j).task != tt)
            j--;
          if (j < 0) continue; // no corresponding row found for task tt
          const MigrationTaskRow r1 = _tasks_history.at(j); // corresponding previous evt of task of r2
          const MigrationTaskRow r2 = _tasks_history.at(i); // row of considered evt
          if (r1.task != r2.task)
            continue;

          bool shallSum = false;
          switch (r2.evt) {
          case SUSPEND:
            os << "(case SUSPEND) ";
            if (r1.evt == SCHEDULE || r1.evt == WL_CHANGE)
              shallSum = true;
            break;
          case END:
            os << "(case END) ";
            shallSum = true;
            break;
          case SCHEDULE:
            os << "(case SCHEDULE) ";
            // in all cases, cons += 0.0;
            break;
          case DESCHEDULE:
            os << "(case DESCHEDULE) ";
            shallSum = true;
            if (r1.evt == DESCHEDULE)
              shallSum = false;
            break;
          case WL_CHANGE:
            os << "(case WL_CHANGE) ";
            shallSum = true;
            break;
          default:
            os << "default => error";
            cout << "default => error";
            abort();
          }
          
          if (shallSum) {
            string startingWL = r1.cpu->getWorkload();
            r1.cpu->setWorkload(r1.wl);

            CPU_BL *cpu = dynamic_cast<CPU_BL*> (r1.cpu);
            double freq = cpu->getFrequency(getOPPAtTime(r1.tick, cpu->getIsland()));
            cons += double(r2.tick - r1.tick) * cpu->getPowerConsumption(freq);
         
            char buf[100] = "";
            sprintf(buf, "(%ld-%ld)*%f (freq=%ld) + ", long(double(r2.tick)), long(double(r1.tick)), cpu->getPowerConsumption(freq), long(freq));
            os << buf;

            r1.cpu->setWorkload(startingWL);
          }
          else {
            os << "0 + ";
          }

        } // for

        os << " = " << cons << endl;
        os << "\t(a + at the end is normal)" << endl;

        assert(cons >= 0.0);
        return cons;
      }

    };

    /**
        \ingroup sched

        Extension of MultiCoresScheds for Big-Little.
        Tasks are also bound to the desired OPP.

        Notice this is not a scheduler, its a class to manage cores queues
        of ready and running tasks.
     */
    class EnergyMultiCoresScheds : public MultiCoresScheds {
    private:
        /// OPP needed by tasks
        map<AbsRTTask*, pair<CPU_BL*, unsigned int>> _opps;

      //      multimap<AbsRTTask*, pair<Tick, CPU_BL*> > _tasks_history; // Tasks history: task tt run on core c at time t

    public:
        EnergyMultiCoresScheds(MRTKernel* kernel, vector<CPU*> &cpus, vector<Scheduler*> &s, const string& name);

        ~EnergyMultiCoresScheds() { cout << __func__ << endl; }

        /**
         * Add a task to the queue of a core. Use instead
         * of addTask if you don't need specific task parameters
         */
        virtual void insertTask(AbsRTTask* t, CPU_BL* c, int opp) {
            assert(opp >= 0 && opp < c->getIsland()->getOPPsize());
            MultiCoresScheds::insertTask(t, c);
            _opps[t] = make_pair(c, opp);
        }

        /// Remove the first task of a core queue
        virtual void removeFirstFromQueue(CPU_BL* c) {
            AbsRTTask *t = getFirst(c);
            removeFromQueue(c, t);
        }

        /// Remove a specific task from a core queue
        virtual void removeFromQueue(CPU_BL* c, AbsRTTask* t) {
            MultiCoresScheds::removeFirstFromQueue(c);
            _opps.erase(t);
            assert(_opps.find(t) == _opps.end());
        }

        /// Empties a core queue
        virtual void empty(CPU_BL* c) {
            vector<AbsRTTask*> tasks = getAllTasksInQueue(c);
            MultiCoresScheds::empty(c);
            for (AbsRTTask* t : tasks)
                _opps.erase(t);
        }

        virtual void makeRunning(AbsRTTask* t, CPU_BL* c) {
            MultiCoresScheds::makeRunning(t, c);
            c->setOPP(getOPP(c));
        }

        virtual void endRun() {
            MultiCoresScheds::endRun();
            _opps.clear();
        }

        virtual unsigned int getOPP(CPU_BL* c) {
            unsigned int maxOPP = 0;
            for (const auto& e : _opps)
              if ( e.second.first->getName() == c->getName() && e.second.second > maxOPP)
                maxOPP = e.second.second;
            return maxOPP;
        }

        string toString(CPU_BL* c) {
            vector<AbsRTTask*> tasks = getAllTasksInQueue(c);
            stringstream ss;
            int i = 1;
            for (AbsRTTask* t : tasks)
                ss << "\t" << i++ << ") " << t->toString();
            return ss.str();
        }

        virtual string toString() {
            stringstream ss;
            ss << "EnergyMultiCoresScheds:" << endl;
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

        bool _tryingTaskOnCPU_BL;

        /// The energy migration manager/recorder, recording tasks movements and cpu frequencies over time
        EnergyMigrationManager _e_migration_manager;

        /// cores queues, containing ready and running tasks for each core
        EnergyMultiCoresScheds *_queues;

        /// for debug, if you want to force a certain choice of cores and frequencies and how many times to dispatch
        map<AbsRTTask*, tuple<CPU_BL*, unsigned int, unsigned int>> _m_forcedDispatch;

        /// island cores load balancing policy: if possible, make all island cores work
        void balanceLoad(CPU_BL **chosenCPU, unsigned int &chosenOPP, bool &chosenCPUchanged, vector<struct ConsumptionTable> iDeltaPows);

        /**
        * CPU_BL choice from the table of consumptions (not sorted).
        * It tries to spread tasks on CPU_BLs if they have the same energy consumption
        */
        void chooseCPU_BL(AbsRTTask* t, vector<ConsumptionTable> iDeltaPows);

        Island_BL* getIsland(IslandType island) const { return _islands[island]; }

        /**
           Tells if a task is to be descheduled on a CPU

           This method has been introduced for RRScheduler, which needs to tell wheather a
           task has finished its quantum
        */
        bool isToBeDescheduled(CPU_BL* p, AbsRTTask *t);

        /**
         * Implements the policy of leaving little 3 free, just in case a task with high WCET arrives,
         * risking to be forced to schedule it on big cores, increasing power consumption.
         */
        void leaveLittle3(AbsRTTask *t, std::vector<ConsumptionTable> iDeltaPows, CPU_BL*& chosenCPU_BL);

        /// Implements migration mechanism on task end
        void migrate(CPU_BL* endingCPU_BL);


        /**
           Tries to schedule a task on a CPU_BL, for all valid OPPs,
           remembering power consumption
        */
        void tryTaskOnCPU_BL(AbsRTTask *t, CPU_BL *c, vector<struct ConsumptionTable> &iDeltaPows);

        /// needed for onOPPChanged()
        void setTryingTaskOnCPU_BL(bool b) { _tryingTaskOnCPU_BL = b; }

        /// needed for onOPPChanged()
        bool isTryngTaskOnCPU_BL() { return _tryingTaskOnCPU_BL; }

    public:

        /**
          * Kernel with scheduler s and CPU_BLs CPU_BLs.
          * qs are the schedulers you want for MultiCoresScheds, the queues of cores.
          *
          * @see MultiCoresScheds
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
	Scheduler* getScheduler() { return _sched; }

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

        /// Tells where a task has been dispatched (when it's in the limbo
        /// between onBeginDispatchMulti and onEndDispatchMulti). Similar to getProcessor()
        CPU_BL* getDispatchingProcessor(const AbsRTTask* t);

        /// Returns tasks to be dispatched for the used scheduler
        void getNewTasks(vector<AbsRTTask*> tasks, int& num_newtasks);

        /// Get all processors, in all islands
        vector<CPU_BL*> getProcessors() const { 
            vector<CPU_BL*> CPU_BLs;
            for (CPU_BL* c : getIslandLittle()->getProcessors())
                CPU_BLs.push_back(c);
            for (CPU_BL* c : getIslandBig()->getProcessors())
                CPU_BLs.push_back(c);
            return CPU_BLs;
        }

        /// Get processors of an island
        vector<CPU_BL*> getProcessors(IslandType island) const {
            return getIsland(island)->getProcessors();
        }

        /// For debug. Returns the layer managing CPUs queues/schedulers
        EnergyMultiCoresScheds* getEnergyMultiCoresScheds() const {
          return _queues;
        }

        /**
           Returns island utilization given a capacity to scale up/down tasks WCET.
           It also returns the number of tasks being scheduled in the island
        */
        double getIslandUtilization(double capacity, IslandType  island, int *nTaskIsland);

        /// Returns utilization of task t on CPU_BL c. This method could be defined for tasks, but this way I can make this implementation private
        double getUtilization(AbsRTTask* t, double capacity) const;

        /// Returns utilization of tasks on CPU_BL c, supposing it runs with given freq and capacity. This method could be defined for tasks, but this way I can make this implementation private
        double getUtilization(CPU_BL* c, double capacity) const;

        double getUtilization_active(CPU_BL* c) const;

        /// Dumps cores frequencies over time and (if alsoConsumption=true) also tasks migrations into a file. If filename="", migrationManager.txt is chosen
      void dumpPowerConsumption(bool alsoConsumptions = true, vector<AbsRTTask*> tasks = {}, const string& filename = "") {
          if (filename == "")
            _e_migration_manager.dumpToFile(alsoConsumptions, tasks);
          else
            _e_migration_manager.dumpToFile(alsoConsumptions, tasks, filename);
        }

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

        void onExecutingReleasing(AbsRTTask *t, CPU* cpu, CBServer* cbs) {
          cout << "EMRTK::" << __func__ << "()" << endl;
          _queues->onExecutingReleasing(t, cpu, cbs);
        }

        void onReleasingIdle(CBServer *cbs) {
          cout << "EMRTK::" << __func__ << "()" << endl;
          _queues->onReleasingIdle(cbs);
        }

      /**
       * Specifically called when RRScheduler is used, it informs the kernel that
       * finishingTask has just finished its round (slice)
       */
        void onRound(AbsRTTask* finishingTask);


        /// returns true if we have already decided t's processor (valid before onEndMultiDispatch() completes)
        bool isDispatching(AbsRTTask*);

        /// is any task dispatched on CPU_BL p?
        bool isDispatching(CPU_BL* p);

        /**
         *  Returns a pointer to the task which is executing on given
         *  CPU_BL (NULL if given CPU_BL is idle)
         */
        virtual AbsRTTask* getRunningTask(CPU* c);

         /// Returns the set of tasks in the runqueue of CPU_BL c, but the runnning one, ordered by DL (300, 400, ...)
        virtual vector<AbsRTTask*> getReadyTasks(CPU_BL* c) const;

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

        static double time();

        void printMap();

        void printBool(bool b);

        virtual void printState();

        bool manageForcedDispatch(AbsRTTask*);

        void addForcedDispatch(AbsRTTask *t, CPU_BL *c, unsigned int opp, unsigned int times = 1);
    };
}

#endif //RTLIB2_0_ENERGYMRTKERNEL_H
