#include <metasim.hpp>
#include <rttask.hpp>
#include <mrtkernel.hpp>
#include <energyMRTKernel.hpp>
#include <edfsched.hpp>
#include <cbserver.hpp>

#include "CUnit/Automated.h"
#include "CUnit/CUnit.h"

using namespace MetaSim;
using namespace RTSim;
using namespace std;

/*void testMulticore()
{
    EDFScheduler sched;
    MRTKernel kern(&sched, 2);

    PeriodicTask t1(10, 10, 0, "task 1");
    t1.insertCode("fixed(4);");
    t1.setAbort(false);
    PeriodicTask t2(15, 15, 0, "task 2");
    t2.insertCode("fixed(5);");
    t2.setAbort(false);
    PeriodicTask t3(25, 25, 0, "task 3");
    t3.insertCode("fixed(4);");
    t3.setAbort(false);

    kern.addTask(t1);
    kern.addTask(t2);
    kern.addTask(t3);
    
    SIMUL.initSingleRun();

    SIMUL.run_to(4);

    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 4);
    REQUIRE(t3.getExecTime() == 0);

    SIMUL.run_to(5);

    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t3.getExecTime() == 1);
    REQUIRE(t2.getExecTime() == 5);

    SIMUL.run_to(8);

    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t3.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 5);

    SIMUL.endSingleRun();
}

void test_multicoreWithCbs()
{
    EDFScheduler sched;
    MRTKernel kern(&sched, 2);

    PeriodicTask t1(10, 10, 0, "task 1");
    t1.insertCode("fixed(4);");
    t1.setAbort(false);
    PeriodicTask t2(15, 15, 0, "task 2");
    t2.insertCode("fixed(5);");
    t2.setAbort(false);
    PeriodicTask t3(25, 25, 0, "task 3");
    t3.insertCode("fixed(4);");
    t3.setAbort(false);

    CBServer serv1(4, 10, 10, true,  "server1", "FIFOSched");
    CBServer serv2(5, 15, 15, true,  "server2", "FIFOSched");
    CBServer serv3(2, 12, 12, true,  "server3", "FIFOSched");

    serv1.addTask(t1);
    serv2.addTask(t2);
    serv3.addTask(t3);
    
    kern.addTask(serv1);
    kern.addTask(serv2);
    kern.addTask(serv3);

    SIMUL.initSingleRun();

    SIMUL.run_to(1);
    REQUIRE(t1.getExecTime() == 1);
    REQUIRE(t2.getExecTime() == 0);
    REQUIRE(t3.getExecTime() == 1);
    REQUIRE(serv1.get_remaining_budget() == 3);
    REQUIRE(serv1.getDeadline() == 10);
    REQUIRE(serv2.get_remaining_budget() == 5);
    REQUIRE(serv2.getDeadline() == 15);
    REQUIRE(serv3.get_remaining_budget() == 1);
    REQUIRE(serv3.getDeadline() == 12);

    SIMUL.run_to(2);
    REQUIRE(t1.getExecTime() == 2);
    REQUIRE(t2.getExecTime() == 0);
    REQUIRE(t3.getExecTime() == 2);
    REQUIRE(serv1.get_remaining_budget() == 2);
    REQUIRE(serv1.getDeadline() == 10);
    REQUIRE(serv2.get_remaining_budget() == 5);
    REQUIRE(serv2.getDeadline() == 15);
    REQUIRE(serv3.get_remaining_budget() == 2);
    REQUIRE(serv3.getDeadline() == 24);

    SIMUL.run_to(4);
    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 2);
    REQUIRE(t3.getExecTime() == 2);
    REQUIRE(serv1.get_remaining_budget() == 4);
    REQUIRE(serv1.getDeadline() == 20);
    REQUIRE(serv2.get_remaining_budget() == 3);
    REQUIRE(serv2.getDeadline() == 15);
    REQUIRE(serv3.get_remaining_budget() == 2);
    REQUIRE(serv3.getDeadline() == 24);

    SIMUL.run_to(5);
    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 3);
    REQUIRE(t3.getExecTime() == 2);
    REQUIRE(serv1.get_remaining_budget() == 4);
    REQUIRE(serv1.getDeadline() == 20);
    REQUIRE(serv2.get_remaining_budget() == 2);
    REQUIRE(serv2.getDeadline() == 15);
    REQUIRE(serv3.get_remaining_budget() == 2);
    REQUIRE(serv3.getDeadline() == 24);

    SIMUL.run_to(7);
    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 2);
    REQUIRE(serv1.get_remaining_budget() == 4);
    REQUIRE(serv1.getDeadline() == 20);
    REQUIRE(serv2.get_remaining_budget() == 5);
    REQUIRE(serv2.getDeadline() == 30);
    REQUIRE(serv3.get_remaining_budget() == 2);
    REQUIRE(serv3.getDeadline() == 24);

    SIMUL.run_to(10);
    REQUIRE(t1.getExecTime() == 0);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 2);
    REQUIRE(serv1.get_remaining_budget() == 4);
    REQUIRE(serv1.getDeadline() == 20);
    REQUIRE(serv2.get_remaining_budget() == 5);
    REQUIRE(serv2.getDeadline() == 30);
    REQUIRE(serv3.get_remaining_budget() == 2);
    REQUIRE(serv3.getDeadline() == 24);

    SIMUL.run_to(12);
    REQUIRE(t1.getExecTime() == 2);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 2);
    REQUIRE(serv1.get_remaining_budget() == 2);
    REQUIRE(serv1.getDeadline() == 20);
    REQUIRE(serv2.get_remaining_budget() == 5);
    REQUIRE(serv2.getDeadline() == 30);
    REQUIRE(serv3.get_remaining_budget() == 2);
    REQUIRE(serv3.getDeadline() == 24);

    SIMUL.run_to(14);
    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 4);
    REQUIRE(serv1.get_remaining_budget() == 4);
    REQUIRE(serv1.getDeadline() == 30);
    REQUIRE(serv2.get_remaining_budget() == 5);
    REQUIRE(serv2.getDeadline() == 30);
    REQUIRE(serv3.get_remaining_budget() == 2);
    REQUIRE(serv3.getDeadline() == 36);

   SIMUL.endSingleRun();
}*/

unsigned int OPP_little = 0; // Index of OPP in LITTLE cores
unsigned int OPP_big = 0;    // Index of OPP in big cores
string workload = "bzip2";
vector<CPU*> cpus;
int init_sequence = 0;

vector<EDFScheduler *> schedulers;
vector<RTKernel *> kernels;

int cleanup_suite() {
    cout << "cleanup_suite" << endl;
    cpus.clear();
    schedulers.clear();
    kernels.clear();

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

        /*EDFScheduler *edfsched = new EDFScheduler;
        schedulers.push_back(edfsched);

        RTKernel *kern = new RTKernel(edfsched, "", c);
        kernels.push_back(kern);*/
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

        /*EDFScheduler *edfsched = new EDFScheduler;
        schedulers.push_back(edfsched);

        RTKernel *kern = new RTKernel(edfsched, "", c);
        kernels.push_back(kern);*/
    }

    EDFScheduler *edfsched = new EDFScheduler;
    schedulers.push_back(edfsched);

    EnergyMRTKernel *kern = new EnergyMRTKernel(edfsched, cpus, "The sole kernel" + to_string(init_sequence));
    kernels.push_back(kern);

    CPU::referenceFrequency = 2000; // BIG_3 frequency

    init_sequence++;
    cout << "end init_suite" << endl;
    return 0;
}

string task_name = "";

void energyTest0() {
    cout << "workload " << workload << endl;
    task_name = "T0_Task_LITTLE_0";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask *t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(100," + workload + ");");
    kernels[0]->addTask(*t0, "");
    //ttrace.attachToTask(*t0);
    //jtrace.attachToTask(*t);

    task_name = "T0_Task_LITTLE_1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask *t1 = new PeriodicTask(500, 500, 0, task_name);
    t1->insertCode("fixed(100," + workload + ");");
    kernels[0]->addTask(*t1, "");
    //ttrace.attachToTask(*t1);
    //jtrace.attachToTask(*t);

    task_name = "T0_Task_LITTLE_2";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask *t2 = new PeriodicTask(500, 500, 0, task_name);
    t2->insertCode("fixed(100," + workload + ");");
    kernels[0]->addTask(*t2, "");
    //ttrace.attachToTask(*t2);
    //jtrace.attachToTask(*t);

    task_name = "T0_Task_LITTLE_3";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask *t3 = new PeriodicTask(500, 500, 0, task_name);
    t3->insertCode("fixed(100," + workload + ");");
    kernels[0]->addTask(*t3, "");
    //ttrace.attachToTask(*t3);
    //jtrace.attachToTask(*t);

    task_name = "T0_Task_big_0";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask *t4 = new PeriodicTask(500, 500, 0, task_name);
    t4->insertCode("fixed(100," + workload + ");");
    kernels[0]->addTask(*t4, "");
    //ttrace.attachToTask(*t4);

    SIMUL.initSingleRun();
    SIMUL.run_to(10);

    CPU *c0 = kernels[0]->getProcessor(t0);
    CPU *c1 = kernels[0]->getProcessor(t1);
    CPU *c2 = kernels[0]->getProcessor(t2);
    CPU *c3 = kernels[0]->getProcessor(t3);
    CPU *c4 = kernels[0]->getProcessor(t4);

    CU_ASSERT(t0->getName() == "T0_Task_LITTLE_0");
    CU_ASSERT(c0->getFrequency() == 700);
    CU_ASSERT(c0->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t0->getWCET(c0->getSpeed())))  == 488);

    
    CU_ASSERT(t1->getName() == "T0_Task_LITTLE_1");
    CU_ASSERT(c1->getFrequency() == 700);
    CU_ASSERT(c1->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t1->getWCET(c1->getSpeed()))) == 488);

    CU_ASSERT(t2->getName() == "T0_Task_LITTLE_2");
    CU_ASSERT(c2->getFrequency() == 700);
    CU_ASSERT(c2->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t2->getWCET(c2->getSpeed()))) == 488);

    CU_ASSERT(t3->getName() == "T0_Task_LITTLE_3");
    CU_ASSERT(c3->getFrequency() == 700);
    CU_ASSERT(c3->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t3->getWCET(c3->getSpeed()))) == 488);

    CU_ASSERT(t4->getName() == "T0_Task_big_0");
    CU_ASSERT(c4->getFrequency() == 700);
    CU_ASSERT(c4->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t4->getWCET(c4->getSpeed()))) == 251);

    SIMUL.endSingleRun();
}

void energyTest1() {
    task_name = "T1_task1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
    kernels[0]->addTask(*t0, "");
    //ttrace.attachToTask(*t0);

    SIMUL.initSingleRun();
    SIMUL.run_to(10);

    CPU *c0 = kernels[0]->getProcessor(t0);

    CU_ASSERT(t0->getName() == "T1_task1");
    CU_ASSERT(c0->getFrequency() == 2000);
    CU_ASSERT(c0->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t0->getWCET(c0->getSpeed())))  == 497);

    SIMUL.endSingleRun();
}

void energyTest2() {
    task_name = "T2_task1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
    kernels[0]->addTask(*t0, "");
    //ttrace.attachToTask(*t);
    //jtrace.attachToTask(*t);

    task_name = "T2_task2";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t1 = new PeriodicTask(500, 500, 0, task_name);
    t1->insertCode("fixed(500," + workload + ");");
    kernels[0]->addTask(*t1, "");
    //ttrace.attachToTask(*t);

    SIMUL.initSingleRun();
    SIMUL.run_to(10);

    CPU *c0 = kernels[0]->getProcessor(t0);
    CPU *c1 = kernels[0]->getProcessor(t1);

    CU_ASSERT(t0->getName() == "T2_task1");
    CU_ASSERT(c0->getFrequency() == 2000);
    CU_ASSERT(c0->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t0->getWCET(c0->getSpeed())))  == 497);

    CU_ASSERT(t1->getName() == "T2_task2");
    CU_ASSERT(c1->getFrequency() == 2000);
    CU_ASSERT(c1->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t1->getWCET(c1->getSpeed()))) == 497);

    SIMUL.endSingleRun();
}

void energyTest3() {
    task_name = "T3_task1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
    kernels[0]->addTask(*t0, "");
    //ttrace.attachToTask(*t0);
    //jtrace.attachToTask(*t);

    task_name = "T3_task2";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t1 = new PeriodicTask(500, 500, 0, task_name);
    t1->insertCode("fixed(250," + workload + ");");
    kernels[0]->addTask(*t1, "");
    //ttrace.attachToTask(*t1);

    SIMUL.initSingleRun();
    SIMUL.run_to(10);

    CPU *c0 = kernels[0]->getProcessor(t0);
    CPU *c1 = kernels[0]->getProcessor(t1);

    for(string s : kernels[0]->getRunningTasks())
      cout << "running :" << s<<endl;

    CU_ASSERT(t0->getName() == "T3_task1");
    CU_ASSERT(c0->getFrequency() == 2000);
    CU_ASSERT(c0->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t0->getWCET(c0->getSpeed())))  == 497);

    CU_ASSERT(t1->getName() == "T3_task2");
    CU_ASSERT(c1->getFrequency() == 2000);
    CU_ASSERT(c1->getIsland() == CPU::Island::BIG);
    CU_ASSERT(int(double(t1->getWCET(c1->getSpeed()))) == 248);

    SIMUL.endSingleRun();
}

void energyTest4() {
    task_name = "T4_task1";
    cout << "Creating task: " << task_name << endl;
    PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
    t0->insertCode("fixed(10," + workload + ");"); // WCET 10 at max frequency on big cores
    kernels[0]->addTask(*t0, "");
    //ttrace.attachToTask(*t);

    SIMUL.initSingleRun();
    SIMUL.run_to(10);

    CPU *c0 = kernels[0]->getProcessor(t0);

    CU_ASSERT(t0->getName() == "T4_task1");
    CU_ASSERT(c0->getFrequency() == 500);
    CU_ASSERT(c0->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t0->getWCET(c0->getSpeed())))  == 65);

    SIMUL.endSingleRun();
}

void energyTest5() {
    vector<PeriodicTask*> task;
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
        //ttrace.attachToTask(*t);

        task.push_back(t);
        // LITTLE_0, _1, _2, _3 freq 1400.
    }

    SIMUL.initSingleRun();
    SIMUL.run_to(10);

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

// test showing that frequency of little island may be raised to let
// scheduling on big island be last choice 
void energyTest6() {
    vector<PeriodicTask*> task;
    vector<CPU*> cpu_task;
    int i;
    for (int j = 0; j < 5; j++) {
        int wcet = 5; //* (j+1);
        task_name = "T6_task" + std::to_string(j);
        cout << "Creating task: " << task_name;
        PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
        char instr[60] = "";
        sprintf(instr, "fixed(%d, %s);", wcet, workload.c_str());
        cout << " with abs. WCET " << wcet << endl;
        t->insertCode(instr);
        kernels[0]->addTask(*t, "");
        //ttrace.attachToTask(*t);

        task.push_back(t);
        // LITTLE_0, _1, _2, _3 freq 1400.
    }

    SIMUL.initSingleRun();
    SIMUL.run_to(10);

    cpu_task.push_back(kernels[0]->getProcessor(task[0]));
    cpu_task.push_back(kernels[0]->getProcessor(task[1]));
    cpu_task.push_back(kernels[0]->getProcessor(task[2]));
    cpu_task.push_back(kernels[0]->getProcessor(task[3]));
    cpu_task.push_back(kernels[0]->getProcessor(task[4]));

    i = 0;
    PeriodicTask* t = task[i];
    CPU* c = cpu_task[i];
    CU_ASSERT(t->getName() == "T6_task0");
    CU_ASSERT(c->getFrequency() == 500);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed())))  == 32);
    printf("aaa %s scheduled on %s freq %d with wcet %d\n", t->getName(), c->print(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 1;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T6_task1");
    CU_ASSERT(c->getFrequency() == 500);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 32);
    printf("aaa %s scheduled on %s freq %d with wcet %d\n", t->getName(), c->print(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 2;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T6_task2");
    CU_ASSERT(c->getFrequency() == 500);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 32);
    printf("aaa %s scheduled on %s freq %d with wcet %d\n", t->getName(), c->print(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 3;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T6_task3");
    CU_ASSERT(c->getFrequency() == 500);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 32);
    printf("aaa %s scheduled on %s freq %d with wcet %d\n", t->getName(), c->print(), c->getFrequency(), t->getWCET(c->getSpeed()));

    i = 4;
    t = task[i];
    c = cpu_task[i];
    CU_ASSERT(t->getName() == "T6_task4");
    CU_ASSERT(c->getFrequency() == 500);
    CU_ASSERT(c->getIsland() == CPU::Island::LITTLE);
    CU_ASSERT(int(double(t->getWCET(c->getSpeed()))) == 32);
    printf("aaa %s scheduled on %s freq %d with wcet %d\n", t->getName(), c->print(), c->getFrequency(), t->getWCET(c->getSpeed()));

    SIMUL.endSingleRun();
}

int main()
{
    // create a suite
    CU_pSuite pSuite[6] = {NULL};
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry. A suite per test */
    pSuite[0] = CU_add_suite("Suite_0", init_suite, cleanup_suite);
    if (NULL == pSuite[0]) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    pSuite[1] = CU_add_suite("Suite_1", init_suite, cleanup_suite);
    if (NULL == pSuite[1]) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    pSuite[2] = CU_add_suite("Suite_2", init_suite, cleanup_suite);
    if (NULL == pSuite[2]) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    pSuite[3] = CU_add_suite("Suite_3", init_suite, cleanup_suite);
    if (NULL == pSuite[3]) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    pSuite[4] = CU_add_suite("Suite_4", init_suite, cleanup_suite);
    if (NULL == pSuite[4]) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    pSuite[5] = CU_add_suite("Suite_5", init_suite, cleanup_suite);
    if (NULL == pSuite[5]) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (
        (NULL == CU_add_test(pSuite[0], "test of test_energy0()", energyTest0))
        )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        (NULL == CU_add_test(pSuite[1], "test of test_energy1()", energyTest1))
        )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        (NULL == CU_add_test(pSuite[2], "test of test_energy2()", energyTest2))
        )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        (NULL == CU_add_test(pSuite[3], "test of test_energy3()", energyTest3))
        )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        (NULL == CU_add_test(pSuite[4], "test of test_energy4()", energyTest4))
        )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        (NULL == CU_add_test(pSuite[5], "test of test_energy5()", energyTest5))
        )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_list_tests_to_file();
    CU_set_output_filename("results");
    CU_set_error_action(CUEA_FAIL);

    /* Run all tests using the CUnit Basic interface */
    //init_suite();

    CU_automated_run_tests();
    CU_cleanup_registry();
    return CU_get_error();

    printf("ciaO");
}
