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

    SchedulerMulti::SchedulerMulti(vector<CPU*> cpus, vector<Scheduler*> scheds, const string& name)
            : Entity(name) {
        assert(cpus.size() == scheds.size());
        for (int i = 0; i < cpus.size(); i++)
            _queues[cpus.at(i)] = scheds.at(i);
    }

    bool SchedulerMulti::isInAnyQueue(AbsRTTask* t) const {
        bool found = false;
        for (const auto& q : _queues)
            for (int i = 0; i < countTasks(q.first); i++)
                if (t == q.second->getTaskN(i) ) {
                    found = true;
                    break;
                }
        return found;
    }

    void SchedulerMulti::addTask(AbsRTTask* t, CPU* c) {
        _queues[c]->insert(t);
    }

    void SchedulerMulti::removeFirstFromQueue(CPU* c) {
        extract(c);
    }
    
    void SchedulerMulti::removeFromQueue(CPU* c, AbsRTTask* t) {
        _queues[c]->removeTask(t);
    }

    AbsRTTask* SchedulerMulti::getFirst(CPU* c) const {
        return _queues[c]->getFirst();
    }

    AbsRTTask* SchedulerMulti::extract(CPU* c) {
        _queues[c]->extract();
    }

    void SchedulerMulti::empty(CPU* c) {
        while (!isEmpty(c)) {
            removeFirstFromQueue(c);
        }
    }

    bool SchedulerMulti::isEmpty(CPU* c) const {
        return countTasks(c) == 0;
    }

    unsigned int SchedulerMulti::countTasks(CPU* c) const {
        int i = 0;
        while( _queues[c]->getTaskN(i) != NULL )
            i++
        return i;
    }

    void SchedulerMulti::migrate(MigrationPolicy* m, Event *e) {
        m->perform(e);
    }

    void SchedulerMulti::balance(BalancingPolicy* b, Event *e) {
        b->perform(e);
    }

}