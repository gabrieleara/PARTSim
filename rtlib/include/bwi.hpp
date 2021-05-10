#ifndef __BWI_HPP__
#define __BWI_HPP__

#include "resmanager.hpp"
#include "server.hpp"
#include "scheduler.hpp"
#include <string>
#include <map>

namespace RTSim {

    class BWI : public ResManager {
        friend class Server;
    public:
        BWI(const string &n);
        ~BWI();

    protected:

        std::map<Server *, Scheduler *> schedulers;

        void addServer(Server *serv, Scheduler *sched);

        bool request(AbsRTTask *t, Resource *r, int n=1) override;
        void release(AbsRTTask *t, Resource *r, int n=1) override;
    };

}


#endif
