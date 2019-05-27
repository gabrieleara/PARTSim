#include <metasim.hpp>
#include <rttask.hpp>
#include <mrtkernel.hpp>
#include <energyMRTKernel.hpp>
#include <edfsched.hpp>
#include <cbserver.hpp>

#include "catch.hpp"

using namespace MetaSim;
using namespace RTSim;
using namespace std;

#define FORCE_REQUISITES    1 /* 1 if you want to try all experiments */

string workload = "bzip2";
string task_name = "";
int init_sequence = 0;

class Requisite { 
    public:
        bool EMRTK_leave_little3, EMRTK_migrate, EMRTK_cbs_yield;
        bool satisfied;

        // in the default case, you don't want neither leave_little3 and migrate and CBS servers yielding
        Requisite(bool leave_little3 = false, bool migrate = false, bool cbs_yield = false)
            : EMRTK_leave_little3(leave_little3), EMRTK_migrate(migrate), EMRTK_cbs_yield(cbs_yield) { }

        string toString() const {
            string s = "Requisite with leave_little_3: " + to_string(EMRTK_leave_little3) + ", migrate: " + to_string(EMRTK_migrate) + ", cbs yielding: " + to_string(EMRTK_cbs_yield);
            return s;
        }
};

map<int, Requisite> performedTests;

void getCores(vector<CPU_BL*> &big, vector<CPU_BL*> &little, Island_BL **island_bl_little, Island_BL **island_bl_big);
int  init_suite(EnergyMRTKernel** kern);
bool inRange(int,int);
bool inRangeMinMax(double eval, const double min, const double max);
bool checkRequisites(Requisite reqs);

TEST_CASE("exp0") {
    init_sequence = 0;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;

    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);

    task_name = "T0_task1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
    kern->addTask(*t0, "");

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    CPU_BL *c0 = dynamic_cast<CPU_BL*>(dynamic_cast<CPU_BL*>(kern->getProcessor(t0)));

    REQUIRE (t0->getName() == "T0_task1");
    REQUIRE (c0->getFrequency() == 2000);
    REQUIRE (c0->getIslandType() == IslandType::BIG);
    REQUIRE (inRange(int(t0->getWCET(c0->getSpeed())), 497));

    SIMUL.endSingleRun();
    delete t0;
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

TEST_CASE("exp1") {
    init_sequence = 1;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;

    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);

    task_name = "T1_task1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
    kern->addTask(*t0, "");

    task_name = "T1_task2";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t1 = new PeriodicTask(500, 500, 0, task_name);
    t1->insertCode("fixed(500," + workload + ");");
    kern->addTask(*t1, "");

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    CPU_BL *c0 = dynamic_cast<CPU_BL*>(kern->getProcessor(t0));
    CPU_BL *c1 = dynamic_cast<CPU_BL*>(kern->getProcessor(t1));

    REQUIRE (t0->getName() == "T1_task1");
    REQUIRE (c0->getFrequency() == 2000);
    REQUIRE (c0->getIslandType() == IslandType::BIG);
    REQUIRE (inRange(int(t0->getWCET(c0->getSpeed())), 497));

    REQUIRE (t1->getName() == "T1_task2");
    REQUIRE (c1->getFrequency() == 2000);
    REQUIRE (c1->getIslandType() == IslandType::BIG);
    REQUIRE (inRange(int(t1->getWCET(c1->getSpeed())), 497));

    SIMUL.endSingleRun();
    delete t1; delete t0;
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

TEST_CASE("exp2") {
    init_sequence = 2;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;

    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);

    task_name = "T2_task1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
    kern->addTask(*t0, "");

    task_name = "T2_task2";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t1 = new PeriodicTask(500, 500, 0, task_name);
    t1->insertCode("fixed(250," + workload + ");");
    kern->addTask(*t1, "");

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    CPU_BL *c0 = dynamic_cast<CPU_BL*>(kern->getProcessor(t0));
    CPU_BL *c1 = dynamic_cast<CPU_BL*>(kern->getProcessor(t1));

    for(string s : kern->getRunningTasks())
      cout << "running :" << s<<endl;

    REQUIRE (t0->getName() == "T2_task1");
    REQUIRE (c0->getFrequency() == 2000);
    REQUIRE (c0->getIslandType() == IslandType::BIG);
    REQUIRE (inRange(int(t0->getWCET(c0->getSpeed())), 497));

    REQUIRE (t1->getName() == "T2_task2");
    REQUIRE (c1->getFrequency() == 2000);
    REQUIRE (c1->getIslandType() == IslandType::BIG);
    REQUIRE (inRange(int(t1->getWCET(c1->getSpeed())), 248));

    SIMUL.endSingleRun();
    delete t0; delete t1;
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

TEST_CASE("exp3") {
    init_sequence = 3;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;

    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);

    task_name = "T3_task1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(10," + workload + ");"); // WCET 10 at max frequency on big cores
    kern->addTask(*t0, "");

    SIMUL.initSingleRun();
    SIMUL.run_to(10);

    CPU_BL *c0 = dynamic_cast<CPU_BL*>(kern->getProcessor(t0));

    REQUIRE (t0->getName() == "T3_task1");
    REQUIRE (c0->getFrequency() == 500);
    REQUIRE (c0->getIslandType() == IslandType::LITTLE);
    REQUIRE (inRange(int(t0->getWCET(c0->getSpeed())), 65));

    SIMUL.endSingleRun();
    delete t0;
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

TEST_CASE("exp4") {
    init_sequence = 4;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;

    PeriodicTask* task[5]; // to be cleared after each test
    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);

    for (int j = 0; j < 4; j++) {
        task_name = "T4_Task_LITTLE_" + std::to_string(j);
        cout << "Creating task: " << task_name;
        PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
        char instr[60] = "";
        sprintf(instr, "fixed(100, %s);", workload.c_str());
        t->insertCode(instr);
        kern->addTask(*t, "");

        task[j] = t;
    }

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    CPU_BL *c0 = dynamic_cast<CPU_BL*>(kern->getProcessor(task[0]));
    CPU_BL *c1 = dynamic_cast<CPU_BL*>(kern->getProcessor(task[1]));
    CPU_BL *c2 = dynamic_cast<CPU_BL*>(kern->getProcessor(task[2]));
    CPU_BL *c3 = dynamic_cast<CPU_BL*>(kern->getProcessor(task[3]));

    REQUIRE (task[0]->getName() == "T4_Task_LITTLE_0");
    REQUIRE (c0->getFrequency() == 700);
    REQUIRE (c0->getIslandType() == IslandType::LITTLE);
    REQUIRE (inRange(int(task[0]->getWCET(c0->getSpeed())), 488));

    REQUIRE (task[1]->getName() == "T4_Task_LITTLE_1");
    REQUIRE (c1->getFrequency() == 700);
    REQUIRE (c1->getIslandType() == IslandType::LITTLE);
    REQUIRE (inRange(int(task[1]->getWCET(c1->getSpeed())), 488));

    REQUIRE (task[2]->getName() == "T4_Task_LITTLE_2");
    REQUIRE (c2->getFrequency() == 700);
    REQUIRE (c2->getIslandType() == IslandType::LITTLE);
    REQUIRE (inRange(int(task[2]->getWCET(c2->getSpeed())), 488));

    REQUIRE (task[3]->getName() == "T4_Task_LITTLE_3");
    REQUIRE (c3->getFrequency() == 700);
    REQUIRE (c3->getIslandType() == IslandType::LITTLE);
    REQUIRE (inRange(int(task[3]->getWCET(c3->getSpeed())), 488));

    SIMUL.endSingleRun();
    for (int j = 0; j < 4; j++)
        delete task[j];
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

TEST_CASE("exp5") {
    init_sequence = 5;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;

    vector<CPU_BL*> cpus;
    PeriodicTask* task[5]; // to be cleared after each test
    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);

    for (int j = 0; j < 4; j++) {
        int wcet = 5; //* (j+1);
        task_name = "T5_task" + std::to_string(j);
        cout << "Creating task: " << task_name;
        PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
        char instr[60] = "";
        sprintf(instr, "fixed(%d, %s);", wcet, workload.c_str());
        cout << " with abs. WCET " << wcet << endl;
        t->insertCode(instr);
        kern->addTask(*t, "");

        task[j] = t;
        // LITTLE_0, _1, _2, _3 freq 1400.
    }

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    CPU_BL *c0 = dynamic_cast<CPU_BL*>(kern->getProcessor(task[0]));
    CPU_BL *c1 = dynamic_cast<CPU_BL*>(kern->getProcessor(task[1]));
    CPU_BL *c2 = dynamic_cast<CPU_BL*>(kern->getProcessor(task[2]));
    CPU_BL *c3 = dynamic_cast<CPU_BL*>(kern->getProcessor(task[3]));
    cout << c0->toString() << endl;
    cout << c1->toString() << endl;
    cout << c2->toString() << endl;
    cout << c3->toString() << endl;

    PeriodicTask* t = task[0];
    REQUIRE (t->getName() == "T5_task0");
    REQUIRE (c0->getFrequency() == 500);
    REQUIRE (c0->getIslandType() == IslandType::LITTLE);
    REQUIRE (inRange(int(t->getWCET(c0->getSpeed())), 32));
    cout << t->toString() << " on "<< c0->toString()<<endl;

    t = task[1];
    REQUIRE (t->getName() == "T5_task1");
    REQUIRE (c1->getFrequency() == 500);
    REQUIRE (c1->getIslandType() == IslandType::LITTLE);
    REQUIRE (inRange(int(t->getWCET(c1->getSpeed())), 32));
    cout << t->toString() << " on "<< c1->toString()<<endl;

    t = task[2];
    REQUIRE (t->getName() == "T5_task2");
    REQUIRE (c2->getFrequency() == 500);
    REQUIRE (c2->getIslandType() == IslandType::LITTLE);
    REQUIRE (inRange(int(t->getWCET(c2->getSpeed())), 32));
    cout << t->toString() << " on "<< c2->toString()<<endl;

    t = task[3];
    REQUIRE (t->getName() == "T5_task3");
    REQUIRE (c3->getFrequency() == 500);
    REQUIRE (c3->getIslandType() == IslandType::LITTLE);
    REQUIRE (inRange(int(t->getWCET(c3->getSpeed())), 32));
    cout << t->toString() << " on "<< c3->toString()<<endl;

    SIMUL.endSingleRun();
    for (int j = 0; j < 4; j++)
        delete task[j];
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

// test showing that frequency of little/big island may be raised
TEST_CASE("exp6") {
    init_sequence = 6;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;

    vector<CPU_BL*> cpus;
    CPU_BL* cpu_task[5]; // to be cleared after each test
    PeriodicTask* task[5]; // to be cleared after each test
    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);

    int i, wcet = 300;
    for (int j = 0; j < 5; j++) {
        if (j == 4)
            wcet = 200;
        task_name = "T6_task" + std::to_string(j);
        cout << "Creating task: " << task_name;
        PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
        char instr[60] = "";
        sprintf(instr, "fixed(%d, %s);", wcet, workload.c_str());
        cout << " with abs. WCET " << wcet << endl;
        t->insertCode(instr);
        kern->addTask(*t, "");

        task[j] = t;
    }

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    for (int j = 0; j < 5; j++) {
    	cout << j << endl;
        cpu_task[j] = dynamic_cast<CPU_BL*>(kern->getProcessor(task[j]));
    	if (j == 4)
    		cpu_task[j] = kern->getProcessorReady(task[j]);
    }

    i = 0;
    PeriodicTask* t = task[i];
    CPU_BL* c = cpu_task[i];
    REQUIRE (t->getName() == "T6_task0");
    REQUIRE (c->getFrequency() == 2000);
    REQUIRE (c->getIslandType() == IslandType::BIG);
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 299));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 1;
    t = task[i];
    c = cpu_task[i];
    REQUIRE (t->getName() == "T6_task1");
    REQUIRE (c->getFrequency() == 2000);
    REQUIRE (c->getIslandType() == IslandType::BIG);
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 299));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 2;
    t = task[i];
    c = cpu_task[i];
    REQUIRE (t->getName() == "T6_task2");
    REQUIRE (c->getFrequency() == 2000);
    REQUIRE (c->getIslandType() == IslandType::BIG);
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 299));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 3;
    t = task[i];
    c = cpu_task[i];
    REQUIRE (t->getName() == "T6_task3");
    REQUIRE (c->getFrequency() == 2000);
    REQUIRE (c->getIslandType() == IslandType::BIG);
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())),299));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 4;
    t = task[i];
    c = cpu_task[i];
    REQUIRE (t->getName() == "T6_task4");
    REQUIRE (c->getFrequency() == 2000);
    REQUIRE (c->getIslandType() == IslandType::BIG);
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 199));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    SIMUL.endSingleRun();
    for (int j = 0; j < 5; j++)
        delete task[j];
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

TEST_CASE("exp7") {
    init_sequence = 7;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;

    vector<CPU_BL*> cpus;
    PeriodicTask* task[5]; // to be cleared after each test
    CPU_BL* cpu_task[5]; // to be cleared after each test
    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);

    int wcets[] = { 63, 63, 63, 63, 30 };
    int i;
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
        task_name = "T7_task" + std::to_string(j);
        cout << "Creating task: " << task_name;
        PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
        char instr[60] = "";
        sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
        t->insertCode(instr);
        kern->addTask(*t, "");

        task[j] = t;
    }

    /* Towards random workloads, but this time alg. first decides to
       schedule all tasks on littles, and then, instead of schedule the
       next one in bigs, it shall increase littles frequency so to make
       space to it too and save energy */

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
        cpu_task[j] = dynamic_cast<CPU_BL*>(kern->getProcessor(task[j]));
    }

    i = 0;
    PeriodicTask* t = task[i];
    CPU_BL* c = cpu_task[i];
    REQUIRE (t->getName() == "T7_task0");
    REQUIRE (c->getFrequency() == 500);
    REQUIRE (c->getIslandType() == IslandType::LITTLE);
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 415));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 1;
    t = task[i];
    c = cpu_task[i];
    REQUIRE (t->getName() == "T7_task1");
    REQUIRE (c->getFrequency() == 500);
    REQUIRE (c->getIslandType() == IslandType::LITTLE);
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 415));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 2;
    t = task[i];
    c = cpu_task[i];
    REQUIRE (t->getName() == "T7_task2");
    REQUIRE (c->getFrequency() == 500);
    REQUIRE (c->getIslandType() == IslandType::LITTLE);
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 415));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 3;
    t = task[i];
    c = cpu_task[i];
    REQUIRE (t->getName() == "T7_task3");
    REQUIRE (c->getFrequency() == 500);
    REQUIRE (c->getIslandType() == IslandType::LITTLE);
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 415));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 4;
    t = task[i];
    c = cpu_task[i];
    REQUIRE (t->getName() == "T7_task4");
    REQUIRE (c->getFrequency() == 700);
    REQUIRE (c->getIslandType() == IslandType::BIG);
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 75));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    SIMUL.endSingleRun();
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++)
        delete task[j];
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

TEST_CASE("exp8") {
    init_sequence = 8;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;

    vector<CPU_BL*> cpus;
    PeriodicTask* task[9]; // to be cleared after each test
    CPU_BL* cpu_task[9]; // to be cleared after each test
    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);

    int wcets[] = { 181, 419, 261, 163, 65, 8, 61, 170, 273 };
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
        task_name = "T8_task" + std::to_string(j);
        cout << "Creating task: " << task_name;
        PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
        char instr[60] = "";
        sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
        t->insertCode(instr);
        kern->addTask(*t, "");
        task[j] = t;
    }
    // towards random workloads...

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
        cpu_task[j] = dynamic_cast<CPU_BL*>(kern->getProcessor(task[j]));
    }

    int i = 0;
    PeriodicTask* t = task[i];
    CPU_BL* c = cpu_task[i];
    REQUIRE (t->getName() == "T8_task" + to_string(i));
    REQUIRE (c->getFrequency() == 1400);
    REQUIRE (c->getIslandType() == IslandType::LITTLE);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 499));

    i = 1;
    t = task[i];
    c = cpu_task[i];
    REQUIRE (t->getName() == "T8_task" + to_string(i));
    REQUIRE (c->getFrequency() == 1700);
    REQUIRE (c->getIslandType() == IslandType::BIG);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 477));

    i = 2;
    t = task[i];
    c = cpu_task[i];
    REQUIRE (t->getName() == "T8_task" + to_string(i));
    REQUIRE (c->getFrequency() == 1700);
    REQUIRE (c->getIslandType() == IslandType::BIG);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 297));

    i = 3;
    t = task[i];
    c = cpu_task[i];
    REQUIRE (t->getName() == "T8_task" + to_string(i));
    REQUIRE (c->getFrequency() == 1400);
    REQUIRE (c->getIslandType() == IslandType::LITTLE);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 449));

    i = 4;
    t = task[i];
    c = cpu_task[i];
    REQUIRE (t->getName() == "T8_task" + to_string(i));
    REQUIRE (c->getFrequency() == 1400);
    REQUIRE (c->getIslandType() == IslandType::LITTLE);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 179));

    i = 5;
    t = task[i];
    c = cpu_task[i];
    REQUIRE (t->getName() == "T8_task" + to_string(i));
    REQUIRE (c->getFrequency() == 1400);
    REQUIRE (c->getIslandType() == IslandType::LITTLE);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 22));

    i = 6;
    t = task[i];
    c = kern->getProcessorReady(t);
    REQUIRE (t->getName() == "T8_task" + to_string(i));
    cout << "a"<<endl;
    REQUIRE (c->getFrequency() == 1400);
    cout << "b"<<endl;
    REQUIRE (c->getIslandType() == IslandType::LITTLE);
    cout << "c"<<endl;
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 168));

    i = 7;
    t = task[i];
    c = kern->getProcessorReady(t);
    REQUIRE (t->getName() == "T8_task" + to_string(i));
    REQUIRE (c->getFrequency() == 1400);
    REQUIRE (c->getIslandType() == IslandType::LITTLE);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 468));

    i = 8;
    t = task[i];
    c = cpu_task[i];
    REQUIRE (t->getName() == "T8_task" + to_string(i));
    REQUIRE (c->getFrequency() == 1700);
    REQUIRE (c->getIslandType() == IslandType::BIG);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));
    REQUIRE (inRange(int(t->getWCET(c->getSpeed())), 310));

    SIMUL.endSingleRun();
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++)
        delete task[j];
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

TEST_CASE("exp9") {
    init_sequence = 9;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, true);
    if (!checkRequisites( req ))  return;

    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);
    vector<CPU_BL *> cpus_little = kern->getIslandLittle()->getProcessors();
    vector<CPU_BL *> cpus_big = kern->getIslandBig()->getProcessors();

    int wcets[] = { 101,101,101,8,   200,500,500,500,   101, 1  }; // 9 tasks
    vector<PeriodicTask*> tasks;
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
        task_name = "T" + to_string(init_sequence) + "_task" + to_string(j);
        cout << "Creating task: " << task_name;
        PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
        char instr[60] = "";
        sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
        t->insertCode(instr);
        kern->addTask(*t, "");
        tasks.push_back(t);
    }
    EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
    k->addForcedDispatch(tasks[0], cpus_little[0], 6);
    k->addForcedDispatch(tasks[1], cpus_little[1], 6);
    k->addForcedDispatch(tasks[2], cpus_little[2], 6);
    k->addForcedDispatch(tasks[3], cpus_little[3], 6);

    k->addForcedDispatch(tasks[4], cpus_big[0], 18);
    k->addForcedDispatch(tasks[5], cpus_big[1], 18);
    k->addForcedDispatch(tasks[6], cpus_big[2], 18);
    k->addForcedDispatch(tasks[7], cpus_big[3], 18);

    k->addForcedDispatch(tasks[8], cpus_big[3], 18);
    k->addForcedDispatch(tasks[9], cpus_big[3], 18);

    SIMUL.initSingleRun();
    SIMUL.run_to(36);

    REQUIRE (k->getProcessor(tasks[0])->getName() == cpus_little[0]->getName());
    REQUIRE (k->getProcessor(tasks[1])->getName() == cpus_little[1]->getName());
    REQUIRE (k->getProcessor(tasks[2])->getName() == cpus_little[2]->getName());
    //REQUIRE (k->getProcessor(tasks[3])->getName() == cpus_little[3]->getName()); has ended already

    REQUIRE (k->getProcessor(tasks[4])->getName() == cpus_big[0]->getName());
    REQUIRE (k->getProcessor(tasks[5])->getName() == cpus_big[1]->getName());
    REQUIRE (k->getProcessor(tasks[6])->getName() == cpus_big[2]->getName());
    REQUIRE (k->getProcessor(tasks[7])->getName() == cpus_big[3]->getName());

    // task8 comes in place of task3
    REQUIRE (k->getProcessor(tasks[8])->getName() == cpus_little[3]->getName());

    SIMUL.run_to(199);

    REQUIRE (k->getProcessor(tasks[0])->getName() == cpus_little[0]->getName());
    REQUIRE (k->getProcessor(tasks[1])->getName() == cpus_little[1]->getName());
    REQUIRE (k->getProcessor(tasks[2])->getName() == cpus_little[2]->getName());
    //REQUIRE (k->getProcessor(tasks[3])->getName() == cpus_little[3]->getName()); has ended already

    //REQUIRE (k->getProcessor(tasks[4])->getName() == cpus_big[0]->getName()); has ended already
    REQUIRE (k->getProcessor(tasks[5])->getName() == cpus_big[1]->getName());
    REQUIRE (k->getProcessor(tasks[6])->getName() == cpus_big[2]->getName());
    REQUIRE (k->getProcessor(tasks[7])->getName() == cpus_big[3]->getName());

    // task9 comes in place of task4
    REQUIRE (k->getProcessor(tasks[9])->getName() == cpus_big[0]->getName());

    SIMUL.run_to(500);

    REQUIRE (k->getProcessor(tasks[0])->getName() == cpus_little[0]->getName());
    REQUIRE (k->getProcessor(tasks[1])->getName() == cpus_little[1]->getName());
    REQUIRE (k->getProcessor(tasks[2])->getName() == cpus_little[2]->getName());
    REQUIRE (k->getProcessor(tasks[3])->getName() == cpus_little[3]->getName()); // has ended already

    REQUIRE (k->getProcessor(tasks[4])->getName() == cpus_big[0]->getName()); // has ended already
    REQUIRE (k->getProcessor(tasks[5])->getName() == cpus_big[1]->getName());
    REQUIRE (k->getProcessor(tasks[6])->getName() == cpus_big[2]->getName());
    REQUIRE (k->getProcessor(tasks[7])->getName() == cpus_big[3]->getName());

    SIMUL.run_to(536);

    REQUIRE (k->getProcessor(tasks[8])->getName() == cpus_little[3]->getName());

    SIMUL.run_to(941);

    REQUIRE (k->getProcessor(tasks[9])->getName() == cpus_little[0]->getName());    

    SIMUL.run_to(1000);
    SIMUL.endSingleRun();
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++)
        delete tasks[j];
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

TEST_CASE("exp10") {
	/**
	    At time 0, you have a task on a CPU, which is under a context switch. Say it lasts 8 ticks.
	    Then, another task, more important, arrives at time 6. Would this last task begin its
	    context switch at time 8, thus being scheduled at time 8+8=16?
	    Experiment requires EDF scheduler.
	  */
	init_sequence = 10;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;

    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);
    vector<CPU_BL *> cpus_little = kern->getIslandLittle()->getProcessors();
    vector<CPU_BL *> cpus_big = kern->getIslandBig()->getProcessors();

	int wcets[] = { 30,  30  };
	int deadl[] = { 500, 400 };
	int activ[] = { 0,   6   };
	vector<PeriodicTask*> tasks;
	for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
	    task_name = "T" + to_string(init_sequence) + "_task" + to_string(j);
	    cout << "Creating task: " << task_name;
	    PeriodicTask* t = new PeriodicTask(deadl[j], deadl[j], 0, task_name);
	    char instr[60] = "";
	    sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
	    t->insertCode(instr);
	    kern->addTask(*t, "");
	    tasks.push_back(t);
	}
	EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
	k->setContextSwitchDelay(Tick(8));
	k->addForcedDispatch(tasks[0], cpus_little[0], 6);
	k->addForcedDispatch(tasks[1], cpus_little[0], 6);

	SIMUL.initSingleRun();
	tasks[1]->activate(Tick(activ[1]));
	SIMUL.run_to(17);

	REQUIRE(k->getProcessor(tasks[1]) == cpus_little[0]);
	REQUIRE(k->getProcessorReady(tasks[0]) == cpus_little[0]);

	SIMUL.run_to(156);

	REQUIRE(k->getProcessor(tasks[0]) == cpus_little[0]);
	REQUIRE(tasks[1]->isActive() == false);

	SIMUL.endSingleRun();
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++)
        delete tasks[j];
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

TEST_CASE("exp12") {
    /**
      * Testing RRScheduler. 2 tasks on the same processors.
      * At 500, they go into two different ones.
      */
    init_sequence = 12;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;

    vector<Scheduler*> schedulers;
    vector<RTKernel*> kernels;
    vector<CPU_BL *> cpus_little, cpus_big;
    Island_BL *island_bl_little, *island_bl_big;
    getCores(cpus_little, cpus_big, &island_bl_little, &island_bl_big);
    
    RRScheduler *rrsched = new RRScheduler(100); // 100 is result of sysctl kernel.sched_rr_timeslice_ms on my machine, L5.0.2
    rrsched->disable();
    rrsched->setName("RRScheduler for arrival queue");
    for (int i = 0; i < 8; i++) {
        //delete schedulers[i];
        Scheduler *s = new RRScheduler(100);
        s->setName("RRScheduler #" + to_string(i));
        schedulers.push_back(s);
    }
    EnergyMRTKernel *kern = new EnergyMRTKernel(schedulers, rrsched, island_bl_big, island_bl_little, "Round Robin");
    kernels.push_back(kern);

    int wcets[] = { 30, 30, 30  };
    int deadl[] = { 500, 500, 500 };
    vector<PeriodicTask*> tasks;
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
        task_name = "T" + to_string(init_sequence) + "_task" + to_string(j);
        cout << "Creating task: " << task_name;
        PeriodicTask* t = new PeriodicTask(deadl[j], deadl[j], 0, task_name);
        char instr[60] = "";
        sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
        t->insertCode(instr);
        kern->addTask(*t, "");
        tasks.push_back(t);
    }
    kern->addForcedDispatch(tasks[0], cpus_little[0], 6);
    kern->addForcedDispatch(tasks[1], cpus_little[0], 6);

    CPU_BL::referenceFrequency = 2000; // BIG_3 frequency

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    REQUIRE (kern->getProcessor(tasks.at(0)) == cpus_little[0]);
    REQUIRE (kern->getProcessorReady(tasks.at(1)) == kern->getProcessor(tasks.at(0)));
    REQUIRE (dynamic_cast<CPU_BL*>(kern->getProcessor(tasks.at(2)))->getIslandType() == IslandType::LITTLE);

    SIMUL.run_to(101);
    REQUIRE (kern->getProcessor(tasks.at(1)) == cpus_little[0]);
    REQUIRE (kern->getProcessorReady(tasks.at(0)) == kern->getProcessor(tasks.at(1)));
    REQUIRE (dynamic_cast<CPU_BL*>(kern->getProcessor(tasks.at(2)))->getIslandType() == IslandType::LITTLE);

    SIMUL.run_to(201);
    REQUIRE (kern->getProcessor(tasks.at(0)) == cpus_little[0]);
    REQUIRE (kern->getProcessor(tasks.at(0)) == kern->getProcessorReady(tasks.at(1)));
    REQUIRE (!tasks.at(2)->isActive());

    SIMUL.run_to(232);
    REQUIRE (kern->getProcessor(tasks.at(1)) == cpus_little[0]);
    REQUIRE (!tasks.at(0)->isActive());

    SIMUL.run_to(263);
    REQUIRE (!tasks.at(1)->isActive());

    SIMUL.run_to(501);
    REQUIRE (dynamic_cast<CPU_BL*>(kern->getProcessor(tasks.at(0)))->getIslandType() == IslandType::LITTLE);
    REQUIRE (dynamic_cast<CPU_BL*>(kern->getProcessor(tasks.at(1)))->getIslandType() == IslandType::LITTLE);
    REQUIRE (dynamic_cast<CPU_BL*>(kern->getProcessor(tasks.at(2)))->getIslandType() == IslandType::LITTLE);
    REQUIRE (kern->getProcessor(tasks.at(0)) != kern->getProcessor(tasks.at(1)));
    REQUIRE (kern->getProcessor(tasks.at(0)) != kern->getProcessor(tasks.at(2)));
    REQUIRE (kern->getProcessor(tasks.at(1)) != kern->getProcessor(tasks.at(2)));

    SIMUL.run_to(601);
    REQUIRE (dynamic_cast<CPU_BL*>(kern->getProcessor(tasks.at(0)))->getIslandType() == IslandType::LITTLE);
    REQUIRE (dynamic_cast<CPU_BL*>(kern->getProcessor(tasks.at(1)))->getIslandType() == IslandType::LITTLE);
    REQUIRE (dynamic_cast<CPU_BL*>(kern->getProcessor(tasks.at(2)))->getIslandType() == IslandType::LITTLE);
    REQUIRE (kern->getProcessor(tasks.at(0)) != kern->getProcessor(tasks.at(1)));
    REQUIRE (kern->getProcessor(tasks.at(0)) != kern->getProcessor(tasks.at(2)));
    REQUIRE (kern->getProcessor(tasks.at(1)) != kern->getProcessor(tasks.at(2)));

    SIMUL.run_to(699);
    REQUIRE (!tasks.at(0)->isActive());
    REQUIRE (!tasks.at(1)->isActive());
    REQUIRE (!tasks.at(2)->isActive());

    SIMUL.endSingleRun();
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++)
        delete tasks[j];
    delete kern;
    cout << "End of Experiment #" << init_sequence << " (RR)" << endl;
}

TEST_CASE("exp13") {
    /**
        Demostrating what happens when a task is killed.
        It will come into play again in its next period and hopefully it can 
        be scheduled.
      */
    init_sequence = 13;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;
    performedTests[init_sequence] = req;

    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);
    vector<RTKernel*> kernels = { kern };
    vector<AbsRTTask*> tasks;
    vector<CPU_BL *> cpus_little = kern->getIslandLittle()->getProcessors();
    vector<CPU_BL *> cpus_big = kern->getIslandBig()->getProcessors();


    int wcets[] = { 450, 450, 450, 450, 1 };
    int deads[] = { 500, 500, 500, 500, 500 };
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
        task_name = "T13_task_BIG_" + to_string(j);
        cout << "Creating task: " << task_name;
        PeriodicTask* t = new PeriodicTask(deads[j], deads[j], 0, task_name);
        char instr[60] = "";
        sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
        cout << instr << endl;
        t->insertCode(instr);
        kernels[0]->addTask(*t, "");
        tasks.push_back(t);

        t->killOnMiss(true);
    }
    EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
    k->addForcedDispatch(tasks[0], cpus_big[0], 18);
    k->addForcedDispatch(tasks[1], cpus_big[1], 18);
    k->addForcedDispatch(tasks[2], cpus_big[2], 18);
    k->addForcedDispatch(tasks[3], cpus_big[3], 18);

    cpus_little[0]->toggleDisabled();
    cpus_little[1]->toggleDisabled();
    cpus_little[2]->toggleDisabled();
    cpus_little[3]->toggleDisabled();


    SIMUL.initSingleRun();
    SIMUL.run_to(440);
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]) - 1; j++){
        cout << "killing tasks" << endl;
        Task *t = dynamic_cast<Task*>(tasks[j]);
        t->killInstance();
        REQUIRE (t->getState() == TSK_IDLE);
    }
    SIMUL.run_to(501);
    REQUIRE (dynamic_cast<CPU_BL*>(k->getProcessor(tasks[0]))->getIslandType() == IslandType::BIG);
    REQUIRE (dynamic_cast<CPU_BL*>(k->getProcessor(tasks[1]))->getIslandType() == IslandType::BIG);
    REQUIRE (dynamic_cast<CPU_BL*>(k->getProcessor(tasks[2]))->getIslandType() == IslandType::BIG);
    REQUIRE (dynamic_cast<CPU_BL*>(k->getProcessor(tasks[3]))->getIslandType() == IslandType::BIG);
    REQUIRE (dynamic_cast<CPU_BL*>(k->getProcessorReady(tasks[4]))->getIslandType() == IslandType::BIG);

    SIMUL.run_to(1000);
    SIMUL.endSingleRun();
    
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++)
        delete tasks[j];
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

TEST_CASE("exp14") {
    /**
        What happens if task is not schedulable?

        @Mr. Cucinotta, Mr. Marinoni:
        This example also demonstrates that a task is either on the queue/scheduler
        of arrived tasks or in the one of the core selected for its dispatching.
        Moreover, if a task cannot be scheduled in its current, it has another chance
        on the next one.
      */
    init_sequence = 14;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;
    performedTests[init_sequence] = req;

    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);
    vector<RTKernel*> kernels = { kern };
    vector<AbsRTTask*> tasks;
    vector<CPU_BL *> cpus_little = kern->getIslandLittle()->getProcessors();
    vector<CPU_BL *> cpus_big = kern->getIslandBig()->getProcessors();


    cpus_little[0]->toggleDisabled();
    cpus_little[1]->toggleDisabled();
    cpus_little[2]->toggleDisabled();
    cpus_little[3]->toggleDisabled();
    cpus_big[1]->toggleDisabled();
    cpus_big[2]->toggleDisabled();
    cpus_big[3]->toggleDisabled();

    int wcets[] = { 200, 300, 450, 450};
    int deads[] = { 500, 500, 500, 500 };
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
        task_name = "T14_task_BIG_" + to_string(j);
        cout << "Creating task: " << task_name;
        PeriodicTask* t = new PeriodicTask(deads[j], deads[j], 0, task_name);
        char instr[60] = "";
        sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
        cout << instr << endl;
        t->insertCode(instr);
        kernels[0]->addTask(*t, "");
        tasks.push_back(t);

        t->killOnMiss(false);
    }

    SIMUL.initSingleRun();

    SIMUL.run_to(1);
    EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
    REQUIRE (dynamic_cast<CPU_BL*>(k->getProcessor(tasks[0])) == cpus_big[0]);
    REQUIRE (dynamic_cast<CPU_BL*>(k->getProcessorReady(tasks[1])) == cpus_big[0]);
    REQUIRE (k->getScheduler()->isInQueue(tasks[0]) == false); // an executing task
    REQUIRE (k->getScheduler()->isInQueue(tasks[1]) == false); // a ready task
    REQUIRE (k->getScheduler()->isInQueue(tasks[2]) == false); // a discarded task
    REQUIRE (k->getScheduler()->isInQueue(tasks[3]) == false); // a discarded task

    REQUIRE (k->getEnergyMultiCoresScheds()->getScheduler(k->getProcessor(tasks[0]))->isInQueue(tasks[0]) == true);
    REQUIRE (k->getEnergyMultiCoresScheds()->getScheduler(k->getProcessor(tasks[0]))->isInQueue(tasks[1]) == true);
    REQUIRE (k->getEnergyMultiCoresScheds()->isInAnyQueue(tasks[2]) == NULL);
    REQUIRE (k->getEnergyMultiCoresScheds()->isInAnyQueue(tasks[3]) == NULL);

    SIMUL.run_to(501);
    // same tests as above
    REQUIRE (dynamic_cast<CPU_BL*>(k->getProcessor(tasks[0])) == cpus_big[0]);
    REQUIRE (dynamic_cast<CPU_BL*>(k->getProcessorReady(tasks[1])) == cpus_big[0]);
    REQUIRE (k->getScheduler()->isInQueue(tasks[0]) == false); // an executing task
    REQUIRE (k->getScheduler()->isInQueue(tasks[1]) == false); // a ready task
    REQUIRE (k->getScheduler()->isInQueue(tasks[2]) == false); // a discarded task
    REQUIRE (k->getScheduler()->isInQueue(tasks[3]) == false); // a discarded task

    REQUIRE (k->getEnergyMultiCoresScheds()->getScheduler(k->getProcessor(tasks[0]))->isInQueue(tasks[0]) == true);
    REQUIRE (k->getEnergyMultiCoresScheds()->getScheduler(k->getProcessor(tasks[0]))->isInQueue(tasks[1]) == true);
    REQUIRE (k->getEnergyMultiCoresScheds()->isInAnyQueue(tasks[2]) == NULL);
    REQUIRE (k->getEnergyMultiCoresScheds()->isInAnyQueue(tasks[3]) == NULL);

    SIMUL.run_to(1000);
    SIMUL.endSingleRun();
    
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++)
        delete tasks[j];
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

TEST_CASE("exp15, starting with CBS") {
    /**
        Towards servers...y màs allà!
     */
    init_sequence = 15;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;
    performedTests[init_sequence] = req;

    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);
    vector<RTKernel*> kernels = { kern };
    vector<AbsRTTask*> tasks;
    vector<CPU_BL *> cpus_little = kern->getIslandLittle()->getProcessors();
    vector<CPU_BL *> cpus_big = kern->getIslandBig()->getProcessors();


    PeriodicTask *t2 = new PeriodicTask(15, 15 , 0, "TaskA"); 
    t2->insertCode("fixed(4,bzip2);");
    t2->setAbort(false);

    CBServerCallingEMRTKernel *serv = new CBServerCallingEMRTKernel(4, 15, 15, "hard",  "server1", "FIFOSched");
    serv->addTask(*t2);
    tasks.push_back(serv);
    tasks.push_back(t2);
    kernels[0]->addTask(*serv, "");

    EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
    k->addForcedDispatch(tasks[0], cpus_big[0], 18, 999);

    SIMUL.initSingleRun();
    
    SIMUL.run_to(1);
    REQUIRE (dynamic_cast<CPU_BL*>(k->getProcessor(tasks[0]))->getIslandType() == IslandType::BIG);

    SIMUL.run_to(5);
    REQUIRE (dynamic_cast<Task*>(t2)->getState() == TSK_IDLE);

    SIMUL.run_to(16);
    REQUIRE (dynamic_cast<CPU_BL*>(k->getProcessor(tasks[0]))->getIslandType() == IslandType::BIG);

    SIMUL.run_to(20);
    REQUIRE (dynamic_cast<Task*>(t2)->getState() == TSK_IDLE);

    SIMUL.run_to(50);

    SIMUL.endSingleRun();
    
    for (int j = 0; j < tasks.size(); j++)
        delete tasks[j];
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

TEST_CASE("Experiment 16") {
    /**
        Towards servers. Reproducing Mr.Cucinotta's example.
        A server with (Q=2,T=10) and a task arriving at 0 and ending at 2, period 10.
        Its active utilization is kept until 10.
     */
    init_sequence = 16;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false);
    if (!checkRequisites( req ))  return;
    performedTests[init_sequence] = req;

    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);
    vector<RTKernel*> kernels = { kern };
    vector<AbsRTTask*> tasks;
    vector<CPU_BL *> cpus_little = kern->getIslandLittle()->getProcessors();
    vector<CPU_BL *> cpus_big = kern->getIslandBig()->getProcessors();


    PeriodicTask *t2 = new PeriodicTask(10, 10 , 0, "TaskA"); 
    t2->insertCode("fixed(2,bzip2);");
    t2->setAbort(false);

    CBServerCallingEMRTKernel *serv = new CBServerCallingEMRTKernel(2, 10, 10, "hard",  "server1", "FIFOSched");
    serv->addTask(*t2);
    tasks.push_back(serv);
    tasks.push_back(t2);
    kernels[0]->addTask(*serv, "");

    EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
    k->addForcedDispatch(tasks[0], cpus_big[0], 18, 999);

    SIMUL.initSingleRun();

    SIMUL.run_to(3); // t=2: executing to releasing
    cout << "time = " << SIMUL.getTime() << endl;
    REQUIRE (k->getUtilization_active(dynamic_cast<CPU_BL*>(cpus_little.at(0))) == 0.0);
    REQUIRE (k->getUtilization_active(dynamic_cast<CPU_BL*>(cpus_big.at(0))) == 0.2);
    k->print();
    REQUIRE (k->getIslandUtilization(1.0, cpus_big[0]->getIslandType(), NULL) == 0.2);
    REQUIRE (k->getUtilization(cpus_big.at(0), 1.0) == 0.2); // float repr. precision

    SIMUL.run_to(5); // t=4: releasing to idle
    cout << "time = " << SIMUL.getTime() << endl;
    for (CPU_BL* c : cpus_big) {
        if (c == cpus_big.at(0)) continue;
        REQUIRE (k->getUtilization_active(dynamic_cast<CPU_BL*>(c)) == 0.0);
        REQUIRE (k->getUtilization(c, 1.0) == 0.0);
    }

    SIMUL.run_to(20);
    cout << "time = " << SIMUL.getTime() << endl;

    SIMUL.endSingleRun();
    for (int j = 0; j < tasks.size(); j++)
        delete tasks[j];
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}

TEST_CASE("Experiment 18") {
    /**
        The objective here is to find the time at which the task in the server
        goes from releasing to idle (time_t). Also, I want to see how
        cores utilization is computed (=> leave 2 bigs and 1 little enabled).

        Given that time, 3 experiments are performed. In the first one, another
        task comes before the server task's over. In the second one, it comes before time_t.
        In the third one, it comes afterwards.
        You should observe that, in the first case, the core utilization
        considers server task; in the second one, the releasing task; in the other,
        the idle task is not considered.

        In all cases, the server task remains in the core queue until time_t. Later, it
        goes back to the general scheduler.

        Server and tasks run on the same core, a big max freq for convenience.
      */
    init_sequence = 18;
    cout << "Begin of experiment " << init_sequence << endl;
    Requisite req(false, false, true);
    if (!checkRequisites( req ))  return;
    performedTests[init_sequence] = req;

    EnergyMRTKernel *kern;
    init_suite(&kern);
    REQUIRE(kern != NULL);
    vector<RTKernel*> kernels = { kern };
    vector<AbsRTTask*> tasks;
    vector<CPU_BL *> cpus_little = kern->getIslandLittle()->getProcessors();
    vector<CPU_BL *> cpus_big = kern->getIslandBig()->getProcessors();

    cpus_little[0]->toggleDisabled();
    cpus_little[1]->toggleDisabled();
    cpus_little[2]->toggleDisabled();
    cpus_little[3]->toggleDisabled();
    cpus_big[2]->toggleDisabled();
    cpus_big[3]->toggleDisabled();

    cpus_big[0]->setOPP(18);
    cpus_big[1]->setOPP(18);

    vector<int> activ = { 6, 8, 12 };

    // Already-there tasks
    PeriodicTask *t0_little = new PeriodicTask(20, 20 , 0, "Task1_little0"); 
    t0_little->insertCode("fixed(8,bzip2);");
    t0_little->setAbort(false);
    tasks.push_back(t0_little);
    kernels[0]->addTask(*t0_little, "");

    NonPeriodicTask *t0_big0 = new NonPeriodicTask(10, 10, 0, "Task2_Big0"); 
    t0_big0->insertCode("fixed(5,bzip2);");
    t0_big0->setAbort(false);
    tasks.push_back(t0_big0);
    kernels[0]->addTask(*t0_big0, "");

    PeriodicTask *t0_big1 = new PeriodicTask(30, 30 , 0, "Task3_Big1"); 
    t0_big1->insertCode("fixed(15,bzip2);");
    t0_big1->setAbort(false);
    tasks.push_back(t0_big1);
    kernels[0]->addTask(*t0_big1, "");

    PeriodicTask *t1_big1 = new PeriodicTask(31, 31 , 0, "TaskReady_Big1"); 
    t1_big1->insertCode("fixed(1,bzip2);");
    t1_big1->setAbort(false);
    tasks.push_back(t1_big1);
    kernels[0]->addTask(*t1_big1, "");
    


    // CBS server tasks
    NonPeriodicTask *tos = new NonPeriodicTask(10, 10 , 0, "TaskOnServer"); 
    tos->insertCode("fixed(2,bzip2);"); // => its releasing_idle will be at t=4
    tos->setAbort(false);

    NonPeriodicTask *tos2 = new NonPeriodicTask(10, 10 , 0, "AfterTaskOnServer"); 
    tos2->insertCode("fixed(2,bzip2);");
    tos2->setAbort(false);

    CBServerCallingEMRTKernel *serv = new CBServerCallingEMRTKernel(2, 10, 10, "hard",  "server1", "FIFOSched");
    serv->addTask(*tos);
    serv->addTask(*tos2);
    tasks.push_back(serv);
    kernels[0]->addTask(*serv, "");



    // Tasks coming freely: the dynamic situations
    PeriodicTask *t5 = new PeriodicTask(30, 30 , 0, "TaskDuring"); 
    t5->insertCode("fixed(2,bzip2);");
    t5->setAbort(false);
    tasks.push_back(t5);
    kernels[0]->addTask(*t5, "");

    PeriodicTask *t3 = new PeriodicTask(30, 30 , 0, "TaskBefore"); 
    t3->insertCode("fixed(1,bzip2);");
    t3->setAbort(false);
    tasks.push_back(t3);
    kernels[0]->addTask(*t3, "");

    PeriodicTask *t4 = new PeriodicTask(30, 30 , 0, "TaskAfter"); 
    t4->insertCode("fixed(1,bzip2);");
    t4->setAbort(false);
    tasks.push_back(t4);
    kernels[0]->addTask(*t4, "");
    


    EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
    k->addForcedDispatch(tasks[0], cpus_little[0], 12, 1); // note: normally it wouldn't fit this way
    k->addForcedDispatch(tasks[1], cpus_big[0], 18, 1);
    k->addForcedDispatch(tasks[2], cpus_big[1], 18, 1);
    k->addForcedDispatch(t1_big1,  cpus_big[1], 18, 1);  // it should preempt the executing task on big 0
    // server's free to go wherever.

    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "Running simulation!" << endl;
    SIMUL.initSingleRun();

    t5->activate(Tick(activ[0]));
    t3->activate(Tick(activ[1]));
    t4->activate(Tick(activ[2]));
    tos2->activate(Tick(11));

    double init_util = 0.0;

    // Note: island utilization is checked at t-1 because at time t the new task is already dispatched

    SIMUL.run_to(1); // init setup
    REQUIRE (k->getProcessorRunning(t0_little) == cpus_little[0]);
    REQUIRE (k->getProcessorRunning(t0_big0) == cpus_big[0]);
    REQUIRE (k->getProcessorReady(serv) == cpus_big[0]);
    REQUIRE (k->getProcessorRunning(t0_big1) == cpus_big[1]);
    REQUIRE (k->getProcessorReady(t1_big1) == cpus_big[1]);

    SIMUL.run_to(5); // t0_big0 ends and tos (serv) starts
    REQUIRE (k->getProcessorRunning(t0_little) == cpus_little[0]);
    REQUIRE (k->getProcessorRunning(serv) == cpus_big[0]);
    REQUIRE (k->getProcessorRunning(t0_big1) == cpus_big[1]);
    REQUIRE (k->getProcessorReady(t1_big1) == cpus_big[1]);
    REQUIRE (serv->getTasks().size() == 1);
    REQUIRE (serv->getTasks().at(0) == tos);
    cout << " wcet " << tos->getWCET(1.0) << endl;
    REQUIRE ((tos->getWCET(1.0) + double(SIMUL.getTime())) == 7.0);
    REQUIRE (k->getCBServer_CEMRTK_Utilization(serv, init_util, 1.0)); // is 0.2 server util or core active util? exp. server util
    REQUIRE (k->getIslandUtilization(1.0, IslandType::BIG, NULL) == 0.2 + 10.0 / 30.0 + 1.0 / 31.0);
    
    SIMUL.run_to(6); // taskDuring comes and goes ready on big0. Island util considers server util and tasks on big1
    REQUIRE (k->getProcessorRunning(serv) == cpus_big[0]);
    REQUIRE (k->getProcessorReady(t5) == cpus_big[0]);

    SIMUL.run_to(7); // tos ends, yields, goes ready and taskDuring starts
    REQUIRE (serv->isEmpty());
    REQUIRE (k->getProcessorReady(serv) == cpus_big[0]);
    REQUIRE (k->getProcessorRunning(t5) == cpus_big[0]);
    REQUIRE ((t5->getWCET(1.0) + double(SIMUL.getTime())) == 9.0);
    REQUIRE (k->getUtilization_active(dynamic_cast<CPU_BL*>(cpus_big[0])) == 0.2);
    REQUIRE (k->getCBServer_CEMRTK_Utilization(serv, init_util, 1.0)); // is 0.2 server util or core active util? exp. core active util
    
    SIMUL.run_to(8); // taskBefore (the server DL) comes and goes ready on big0. Island util considers u_active of big0 and tasks on big1
    REQUIRE (k->getProcessorRunning(t5) == cpus_big[0]);
    REQUIRE (k->getProcessorReady(serv) == cpus_big[0]);
    REQUIRE (k->getProcessorReady(t3) == cpus_big[0]);

    SIMUL.run_to(9); // taskDuring ends, serv yields and is ready, taskBefore starts
    REQUIRE (serv->isEmpty());
    REQUIRE (k->getProcessorReady(serv) == cpus_big[0]);
    REQUIRE (k->getProcessorRunning(t3) == cpus_big[0]);
    REQUIRE ((t3->getWCET(1.0) + double(SIMUL.getTime())) == 10.0);

    SIMUL.run_to(10); // server deadline, it recharges itself
    REQUIRE (serv->isEmpty());
    for (CPU* c : cpus_big)
        REQUIRE (k->getUtilization_active(dynamic_cast<CPU_BL*>(c)) == 0.0);

    SIMUL.run_to(11); // tos2 comes, server gets running
    REQUIRE (serv->getTasks().size() == 1);
    REQUIRE (serv->getTasks().at(0) == tos2);
    REQUIRE (serv->getStatus() == ServerStatus::EXECUTING);
    REQUIRE (k->getIslandUtilization(1.0, IslandType::BIG, NULL) == 2.0 / 21.0 + 4.0/30.0 + 1.0 / 31.0);
    REQUIRE (k->getCBServer_CEMRTK_Utilization(serv, init_util, 1.0)); // is 0.2 server util or core active util? exp. server util
    REQUIRE ((tos2->getWCET(1.0) + double(SIMUL.getTime())) == 13.0);

    SIMUL.run_to(12); // taskAfter comes. Island utilization considers server and tasks on big1
    REQUIRE (k->getProcessorRunning(serv) == cpus_big[0]);
    REQUIRE (k->getProcessorReady(t4) == cpus_big[0]);

    SIMUL.run_to(13); // tos2 ends, task after starts
    REQUIRE (k->getProcessorRunning(t4) == cpus_big[0]);
    REQUIRE (k->getProcessorReady(serv) == cpus_big[0]);
    REQUIRE (k->getUtilization_active(dynamic_cast<CPU_BL*>(cpus_big[0])) == 0.2);
    REQUIRE (k->getProcessorRunning(t0_little) == cpus_little[0]);
    REQUIRE (k->getProcessorRunning(t0_big1) == cpus_big[1]);
    REQUIRE (k->getProcessorReady(t1_big1) == cpus_big[1]);
    REQUIRE ((t4->getWCET(1.0) + double(SIMUL.getTime())) == 14.0);

// todo put back    SIMUL.run_to(16);

    SIMUL.endSingleRun();
    for (int j = 0; j < tasks.size(); j++)
        delete tasks[j];
    delete tos; delete tos2;
    delete kern;
    cout << "End of Experiment #" << init_sequence << endl << endl;
    performedTests[init_sequence] = req;
}


TEST_CASE("None") {
    for (const auto& elem : performedTests) {
        cout << "Performed experiment #" << elem.first << " with " << elem.second.toString() << endl;
    }
}

void getCores(vector<CPU_BL*> &cpus_little, vector<CPU_BL*> &cpus_big, Island_BL **island_bl_little, Island_BL **island_bl_big) {
    unsigned int OPP_little = 0; // Index of OPP in LITTLE cores
    unsigned int OPP_big = 0;    // Index of OPP in big cores

    vector<double> V_little = {
            0.92, 0.919643, 0.919357, 0.918924, 0.95625, 0.9925, 1.02993, 1.0475, 1.08445, 1.12125, 1.15779, 1.2075,
            1.25625
    };
    vector<unsigned int> F_little = {
            200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400
    };

    vector<double> V_big = {
            0.916319, 0.915475, 0.915102, 0.91498, 0.91502, 0.90375, 0.916562, 0.942543, 0.96877, 0.994941, 1.02094,
            1.04648, 1.05995, 1.08583, 1.12384, 1.16325, 1.20235, 1.2538, 1.33287
    };
    vector<unsigned int> F_big = {
            200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000
    };

    if (OPP_little >= V_little.size() || OPP_big >= V_big.size())
        exit(-1);

    unsigned long int max_frequency = max(F_little[F_little.size() - 1], F_big[F_big.size() - 1]);

    /* ------------------------- Creating CPU_BLs -------------------------*/
    for (unsigned int i = 0; i < 4; ++i) {
        /* Create 4 LITTLE CPU_BLs */
        string cpu_name = "LITTLE_" + to_string(i);

        cout << "Creating CPU_BL: " << cpu_name << endl;

        cout << "f is " << F_little[F_little.size() - 1] << " max_freq " << max_frequency << endl;

        CPUModelBP *pm = new CPUModelBP(V_little[V_little.size() - 1], F_little[F_little.size() - 1], max_frequency);
        {
            CPUModelBP::PowerModelBPParams idle_pp = {0.00134845, 1.76307e-5, 124.535, 1.00399e-10};
            CPUModelBP::ComputationalModelBPParams idle_cp = {1, 0, 0, 0};
            dynamic_cast<CPUModelBP *>(pm)->setWorkloadParams("idle", idle_pp, idle_cp);

            CPUModelBP::PowerModelBPParams bzip2_pp = {0.00775587, 33.376, 1.54585, 9.53439e-10};
            CPUModelBP::ComputationalModelBPParams bzip2_cp = {0.0256054, 2.9809e+6, 0.602631, 8.13712e+9};
            dynamic_cast<CPUModelBP *>(pm)->setWorkloadParams("bzip2", bzip2_pp, bzip2_cp);

            CPUModelBP::PowerModelBPParams hash_pp = {0.00624673, 176.315, 1.72836, 1.77362e-10};
            CPUModelBP::ComputationalModelBPParams hash_cp = {0.00645628, 3.37134e+6, 7.83177, 93459};
            dynamic_cast<CPUModelBP *>(pm)->setWorkloadParams("hash", hash_pp, hash_cp);

            CPUModelBP::PowerModelBPParams encrypt_pp = {0.00676544, 26.2243, 5.6071, 5.34216e-10};
            CPUModelBP::ComputationalModelBPParams encrypt_cp = {6.11496e-78, 3.32246e+6, 6.5652, 115759};
            dynamic_cast<CPUModelBP *>(pm)->setWorkloadParams("encrypt", encrypt_pp, encrypt_cp);

            CPUModelBP::PowerModelBPParams decrypt_pp = {0.00629664, 87.1519, 2.93286, 2.80871e-10};
            CPUModelBP::ComputationalModelBPParams decrypt_cp = {5.0154e-68, 3.31791e+6, 7.154, 112163};
            dynamic_cast<CPUModelBP *>(pm)->setWorkloadParams("decrypt", decrypt_pp, decrypt_cp);

            CPUModelBP::PowerModelBPParams cachekiller_pp = {0.0126737, 67.9915, 1.63949, 3.66185e-10};
            CPUModelBP::ComputationalModelBPParams cachekiller_cp = {1.20262, 352597, 2.03511, 169523};
            dynamic_cast<CPUModelBP *>(pm)->setWorkloadParams("cachekiller", cachekiller_pp, cachekiller_cp);
        }

        cout << "creating cpu" << endl;
        CPU_BL *c = new CPU_BL(cpu_name, "idle", pm);
        c->setIndex(i);
        pm->setCPU(c);
        pm->setFrequencyMax(max_frequency);
        //TracePowerConsumption *power_trace = new TracePowerConsumption(c, 1, "power_" + cpu_name + ".txt");
        //ptrace.push_back(power_trace);

        cpus_little.push_back(c);
    }

    for (unsigned int i = 0; i < 4; ++i) {
        /* Create 4 big CPU_BLs */

        string cpu_name = "BIG_" + to_string(i);

        cout << "Creating CPU_BL: " << cpu_name << endl;

        CPUModelBP *pm = new CPUModelBP(V_big[V_big.size() - 1], F_big[F_big.size() - 1], max_frequency);
        {
            CPUModelBP::PowerModelBPParams idle_pp = {0.0162881, 0.00100737, 55.8491, 1.00494e-9};
            CPUModelBP::ComputationalModelBPParams idle_cp = {1, 0, 0, 0};
            dynamic_cast<CPUModelBP *>(pm)->setWorkloadParams("idle", idle_pp, idle_cp);

            CPUModelBP::PowerModelBPParams bzip2_pp = {0.0407739, 12.022, 3.33367, 7.4577e-9};
            CPUModelBP::ComputationalModelBPParams bzip2_cp = {0.17833, 1.63265e+6, 1.62033, 118803};
            dynamic_cast<CPUModelBP *>(pm)->setWorkloadParams("bzip2", bzip2_pp, bzip2_cp);

            CPUModelBP::PowerModelBPParams hash_pp = {0.0388215, 16.3205, 4.3418, 5.07039e-9};
            CPUModelBP::ComputationalModelBPParams hash_cp = {0.017478, 1.93925e+6, 4.22469, 83048.3};
            dynamic_cast<CPUModelBP *>(pm)->setWorkloadParams("hash", hash_pp, hash_cp);

            CPUModelBP::PowerModelBPParams encrypt_pp = {0.0348728, 8.14399, 5.64344, 7.69915e-9};
            CPUModelBP::ComputationalModelBPParams encrypt_cp = {8.39417e-34, 1.99222e+6, 3.33002, 96949.4};
            dynamic_cast<CPUModelBP *>(pm)->setWorkloadParams("encrypt", encrypt_pp, encrypt_cp);

            CPUModelBP::PowerModelBPParams decrypt_pp = {0.0320508, 25.8727, 3.27135, 4.11773e-9};
            CPUModelBP::ComputationalModelBPParams decrypt_cp = {9.49471e-35, 1.98761e+6, 2.65652, 109497};
            dynamic_cast<CPUModelBP *>(pm)->setWorkloadParams("decrypt", decrypt_pp, decrypt_cp);

            CPUModelBP::PowerModelBPParams cachekiller_pp = {0.086908, 9.17989, 2.5828, 7.64943e-9};
            CPUModelBP::ComputationalModelBPParams cachekiller_cp = {0.825212, 235044, 786.368, 25622.1};
            dynamic_cast<CPUModelBP *>(pm)->setWorkloadParams("cachekiller", cachekiller_pp, cachekiller_cp);
        }

        CPU_BL *c = new CPU_BL(cpu_name, "idle", pm);
        c->setIndex(i);
        pm->setCPU(c);
        pm->setFrequencyMax(max_frequency);
        //TracePowerConsumption *power_trace = new TracePowerConsumption(c, 1, "power_" + cpu_name + ".txt");
        //ptrace.push_back(power_trace);

        cpus_big.push_back(c);
    }


    vector<struct OPP> opps_little = Island_BL::buildOPPs(V_little, F_little);
    vector<struct OPP> opps_big = Island_BL::buildOPPs(V_big, F_big);
    *island_bl_little = new Island_BL("little island", IslandType::LITTLE, cpus_little, opps_little);
    *island_bl_big = new Island_BL("big island", IslandType::BIG, cpus_big, opps_big);
}

int init_suite(EnergyMRTKernel** kern) {
    cout << "init_suite" << endl;

    #if LEAVE_LITTLE3_ENABLED
        cout << "Error: tests thought for LEAVE_LITTLE3_ENABLED disabled" << endl;
        abort();
    #endif

    Island_BL *island_bl_big = NULL, *island_bl_little = NULL;
    vector<CPU_BL *> cpus_little, cpus_big;
    vector<Scheduler*> schedulers;
    vector<RTKernel*> kernels;

    getCores(cpus_little, cpus_big, &island_bl_little, &island_bl_big);
    REQUIRE(island_bl_big != NULL); REQUIRE(island_bl_little != NULL);
    REQUIRE(cpus_big.size() == 4); REQUIRE(cpus_little.size() == 4);

    EDFScheduler *edfsched = new EDFScheduler;
    for (int i = 0; i < 8; i++)
      schedulers.push_back(new EDFScheduler());

    *kern = new EnergyMRTKernel(schedulers, edfsched, island_bl_big, island_bl_little, "The sole kernel");
    kernels.push_back(*kern);

    CPU_BL::referenceFrequency = 2000; // BIG_3 frequency

    cout << "end init_suite of Experiment #" << init_suite << endl;
    return 0;
}

/// Returns true if the value 'eval' and 'expected' are distant 'error'%
bool inRange(int eval, int expected) {
    const unsigned int error = 5;

    int min = int(eval - eval * error/100);
    int max = int(eval + eval * error/100);

    return expected >= min && expected <= max; 
}

/// True if min <= eval <= max
bool inRangeMinMax(double eval, const double min, const double max) {
    return min <= eval <= max;
}

bool checkRequisites(Requisite reqs) {
    cout << "Experiments with " << reqs.toString() << endl; 
    cout << "Current settings: EMRTK_LEAVE_LITTLE3_ENABLED: " << EnergyMRTKernel::EMRTK_LEAVE_LITTLE3_ENABLED << ", EMRTK_MIGRATE_ENABLED: " << EnergyMRTKernel::EMRTK_MIGRATE_ENABLED << ", EMRTK_CBS_YIELD_ENABLED: " << EnergyMRTKernel::EMRTK_CBS_YIELD_ENABLED << endl; 
    reqs.satisfied = false;

    if (FORCE_REQUISITES) {
        cout << "Changing EMRTK policies settings as needed" << endl;

        cout << "\tSetting Leave Little 3 free policy to " << reqs.EMRTK_leave_little3 << endl;
        EnergyMRTKernel::EMRTK_LEAVE_LITTLE3_ENABLED = reqs.EMRTK_leave_little3;

        cout << "\tSetting migration policy to " << reqs.EMRTK_migrate << endl;
        EnergyMRTKernel::EMRTK_MIGRATE_ENABLED = reqs.EMRTK_migrate;

        cout << "\tSetting CBS Server yielding policy to " << reqs.EMRTK_cbs_yield << endl;
        EnergyMRTKernel::EMRTK_CBS_YIELD_ENABLED = reqs.EMRTK_cbs_yield;

        reqs.satisfied = true;
        return true;
    }

    if ( reqs.EMRTK_leave_little3 != EnergyMRTKernel::EMRTK_LEAVE_LITTLE3_ENABLED ) {
        cout << "Test requires EMRTK_LEAVE_LITTLE3_ENABLED = 1, but it's disabled: " << EnergyMRTKernel::EMRTK_LEAVE_LITTLE3_ENABLED << ". Skip" << endl;
        return false;
    }
    if ( reqs.EMRTK_migrate != EnergyMRTKernel::EMRTK_MIGRATE_ENABLED ) {
        cout << "Test requires EMRTK_MIGRATE_ENABLED = 1, but it's disabled: " << EnergyMRTKernel::EMRTK_MIGRATE_ENABLED << ". Skip" << endl;
        return false;
    }
    if (reqs.EMRTK_cbs_yield != EnergyMRTKernel::EMRTK_CBS_YIELD_ENABLED) {
        cout << "Test requires EMRTK_CBS_YIELD_ENABLED = 1, but it's disabled: " << EnergyMRTKernel::EMRTK_CBS_YIELD_ENABLED << ". Skip" << endl;
        return false;
    }
    reqs.satisfied = true;
    return true;
}
