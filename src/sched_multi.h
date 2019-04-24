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

#ifndef SIMPLE_EXAMPLE_SCHED_MULTI_H
#define SIMPLE_EXAMPLE_SCHED_MULTI_H

#include <string>
#include "entity.hpp"
#include "scheduler.hpp"

namespace RTSim {

    using namespace MetaSim;
    using namespace std;

    class Policy {
    public:
        /// Activates the policy
        virtual void perform() = 0;
    };

    class MigrationPolicy : public Policy {
    public:
        /// Activates the policy
        virtual void perform() = 0;
    };

    class BalancingPolicy : public Policy {
    public:
        /// Activates the policy
        virtual void perform() = 0;
    };

    /**
        \ingroup sched

        An implementation of multi-scheduler. The typical scenario is:
        you have N cores and you need a queue per core. Each queue can be
        modelled as a scheduler. Schedulers are supposed to be equal.

        This implementation is quite general and you can specialize it.

        @see CPU, AbsTask
     */
    class SchedulerMulti : public MetaSim::Entity {
    private:
        map<CPU*, Scheduler> _queues;

        virtual bool isInAnyQueue(AbsTask* t) const;
    public:
        SchedulerMulti(vector<CPU*> cpus, Scheduler s, const string& name);

        virtual void addTask(AbsTask* t, CPU* c);

        virtual void removeFromQueue(CPU* c);

        virtual AbsTask* extract(CPU* c) const;

        virtual AbsTask* pop(CPU* c);

        void empty(CPU* c);

        bool isEmpty(CPU* c) const;

        void migrate(MigrationPolicy* m) {
            m->perform();
        }

        void balance(BalancingPolicy* b) {
            b->perform();
        }

        virtual void setRunning(CPU* c);

        bool isRunning(CPU* c) const;
    };

}
#endif //SIMPLE_EXAMPLE_SCHED_MULTI_H
