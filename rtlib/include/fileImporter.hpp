#ifndef __FILE_IMPORTER_H__
#define __FILE_IMPORTER_H__

#include <fstream>
#include <iostream>
#include <string>
#include <cstdio>
#include <ctime>
#include <iomanip>

namespace RTSim {
    using namespace MetaSim;

    class StaffordImporter {
    public:

    static string generate(unsigned int period, double utilization) {
        char s[200]         = "";
        char filename[120]  = "";

        time_t n            = time(0);
        struct tm* _tm      = localtime(&n);
        char now[50]        = "";
        sprintf(now, "%d_%d_%d_%d_%d_%d", _tm->tm_mday, _tm->tm_mon + 1, _tm->tm_year + 1900, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);

        sprintf(filename, "taskset_generator/%s_P_%u_u_%f.txt", now, period, utilization);
        sprintf(s, "python taskset_generator/taskgen.py -s 8 -n 3 -p %u -u %f --round-C > %s", period, utilization, filename);

        if (std::ifstream(filename)) {
          remove(filename);
        }

        cout << "executing: " << s << endl;
        int res = system(s);
        cout << "file made" << endl;
        saveLastGenerated(filename);
        return string(filename);
    }

    /// Imports tasks from file. Returns CPU/set number -> (WCET, Period = Deadline) for all CPU, tasks.
    static map<unsigned int, pair<unsigned int, unsigned int>> importFromFile(const string& filename) {
       ifstream fd(filename.c_str());
       double trash, util, wcet, period;
       map<unsigned int, pair<unsigned int, unsigned int>> tasks;
       int i = 0;

       while (fd >> trash >> util >> wcet >> period) {
          tasks[i] = make_pair((unsigned int) wcet, (unsigned int) period);
          i++;
       }

       for (const auto &elem : tasks) {
          cout << elem.first << ")\twcet " << elem.second.first << " period " << elem.second.second << endl;
       }

       fd.close();
       cout << filename << endl;
       cout << "imported " << tasks.size() << endl;
       return tasks;
    }

    static vector<PeriodicTask*> getPeriodicTasks(const string& filename, const int experiment_no) {
        map<unsigned int, pair<unsigned int, unsigned int>> tasks = importFromFile(filename);
        vector<PeriodicTask*> res;
        char instr[60] = "";

        for (const auto &elem : tasks) {
            PeriodicTask* t = new PeriodicTask((int)elem.second.second, (int)elem.second.second, 0, "t_" + std::to_string(experiment_no) + "_" + std::to_string(elem.first));
            sprintf(instr, "fixed(%d, %s);", elem.second.first, "bzip2");
            t->insertCode(instr);
            if (t->getWCET(1.0) == 0.0) // stafford may generate tasks with WCET 0
              throw std::invalid_argument("Creating task with WCET 0");
            res.push_back(t);
        }

        cout << "read " << res.size() << endl;

        return res;
    }

    static vector<CBServerCallingEMRTKernel*> getEnvelopedPeriodcTasks(const string& filename, EnergyMRTKernel *kern, const int experiment_no = -1) {
        assert (filename != ""); assert (kern != NULL);
        vector<CBServerCallingEMRTKernel*> ets;
        vector<PeriodicTask*> tasks = getPeriodicTasks(filename, experiment_no);
        cout << "received " << tasks.size() << endl;

        for (PeriodicTask* t : tasks) {
            ets.push_back(kern->addTaskAndEnvelope(t, ""));
        }

        return ets;
    }


    static string getLastGenerated() {
        string filename = "";

        ifstream in("taskset_generator/saved.conf");
        in >> filename;
        in.close();

        assert(filename != "");

        return filename;
    }

  private:
    static void saveLastGenerated(const string& filename) {
        assert (filename != "");
        ofstream out("taskset_generator/saved.conf");
        out << filename;
        out.close();
    }

    };

}


#endif
