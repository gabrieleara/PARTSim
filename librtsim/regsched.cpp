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
#include <metasim/factory.hpp>
#include <metasim/strtoken.hpp>

#include <rtsim/edfsched.hpp>
#include <rtsim/fifosched.hpp>
#include <rtsim/fpsched.hpp>
#include <rtsim/rrsched.hpp>

namespace RTSim {

    const string FIFOName("FIFOSched");
    const string FPName("FPSched");
    const string EDFName("EDFSched");
    const string RRName("RRSched");

    /**
        This namespace should never be used by the user. Contains
        functions to initialize the abstract factory that builds
        the scheduler.
    */
    namespace __sched_stub {
        static registerInFactory<Scheduler, FPScheduler, string>
            registerfp(FPName);

        static registerInFactory<Scheduler, EDFScheduler, string>
            registeredf(EDFName);

        static registerInFactory<Scheduler, RRScheduler, string>
            registerrr(RRName);

        static registerInFactory<Scheduler, FIFOScheduler, string>
            registerfifo(FIFOName);
    } // namespace __sched_stub
    void __regsched_init() {}
} // namespace RTSim
