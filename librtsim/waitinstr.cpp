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
#include <metasim/simul.hpp>

#include <rtsim/kernel.hpp>
#include <rtsim/task.hpp>
#include <rtsim/waitinstr.hpp>

namespace RTSim {

    using std::unique_ptr;
    using std::vector;

    WaitInstr::WaitInstr(Task *f, const string &r, int nr, const string &n) :
        Instr(f, n),
        _res(r),
        _endEvt(this),
        _waitEvt(f, this),
        _numberOfRes(nr) {}

    WaitInstr::WaitInstr(const WaitInstr &other) :
        Instr(other),
        _res(other.getResource()),
        _endEvt(this),
        _waitEvt(other.getTask(), this),
        _numberOfRes(other.getNumOfResources()) {}

    unique_ptr<WaitInstr> WaitInstr::createInstance(vector<string> &par) {
        unique_ptr<WaitInstr> ptr(
            new WaitInstr(dynamic_cast<Task *>(Entity::_find(par[1])), par[0]));

        return ptr;
    }

    void WaitInstr::endRun() {
        _endEvt.drop();
        _waitEvt.drop();
    }

    void WaitInstr::schedule() {
        DBGENTER(_INSTR_DBG_LEV);
        DBGPRINT("Scheduling WaitInstr named: ", getName());

        _endEvt.post(SIMUL.getTime());
    }

    void WaitInstr::deschedule() {
        _endEvt.drop();
    }

    // void WaitInstr::setTrace(Trace *t)
    // {
    //     _endEvt.addTrace(t);
    //     _waitEvt.addTrace(t);
    // }

    /// Since tasks can be nested and parent tasks usually implement both the
    /// AbsRTTask and the AbsRTKernel interfaces (see Servers), we need to climb
    /// up the ladder until we find a kernel on which we can request the
    /// resource and the task seen by that kernel that is issuing the request.
    ///
    /// Given the task that issued the wait/signal instruction, this function
    /// will climb up said ladder until a suitable pair is found.
    ///
    /// @note this means that, in case a task is wrapped in a server, the server
    /// will be the one issuing the request for the task, at least from a
    /// RTKernel/Scheduler point of view. This way, RTKernel/Scheduler
    /// implementation can be agnostic with respect to tasks hierarchy.
    ///
    /// @returns true if a suitable pair is found
    ///
    /// @param[inout] task_ptr      used to supply the initial task on which
    /// perform the research; after this call it will contain the task on which
    /// the actual request must be done
    ///
    /// @param[out]   kernel_ptr    the kernel on which the request must be done
    bool findKernelTask(AbsRTTask **task_ptr, RTKernel **rtkernel_ptr) {
        AbsRTTask *&task = (*task_ptr);
        RTKernel *&rtkernel = (*rtkernel_ptr);

        rtkernel = nullptr;
        while (rtkernel == nullptr && task != nullptr) {
            AbsKernel *kernel = task->getKernel();
            rtkernel = dynamic_cast<RTKernel *>(kernel);

            if (rtkernel == nullptr) {
                // The kernel is a server, not an actual rtkernel convert it to
                // its task interface and keep climbing up
                task = dynamic_cast<AbsRTTask *>(kernel);
            }
        }

        return task != nullptr;
    }

    void WaitInstr::onEnd() {
        DBGENTER(_INSTR_DBG_LEV);

        _father->onInstrEnd();

        AbsRTTask *task = _father;
        RTKernel *rtkernel;
        auto found = findKernelTask(&task, &rtkernel);
        if (!found)
            throw BaseExc("WaitInstr: Kernel not found!");

        rtkernel->requestResource(task, _res, _numberOfRes);

        _waitEvt.process();
    }

    SignalInstr::SignalInstr(Task *f, const string &r, int nr,
                             const string &n) :
        Instr(f, n),
        _res(r),
        _endEvt(this),
        _signalEvt(f, this),
        _numberOfRes(nr) {}

    SignalInstr::SignalInstr(const SignalInstr &other) :
        Instr(other),
        _res(other.getResource()),
        _endEvt(this),
        _signalEvt(other.getTask(), this),
        _numberOfRes(other.getNumOfResources()) {}

    unique_ptr<SignalInstr> SignalInstr::createInstance(vector<string> &par) {
        unique_ptr<SignalInstr> ptr(new SignalInstr(
            dynamic_cast<Task *>(Entity::_find(par[1])), par[0]));
        return ptr;
    }

    void SignalInstr::endRun() {
        _endEvt.drop();
        _signalEvt.drop();
    }

    void SignalInstr::schedule() {
        _endEvt.post(SIMUL.getTime());
    }

    void SignalInstr::deschedule() {
        _endEvt.drop();
    }

    // void SignalInstr::setTrace(Trace *t)
    // {
    //     _endEvt.addTrace(t);
    //     _signalEvt.addTrace(t);
    // }

    void SignalInstr::onEnd() {
        DBGENTER(_INSTR_DBG_LEV);

        _endEvt.drop();
        _signalEvt.process();
        _father->onInstrEnd();

        AbsRTTask *task = _father;
        RTKernel *rtkernel;
        auto found = findKernelTask(&task, &rtkernel);
        if (!found)
            throw BaseExc("SignalInstr: Kernel not found!");

        rtkernel->releaseResource(task, _res, _numberOfRes);
    }

} // namespace RTSim
