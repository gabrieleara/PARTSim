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

#include <rtsim/resmanager.hpp>

#define _FCFS_RES_MAN_DBG_LEV "FCFSResManager"

namespace RTSim {

    using std::map;

    class AbsRTTask;
    class Task;

    /**
       \ingroup resman
       Simple Resource manager which implements a FCFS strategy
       for a single resource
       @ see Resource
    */
    class FCFSResManager : public ResManager {
    public:
        /** Constructor of FCFSResManager
         *
         * @param n is the resource manager name
         */
        FCFSResManager(const string &n = "");

        void newRun() override;
        void endRun() override;

    protected:
        bool request(AbsRTTask *, Resource *, int n = 1) override;
        void release(AbsRTTask *, Resource *, int n = 1) override;

    private:
        map<Resource *, AbsRTTask *> _resAndCurrUsers;
        typedef std::deque<AbsRTTask *> BLOCKED_QUEUE;
        map<Resource *, BLOCKED_QUEUE> _blocked;
    };

} // namespace RTSim

#endif
