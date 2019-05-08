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

#include <multi_cores_queues.hpp>
#include <mrtkernel.hpp>

namespace RTSim {
    using namespace MetaSim;
    using namespace std;

    string MigrationManager::mapEventType(enum EventType e) const {
        string res;
        switch (e) {
        case SCHEDULE:
            res = "schedule";
            break;
        case DESCHEDULE:
            res = "deschedule";
            break;
        case SUSPEND:
            res = "suspend";
            break;
        case END:
            res = "end";
            break;
        default:
            res = "?";
            break;
        }
        return res;
    }

    ostream& operator<<(ostream& out, const MigrationManager& m) {
        return out << m.toString();
    }

    MultiScheduler::MultiScheduler(MRTKernel* k, vector<CPU*>& cpus, vector<Scheduler*>& scheds, const string& name)
            : Entity(name) {
        assert(cpus.size() == scheds.size());
        for (int i = 0; i < cpus.size(); i++) {
          //scheds.at(i)->setKernel(NULL);
            _queues[cpus.at(i)] = scheds.at(i);
        }
        _kernel = k;
    }

    void MultiScheduler::addTask(AbsRTTask* t, CPU* c, const string &params) {
        _queues[c]->addTask(t, params);
    }

    void MultiScheduler::insertTask(AbsRTTask* t, CPU* c) {
        try {
            _queues[c]->insert(t);
        } catch(RTSchedExc &e) {
            // core schedulers/queues don't know tasks until this point
            cout << "Receiving this error once per task is ok" << endl;
            addTask(t,c,"");
            insertTask(t,c);
        }
    }

    void MultiScheduler::removeFirstFromQueue(CPU* c) {
        AbsRTTask *t = getFirst(c);
        removeFromQueue(c, t);
    }
    
    void MultiScheduler::removeFromQueue(CPU* c, AbsRTTask* t) {
        assert(c != NULL); assert(t != NULL);
        _queues[c]->extract(t);
	      dropEvt(c, t);
    }

    AbsRTTask* MultiScheduler::getFirst(CPU* c) {
        assert(c != NULL);
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
        assert(c != NULL); assert(t != NULL); assert(when >= SIMUL.getTime());

            CPU_BL* cc = dynamic_cast<CPU_BL*>(c);
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
        assert(c != NULL); assert(t != NULL);

        if (_beginEvts.find(c) != _beginEvts.end() && _beginEvts[c]->getTask() == t) {
            _beginEvts[c]->drop();
            _beginEvts.erase(c);
        }

        if (_endEvts.find(c) != _endEvts.end() && _endEvts[c]->getTask() == t) {
            _endEvts[c]->drop();
            _endEvts.erase(c);
        }
    }

    string MultiScheduler::toString() {
        return "MultiScheduler toString().";
    }

}
