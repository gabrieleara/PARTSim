 /***************************************************************************
 *   begin                : Thu Apr 24 15:54:58 CEST 2003
 *   copyright            : (C) 2003 by Agostino Mascitti
 *   email                : a.mascitti@sssup.it
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "energyMRTKernel.hpp"
#include "task.hpp"
#include "scheduler.hpp"
#include <assert.h>
#include "rttask.hpp"
#include "exeinstr.hpp"
#include "cpu.hpp"

namespace RTSim {
    using namespace MetaSim;

    EnergyMRTKernel::EnergyMRTKernel(vector<Scheduler*> &qs, Scheduler *s, Island_BL* big, Island_BL* little, const string& name)
      : MRTKernel(s, big->getProcessors().size() + little->getProcessors().size(), name), _e_migration_manager({big, little}) {
        setIslandBig(big); setIslandLittle(little);

        for(CPU_BL* c : getProcessors())  {
            _m_currExe[c] = NULL;
            _isContextSwitching[c] = false;
        }

        _sched->setKernel(this);
        setTryingTaskOnCPU_BL(false);

        // todo is there a shorter solution to pass directly vector<CPU_BL*> to EMS?
        vector<CPU_BL*> cpus = getProcessors();
        vector<CPU*> v;
        for (CPU_BL* c : cpus)
            v.push_back((CPU*) c);

        _queues = new EnergyMultiScheduler(this, v, qs, "energymultischeduler");
    }

    EnergyMultiScheduler::EnergyMultiScheduler(MRTKernel *kernel, vector<CPU*> &cpus, vector<Scheduler*> &s, const string& name)
        : MultiScheduler(kernel, cpus, s, name) { }

    AbsRTTask* EnergyMRTKernel::getRunningTask(CPU* c) {
        AbsRTTask* t = _queues->getRunningTask(c);
        return t;
    }

    vector<AbsRTTask*> EnergyMRTKernel::getReadyTasks(CPU_BL* c) const {
        vector<AbsRTTask*> t = _queues->getReadyTasks(c);
        return t;
    }

    double EnergyMRTKernel::getUtilization(AbsRTTask* task, CPU_BL* c, double capacity) const {
        double util = ceil(task->getRemainingWCET(capacity)) / double(task->getDeadline());

#include <cstdio>
        printf("\t\t\tgetUtilization of considered task %f/%f (capacity=%f)=%f\n",task->getRemainingWCET(capacity), double(task->getDeadline()), capacity, util);
        return util;
    }

    CPU_BL *EnergyMRTKernel::getDispatchingProcessor(const AbsRTTask *t) {
        CPU_BL* c = dynamic_cast<CPU_BL*>(_queues->isInAnyQueue(t));
        if (_queues->getRunningTask(c) == t)
            c = NULL;

        return c;
    }

    bool EnergyMRTKernel::isDispatching(AbsRTTask* t) {
        return _queues->isInAnyQueue(t) != NULL;
    }

    bool EnergyMRTKernel::isDispatching(CPU_BL *p) {
      return !_queues->isEmpty(p);
    }

    double EnergyMRTKernel::getUtilization(CPU_BL* c, double freq, double capacity) const {
        double utilization = 0.0;
        vector<AbsRTTask*> ths = getReadyTasks(c);
        AbsRTTask* runningTask = const_cast<EnergyMRTKernel*>(this)->getRunningTask(c);
        if (runningTask != NULL)
            ths.push_back(runningTask);

        for (AbsRTTask* th : ths) {
            utilization += ceil(th->getRemainingWCET(capacity)) / double(th->getDeadline());
            cout << "\t\t\tUtilization task already in CPU, " << th->toString() << ", is "
                 << ceil(th->getWCET(capacity)) << "/" << th->getDeadline() << " = "
                 << ceil(th->getRemainingWCET(capacity)) / double(th->getDeadline())
                 <<  " - CPU capacity=" << capacity << endl;
            cout << "\t\t\t\ttask WCET " << ceil(th->getRemainingWCET(capacity)) << " DL "
                 << th->getDeadline() << endl;
        }

        return utilization;
    }

    double EnergyMRTKernel::getIslandUtilization(double capacity, IslandType island, int *nTasksIsland) {
        double utilizationIsland = 0.0;

        for (CPU_BL* c1 : getProcessors(island)) {
            vector<AbsRTTask*> tasks = getReadyTasks(c1);
            AbsRTTask* runningTask = getRunningTask(c1);
            if (runningTask != NULL) {
                utilizationIsland += ceil(runningTask->getRemainingWCET(capacity)) / double(runningTask->getDeadline());
                if (nTasksIsland != NULL)
                    *nTasksIsland = *nTasksIsland + 1;
            }
            for (AbsRTTask* th : tasks) {
                if (getDispatchingProcessor(th)->getIslandType() == c1->getIslandType() ||
                    dynamic_cast<CPU_BL*>(getProcessor(th))->getIslandType() == c1->getIslandType()) {
                    utilizationIsland += ceil(th->getRemainingWCET(capacity)) / double(th->getDeadline());
                    if (nTasksIsland != NULL)
                        *nTasksIsland = *nTasksIsland + 1;
                }
            }
        }

        return utilizationIsland;
    }

    void EnergyMRTKernel::onOppChanged(unsigned int curropp, Island_BL* island) {
        if (isTryngTaskOnCPU_BL())
            return;

        cout << __func__ << endl;
        _e_migration_manager.addFrequencyChangeEvent(island, SIMUL.getTime(), curropp);
        // scale wcets of tasks on the island
    }


    // ----------------------------------------------------------- testing

    void EnergyMRTKernel::test() {
        // PeriodicTask T7_task0 DL = T 500 WCET(abs) 63 in CPU LITTLE_0 freq 500 freq 3
        Task *t = dynamic_cast<Task*>(_sched->getTaskN(0));
        CPU_BL* p;
        dispatch(p, t, 3);
        p->setWorkload("bzip2");
        cout << "CPU is " << p->toString() << " freq " << p->getFrequency()<< " "<< t->toString() << endl;

        int nTaskIsland = 0;
        //cout << "task util " << getIslandUtilization(p->getSpeed(500.0), p->getIslandType(), &nTaskIsland);

        exit(0);
    }

    // for gdb
    void EnergyMRTKernel::printMap() {
        cout << _queues->toString();
    }

    // for gdb
    void EnergyMRTKernel::printBool(bool b) {
        cout << b << endl;
    }

    void EnergyMRTKernel::printState() {
      for (CPU_BL *c : getProcessors()) {
          AbsRTTask *t = _queues->getRunningTask(c);
          cout << c->getName() << ": " << (t == NULL ? "0" : taskname(t)) << "\t";
      }
      cout << endl;
    }

    void EnergyMRTKernel::addForcedDispatch(PeriodicTask* t, CPU_BL* c, unsigned int opp) {
        _m_forcedDispatch[t] = make_tuple(c, opp);
    }

    // for gdb
    bool EnergyMRTKernel::manageForcedDispatch(Task* t) {
        if (_m_forcedDispatch.find(t) != _m_forcedDispatch.end() ) { //&& get<2>(_m_forcedDispatch[t]) == SIMUL.getTime()) {
            cout << __func__ << endl;
            dispatch(get<0>(_m_forcedDispatch[t]), t, get<1>(_m_forcedDispatch[t]));
            _m_forcedDispatch.erase(t);
            return true;
        }
        return false;
    }

    double EnergyMRTKernel::time() {
        return double(SIMUL.getTime());
    }

    // --------------------------------------------------------------- context switch

    // Note MRTKernel version differs: dispatch() tasks a free CUP and calls onBDM(), which in turns
    // assigns a task. EnergyMRTKernel, instead, needs to make assignment decisions: dispatch() chooses
    // a CPU for all tasks, and on*DM() makes the context switch (split into onBDM() and onEBM(), as in MRTKernel)
    void EnergyMRTKernel::onBeginDispatchMulti(BeginDispatchMultiEvt* e) {
        DBGENTER(_KERNEL_DBG_LEV);

        CPU_BL    *p    = dynamic_cast<CPU_BL*>(e->getCPU());
        AbsRTTask *dt   = _queues->getRunningTask(p);
        AbsRTTask *st   = e->getTask();
        assert(st != NULL); assert(p != NULL);

        cout << endl << "time =" << SIMUL.getTime() << " EnergyMRTKernel::onBeginDispatchMulti() for " << taskname(st) << " on " << p->toString() << endl
          ;
        if ( st != NULL && dt == st ) {
            stringstream ss;
            ss << "Decided to dispatch " << st->toString() << " on its former CPU => skip context switch";
            DBGPRINT(ss.str());
            cout << ss.str() << endl;
            return;
        }
        // if necessary, deschedule the task.
        if ( dt != NULL ) {
            _m_oldExe[dt] = p;
            _m_currExe[p] = NULL;
            dt->deschedule();
            _e_migration_manager.addDeschedulingEvent(dt, SIMUL.getTime());
            cout << dt->toString() << " descheduled for " << taskname(st) <<endl;
        }

        DBGPRINT_4("Scheduling task ", taskname(st), " on cpu ", p->toString());
        cout << __func__ << " Scheduling task " << taskname(st) << " on cpu " << p->toString() << endl;

        //_endEvt[p]->setTask(st);
        _isContextSwitching[p] = true;
        // if you exit(0) here, dispatch() has already chosen a CPU forall tasks
        // exit(0);
        Tick overhead (_contextSwitchDelay);
        CPU_BL* oldProcessor = dynamic_cast<CPU_BL*>(getOldProcessor(st));
        if (st != NULL && oldProcessor != p && oldProcessor != NULL)
            overhead += _migrationDelay;
        
        _queues->onBeginDispatchMultiFinished(p, st, overhead);

        cout << taskname(st) << " end ctx switch set at t=" << SIMUL.getTime() + overhead << endl;

    }

    // Called after dispatch(), i.e. after choosing a CPU forall arrived tasks.
    // Also called when a periodic task ends its WCET
    void EnergyMRTKernel::onEndDispatchMulti(EndDispatchMultiEvt* e) {
        AbsRTTask* t      = e->getTask();
        CPU_BL* cpu       = dynamic_cast<CPU_BL*>(e->getCPU());
        assert (t != NULL); assert(cpu != NULL);
        
        cout << endl << "time =" << SIMUL.getTime() << " EnergyMRTKernel::onEndDispatchMulti() for " << taskname(t) << " on " << cpu->toString() << endl;
        _queues->onEndDispatchMultiFinished(cpu,t);
        MRTKernel::onEndDispatchMulti(e);

        // use case: 2 tasks arrive at t=0 and are scheduled on big 0 and big 1 freq 1100.
        // Then, at time t=100, another task arrives and the algorithm decides to schedule it on big 2 freq 2000.
        // Thus, big island freq is 2000, not 1100 (the max). todo: maybe it's useless to have these instructions below
        unsigned int opp = _queues->getOPP(cpu);
        if (opp > cpu->getOPP()) {
          cout << t->toString() << " " << cpu->toString() << " updating opp to " << opp << endl;
          cpu->setOPP(opp);
        }
        else // otherwise you have to same freq change events
          _e_migration_manager.addFrequencyChangeEvent(cpu->getIsland(), SIMUL.getTime(), opp);

        _e_migration_manager.addSchedulingEvent(t, SIMUL.getTime(), cpu);
        _m_oldExe[t] = cpu;

        //todo remove
        _queues->toString();

        // If you exit(0) here, trace.txt arrives 'til [Time:0]	T6_task4 arrived at 0.
        // ExecInstr::schedule() is called after each task's onEndDispatchMulti()
        // exit(0);
    }



    // ----------------------------------------------------------- task ends WCET

    void EnergyMRTKernel::onEnd(AbsRTTask* t) {
        DBGENTER(_KERNEL_DBG_LEV);

        // only on big-little: update the state of the CPUs island
        CPU_BL* p = dynamic_cast<CPU_BL*>(getProcessor(t));
        DBGPRINT_6(t->toString(), " has just finished on ", p->toString(), ". Actual time = [", SIMUL.getTime(), "]");
        cout << ".............................." << endl;
        cout << t->toString() << " has just finished on " << p->toString() << ". Actual time = [" << SIMUL.getTime() << "]" << endl;

        vector<CPU_BL*> cp = getProcessors(p->getIslandType());

        _sched->extract(t);
        _m_oldExe[t] = p;
        _m_currExe.erase(p);
        _m_dispatched.erase(t);
        _queues->onEnd(p);
        _e_migration_manager.addEndEvent(t, SIMUL.getTime());

        if (!isDispatching(p) && getRunningTask(p) == NULL)
            p->setBusy(false);

        if (!p->isBusy())
            migrate(p);
        else // core has already some ready tasks
            _queues->schedule(p);

        if (!p->getIsland()->isBusy()) {
            cout << p->getIsland()->getName() << "'s got free => clock down to min speed" << endl;
            p->getIsland()->setOPP(0);
        }

        printState();

    }

    void EnergyMRTKernel::migrate(CPU_BL* endingCPU) {
        /**
           Migration mechanism: a task finishes on CPU c, leaving it idle.
           If c is a little, try moving ready tasks originally assigned to
           big island. If you can't find any, then balance load on little island.
           If c is a big core, balance load on big island.

           Try not to touch running tasks.
        */
        if (!EMRTK_MIGRATE_ENABLED) { cout << "Migration policy disabled => skip" << endl; return; }
        if (getReadyTasks(endingCPU).size() != 0) { cout << endingCPU->getName() << " already has some ready task => skip migration" << endl; return; }
        cout << "\t" << __func__ << "() time=" << SIMUL.getTime() << endl;

        vector<AbsRTTask*> readyTasks;
        if (endingCPU->getIslandType() == IslandType::LITTLE) {
            for (CPU_BL * c : getIslandBig()->getProcessors()) {
                readyTasks = getReadyTasks(c);
                for (AbsRTTask * tt : readyTasks) {
                    vector<struct ConsumptionTable> iDeltaPows;
                    tryTaskOnCPU_BL(tt, endingCPU, iDeltaPows);
                    if (!iDeltaPows.empty()) {
                        cout << "\tMigrating " << taskname(tt) << " from " << c->toString() << " to " << iDeltaPows.at(0).cpu->toString() << " with frequency " << endingCPU->getFrequency(iDeltaPows.at(0).opp) << endl;                        
                        _queues->onMigrationFinished(tt, c, endingCPU);
                        // onEndDispatchMulti will take care of increasing core OPP
                        return;
                    }
                }
            }
            cout << "\tNo migration from big island to endingCPU => balance little island load" << endl;
        }

        if (endingCPU->getIslandType() == IslandType::BIG)
            cout << "\tBalancing on big island load" << endl;
        
        // Take a ready task of the island and put it into endingCPU.
        // However, if core has only ready tasks, then such task, shouldn't
        // be the only ready one for its original core (why to move it in that case?)
        for (CPU_BL * c : getProcessors(endingCPU->getIslandType())) {
            readyTasks = getReadyTasks(c);
            unsigned int nTasksOnCore = readyTasks.size() + (getRunningTask(c) == NULL ? 0 : 1);
            if (nTasksOnCore > 1) {
                AbsRTTask *tt = readyTasks.at(0);
                cout << "\tMigrating " << taskname(tt) << " from " << c->toString() << " to " << endingCPU->toString() << " with same frequency " << endl;
                _queues->onMigrationFinished(tt, c, endingCPU); // todo add migration overhead
                break;
            }
        }
         
    } // end EMRTK::migrate()


    // ----------------------------------------------------------- choosing cores for tasks

    void EnergyMRTKernel::leaveLittle3(AbsRTTask* t, vector<struct ConsumptionTable> iDeltaPows, CPU_BL*& chosenCPU) {
        /**
         * Policy of leaving a little core free for big-WCET tasks, which otherwise would be scheduled on
         * big cores, thus increasing power consumption.
         * Mechanism: if you want to put a task on little 3, but it also fits in another little core,
         * put it in the other little. Otherwise (if it only fits on little 3 and in bigs, choose little 3).
         */

        cout << __func__ << "():" << endl;
        if (!EMRTK_LEAVE_LITTLE3_ENABLED) { cout << "\tPolicy deactivated. Skip"<<endl; return; }
        if (chosenCPU->getName().find("LITTLE_3") == string::npos || chosenCPU->getIslandType() == IslandType::BIG) {
            cout << "chosenCPU in big island or is not little_3 => skip" << endl;
            return;
        }

        bool fitsInOtherCore = true; // if it only fits on little 3 and in bigs

        for (int i = 0; i < iDeltaPows.size(); i++) {
            cout << iDeltaPows[i].cons << " " << iDeltaPows[i].cpu->toString() << endl;
            if (iDeltaPows[i].cpu->getIslandType() == IslandType::LITTLE && iDeltaPows[i].cpu->getName().find("LITTLE_3") == string::npos) {
                chosenCPU = iDeltaPows[i].cpu;
                chosenCPU->setOPP(iDeltaPows[i].opp);
                cout << "Changing to " << iDeltaPows[i].cpu->toString() << " - chosenCPU=" << chosenCPU->toString() << endl;
                fitsInOtherCore = false;
                break;
            }
        }

        if (fitsInOtherCore) {
            cout << "Task only fits on little 3 and in bigs => stay in LITTLE_3, CPU not changed" << endl;
        }
    }

    void EnergyMRTKernel::balanceLoad(CPU_BL **chosenCPU, unsigned int &chosenOPP, bool &chosenCPUchanged, vector<struct ConsumptionTable> iDeltaPows) {
        // Load balancing policy: don't put all the load on a core of an island, but use also the others.
        // Mechanism: if chosen CPU is busy, find a free CPU in the island with the same consumption.
        // Note: with this algorithm tasks cannot be assigned to a core in an island
        // different than the originally chosen one
        cout << __func__ << ":" << endl;
        if ( (*chosenCPU)->isBusy() ) {
            cout << (*chosenCPU)->toString() << " was chosen but it's busy" << endl;
            for (int i = 1; i < iDeltaPows.size(); i++) {
                cout << iDeltaPows[i].cons << " VS " << iDeltaPows[0].cons << " busy? "
                     << iDeltaPows[i].cpu->isBusy() << " " << iDeltaPows[i].cpu->toString() << endl;
                if (iDeltaPows[i].cons == iDeltaPows[0].cons && !iDeltaPows[i].cpu->isBusy()) {
                    *chosenCPU = iDeltaPows[i].cpu;
                    chosenOPP = iDeltaPows[i].opp;
                    chosenCPUchanged = true;
                    break;
                }
            }
        }
        else cout << "\tCPU is not busy => skip" << endl;
    }

    void EnergyMRTKernel::chooseCPU_BL(AbsRTTask* t, vector<struct ConsumptionTable> iDeltaPows) {
        // TODO: switch from nlogn to n for min()
        // sort table of consumption
        sort(iDeltaPows.begin(), iDeltaPows.end(),
             [] (struct ConsumptionTable const& e1, struct ConsumptionTable const& e2) { return e1.cons < e2.cons; });

        // todo delete after debug
        for (auto elem: iDeltaPows) {
            cout << elem.cons << " "<< elem.cpu->toString() << " " << elem.cpu->getFrequency(elem.opp) << endl;
        }

        struct ConsumptionTable chosen = iDeltaPows[0];
        CPU_BL* chosenCPU = chosen.cpu;
        unsigned int chosenOPP = chosen.opp;
        bool chosenCPUchanged = false;
        bool toBeChanged = chosenCPU->isBusy();

        balanceLoad(&chosenCPU, chosenOPP, chosenCPUchanged, iDeltaPows);

        cout << "Temporarily chosenCPU: " << chosenCPU->toString() << " with freq " << chosenCPU->getFrequency(chosenOPP) << endl;
        leaveLittle3(t, iDeltaPows, chosenCPU);

        cout << "time = " << SIMUL.getTime() << " - going to schedule task " << t->toString() << " in CPU " << chosenCPU->getName() <<
             " with freq " << chosenCPU->getFrequency(chosenOPP) << " speed=" << chosenCPU->getSpeed(chosenOPP) <<
             " chosenOPP " << chosenOPP << " - CPU" << (chosenCPUchanged && toBeChanged ? "":" not") << " changed "  << endl;
        dispatch(chosenCPU, t, chosenOPP);
    }

    void EnergyMRTKernel::dispatch(CPU *p, AbsRTTask *t, int opp) {
        cout << __func__ << " " << t->toString() << endl;
        CPU_BL* pp = dynamic_cast<CPU_BL*>(p);
        // This variable is only needed before the scheduling finishes (onBegin/onEndDispatchMulti())
        _queues->insertTask(t, pp, opp);

        // This is meant to be a virtual assignment of CPU OPP.
        // Needed to make the rest of the code work properly
        pp->setOPP(opp);
        pp->setBusy(true);
    }

    /* Decide a CPU for each ready task */
    void EnergyMRTKernel::dispatch() {
        // test();
        DBGENTER(_KERNEL_DBG_LEV);
        setTryingTaskOnCPU_BL(true);

        int num_newtasks    = 0; // # "new" tasks in the ready queue
        int i               = 0;

        // how many "new" tasks in the ready queue?
        while (_sched->getTaskN(num_newtasks) != NULL)
            num_newtasks++;

        _sched->print();
        DBGPRINT_2("New tasks: ", num_newtasks);
        print();
        if (num_newtasks == 0) return; // nothing to do

        i = 0;
        std::vector<CPU_BL*> cpus = getProcessors();
        do {
            Task *t = dynamic_cast<Task*>(_sched->getTaskN(i++));
            if (t == NULL) break;
            cout << "Actual time = [" << SIMUL.getTime() << "]" << endl;
            cout << "Dealing with task " << t->toString() << "." << endl;

            // for testing
            if (manageForcedDispatch(t)) {
                num_newtasks--;
                continue;
            }

            if (isDispatching(t)) {
                // dispatch() is called even before onEndMultiDispatch() finishes and thus tasks seem
                // not to be dispatching (i.e., assigned to a processor)
                cout << "Task has already been dispatched, but dispatching is not complete => skip" << endl;
                continue;
            }

            if (getProcessor(t) != NULL) { // e.g., task ends => migrate() => dispatch()
                cout << "Task is running on a CPU already => skip" << endl;
                continue;
            }

            // otherwise scale up CPUs frequency
            DBGPRINT("Trying to scale up CPUs");
            cout << endl << "Trying to scale up CPUs" << endl;
            vector<struct ConsumptionTable> iDeltaPows;

            for (CPU_BL* c : cpus) {
                tryTaskOnCPU_BL(t, c, iDeltaPows);
            }

            if (!iDeltaPows.empty()) {
                chooseCPU_BL(t, iDeltaPows);
            } else {
                // TODO possibly move something
                cout << "Cannot schedule " << t->toString() << " anywhere" << endl;
                // todo not good, this makes the task arrive
                _sched->extract(t);
                _sched->removeTask(t);
            }
            num_newtasks--;

            cout << "Decisions 'til now:" << endl;
            cout << _queues->toString() << endl;

            // if you get here, task is not schedulable in real-time
        } while (num_newtasks > 0);
        setTryingTaskOnCPU_BL(false);

        for (CPU_BL *c : getProcessors())
            _queues->schedule(c);
    }

    void EnergyMRTKernel::tryTaskOnCPU_BL(AbsRTTask* t, CPU_BL* c, vector<struct ConsumptionTable>& iDeltaPows) {
        int startingOPP = c->getOPP();
        string startingWL = c->getWorkload();
        c->setWorkload(dynamic_cast<ExecInstr*>(dynamic_cast<Task*>(t)->getInstrQueue().at(0).get())->getWorkload());
        double frequency = c->getFrequency(); //!c->isBusy() ? c->getStructOPP(c->getIslandCurOPP()).frequency : c->getFrequency();
        cout << "\tTrying to schedule on CPU " << c->toString() << " using freq " << frequency
             << " - it has already ntasks=" << getReadyTasks(c).size() << endl;

        //for (int ooo = c->getIslandCurOPP(); ooo < c->getOPPs().size(); ooo++) {
        for (struct OPP tryOPP : c->getHigherOPPs()) {
            int ooo             = c->getIsland()->getOPPindex(tryOPP);
            double newFreq      = c->getFrequency(ooo);
            double newCapacity  = 0.0;

            c->setOPP(ooo);
            newCapacity = c->getSpeed(newFreq);
            printf("\t\tUsing frequency %d instead of %d (cap. %f)\n", (int) newFreq, (int) frequency, newCapacity);

            // check whether task is admissible with the new frequency and where
            if (_sched->isAdmissible(c, getReadyTasks(c), t)) {
                cout << "\t\t\tHere task would be admissible" << endl;

                double utilization          = 0.0; // utilization on the CPU c (without new task)
                double utilization_t        = 0.0; // utilization of the considered new task
                double newUtilizationIsland = 0.0; // utilization of tasks in the island with new freq - cores share frequency
                double oldUtilizationIsland = 0.0;
                double iPowWithNewTask      = 0.0;
                double iOldPow              = 0.0;
                double iDeltaPow            = 0.0; // additional power to schedule t on CPU c on the whole island (big/little)
                int    nTaskIsland          = 0;
                IslandType island;


                if (t->toString().find("T4_task_LITTLE_1") != string::npos && c->toString().find("LITTLE_1") != string::npos)
                    cout << "adsa";

                // utilization on CPU c with the new frequency
                utilization = getUtilization(c, newFreq, newCapacity);

                if (utilization > 1.0) {
                    cout << "\t\t\tCPU utilization is already >= 100% => skip OPP" << endl;
                    continue;
                } else
                    cout << "\t\t\tTotal utilization tasks already in CPU " << c->toString() << " = " << utilization << endl;

                utilization_t = getUtilization(t, c, newCapacity);
                cout << "\t\t\tUtilization cur task " << t->toString() << " would be " << utilization_t
                     << " - CPU capacity=" << newCapacity << endl;
                cout << "\t\t\t\tScaled task WCET " << t->getRemainingWCET(newCapacity) << " DL "
                     << t->getDeadline() << endl;

                if (utilization + utilization_t > 1.0) {
                    cout << "\t\t\tTotal utilization + cur task utilization would be " << utilization << "+" <<
                         utilization_t << "=" << utilization + utilization_t << " >= 100% => skip OPP" << endl;
                    continue;
                }

                // Ok, task can be placed on CPU c, compute power delta

                // utilization island where CPU c is
                island = c->getIslandType();
                newUtilizationIsland = getIslandUtilization(newCapacity, island, NULL);
                oldUtilizationIsland = getIslandUtilization(c->getSpeed(frequency), island, &nTaskIsland);
                cout << "\t\t\tIn the CPU island of " << c->getName() << ", " << nTaskIsland << " are being scheduled" << endl;

                iPowWithNewTask = (newUtilizationIsland + utilization_t) * c->getPowerConsumption(newFreq);
                iOldPow = oldUtilizationIsland * c->getPowerConsumption(frequency);

                // todo remove after debug
#include <cstdio>

                printf("\t\t\tnew = (%f + %f)*%.17g=%f, old %f*%.17g=%f\n", newUtilizationIsland,
                       utilization_t, c->getPowerConsumption(newFreq),
                       iPowWithNewTask, oldUtilizationIsland,
                       c->getPowerConsumption(frequency), iOldPow);

                iDeltaPow = iPowWithNewTask - iOldPow;
                assert(iPowWithNewTask >= 0.0); assert(iOldPow >= 0.0);
                cout << "\t\t\tiDeltaPow = new-old = " << iDeltaPow << endl;
                struct ConsumptionTable row = {.cons = iDeltaPow, .cpu = c, .opp = ooo};
                iDeltaPows.push_back(row);

                // break; (i.e. skip foreach OPP) xk è ovvio che aumentando la freq della stessa CPU, t è ammissibile
            } else {
                cout << "\t\t\tHere task wouldn't be admissible (U + U_newTask > 1)" << endl;
            }

        }

        c->setOPP(startingOPP);
        c->setWorkload(startingWL);
    } // end of tryTaskOnCPU_BL()

}
