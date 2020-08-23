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
#include <plist.hpp>

#include <experimental/random>

using namespace MetaSim;
using namespace RTSim;

static deque<PeriodicTask *> tasks;
static int tid = 0;
static double ustart = 0.9;
static int nmax = 10;
static int tokill = 1;

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

int main(int argc, char *argv[]) {
  int seed = time(NULL);
    --argc;  ++argv;
    while (argc > 0) {
      if (strcmp(*argv, "-h") == 0) {
        printf("Usage: edf-death-birth [-s seed][-n nmax_tasks][-u umax]\n");
        exit(0);
      } else if (strcmp(*argv, "-s") == 0) {
        --argc;  ++argv;
        assert(argc > 0);
        seed = atoi(*argv);
      } else if (strcmp(*argv, "-n") == 0) {
        --argc;  ++argv;
        assert(argc > 0);
        nmax = atoi(*argv);
      } else if (strcmp(*argv, "-u") == 0) {
        --argc;  ++argv;
        assert(argc > 0);
        ustart = atof(*argv);
        assert(ustart > 0.0 && ustart <= 1.0);
      }
      --argc;  ++argv;
    }

    assert(nmax > tokill);

    try {

        SIMUL.dbg.enable("All");
        SIMUL.dbg.setStream("debug.txt");
        // set the trace file. This can be visualized by the
        // rttracer tool

        TextTrace ttrace("trace.txt");

        cout << "Creating Scheduler and kernel" << endl;
        EDFScheduler edfsched;
        RTKernel kern(&edfsched);

        cout << "Simulation seed: " << seed << endl;
        std::experimental::reseed(seed);

        int n = std::experimental::randint(tokill + 1, nmax);
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

        int killable;
        Tick now;
        do {
          Tick next = std::experimental::randint(int(getMaxPeriod() * 2), int(getMaxPeriod() * 10));
          cout << "Running till time " << next << endl;

          SIMUL.run_to(next);
          now = SIMUL.getTime();
          cout << "sim paused at: " << now << endl;

          killable = 0;
          for (auto t: tasks) {
            Tick vtime = t->getArrival() + t->getExecTime() * t->getPeriod() / t->getWCET();
            if (vtime >= now)
              killable++;
          }
        } while (killable < tokill);

        std::vector<pair<Tick, double>> expiring_uacts;
        int nkilled = 0;
        for (int i = 0; i < tokill; i++) {
          for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            auto t = *it;
            double ukill = double(t->getWCET()) / double(t->getPeriod());
            Tick vtime = t->getArrival() + t->getExecTime() * t->getPeriod() / t->getWCET();
            if (vtime < now)
              continue;
            expiring_uacts.push_back(make_pair(vtime, double(t->getRemainingWCET()) / double(t->getPeriod())));
            tasks.erase(it);
            t->killInstance();
            ++nkilled;
            break;
          }
        }

        assert(nkilled == tokill);

        std:sort(expiring_uacts.begin(), expiring_uacts.end(),
                 [](const pair<Tick, double>& p1, const pair<Tick, double>& p2) {
                   return p1.first < p2.first;
                 });
        
        Tick a = expiring_uacts.front().first - now;
        Tick b = expiring_uacts.back().first + getMaxPeriod() - now;
        Tick period = std::experimental::randint((int)a, (int)b);
        Tick max_runtime = 0;
        Tick ref = now;
        Tick ref_end = now + period;
        double Uavail = 1.0 - utot;
        auto it = expiring_uacts.begin();
        Tick min_runtime = Tick(Uavail * double(std::min(it->first, ref_end) - ref));
        for (; it != expiring_uacts.end() && it->first <= ref_end; ++it) {
          max_runtime += Tick(Uavail * double(std::min(it->first, ref_end) - ref));
          Uavail += it->second;
          ref = it->first;
        }
        if (it == expiring_uacts.end())
          max_runtime += Tick(Uavail * double(ref_end - ref));

        Tick runtime = std::experimental::randint((int)min_runtime, (int)max_runtime);
        cout << endl << "runtime=" << runtime << ", period=" << period << ", max_runtime=" << max_runtime << ", min_runtime=" << min_runtime << endl << endl;
        PeriodicTask *t = new PeriodicTask(period, period, 0, string("T") + to_string(tid++));
        t->insertCode(string("fixed(") + to_string((int)runtime) + ");");
        ttrace.attachToTask(*t);
        kern.addTask(*t, "");
        tasks.push_back(t);
        utot = getUtot();

        SIMUL.run_to(now + getMaxPeriod() * 10);
        SIMUL.endSingleRun();
    } catch (BaseExc &e) {
        cout << e.what() << endl;
    }
}
