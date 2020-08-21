/*
  In this example, a simple system is simulated, consisting of a few
  random real-time tasks scheduled by EDF on a single processor, where one
  task is killed at a random time, and another one with a random period and
  maximum possible budget is created, to check whether any deadline miss occurs
*/
#include <kernel.hpp>
#include <edfsched.hpp>
#include <jtrace.hpp>
#include <texttrace.hpp>
#include <json_trace.hpp>
#include <rttask.hpp>
#include <instr.hpp>

#include <experimental/random>

using namespace MetaSim;
using namespace RTSim;

static deque<PeriodicTask *> tasks;
static int tid = 0;

static double getUtot() {
  double usum = 0.0;
  for (auto t : tasks)
    usum += double(t->getWCET()) / double(t->getPeriod());
  return usum;
}

static Tick getMaxPeriod() {
  Tick period = 0;
  for (auto t : tasks)
    if (t->getPeriod() > period)
      period = t->getPeriod();
  return period;
}

int main()
{
    double ustart = 0.9;
    try {

        SIMUL.dbg.enable("All");
        SIMUL.dbg.setStream("debug.txt");
        // set the trace file. This can be visualized by the
        // rttracer tool

        TextTrace ttrace("trace.txt");

        cout << "Creating Scheduler and kernel" << endl;
        EDFScheduler edfsched;
        RTKernel kern(&edfsched);

        int n = std::experimental::randint(3, 10);
        vector<pair<Tick, Tick>> tset(n);
        double usum = 0.0;
        for (int i = 0; i < n; i++) {
            Tick period = std::experimental::randint(10, 20) * 100;
            Tick runtime = std::experimental::randint(int(period/4), int(period/2));
            cout << "generated " << runtime << ", " << period << endl;
            tset[i] = pair<Tick, Tick>(runtime, period);
            usum += double(runtime) / double(period);
        }

        for (int i = 0; i < n; i++) {
            Tick period = tset[i].second;
            Tick runtime = Tick(double(tset[i].first) * (ustart / usum));
            cout << "creating " << runtime << ", " << period << endl;
            PeriodicTask *t = new PeriodicTask(period, period, 0, string("T") + to_string(tid++));
            t->insertCode(string("fixed(") + to_string((int)runtime) + ");");
            ttrace.attachToTask(*t);
            kern.addTask(*t, "");
            tasks.push_back(t);
        }

        double utot = getUtot();
        cout << "ustart=" << ustart << ", Utot=" << utot << endl;

        SIMUL.initSingleRun();

        Tick next = std::experimental::randint(int(getMaxPeriod() * 2), int(getMaxPeriod() * 10));
        cout << "Running till time " << next << endl;

        SIMUL.run_to(next);
        cout << "sim paused at: " << SIMUL.getTime() << endl;

        PeriodicTask *t = tasks[0];
        tasks.pop_front();
        double ukill = double(t->getWCET()) / double(t->getPeriod());
        Tick rel_vtime = t->getExecTime() * t->getPeriod() / t->getWCET();
        Tick period = std::experimental::randint((int)rel_vtime, (int)t->getPeriod());
        Tick runtime = Tick(double(rel_vtime) * (1.0 - utot) + double(period - rel_vtime) * (1.0 - utot + ukill));

        t->killInstance();

        t = new PeriodicTask(period, period, 0, string("T") + to_string(tid++));
        t->insertCode(string("fixed(") + to_string((int)runtime) + ");");
        ttrace.attachToTask(*t);
        kern.addTask(*t, "");
        tasks.push_back(t);
        utot = getUtot();

        SIMUL.run_to(SIMUL.getTime() + getMaxPeriod() * 10);
        SIMUL.endSingleRun();
    } catch (BaseExc &e) {
        cout << e.what() << endl;
    }
}
