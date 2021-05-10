#ifndef __BWI_HPP__
#define __BWI_HPP__

#include <resmanager.hpp>
#include <scheduler.hpp>
#include <server.hpp>
#include <map>
#include <string>

namespace RTSim {

    class BWI : public ResManager {
        friend class Server;

    public:
        BWI(const string &n);
        ~BWI();

    protected:
        std::map<Server *, Scheduler *> schedulers;

        void addServer(Server *serv, Scheduler *sched);

        bool request(AbsRTTask *t, Resource *r, int n = 1) override;
        void release(AbsRTTask *t, Resource *r, int n = 1) override;
    };

} // namespace RTSim

#endif
