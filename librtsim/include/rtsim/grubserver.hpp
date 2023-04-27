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
#ifndef __GRUB_HPP__
#define __GRUB_HPP__

#include <rtsim/capacitytimer.hpp>
#include <rtsim/server.hpp>

#define _GRUB_DBG_LEV "grub"

namespace RTSim {
    using namespace MetaSim;

    class Grub;

    class GrubExc : public ServerExc {
    public:
        GrubExc(const string &m) : ServerExc(m, "Grub") {}
    };

    /** This supervisor stores the status of all registered servers,
        so to be able to compute U^act

        @todo (glipari) there should be one grub supervisor per CPU
    */
    class UtilizationManager : public Entity {
        std::vector<Grub *> servers;
        double total_u;
        Tick residual_capacity;
        double active_u;

    public:
        UtilizationManager(const std::string &name = "");
        ~UtilizationManager();
        bool addServer(Server *g);
        bool removeServer(Server *g);
        void set_active(Server *g);
        void set_idle(Server *g);
        void set_capacity(Tick cap) {
            residual_capacity = cap;
        }
        Tick get_capacity();

        double getActiveUtilization() {
            return active_u;
        }

        void newRun() override;
        void endRun() override;
    };

    class Grub : public Server {
    private:
        Tick Q, P, d;
        double util;
        Tick recharging_time;

        CapacityTimer cap;
        CapacityTimer vtime;
        UtilizationManager *supervisor;
        friend class UtilizationManager;
        void set_supervisor(UtilizationManager *s);

        GEvent<Grub> _idleEvt;
        friend class GEvent<Grub>;

    public:
        Grub(Tick q, Tick p, const std::string &name,
             const std::string &sched = "fifo");
        ~Grub();

        Tick getBudget() const override;
        Tick getPeriod() const override;
        double getUtil() const;

        void updateBudget();
        void startAccounting();

        Tick changeBudget(const Tick &new_budget) override;

        double getVirtualTime() override {
            return vtime.get_value();
        }

        void newRun() override;
        void endRun() override;

        // @todo (glipari) very strange function, is it correct ?
        double getWCET(double capacity) const override {
            return Q;
        }

        /// @todo (glipari) I just copied this function from the CBS,
        /// I do not know/remember what it is supposed to do ...
        double getRemainingWCET(double capacity) const override {
            return getWCET(capacity);
        }

    protected:
        void onIdle(Event *evt);

        /// from idle to active contending (new work to do)
        void idle_ready() override;

        /// from active non contending to active contending (more work)
        void releasing_ready() override;

        /// from active contending to executing (dispatching)
        void ready_executing() override;

        /// from executing to active contenting (preemption)
        void executing_ready() override;

        /// from executing to active non contending (no more work)
        void executing_releasing() override;

        /// from active non contending to idle (no lag)
        void releasing_idle() override;

        /// from executing to recharging (budget exhausted)
        void executing_recharging() override;

        /// from recharging to active contending (budget recharged)
        void recharging_ready() override;

        /// from recharging to idle (nothing remains to be done)
        void recharging_idle() override;
    };
} // namespace RTSim

#endif
