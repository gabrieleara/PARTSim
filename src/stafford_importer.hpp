#ifndef __STAFFORD_IMPORTER_H__
#define __STAFFORD_IMPORTER_H__

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
       return tasks;
    }

    static vector<PeriodicTask*> getPeriodicTasks(cnst string& filename) {
        map<unsigned int, pair<unsigned int, unsigned int>> tasks;
        vector<PeriodicTask*> res;
        char instr[60] = "";

        for (const auto &elem : tasks) {
            PeriodicTask* t = new PeriodicTask(elem.second.second, elem.second.second, 0, "task_" + to_string(elem.first));
            sprintf(instr, "fixed(%d, %s);", elem.second.first, "bzip2");
            t->insertCode(instr);
        }

        return res;
    }

    static vector<CBServerCallingEMRTKernel*> getEnvelopedPeriodcTasks(const string& filename) {
        vector<CBServerCallingEMRTKernel*> ets;
        vector<PeriodicTask*> tasks = getPeriodicTasks(filename);

        for (PeriodicTask* t : tasks) {
            ets.push_back(kern->addTaskAndEnvelope(t, ""));
        }

        return ets;
    }

    };

}


#endif