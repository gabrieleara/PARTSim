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

#include <multi_cores_scheds.hpp>
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

    MultiCoresScheds::MultiCoresScheds(MRTKernel* k, vector<CPU*>& cpus, vector<Scheduler*>& scheds, const string& name)
            : Entity(name) {
        assert(cpus.size() == scheds.size());
        for (int i = 0; i < cpus.size(); i++) {
            scheds.at(i)->setKernel(k);
            _queues[cpus.at(i)] = scheds.at(i);
        }
        _kernel = k;
    }

    void MultiCoresScheds::addTask(AbsRTTask* t, CPU* c, const string &params) {
        _queues[c]->addTask(t, params);
        if (dynamic_cast<RRScheduler*>(_queues[c]))
          dynamic_cast<RRScheduler*>(_queues[c])->notify(t);
    }

    void MultiCoresScheds::insertTask(AbsRTTask* t, CPU* c) {
        try {
            _queues[c]->insert(t);
        } catch(RTSchedExc &e) {
            // core schedulers/queues don't know tasks until this point
            cout << "Receiving this error once per task is ok" << endl;
            addTask(t,c,"");
            insertTask(t,c);
        }
    }

    void MultiCoresScheds::removeFirstFromQueue(CPU* c) {
        AbsRTTask *t = getFirst(c);
        removeFromQueue(c, t);
    }
    
    void MultiCoresScheds::removeFromQueue(CPU* c, AbsRTTask* t) {
        assert(c != NULL); assert(t != NULL);
        _queues[c]->extract(t);
	      dropEvt(c, t);
    }

    AbsRTTask* MultiCoresScheds::getFirst(CPU* c) {
        assert(c != NULL);
        return _queues[c]->getFirst();
    }

    vector<AbsRTTask*> MultiCoresScheds::getAllTasksInQueue(CPU* c) {
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

    void MultiCoresScheds::empty(CPU* c) {
        while (!isEmpty(c)) {
            removeFirstFromQueue(c);
        }
    }

    bool MultiCoresScheds::isEmpty(CPU* c) {
        return countTasks(c) == 0;
    }

    CPU* MultiCoresScheds::isInAnyQueue(const AbsRTTask* t) {
        for (const auto& q : _queues) {
            vector<AbsRTTask*> tasks = getAllTasksInQueue(q.first);
            for (AbsRTTask* tt : tasks)
                if (tt == t)
                    return q.first;
        }
        return NULL;
    }

    unsigned int MultiCoresScheds::countTasks(CPU* c) {
        int i = 0;
        while( _queues[c]->getTaskN(i) != NULL )
            i++;
        return i;
    }

    PIRManagerMulti* MultiCoresScheds::setResources(vector<string> resources, vector<unsigned int> quantities) {
        assert(resources.size() == quantities.size());

        vector<CPU*> cpus;
        for (const auto& elem : _queues) {
            cpus.push_back(elem.first);
        }

        PIRManagerMulti *resMan = new PIRManagerMulti("PIRManagerMulti", this, cpus);
        return resMan;
    }

    void MultiCoresScheds::postEvt(CPU* c, AbsRTTask* t, Tick when, bool endevt) {
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

    void MultiCoresScheds::dropEvt(CPU* c, AbsRTTask* t) {
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


    string MultiCoresScheds::toString() {
        return "MultiCoresScheds toString().";
    }

}
