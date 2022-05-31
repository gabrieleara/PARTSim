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

#include <iostream>

#include <rtsim/cbserver.hpp>
#include <rtsim/mrtkernel.hpp>
#include <rtsim/scheduler/scheduler.hpp>

namespace RTSim {
    // =========================================================================
    // class BeginDispatchMultiEvt
    // =========================================================================

    BeginDispatchMultiEvt::BeginDispatchMultiEvt(MRTKernel &k, CPU &c) :
        DispatchMultiEvt(k, c, Event::_DEFAULT_PRIORITY + 10) {}

    void BeginDispatchMultiEvt::doit() {
        _kernel.onBeginDispatchMulti(this);
    }

    // =========================================================================
    // class EndDispatchMultiEvt
    // =========================================================================

    EndDispatchMultiEvt::EndDispatchMultiEvt(MRTKernel &k, CPU &c) :
        DispatchMultiEvt(k, c, Event::_DEFAULT_PRIORITY + 10) {}

    void EndDispatchMultiEvt::doit() {
        _kernel.onEndDispatchMulti(this);
    }

    // =========================================================================
    // class MRTKernel
    // =========================================================================

    // =====================================================
    // Constructors and Destructor
    // =====================================================

    static inline std::set<CPU *> createCPUSet(absCPUFactory *factory,
                                               size_t n) {
        std::set<CPU *> cpus;

        for (size_t i = 0; i < n; i++) {
            cpus.insert(factory->createCPU());
        }

        return cpus;
    }

    MRTKernel::MRTKernel(Scheduler *s, std::set<CPU *> cpus,
                         const string &name) :
        RTKernel(s, name),
        _migrationDelay(0) {
        // internalConstructor(cpus);
        for (auto c : cpus) {
            addCPU(c);
        }
        _sched->setKernel(this);
    }

    // MRTKernel::MRTKernel(Scheduler *s, std::vector<CPU *> cpus,
    //                      const string &name) :
    //     MRTKernel(s, std::set<CPU *>(cpus.begin(), cpus.end()), name) {}

    // MRTKernel::MRTKernel(Scheduler *s, absCPUFactory *factory, int n,
    //                      const string &name) :
    //     MRTKernel(s, createCPUSet(factory, n)) {}

    // // Using std::make_unique, we create a temporary unique_ptr that will
    // // automatically delete the uniformCPUFactory once done
    // MRTKernel::MRTKernel(Scheduler *s, int n, const string &name) :
    //     MRTKernel(s, std::make_unique<uniformCPUFactory>().get(), n, name) {}

    // MRTKernel::MRTKernel(Scheduler *s, const string &name) :
    //     MRTKernel(s, 1, name) {}

    /// Deletes elements pointed by maps
    template <class IT>
    static inline void clean_mapcontainer(IT b, IT e) {
        for (IT i = b; i != e; i++)
            delete i->second;
    }

    MRTKernel::~MRTKernel() {
        // delete _CPUFactory;
        clean_mapcontainer(_beginEvt.begin(), _beginEvt.end());
        clean_mapcontainer(_endEvt.begin(), _endEvt.end());
    }

    // =====================================================
    // Methods
    // =====================================================

    CPU *MRTKernel::getFreeProcessor() {
        for (auto it : _m_currExe) {
            if (it.second == nullptr)
                return it.first;
        }
        return nullptr;
    }

    bool MRTKernel::isDispatched(CPU *p) const {
        for (auto it : _m_dispatched) {
            if (it.second == p)
                return true;
        }
        return false;
    }

    MRTKernel::ITCPU MRTKernel::getNextFreeProc(ITCPU begin, ITCPU end) {
        for (auto it = begin; it != end; ++it) {
            if (it->second == nullptr && !isDispatched(it->first))
                return it;
        }
        return end;
    }

    void MRTKernel::addCPU(CPU *c) {
        DBGENTER(_KERNEL_DBG_LEV);

        _m_currExe[c] = nullptr;
        _isContextSwitching[c] = false;
        _beginEvt[c] = new BeginDispatchMultiEvt(*this, *c);
        _endEvt[c] = new EndDispatchMultiEvt(*this, *c);

        c->setKernel(this);
    }

    void MRTKernel::addTask(AbsRTTask &t, const string &param) {
        RTKernel::addTask(t, param);
        _m_oldExe[&t] = nullptr;
        _m_dispatched[&t] = nullptr;

        CBServer *cbs = dynamic_cast<CBServer *>(&t);
        if (cbs != nullptr)
            _servers.push_back(cbs);
    }

    void MRTKernel::onArrival(AbsRTTask *task) {
        DBGENTER(_KERNEL_DBG_LEV);

        _sched->insert(task);
        dispatch();
    }

    void MRTKernel::suspend(AbsRTTask *task) {
        DBGENTER(_MRTKERNEL_DBG_LEV);

        _sched->extract(task);
        CPU *p = getProcessor(task);
        if (p != nullptr) {
            task->deschedule();

            _m_currExe[p] = nullptr;
            _m_oldExe[task] = p;
            _m_dispatched[task] = nullptr;
            dispatch(p);
        }
    }

    void MRTKernel::onEnd(AbsRTTask *task) {
        DBGENTER(_KERNEL_DBG_LEV);

        CPU *p = getProcessor(task);

        if (p == nullptr)
            throw RTKernelExc("Received a onEnd of a non executing task");

        _sched->extract(task);
        _m_oldExe[task] = p;
        _m_currExe[p] = nullptr;
        _m_dispatched[task] = nullptr;

        dispatch(p);
    }

    void MRTKernel::dispatch(CPU *p) {
        DBGENTER(_KERNEL_DBG_LEV);

        if (p == nullptr)
            throw RTKernelExc("Dispatch with NULL parameter");
        DBGPRINT("dispatching on processor ", p);

        // Undo any previous "begin dispatch event" existing on this CPU
        _beginEvt[p]->drop();

        if (_isContextSwitching[p]) {
            DBGPRINT("Context switch is disabled!");

            // Shifting forward the dispatch time on this cpu until the current
            // context switch (event) is done
            _beginEvt[p]->post(_endEvt[p]->getTime());

            // The previous context switch is canceled (the time it took to run
            // will still be accounted though)
            AbsRTTask *task = _endEvt[p]->getTask();
            _endEvt[p]->drop();
            if (task != nullptr) {
                _endEvt[p]->setTask(nullptr);
                _m_dispatched[task] = nullptr;
            }
        } else {
            // Perform the dispatch now (see onBeginDispatchMulti)
            _beginEvt[p]->post(SIMUL.getTime());
        }
    }

    void MRTKernel::dispatch() {
        DBGENTER(_KERNEL_DBG_LEV);

        size_t ncpu = _m_currExe.size();

        // Tells us how many of the first ncpu tasks in the ready queue are not
        // yet scheduled or dispatched for scheduling.
        int num_newtasks = 0;

        // Check whether the first ncpu tasks in the ready queue are already
        // dispatched or not.
        for (size_t i = 0; i < ncpu; ++i) {
            AbsRTTask *t = _sched->getTaskN(i);
            if (t == nullptr)
                break;
            else if (getProcessor(t) == nullptr && _m_dispatched[t] == nullptr)
                ++num_newtasks;
        }

        // Technically, in the old impl i can be less than ncpu, but if we break
        // before reaching ncpu for sure there are NO tasks to evict and i will
        // never be used.
        int i = ncpu;

        _sched->print();
        DBGPRINT("New tasks: ", num_newtasks);
        print();

        if (num_newtasks < 1)
            return;

        for (auto f = getNextFreeProc(_m_currExe.begin(), _m_currExe.end());
             num_newtasks > 0; f = getNextFreeProc(f, _m_currExe.end())) {
            if (f != _m_currExe.end()) {
                DBGPRINT("Dispatching on free processor ", f->first);
                dispatch(f->first);
                --num_newtasks;
                ++f;
            } else {
                // We have to "evict" a task from being scheduled/dispatched
                // because there are no more CPUs and a task that is "higher" in
                // the ready queue has to run on its CPU.

                // ORIGINAL NOTE FROM TOMMASO:
                // NON-SENSE: this is putting all new tasks on WHAT CPU ?!?

                // NOTE: this loop takes a long while to complete
                for (;;) {
                    AbsRTTask *t = _sched->getTaskN(i++);
                    if (t == nullptr) {
                        throw RTKernelExc(
                            "Can't find enough tasks to deschedule!");
                    }

                    // NOTE: does not check for running tasks, only dispatched
                    // ones!
                    CPU *c = _m_dispatched[t];
                    if (c != nullptr) {
                        DBGPRINT("Dispatching on processor ", c,
                                 " which is executing task ", taskname(t));
                        dispatch(c);
                        --num_newtasks;
                        break;
                    }
                }
            }
        }
    }

    void MRTKernel::onBeginDispatchMulti(BeginDispatchMultiEvt *e) {
        DBGENTER(_KERNEL_DBG_LEV);

        // if necessary, deschedule the task.
        CPU *p = e->getCPU();
        AbsRTTask *dt = _m_currExe[p];
        AbsRTTask *st = nullptr;

        if (dt != nullptr) {
            _m_oldExe[dt] = p;
            _m_currExe[p] = nullptr;
            _m_dispatched[dt] = nullptr;
            dt->deschedule();
        }

        // select the first non dispatched task in the queue
        int i = 0;
        while ((st = _sched->getTaskN(i)) != nullptr)
            if (_m_dispatched[st] == nullptr)
                break;
            else
                i++;

        if (st == nullptr) {
            DBGPRINT("Nothing to schedule, finishing");
        }

        DBGPRINT("Scheduling task ", taskname(st), " on cpu ", p->toString());

        if (st)
            _m_dispatched[st] = p;
        _endEvt[p]->setTask(st);
        _isContextSwitching[p] = true;
        Tick overhead(_contextSwitchDelay);
        if (st != nullptr && _m_oldExe[st] != p && _m_oldExe[st] != nullptr)
            overhead += _migrationDelay;
        _endEvt[p]->post(SIMUL.getTime() + overhead);
    }

    void MRTKernel::onEndDispatchMulti(EndDispatchMultiEvt *e) {
        // performs the "real" context switch
        DBGENTER(_KERNEL_DBG_LEV);

        AbsRTTask *st = e->getTask();
        CPU *p = e->getCPU();

        _m_currExe[p] = st;

        DBGPRINT("CPU: ", p->toString());
        DBGPRINT("Task: ", taskname(st));
        printState();

        // st could be null (because of an idling processor)
        if (st)
            st->schedule();

        _isContextSwitching[p] = false;
        _sched->notify(st);
    }

    CPU *MRTKernel::getProcessor(const AbsRTTask *t) const {
        DBGENTER(_KERNEL_DBG_LEV);
        CPU *ret = nullptr;

        for (auto i = _m_currExe.cbegin(); i != _m_currExe.cend(); i++)
            if (i->second == t)
                ret = i->first;
        return ret;
    }

    CPU *MRTKernel::getOldProcessor(const AbsRTTask *t) const {
        CPU *ret = nullptr;

        DBGENTER(_KERNEL_DBG_LEV);

        auto it = _m_oldExe.find(t);
        if (it != _m_oldExe.cend())
            ret = it->second;

        return ret;
    }

    std::vector<CPU *> MRTKernel::getProcessors() const {
        std::vector<CPU *> s;

        // typedef map<CPU *, AbsRTTask *>::const_iterator IT;
        int j = 0;

        // TODO: either pre-allocate or push_back?
        for (auto i = _m_currExe.cbegin(); i != _m_currExe.cend(); i++, j++)
            s[j] = i->first;
        return s;
    }

    void MRTKernel::newRun() {
        for (auto i = _m_currExe.begin(); i != _m_currExe.end(); i++) {
            if (i->second != nullptr)
                _sched->extract(i->second);
            i->second = nullptr;
        }

        for (auto j = _m_dispatched.begin(); j != _m_dispatched.end(); ++j) {
            j->second = nullptr;
        }

        for (auto j = _m_oldExe.begin(); j != _m_oldExe.end(); ++j) {
            j->second = nullptr;
        }
    }

    void MRTKernel::endRun() {
        for (auto i = _m_currExe.begin(); i != _m_currExe.end(); i++) {
            if (i->second != nullptr)
                _sched->extract(i->second);
            i->second = nullptr;
        }
    }

    void MRTKernel::print() const {
        DBGPRINT("Executing");
        for (auto i = _m_currExe.cbegin(); i != _m_currExe.cend(); ++i)
            DBGPRINT("  [", i->first, "] --> ", taskname(i->second));

        DBGPRINT("Dispatched");
        for (auto j = _m_dispatched.cbegin(); j != _m_dispatched.cend(); ++j)
            DBGPRINT("  [", taskname(j->first), "] --> ", j->second);
    }

    void MRTKernel::printState() const {
        Entity *task;
        std::cout << "MRTKernel::printstate(), time " << SIMUL.getTime() << " ";
        for (auto i = _m_currExe.cbegin(); i != _m_currExe.cend(); i++) {
            task = dynamic_cast<Entity *>(i->second);
            if (task != nullptr)
                std::cout << i->first->getName() << " : " << task->getName()
                          << "   ";
            else
                std::cout << i->first->getName() << " :   0   ";
        }
        std::cout << std::endl;
    }

    AbsRTTask *MRTKernel::getTask(const CPU *c) {
        // Not the cleanest solution, but the comparison operator doesn't care
        // about the pointd element anyway
        return _m_currExe[const_cast<CPU *>(c)];
    }

    std::vector<std::string> MRTKernel::getRunningTasks() {
        std::vector<std::string> tmp_ts;
        for (auto i = _m_currExe.cbegin(); i != _m_currExe.cend(); i++) {
            std::string tmp_name = taskname((*i).second);
            if (tmp_name != "(nil)")
                tmp_ts.push_back(tmp_name);
        }
        return tmp_ts;
    }

} // namespace RTSim
