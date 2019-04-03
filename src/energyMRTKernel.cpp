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
#include <scheduler.hpp>
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

    // todo do I need getTask or getTask?
    AbsRTTask* EnergyMRTKernel::getTask(CPU* c)
    {
        AbsRTTask* t = _m_currExe[c];

        if (t == NULL)
            for (const auto& elem : _m_dispatching) {
                if (c == elem.second.first) {
                    AbsRTTask *tt = const_cast<AbsRTTask *>(elem.first);
                    t = tt;
                    break;
                }
            }

        return t;
    }

    std::vector<AbsRTTask*> EnergyMRTKernel::getTasks(CPU* c) const
    {
        std::vector<AbsRTTask*> t;

        for (const auto& elem : _m_dispatching)
            if (c == elem.second.first) {
                AbsRTTask *tt = const_cast<AbsRTTask *>(elem.first);
                t.push_back(tt);
            }

        return t;
    }

    double EnergyMRTKernel::getIslandUtilization(double capacity, enum CPU::Island island, int *nTasksIsland) {
        double utilizationIsland = 0.0;

        for (CPU* c1 : CPU::getCPUsInIsland(getProcessors(), island)) {
            for (AbsRTTask* th : getTasks(c1)) {
                if (getProcessor(th)->getIsland() == c1->getIsland()) {
                    utilizationIsland += ceil(th->getWCET(capacity)) / double(th->getDeadline());
                    if (nTasksIsland != NULL)
                        *nTasksIsland = *nTasksIsland + 1;
                }
            }
        }

        return utilizationIsland;
    }

    double EnergyMRTKernel::getUtilization(AbsRTTask* task, CPU* c, double capacity) const {
        // todo implementation of getwcet is wrong since it should handle cases with many instructions (sum of their wcet)
        double util = ceil(task->getWCET(capacity)) / double(task->getDeadline());

        //cout<<"\t\t\tgetUtilization " << typeid(*task).name()<<endl;
#include <cstdio>
        printf("\t\t\tgetUtilization of considered task %f/%f (capaity=%f)=%f\n",task->getWCET(capacity), double(task->getDeadline()), capacity, util);
        return util;
    }

    double EnergyMRTKernel::getUtilization(CPU* c, double freq, double capacity) const {
        double utilization = 0.0;
        vector<AbsRTTask*> ths = getTasks(c);

        for (AbsRTTask* th : ths) {
            //utilization += th->getWCET(c->getCapacity(freq)) / double(th->getDeadline());
            utilization += ceil(th->getWCET(capacity)) / double(th->getDeadline());
            cout << "\t\t\tUtilization task already in CPU, " << th->print() << ", is "
                 << ceil(th->getWCET(capacity)) << "/" << th->getDeadline() << " = "
                 << ceil(th->getWCET(capacity)) / double(th->getDeadline())
                 <<  " - CPU capacity=" << capacity << endl;
            cout << "\t\t\t\ttask WCET " << ceil(th->getWCET(capacity)) << " DL "
                 << th->getDeadline() << endl;
        }

        return utilization;
    }

    CPU *EnergyMRTKernel::getProcessor(const AbsRTTask *t) const
    {
        CPU* ret = MRTKernel::getProcessor(t);

        // process may be in the limbo between onBegin and onEndMultiDispatch,
        // thus it might not be caught by MRTKernel
        if (ret == NULL) {
            for (auto &elem : _m_dispatching)
                if (elem.first == t) {
                    ret = elem.second.first;
                    break;
                }
        }

        return ret;
    }

    bool EnergyMRTKernel::isDispatching(AbsRTTask* t) {
        if (t == NULL || _m_dispatching.empty())
            return false;

        auto it = _m_dispatching.find(t);
        if(it != _m_dispatching.end())
            return true;
        return false;
    }

    void EnergyMRTKernel::test() {
        CPU* p = CPUs[2];
        p->setOPP(0);
        p->setWorkload("bzip2");
        Task *t = dynamic_cast<Task*>(_sched->getTaskN(0));
        cout << "CPU is " << p->print() << " freq " << p->getFrequency()<< " "<< t->print() << endl;

        cout << "task util " << getUtilization(t, p, p->getSpeed());

        exit(0);
    }

    bool EnergyMRTKernel::existDispatchingTask(CPU* p) {
        bool res = false;

        for (auto elem : _m_dispatching) {
            if (elem.second.first == p) {
                res = true;
                break;
            }
        }

        return res;
    }

    // also called when a periodic task ends its WCET
    void EnergyMRTKernel::onEndDispatchMulti(EndDispatchMultiEvt* e)
    {
        cout << "time =" << SIMUL.getTime() << " EnergyMRTKernel::onEndDispatchMulti() " << (e->getTask()==NULL?"":e->getTask()->print()) << endl;
        MRTKernel::onEndDispatchMulti(e);

        // when its context switch ends, set the task OPP, as decided in dispatch().
        // This is needed when dispatch() decides to dispatch 2 tasks with equal
        // arrival time on the same processor: the first task ends and clocks down
        // the speed. The second task uses that one, wrongly
        AbsRTTask* t = e->getTask();
        if (t != NULL) {
            e->getCPU()->setOPP(_m_dispatching[e->getTask()].second);
            _m_dispatching.erase(t);
        }
    }

    void EnergyMRTKernel::onEnd(AbsRTTask* t) {
        DBGENTER(_KERNEL_DBG_LEV);

        // only on big-little: update the state of the CPUs island
        bool islandBusy = false;
        CPU* p = getProcessor(t);
        vector<CPU *> cp = CPU::getCPUsInIsland(getProcessors(), p->getIsland());
        p->setBusy(false);
        DBGPRINT_6(t->print(), " has just finished on ", p->print(), ". Actual time = [", SIMUL.getTime(), "]");

        // determine if island where task was scheduled is busy
        for (CPU *c : cp) {
            if (c->isCPUBusy()) {
                islandBusy = true;
                break;
            }
            c->setIsIslandBusy(islandBusy);
        }

        // this wouldn't be done on Linux, since it's not convenient to clock down CPUs after task ends.
        // Here, however, it' needed to make formulas and the framework work
        DBGPRINT_2("Is island busy? ", bool(islandBusy) );
        if (!islandBusy) {
            for (CPU* c : cp) {
                c->setOPP(0);
                DBGPRINT_2("Clock down ", c->print());
            }
        }

        MRTKernel::onEnd(t);
    }

    void EnergyMRTKernel::chooseCPU(AbsRTTask* t, vector<struct EnergyMRTKernel::ConsumptionTable> iDeltaPows) {
        // TODO: switch from nlogn to n for min()
        // sort table of consumption
        sort(iDeltaPows.begin(), iDeltaPows.end(),
             [] (struct ConsumptionTable const& e1, struct ConsumptionTable const& e2) { return e1.cons < e2.cons; });

        // todo delete after debug
        for (auto elem: iDeltaPows) {
            cout << elem.cons << " "<< elem.cpu->print() << " " << elem.opp.frequency << endl;
        }

        // TODO: if iDeltaPows[0].cons == iDeltaPows[1].cons == ..., then add logic to choose one of them
        struct ConsumptionTable chosen = iDeltaPows[0];
        CPU* chosenCPU = chosen.cpu;
        struct OPP chosenOPP = chosen.opp;

        // if chosen CPU is busy, find a free CPU in the island with the same consumption.
        // Note: with this algorithm tasks cannot be assigned to a core in an island
        // different than the originally chosen one
        if (chosenCPU->isCPUBusy())
            cout << chosenCPU->print() << " was chosen but it's busy" << endl;
            for (int i = 1; i < iDeltaPows.size(); i++) {
                cout << iDeltaPows[i].cons << " VS " << iDeltaPows[0].cons << " busy? " << iDeltaPows[i].cpu->isCPUBusy() << endl;
                if (iDeltaPows[i].cons == iDeltaPows[0].cons && !iDeltaPows[i].cpu->isCPUBusy()) {
                    chosenCPU = iDeltaPows[i].cpu;
                    chosenOPP = iDeltaPows[i].opp;
                    break;
                }
            }

        cout << "time=" << SIMUL.getTime() << " - going to schedule task " << t->print() << " in CPU " << chosenCPU->getName() <<
             " with freq " << chosenOPP.frequency << " - else " << endl;
        dispatch(chosenCPU, t, chosenOPP);
    }

    void EnergyMRTKernel::dispatch(CPU *p, AbsRTTask *t, const struct OPP opp)
    {
        // This variable is only needed before the scheduling finishes (onEndDispatchMulti())
        _m_dispatching[t] = make_pair(p, opp);

        p->setOPP(opp);
        p->setBusy(true);

        dispatch(p);
    }

    void EnergyMRTKernel::dispatch(CPU *p)
    {
        DBGENTER(_KERNEL_DBG_LEV);
        cout << "EnergyMRTKernel::dispatch()" << endl;

        if (p == NULL) throw RTKernelExc("Dispatch with NULL parameter");

        DBGPRINT_2("dispatching on processor ", p);
        _beginEvt[p]->drop();

        // on BIG-LITTLE, the whole island has the same frequency/voltage
        map<CPU*, double> oldSpeeds;
        vector<CPU*> cpus = CPU::getCPUsInIsland(getProcessors(), p->getIsland());
        cpus.erase(remove(cpus.begin(), cpus.end(), p), cpus.end());
        for (CPU *c : cpus) {
          oldSpeeds[c] = c->getSpeed();
          cout << c->print() << " oldSpeed " << oldSpeeds[c];
          c->setOPP(p->getOPP());
          cout << " to " << p->getSpeed() << endl;
          c->setIsIslandBusy(true);
        }

        // In case scheduler decides to clock up CPU freq while other tasks are already ready/running on the island,
        // other tasks instructions should be reweighted and beginEvt too
        if (p->isCPUIslandBusy())
        for (CPU* c : cpus) {
            cout << "beginEvt " << _beginEvt[c]->getTime() << " VS " << SIMUL.getTime() << endl;
            _beginEvt[c]->drop();
            if (c != p) {
              cout << "ciao" << endl;
                //_beginEvt[c]->post(SIMUL.getTime()); // todo bug?

                for (AbsRTTask* t : getTasks(c)) {
                    // todo it should be done forall instr in queue?
                    cout << "fatto " << t->print() <<  endl;
                    ExecInstr* instr = dynamic_cast<ExecInstr*>(dynamic_cast<Task*>(t)->getInstrQueue().at(0).get());
                                                       cout << instr->getWCET() << endl;
                    instr->refreshExec(oldSpeeds[c], c->getSpeed());
                }
            }
        }
 
        if (_isContextSwitching[p]) {
            // I've seen it doesn't get here normally
            DBGPRINT("Context switch is disabled!");
            _beginEvt[p]->post(_endEvt[p]->getTime());
            _endEvt[p]->drop();
            if (_endEvt[p]->getTask() != NULL)
                _m_dispatched[_endEvt[p]->getTask()] = NULL;
        }
        else
            _beginEvt[p]->post(SIMUL.getTime());
    }

    /* Select a free CPU */
    void EnergyMRTKernel::dispatch()
    {
        // test();

        DBGENTER(_KERNEL_DBG_LEV);

        int ncpu = _m_currExe.size();
        int num_newtasks = 0; // # "new" tasks in the ready queue
        int i = 0;

        // how many "new" tasks in the ready queue?
        for (i = 0; i < ncpu; ++i) {
            AbsRTTask *t = _sched->getTaskN(i);
            if (t == NULL) break;
            else if (getProcessor(t) == NULL &&
                     _m_dispatched[t] == NULL) num_newtasks++;
        }

        _sched->print();
        DBGPRINT_2("New tasks: ", num_newtasks);
        print();
        if (num_newtasks == 0) return; // nothing to do
        cout << "I must schedule #task=" << num_newtasks << endl;

        i = 0;
        ITCPU start = _m_currExe.begin();
        ITCPU stop = _m_currExe.end();
        ITCPU f = start;
        std::vector<CPU*> cpus = getProcessors();
        do {
            cout << "Actual time = [" << SIMUL.getTime() << "]" << endl;
            double e = 0.0;
            std::map<double, CPU*> energies;
            cout << "getTaskN"<<i<<endl;
            Task *t = dynamic_cast<Task*>(_sched->getTaskN(i++));
            if (t == NULL) break;
            cout << "Dealing with task " << t->print() << "." << endl;

            if (isDispatching(t)) {
                // dispatch() is called even before onEndMultiDispatch() finishes and thus tasks seem
                // not to be dispatched (i.e., assigned to a processor)
                cout << "Task has already been dispatched, but dispatching is not complete => skip" << endl;
                continue;
            }

            // otherwise scale up CPUs frequency
            DBGPRINT("Trying to scale up CPUs");
            cout << "Trying to scale up CPUs" << endl;
            //std::map<double, std::pair<CPU*, struct OPP>> iDeltaPows; // power delta -> [CPU, new freq]
            vector<struct EnergyMRTKernel::ConsumptionTable> iDeltaPows;

            for (CPU* c : cpus) {
                c->setWorkload(dynamic_cast<ExecInstr*>(t->getInstrQueue().at(0).get())->getWorkload());
                double frequency = !c->isCPUIslandBusy() ? c->getMinOPP().frequency : c->getFrequency();
                cout << "\tTrying to schedule on CPU " << c->print() << " freq " << frequency << " - it has already ntasks=" << getTasks(c).size() << endl;
                for (struct OPP o : c->getNextOPPs()) {
                  //double frequency = !c->isCPUIslandBusy() ? c->getMinOPP().frequency : c->getFrequency();
                    double newFreq = o.frequency;
                    double newCapacity = 0.0;

                    c->setOPP(o);
                    newCapacity = c->getSpeed(newFreq);
                    printf("\t\tUsing frequency %d instead of %d (cap. %f)\n", (int) newFreq, (int) frequency, newCapacity);

                    // check whether task is admissible with the new frequency and where
                    if (_sched->isAdmissible(c, getTasks(c), t)) {
                        cout << "\t\t\tHere task would be admissible" << endl;

                        double utilization = 0.0; // utilization on the CPU c (without new task)
                        double utilization_t = 0.0; // utilization of the considered new task
                        double newUtilizationIsland = 0.0; // utilization of tasks in the island with new freq - cores share frequency
                        double oldUtilizationIsland = 0.0;
                        double iPowWithNewTask = 0.0;
                        double iOldPow = 0.0;
                        double iDeltaPow = 0.0; // additional power to schedule t on CPU c on the whole island (big/little)
                        int    nTaskIsland = 0;
                        enum   CPU::Island island;
                        Task   *task = t;

                        // utilization on CPU c with the new frequency
                        utilization = getUtilization(c, newFreq, newCapacity);

                        if (utilization > 1.0) {
                            cout << "\t\t\tCPU utilization is already >= 100% => skip OPP" << endl;
                            continue;
                        }
                        else cout << "\t\t\tTotal utilization tasks already in CPU " << c->print() << " = " << utilization << endl;

                        utilization_t = getUtilization(task, c, newCapacity);
                        cout << "\t\t\tUtilization cur task " << t->print() << " would be " << utilization_t
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

                        // additional required power
                        // ipowWithNewTask = (c->getIslandUtilization(c->getCapacity(newFreq)) + utilization_t) * c->getPowerConsumption(newFreq);
                        iPowWithNewTask = (newUtilizationIsland + utilization_t) * c->getPowerConsumption(newFreq);
                        // ioldPow = c->getIslandUtilization(c->getCapacity(frequency)) * c->getPowerConsumption(frequency);
                        iOldPow = oldUtilizationIsland * c->getPowerConsumption(frequency);

                        // todo remove after debug
                        #include <cstdio>
                        printf("\t\t\tnew = (%f + %f)*%.17g=%f, old %f*%.17g=%f\n", newUtilizationIsland,
                                utilization_t, c->getPowerConsumption(newFreq),
                                iPowWithNewTask, oldUtilizationIsland,
                                c->getPowerConsumption(frequency), iOldPow);

                        iDeltaPow = iPowWithNewTask - iOldPow;
                        cout << "\t\t\tiDeltaPow = new-old = " << iDeltaPow << endl;
                        struct ConsumptionTable row = { .cons = iDeltaPow, .cpu = c, .opp = o } ;
                        iDeltaPows.push_back(row);

                        // break; (i.e. skip foreach OPP) xk è ovvio che aumentando la freq della stessa CPU, t è ammissibile
                    }
                    else {
                        cout << "\t\t\tHere task wouldn't be admissible (U + U_newTask > 1)" << endl;
                    }

                }
            }

            if (!iDeltaPows.empty()) {
                chooseCPU(t, iDeltaPows);
            } else {
                // TODO possibly move something
            }
            num_newtasks--;

            // if you get here, task is not schedulable in real-time
        } while (num_newtasks > 0);

    }

}
