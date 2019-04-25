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

#include "multi_sched.h"
#include <mrtkernel.hpp>

namespace RTSim {
    using namespace MetaSim;
    using namespace std;

    MultiScheduler::MultiScheduler(MRTKernel* k, vector<CPU*> cpus, vector<Scheduler*> scheds, const string& name)
            : Entity(name) {
        assert(cpus.size() == scheds.size());
        for (int i = 0; i < cpus.size(); i++)
            _queues[cpus.at(i)] = scheds.at(i);
        _kernel = k;
    }

    void MultiScheduler::addTask(AbsRTTask* t, CPU* c, const string &params) {
        _queues[c]->addTask(t, params);
    }

    void MultiScheduler::insertTask(AbsRTTask* t, CPU* c) {
        _queues[c]->insert(t);
    }

    void MultiScheduler::removeFirstFromQueue(CPU* c) {
        AbsRTTask *t = getFirst(c);
        removeFromQueue(c, t);
    }
    
    void MultiScheduler::removeFromQueue(CPU* c, AbsRTTask* t) {
        _queues[c]->extract(t);
    }

    AbsRTTask* MultiScheduler::getFirst(CPU* c) {
        return _queues[c]->getFirst();
    }

    vector<AbsRTTask*> MultiScheduler::getAllTasksInQueue(CPU* c) {
        vector<AbsRTTask*> tasks;

        Scheduler* s = _queues[c];
        AbsRTTask *t;
        int i = 0;
        while( (t = s->getTaskN(i)) != NULL) {
            tasks.push_back(t);
            i++;
        }

        return tasks;
    }

    void MultiScheduler::empty(CPU* c) {
        while (!isEmpty(c)) {
            removeFirstFromQueue(c);
        }
    }

    bool MultiScheduler::isEmpty(CPU* c) {
        return countTasks(c) == 0;
    }

    CPU* MultiScheduler::isInAnyQueue(const AbsRTTask* t) {
        for (const auto& q : _queues) {
            vector<AbsRTTask*> tasks = getAllTasksInQueue(q.first);
            for (AbsRTTask* tt : tasks)
                if (tt == t)
                    return q.first;
        }
        return NULL;
    }

    unsigned int MultiScheduler::countTasks(CPU* c) {
        int i = 0;
        while( _queues[c]->getTaskN(i) != NULL )
            i++;
        return i;
    }

    void MultiScheduler::postEvt(CPU* c, AbsRTTask* t, Tick when, bool endevt) {
        CPU_BL* cc = dynamic_cast<CPU_BL*>(c);
        assert(c != NULL); assert(t != NULL); assert(when >= SIMUL.getTime());
        if (endevt) {
            EndDispatchMultiEvt *ee  = new EndDispatchMultiEvt(*_kernel, *c);
            ee->post(when);
            ee->setTask(t);
            _endEvts[cc] = ee;
        }
        else {
            BeginDispatchMultiEvt *ee = new BeginDispatchMultiEvt(*_kernel, *c);
            ee->post(when);
            ee->setTask(t);
            _beginEvts[cc] = ee;
        }

    }

    void MultiScheduler::dropEvt(CPU* c, AbsRTTask* t) {
        bool found[2] = {false, false};

        if (_beginEvts[c]->getTask() == t) {
            _beginEvts[c]->drop();
            _beginEvts.erase(c);
            found[0] = true;
        }

        if (_endEvts[c]->getTask() == t) {
            _endEvts[c]->drop();
            _endEvts.erase(c);
            found[1] = true;
        }

        assert(found[0] || found[1]); // the event was found in either queues
        assert(found[0] != found[1]); // task can be either in begin dispatch or end dispatch on c
    }

}