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

#include <sched_multi.h>

namespace RTSim {
    using namespace MetaSim;
    using namespace std;

    SchedulerMulti::SchedulerMulti(vector<CPU*> cpus, Scheduler s, const string& name)
            : Entity(name) {
        for (CPU* c : cpus)
            _queues[c] = s;
    }

    bool SchedulerMulti::isInAnyQueue(AbsTask* t) const {

    }

    void addTask(AbsTask* t, CPU* c) {

    }

    void removeFromQueue(CPU* c) {

    }

    AbsTask* extract(CPU* c) const {

    }

    AbsTask* pop(CPU* c) {

    }

        void empty(CPU* c);

        bool isEmpty(CPU* c) const;

        void migrate(MigrationPolicy* m) {
            m->perform();
        }

        void balance(BalancingPolicy* b) {
            b->perform();
        }

    void setRunning(CPU* c);

        bool isRunning(CPU* c) const;
    };
}