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
#ifndef __EDFSCHED_HPP__
#define __EDFSCHED_HPP__

#include <rtsim/scheduler/scheduler.hpp>

namespace RTSim {

    using namespace MetaSim;

    /**
       \ingroup kernels

       Model for the EDF scheduler
    */
    class EDFModel : public TaskModel {
    protected:
        bool extP;
        int prio;

    public:
        EDFModel(AbsRTTask *t);
        Tick getPriority() const override;
        void changePriority(Tick p) override;
    };

    /**
       \ingroup kernels

       This class implements an EDF scheduler. It redefines only
       the addTask function, because most of the work is done in
       the Scheduler class.
    */
    class EDFScheduler : public Scheduler {
    public:
        /**
         *  Creates an EDFModel, passing the task!! It throws a RTSchedExc
         *  exception if the task is already present in this scheduler.
         */
        void addTask(AbsRTTask *task); // throw(RTSchedExc);

        void addTask(AbsRTTask *task, const string &p) override;

        void removeTask(AbsRTTask *task) override;

        static EDFScheduler *createInstance(vector<string> &par);

        /**
         * Admission test for a task t on a CPU c
         * @return true if t is admissible on c
         */
        bool isAdmissible(CPU *c, vector<AbsRTTask *> tasks,
                          AbsRTTask *toBeAdmitted) override;
    };

} // namespace RTSim

#endif
