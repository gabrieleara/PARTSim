#ifndef __CBSERVER_H__
#define __CBSERVER_H__

#include <server.hpp>
#include <capacitytimer.hpp>
#include <list>
#include "rttask.hpp"
#include <sstream>

namespace RTSim {
    using namespace MetaSim;

    class CBServer : public Server {
    public:
  typedef enum {ORIGINAL, REUSE_DLINE } policy_t;

  CBServer(Tick q, Tick p, Tick d, bool HR, const std::string &name, 
     const std::string &sched = "FIFOSched");

        void newRun() override;
        void endRun() override;
      
        Tick getBudget() const override { return Q;}
        Tick getPeriod() const override { return P;}

        Tick changeBudget(const Tick &n) override;

        Tick changeQ(const Tick &n);
        double getVirtualTime() override;
        Tick get_remaining_budget() const; 

        policy_t get_policy() const { return idle_policy; }
        void set_policy(policy_t p) { idle_policy = p; }

// ------------------------------------ functions added by me todo

      /**
       Add a new task to this server, with parameters
       specified in params.
               
       @params task the task to be added
       @params the scheduling parameters

       @see Scheduler
      */
      void addTask(AbsRTTask &task, const std::string &params = "") override {
        Server::addTask(task, params);

        _yielding = false;
      }

        /// Returns all tasks currently in the scheduler
        vector<AbsRTTask*> getAllTasks() const { return sched_->getTasks(); }

        /// Returns all tasks active in the server TODO IT MAY BE WRONG, but I cannot rely on sched_ (erase not working?)!!
        vector<AbsRTTask*> getTasks() const {
          //std::cout << "CBServerCallingEMRTKernel::" << __func__ << "()" << std::endl;
          vector<AbsRTTask*> res;
          vector<AbsRTTask*> tasks = getAllTasks();
          for (int i = 0; i < tasks.size(); i++) {
            Task *tt = dynamic_cast<Task*>(tasks.at(i));
            //std::cout << "\t" << tt->toString() << " arrtime " << tt->arrEvt.getTime() << std::endl;
            NonPeriodicTask *ntt = dynamic_cast<NonPeriodicTask*>(tt);
            printf("%f > %f && %d || %f == %f\n", (double) tt->arrEvt.getTime(), (double) SIMUL.getTime(), !tt->isActive(), (double) tt->endEvt.getTime(), (double) SIMUL.getTime());
            if ( ( tt->arrEvt.getTime() > SIMUL.getTime() && !tt->isActive() ) || tt->endEvt.getTime() == SIMUL.getTime() ) // todo make easier?
              continue;
            if ( (ntt != NULL && (tt->arrEvt.getTime() + tt->getDeadline() <= SIMUL.getTime() || !tt->isActive())) )
              continue;
            res.push_back(tt);
          }

          //std::cout << "\t-----------\n\tCBS::gettasks() t=" << SIMUL.getTime() << std::endl;
          //for (AbsRTTask* t:res)
          //  std::cout << "\t\t" << t->toString() << std::endl;
          //std::cout << "\tend tasks"<<std::endl;

          return res;
        }

        /// Tells if scheduler currently holds any task. Function not that much tested!
        bool isEmpty() const {
          vector<AbsRTTask*> tasks = getTasks();
          unsigned int numTasks = tasks.size();
          return numTasks == 0;
        }

        bool isKilled() const { return _killed; }
        
        /// Tells if task is in scheduler
        bool isInServer(AbsRTTask* t) {
            if (dynamic_cast<Server*>(t))
              return false;

            bool res = sched_->isFound(t);
            return res;
        }

        bool isYielding() const { return _yielding; }

        /// The server yield the core where it's running
        void yield() {
          _yielding = true;
        }

        /// Server to human-readable string
        string toString() const override { 
          std::stringstream s; 
          s << "\ttasks: [ " << sched_->toString() << "]" << (isYielding() ? " yielding":"") <<
            " (Q:" << getBudget() << ", P:" << getPeriod() << ")\tstatus: " << getStatusString();
          return s.str();
        }

// ------------------------------------- end fabm todo

    protected:
                
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

        /// from recharging to active contending (budget recharged)
        void recharging_idle() override;

        virtual void onReplenishment(Event *e);

        virtual void onIdle(Event *e);

        void prepare_replenishment(const Tick &t);
        
        void check_repl();

        /// True if CBS server has decided to yield core (= to leave it to ready tasks)
        bool _yielding;

    private:
        Tick Q,P,d;
        Tick cap; 
        Tick last_time;
        // // Never used
        // Tick recharging_time;
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

    protected:
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

        /// Is CBS server killed?
        bool _killed = false;
    };

    /**
      CBS server augmented specifically for EnergyMRTKernel.
      EMRTK. needs to know WCET to compute utilizations, and some callbacks to
      make decisions.

      Acronym: CBServer CEMRTK.
      */
    class CBServerCallingEMRTKernel : public CBServer {
    protected:
      /// same as the parent releasing_idle() but also calls EMRTKernel
      void releasing_idle() override;

      // same as parent, but also calls EMRTKernel
      void executing_releasing() override;

      /// from executing to recharging (budget exhausted)
      void executing_recharging() override;

    public:
      CBServerCallingEMRTKernel(Tick q, Tick p, Tick d, bool HR, const std::string &name, 
        const std::string &sched = "FIFOSched") : CBServer(q,p,d,HR,name,sched) { };

      /// Kills the server and its task. It can stay killed since now on or only until task next period
      void killInstance(bool onlyOnce = true);

      AbsRTTask* getFirstTask() const {
        AbsRTTask* t = sched_->getFirst();
        return t;
      }

      // todo useless (getDeadline())?
      Tick getNextActivation() const {
        Tick lastArrival = getArrival();
        Tick nextArrival = lastArrival + getPeriod();
        return nextArrival;
      }

      Tick getEndBandwidthEvent() const {
        return _bandExEvt.getTime();
      }

      Tick getIdleEvent() const {
        return _idleEvt.getTime();
      }

      double getEndOfVirtualTimeEvent() { return getVirtualTime(); }

      Tick getReplenishmentEvent() const { return _replEvt.getTime(); }

      /// Get WCET
      double getWCET(double capacity) const override {
        double wcet = 0.0;
 
        for (AbsRTTask* t : getTasks())
          wcet += double(dynamic_cast<Task*>(t)->getWCET());

        wcet = wcet / capacity;
        return wcet;
      }

      CPU* getProcessor(const AbsRTTask* t) const override;

      /// Get remaining WCET - practically WCET
      double getRemainingWCET(double capacity) const override {
        return getWCET(capacity);
      }

      /// Arrival event of task of server
      void onArrival(AbsRTTask *t) override {
        std::cout << "CBServerCallingEMRTKernel::" << __func__ << "(). " << t->toString() << std::endl;
        _yielding = false;
        Server::onArrival(t);
      }

      /// Task of server ends, callback
      void onEnd(AbsRTTask *t) override;

      /// On deschedule event (of server - and of tasks in it)
      void onDesched(Event *e) override {
        std::cout << "CBServerCallingEMRTKernel is empty? " << isEmpty() << std::endl;
        if (isEmpty())
            yield();
        else // could happend with non-periodic tasks
            Server::onDesched(e);
      }


      void onReplenishment(Event *e) override;

      /// Object to human-readable string
      string toString() const override {
        string s = "CBSCEMRTK " + getName() + ". " + CBServer::toString();
        return s;
      }

      /// Prints (std::cout) all events of CBS Server
      virtual void printEvts() const {
        std::cout << std::endl << toString();
        std::cout << "_bandExEvt: " << _bandExEvt.getTime() << ", ";
        std::cout << "_rechargingEvt: " << _rechargingEvt.getTime() << ", ";
        std::cout << "_idleEvt: " << _idleEvt.getTime() << ", ";
        std::cout << std::endl << std::endl;
      } 

    };

}


#endif
