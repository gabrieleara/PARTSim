#include <metasim.hpp>
#include <rttask.hpp>
#include <mrtkernel.hpp>
#include <energyMRTKernel.hpp>
#include <edfsched.hpp>
#include <cbserver.hpp>

#include "CUnit/Automated.h"
#include "CUnit/CUnit.h"

#define SUITES_NO 8

using namespace MetaSim;
using namespace RTSim;
using namespace std;

unsigned int OPP_little = 0; // Index of OPP in LITTLE cores
unsigned int OPP_big = 0;    // Index of OPP in big cores
string workload = "bzip2";
string task_name = "";
int init_sequence = 0;
vector<CPU*> cpus;
vector<CPU*> cpu_task; // to be cleared after each test
vector<PeriodicTask*> task; // to be cleared after each test
vector<RTKernel *> kernels;

int cleanup_suite();
int init_suite();

void energyTest0() {
    task_name = "T0_task1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
    kernels[0]->addTask(*t0, "");
    //ttrace.attachToTask(*t0);

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    CPU *c0 = kernels[0]->getProcessor(t0);

    CU_ASSERT(t0->getName() == "T0_task1");
    CU_ASSERT(c0->getFrequency() == 2000);
    CU_ASSERT(c0->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t0->getWCET(c0->getSpeed())))  == 497);

    SIMUL.endSingleRun();
}

void energyTest1() {
    task_name = "T1_task1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
    kernels[0]->addTask(*t0, "");

    task_name = "T1_task2";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t1 = new PeriodicTask(500, 500, 0, task_name);
    t1->insertCode("fixed(500," + workload + ");");
    kernels[0]->addTask(*t1, "");

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    CPU *c0 = kernels[0]->getProcessor(t0);
    CPU *c1 = kernels[0]->getProcessor(t1);

    CU_ASSERT(t0->getName() == "T1_task1");
    CU_ASSERT(c0->getFrequency() == 2000);
    CU_ASSERT(c0->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t0->getWCET(c0->getSpeed())))  == 497);

    CU_ASSERT(t1->getName() == "T1_task2");
    CU_ASSERT(c1->getFrequency() == 2000);
    CU_ASSERT(c1->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t1->getWCET(c1->getSpeed()))) == 497);

    SIMUL.endSingleRun();
}

void energyTest2() {
    task_name = "T2_task1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
    kernels[0]->addTask(*t0, "");

    task_name = "T2_task2";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t1 = new PeriodicTask(500, 500, 0, task_name);
    t1->insertCode("fixed(250," + workload + ");");
    kernels[0]->addTask(*t1, "");

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    CPU *c0 = kernels[0]->getProcessor(t0);
    CPU *c1 = kernels[0]->getProcessor(t1);

    for(string s : kernels[0]->getRunningTasks())
      cout << "running :" << s<<endl;

    CU_ASSERT(t0->getName() == "T2_task1");
    CU_ASSERT(c0->getFrequency() == 2000);
    CU_ASSERT(c0->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t0->getWCET(c0->getSpeed())))  == 497);

    CU_ASSERT(t1->getName() == "T2_task2");
    CU_ASSERT(c1->getFrequency() == 2000);
    CU_ASSERT(c1->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t1->getWCET(c1->getSpeed()))) == 248);

    SIMUL.endSingleRun();
}

void energyTest3() {
    task_name = "T3_task1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(10," + workload + ");"); // WCET 10 at max frequency on big cores
    kernels[0]->addTask(*t0, "");

    SIMUL.initSingleRun();
    SIMUL.run_to(10);

    CPU *c0 = kernels[0]->getProcessor(t0);

    CU_ASSERT(t0->getName() == "T3_task1");
    CU_ASSERT(c0->getFrequency() == 500);
    CU_ASSERT(c0->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t0->getWCET(c0->getSpeed())))  == 65);

    SIMUL.endSingleRun();
}

void energyTest4() {
    vector<CPU*> cpus;
    for (int j = 0; j < 4; j++) {
        task_name = "T4_task_LITTLE_" + std::to_string(j);
        cout << "Creating task: " << task_name;
        PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
        char instr[60] = "";
        sprintf(instr, "fixed(100, %s);", workload.c_str());
        t->insertCode(instr);
        kernels[0]->addTask(*t, "");

        task.push_back(t);
    }

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    CPU *c0 = kernels[0]->getProcessor(task[0]);
    CPU *c1 = kernels[0]->getProcessor(task[1]);
    CPU *c2 = kernels[0]->getProcessor(task[2]);
    CPU *c3 = kernels[0]->getProcessor(task[3]);
    CPU *c4 = kernels[0]->getProcessor(task[4]);

    CU_ASSERT(task[0]->getName() == "T4_Task_LITTLE_0");
    CU_ASSERT(c0->getFrequency() == 700);
    CU_ASSERT(c0->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(task[0]->getWCET(c0->getSpeed())))  == 488);

    CU_ASSERT(task[1]->getName() == "T4_Task_LITTLE_1");
    CU_ASSERT(c1->getFrequency() == 700);
    CU_ASSERT(c1->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(task[1]->getWCET(c1->getSpeed()))) == 488);

    CU_ASSERT(task[2]->getName() == "T4_Task_LITTLE_2");
    CU_ASSERT(c2->getFrequency() == 700);
    CU_ASSERT(c2->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(task[2]->getWCET(c2->getSpeed()))) == 488);

    CU_ASSERT(task[3]->getName() == "T4_Task_LITTLE_3");
    CU_ASSERT(c3->getFrequency() == 700);
    CU_ASSERT(c3->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(task[3]->getWCET(c3->getSpeed()))) == 488);

    CU_ASSERT(task[4]->getName() == "T4_Task_LITTLE_4");
    CU_ASSERT(c4->getFrequency() == 700);
    CU_ASSERT(c4->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(task[4]->getWCET(c4->getSpeed()))) == 251);

    SIMUL.endSingleRun();
}

void energyTest5() {
    for (int j = 0; j < 4; j++) {
        int wcet = 5; //* (j+1);
        task_name = "T5_task" + std::to_string(j);
        cout << "Creating task: " << task_name;
        PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
        char instr[60] = "";
        sprintf(instr, "fixed(%d, %s);", wcet, workload.c_str());
        cout << " with abs. WCET " << wcet << endl;
        t->insertCode(instr);
        kernels[0]->addTask(*t, "");

        task.push_back(t);
        // LITTLE_0, _1, _2, _3 freq 1400.
    }

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    CPU *c0 = kernels[0]->getProcessor(task[0]);
    CPU *c1 = kernels[0]->getProcessor(task[1]);
    CPU *c2 = kernels[0]->getProcessor(task[2]);
    CPU *c3 = kernels[0]->getProcessor(task[3]);

    PeriodicTask* t = task[0];
    CU_ASSERT(t->getName() == "T5_task0");
    CU_ASSERT(c0->getFrequency() == 500);
    CU_ASSERT(c0->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c0->getSpeed())))  == 32);

    t = task[1];
    CU_ASSERT(t->getName() == "T5_task1");
    CU_ASSERT(c1->getFrequency() == 500);
    CU_ASSERT(c1->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c1->getSpeed()))) == 32);

    t = task[2];
    CU_ASSERT(t->getName() == "T5_task2");
    CU_ASSERT(c2->getFrequency() == 500);
    CU_ASSERT(c2->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c2->getSpeed()))) == 32);

    t = task[3];
    CU_ASSERT(t->getName() == "T5_task3");
    CU_ASSERT(c3->getFrequency() == 500);
    CU_ASSERT(c3->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c3->getSpeed()))) == 32);
    SIMUL.endSingleRun();
}

// test showing that frequency of little/big island may be raised
void energyTest6() {
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
        kernels[0]->addTask(*t, "");

        task.push_back(t);
    }

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    for (int j = 0; j < 5; j++) {
        cpu_task.push_back(kernels[0]->getProcessor(task[j]));
    }

    i = 0;
    PeriodicTask* t = task[i];
    CPU* c = cpu_task[i];
    CU_ASSERT(t->getName() == "T6_task0");
    CU_ASSERT(c->getFrequency() == 2000);
    CU_ASSERT(c->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed())))  == 298);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 1;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T6_task1");
    CU_ASSERT(c->getFrequency() == 2000);
    CU_ASSERT(c->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 298);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 2;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T6_task2");
    CU_ASSERT(c->getFrequency() == 2000);
    CU_ASSERT(c->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 298);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 3;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T6_task3");
    CU_ASSERT(c->getFrequency() == 2000);
    CU_ASSERT(c->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 298);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 4;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T6_task4");
    CU_ASSERT(c->getFrequency() == 2000);
    CU_ASSERT(c->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 198);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    SIMUL.endSingleRun();
}

void energyTest7() {
    int wcets[] = { 63, 63, 63, 63, 30 };
    vector<PeriodicTask*> task;
    int i;
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
        task_name = "T7_task" + std::to_string(j);
        cout << "Creating task: " << task_name;
        PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
        char instr[60] = "";
        sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
        t->insertCode(instr);
        kernels[0]->addTask(*t, "");
        task.push_back(t);
    }

    /* Towards random workloads, but this time alg. first decides to
       schedule all tasks on littles, and then, instead of schedule the
       next one in bigs, it shall increase littles frequency so to make
       space to it too and save energy */

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
        cpu_task.push_back(kernels[0]->getProcessor(task[j]));
    }

    i = 0;
    PeriodicTask* t = task[i];
    CPU* c = cpu_task[i];
    CU_ASSERT(t->getName() == "T7_task0");
    CU_ASSERT(c->getFrequency() == 500);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed())))  == 415);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 1;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T7_task1");
    CU_ASSERT(c->getFrequency() == 500);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 415);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 2;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T7_task2");
    CU_ASSERT(c->getFrequency() == 500);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 415);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 3;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T7_task3");
    CU_ASSERT(c->getFrequency() == 500);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 415);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 4;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T7_task4");
    CU_ASSERT(c->getFrequency() == 500);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 28);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    SIMUL.endSingleRun();
}

void energyTest8() {
    int wcets[] = { 181, 419, 261, 163, 65, 8, 61, 170, 273 };
    vector<PeriodicTask*> task;
    int i;
    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
        task_name = "T8_task" + std::to_string(j);
        cout << "Creating task: " << task_name;
        PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
        char instr[60] = "";
        sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
        t->insertCode(instr);
        kernels[0]->addTask(*t, "");
        task.push_back(t);
    }
    // towards random workloads...

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
        cpu_task.push_back(kernels[0]->getProcessor(task[j]));
    }

    i = 0;
    PeriodicTask* t = task[i];
    CPU* c = cpu_task[i];
    CU_ASSERT(t->getName() == "T8_task" + i);
    CU_ASSERT(c->getFrequency() == 1400);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed())))  == 181);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 1;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T8_task" + i);
    CU_ASSERT(c->getFrequency() == 1700);
    CU_ASSERT(c->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 419);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 2;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T8_task" + i);
    CU_ASSERT(c->getFrequency() == 1700);
    CU_ASSERT(c->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 261);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 3;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T8_task"+i);
    CU_ASSERT(c->getFrequency() == 1400);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 163);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 4;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T8_task"+i);
    CU_ASSERT(c->getFrequency() == 1400);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 65);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 5;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T8_task"+i);
    CU_ASSERT(c->getFrequency() == 1400);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 8);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 6;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T8_task"+i);
    CU_ASSERT(c->getFrequency() == 1400);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 61);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 7;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T8_task"+i);
    CU_ASSERT(c->getFrequency() == 1400);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 170);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 8;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T8_task"+i);
    CU_ASSERT(c->getFrequency() == 1700);
    CU_ASSERT(c->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 273);
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    SIMUL.endSingleRun();
}

void createSuites(CU_pSuite* pSuites) {
    char str_name[20];
    for (int i = 0; i < SUITES_NO; i++) {
        sprintf(str_name, "Suite_%d", i);
        pSuites[i] = CU_add_suite(str_name, init_suite, cleanup_suite);
        if (NULL == pSuites[i]) {
            CU_cleanup_registry();
            perror("Suite creation");
            exit(CU_get_error());
        }
    }
}

void addTest(int exp_no,  CU_pSuite *pSuites, void (*f)() ) {
    char exp_name[40] = "";
    sprintf(exp_name, "test of test_energy%d()", exp_no);
    if ( (NULL == CU_add_test(pSuites[exp_no], exp_name, f)) )
    {
        CU_cleanup_registry();
        perror("Suite creation");
        exit(CU_get_error());
    }
}

int main()
{
    // create a suite
    CU_pSuite pSuites[SUITES_NO] = { NULL };
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry. A suite per test */
    createSuites(pSuites);

    /* add the tests to the suite */
    int exp_no = 0;
    addTest(exp_no++, pSuites, energyTest0);
    addTest(exp_no++, pSuites, energyTest1);
    addTest(exp_no++, pSuites, energyTest2);
    addTest(exp_no++, pSuites, energyTest3);
    addTest(exp_no++, pSuites, energyTest4);
    addTest(exp_no++, pSuites, energyTest5);
    addTest(exp_no++, pSuites, energyTest6);

    CU_list_tests_to_file();
    CU_set_output_filename("results");
    CU_set_error_action(CUEA_FAIL);

    /* Run all tests using the CUnit Basic interface */
    CU_automated_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}

int cleanup_suite() {
    cout << "cleanup_suite" << endl;
    cpus.clear();
    kernels.clear();
    cpu_task.clear();

    cout << "end cleanup_suite" << endl;
    return 0;
}

int init_suite() {
    cout << "init_suite" << endl;
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

    /* ------------------------- Creating CPUs -------------------------*/
    for (unsigned int i = 0; i < 4; ++i) {
        /* Create 4 LITTLE CPUs */
        string cpu_name = to_string(init_sequence) + "LITTLE_" + to_string(i);

        cout << "Creating CPU: " << cpu_name << endl;

        cout << "f is " << F_little[F_little.size() - 1] << " max_freq " << max_frequency << endl;

        CPUModel *pm = new CPUModelBP(V_little[V_little.size() - 1], F_little[F_little.size() - 1], max_frequency);
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
        CPU *c = new CPU(cpu_name, V_little, F_little, pm);
        c->setOPP(OPP_little);
        c->setWorkload("idle");
        c->setIsland(CPU::Island::LITTLE);
        pm->setFrequencyMax(max_frequency);
        //TracePowerConsumption *power_trace = new TracePowerConsumption(c, 1, "power_" + cpu_name + ".txt");
        //ptrace.push_back(power_trace);

        cpus.push_back(c);
    }

    for (unsigned int i = 0; i < 4; ++i) {
        /* Create 4 big CPUs */

        string cpu_name = to_string(init_sequence) + "BIG_" + to_string(i);

        cout << "Creating CPU: " << cpu_name << endl;

        CPUModel *pm = new CPUModelBP(V_big[V_big.size() - 1], F_big[F_big.size() - 1], max_frequency);
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

        CPU *c = new CPU(cpu_name, V_big, F_big, pm);
        c->setOPP(OPP_big);
        c->setWorkload("idle");
        c->setIsland(CPU::Island::BIG);
        pm->setFrequencyMax(max_frequency);
        //TracePowerConsumption *power_trace = new TracePowerConsumption(c, 1, "power_" + cpu_name + ".txt");
        //ptrace.push_back(power_trace);

        cpus.push_back(c);
    }

    EDFScheduler *edfsched = new EDFScheduler;

    EnergyMRTKernel *kern = new EnergyMRTKernel(edfsched, cpus, "The sole kernel" + to_string(init_sequence));
    kernels.push_back(kern);

    CPU::referenceFrequency = 2000; // BIG_3 frequency

    init_sequence++;
    cout << "end init_suite" << endl;
    return 0;
}