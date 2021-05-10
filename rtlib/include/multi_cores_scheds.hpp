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

#include <entity.hpp>
#include <mrtkernel.hpp>
#include <scheduler.hpp>
#include <taskevt.hpp>
#include <utils.hpp>
#include <exeinstr.hpp>
#include <string>

#include <edfsched.hpp>
#include <rrsched.hpp>

namespace RTSim {

    using namespace MetaSim;
    using std::map;

    /**
       Manages tasks migration among cores, remembering what they executed and
       how long.
    */
    class MigrationManager {
    protected:
        /// The managed events
        enum EventType { SCHEDULE = 0, DESCHEDULE, WL_CHANGE, SUSPEND, END };

        typedef struct MigrationTaskRow {
            AbsRTTask *task;
            Tick tick;
            enum EventType evt;
            CPU *cpu;
            string wl;
        } MigrationRow;

        /// tasks history table. E.g., Task tt at time t  has got running on CPU
        /// c executing bzip2
        ///                            Task tt at time t1 got descheduled
        vector<MigrationTaskRow> _tasks_history;

    private:
        /// Adds an event to the task history
        void addTaskEvent(AbsRTTask *t, Tick when, enum EventType e, CPU *c) {
            MigrationTaskRow r{t, when, e, c,
                               (c == NULL ? "" : c->getWorkload())};
            _tasks_history.push_back(r);
        }

        string mapEventType(enum EventType e) const;

    public:
        MigrationManager(){};
        ~MigrationManager() {
            _tasks_history.clear();
        };

        void addSchedulingEvent(AbsRTTask *tt, Tick when, CPU *c) {
            string wl = c->getWorkload();
            c->setWorkload(Utils::getTaskWorkload(tt));
            addTaskEvent(tt, when, SCHEDULE, c);
            c->setWorkload(wl);
        }

        void addSupensionEvent(AbsRTTask *tt, Tick when) {
            addTaskEvent(tt, when, SUSPEND, NULL);
        }

        void addDeschedulingEvent(AbsRTTask *tt, Tick when) {
            addTaskEvent(tt, when, DESCHEDULE, NULL);
        }

        void addEndEvent(AbsRTTask *tt, Tick when) {
            addTaskEvent(tt, when, END, NULL);
        }

        /// you should change CPU worload before to call this method
        void addWorloadChangeEvent(AbsRTTask *tt, Tick when, CPU *c) {
            addTaskEvent(tt, when, WL_CHANGE, c);
        }

        vector<MigrationTaskRow> getEventsForTask(AbsRTTask *tt) const {
            vector<MigrationTaskRow> rows;
            for (const auto &elem : _tasks_history)
                if (elem.task == tt)
                    rows.push_back(elem);
            return rows;
        }

        /// Returns true if the task has some migrations
        bool isMigrated(AbsRTTask *t) const {
            vector<MigrationTaskRow> res = getEventsForTask(t);
            return !res.empty();
        }

        string toString() const {
            std::stringstream ss;
            ss << "Tasks migration histories:" << std::endl;
            ss << "Task\tTick\tEvt\t\tcpu\twl" << std::endl;

            for (const auto &elem : _tasks_history) {
                ss << taskname(elem.task) << "\t" << double(elem.tick) << "\t"
                   << mapEventType(elem.evt) << "\t"
                   << (elem.cpu == NULL ? "" : elem.cpu->getName()) << "\t"
                   << elem.wl << std::endl;
            }
            return ss.str();
        }
    };

    /**
        \ingroup sched

        An implementation of multi-scheduler, i.e. every core has a queue,
       implemented as scheduler. Notice it is not a scheduler, but an interface
       to manage cores queues, which are implemented as schedulers. This class
       is also named Multi Cores Queues.

        The typical scenario is:
        you have N cores and you need a queue per core. Each queue can be
        modelled as a scheduler. Schedulers are supposed to be equal!

        This interface has been introduced for EnergyMRTkernel, managing
        big-littles, where for energetic reasons the scheduler (the kernel in
       RTSim) might decide to dispatch many tasks to a certain core.

        Notice that this layer (laid above a multicore kernel) does not
       de/schedule() tasks, it only post begin and end context switches. To
       enforce consistency between kernel and multi-scheduler, kernel needs to
       provide, for each relevant function, task and core where it know that
       core is currently scheduled.

        Before using this layer, please disable (disable()) the kernel
       scheduler. It will still generate onArrival() => dispatch() events, but
       other callbacks will get disabled.

        This implementation is general and you can specialize it.

        @see CPU, AbsRTTask, EnergyMultiCoresScheds
     */
    class MultiCoresScheds : public MetaSim::Entity {
    protected:
        /// cores queues, ordered according to a scheduling policy
        map<CPU *, Scheduler *> _queues;

        /// running task of a queue, for all queues. Running tasks are also in
        /// _queues
        map<CPU *, AbsRTTask *> _running_tasks;

        /// cores have a queue of ready=dispatching tasks. These are the context
        /// switch events
        map<CPU *, BeginDispatchMultiEvt *> _beginEvts;
        map<CPU *, EndDispatchMultiEvt *> _endEvts;

        /// MRTKernel this the queue are related to
        MRTKernel *_kernel;

        // CBS: task -> cpu, virtual time (= time to forget its active
        // utilization), u_active
        map<AbsRTTask *, std::tuple<CPU *, Tick, double>> _active_utilizations;

        /// Post begin event for a task on a core
        void postBeginEvt(CPU *c, AbsRTTask *t, Tick when) {
            postEvt(c, t, when, false);
        }
        /// Post end event for a task on a core
        void postEndEvt(CPU *c, AbsRTTask *t, Tick when) {
            postEvt(c, t, when, true);
        }

        /// Drops all context switch events of task t on core c
        void dropEvt(CPU *c, AbsRTTask *t);

        /// Transition from ready to running for task t on core c
        virtual void makeRunning(AbsRTTask *t, CPU *c) {
            assert(c != NULL);
            assert(t != NULL);

            Tick when = SIMUL.getTime();
            if (isContextSwitching(c))
                when = _endEvts[c]->getTime();
            dropEvt(c, t);
            postBeginEvt(c, t, when);
            std::cout << "t = " << SIMUL.getTime() << ", ctx switch set at "
                      << double(when) << " for " << taskname(t) << std::endl;
        }

        /// Transition from running to ready on core c
        virtual void makeReady(CPU *c) {
            /* - Drop possible end (ctx switch) event. Occurs if oldTask has
             * ctx switch overhead and another, more important task is
             * scheduled during it.
             * - Ready tasks don't have ctx events.
             */
            AbsRTTask *oldTask = getRunningTask(c);
            dropEvt(c, oldTask);

            _running_tasks.erase(c);
            oldTask->deschedule();
            // std::cout << "t = " << SIMUL.getTime() << ", " <<
            // taskname(oldTask) << " descheduled" << std::endl;
        }

        /// Add a task to the scheduler of a core. @see insertTask
        virtual void addTask(AbsRTTask *t, CPU *c, const string &params);

    private:
        /// Posts a context switch event
        void postEvt(CPU *c, AbsRTTask *t, Tick when, bool endevt);

    public:
        MultiCoresScheds() : Entity("trash multisched"){};

        MultiCoresScheds(MRTKernel *k, vector<CPU *> &cpus,
                         vector<Scheduler *> &s, const string &name);

        ~MultiCoresScheds() {
            for (auto c : _queues) {
                vector<AbsRTTask *> tt = getAllTasksInQueue(c.first);
                for (AbsRTTask *t : tt)
                    delete t;
                delete c.second;
            }
        }

        /// Counts the tasks of a core queue
        unsigned int countTasks(CPU *c);

        /// Empties a core queue
        virtual void empty(CPU *c);

        /// True if the core queue is empty (i.e., no ready and running tasks on
        /// c)
        bool isEmpty(CPU *c);

        /// Get scheduler of a core
        Scheduler *getScheduler(CPU *c) {
            return _queues[c];
        }

        /// Get the first (running or ready) task of a core queue
        virtual AbsRTTask *getFirst(CPU *c);

        /// Get the first ready task of a core or NULL if no ready tasks are
        /// available
        virtual AbsRTTask *getFirstReady(CPU *c);

        /// Get processor where task is running
        CPU *getProcessorRunning(const AbsRTTask *t) const {
            for (const auto &elem : _running_tasks)
                if (elem.second == t)
                    return elem.first;
            return NULL;
        }

        /// Get processor where task is ready
        CPU *getProcessorReady(const AbsRTTask *t) const {
            for (auto &elem : _queues) {
                vector<AbsRTTask *> tasks = getAllTasksInQueue(elem.first);
                for (AbsRTTask *task : tasks)
                    if (task == t) {
                        CPU *cpu = elem.first;
                        return cpu;
                    }
            }
            return NULL;
        }

        /// get core where task is dispatched (either running and ready)
        CPU *getProcessor(const AbsRTTask *t) const {
            CPU *cpu = getProcessorRunning(t);
            if (cpu == NULL)
                cpu = getProcessorReady(t);
            return cpu;
        }

        /// Get running task for core c
        virtual AbsRTTask *getRunningTask(CPU *c) {
            assert(c != NULL);
            return _running_tasks[c];
        }

        /// Get all tasks of a core queue
        vector<AbsRTTask *> getAllTasksInQueue(CPU *c) const;

        vector<AbsRTTask *> getReadyTasks(CPU *c) {
            assert(c != NULL);

            vector<AbsRTTask *> tasks = getAllTasksInQueue(c);
            AbsRTTask *r = getRunningTask(c);
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
        virtual void insertTask(AbsRTTask *t, CPU *c);

        /// Its CPU if task in dispatched in any queue, else NULL
        virtual CPU *isInAnyQueue(const AbsRTTask *t);

        /// True if CPU is under a context switch (= there is a task in the
        /// limbo between beginDispatchMulti - endDispatchMulti)
        bool isContextSwitching(CPU *c) const {
            bool ret = _endEvts.find(c) != _endEvts.end();
            return ret;
        }

        void onBeginDispatchMultiFinished(CPU *c, AbsRTTask *newTask,
                                          Tick overhead) {
            assert(c != NULL);
            assert(newTask != NULL);
            assert(double(overhead) >= 0.0);

            dropEvt(c, newTask);
            postEndEvt(c, newTask, SIMUL.getTime() + overhead);
        }

        void onEndDispatchMultiFinished(CPU *c, AbsRTTask *t) {
            assert(c != NULL);
            assert(t != NULL);

            _running_tasks[c] = t;
            dropEvt(c, t);
        }

        /// Kernel signals end of task event (WCET finished)
        void onEnd(AbsRTTask *t, CPU *c) {
            assert(c != NULL);
            assert(t != NULL);

            std::cout << "\t" << c->getName()
                      << " has now wl: " << c->getWorkload()
                      << ", speed: " << c->getSpeed() << std::endl
                      << std::endl;

            // check if there is consistency still
            AbsRTTask *tt = getRunningTask(c);
            assert(tt != NULL && t == tt);

            removeFromQueue(c, t);
            makeReady(c);
        }

        /**
           To be called when migration finishes.
           It deletes ctx switch events on the original core and
           prepares for context switch on the final core (= chosen after
           migration).

           Migration is not specifically meant for big-littles, it can also be
           a task movement between 2 cores. That's why this function is here
        */
        virtual void onMigrationFinished(AbsRTTask *t, CPU *original,
                                         CPU *final) {
            assert(t != NULL);
            assert(original != NULL);
            assert(final != NULL);
            std::cout << "\t\tMCS::" << __func__ << "() migrate "
                      << t->toString() << " from " << original->toString()
                      << " to " << final->toString() << std::endl;

            try {
                removeFromQueue(original, t);
                insertTask(t, final);
                makeRunning(t, final);
            } catch (RTSchedExc &e) {
                insertTask(t, final);
                onMigrationFinished(t, original, final);
            }
        }

        void onExecutingRecharging(CBServer *cbs) {
            // same as onReleasingIdle()
            std::cout << "t=" << SIMUL.getTime() << " MCS::" << __func__
                      << "() for " << cbs->getName() << std::endl;

            forgetU_active(cbs);
        }

        void onExecutingReleasing(CPU *cpu, CBServer *cbs) {
            std::cout << "t=" << SIMUL.getTime() << " MCS::" << __func__
                      << "() for " << cbs->getName() << std::endl;
            assert(cbs != NULL);
            assert(cpu != NULL);

            saveU_active(cpu, cbs);

            std::cout << "\tCBS server has now #tasks="
                      << cbs->getTasks().size()
                      << (cbs->getTasks().size() > 0
                              ? " - first one: " +
                                    cbs->getTasks().at(0)->toString()
                              : "")
                      << std::endl;
        }

        /**
          Callback for CBServer task going from releasing to idle => you can
          forget task active utilization. Returns the core where the CBS server
          was
          */
        CPU *onReleasingIdle(
            CBServer *cbs) { // todo is it better to use
                             // onVirtualTimeReached(CBServer* cbs) and method
                             // remains the same?
            std::cout << "t=" << SIMUL.getTime() << " MCS::" << __func__
                      << "() for " << cbs->getName() << std::endl;

            CPU *c = forgetU_active(cbs);
            assert(c != NULL);
            return c;
        }

        /// cbs recharging itself
        void onReplenishment(CBServer *cbs) {
            std::cout << "t=" << SIMUL.getTime() << " MCS::" << __func__
                      << "() for " << cbs->getName() << std::endl;

            // if (SIMUL.getTime() == cbs->getPeriod())
            forgetU_active(cbs);

            CPU *cpu = getProcessorRunning(cbs);
            if (cpu != NULL) // server might be with no task => already ready
                makeReady(cpu);
        }

        /**
         * Function called only when RRScheduler is used. It informs
         * the queues manager that a task has finished its round => remove from
         * queue and take next task
         */
        void onRound(AbsRTTask *finishingTask, CPU *c) {
            if (c == NULL) // task has just finished its WCET
                return;
            schedule(c);
        }

        /// When task in CBS server ends
        void onTaskInServerEnd(AbsRTTask *t, CPU *cpu, CBServer *cbs) {
            std::cout << "MCS::" << __func__ << "() for " << cbs->getName()
                      << std::endl;
            assert(t != NULL);
            assert(cbs != NULL);
            assert(cpu != NULL);

            // saveU_active (cpu, cbs);
        }

        /// Remove the first task of a core queue. Also removes its ctx evt
        virtual void removeFirstFromQueue(CPU *c);

        /// Remove a specific task from a core queue. Also removes its ctx evt
        virtual void removeFromQueue(CPU *c, AbsRTTask *t);

        /// schedule first task of core queue, i.e. posts its context switch
        /// event/time
        void schedule(CPU *c) {
            assert(c != NULL);
            AbsRTTask *t = getFirst(c);
            // todo rem
            std::cout << __func__ << "() "
                      << (t == NULL ? "" : t->toString() + " on ")
                      << c->getName() << std::endl;

            if (shouldDeschedule(c, t))
                makeReady(c);
            if (shouldSchedule(
                    c, t)) // request to schedule on core with no assigned tasks
                makeRunning(t, c);
        }

        bool shouldDeschedule(CPU *c, AbsRTTask *t);

        bool shouldSchedule(CPU *c, AbsRTTask *t) {
            CBServer *cbs = dynamic_cast<CBServer *>(t);
            if (cbs != NULL && (getRunningTask(c) == t || cbs->isYielding()))
                return false;
            return t != NULL;
        }

        /// Executes next ready task on core c
        void yield(CPU *c) {
            assert(c != NULL); // running task might have already ended

            std::cout << "\tCore status: " << _queues[c]->toString()
                      << std::endl;
            AbsRTTask *nextReady = getFirstReady(c);
            if (nextReady != NULL) {
                // todo remove
                std::cout << "\tYielding in favour of " << nextReady->toString()
                          << std::endl;
                AbsRTTask *runningTask = getRunningTask(c);
                if (runningTask != NULL) {
                    makeReady(c);
                    runningTask->deschedule();
                }
                makeRunning(nextReady, c);
            }
        }

        // ------------------------------------------ Utilizations active

        /// Removes Utilization active that must end CBS server t and returns
        /// core where t was
        CPU *forgetU_active(AbsRTTask *t);

        /// Returns the U_active for tasks "releasing" on CBS server
        double getUtilization_active(CPU *cpu) const {
            assert(cpu != NULL);
            double u_active = 0.0;

            for (const auto &elem : _active_utilizations)
                if (std::get<0>(elem.second) == cpu)
                    u_active += std::get<2>(elem.second);

            assert(u_active >= 0.0);
            return u_active;
        }

        /// Only for debug, get utilization active of task t
        double getUtilization_active(AbsRTTask *t) {
            assert(t != NULL);

            return std::get<2>(_active_utilizations[t]);
        }

        /// Save U_active of an ending task t
        void saveU_active(CPU *cpu, CBServer *cbs) {
            std::cout << "MCS::" << __func__ << "() " << std::endl;
            assert(cbs != NULL);
            assert(cpu != NULL);

            // todo: e se il task e' migrato? allora sommo le utilizzazioni
            // parziali, che e' facile double u_active =
            // double(cbs->getWCET(cpu->getSpeed())) / (double)
            // cbs->getDeadline(); printf("\tu_active = %f/%f (wl: %s, speed:
            // %f)\n", double(cbs->getWCET(cpu->getSpeed())), (double)
            // cbs->getDeadline(), cpu->getWorkload().c_str(), cpu->getSpeed());
            // if (cbs->isKilled()) {
            double u_active =
                double(cbs->getBudget()) /
                (double)
                    cbs->getDeadline(); // todo right? or it applies only in
                                        // case of CBS_ENVELOPING_PER_TASK?
            // printf("\tu_active = %f/%f (wl: %s, speed: %f). Amendment,
            // \'cause task\'s killed\n", double(cbs->getBudget()), (double)
            // cbs->getDeadline(), cpu->getWorkload().c_str(), cpu->getSpeed());
            printf(
                "\tu_active = %f/%f (wl: %s, speed: %f, based on CBS budget)\n",
                double(cbs->getBudget()), (double) cbs->getDeadline(),
                cpu->getWorkload().c_str(), cpu->getSpeed());
            // }

            // a better map is by cpu, but then cpus can collide
            Tick vt = Tick(cbs->getVirtualTime());
            if (double(vt) < double(SIMUL.getTime())) {
                std::cout << "\tvt = " << vt
                          << " <= simul time = " << SIMUL.getTime()
                          << " => skip" << std::endl;
                return;
            }
            _active_utilizations[cbs] = std::make_tuple(cpu, vt, u_active);
            std::cout << "\tadded active utilization for " << cbs->getName()
                      << " cpu " << cpu->toString() << " U_act " << u_active
                      << ", cancel at t="
                      << std::get<1>(_active_utilizations[cbs]) << std::endl;
        }

        void newRun() override {}

        void endRun() override {
            for (auto &e : _queues)
                empty(e.first);
        }

        string toString() const override;
    };

} // namespace RTSim
#endif // SIMPLE_EXAMPLE_MULTI_SCHED_HPP