#include <assert.h>
#include <iostream>

#include <rtsim/grubserver.hpp>

namespace RTSim {
    UtilizationManager::UtilizationManager(const std::string &name) :
        Entity(name),
        servers(),
        total_u(0),
        residual_capacity(0),
        active_u(0) {}

    UtilizationManager::~UtilizationManager() {}

    bool UtilizationManager::addServer(Server *s) {
        Grub *g = dynamic_cast<Grub *>(s);
        assert(g);
        if ((total_u + g->getUtil()) > 1)
            return false;
        servers.push_back(g);
        g->set_supervisor(this);
        total_u += g->getUtil();
        return true;
    }

    bool UtilizationManager::removeServer(Server *s) {
        Grub *g = dynamic_cast<Grub *>(s);
        assert(g);
        auto it = std::find(servers.begin(), servers.end(), g);
        if (it == servers.end()) return false;
        
        // @todo (glipari) to be discussed: can we remove a Grub that
        // is executing ?  in general the answer is yes, for example
        // during migration from one processor to another
        // one. However, the decrease of the active_u and total_u are
        // not immediate, we must wait for the zero-lag instant before
        // we can modify active_u and total_u.
        //
        // Two choices :
        // 
        // 1) we do this here: then we need to implement a
        //    sort of zero-lag timer again;
        // 2) we do this in Grub, as it is already
        //    implemented there.
        // 
        // The second choice is less general ... in fact, we force the
        // server to delay the migration until the zero lag, which may
        // be restrictive
        // 
        // The first choice implies moving a lot of grub-related
        // information inside the UtilisationManager (the lag itself
        // is specific to grub) and this will not work with e.g. CBS.
        //
        // A third option is to pass a second parameter, the zero-lag
        // time, and this manager will set a timer to expire on that
        // instant and remove the server from the list of servers. 

        // TO BE COMPLETED
        return false;
    }

    void UtilizationManager::set_active(Server *s) {
        Grub *g = dynamic_cast<Grub *>(s);
        assert(g);
        
        for (auto sp = servers.begin(); sp != servers.end(); ++sp)
            (*sp)->updateBudget();

        active_u += g->getUtil();

        for (auto sp = servers.begin(); sp != servers.end(); ++sp)
            (*sp)->startAccounting();
    }

    void UtilizationManager::set_idle(Server *s) {
        Grub *g = dynamic_cast<Grub *>(s);
        assert(s);
        
        for (auto sp = servers.begin(); sp != servers.end(); ++sp)
            (*sp)->updateBudget();

        active_u -= g->getUtil();

        for (auto sp = servers.begin(); sp != servers.end(); ++sp)
            (*sp)->startAccounting();
    }

    Tick UtilizationManager::get_capacity() {
        Tick c = residual_capacity;
        residual_capacity = 0;
        return c;
    }

    void UtilizationManager::newRun() {
        active_u = 0;
        residual_capacity = 0;
    }

    void UtilizationManager::endRun() {}

    /*----------------------------------------------------*/

    Grub::Grub(Tick q, Tick p, const std::string &name,
               const std::string &sched) :
        Server(name, sched),
        Q(q),
        P(p),
        d(0),
        util(double(Q) / double(P)),
        recharging_time(0),
        cap(),
        vtime(),
        supervisor(0),
        _idleEvt("going idle", this, &Grub::onIdle) {
    }

    Grub::~Grub() {}

    Tick Grub::getBudget() const {
        return Q;
    }
    Tick Grub::getPeriod() const {
        return P;
    }
    double Grub::getUtil() const {
        return util;
    }

    void Grub::updateBudget() {
        if (status == EXECUTING) {
            vtime.stop();
            cap.stop();
            _bandExEvt.drop();
        }
    }

    void Grub::startAccounting() {
        if (status == EXECUTING) {
            vtime.start(supervisor->getActiveUtilization() / getUtil());
            cap.start(-supervisor->getActiveUtilization());
            Tick delta = cap.get_intercept(0);
            if (delta < 0) {
                std::cerr << "Task: "
                          << dynamic_cast<Task *>(tasks[0])->getName()
                          << std::endl;
                std::cerr << "Time: " << SIMUL.getTime() << std::endl;
                std::cerr << "Status: " << status << " -- intercept: " << delta
                          << std::endl;
                std::cerr << "capacity: " << cap.get_value() << std::endl;
                std::cerr << "Supervisor utilization: "
                          << supervisor->getActiveUtilization() << std::endl;
                assert(0);
            }
            _bandExEvt.post(SIMUL.getTime() + cap.get_intercept(0));
        }
    }

    void Grub::onIdle(Event *evt) {
        assert(status == RELEASING);
        releasing_idle();
    }

    void Grub::idle_ready() {
        // std::cout << "IDLE-READY" << std::endl;
        DBGENTER(_SERVER_DBG_LEV);
        assert(status == IDLE);
        status = READY;
        cap.set_value(Q);
        d = SIMUL.getTime() + P;
        DBGPRINT("new deadline ", d);
        setAbsDead(d);
        vtime.set_value(SIMUL.getTime());
        supervisor->set_active(this);
        DBGPRINT("Going to READY at ", SIMUL.getTime());
    }

    void Grub::releasing_ready() {
        DBGENTER(_SERVER_DBG_LEV);
        status = READY;
        _idleEvt.drop();
        DBGPRINT("FROM RELEASING TO READY");
    }

    void Grub::ready_executing() {
        DBGENTER(_SERVER_DBG_LEV);
        status = EXECUTING;
        Tick extra = supervisor->get_capacity();
        cap.set_value(cap.get_value() + double(extra));
        startAccounting();
    }

    void Grub::executing_ready() {
        DBGENTER(_SERVER_DBG_LEV);
        updateBudget();
        status = READY;
    }

    void Grub::executing_releasing() {
        DBGENTER(_SERVER_DBG_LEV);
        updateBudget();
        status = RELEASING;
        if (SIMUL.getTime() < Tick(vtime.get_value()))
            _idleEvt.post(Tick(vtime.get_value()));
        else
            _idleEvt.post(SIMUL.getTime());
    }

    void Grub::releasing_idle() {
        DBGENTER(_SERVER_DBG_LEV);
        if (double(SIMUL.getTime()) > vtime.get_value()) {
            Tick extra =
                Tick(getUtil() * (double(SIMUL.getTime()) - vtime.get_value()));
            supervisor->set_capacity(extra);
        }
        status = IDLE;
        supervisor->set_idle(this);
    }

    void Grub::executing_recharging() {
        DBGENTER(_SERVER_DBG_LEV);
        updateBudget();
        status = RECHARGING;
        if (getDeadline() < SIMUL.getTime())
            _rechargingEvt.post(SIMUL.getTime());
        else
            _rechargingEvt.post(getDeadline());
    }

    void Grub::recharging_ready() {
        DBGENTER(_SERVER_DBG_LEV);
        cap.set_value(Q);
        d = SIMUL.getTime() + P;
        setAbsDead(d);
        vtime.set_value(SIMUL.getTime());
        status = READY;
    }

    void Grub::recharging_idle() {
        // no way
        // assert(0);
        // std::cout << "RECHARGING - IDLE!" << std::endl;
        releasing_idle();
    }

    Tick Grub::changeBudget(const Tick &new_budget) {
        // nothing to do now
        assert(0);
        return 0;
    }

    void Grub::set_supervisor(UtilizationManager *s) {
        supervisor = s;
    }

    void Grub::newRun() {
        Server::newRun();
        d = 0;
        recharging_time = 0;
    }

    void Grub::endRun() {
        Server::endRun();
    }

} // namespace RTSim
