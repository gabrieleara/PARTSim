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
#ifndef __FCFSRESMANAGER_HPP__
#define __FCFSRESMANAGER_HPP__

#include <deque>
#include <map>

#include <rtsim/resource/resmanager.hpp>

#define _FCFS_RES_MAN_DBG_LEV "FCFSResManager"

namespace RTSim {

    class AbsRTTask;
    class Task;

    /// @ingroup resman
    ///
    /// Simple resource manager which implements a FCFS strategy for each
    /// resource.
    ///
    /// @see Resource
    class FCFSResManager : public ResManager {
    public:
        FCFSResManager(const string &n = "");

        void newRun() override;
        void endRun() override;

    protected:
        bool request(AbsRTTask *t, Resource *resource, int nr) override;
        void release(AbsRTTask *t, Resource *resource, int nr) override;

    private:
        struct request_t {
            AbsRTTask *const task;
            const int nr;

            request_t(AbsRTTask *task, int nr) : task(task), nr(nr) {}
        };

        using blocked_queue_t = std::deque<request_t>;
        std::map<Resource *, blocked_queue_t> _blocked;
    };

} // namespace RTSim

#endif
