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
#include <sstream>
#include <string>
#include <typeinfo>

#include <metasim/entity.hpp>
#include <metasim/event.hpp>
#include <metasim/simul.hpp>

// Used to demangle compiler class names
// #include <metasim/demangle.hpp>

namespace MetaSim {
    /** ------------------------------------------------------------
     * Initialize static members of class Event
     * ------------------------------------------------------------
     */
    Event::EventQueue Event::_eventQueue;

    long Event::counter = 0;

    /**
     * Constructor for Event.
     */
    Event::Event(const std::string &name, int p) :
        _name(name),
        _order(0),
        _isInQueue(false),
        _particles(),
        _time(MAXTICK),
        _lastTime(MAXTICK),
        _priority(p),
        _std_priority(p),
        _disposable(false) {}

    // Event::Event(int p) : Event(demangle_compiler_name(typeid(*this).name()),
    // p) {}

    Event::~Event() {
        drop();
    }

    // Copy constructor
    Event::Event(const Event &e) :
        _order(0),
        _isInQueue(false),
        _particles(),
        _time(MAXTICK),
        _lastTime(MAXTICK),
        _priority(e._priority),
        _std_priority(e._std_priority),
        _name(e._name),
        _disposable(e._disposable) {
        for (auto &p : e._particles)
            p->clone_to(*this);
    }

    bool Event::Cmp::operator()(Event *e1, Event *e2) const {
        Tick firstTime, secondTime;

        firstTime = e1->getTime();
        secondTime = e2->getTime();

        if (firstTime < secondTime)
            return true;
        else if (firstTime == secondTime) {
            if (e1->getPriority() < e2->getPriority())
                return true;
            else if (e1->getPriority() == e2->getPriority()) {
                if (e1->_order < (unsigned long) e2->_order)
                    return true;
                else
                    return false;
            } else
                return false;
        } else
            return false;
    }

    void Event::post(Tick myTime, bool disp) { // throw(Exc, BaseExc) {
        if (_isInQueue) {
            std::stringstream str;
            str << "Time: " << SIMUL.getTime() << " -- Event"
                << toString() // demangle_compiler_name(typeid(*this).name())
                << " already posted";
            throw Exc(str.str());
        }

        if (myTime < SIMUL.getTime()) {
            std::stringstream str;
            str << "Time: " << SIMUL.getTime() << " -- Posting event"
                << toString() // demangle_compiler_name(typeid(*this).name())
                << " in the past at time: " << myTime;
            throw Exc(str.str());
        }

        setTime(myTime);

        _order = counter++;

        std::pair<EventQueue::iterator, bool> p;
        p = _eventQueue.insert(this);

        if (!p.second) {
            std::stringstream str;
            str << "Time: " << SIMUL.getTime() << " -- Posting event"
                << toString() // demangle_compiler_name(typeid(*this).name())
                << " Already in queue!, at time = " << myTime;
            throw Exc(str.str());
        }

        _isInQueue = true;
        _disposable = disp;

        DBGENTER(_EVENT_DBG_LEV);
        print();
    }

    // erase the event from the event queue
    void Event::drop() {
        DBGENTER(_EVENT_DBG_LEV);
        print();

        _eventQueue.erase(this);
        _isInQueue = false;
    };

    void Event::process(bool disp) {
        DBGENTER(_EVENT_DBG_LEV);
        drop();
        print();
        setPriority(_IMMEDIATE_PRIORITY);
        post(SIMUL.getTime(), disp);
    }

    // Function to set the event time
    // (only if the event is not in any queue).
    void Event::setTime(Tick actTime) { // throw(Exc) {
        if (_isInQueue)
            throw Exc("Cannot set the time if the event is already queued\n");
        else
            _time = actTime;
    }

    // see comment below on exceptions to be thrown by this function
    void Event::action() {
        DBGENTER(_EVENT_DBG_LEV);

        /* Handles the event ONLY if it has target!!! Otherwise executes
         * its tracing and calls the related stats.....
         */
        _lastTime = _time;

        // restore old priority
        restorePriority();

        // the doit(), probe() and record() could raise
        // arbitrary exception...  hence, I can't specify the
        // exception type in the interface!
        // It may repost the event
        doit();

        // for(its = _stats.begin(); its != _stats.end(); its++)
        //     (*its)->probe(this);

        // the new way of doing statistics. The old way
        // remains valid, but it is deprecated.
        DBGPRINT("Calling the particle probes, size = ", _particles.size());
        for (auto itp = _particles.begin(); itp != _particles.end(); itp++) {
            DBGPRINT("Calling probe");
            (*itp)->probe();
        }
    }

    // DEBUG!!! Prints events data on the dbg stream.
    void Event::print() {
        DBGPRINT("t=[", _time, "] prio=[", _priority, "] event_type=", _name,
                 " event=", toString());
    }

    void Event::addParticle(std::unique_ptr<ParticleInterface> s) {
        DBGENTER(_EVENT_DBG_LEV);
        DBGPRINT("Event name ",
                 toString() // demangle_compiler_name(typeid(*this).name())
        );
        _particles.push_back(std::move(s));
        DBGPRINT("size is now: ", _particles.size());
    }

} // namespace MetaSim
