/***************************************************************************
    begin                : Thu Apr 24 15:54:58 CEST 2003
    copyright            : (C) 2003 by Giuseppe Lipari
    email                : lipari@sssup.it
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SIMPLE_EXAMPLE_MULTI_SCHED_HPP
#define SIMPLE_EXAMPLE_MULTI_SCHED_HPP

#include <string>
#include "entity.hpp"
#include "scheduler.hpp"
#include "taskevt.hpp"
#include "mrtkernel.hpp"
#include <exeinstr.hpp>

namespace RTSim {

    using namespace MetaSim;
    using namespace std;

  /**
     Manages tasks migration among cores, remembering what they executed and how long.
  */
  class MigrationManager {
  protected:
    /// The managed events
    enum EventType { SCHEDULE = 0, DESCHEDULE, WL_CHANGE, SUSPEND, END };

    typedef struct MigrationTaskRow { AbsRTTask* task; Tick tick; enum EventType evt; CPU* cpu; string wl; } MigrationRow;

    /// tasks history table. E.g., Task tt at time t  has got running on CPU c executing bzip2
    ///                            Task tt at time t1 got descheduled
    vector<MigrationTaskRow> _tasks_history;

  private:
    /// Adds an event to the task history
    void addTaskEvent(AbsRTTask* t, Tick when, enum EventType e, CPU* c) {
      MigrationTaskRow r { t, when, e, c, (c == NULL ? "" : c->getWorkload()) };
      _tasks_history.push_back(r);
    }

    string mapEventType(enum EventType e) const;

  public:
    MigrationManager(){};
    ~MigrationManager() { _tasks_history.clear(); };

    void addSchedulingEvent(AbsRTTask* tt, Tick when, CPU* c) {
      string wl = c->getWorkload();
      c->setWorkload(dynamic_cast<ExecInstr*>(dynamic_cast<Task*>(tt)->getInstrQueue().at(0).get())->getWorkload());
      addTaskEvent(tt, when, SCHEDULE, c);
      c->setWorkload(wl);
    }

    void addSupensionEvent(AbsRTTask* tt, Tick when) {
      addTaskEvent(tt, when, SUSPEND, NULL);
    }

    void addDeschedulingEvent(AbsRTTask* tt, Tick when) {
      addTaskEvent(tt, when, DESCHEDULE, NULL);
    }

    void addEndEvent(AbsRTTask* tt, Tick when) {
      addTaskEvent(tt, when, END, NULL);
    }

    /// you should change CPU worload before to call this method
    void addWorloadChangeEvent(AbsRTTask* tt, Tick when, CPU* c) {
      addTaskEvent(tt, when, WL_CHANGE, c);
    }

    vector<MigrationTaskRow> getEventsForTask(AbsRTTask* tt) {
      vector<MigrationTaskRow> rows;
      for (const auto& elem : _tasks_history)
        if (elem.task == tt)
          rows.push_back(elem);
      return rows;
    }

    string toString() const {
      stringstream ss;
      ss << "Tasks migration histories:" << endl;
      ss << "Task\tTick\tEvt\t\tcpu\twl" << endl;

      for (const auto &elem : _tasks_history) {
        ss << taskname(elem.task) << "\t" << double(elem.tick) << 
          "\t" << mapEventType(elem.evt) << "\t" << (elem.cpu == NULL ? "" : elem.cpu->getName()) << "\t" << elem.wl << endl;
      }
      return ss.str();
    }

  }; 

    /**
        \ingroup sched

        An implementation of multi-scheduler.
        Notice it is not a scheduler, but an interface to
        manage cores queues, which are implemented as schedulers.

        The typical scenario is:
        you have N cores and you need a queue per core. Each queue can be
        modelled as a scheduler. Schedulers are supposed to be equal.

        This interface has been introduced for EnergyMRTkernel, managing
        big-littles, where for energetic reasons the scheduler (the kernel in RTSim)
        might decide

        This implementation is general and you can specialize it.

        @see CPU, AbsRTTask
     */
    class MultiScheduler : public MetaSim::Entity {
    protected:
        /// cores queues, ordered according to a scheduling policy
        map<CPU*, Scheduler*> _queues;

        /// running task of a queue, for all queues. Running tasks are also in _queues
        map<CPU*, AbsRTTask*>  _running_tasks;

        /// cores have a queue of ready=dispatching tasks. These are the context switch events
        map<CPU*, BeginDispatchMultiEvt*> _beginEvts;
        map<CPU*, EndDispatchMultiEvt*> _endEvts;

        /// MRTKernel this the queue are related to
        MRTKernel* _kernel;


        /// Post begin event for a task on a core
        void postBeginEvt(CPU* c, AbsRTTask* t, Tick when) {
            postEvt(c, t, when, false);
        }
        /// Post end event for a task on a core
        void postEndEvt(CPU* c, AbsRTTask* t, Tick when) {
            postEvt(c, t, when, true);
        }

        /// Drops all context switch events of task t on core c
        void dropEvt(CPU* c, AbsRTTask* t);

        /// Transition from ready to running for task t on core c
        virtual void makeRunning(AbsRTTask* t, CPU* c) {
            assert(c != NULL); assert(t != NULL);

            Tick when = SIMUL.getTime();
            if (isContextSwitching(c))
              when = _endEvts[c]->getTime();
            dropEvt(c, t);
            postBeginEvt(c, t, when);
        }

        /// Transition from running to ready on core c
        virtual void makeReady(CPU* c) {
            /* - Drop possible end (ctx switch) event. Occurs if oldTask has
             * ctx switch overhead and another, more important task is
             * scheduled during it.
             * - Ready tasks don't have ctx events.
             */
            AbsRTTask *oldTask = getRunningTask(c);
            dropEvt(c, oldTask);

            //rimuovere da shcuedler o modificare getalltasksinqueue
            _running_tasks.erase(c);
        }

        /// Add a task to the scheduler of a core. @see insertTask
        virtual void addTask(AbsRTTask* t, CPU* c, const string& params);
    private:

        /// Posts a context switch event
        void postEvt(CPU* c, AbsRTTask* t, Tick when, bool endevt);

    public:

        MultiScheduler() : Entity("trash multisched"){};

        MultiScheduler(MRTKernel *k, vector<CPU*> &cpus, vector<Scheduler*> &s, const string& name);

        ~MultiScheduler() {
            for (auto c : _queues) {
                vector<AbsRTTask*> tt = getAllTasksInQueue(c.first);
                for (AbsRTTask* t : tt)
                  delete t;
                delete c.second;
            }
        }

        /// Get scheduler of a core
        Scheduler* getScheduler(CPU* c) {
            return _queues[c];
        }

        /// Counts the tasks of a core queue
        unsigned int countTasks(CPU* c);

        /// Empties a core queue
        virtual void empty(CPU* c);

        /// True if the core queue is empty
        bool isEmpty(CPU* c);

        /// Get the task of a core queue
        virtual AbsRTTask* getFirst(CPU* c);

        /// Get running task for core c
        virtual AbsRTTask* getRunningTask(CPU* c) {
            assert(c != NULL);
            return _running_tasks[c];
        }

        /// Get all tasks of a core queue
        vector<AbsRTTask*> getAllTasksInQueue(CPU* c);

        vector<AbsRTTask*> getReadyTasks(CPU* c) {
            assert(c != NULL);

            vector<AbsRTTask*> tasks = getAllTasksInQueue(c);
            AbsRTTask* r = getRunningTask(c);
            for (int i = 0; r != NULL && i < tasks.size(); i++)
                if (tasks.at(i) == r) {
                    tasks.erase(tasks.begin() + i);
                    break;
                }
            return tasks;
        }

        /**
         * Add a task to the queue of a core.
         */
        virtual void insertTask(AbsRTTask* t, CPU* c);

        /// Its CPU if task in dispatched in any queue, else NULL
        virtual CPU* isInAnyQueue(const AbsRTTask* t);

        /// True if CPU is under a context switch (= there is a task in the limbo between beginDispatchMulti - endDispatchMulti)
        bool isContextSwitching(CPU* c) const {
          bool ret = _endEvts.find(c) != _endEvts.end();
          return ret;
        }

        void onBeginDispatchMultiFinished(CPU* c, AbsRTTask* newTask, Tick overhead) {
          assert(c != NULL); assert(newTask != NULL); assert(double(overhead) >= 0.0);

          dropEvt(c, newTask);
          postEndEvt(c, newTask, SIMUL.getTime() + overhead);
        }

        void onEndDispatchMultiFinished(CPU* c, AbsRTTask* t) {
          assert(c != NULL); assert(t != NULL);

          _running_tasks[c] = t;
          dropEvt(c, t);
        }

        /// Kernel signals end of task event (WCET finished)
        void onEnd(CPU* c) {
          assert(c != NULL);

          AbsRTTask *t = getRunningTask(c);
          assert(t != NULL);
          removeFromQueue(c, t);
          makeReady(c);
        }

        /**
           To be called when migration finishes. 
           It deletes ctx switch events on the original core and
           prepares for context switch on the final core (= chosen after migration)
        */
        void onMigrationFinished(AbsRTTask* t, CPU* original, CPU* final) {
            assert(t != NULL); assert(original != NULL); assert(final != NULL);

            try {
// todo
if (taskname(t).find("task9") != string::npos)
    cout<<"";
                removeFromQueue(original, t);
                insertTask(t, final);
                makeRunning(t, final);
            } catch(RTSchedExc &e) {
                insertTask(t, final);
                onMigrationFinished(t,original, final);
            }
        }

        /// Remove the first task of a core queue. Also removes its ctx evt
        virtual void removeFirstFromQueue(CPU* c);

        /// Remove a specific task from a core queue. Also removes its ctx evt
        virtual void removeFromQueue(CPU* c, AbsRTTask* t);

        /// schedule first task of core queue, i.e. posts its context switch event/time
        void schedule(CPU* c) {
        	  assert(c != NULL);
            AbsRTTask *t = getFirst(c);
            if (getRunningTask(c) != NULL && getRunningTask(c) != t)
                makeReady(c);
            if (t != NULL)  // request to schedule on core with no assigned tasks
                 makeRunning(t, c);
        }

        virtual void newRun() {}

        virtual void endRun() {
            for (auto& e : _queues)
                empty(e.first);
        }

        virtual string toString();

    };

} // namespace RTSim
#endif //SIMPLE_EXAMPLE_MULTI_SCHED_HPP
