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

#ifndef SIMPLE_EXAMPLE_MULTI_SCHED_H
#define SIMPLE_EXAMPLE_MULTI_SCHED_H

#include <string>
#include "entity.hpp"
#include "scheduler.hpp"
#include "taskevt.hpp"
#include "mrtkernel.hpp"

namespace RTSim {

    using namespace MetaSim;
    using namespace std;

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


        /// Post begin/end event
        void postBeginEvt(CPU* c, AbsRTTask* t, Tick when) {
            postEvt(c, t, when, true);
        }
        void postEndEvt(CPU* c, AbsRTTask* t, Tick when) {
            postEvt(c, t, when, false);
        }

        /// Drops all context switch events of task t on core c
        void dropEvt(CPU* c, AbsRTTask* t);

        /// Transition from ready to running for task t on core c
        virtual void makeRunning(AbsRTTask* t, CPU* c) {
            _running_tasks[c] = t;
            postBeginEvt(c, t, SIMUL.getTime());
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

            _running_tasks.erase(c);
        }

    private:
        /// Posts a context switch event
        void postEvt(CPU* c, AbsRTTask* t, Tick when, bool endevt);

    public:

        MultiScheduler(MRTKernel *k, vector<CPU*> cpus, vector<Scheduler*> s, const string& name);

        ~MultiScheduler() {
            for (auto c : _queues)
                delete c.second;
        }

        /// Get scheduler of a core
        Scheduler* getScheduler(CPU* c) {
            return _queues[c];
        }

        /// Add a task to the queue of a core
        virtual void addTask(AbsRTTask* t, CPU* c, const string& params);

        /*
         * Add a task to the queue of a core. Use instead
         * of addTask if you don't need specific task parameters
         */
        virtual void insertTask(AbsRTTask* t, CPU* c);

        /// Remove the first task of a core queue
        virtual void removeFirstFromQueue(CPU* c);

        /// Remove a specific task from a core queue
        virtual void removeFromQueue(CPU* c, AbsRTTask* t);

        /// Get the task of a core queue
        virtual AbsRTTask* getFirst(CPU* c);

        /// Its CPU if task in dispatched in any queue, else NULL
        virtual CPU* isInAnyQueue(const AbsRTTask* t);

        /// Get running task for core c
        virtual AbsRTTask* getRunningTask(CPU* c) {
            return _running_tasks[c];
        }

        /// Get all tasks of a core queue
        vector<AbsRTTask*> getAllTasksInQueue(CPU* c);

        vector<AbsRTTask*> getReadyTasks(CPU* c) {
            vector<AbsRTTask*> tasks = getAllTasksInQueue(c);
            AbsRTTask* r = getRunningTask(c);
            for (int i = 0; r != NULL && i < tasks.size(); i++)
                if (tasks.at(i) == r) {
                    tasks.erase(tasks.begin() + i);
                    break;
                }
            return tasks;
        }

        /// pop (= get & remove) the first task of a core queue
        virtual AbsRTTask* popFromQueue(CPU* c) {
            AbsRTTask* t = getFirst(c);
            removeFirstFromQueue(c);
            return t;
        }

        /// Empties a core queue
        virtual void empty(CPU* c);

        /// True if the core queue is empty
        bool isEmpty(CPU* c);

        /// Counts the tasks of a core queue
        unsigned int countTasks(CPU* c);

        /// schedule first task of core queue
        void schedule(CPU* c) {
            if (getRunningTask(c) != NULL)
                makeReady(c);
            makeRunning(getFirst(c), c);
        }

        void onBeginDispatchMultiFinished(CPU* c, AbsRTTask* newTask, Tick overhead) {
            dropEvt(c, newTask);
            postEndEvt(c, newTask, SIMUL.getTime() + overhead);
        }

        void onEndDispatchMultiFinished(CPU* c, AbsRTTask* t) {
            dropEvt(c, t);
        }

        virtual void newRun() {}

        virtual void endRun() {
            for (auto& e : _queues)
                empty(e.first);
        }

    };

}
#endif //SIMPLE_EXAMPLE_MULTI_SCHED_H
