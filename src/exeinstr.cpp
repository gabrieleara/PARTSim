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
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

#include <factory.hpp>
#include <simul.hpp>
#include <strtoken.hpp>

#include <cpu.hpp>
#include <exeinstr.hpp>
#include <task.hpp>
#include <assert.h>

namespace RTSim {

    using namespace MetaSim;
    using namespace std;
    using namespace parse_util;

    ExecInstr::ExecInstr(Task *f,
                         unique_ptr<RandomVar> c,
                         const string &wl,
                         const string &n) :
        Instr(f, n), isBegOfInstr(false),
        cost(std::move(c)),
        workload(wl),
        execdTime(0),
        currentCost(0),
        actCycles(0),
        lastTime(0),
        executing(false),
        _endEvt(this) 
    {
        DBGTAG(_INSTR_DBG_LEV,"ExecInstr constructor");
        cout << "wl: " << wl<<endl;
    }

    
    ExecInstr::ExecInstr(const ExecInstr &other) : 
        Instr(other), isBegOfInstr(false),
        cost(other.cost->clone()),
        execdTime(0),
        currentCost(0),
        actCycles(0),
        lastTime(0),
        executing(false),
        _endEvt(this)
    {
        DBGTAG(_INSTR_DBG_LEV, "ExecInstr copy constructor");
    }

    
    ExecInstr::~ExecInstr()
    {
        DBGTAG(_INSTR_DBG_LEV, "ExecInstr::~ExecInstr() called");
    }    
    

    Instr *ExecInstr::createInstance(const vector<string> &par)
    {
        Instr *temp = 0;

        Task *task = dynamic_cast<Task *>(Entity::_find(par[par.size() - 1]));
        //if (isdigit((par[0].c_str())[0])) {
        if (isdigit(par[0][0])) {
            temp = new FixedInstr(task, atoi(par[0].c_str()));
        }
        else {
            string token = get_token(par[0]);
            string p = get_param(par[0]);
            vector<string> parms = split_param(p);

            unique_ptr<RandomVar> var(FACT(RandomVar).create(token,parms));
    
            if (var.get() == 0) throw ParseExc("ExecInstr", par[0]);
            
            temp = new ExecInstr(task, std::move(var));
        }
        return temp;
    }

    void ExecInstr::newRun() 
    {
        actCycles = lastTime = 0;
        isBegOfInstr = true;
        execdTime = 0;
        executing = false;
    }

    void ExecInstr::endRun() 
    {
        _endEvt.drop();
    }

    Tick ExecInstr::getExecTime() const 
    {
        Tick t = SIMUL.getTime();
        if (executing) return (execdTime + t - lastTime);
        else return execdTime;
    }

    Tick ExecInstr::getDuration() const 
    { 
        return (Tick)cost->get();
    }

    Tick ExecInstr::getWCET() const throw(RandomVar::MaxException)
    {
      Tick wcet;
      // todo code not flexible and only applies to Uniform distrib
      if (dynamic_cast<UniformVar*>(cost.get()) != NULL)
        wcet = (Tick) cost->get();
      else
        wcet = (Tick) cost->getMaximum();
        return wcet;
    }

    // Attention: here you decide when the WCET ends!
    void ExecInstr::schedule() throw (InstrExc)
    {
        DBGENTER(_INSTR_DBG_LEV);

        Tick t = SIMUL.getTime();
        lastTime = t;
        executing = true;

        if (isBegOfInstr) {  
            DBGPRINT_3("Initializing ExecInstr ",
                       getName(), 
                       " at first schedule.");
            DBGPRINT_2("Time executed during the prev. instance: ", 
                       execdTime);

            execdTime = 0;
            actCycles = 0;
            isBegOfInstr = false;
            currentCost = Tick(cost->get());

            DBGPRINT_2("Time to execute for this instance: ",
                       currentCost);
        }

        CPU *p = _father->getCPU();
        if (!dynamic_cast<CPU *>(p))
            throw InstrExc("No CPU!", "ExeInstr::schedule()");
        p->setWorkload(workload);

        double currentSpeed = p->getSpeed();


        DBGPRINT_2("father ", _father->toString());
        DBGPRINT_4("CPU ", p->getName(), " freq ", p->getFrequency());
        DBGPRINT_6(" currentCost ", currentCost, " actCycles ", actCycles, "Current speed ", currentSpeed);
        DBGPRINT_4(" result ", ((double)currentCost - actCycles)/currentSpeed, " to tick ", ceil( ((double)currentCost - actCycles)/currentSpeed) );
        Tick tmp = 0;
        if (((double)currentCost) > actCycles)
            tmp = (Tick) ceil( ((double)currentCost - actCycles)/currentSpeed);
        assert(tmp >= 0);
        _endEvt.post(t + tmp);
	      
        DBGPRINT("End of ExecInstr::schedule() ");
        
    }

    void ExecInstr::deschedule()
    {
        Tick t = SIMUL.getTime();

        DBGENTER(_INSTR_DBG_LEV);
        DBGPRINT("Descheduling ExecInstr named: " << getName());

        _endEvt.drop();

        if (executing) {
            CPU *p = _father->getOldCPU();
            if (!dynamic_cast<CPU *>(p)) 
                throw InstrExc("No CPU!", 
                               "ExeInstr::deschedule()");

            p->setWorkload("idle");

            double currentSpeed = p->getSpeed();

            actCycles += ((double)(t - lastTime))*currentSpeed;// number of cycles
            execdTime += (t - lastTime); // number of time ticks
            lastTime = t;
        }
        executing = false;
    }

    // void ExecInstr::setTrace(Trace *t) {
    //     attach_stat(*t, _endEvt); 
    //     //_endEvt.addTrace(t);
    // }

    void ExecInstr::onEnd() 
    {
        DBGENTER(_INSTR_DBG_LEV);
        DBGPRINT("Ending ExecInstr named: " << getName());

        Tick t = SIMUL.getTime();
        execdTime += t - lastTime;
        isBegOfInstr = true;
        executing = false;
        lastTime = t;
        actCycles = 0;
        _endEvt.drop();

        DBGPRINT("internal data set... now calling the _father->onInstrEnd()");

        _father->onInstrEnd();        
    }


    void ExecInstr::reset() 
    {
        DBGENTER(_INSTR_DBG_LEV);

        actCycles = lastTime = 0;
        isBegOfInstr = true;
        execdTime = 0;
        _endEvt.drop();

        DBGPRINT("internal data reset...");
    }

    void ExecInstr::refreshExec(double oldSpeed, double newSpeed){
        cout << "execinstr refreshExec from " << oldSpeed << " to " << newSpeed << endl;
        Tick t = SIMUL.getTime();
        _endEvt.drop();
        actCycles += ((double)(t - lastTime))*oldSpeed;
        execdTime += (t - lastTime);
        lastTime = t;
   
	if (isBegOfInstr)
		currentCost = getDuration();

        Tick tmp = 0;
        if (((double)currentCost) > actCycles) {
            tmp = (Tick) ceil ((((double) currentCost) - actCycles)/newSpeed);
            assert(tmp >= 0);
            cout << double(currentCost) << " " << execdTime << endl;
            cost->setMaximum(double(currentCost - execdTime));
        }
	   
        assert(tmp >= 0);
        _endEvt.post(t + tmp);
    }

    /*---------------------------- */

    FixedInstr::FixedInstr(Task *t,
                           Tick duration,
                           const string &wl,
                           const std::string &n) :
        ExecInstr(t,
                  unique_ptr<DeltaVar>(new DeltaVar(duration)),
                  wl,
                  n)
    {}

    unique_ptr<Instr> FixedInstr::createInstance(const vector<string> &par)
    {
        Task *task = dynamic_cast<Task *>(Entity::_find(par[par.size() - 1]));
        string workload = par.size() <= 2 ? "" : par[1];
        return unique_ptr<FixedInstr>(new FixedInstr(task, stoi(par[0]), workload));
    }
}
