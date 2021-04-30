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
#include <sys/time.h>

using namespace MetaSim;
using namespace RTSim;

static deque<PeriodicTask *> tasks;
static int tid = 0;
static double ustart = 0.9;
static int nmax = 10;
static int tokill = 1;
static double sat_factor = 0.0;

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

// compute the area as sum of horizontal rects (time on X axis)
static Tick computeMaxRuntime(Tick now, Tick period, double uavail, const std::vector<pair<Tick, double>>& expiring_uacts) {
  Tick ref_end = now + period;
  double max_runtime = uavail * double(ref_end - now);
  cout << "computeMaxRuntime: max_runtime=" << max_runtime << ", uavail=" << uavail << endl;
  for (auto it = expiring_uacts.begin(); it != expiring_uacts.end() && it->first <= ref_end; ++it) {
    max_runtime += it->second * double(ref_end - it->first);
    cout << "computeMaxRuntime: max_runtime=" << max_runtime << endl;
  }
  return Tick::floor(max_runtime);
}

int main(int argc, char *argv[]) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  long seed = ts.tv_sec * 1000000000L + ts.tv_nsec;
    --argc;  ++argv;
    while (argc > 0) {
      if (strcmp(*argv, "-h") == 0) {
        printf("Usage: edf-death-birth [-s seed][-n nmax_tasks][-k tokill][-u umax][-f sat_factor]\n");
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
      } else if (strcmp(*argv, "-f") == 0) {
        --argc;  ++argv;
        assert(argc > 0);
        sat_factor = atof(*argv);
        assert(sat_factor >= 0.0 && sat_factor <= 1.0);
      } else if (strcmp(*argv, "-k") == 0) {
        --argc;  ++argv;
        assert(argc > 0);
        tokill = atoi(*argv);
      }
      --argc;  ++argv;
    }

    cout << "Running with: seed=" << seed << ", nmax=" << nmax << ", tokill=" << tokill << ", utot=" << ustart << ", sat_factor=" << sat_factor << endl;

    assert(nmax > tokill);

    try {

        SIMUL.dbg.enable("All");
        // SIMUL.dbg.setStream("debug.txt");
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
        cout << "usum: " << usum << endl;

        for (int i = 0; i < n; i++) {
            Tick period = tset[i].second;
            Tick runtime = Tick(double(tset[i].first) * (ustart / usum));
            cout << "creating (normalized) runtime: " << runtime << ", period: " << period << endl;
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
        Tick maxvtime_now;
        do {
          Tick next = std::experimental::randint(int(getMaxPeriod() * 2), int(getMaxPeriod() * 10));
          cout << "Running till time " << next << endl;

          SIMUL.run_to(next);
          now = SIMUL.getTime();
          cout << "sim paused at: " << now << endl;

          killable = 0;
          maxvtime_now = 0;
          for (auto t: tasks) {
            Tick vtime_now = t->getArrival() + t->getExecTime() * t->getPeriod() / t->getWCET() - now;
            if (vtime_now > 10) {
              killable++;
              if (vtime_now > maxvtime_now)
                maxvtime_now = vtime_now;
            }
          }
        } while (killable < tokill || maxvtime_now < 100);

        std::vector<pair<Tick, double>> expiring_uacts;
        int nkilled = 0;
        for (int i = 0; i < tokill; i++) {
          for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            auto t = *it;
            double ukill = double(t->getWCET()) / double(t->getPeriod());
            Tick vtime = t->getArrival() + t->getExecTime() * t->getPeriod() / t->getWCET();
            if (vtime <= now + 10)
              continue;
            expiring_uacts.push_back(make_pair(vtime, double(t->getWCET()) / double(t->getPeriod())));
            tasks.erase(it);
            t->killInstance();
            ++nkilled;
            break;
          }
        }

        assert(nkilled == tokill);

        sort(expiring_uacts.begin(), expiring_uacts.end(),
             [](const pair<Tick, double>& p1, const pair<Tick, double>& p2) {
               return p1.first < p2.first;
             });

        cout << "expiring_uacts: ";
        for (auto p: expiring_uacts) {
          cout << "(" << p.first << ", " << p.second << "), ";
        }
        cout << endl;

        Tick a = expiring_uacts.front().first - now;
        Tick b = (expiring_uacts.back().first - now) * 2;
        cout << "a=" << a << ", b=" << b << endl;
        assert (a > 0 && b > 0);
        Tick period = std::experimental::randint((int)a, (int)b);
        double Uavail = 1.0 - utot;
        Tick min_runtime = Tick::floor(Uavail * double(period));
        Tick max_runtime = computeMaxRuntime(now, period, Uavail, expiring_uacts);
        Tick min_runtime_sat = min_runtime + Tick(sat_factor * double(max_runtime - min_runtime));
        Tick runtime = std::experimental::randint((int)min_runtime_sat, (int)max_runtime);
        cout << endl;
        cout << "New task: runtime=" << runtime << ", period=" << period << ", max_runtime=" << max_runtime << ", min_runtime_sat=" << min_runtime_sat << ", min_runtime=" << min_runtime << endl << endl;
        cout << "creating " << runtime << ", " << period << endl;
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
