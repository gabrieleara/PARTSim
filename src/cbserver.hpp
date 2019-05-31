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

        void newRun();
        void endRun();
      
        virtual Tick getBudget() const { return Q;}
        virtual Tick getPeriod() const { return P;}

        virtual Tick changeBudget(const Tick &n);

        Tick changeQ(const Tick &n);
        virtual double getVirtualTime();
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
      virtual void addTask(AbsRTTask &task, const std::string &params = "") {
        Server::addTask(task, params);

        _yielding = false;
      }

        /// Returns all tasks currently in the scheduler
        vector<AbsRTTask*> getAllTasks() const { return sched_->getTasks(); }

        /// Returns all tasks active in the server TODO IT MAY BE WRONG, but I cannot rely on sched_ (erase not working?)!!
        vector<AbsRTTask*> getTasks() const {
          //cout << "CBServerCallingEMRTKernel::" << __func__ << "()" << endl;
          vector<AbsRTTask*> res;
          vector<AbsRTTask*> tasks = getAllTasks();
          for (int i = 0; i < tasks.size(); i++) {
            Task *tt = dynamic_cast<Task*>(tasks.at(i));
            //cout << "\t" << tt->toString() << " arrtime " << tt->arrEvt.getTime() << endl;
            NonPeriodicTask *ntt = dynamic_cast<NonPeriodicTask*>(tt);
            if ( ( tt->arrEvt.getTime() > SIMUL.getTime() && !tt->isActive() ) || tt->endEvt.getTime() == SIMUL.getTime() ) // todo make easier?
              continue;
            if ( (ntt != NULL && (tt->arrEvt.getTime() + tt->getDeadline() <= SIMUL.getTime() || !tt->isActive())) )
              continue;
            res.push_back(tt);
          }

          //cout << "\t-----------\n\tCBS::gettasks() t=" << SIMUL.getTime() << endl;
          //for (AbsRTTask* t:res)
          //  cout << "\t\t" << t->toString() << endl;
          //cout << "\tend tasks"<<endl;

          return res;
        }

        /// Tells if scheduler currently holds any task. Function not that much tested!
        bool isEmpty() const {
          vector<AbsRTTask*> tasks = getTasks();
          unsigned int numTasks = tasks.size();
          return numTasks == 0;
        }
        
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
        virtual string toString() const { 
          stringstream s; 
          s << "tasks: [ " << sched_->toString() << "]" << (isYielding() ? " yielding":"") <<
            " (Q:" << getBudget() << ", P:" << getPeriod() << ")";
          return s.str();
        }

// ------------------------------------- end fabm todo

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
      virtual void releasing_idle();

      // same as parent, but also calls EMRTKernel
      virtual void executing_releasing();

      /// from executing to recharging (budget exhausted)
      virtual void executing_recharging();
    public:
      CBServerCallingEMRTKernel(Tick q, Tick p, Tick d, bool HR, const std::string &name, 
        const std::string &sched = "FIFOSched") : CBServer(q,p,d,HR,name,sched) { };

      AbsRTTask* getFirstTask() const {
        AbsRTTask* t = sched_->getFirst();
        return t;
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
      virtual double getWCET(double capacity) const {
        double wcet = 0.0;
 
        for (AbsRTTask* t : getTasks())
          wcet += double(dynamic_cast<Task*>(t)->getWCET());

        wcet = wcet / capacity;
        return wcet;
      }

      /// Get remaining WCET - practically WCET
      virtual double getRemainingWCET(double capacity) const {
        return getWCET(capacity);
      }

      /// Arrival event of task of server
      virtual void onArrival(AbsRTTask *t) {
        cout << "CBServerCallingEMRTKernel::" << __func__ << "(). " << t->toString() << endl;
        _yielding = false;
        Server::onArrival(t);
      }

      /// Task of server ends, callback
      virtual void onEnd(AbsRTTask *t);

      /// On deschedule event (of server - and of tasks in it)
      virtual void onDesched(Event *e) {
        cout << "CBServerCallingEMRTKernel is empty? " << isEmpty() << endl;
        if (isEmpty())
            yield();
        else // could happend with non-periodic tasks
            Server::onDesched(e);
      }


      virtual void onReplenishment(Event *e);

      /// Object to human-readable string
      virtual string toString() const {
        string s = "CBServerCallingEMRTKernel " + getName() + ". " + CBServer::toString();
        return s;
      }

      /// Prints (cout) all events of CBS Server
      virtual void printEvts() {
        cout << endl << toString();
        cout << "_bandExEvt: " << _bandExEvt.getTime() << ", ";
        cout << "_rechargingEvt: " << _rechargingEvt.getTime() << ", ";
        cout << "_idleEvt: " << _idleEvt.getTime() << ", ";
        cout << endl << endl;
      } 

    };

}


#endif