#ifndef __FILE_IMPORTER_H__
#define __FILE_IMPORTER_H__

#include <fstream>
#include <iostream>
#include <string>

namespace RTSim {
    using namespace MetaSim;

    class StaffordImporter {
    public:

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
          cout << "wcet " << elem.second.first << " period " << elem.second.second << endl;
       }

       fd.close();
       cout << filename << endl;
       cout << "imported " << tasks.size() << endl;
       return tasks;
    }

    static vector<PeriodicTask*> getPeriodicTasks(const string& filename) {
        map<unsigned int, pair<unsigned int, unsigned int>> tasks = importFromFile(filename);
        vector<PeriodicTask*> res;
        char instr[60] = "";

        for (const auto &elem : tasks) {
            PeriodicTask* t = new PeriodicTask((int)elem.second.second, (int)elem.second.second, 0, "task_" + to_string(elem.first));
            sprintf(instr, "fixed(%d, %s);", elem.second.first, "bzip2");
            t->insertCode(instr);
            res.push_back(t);
        }

        cout << "read " << res.size() << endl;

        return res;
    }

    static vector<CBServerCallingEMRTKernel*> getEnvelopedPeriodcTasks(const string& filename, EnergyMRTKernel *kern) {
        vector<CBServerCallingEMRTKernel*> ets;
        vector<PeriodicTask*> tasks = getPeriodicTasks(filename);
        cout << "received " << tasks.size() << endl;

        for (PeriodicTask* t : tasks) {
            ets.push_back(kern->addTaskAndEnvelope(t, ""));
        }

        return ets;
    }

    };

}


#endif