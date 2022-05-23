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
/*
 * $Id: fcfsresmanager.cpp,v 1.2 2005/04/28 01:34:48 cesare Exp $
 *
 * $Log: fcfsresmanager.cpp,v $
 * Revision 1.2  2005/04/28 01:34:48  cesare
 * Moved to sstream. Headers install. Code cleaning.
 *
 * Revision 1.1  2004/12/01 02:08:18  cesare
 * *** empty log message ***
 *
 * Revision 1.3  2004/11/26 03:47:10  cesare
 * Finished merging the main trunk with Lipari's branch.
 *
 */
#include <rtsim/abskernel.hpp>
#include <rtsim/resource/fcfsresmanager.hpp>
#include <rtsim/resource/resource.hpp>
#include <rtsim/task.hpp>

namespace RTSim {

    FCFSResManager::FCFSResManager(const string &n) : ResManager(n) {}

    void FCFSResManager::newRun() {
        _blocked.clear();
    }

    void FCFSResManager::endRun() {}

    bool FCFSResManager::request(AbsRTTask *t, Resource *r, int nr) {
        DBGENTER(_FCFS_RES_MAN_DBG_LEV);

        if (_blocked[r].size() > 0 || r->available() < nr) {
            // If there are tasks waiting, even if this request could be served,
            // we push it back into the queue to avoid starvation.
            DBGPRINT("Suspending task ");

            AbsKernel *kernel = t->getKernel();
            kernel->suspend(t);

            _blocked[r].emplace_back(t, nr);
            return false;
        }

        // TODO: setting the owner for multi-unit resources is kinda pointless?
        DBGPRINT("Locking resource");
        r->lock(t, nr);
        return true;
    }

    void FCFSResManager::release(AbsRTTask *t, Resource *r, int nr) {
        DBGENTER(_FCFS_RES_MAN_DBG_LEV);

        DBGPRINT("Unlocking resource ");
        r->unlock(nr);

        int nr_available = r->available();
        while (nr_available > 0 && !_blocked[r].empty()) {
            auto [task, nr_required] = _blocked[r].front();
            if (nr_required <= nr_available) {
                DBGPRINT("Relocking resource");
                _blocked[r].pop_front();
                r->lock(task, nr_required);

                AbsKernel *kernel = task->getKernel();
                kernel->activate(task);

                // NOTE: the kernel may be the same of the caller, in that case
                // dispatch will be called more than once even if only one call
                // would be sufficient. But if the kernels differ this call is
                // absolutely necessary to notify the other kernel that its
                // situation changed.
                kernel->dispatch();
            }

            // Remove anyway so that even in the case in which we do not lock
            // the resource again we exit the loop because there's not enough
            // available
            nr_available -= nr_required;
        }
    }

} // namespace RTSim
