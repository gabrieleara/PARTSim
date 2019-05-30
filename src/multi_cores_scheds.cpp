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
#include <energyMRTKernel.hpp>


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
        if (_queues[c]->isFound(t))
            _queues[c]->extract(t);
        dropEvt(c, t);
    }

    AbsRTTask* MultiCoresScheds::getFirst(CPU* c) {
        assert(c != NULL);
        AbsRTTask *t = _queues[c]->getFirst();

        CBServer* cbs = dynamic_cast<CBServer*>(t);
        if (cbs != NULL && cbs->isYielding())
            t = getFirstReady(c);

        return t;
    }

    AbsRTTask* MultiCoresScheds::getFirstReady(CPU* c) {
        assert(c != NULL);
        // assumes there is only 1 CBS server per core

        AbsRTTask *t = _queues[c]->getTaskN(1);
        return t;
    }

    vector<AbsRTTask*> MultiCoresScheds::getAllTasksInQueue(CPU* c) const {
        vector<AbsRTTask*> tasks;

        Scheduler* s = _queues.at(c);
        AbsRTTask *t;
        int i = 0;
        while( (t = s->getTaskN(i)) != NULL) {
            tasks.push_back(t);
            i++;
        }

        return tasks;
    }

    void MultiCoresScheds::forgetU_active(AbsRTTask* t) {
      cout << "\tMSC::" << __func__ << "() for " << t->toString() << endl;

        if (EnergyMRTKernel::CBS_ENVELOPING_PER_TASK_ENABLED)
            t = dynamic_cast<EnergyMRTKernel*>(_kernel)->getEnveloped(t);

        for (auto& elem : _active_utilizations) {
            if (elem.first == t) {
                cout << "\treleasing_idle for " << elem.first->toString() << ". Its U_act was " << get<2>(elem.second) << endl;
                _active_utilizations.erase(elem.first);
            }
        }
    }

    void MultiCoresScheds::empty(CPU* c) {
        for (CBServer* s : _kernel->getServers())
            removeFromQueue(c, s);

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

    bool MultiCoresScheds::shouldDeschedule(CPU *c, AbsRTTask *t) {
        if (dynamic_cast<RRScheduler*>(_queues[c])) {
            return getRunningTask(c) != NULL;
        }
        else if (dynamic_cast<EDFScheduler*>(_queues[c])){
            if (dynamic_cast<CBServer*>(t) && getRunningTask(c) == t)
                return false;
            bool isNotSameTask = getRunningTask(c) != NULL && getRunningTask(c) != t;
            return isNotSameTask;
        }
        assert(false);  // add your choice/branch
        return false;
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
