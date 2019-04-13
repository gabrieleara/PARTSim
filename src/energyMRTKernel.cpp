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


namespace RTSim {
    using namespace MetaSim;

    EnergyMRTKernel::EnergyMRTKernel(Scheduler *s, std::vector<CPU*> cpus, const string& name)
            : MRTKernel(s, cpus.size(), name)
    {
        CPUs = cpus;

        for(CPU* c : cpus)  {
            _m_currExe[c] = NULL;
            _isContextSwitching[c] = false;
            _beginEvt[c] = new BeginDispatchMultiEvt(*this, *c);
            _endEvt[c] = new EndDispatchMultiEvt(*this, *c);
        }

        _sched->setKernel(this);
    }

    AbsRTTask* EnergyMRTKernel::getTaskRunning(CPU* c) {
        AbsRTTask* t = MRTKernel::getTask(c);
        return t;
    }

    std::vector<AbsRTTask*> EnergyMRTKernel::getTasks(CPU* c) const {
        std::vector<AbsRTTask*> t;

        for (const auto& elem : _m_dispatching)
            if (c == elem.second.first) {
                AbsRTTask *tt = const_cast<AbsRTTask *>(elem.first);
                t.push_back(tt);
            }

        return t;
    }

    double EnergyMRTKernel::getIslandUtilization(double capacity, CPU::Island island, int *nTasksIsland) {
        double utilizationIsland = 0.0;

        for (CPU* c1 : CPU::getCPUsInIsland(getProcessors(), island)) {
            vector<AbsRTTask*> tasks = getTasks(c1);
            AbsRTTask* runningTask = getTaskRunning(c1);
            if (runningTask != NULL) {
                utilizationIsland += ceil(runningTask->getWCET(capacity)) / double(runningTask->getDeadline());
                if (nTasksIsland != NULL)
                    *nTasksIsland = *nTasksIsland + 1;
            }
            for (AbsRTTask* th : tasks) {
                if (getDispatchingProcessor(th)->getIsland() == c1->getIsland() || getProcessor(th)->getIsland() == c1->getIsland()) {
                    utilizationIsland += ceil(th->getWCET(capacity)) / double(th->getDeadline());
                    if (nTasksIsland != NULL)
                        *nTasksIsland = *nTasksIsland + 1;
                }
            }
        }

        return utilizationIsland;
    }

    double EnergyMRTKernel::getUtilization(AbsRTTask* task, CPU* c, double capacity) const {
        double util = ceil(task->getWCET(capacity)) / double(task->getDeadline());

#include <cstdio>
        printf("\t\t\tgetUtilization of considered task %f/%f (capaity=%f)=%f\n",task->getWCET(capacity), double(task->getDeadline()), capacity, util);
        return util;
    }

    double EnergyMRTKernel::getUtilization(CPU* c, double freq, double capacity) const {
        double utilization = 0.0;
        vector<AbsRTTask*> ths = getTasks(c);
        AbsRTTask* runningTask = const_cast<EnergyMRTKernel*>(this)->getTaskRunning(c);
        if (runningTask != NULL)
            ths.push_back(runningTask);

        for (AbsRTTask* th : ths) {
            utilization += ceil(th->getWCET(capacity)) / double(th->getDeadline());
            cout << "\t\t\tUtilization task already in CPU, " << th->toString() << ", is "
                 << ceil(th->getWCET(capacity)) << "/" << th->getDeadline() << " = "
                 << ceil(th->getWCET(capacity)) / double(th->getDeadline())
                 <<  " - CPU capacity=" << capacity << endl;
            cout << "\t\t\t\ttask WCET " << ceil(th->getWCET(capacity)) << " DL "
                 << th->getDeadline() << endl;
        }

        return utilization;
    }

    CPU *EnergyMRTKernel::getDispatchingProcessor(const AbsRTTask *t) const
    {
        // process may be in the limbo between onBegin and onEndMultiDispatch,
        // thus it might not be caught by MRTKernel
        CPU* ret = NULL;

        for (const auto &elem : _m_dispatching)
            if (elem.first == t) {
                ret = elem.second.first;
                break;
            }

        return ret;
    }

    AbsRTTask* EnergyMRTKernel::getDispatchingTask(const CPU* cpu) const {
        // process may be in the limbo between onBegin and onEndMultiDispatch,
        // thus it might not be caught by MRTKernel
        AbsRTTask* ret = NULL;

        for (const auto &elem : _m_dispatching)
            if (elem.second.first == cpu) {
                ret = const_cast<AbsRTTask*>(elem.first);
                break;
            }

        return ret;
    }

    double EnergyMRTKernel::getTotalPowerConsumption() {
        return totalPowerCosumption;
    }

    bool EnergyMRTKernel::isDispatching(AbsRTTask* t) {
        if (t == NULL || _m_dispatching.empty())
            return false;

        auto it = _m_dispatching.find(t);
        if(it != _m_dispatching.end())
            return true;
        return false;
    }

    void EnergyMRTKernel::setIslandFrequency(CPU::Island island) {
        vector<CPU*> cpus = CPU::getCPUsInIsland(CPUs, island);
        CPU* max = cpus[0];
        for (CPU* cc : cpus) if (cc->getFrequency() > max->getFrequency()) max = cc;
        cout << "max opp is " << max->toString()<<endl;

        for (CPU* cc : cpus) cc->setOPP(max->getOPP());
    }



    // ----------------------------------------------------------- testing

    void EnergyMRTKernel::test() {
        // PeriodicTask T7_task0 DL = T 500 WCET(abs) 63 in CPU LITTLE_0 freq 500 freq 3
        Task *t = dynamic_cast<Task*>(_sched->getTaskN(0));
        CPU* p = CPUs[0];
        dispatch(p, t, 3);
        p->setWorkload("bzip2");
        cout << "CPU is " << p->toString() << " freq " << p->getFrequency()<< " "<< t->toString() << endl;

        int nTaskIsland = 0;
        cout << "task util " << getIslandUtilization(p->getSpeed(500.0), p->getIsland(), &nTaskIsland);

        exit(0);
    }

    // for gdb
    void EnergyMRTKernel::printMap() {
        for (const auto& elem:_m_dispatching) {
            if (elem.first != NULL)
                cout << elem.first->toString();
            if (elem.second.first != NULL)
                cout << " in " << elem.second.first->toString() << " freq " << elem.second.second;
            cout << endl;
        }
    }

    // for gdb
    void EnergyMRTKernel::printBool(bool b) {
        cout << b << endl;
    }

    void EnergyMRTKernel::addForcedDispatch(PeriodicTask* t, CPU* c, int opp) {
        _m_forcedDispatch[t] = make_pair(c, opp);
    }

    // for gdb
    bool EnergyMRTKernel::manageForcedDispatch(Task* t) {
        if (_m_forcedDispatch.find(t) != _m_forcedDispatch.end()) {
            cout << __func__ << endl;
            dispatch(_m_forcedDispatch[t].first, t, _m_forcedDispatch[t].second);
            _m_forcedDispatch.erase(t);
            return true;
        }
        return false;
    }

    // --------------------------------------------------------------- context switch

    // Note MRTKernel version differs: dispatch() tasks a free CUP and calls onBDM(), which in turns
    // assigns a task. EnergyMRTKernel, instead, needs to make assignment decisions: dispatch() chooses
    // a CPU for all tasks, and on*DM() makes the context switch (split into onBDM() and onEBM(), as in MRTKernel)
    void EnergyMRTKernel::onBeginDispatchMulti(BeginDispatchMultiEvt* e) {
        DBGENTER(_KERNEL_DBG_LEV);

        // if necessary, deschedule the task.
        CPU * p         = e->getCPU();
        AbsRTTask *dt   = _m_currExe[p];
        AbsRTTask *st   = getDispatchingTask(p);
        assert(st != NULL);


        if ( st != NULL && dt == st ) {
            stringstream ss;
            ss << "Decided to dispatch " << st->toString() << " on its former CPU => skip context switch";
            DBGPRINT(ss.str());
            cout << ss.str() << endl;
            return;
        }
        if ( dt != NULL ) {
            _m_oldExe[dt] = p;
            _m_currExe[p] = NULL;
            _m_dispatching.erase(dt);
            dt->deschedule();
        }

        DBGPRINT_4("Scheduling task ", taskname(st), " on cpu ", p->toString());
        // todo
        cout << __func__ << " Scheduling task " << taskname(st) << " on cpu " << p->toString() << endl;

        _endEvt[p]->setTask(st);
        _isContextSwitching[p] = true;
        // if you exit(0) here, dispatch() has already chosen a CPU forall tasks
        // exit(0);
        Tick overhead (_contextSwitchDelay);
        CPU* oldProcessor = getOldProcessor(st);
        if (oldProcessor != p && oldProcessor != NULL)
            overhead += _migrationDelay;
        _endEvt[p]->post(SIMUL.getTime() + overhead);
    }

    // Called after dispatch(), i.e. after choosing a CPU forall arrived tasks.
    // Also called when a periodic task ends its WCET
    void EnergyMRTKernel::onEndDispatchMulti(EndDispatchMultiEvt* e)
    {
        cout << "time =" << SIMUL.getTime() << " EnergyMRTKernel::onEndDispatchMulti() " << (e->getTask()==NULL?"":e->getTask()->toString()) << endl;
        MRTKernel::onEndDispatchMulti(e);
        AbsRTTask* t = e->getTask();
        _m_currExe_OPP[t] = _m_dispatching[t].second;

        // when its context switch ends, set the task OPP, as decided in dispatch().
        // This is needed when dispatch() decides to dispatch 2 tasks with equal
        // arrival time on the same processor: the first task ends and clocks down
        // the speed; the second task uses that one, wrongly.
        CPU* cpu = _m_dispatching[t].first;
        int opp = _m_dispatching[t].second;
        cout << t->toString() << " " << cpu->toString() << " setting opp to " << opp << endl;
        cpu->setOPP(opp);
        _m_oldExe[t] = cpu;
        _m_dispatching.erase(t);

        // Maybe a task has arrived and it needs to be scheduled on higher freq than
        // curr island freq -> on BL all CPUs have the same freq
        // todo useless?
        setIslandFrequency(cpu->getIsland());

        if (SIMUL.getTime() == _migrationDelay) // only for the first dispatch() of tasks
            totalPowerCosumption += cpu->getPowerConsumption(cpu->getFrequency());

        //todo remove
        for (const auto& elem : _m_dispatching)
        {
            cout << elem.first->toString() << " in " << elem.second.first->toString() << ", " << elem.second.first->getStructOPP(elem.second.second).frequency << endl;
        }

        // If you exit(0) here, trace.txt arrives 'til [Time:0]	T6_task4 arrived at 0.
        // ExecInstr::schedule() is called after each task's onEndDispatchMulti()
        // exit(0);
    }



    // ----------------------------------------------------------- task ends WCET

    void EnergyMRTKernel::onEnd(AbsRTTask* t) {
        DBGENTER(_KERNEL_DBG_LEV);

        // only on big-little: update the state of the CPUs island
        CPU* p = getProcessor(t);
        p->setBusy(false);
        DBGPRINT_6(t->toString(), " has just finished on ", p->toString(), ". Actual time = [", SIMUL.getTime(), "]");
        cout << ".............................." << endl;
        cout << t->toString() << " has just finished on " << p->toString() << ". Actual time = [" << SIMUL.getTime() << "]" << endl;

        vector<CPU *> cp = CPU::getCPUsInIsland(getProcessors(), p->getIsland());
        bool islandBusy = CPU::isCPUIslandBusy(CPUs, p->getIsland());
        cp[0]->setIsIslandBusy(islandBusy);

        _sched->extract(t);
        _m_oldExe[t] = p;
        _m_currExe.erase(p);
        _m_dispatched.erase(t);

        if (!CPU::isCPUIslandBusy(CPUs, CPU::Island::LITTLE))
            CPU::updateIslandCurOPP(CPUs, CPU::Island::LITTLE, 0);
        if (!CPU::isCPUIslandBusy(CPUs, CPU::Island::BIG))
            CPU::updateIslandCurOPP(CPUs, CPU::Island::BIG, 0);

        printState();

        migrate(p);

        // migrate ( and its dispatch() ) needs to know the required OPP of t on its core
        _m_currExe_OPP.erase(t);
    }

    void EnergyMRTKernel::migrate(CPU* endingCPU) {
        /**
           Migration mechanism: a task finishes on CPU c.
           Find again a feasible assignment of tasks to cores (dispatch), only considering tasks in
           dispatching[] originally scheduled on big and starting from OPP given by running tasks.
           If c is little and has no ready and running tasks => min freq.
           If c is big, same.

           Try not to touch running tasks.
         */
         if (_m_dispatching.empty())
             return;

         cout << __func__ << " time=" << SIMUL.getTime() << endl;

         // update WCET of tasks
         for (auto& elem : _m_currExe) {
             if (elem.second == NULL) continue; // happens 'cause of _m_currExe[p] = NULL instead of erasing...
             CPU* c = elem.first;
             double cap = c->getSpeed(double(c->getFrequency()));
             cout << cap << " " << c->getFrequency() << " " << c->toString() << endl;
             cout << elem.second->toString() << " needed WCET=" << elem.second->getWCET(cap) << endl;
             elem.second->refreshExec(cap, cap);
             cout << " and now still needs WCET = " << elem.second->getWCET(cap) << endl;
         }

         for (auto& t : _m_dispatching)
             if (t.second.first->getIsland() == CPU::Island::BIG || t.second.first->getName() == "LITTLE_3") {
                 vector<struct ConsumptionTable> iDeltaPows;
                 Task* tt = dynamic_cast<Task*>(const_cast<AbsRTTask*>(t.first));

                 cout << "Dealing with task " << tt->toString() << "." << endl;
                 tryTaskOnCPU(tt, CPUs[0], iDeltaPows);
                 tryTaskOnCPU(tt, CPUs[1], iDeltaPows);
                 tryTaskOnCPU(tt, CPUs[2], iDeltaPows);
                 tryTaskOnCPU(tt, CPUs[3], iDeltaPows);

                 if (!iDeltaPows.empty()) {
                     _m_dispatching.erase(tt);
                     chooseCPU(tt, iDeltaPows);
                 }
             }

    }



    // ----------------------------------------------------------- choosing cores for tasks

    void EnergyMRTKernel::leaveLittle3(AbsRTTask* t, vector<struct EnergyMRTKernel::ConsumptionTable> iDeltaPows, CPU*& chosenCPU) {
        /**
         * Policy of leaving a little core free for big-WCET tasks, which otherwise would be scheduled on
         * big cores, thus increasing power consumption.
         * Mechanism: if you want to put a task on little 3, but it also fits in another little core,
         * put it in the other little. Otherwise (if it only fits on little 3 and in bigs, choose little 3).
         */

        cout << __func__ << "():" << endl;
        if (chosenCPU->getName().find("LITTLE_3") == string::npos || chosenCPU->getIsland() == CPU::Island::BIG) {
            cout << "chosenCPU in big island or is not little_3 => skip" << endl;
            return;
        }

        bool fitsInOtherCore = true; // if it only fits on little 3 and in bigs

        for (int i = 0; i < iDeltaPows.size(); i++) {
            cout << iDeltaPows[i].cons << " " << iDeltaPows[i].cpu->toString() << endl;
            if (iDeltaPows[i].cpu->getIsland() == CPU::Island::LITTLE && iDeltaPows[i].cpu->getName().find("LITTLE_3") == string::npos) {
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

    void EnergyMRTKernel::chooseCPU(AbsRTTask* t, vector<struct EnergyMRTKernel::ConsumptionTable> iDeltaPows) {
        // TODO: switch from nlogn to n for min()
        // sort table of consumption
        sort(iDeltaPows.begin(), iDeltaPows.end(),
             [] (struct ConsumptionTable const& e1, struct ConsumptionTable const& e2) { return e1.cons < e2.cons; });

        // todo delete after debug
        for (auto elem: iDeltaPows) {
            cout << elem.cons << " "<< elem.cpu->toString() << " " << elem.cpu->getStructOPP(elem.opp).frequency << endl;
        }

        struct ConsumptionTable chosen = iDeltaPows[0];
        CPU* chosenCPU = chosen.cpu;
        int chosenOPP = chosen.opp;
        bool chosenCPUchanged = false;
        bool toBeChanged = chosenCPU->isCPUBusy();

        // if chosen CPU is busy, find a free CPU in the island with the same consumption.
        // Note: with this algorithm tasks cannot be assigned to a core in an island
        // different than the originally chosen one
        if (chosenCPU->isCPUBusy()) {
            cout << chosenCPU->toString() << " was chosen but it's busy" << endl;
            for (int i = 1; i < iDeltaPows.size(); i++) {
                cout << iDeltaPows[i].cons << " VS " << iDeltaPows[0].cons << " busy? "
                     << iDeltaPows[i].cpu->isCPUBusy() << " " << iDeltaPows[i].cpu->toString() << endl;
                if (iDeltaPows[i].cons == iDeltaPows[0].cons && !iDeltaPows[i].cpu->isCPUBusy()) {
                    chosenCPU = iDeltaPows[i].cpu;
                    chosenOPP = iDeltaPows[i].opp;
                    chosenCPUchanged = true;
                    break;
                }
            }
        }

        cout << "Temporarily chosenCPU: " << chosenCPU->toString() << " with freq " << chosenCPU->getStructOPP(chosenOPP).frequency << endl;
        leaveLittle3(t, iDeltaPows, chosenCPU);

        cout << "time = " << SIMUL.getTime() << " - going to schedule task " << t->toString() << " in CPU " << chosenCPU->getName() <<
             " with freq " << chosenCPU->getStructOPP(chosenOPP).frequency << " - CPU" << (chosenCPUchanged && toBeChanged ? "":" not") << " changed "  << endl;
        dispatch(chosenCPU, t, chosenOPP);
    }

    void EnergyMRTKernel::dispatch(CPU *p, AbsRTTask *t, int opp)
    {
        // This variable is only needed before the scheduling finishes (onEndDispatchMulti())
        _m_dispatching[t] = make_pair(p, opp);

        // this is meant to be a virtual assignment of CPU OPP
        p->setOPP(opp);
        p->setBusy(true);
        unsigned int maxOPP = CPU::findMaxOPP(&CPUs, p->getIsland());
        CPU::updateIslandCurOPP(CPUs, p->getIsland(), maxOPP);

        dispatch(p);
    }

    void EnergyMRTKernel::dispatch(CPU *p)
    {
        DBGENTER(_KERNEL_DBG_LEV);
        cout << "EnergyMRTKernel::dispatch()" << endl;

        if (p == NULL) throw RTKernelExc("Dispatch with NULL parameter");

        DBGPRINT_2("dispatching on processor ", p);
        _beginEvt[p]->drop();

        if (_isContextSwitching[p]) {
            // memo: I've seen it doesn't get here normally
            DBGPRINT("Context switch is disabled!");
            _beginEvt[p]->post(_endEvt[p]->getTime());
            AbsRTTask *task = _endEvt[p]->getTask();
            _endEvt[p]->drop();
            if (task != NULL) {
                _endEvt[p]->setTask(NULL);
                _m_dispatching[task].first = NULL;
            }
        } else {
            _beginEvt[p]->post(SIMUL.getTime());
        }
    }

    /* Decide a CPU for each ready task */
    void EnergyMRTKernel::dispatch()
    {
        // test();
        DBGENTER(_KERNEL_DBG_LEV);

        int ncpu            = _m_currExe.size();
        int num_newtasks    = 0; // # "new" tasks in the ready queue
        int i               = 0;

        // how many "new" tasks in the ready queue?
        for (i = 0; i < ncpu; ++i) {
            AbsRTTask *t = _sched->getTaskN(i);
            if (t == NULL) break;
            else if (getProcessor(t) == NULL &&
                     !isDispatching(t)) num_newtasks++;
        }

        _sched->print();
        DBGPRINT_2("New tasks: ", num_newtasks);
        print();
        if (num_newtasks == 0) return; // nothing to do

        i = 0;
        std::vector<CPU*> cpus = getProcessors();
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

            for (CPU* c : cpus) {
                int startingOPP = c->getOPP();

                c->setWorkload(dynamic_cast<ExecInstr*>(t->getInstrQueue().at(0).get())->getWorkload());
                tryTaskOnCPU(t, c, iDeltaPows);

                c->setOPP(startingOPP);
            }

            if (!iDeltaPows.empty()) {
                chooseCPU(t, iDeltaPows);
            } else {
                // TODO possibly move something
                cout << "Cannot schedule " << t->toString() << " anywhere" << endl;
                // todo don't know how to discard a task here. Think you need to work with _beginEvt and _endEvt
                _sched->extract(t);
            }
            num_newtasks--;

            cout << "Decisions 'til now:" << endl;
            for (auto const& elem : _m_dispatching)
                cout << elem.first->toString() << " in " << elem.second.first->toString() << " freq " << elem.second.second << endl;

            // if you get here, task is not schedulable in real-time
        } while (num_newtasks > 0);

    }

    void EnergyMRTKernel::tryTaskOnCPU(Task* t, CPU* c, vector<struct ConsumptionTable>& iDeltaPows) {
        double frequency = !c->isCPUIslandBusy() ? c->getStructOPP(c->getIslandCurOPP()).frequency : c->getFrequency();
        cout << "\tTrying to schedule on CPU " << c->toString() << " using freq " << frequency
             << " - it has already ntasks=" << getTasks(c).size() << endl;

        for (int ooo = c->getIslandCurOPP(); ooo < c->getOPPs().size(); ooo++) {
            double newFreq      = c->getOPPs()[ooo].frequency;
            double newCapacity  = 0.0;

            c->setOPP(ooo);
            newCapacity = c->getSpeed(newFreq);
            printf("\t\tUsing frequency %d instead of %d (cap. %f)\n", (int) newFreq, (int) frequency, newCapacity);

            // check whether task is admissible with the new frequency and where
            if (_sched->isAdmissible(c, getTasks(c), t)) {
                cout << "\t\t\tHere task would be admissible" << endl;

                double utilization          = 0.0; // utilization on the CPU c (without new task)
                double utilization_t        = 0.0; // utilization of the considered new task
                double newUtilizationIsland = 0.0; // utilization of tasks in the island with new freq - cores share frequency
                double oldUtilizationIsland = 0.0;
                double iPowWithNewTask      = 0.0;
                double iOldPow              = 0.0;
                double iDeltaPow            = 0.0; // additional power to schedule t on CPU c on the whole island (big/little)
                int nTaskIsland             = 0;
                CPU::Island island;
                Task *task = t;

                // utilization on CPU c with the new frequency
                utilization = getUtilization(c, newFreq, newCapacity);

                if (utilization > 1.0) {
                    cout << "\t\t\tCPU utilization is already >= 100% => skip OPP" << endl;
                    continue;
                } else
                    cout << "\t\t\tTotal utilization tasks already in CPU " << c->toString() << " = " << utilization
                         << endl;

                utilization_t = getUtilization(task, c, newCapacity);
                cout << "\t\t\tUtilization cur task " << t->toString() << " would be " << utilization_t
                     << " - CPU capacity=" << newCapacity << endl;
                cout << "\t\t\t\tScaled task WCET " << t->getWCET(newCapacity) << " DL "
                     << t->getDeadline() << endl;

                if (utilization + utilization_t > 1.0) {
                    cout << "\t\t\tTotal utilization + cur task utilization would be " << utilization << "+" <<
                         utilization_t << "=" << utilization + utilization_t << " >= 100% => skip OPP" << endl;
                    continue;
                }

                // Ok, task can be placed on CPU c, compute power delta

                // utilization island where CPU c is
                island = c->getIsland();
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
                assert(iDeltaPow >= 0.0);
                cout << "\t\t\tiDeltaPow = new-old = " << iDeltaPow << endl;
                struct ConsumptionTable row = {.cons = iDeltaPow, .cpu = c, .opp = ooo};
                iDeltaPows.push_back(row);

                // break; (i.e. skip foreach OPP) xk è ovvio che aumentando la freq della stessa CPU, t è ammissibile
            } else {
                cout << "\t\t\tHere task wouldn't be admissible (U + U_newTask > 1)" << endl;
            }

        }
    }

}
