#include <metasim.hpp>
#include <rttask.hpp>
#include <mrtkernel.hpp>
#include <energyMRTKernel.hpp>
#include <edfsched.hpp>
#include <cbserver.hpp>

#include "catch.hpp"

#define SUITES_NO 8

using namespace MetaSim;
using namespace RTSim;
using namespace std;

string workload = "bzip2";
string task_name = "";
int init_sequence = 0;

int cleanup_suite();
int init_suite(vector<CPU_BL*> *cpus, EnergyMRTKernel** kern, EDFScheduler** edfsched);
bool inRange(int,int);


TEST_CASE("exp0") {
    cout << "Begin of experiment " << init_sequence << endl;

    vector<CPU_BL*> cpus;
    vector<PeriodicTask*> task; // to be cleared after each test
    EDFScheduler *edfsched; EnergyMRTKernel *kern;
    init_suite(&cpus, &kern, &edfsched);
    assert(kern != NULL); assert(edfsched != NULL);

    task_name = "T0_task1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
    kern->addTask(*t0, "");
    //ttrace.attachToTask(*t0);

    SIMUL.initSingleRun();
    SIMUL.run_to(1);

    CPU_BL *c0 = dynamic_cast<CPU_BL*>(dynamic_cast<CPU_BL*>(kern->getProcessor(t0)));

    REQUIRE(t0->getName() == "T0_task1");
    REQUIRE(c0->getFrequency() == 2000);
    REQUIRE(c0->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t0->getWCET(c0->getSpeed())), 497));

    SIMUL.endSingleRun();
    delete t0;
    for (CPU_BL* c:cpus) delete c;
    cpus.clear();
    delete edfsched; delete kern;
}

TEST_CASE("exp1") {
    cout << "Begin of experiment " << init_sequence << endl;

    vector<CPU_BL*> cpus;
    vector<PeriodicTask*> task; // to be cleared after each test
    EDFScheduler *edfsched; EnergyMRTKernel *kern;
    init_suite(&cpus, &kern, &edfsched);
    assert(kern != NULL); assert(edfsched != NULL);

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

    REQUIRE(t0->getName() == "T1_task1");
    REQUIRE(c0->getFrequency() == 2000);
    REQUIRE(c0->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t0->getWCET(c0->getSpeed())), 497));

    REQUIRE(t1->getName() == "T1_task2");
    REQUIRE(c1->getFrequency() == 2000);
    REQUIRE(c1->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t1->getWCET(c1->getSpeed())), 497));

    SIMUL.endSingleRun();
    delete t1; delete t0;
    for (CPU_BL* c:cpus) delete c;
    cpus.clear();
    delete edfsched;
    delete kern;
}

TEST_CASE("exp2") {
    cout << "Begin of experiment " << init_sequence << endl;

    vector<CPU_BL*> cpus;
    EDFScheduler *edfsched; EnergyMRTKernel *kern;
    init_suite(&cpus, &kern, &edfsched);
    assert(kern != NULL); assert(edfsched != NULL);

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

    REQUIRE(t0->getName() == "T2_task1");
    REQUIRE(c0->getFrequency() == 2000);
    REQUIRE(c0->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t0->getWCET(c0->getSpeed())), 497));

    REQUIRE(t1->getName() == "T2_task2");
    REQUIRE(c1->getFrequency() == 2000);
    REQUIRE(c1->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t1->getWCET(c1->getSpeed())), 248));

    SIMUL.endSingleRun();
    delete t0; delete t1;
    for (CPU_BL* c:cpus) delete c;
    cpus.clear();
    delete edfsched;
    delete kern;
}

TEST_CASE("exp3") {
    cout << "Begin of experiment " << init_sequence << endl;

    vector<CPU_BL*> cpus;
    EDFScheduler *edfsched; EnergyMRTKernel *kern;
    init_suite(&cpus, &kern, &edfsched);
    assert(kern != NULL); assert(edfsched != NULL);

    task_name = "T3_task1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(10," + workload + ");"); // WCET 10 at max frequency on big cores
    kern->addTask(*t0, "");

    SIMUL.initSingleRun();
    SIMUL.run_to(10);

    CPU_BL *c0 = dynamic_cast<CPU_BL*>(kern->getProcessor(t0));

    REQUIRE(t0->getName() == "T3_task1");
    REQUIRE(c0->getFrequency() == 500);
    REQUIRE(c0->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(t0->getWCET(c0->getSpeed())), 65));

    SIMUL.endSingleRun();
    delete t0;
    for (CPU_BL* c:cpus) delete c;
    cpus.clear();
    delete edfsched;
    delete kern;
}

TEST_CASE("exp4") {
    cout << "Begin of experiment " << init_sequence << endl;

    vector<CPU_BL*> cpus;
    PeriodicTask* task[5]; // to be cleared after each test
    EDFScheduler *edfsched; EnergyMRTKernel *kern;
    init_suite(&cpus, &kern, &edfsched);
    assert(kern != NULL); assert(edfsched != NULL);

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

    REQUIRE(task[0]->getName() == "T4_Task_LITTLE_0");
    REQUIRE(c0->getFrequency() == 700);
    REQUIRE(c0->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(task[0]->getWCET(c0->getSpeed())), 488));

    REQUIRE(task[1]->getName() == "T4_Task_LITTLE_1");
    REQUIRE(c1->getFrequency() == 700);
    REQUIRE(c1->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(task[1]->getWCET(c1->getSpeed())), 488));

    REQUIRE(task[2]->getName() == "T4_Task_LITTLE_2");
    REQUIRE(c2->getFrequency() == 700);
    REQUIRE(c2->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(task[2]->getWCET(c2->getSpeed())), 488));

    REQUIRE(task[3]->getName() == "T4_Task_LITTLE_3");
    REQUIRE(c3->getFrequency() == 700);
    REQUIRE(c3->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(task[3]->getWCET(c3->getSpeed())), 488));

    SIMUL.endSingleRun();
    for (int j = 0; j < 4; j++)
        delete task[j];
    for (CPU_BL* c:cpus) delete c;
    cpus.clear();
    delete edfsched;
    delete kern;
}

TEST_CASE("exp5") {
    cout << "Begin of experiment " << init_sequence << endl;

    vector<CPU_BL*> cpus;
    PeriodicTask* task[5]; // to be cleared after each test
    EDFScheduler *edfsched; EnergyMRTKernel *kern;
    init_suite(&cpus, &kern, &edfsched);
    assert(kern != NULL); assert(edfsched != NULL);

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
    abort();

    PeriodicTask* t = task[0];
    REQUIRE(t->getName() == "T5_task0");
    REQUIRE(c0->getFrequency() == 500);
    REQUIRE(c0->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(t->getWCET(c0->getSpeed())), 32));
    cout << t->toString() << " on "<< c0->toString()<<endl;

    t = task[1];
    REQUIRE(t->getName() == "T5_task1");
    REQUIRE(c1->getFrequency() == 500);
    REQUIRE(c1->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(t->getWCET(c1->getSpeed())), 32));
    cout << t->toString() << " on "<< c1->toString()<<endl;

    t = task[2];
    REQUIRE(t->getName() == "T5_task2");
    REQUIRE(c2->getFrequency() == 500);
    REQUIRE(c2->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(t->getWCET(c2->getSpeed())), 32));
    cout << t->toString() << " on "<< c2->toString()<<endl;

    t = task[3];
    REQUIRE(t->getName() == "T5_task3");
    REQUIRE(c3->getFrequency() == 500);
    REQUIRE(c3->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(t->getWCET(c3->getSpeed())), 32));
    cout << t->toString() << " on "<< c3->toString()<<endl;

    SIMUL.endSingleRun();
    for (int j = 0; j < 4; j++)
        delete task[j];
    for (CPU_BL* c:cpus) delete c;
    cpus.clear();
    delete edfsched; delete kern;
}

// test showing that frequency of little/big island may be raised
TEST_CASE("exp6") {
    cout << "Begin of experiment " << init_sequence << endl;

    vector<CPU_BL*> cpus;
    CPU_BL* cpu_task[5]; // to be cleared after each test
    PeriodicTask* task[5]; // to be cleared after each test
    EDFScheduler *edfsched; EnergyMRTKernel *kern;
    init_suite(&cpus, &kern, &edfsched);
    assert(kern != NULL); assert(edfsched != NULL);

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
    		cpu_task[j] = kern->getDispatchingProcessor(task[j]);
    }

    i = 0;
    PeriodicTask* t = task[i];
    CPU_BL* c = cpu_task[i];
    REQUIRE(t->getName() == "T6_task0");
    REQUIRE(c->getFrequency() == 2000);
    REQUIRE(c->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 299));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 1;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T6_task1");
    REQUIRE(c->getFrequency() == 2000);
    REQUIRE(c->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 299));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 2;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T6_task2");
    REQUIRE(c->getFrequency() == 2000);
    REQUIRE(c->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 299));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 3;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T6_task3");
    REQUIRE(c->getFrequency() == 2000);
    REQUIRE(c->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())),299));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 4;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T6_task4");
    REQUIRE(c->getFrequency() == 2000);
    REQUIRE(c->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 199));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    SIMUL.endSingleRun();
    for (int j = 0; j < 5; j++)
        delete task[j];
    for (CPU_BL* c:cpus) delete c;
    delete edfsched; delete kern;
}

TEST_CASE("exp7") {
    cout << "Begin of experiment " << init_sequence << endl;

    vector<CPU_BL*> cpus;
    PeriodicTask* task[5]; // to be cleared after each test
    CPU_BL* cpu_task[5]; // to be cleared after each test
    EDFScheduler *edfsched; EnergyMRTKernel *kern;
    init_suite(&cpus, &kern, &edfsched);
    assert(kern != NULL); assert(edfsched != NULL);

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
    REQUIRE(t->getName() == "T7_task0");
    REQUIRE(c->getFrequency() == 500);
    REQUIRE(c->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 415));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 1;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T7_task1");
    REQUIRE(c->getFrequency() == 500);
    REQUIRE(c->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 415));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 2;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T7_task2");
    REQUIRE(c->getFrequency() == 500);
    REQUIRE(c->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 415));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 3;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T7_task3");
    REQUIRE(c->getFrequency() == 500);
    REQUIRE(c->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 415));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 4;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T7_task4");
    REQUIRE(c->getFrequency() == 700);
    REQUIRE(c->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 75));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    SIMUL.endSingleRun();
    for (int j = 0; j < 4; j++)
        delete task[j];
    for (CPU_BL* c:cpus) delete c;
    delete edfsched; delete kern;
}

TEST_CASE("exp8") {
    cout << "Begin of experiment " << init_sequence << endl;

    vector<CPU_BL*> cpus;
    PeriodicTask* task[5]; // to be cleared after each test
    CPU_BL* cpu_task[5]; // to be cleared after each test
    EDFScheduler *edfsched; EnergyMRTKernel *kern;
    init_suite(&cpus, &kern, &edfsched);
    assert(kern != NULL); assert(edfsched != NULL);

    int wcets[] = { 181, 419, 261, 163, 65, 8, 61, 170, 273 };
    int i;
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

    i = 0;
    PeriodicTask* t = task[i];
    CPU_BL* c = cpu_task[i];
    REQUIRE(t->getName() == "T8_task" + i);
    REQUIRE(c->getFrequency() == 1400);
    REQUIRE(c->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 499));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 1;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T8_task" + i);
    REQUIRE(c->getFrequency() == 1700);
    REQUIRE(c->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 477));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 2;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T8_task" + i);
    REQUIRE(c->getFrequency() == 1700);
    REQUIRE(c->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 261));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 3;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T8_task"+i);
    REQUIRE(c->getFrequency() == 1700);
    REQUIRE(c->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 297));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 4;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T8_task"+i);
    REQUIRE(c->getFrequency() == 1400);
    REQUIRE(c->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 162));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 5;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T8_task"+i);
    REQUIRE(c->getFrequency() == 1400);
    REQUIRE(c->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 8));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 6;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T8_task"+i);
    REQUIRE(c->getFrequency() == 1400);
    REQUIRE(c->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 61));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 7;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T8_task"+i);
    REQUIRE(c->getFrequency() == 1400);
    REQUIRE(c->getIslandType() == Island::LITTLE);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 170));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 8;
    t = task[i];
    c = cpu_task[i];
    REQUIRE(t->getName() == "T8_task"+i);
    REQUIRE(c->getFrequency() == 1700);
    REQUIRE(c->getIslandType() == Island::BIG);
    REQUIRE(inRange(int(t->getWCET(c->getSpeed())), 273));
    printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

    SIMUL.endSingleRun();
    for (int j = 0; j < 4; j++)
        delete task[j];
    for (CPU_BL* c:cpus) delete c;
    delete edfsched; delete kern;
}

// Not finding an example with a call to a local function, I outed out for CUnit.
// But I think it's duable to conform to framework TEST UNIT
int init_suite(vector<CPU_BL*> *cpus, EnergyMRTKernel** kern, EDFScheduler** edfsched) {
    cout << "init_suite" << endl;

    unsigned int OPP_little = 0; // Index of OPP in LITTLE cores
    unsigned int OPP_big = 0;    // Index of OPP in big cores
    vector<CPU_BL *> cpus_little, cpus_big;

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
    Island_BL *island_bl_little = new Island_BL("island_little", Island::LITTLE, cpus_little, opps_little);
    Island_BL *island_bl_big = new Island_BL("island_big", Island::BIG, cpus_big, opps_big);

    *edfsched = new EDFScheduler;
    //schedulers.push_back(edfsched);

    *kern = new EnergyMRTKernel(*edfsched, island_bl_big, island_bl_little, "The sole kernel");
    //kernels.push_back(kern);

    island_bl_big->setKernel(*kern);
    island_bl_little->setKernel(*kern);
    CPU_BL::referenceFrequency = 2000; // BIG_3 frequency

    init_sequence++;
    cout << "end init_suite" << endl;
    return 0;
}

bool inRange(int eval, int expected) {
    const int error = 5;

    int min = int(eval - eval * error/100);
    int max = int(eval + eval * error/100);

    return expected >= min && expected <= max; 
}