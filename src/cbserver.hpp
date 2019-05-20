#ifndef __CBSERVER_H__
#define __CBSERVER_H__

#include <server.hpp>
#include <capacitytimer.hpp>
#include <list>

namespace RTSim {
    using namespace MetaSim;

    class CBServer : public Server {
    public:
  typedef enum {ORIGINAL, REUSE_DLINE } policy_t;

  CBServer(Tick q, Tick p, Tick d, bool HR, const std::string &name, 
     const std::string &sched = "FIFOSched");

        void newRun();
        void endRun();
      
        virtual Tick getBudget() const { return Q;}
        virtual Tick getPeriod() const { return P;}

        Tick changeBudget(const Tick &n);

        Tick changeQ(const Tick &n);
        virtual double getVirtualTime();
      Tick get_remaining_budget(); 

      policy_t get_policy() const { return idle_policy; }
      void set_policy(policy_t p) { idle_policy = p; }
      
      /// Server to human-readable string
      virtual string toString() const { 
        string s = "tasks: [ "; 
        s += sched_->toString() + "]";
        return s;
      }

    protected:
                
        /// from idle to active contending (new work to do)
        virtual void idle_ready();

        /// from active non contending to active contending (more work)
        virtual void releasing_ready();
                
        /// from active contending to executing (dispatching)
        virtual void ready_executing();

        /// from executing to active contenting (preemption)
        virtual void executing_ready();

        /// from executing to active non contending (no more work)
        virtual void executing_releasing();

        /// from active non contending to idle (no lag)
        virtual void releasing_idle();

        /// from executing to recharging (budget exhausted)
        virtual void executing_recharging();

        /// from recharging to active contending (budget recharged)
        virtual void recharging_ready();

        /// from recharging to active contending (budget recharged)
        virtual void recharging_idle();

        void onReplenishment(Event *e);

        void onIdle(Event *e);

        void prepare_replenishment(const Tick &t);
        
        void check_repl();


    private:
        Tick Q,P,d;
        Tick cap; 
        Tick last_time;
        Tick recharging_time;
        int HR;
        
        /// replenishment: it is a pair of <t,b>, meaning that
        /// at time t the budget should be replenished by b.
        typedef std::pair<Tick, Tick> repl_t;

        /// queue of replenishments
        /// all times are in the future!
        std::list<repl_t> repl_queue;

        /// at the replenishment time, the replenishment is moved
        /// from the repl_queue to the capacity_queue, so 
        /// all times are in the past.
        std::list<repl_t> capacity_queue;

        /// A new event replenishment, different from the general
        /// "recharging" used in the Server class
        GEvent<CBServer> _replEvt;

        /// when the server becomes idle
        GEvent<CBServer> _idleEvt;

        CapacityTimer vtime;

        /** if the server is in IDLE, and idle_policy==true, the
            original CBS policy is used (that computes a new deadline
            as t + P) 
            If the server is IDLE and t < d and idle_policy==false, then 
            reuses the old deadline, and computes a new "safe" budget as 
            floor((d - vtime) * Q / P). 
        */
        policy_t idle_policy; 
    };



    class CBServerCallingEMRTKernel : public CBServer {
    protected:
      /// same as the parent releasing_idle() but also calls EMRTKernel
      virtual void releasing_idle();

      // same as parent, but also calls EMRTKernel
      virtual void executing_releasing();
    public:
      CBServerCallingEMRTKernel(Tick q, Tick p, Tick d, bool HR, const std::string &name, 
     const std::string &sched = "FIFOSched") : CBServer(q,p,d,HR,name,sched){};


      AbsRTTask* getFirstTask() const {
        AbsRTTask* t = sched_->getFirst();
        return t;
      }

      bool isInServer(AbsRTTask* t) {
        bool res = sched_->isFound(t);
        return res;
      }

      /// Get WCET
      virtual double getWCET(double capacity) const {
        double wcet = 0.0;

        for (AbsRTTask* t : tasks)
          wcet += double(dynamic_cast<Task*>(t)->getWCET());

        wcet = wcet / capacity;
        return wcet; 
      }

      /// Get remaining WCET - practically WCET
      virtual double getRemainingWCET(double capacity) const {
        return getWCET(capacity);
      }

      /// Task of server ends, callback
      virtual void onEnd(AbsRTTask *t);

      /// Object to human-readable string
      virtual string toString() const {
        string s = "CBServerCallingEMRTKernel. " + CBServer::toString();

        return s;
      } 

    };

}


#endif