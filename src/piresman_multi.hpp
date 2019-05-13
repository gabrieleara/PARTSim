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
#ifndef __PIRESMAN_MULTI_HPP__
#define __PIRESMAN_MULTI_HPP__

#include <piresman.hpp>
#include <resource.hpp>

#define _PIRESMAN_MULTI_DBG_LEV  "piresman_multi"

namespace RTSim {
    using namespace std;
    using namespace MetaSim;

    class MultiCoresScheds;

    /**
       \ingroup resman

       This class implements the Priority Inheritance Protocol. It has the
       same interface of any Resource Manager class, in addition it has a
       setScheduler() function, because the PI needs the scheduler for
       changing the priority of a task.

       It adds support to MultiCoresScheds to PIRManager, so that the resource
       manager knows the scheduler where to take the task model from.

       The trick is to dynamically change the scheduler that holds the task wanting
       the resource.
    */
    class PIRManagerMulti : public PIRManager {
    public:
      /// Constructor
      PIRManagerMulti(const std::string &name, MultiCoresScheds* scheds, vector<CPU*> cpus);
        
        /**
           Returns true if the resource can be locked, false otherwise
           (in such a case, the task should be blocked)
         */
      virtual bool request(AbsRTTask *t, Resource *r, int n = 1);
        
        /**
           Releases the resource.
         */
      virtual void release(AbsRTTask *t, Resource *r, int n = 1);

    private:
      /// The MultiCoresScheds where to take the task
      MultiCoresScheds* _scheds;

      /// A PIRManager (priority inheritance) per core
      map<CPU*, PIRManager*> _resManagers;

      //void setSchedulerForTask(Scheduler* s) {
        //assert(s != NULL);
        //_sched = s;
        //}

    };
}

#endif
