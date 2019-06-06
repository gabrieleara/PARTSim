/*
  In this example, a simple system is simulated, to provide
  an evaluation of different workloads running on an big.LITTLE
  Odroid-XU3 embedded board.
*/

#include <cstring>
#include <string>
#include <fstream>

#include <mrtkernel.hpp>
#include <cpu.hpp>
#include <edfsched.hpp>
#include <jtrace.hpp>
#include <texttrace.hpp>
#include <json_trace.hpp>
#include <ps_trace.hpp>
#include <tracepower.hpp>
#include <rttask.hpp>
#include <instr.hpp>
#include <powermodel.hpp>
#include <energyMRTKernel.hpp>
#include <assert.h>
#include "rrsched.hpp"
#include "rttask.hpp"
#include "cpu.hpp"
#include "cbserver.hpp"

using namespace MetaSim;
using namespace RTSim;

#define REQUIRE assert
#define time()  SIMUL.getTime()

void dumpSpeeds(CPUModelBP::ComputationalModelBPParams const & params);

void dumpAllSpeeds();
bool isInRange(int,int);
bool isInRange(Tick t1, int t2) { return isInRange(int(t1), t2); }
bool isInRange(Tick t1, Tick t2) { return isInRange(int(t1), int(t2)); }
bool isInRangeMinMax(double eval, const double min, const double max);

int main(int argc, char *argv[]) {
    unsigned int OPP_little = 0; // Index of OPP in LITTLE cores
    unsigned int OPP_big = 0;    // Index of OPP in big cores
    string workload = "bzip2";
    int TEST_NO = 20;

    if (argc == 4) {
        OPP_little = stoi(argv[1]);
        OPP_big = stoi(argv[2]);
        workload = argv[3];
    }
   if (argc == 2) { TEST_NO = stoi(argv[1]);  }

   cout << "Performing experiment #" << TEST_NO << endl;

    cout << "current OPPs indices: [" << OPP_little << ", " << OPP_big << "]" << endl;
    cout << "Workload: [" << workload << "]" << endl;



    EnergyMRTKernel::EMRTK_LEAVE_LITTLE3_ENABLED                     = 0;
    EnergyMRTKernel::EMRTK_MIGRATE_ENABLED                           = 1;
    EnergyMRTKernel::EMRTK_CBS_YIELD_ENABLED                         = 0;

    EnergyMRTKernel::EMRTK_CBS_ENVELOPING_PER_TASK_ENABLED           = 1;
    EnergyMRTKernel::EMRTK_CBS_ENVELOPING_MIGRATE_AFTER_VTIME_END    = 0;
    EnergyMRTKernel::EMRTK_CBS_MIGRATE_AFTER_END                     = 1;



    try {
        SIMUL.dbg.enable("All");
        SIMUL.dbg.setStream("debug.txt");

        vector<TracePowerConsumption *> ptrace;
        vector<Scheduler *> schedulers;
        vector<RTKernel *> kernels;
        vector<CPU_BL *> cpus_little, cpus_big;
        vector<AbsRTTask*> tasks;

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
            string cpu_name = "LITTLE_" + to_string(i);

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
            CPU_BL *c = new CPU_BL(cpu_name, "idle", pm);
            c->setIndex(i);
            pm->setCPU(c);
            pm->setFrequencyMax(max_frequency);
            TracePowerConsumption *power_trace = new TracePowerConsumption(c, 1, "power_" + cpu_name + ".txt");
            ptrace.push_back(power_trace);

            cpus_little.push_back(c);
        }

        for (unsigned int i = 0; i < 4; ++i) {
            /* Create 4 big CPUs */

            string cpu_name = "BIG_" + to_string(i);

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

            CPU_BL *c = new CPU_BL(cpu_name, "idle", pm);
            c->setIndex(i);
            pm->setCPU(c);
            pm->setFrequencyMax(max_frequency);
            TracePowerConsumption *power_trace = new TracePowerConsumption(c, 1, "power_" + cpu_name + ".txt");
            ptrace.push_back(power_trace);

            cpus_big.push_back(c);
        }

        vector<struct OPP> opps_little = Island_BL::buildOPPs(V_little, F_little);
        vector<struct OPP> opps_big = Island_BL::buildOPPs(V_big, F_big);
        Island_BL *island_bl_little = new Island_BL("little island", IslandType::LITTLE, cpus_little, opps_little);
        Island_BL *island_bl_big = new Island_BL("big island", IslandType::BIG, cpus_big, opps_big);

        EDFScheduler *edfsched = new EDFScheduler;
        for (int i = 0; i < 8; i++)
            schedulers.push_back(new EDFScheduler);

        EnergyMRTKernel *kern = new EnergyMRTKernel(schedulers, edfsched, island_bl_big, island_bl_little, "The sole kernel");
        kernels.push_back(kern);

        CPU_BL::referenceFrequency = 2000; // BIG_3 frequency

        /*
         * Creating tasks
         */

        PeriodicTask *t;
        vector<CBServerCallingEMRTKernel*> ets;

        /* LITTLE */

        string task_name;
        TextTrace ttrace("trace" + to_string(TEST_NO) + ".txt");
        JSONTrace jtrace("trace" + to_string(TEST_NO) + ".json");
        PSTrace  pstrace("trace" + to_string(TEST_NO) + ".pst");
        cout << "Test to perform is " << TEST_NO << endl;

        if (TEST_NO == 0) {
            task_name = "T0_task1";
            cout << "Creating task: " << task_name << endl;
            PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
            t0->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
            CBServerCallingEMRTKernel* et_t0 = kern->addTaskAndEnvelope(t0, "");
            ttrace.attachToTask(*t0);
            jtrace.attachToTask(*t0);
            tasks.push_back(t0);

            SIMUL.initSingleRun();
            SIMUL.run_to(1);
            CPU_BL *c0 = dynamic_cast<CPU_BL*>(dynamic_cast<CPU_BL*>(kern->getProcessor(et_t0)));

            REQUIRE (c0->getFrequency() == 2000);
            REQUIRE (c0->getIslandType() == IslandType::BIG);
            REQUIRE (isInRange (int(t0->getWCET(c0->getSpeed())), 497));
            REQUIRE (et_t0->getStatus() == ServerStatus::EXECUTING);
            REQUIRE (et_t0->getPeriod() == 500);
            REQUIRE (isInRange (et_t0->getBudget(), 497));
            REQUIRE (isInRange (et_t0->getEndBandwidthEvent(), 497));
            REQUIRE (isInRange (et_t0->getReplenishmentEvent(), 500));

            SIMUL.run_to(499);
            cout << "end of virtualtime event: t=" << et_t0->getEndOfVirtualTimeEvent() << endl;
            REQUIRE (isInRange (et_t0->getEndOfVirtualTimeEvent(), 500));
            REQUIRE (et_t0->getStatus() == ServerStatus::RELEASING);

            SIMUL.run_to(501);
            REQUIRE (c0->getFrequency() == 2000);
            REQUIRE (c0->getIslandType() == IslandType::BIG);
            REQUIRE (isInRange(int(t0->getWCET(c0->getSpeed())), 497));
            REQUIRE (et_t0->getStatus() == ServerStatus::EXECUTING);
            REQUIRE (et_t0->getPeriod() == 500);
            REQUIRE (isInRange (et_t0->getBudget(), 497));
            REQUIRE (isInRange (et_t0->getEndBandwidthEvent(), 997));
            REQUIRE (isInRange (et_t0->getReplenishmentEvent(), 500));

            SIMUL.run_to(999);
            cout << "end of virtualtime event: t=" << et_t0->getEndOfVirtualTimeEvent() << endl;
            REQUIRE (isInRange (et_t0->getEndOfVirtualTimeEvent(), 1000));

            SIMUL.endSingleRun();
            cout << "-------------" << endl;
            cout << "Simultion finished" << endl;
            return 0;

            // only task1 (500,500) => BIG max freq = 2000, with 500 the scaled WCET
        }
        if (TEST_NO == 1) {
            task_name = "T1_task1";
            cout << "Creating task: " << task_name << endl;
            PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
            t0->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
            CBServerCallingEMRTKernel* et_t0 = kern->addTaskAndEnvelope(t0, "");

            task_name = "T1_task2";
            cout << "Creating task: " << task_name << endl;
            PeriodicTask* t1 = new PeriodicTask(500, 500, 0, task_name);
            t1->insertCode("fixed(500," + workload + ");");
            CBServerCallingEMRTKernel* et_t1 = kern->addTaskAndEnvelope(t1, "");

            SIMUL.initSingleRun();
            SIMUL.run_to(1);

            CPU_BL *c0 = dynamic_cast<CPU_BL*>(kern->getProcessor(et_t0));
            CPU_BL *c1 = dynamic_cast<CPU_BL*>(kern->getProcessor(et_t1));

            REQUIRE (c0->getFrequency() == 2000);
            REQUIRE (c0->getIslandType() == IslandType::BIG);
            REQUIRE (isInRange(int(t0->getWCET(c0->getSpeed())), 497));

            REQUIRE (c1->getFrequency() == 2000);
            REQUIRE (c1->getIslandType() == IslandType::BIG);
            REQUIRE (isInRange(int(t1->getWCET(c1->getSpeed())), 497));

            SIMUL.run_to(1000);
            SIMUL.endSingleRun();

            cout << "-------------" << endl;
            cout << "Simultion finished" << endl;
            return 0;
            // task1 (500,500) => BIG_3 max freq, task2 (500,500) => BIG_2 max freq
        }
        if (TEST_NO == 2) {
            task_name = "T2_task1";
            cout << "Creating task: " << task_name << endl;
            PeriodicTask* t0 = new PeriodicTask(500, 500, 0, task_name);
            t0->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
            CBServerCallingEMRTKernel* et_t0 = kern->addTaskAndEnvelope(t0, "");

            task_name = "T2_task2";
            cout << "Creating task: " << task_name << endl;
            PeriodicTask* t1 = new PeriodicTask(500, 500, 0, task_name);
            t1->insertCode("fixed(250," + workload + ");");
            CBServerCallingEMRTKernel* et_t1 = kern->addTaskAndEnvelope(t1, "");

            SIMUL.initSingleRun();
            SIMUL.run_to(1);

            CPU_BL *c0 = dynamic_cast<CPU_BL*>(kern->getProcessor(et_t0));
            CPU_BL *c1 = dynamic_cast<CPU_BL*>(kern->getProcessor(et_t1));

            for(string s : kern->getRunningTasks())
              cout << "running :" << s<<endl;

            REQUIRE (t0->getName() == "T2_task1");
            REQUIRE (c0->getFrequency() == 2000);
            REQUIRE (c0->getIslandType() == IslandType::BIG);
            REQUIRE (isInRange(int(t0->getWCET(c0->getSpeed())), 497));

            REQUIRE (t1->getName() == "T2_task2");
            REQUIRE (c1->getFrequency() == 2000);
            REQUIRE (c1->getIslandType() == IslandType::BIG);
            REQUIRE (isInRange(int(t1->getWCET(c1->getSpeed())), 248));

            SIMUL.run_to(1000);
            SIMUL.endSingleRun();

            // task1 (500,500) => BIG_3 max freq, task2 (250,500) => same
        }
        if (TEST_NO == 3) {
            task_name = "T3_task1";
            cout << "Creating task: " << task_name << endl;
            PeriodicTask *t0 = new PeriodicTask(500, 500, 0, task_name);
            t0->insertCode("fixed(10," + workload + ");"); // WCET 10 at max frequency on big cores
            CBServerCallingEMRTKernel* et_t0 = kern->addTaskAndEnvelope(t0, "");
            ttrace.attachToTask(*t0);
            //jtrace.attachToTask(*t);
            tasks.push_back(t0);

            SIMUL.initSingleRun();
            SIMUL.run_to(10);

            cout << "t=" << SIMUL.getTime() << endl;
            CPU_BL *c0 = dynamic_cast<CPU_BL*>(kern->getProcessor(et_t0));

            //REQUIRE (t0->getName() == "T3_task1");
            REQUIRE (c0->getFrequency() == 500);
            REQUIRE (c0->getIslandType() == IslandType::LITTLE);
            cout << "qua " << int(t0->getWCET(c0->getSpeed())) << endl;
            cout << "qua " << int(et_t0->getWCET(c0->getSpeed())) << endl;
            cout << "qua " << et_t0->toString() << endl;
            REQUIRE (isInRange(int(t0->getWCET(c0->getSpeed())), 65));

            SIMUL.endSingleRun();

            // little freq 500
        }
        if (TEST_NO == 4) {
            for (int j = 0; j < 4; j++) {
                task_name = "T4_task_LITTLE_" + std::to_string(j);
                cout << "Creating task: " << task_name;
                PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
                char instr[60] = "";
                sprintf(instr, "fixed(100, %s);", workload.c_str());
                t->insertCode(instr);
                ets.push_back(kern->addTaskAndEnvelope(t, ""));
                ttrace.attachToTask(*t);
                tasks.push_back(t);
            }
        }
        else if(TEST_NO == 5) {
            for (int j = 0; j < 4; j++) {
                int wcet = 5; //* (j+1);
                task_name = "T5_task" + std::to_string(j);
                cout << "Creating task: " << task_name;
                t = new PeriodicTask(500, 500, 0, task_name);
                char instr[60] = "";
                sprintf(instr, "fixed(%d, bzip2);", wcet);
                cout << " with abs. WCET " << wcet << endl;
                t->insertCode(instr);
                ets.push_back(kern->addTaskAndEnvelope(t, ""));
                ttrace.attachToTask(*t);
                tasks.push_back(t);
            }

            // small tasks. They should be scheduled on a single little at frequency with min voltage = 500
        }
        else if(TEST_NO == 6) {
          /* This experiment shows that other CPUs frequencies are increased for the
             entire island after a decision for other tasks has already been made */
            vector<CPU*> cpu_task;
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
                ets.push_back(kern->addTaskAndEnvelope(t, ""));
                ttrace.attachToTask(*t);

                tasks.push_back(t);
            }

            SIMUL.initSingleRun();
            SIMUL.run_to(1);
            SIMUL.endSingleRun();
            return 0;
        }
        else if(TEST_NO == 7) {
            vector<CPU_BL*> cpus;
            PeriodicTask* task[5]; // to be cleared after each test
            CPU_BL* cpu_task[5]; // to be cleared after each test
            vector<CBServerCallingEMRTKernel*> ets;

            int wcets[] = { 63, 63, 63, 63, 30 };
            int i;
            for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
                task_name = "T7_task" + std::to_string(j);
                cout << "Creating task: " << task_name;
                PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
                char instr[60] = "";
                sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
                t->insertCode(instr);
                ets.push_back(kern->addTaskAndEnvelope(t, ""));

                task[j] = t;
            }

            /* Towards random workloads, but this time alg. first decides to
               schedule all tasks on littles, and then, instead of schedule the
               next one in bigs, it shall increase littles frequency so to make
               space to it too and save energy */

            SIMUL.initSingleRun();
            SIMUL.run_to(1);

            for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
                cpu_task[j] = dynamic_cast<CPU_BL*>(kern->getProcessor(ets.at(j)));
            }

            i = 0;
            PeriodicTask* t = task[i];
            CPU_BL* c = cpu_task[i];
            REQUIRE (t->getName() == "T7_task0");
            REQUIRE (c->getFrequency() == 500);
            REQUIRE (c->getIslandType() == IslandType::LITTLE);
            REQUIRE (isInRange(int(t->getWCET(c->getSpeed())), 415));
            printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

            i = 1;
            t = task[i];
            c = cpu_task[i];
            REQUIRE (t->getName() == "T7_task1");
            REQUIRE (c->getFrequency() == 500);
            REQUIRE (c->getIslandType() == IslandType::LITTLE);
            REQUIRE (isInRange(int(t->getWCET(c->getSpeed())), 415));
            printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

            i = 2;
            t = task[i];
            c = cpu_task[i];
            REQUIRE (t->getName() == "T7_task2");
            REQUIRE (c->getFrequency() == 500);
            REQUIRE (c->getIslandType() == IslandType::LITTLE);
            REQUIRE (isInRange(int(t->getWCET(c->getSpeed())), 415));
            printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

            i = 3;
            t = task[i];
            c = cpu_task[i];
            REQUIRE (t->getName() == "T7_task3");
            REQUIRE (c->getFrequency() == 500);
            REQUIRE (c->getIslandType() == IslandType::LITTLE);
            REQUIRE (isInRange(int(t->getWCET(c->getSpeed())), 415));
            printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

            i = 4;
            t = task[i];
            c = cpu_task[i];
            REQUIRE (t->getName() == "T7_task4");
            REQUIRE (c->getFrequency() == 700);
            REQUIRE (c->getIslandType() == IslandType::BIG);
            REQUIRE (isInRange(int(t->getWCET(c->getSpeed())), 75));
            printf("aaa %s scheduled on %s freq %lu with wcet %f\n", t->getName().c_str(), c->toString().c_str(), c->getFrequency(), t->getWCET(c->getSpeed()));

            //SIMUL.run_to(1000);
            SIMUL.endSingleRun();
            return 0;
        }
        if (TEST_NO == 8) {
            int wcets[] = { 181, 419, 261, 163, 65, 8, 61, 170, 273 };
            for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
                task_name = "T8_task" + std::to_string(j);
                cout << "Creating task: " << task_name;
                PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
                char instr[60] = "";
                sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
                t->insertCode(instr);
                ets.push_back(kern->addTaskAndEnvelope(t, ""));
                ttrace.attachToTask(*t);
                tasks.push_back(t);
            }
            // towards random workloads...
        }
        /*
        // Useless test. Keeping it just to remember how to use delay, unif, delta.
        if (TEST_NO == 9) {
            // random variables
            int taskNO = 3;
            int task_period = 500;
            int mode = 0;
            int seed = 98;
            srand(seed); // random nums are expected to be the same in all simulations

            for (int j = 0; j < taskNO; j++) {
                task_name = "T9_task" + std::to_string(j);
                cout << "Creating task: " << task_name;
                PeriodicTask* t = new PeriodicTask(task_period, task_period, 0, task_name);
                char instr[60] = "";
                // srand(time(NULL)) or srand(seed)
                switch (mode) {
                case 0:
                  sprintf(instr, "delay(unif(1, %d));", task_period);
                  break;
                case 1:
                  sprintf(instr, "delay(delta(%d));", task_period * rand() / (RAND_MAX + 1u));
                  break;
                case 2:
                  sprintf(instr, "fixed(%d, %s);", task_period * rand() / (RAND_MAX + 1u), workload.c_str());
                  break;
                default: break;
                }
                t->insertCode(instr);
                tasks.push_back(t);
                ttrace.attachToTask(*t);
                ets.push_back(kern->addTaskAndEnvelope(t, ""));
            }

            SIMUL.initSingleRun();
            SIMUL.run_to(1000);
            SIMUL.endSingleRun();

            schedulers.clear();
            kernels.clear();

            RRScheduler *rrsched = new RRScheduler(100); // 100 is result of sysctl kernel.sched_rr_timeslice_ms on my machine, L5.0.2
            //EnergyMRTKernel *kern = new EnergyMRTKernel(rrsched, island_bl_big, island_bl_little, "Round Robin");
            kernels.push_back(kern);
            for (AbsRTTask* t : tasks)
                ets.push_back(kern->addTaskAndEnvelope(t, ""));

            SIMUL.initSingleRun();
            SIMUL.run_to(1000);
            SIMUL.endSingleRun();
            return 0;

            // random workloads...delay(unif,PDF).
            // todo above code not tested
        }*/
        if (TEST_NO == 9) {
            // 100 and 101 will end up in LITTLEs, 500 in BIGs, 101 will end up in big.
            // 100 will finish before, making the task in big (101, the last one in the list) migrate to little

            // needs migrations?
            int wcets[] = { 101,101,101,8,   200,500,500,500,   101, 1  }; // 9 tasks
            for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
                task_name = "T" + to_string(TEST_NO) + "_task" + to_string(j);
                cout << "Creating task: " << task_name;
                PeriodicTask* t = new PeriodicTask(500, 500, 0, task_name);
                char instr[60] = "";
                sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
                t->insertCode(instr);
                ets.push_back(kern->addTaskAndEnvelope(t, ""));
                ttrace.attachToTask(*t);
                tasks.push_back(t);
            }
            EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kernels[0]);
            MultiCoresScheds* queues = k->getEnergyMultiCoresScheds();

            k->addForcedDispatch(ets[0], cpus_little[0], 6);
            k->addForcedDispatch(ets[1], cpus_little[1], 6);
            k->addForcedDispatch(ets[2], cpus_little[2], 6);
            k->addForcedDispatch(ets[3], cpus_little[3], 6);

            k->addForcedDispatch(ets[4], cpus_big[0], 18);
            k->addForcedDispatch(ets[5], cpus_big[1], 18);
            k->addForcedDispatch(ets[6], cpus_big[2], 18);
            k->addForcedDispatch(ets[7], cpus_big[3], 18);

            k->addForcedDispatch(ets[8], cpus_big[3], 18);
            k->addForcedDispatch(ets[9], cpus_big[3], 18);

            SIMUL.initSingleRun();
            SIMUL.run_to(1);
            k->printState(true);

            SIMUL.run_to(34);
            k->printState(true);

            // at t=35 task WCET 8 finishes

            SIMUL.run_to(36);
            k->printState(true);
            for (CBServerCallingEMRTKernel* s : ets)
                assert (s->getStatus() == ServerStatus::EXECUTING || s->getStatus() == ServerStatus::RECHARGING || s->getStatus() == ServerStatus::READY);
            for (CPU* c: k->getProcessors())
                REQUIRE (queues->getUtilization_active(c) == 0.0);

            REQUIRE(k->getProcessor(ets[0]) == cpus_little[0]);
            REQUIRE(k->getProcessor(ets[1]) == cpus_little[1]);
            REQUIRE(k->getProcessor(ets[2]) == cpus_little[2]);
            //REQUIRE(k->getProcessor(ets[3]) == cpus_little[3]); has ended already

            REQUIRE(k->getProcessor(ets[4]) == cpus_big[0]);
            REQUIRE(k->getProcessor(ets[5]) == cpus_big[1]);
            REQUIRE(k->getProcessor(ets[6]) == cpus_big[2]);
            REQUIRE(k->getProcessor(ets[7]) == cpus_big[3]);

            // task8 comes in place of task3
            REQUIRE(k->getProcessor(ets[8]) == cpus_little[3]);
            cout << ets[8]->getBudget() << endl;
            REQUIRE (ets[8]->getBudget() > 400);

            SIMUL.run_to(199);
            k->printState(true, true);
            for (CBServerCallingEMRTKernel* s : ets)
                if (s == ets[4])
                    REQUIRE (s->getStatus() == ServerStatus::RELEASING);
                else
                    assert (s->getStatus() == ServerStatus::EXECUTING || s->getStatus() == ServerStatus::RECHARGING);
            for (CPU* c: k->getProcessors())
                if (c == cpus_big[0] && ets[4]->getStatus() == ServerStatus::RELEASING)
                    REQUIRE (isInRange(queues->getUtilization_active(c), 200.0 / c->getSpeed() / 500.0));
                else
                    REQUIRE (queues->getUtilization_active(c) == 0.0);

            REQUIRE(k->getProcessor(ets[0]) == cpus_little[0]);
            REQUIRE(k->getProcessor(ets[1]) == cpus_little[1]);
            REQUIRE(k->getProcessor(ets[2]) == cpus_little[2]);
            //REQUIRE(k->getProcessor(ets[3]) == cpus_little[3]); has ended already
            cout << "eccomi1 " << ets[3]->toString() << ", " << ets[3]->getEndOfVirtualTimeEvent() << endl;

            //REQUIRE(k->getProcessor(ets[4]) == cpus_big[0]); has ended already
            REQUIRE(k->getProcessor(ets[5]) == cpus_big[1]);
            REQUIRE(k->getProcessor(ets[6]) == cpus_big[2]);
            REQUIRE(k->getProcessor(ets[7]) == cpus_big[3]);

            // task9 comes in place of task4
            REQUIRE(k->getProcessor(ets[9]) == cpus_big[0]);
            SIMUL.run_to(202);
            for (CBServerCallingEMRTKernel* s : ets)
                if (s == ets[4])
                    REQUIRE (s->getStatus() == ServerStatus::RELEASING);
                else
                    assert (s->getStatus() == ServerStatus::EXECUTING || s->getStatus() == ServerStatus::RECHARGING);
            for (CPU* c: k->getProcessors())
                if (c == cpus_big[0]) {
                    c->setWorkload(workload);
                    REQUIRE (isInRange(queues->getUtilization_active(c), 200.0 / c->getSpeed() / 500.0));
                    REQUIRE (ets[4]->getIdleEvent() == 500);
                    c->setWorkload("idle");
                }
                else
                    REQUIRE (queues->getUtilization_active(c) == 0.0);

            SIMUL.run_to(499);
            cout << endl << "t=499. all cores should be empty" << endl << endl;
            k->printState(true);
            for (CBServerCallingEMRTKernel* s : ets)
                if (s == ets[4] || s == ets[5] || s == ets[6] || s == ets[7])
                    REQUIRE (s->getStatus() == ServerStatus::RELEASING);
                else
                    assert (s->getStatus() == ServerStatus::EXECUTING || s->getStatus() == ServerStatus::RECHARGING || s->getStatus() == ServerStatus::IDLE);
            for (CPU* c: k->getProcessors()) {
                c->setWorkload(workload);
                if (c == cpus_big[0])
                    REQUIRE (isInRange(queues->getUtilization_active(c), 200.0 / c->getSpeed(18) / 500.0));
                else if (dynamic_cast<CPU_BL*>(c)->getIslandType() == IslandType::BIG)
                    REQUIRE (isInRange(queues->getUtilization_active(c), 500.0 / c->getSpeed(18) / 500.0));
                else
                    REQUIRE (queues->getUtilization_active(c) == 0.0);
                c->setWorkload("idle");
            }

            SIMUL.run_to(500);
            for (CPU* c: k->getProcessors())
                REQUIRE (queues->getUtilization_active(c) == 0.0);
            k->printState(true);
            
            REQUIRE (k->getProcessor(tasks[0])->getIslandType() == cpus_little[0]->getIslandType());
            REQUIRE (k->getProcessor(tasks[1])->getIslandType() == cpus_little[1]->getIslandType());
            REQUIRE (k->getProcessor(tasks[2])->getIslandType() == cpus_little[2]->getIslandType());
            REQUIRE (k->getProcessor(tasks[3])->getIslandType() == cpus_little[3]->getIslandType()); // has ended already

            REQUIRE (k->getProcessor(tasks[4])->getIslandType() == cpus_big[0]->getIslandType()); // has ended already
            REQUIRE (k->getProcessor(tasks[5])->getIslandType() == cpus_big[1]->getIslandType());
            REQUIRE (k->getProcessor(tasks[6])->getIslandType() == cpus_big[2]->getIslandType());
            REQUIRE (k->getProcessor(tasks[7])->getIslandType() == cpus_big[3]->getIslandType());

            SIMUL.run_to(536);

            REQUIRE (k->getProcessor(tasks[8]) == cpus_little[3]);

            SIMUL.run_to(941);

            REQUIRE (k->getProcessor(tasks[9]) == cpus_little[0]);

            // SIMUL.run_to(1000);
            SIMUL.endSingleRun();

            cout << "-----------------------" << endl;
            cout << "Simulation has finished" << endl;
            return 0;
        }
        if (TEST_NO == 10) {
            /**
                At time 0, you have a task on a CPU, which is under a context switch. Say it lasts 30 ticks.
                Then, another task, more important, arrives at time 6. Would this last task begin its
                context switch at time 30, thus being scheduled at time 30+30=60?
                Experiment requires EDF scheduler.
              */
            int wcets[] = { 30,  30  };
            int deadl[] = { 500, 400 };
            int activ[] = { 0,   6   };
            for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
                task_name = "T" + to_string(TEST_NO) + "_task" + to_string(j);
                cout << "Creating task: " << task_name;
                PeriodicTask* t = new PeriodicTask(deadl[j], deadl[j], 0, task_name);
                char instr[60] = "";
                sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
                t->insertCode(instr);
                ets.push_back(kern->addTaskAndEnvelope(t, ""));
                ttrace.attachToTask(*t);
                tasks.push_back(t);
            }
            EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
            MultiCoresScheds* queues = k->getEnergyMultiCoresScheds();

            k->setContextSwitchDelay(Tick(8));
            k->addForcedDispatch(ets[0], cpus_big[0], 18);
            k->addForcedDispatch(ets[1], cpus_big[0], 18);

            SIMUL.initSingleRun();
            dynamic_cast<Task*>(tasks[1])->activate(Tick(activ[1]));
            SIMUL.run_to(17);

            REQUIRE (k->getProcessor(ets[1]) == cpus_big[0]);
            REQUIRE (k->getProcessorReady(tasks[0]) == cpus_big[0]);

            SIMUL.run_to(400);
            cout << queues->getUtilization_active(cpus_big[0]) <<endl;
            
            SIMUL.run_to(500);
            REQUIRE (queues->getUtilization_active(cpus_big[0]) == 0.0);
                
            SIMUL.run_to(1000);
            SIMUL.endSingleRun();
            cout << "-----------" << endl;
            cout << "Simulation finished" << endl;
            return 0;
        }
        if (TEST_NO == 12) {
	    // does RR work as expected with EMRTK?

            schedulers.clear();
            kernels.clear();

            RRScheduler *rrsched = new RRScheduler(100); // 100 is result of sysctl kernel.sched_rr_timeslice_ms on my machine, L5.0.2
            rrsched->disable();
            rrsched->setName("RRScheduler for arrival queue");
            for (int i = 0; i < 8; i++) {
                delete schedulers[i];
                Scheduler *s = new RRScheduler(100);
                s->setName("RRScheduler #" + to_string(i));
                schedulers.push_back(s);
            }
            EnergyMRTKernel *kern = new EnergyMRTKernel(schedulers, rrsched, island_bl_big, island_bl_little, "Round Robin");
            kernels.push_back(kern);

            int wcets[] = { 30, 30, 30  };
            int deadl[] = { 500, 500, 500 };
            for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
                task_name = "T" + to_string(TEST_NO) + "_task" + to_string(j);
                cout << "Creating task: " << task_name;
                PeriodicTask* t = new PeriodicTask(deadl[j], deadl[j], 0, task_name);
                char instr[60] = "";
                sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
                t->insertCode(instr);
                kern->addTask(*t, "");
                ttrace.attachToTask(*t);
                tasks.push_back(t);
            }
            kern->addForcedDispatch(ets[0], cpus_little[0], 6);
            kern->addForcedDispatch(ets[1], cpus_little[0], 6);

            SIMUL.run(1001);
            return 0;
        }
        if (TEST_NO == 13) {
            /**
                Demostrating what happens when a task is killed.
                It will come into play again in its next period and hopefully it can 
                be scheduled.
              */
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
                CBServerCallingEMRTKernel* et = kern->addTaskAndEnvelope(t, "");
                ttrace.attachToTask(*t);
                tasks.push_back(t);

                t->killOnMiss(true);
            }
            EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
            k->addForcedDispatch(ets[0], cpus_big[0], 18);
            k->addForcedDispatch(ets[1], cpus_big[1], 18);
            k->addForcedDispatch(ets[2], cpus_big[2], 18);
            k->addForcedDispatch(ets[3], cpus_big[3], 18);

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
                REQUIRE(t->getState() == TSK_IDLE);
            }
            SIMUL.run_to(501);
            REQUIRE(dynamic_cast<CPU_BL*>(k->getProcessor(tasks[0]))->getIslandType() == IslandType::BIG);
            REQUIRE(dynamic_cast<CPU_BL*>(k->getProcessor(tasks[1]))->getIslandType() == IslandType::BIG);
            REQUIRE(dynamic_cast<CPU_BL*>(k->getProcessor(tasks[2]))->getIslandType() == IslandType::BIG);
            REQUIRE(dynamic_cast<CPU_BL*>(k->getProcessor(tasks[3]))->getIslandType() == IslandType::BIG);
            REQUIRE(dynamic_cast<CPU_BL*>(k->getProcessorReady(tasks[4]))->getIslandType() == IslandType::BIG);

            SIMUL.run_to(1000);
            SIMUL.endSingleRun();
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            cout << "Simulation finished" << endl;
            return 0;
        }
        if (TEST_NO == 14) {
            /**
                What happens if task is not schedulable?

                @Mr. Cucinotta, Mr. Marinoni:
                This example also demonstrates that a task is either on the queue/scheduler
                of arrived tasks or in the one of the core selected for its dispatching.
                Moreover, if a task cannot be scheduled in its current, it has another chance
                on the next one.
              */
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
                CBServerCallingEMRTKernel* et = kern->addTaskAndEnvelope(t, "");
                ttrace.attachToTask(*t);
                tasks.push_back(t);

                t->killOnMiss(false);
            }

            SIMUL.initSingleRun();

            SIMUL.run_to(1);
            EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
            REQUIRE(dynamic_cast<CPU_BL*>(k->getProcessor(tasks[0])) == cpus_big[0]);
            REQUIRE(dynamic_cast<CPU_BL*>(k->getProcessorReady(tasks[1])) == cpus_big[0]);
            REQUIRE(k->getScheduler()->isInQueue(tasks[0]) == false); // an executing task
            REQUIRE(k->getScheduler()->isInQueue(tasks[1]) == false); // a ready task
            REQUIRE(k->getScheduler()->isInQueue(tasks[2]) == false); // a discarded task
            REQUIRE(k->getScheduler()->isInQueue(tasks[3]) == false); // a discarded task

            REQUIRE(k->getEnergyMultiCoresScheds()->getScheduler(k->getProcessor(tasks[0]))->isInQueue(tasks[0]) == true);
            REQUIRE(k->getEnergyMultiCoresScheds()->getScheduler(k->getProcessor(tasks[0]))->isInQueue(tasks[1]) == true);
            REQUIRE(k->getEnergyMultiCoresScheds()->isInAnyQueue(tasks[2]) == NULL);
            REQUIRE(k->getEnergyMultiCoresScheds()->isInAnyQueue(tasks[3]) == NULL);

            SIMUL.run_to(501);
            // same tests as above
            REQUIRE(dynamic_cast<CPU_BL*>(k->getProcessor(tasks[0])) == cpus_big[0]);
            REQUIRE(dynamic_cast<CPU_BL*>(k->getProcessorReady(tasks[1])) == cpus_big[0]);
            REQUIRE(k->getScheduler()->isInQueue(tasks[0]) == false); // an executing task
            REQUIRE(k->getScheduler()->isInQueue(tasks[1]) == false); // a ready task
            REQUIRE(k->getScheduler()->isInQueue(tasks[2]) == false); // a discarded task
            REQUIRE(k->getScheduler()->isInQueue(tasks[3]) == false); // a discarded task

            REQUIRE(k->getEnergyMultiCoresScheds()->getScheduler(k->getProcessor(tasks[0]))->isInQueue(tasks[0]) == true);
            REQUIRE(k->getEnergyMultiCoresScheds()->getScheduler(k->getProcessor(tasks[0]))->isInQueue(tasks[1]) == true);
            REQUIRE(k->getEnergyMultiCoresScheds()->isInAnyQueue(tasks[2]) == NULL);
            REQUIRE(k->getEnergyMultiCoresScheds()->isInAnyQueue(tasks[3]) == NULL);

            SIMUL.run_to(1000);
            SIMUL.endSingleRun();
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            cout << "Simulation finished" << endl;
            return 0;
        }
        if (TEST_NO == 15) {
            /**
                Towards servers...y màs allà!
             */
            PeriodicTask *t2 = new PeriodicTask(15, 15 , 0, "TaskA"); 
            t2->insertCode("fixed(4,bzip2);");
            t2->setAbort(false);
            ttrace.attachToTask(*t2);

            CBServerCallingEMRTKernel *serv = new CBServerCallingEMRTKernel(4, 15, 15, "hard",  "server1", "FIFOSched");
            serv->setKernel(kern);
            serv->addTask(*t2);
            tasks.push_back(serv);
            CBServerCallingEMRTKernel* et = kern->addTaskAndEnvelope(serv, "");

            EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
            k->addForcedDispatch(serv, cpus_big[0], 18, 999);

            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            cout << "Running simulation!" << endl;
            SIMUL.initSingleRun();

            SIMUL.run_to(1);
            REQUIRE (dynamic_cast<CPU_BL*>(k->getProcessor(tasks[0]))->getIslandType() == IslandType::BIG);

            SIMUL.run_to(5);
            cout << " dasdsad " << dynamic_cast<Task*>(t2)->getStateString() << endl;
            REQUIRE (dynamic_cast<Task*>(t2)->getState() == TSK_IDLE);

            SIMUL.run_to(16);
            cout << " dasdsad " << dynamic_cast<Task*>(t2)->getStateString() << endl;
            REQUIRE (dynamic_cast<CPU_BL*>(k->getProcessor(tasks[0]))->getIslandType() == IslandType::BIG);

            SIMUL.run_to(21);
            cout << " dasdsad " << dynamic_cast<Task*>(t2)->getStateString() << endl;
            REQUIRE (dynamic_cast<Task*>(t2)->getState() == TSK_IDLE);

            SIMUL.run_to(26);

            SIMUL.endSingleRun();
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            cout << "Simulation finished" << endl;

            return 0;
        }
        if (TEST_NO == 16) {
            /**
                Towards servers. Reproducing Mr.Cucinotta's example.
                A server with (Q=2,T=10) and a task arriving at 0 and ending at 2, period 10.
                Its active utilization is kept until 10.
             */
            PeriodicTask *t2 = new PeriodicTask(10, 10 , 0, "TaskA"); 
            t2->insertCode("fixed(2,bzip2);");
            t2->setAbort(false);
            ttrace.attachToTask(*t2);

            CBServerCallingEMRTKernel *serv = new CBServerCallingEMRTKernel(2, 10, 10, "hard",  "server1", "FIFOSched");
            serv->addTask(*t2);
            tasks.push_back(serv);
            CBServerCallingEMRTKernel* et = kern->addTaskAndEnvelope(serv, "");

            EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
            k->addForcedDispatch(ets[0], cpus_big[0], 18, 999);

            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            cout << "Running simulation!" << endl;
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

            SIMUL.endSingleRun();
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            cout << "Simulation finished" << endl;

            return 0;
        }
        if (TEST_NO == 17) {
            /**
                The objective is to understand if utilizations are 
                computed correctly.

                Some tasks are placed on all available cores.
                Let's see where the CBS is placed according to utilizations.
              */
            cpus_little[1]->toggleDisabled();
            cpus_little[2]->toggleDisabled();
            cpus_little[3]->toggleDisabled();
            cpus_big[2]->toggleDisabled();
            cpus_big[3]->toggleDisabled();

            cpus_big[0]->setOPP(18);
            cpus_big[1]->setOPP(18);

            // Already-there tasks
            PeriodicTask *t0_little = new PeriodicTask(20, 20 , 0, "Task1_little0"); 
            t0_little->insertCode("fixed(5,bzip2);");
            t0_little->setAbort(false);
            ttrace.attachToTask(*t0_little);
            pstrace.attachToTask(*t0_little);
            tasks.push_back(t0_little);
            CBServerCallingEMRTKernel* et_t0_little = kern->addTaskAndEnvelope(t0_little, "");

            PeriodicTask *t0_big0 = new PeriodicTask(10, 10 , 0, "Task2_Big0"); 
            t0_big0->insertCode("fixed(5,bzip2);");
            t0_big0->setAbort(false);
            ttrace.attachToTask(*t0_big0);
            pstrace.attachToTask(*t0_big0);
            tasks.push_back(t0_big0);
            CBServerCallingEMRTKernel* et_t0_big0 = kern->addTaskAndEnvelope(t0_big0, "");

            PeriodicTask *t0_big1 = new PeriodicTask(10, 10 , 0, "Task3_Big1"); 
            t0_big1->insertCode("fixed(5,bzip2);");
            t0_big1->setAbort(false);
            ttrace.attachToTask(*t0_big1);
            pstrace.attachToTask(*t0_big1);
            tasks.push_back(t0_big1);
            CBServerCallingEMRTKernel* et_t0_big1 = kern->addTaskAndEnvelope(t0_big1, "");
            


            // CBS server tasks
            PeriodicTask *t2 = new PeriodicTask(10, 10 , 0, "TaskOnServer"); 
            t2->insertCode("fixed(2,bzip2);"); // => its releasing_idle will be at t=4
            t2->setAbort(false);
            ttrace.attachToTask(*t2);
            pstrace.attachToTask(*t2);

            CBServerCallingEMRTKernel *serv = new CBServerCallingEMRTKernel(2, 10, 10, "hard",  "server1", "FIFOSched");
            serv->addTask(*t2);
            tasks.push_back(serv);
            CBServerCallingEMRTKernel* et_serv = kern->addTaskAndEnvelope(serv, "");



            EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
            k->addForcedDispatch(ets[0], cpus_little[0], 11, 1); // note: normally they wouldn't fit this way
            k->addForcedDispatch(ets[1], cpus_big[0], 18, 1);
            k->addForcedDispatch(ets[2], cpus_big[1], 18, 1);
            // server's free to go wherever.

            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            cout << "Running simulation!" << endl;
            SIMUL.initSingleRun();
            
            SIMUL.run_to(50);

            SIMUL.endSingleRun();
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            cout << "Simulation finished" << endl;

            return 0;
        }
        if (TEST_NO == 18) {
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
            ttrace.attachToTask(*t0_little);
            jtrace.attachToTask(*t0_little);
            pstrace.attachToTask(*t0_little);
            tasks.push_back(t0_little);
            CBServerCallingEMRTKernel* et = kern->addTaskAndEnvelope(t0_little, "");

            NonPeriodicTask *t0_big0 = new NonPeriodicTask(10, 10, 0, "Task2_Big0"); 
            t0_big0->insertCode("fixed(5,bzip2);");
            t0_big0->setAbort(false);
            ttrace.attachToTask(*t0_big0);
            jtrace.attachToTask(*t0_big0);
            pstrace.attachToTask(*t0_big0);
            tasks.push_back(t0_big0);
            CBServerCallingEMRTKernel* et_t0_big0 = kern->addTaskAndEnvelope(t0_big0, "");

            PeriodicTask *t0_big1 = new PeriodicTask(30, 30 , 0, "Task3_Big1"); 
            t0_big1->insertCode("fixed(15,bzip2);");
            t0_big1->setAbort(false);
            ttrace.attachToTask(*t0_big1);
            jtrace.attachToTask(*t0_big1);
            pstrace.attachToTask(*t0_big1);
            tasks.push_back(t0_big1);
            CBServerCallingEMRTKernel* et_t0_big1 = kern->addTaskAndEnvelope(t0_big1, "");

            PeriodicTask *t1_big1 = new PeriodicTask(31, 31 , 0, "TaskReady_Big1"); 
            t1_big1->insertCode("fixed(1,bzip2);");
            t1_big1->setAbort(false);
            ttrace.attachToTask(*t1_big1);
            jtrace.attachToTask(*t1_big1);
            pstrace.attachToTask(*t0_big1);
            tasks.push_back(t1_big1);
            CBServerCallingEMRTKernel* et_t1_big1 = kern->addTaskAndEnvelope(t1_big1, "");
            


            // CBS server tasks
            NonPeriodicTask *tos = new NonPeriodicTask(10, 10 , 0, "TaskOnServer"); 
            tos->insertCode("fixed(2,bzip2);"); // => its releasing_idle will be at t=4
            tos->setAbort(false);
            ttrace.attachToTask(*tos);
            jtrace.attachToTask(*tos);
            pstrace.attachToTask(*tos);

            NonPeriodicTask *tos2 = new NonPeriodicTask(10, 10 , 0, "AfterTaskOnServer"); 
            tos2->insertCode("fixed(2,bzip2);");
            tos2->setAbort(false);
            ttrace.attachToTask(*tos2);
            jtrace.attachToTask(*tos2);
            pstrace.attachToTask(*tos2);

            CBServerCallingEMRTKernel *serv = new CBServerCallingEMRTKernel(2, 10, 10, "hard",  "server1", "FIFOSched");
            serv->addTask(*tos);
            serv->addTask(*tos2);
            tasks.push_back(serv);
            CBServerCallingEMRTKernel* et_serv = kern->addTaskAndEnvelope(serv, "");



            // Tasks coming freely: the dynamic situations
            PeriodicTask *t5 = new PeriodicTask(30, 30 , 0, "TaskDuring"); 
            t5->insertCode("fixed(2,bzip2);");
            t5->setAbort(false);
            ttrace.attachToTask(*t5);
            tasks.push_back(t5);
            jtrace.attachToTask(*t5);
            pstrace.attachToTask(*t5);
            CBServerCallingEMRTKernel* et_t5 = kern->addTaskAndEnvelope(t5, "");

            PeriodicTask *t3 = new PeriodicTask(30, 30 , 0, "TaskBefore"); 
            t3->insertCode("fixed(1,bzip2);");
            t3->setAbort(false);
            ttrace.attachToTask(*t3);
            jtrace.attachToTask(*t3);
            pstrace.attachToTask(*t3);
            tasks.push_back(t3);
            CBServerCallingEMRTKernel* et_t3 = kern->addTaskAndEnvelope(t3, "");

            PeriodicTask *t4 = new PeriodicTask(30, 30 , 0, "TaskAfter"); 
            t4->insertCode("fixed(1,bzip2);");
            t4->setAbort(false);
            ttrace.attachToTask(*t4);
            jtrace.attachToTask(*t4);
            pstrace.attachToTask(*t4);
            tasks.push_back(t4);
            CBServerCallingEMRTKernel* et_t4 = kern->addTaskAndEnvelope(t4, "");
            


            EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
            k->addForcedDispatch(ets[0], cpus_little[0], 12, 1); // note: normally it wouldn't fit this way
            k->addForcedDispatch(ets[1], cpus_big[0], 18, 1);
            k->addForcedDispatch(ets[2], cpus_big[1], 18, 1);
            k->addForcedDispatch(t1_big1,  cpus_big[1], 18, 1);  // it should preempt the executing task on big 0
            // server's free to go wherever.

            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            cout << "Running simulation!" << endl;
            SIMUL.initSingleRun();

            double init_util = 0.0;

            t5->activate(Tick(activ[0]));
            t3->activate(Tick(activ[1]));
            t4->activate(Tick(activ[2]));
            tos2->activate(Tick(11));

            SIMUL.run_to(1); // init setup
            REQUIRE (k->getProcessorRunning(t0_little) == cpus_little[0]);
            REQUIRE (k->getProcessorRunning(t0_big0) == cpus_big[0]);
            REQUIRE (k->getProcessorReady(serv) == cpus_big[0]);
            REQUIRE (k->getProcessorRunning(t0_big1) == cpus_big[1]);
            REQUIRE (k->getProcessorReady(t1_big1) == cpus_big[1]);
            // REQUIRE (double(t0_big0->deadEvt.getTime()) == 5.0); todo

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

            SIMUL.endSingleRun();
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            cout << "Simulation finished" << endl;

            return 0;
        }





        EnergyMRTKernel::EMRTK_CBS_ENVELOPING_MIGRATE_AFTER_VTIME_END     = 1;
        EnergyMRTKernel::EMRTK_CBS_MIGRATE_AFTER_END                      = 0;

        // CBS server enveloping Periodic Tasks examples.
        // Require CBS servers enveloping periodic tasks and EMRTK_CBS_ENVELOPING_MIGRATE_AFTER_VTIME_END
        if (TEST_NO == 19) {
            /**
                           vt end
                !=======_____|_______!______...

                1st example. End of virtual time arrives. Kernel tries to pull
                tasks into core but it fails (tasks too big).

                example below:

                            150
                160 450 450 350
                b0  b1  b2  b3   at time 0

                not understood
              */

            string  names[] = { "B0_killed", "B1", "B2", "B3_running", "B3_ready" };
            int     wcets[] = { 160, 450, 450, 350, 150 };
            int     deads[] = { 200, 500, 500, 500, 500 };
            vector<CBServerCallingEMRTKernel*> ets;
            for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
                task_name = "T" + to_string(TEST_NO) + "_task_BIG_" + names[j];
                cout << "Creating task: " << task_name;
                PeriodicTask* t = new PeriodicTask(deads[j], deads[j], 0, task_name);
                char instr[60] = "";
                sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
                cout << instr << endl;
                t->insertCode(instr);
                ets.push_back(kern->addTaskAndEnvelope(t, ""));
                ttrace.attachToTask(*t);
                tasks.push_back(t);
            }
            EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
            k->addForcedDispatch(ets[0], cpus_big[0], 18);
            k->addForcedDispatch(ets[1], cpus_big[1], 18);
            k->addForcedDispatch(ets[2], cpus_big[2], 18);
            k->addForcedDispatch(ets[3], cpus_big[3], 18);
            k->addForcedDispatch(ets[4], cpus_big[3], 18); // ready

            for (CPU_BL* c : cpus_little)
                c->toggleDisabled();

            SIMUL.initSingleRun();
            
            SIMUL.run_to(150); // kill task on big0, task ready on big0 gets running
            ets[0]->killInstance();
            SIMUL.sim_step(); // t=150, but all events have been processed
            cout << "u active big 0 " << k->getUtilization_active(cpus_big[0]) << endl;
            cout << "idle evt " << ets[0]->getIdleEvent() << endl;
            cout << "server status " << ets[0]->getStatusString() << endl;
            REQUIRE (k->getUtilization_active(cpus_big[0]) > 0.7); // shall be 0.8
            REQUIRE ( (double) ets[0]->getIdleEvent() >= 185);
            REQUIRE (ets[0]->getStatus() == ServerStatus::RELEASING);

            SIMUL.run_to(151);
            k->printState(true);
            REQUIRE (k->getRunningTask(cpus_big[0]) == NULL); // no migration
            REQUIRE (k->getRunningTask(cpus_big[3]) != NULL);
            REQUIRE (k->getReadyTasks(cpus_big[3]).size() == 1);

            SIMUL.run_to(189); // t=168, cannot migrate because core's already busy
            REQUIRE (k->getRunningTask(cpus_big[0]) == ets[5]);
            REQUIRE (k->getUtilization_active(cpus_big[0]) == 0.0);

            SIMUL.run_to(500); // all tasks are over, usual dispatch
            SIMUL.sim_step();
            for (CBServerCallingEMRTKernel* e : ets)
                REQUIRE (k->getProcessor(e) != NULL);

            SIMUL.endSingleRun();

            cout << "--------------" << endl;
            cout << "Simulation finished" << endl;
            for (AbsRTTask *t : tasks)
                delete t;
            for (CBServerCallingEMRTKernel* cbs : ets)
                delete cbs;
            delete k;
            return 0;
        }
        if (TEST_NO == 20) {
            /**
                2nd example. End of virtual time of task t. Kernel pulls (migrates) a task into
                the ending core. It has WCET > DL_t (t is now idle). When t arrives again, it can
                be scheduled on its previous core.
              */
            string  names[] = { "B0_killed", "B1", "B2", "B3_running", "B3_ready" };
            int     wcets[] = { 160, 450, 450, 400, 60 };
            int     deads[] = { 200, 500, 500, 500, 500 };
            vector<CBServerCallingEMRTKernel*> ets;
            for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
                task_name = "T" + to_string(TEST_NO) + "_task_BIG_" + names[j];
                cout << "Creating task: " << task_name;
                PeriodicTask* t = new PeriodicTask(deads[j], deads[j], 0, task_name);
                char instr[60] = "";
                sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
                cout << instr << endl;
                t->insertCode(instr);
                ets.push_back(kern->addTaskAndEnvelope(t, ""));
                ttrace.attachToTask(*t);
                tasks.push_back(t);
            }
            EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
            k->addForcedDispatch(ets[0], cpus_big[0], 18);
            k->addForcedDispatch(ets[1], cpus_big[1], 18);
            k->addForcedDispatch(ets[2], cpus_big[2], 18);
            k->addForcedDispatch(ets[3], cpus_big[3], 18);
            k->addForcedDispatch(ets[4], cpus_big[3], 18); // ready

            for (CPU_BL* c : cpus_little)
                c->toggleDisabled();

            SIMUL.initSingleRun();
            
            SIMUL.run_to(150); // kill task on big0, task ready on big0 gets running
            cout << k->getScheduler()->toString() << endl;
            // cout << dynamic_cast<RTKernel*>(ets[0]->getKernel())->getScheduler()->toString() << endl;
            ets[0]->killInstance();
            // cout << k->getScheduler()->toString() << endl;
            // cout << dynamic_cast<RTKernel*>(ets[0]->getKernel())->getScheduler()->toString() << endl;exit(0);
            SIMUL.sim_step(); // t=150, but all events have been processed
            cout << "u active big 0 " << k->getUtilization_active(cpus_big[0]) << endl;
            cout << "idle evt " << ets[0]->getIdleEvent() << endl;
            cout << "server status " << ets[0]->getStatusString() << endl;
            REQUIRE (k->getUtilization_active(cpus_big[0]) > 0.75); // shall be 0.8
            REQUIRE ( (double) ets[0]->getIdleEvent() >= 185);
            REQUIRE (ets[0]->getStatus() == ServerStatus::RELEASING);

            SIMUL.run_to(151);
            k->printState(true);
            // no migration, only schedule ready tasks
            REQUIRE (k->getRunningTask(cpus_big[0]) == NULL);
            REQUIRE (k->getReadyTasks(cpus_big[0]).size() == 0);

            SIMUL.run_to(189); // end vtime => migration
            REQUIRE (k->getRunningTask(cpus_big[0]) == ets[4]);
            REQUIRE (k->getUtilization_active(cpus_big[0]) == 0.0);

            cout << endl << "Scheduler state t=189:"<<endl;
            cout << k->getScheduler()->toString() << endl << endl;

            SIMUL.run_to(201);
            k->printState(true);
            REQUIRE (k->getRunningTask(cpus_big[0]) == ets[0]);
            REQUIRE (k->getReadyTasks(cpus_big[0]).at(0) == ets[4]);

            SIMUL.run_to(501); // all tasks are over, usual dispatch
            k->printState(true);
            for (CBServerCallingEMRTKernel *e : ets)
                REQUIRE (k->getProcessor(e) != NULL);            

            SIMUL.endSingleRun();

            cout << "--------------" << endl;
            cout << "Simulation finished" << endl;
            for (AbsRTTask *t : tasks)
                delete t;
            for (CBServerCallingEMRTKernel* cbs : ets)
                delete cbs;
            delete k;
            return 0;
        }
        if (TEST_NO == 21) {
            /**
                3rd example. End of virtual time of task t. Kernel pulls (migrates) a task into
                the ending core. It has WCET > DL_t (t is now idle). When t arrives again, it cannot
                be scheduled on its previous core, because U + U_t > 1 (EDF).
              */
            string  names[] = { "B0_killed", "B1", "B2", "B3_running", "B3_ready" };
            int     wcets[] = { 450, 450, 450, 400, 343 };
            int     deads[] = { 500, 500, 500, 500, 500 };
            vector<CBServerCallingEMRTKernel*> ets;
            for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
                task_name = "T" + to_string(TEST_NO) + "_task_BIG_" + names[j];
                cout << "Creating task: " << task_name;
                PeriodicTask* t = new PeriodicTask(deads[j], deads[j], 0, task_name);
                char instr[60] = "";
                sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
                cout << instr << endl;
                t->insertCode(instr);
                ets.push_back(kern->addTaskAndEnvelope(t, ""));
                ttrace.attachToTask(*t);
                tasks.push_back(t);
            }
            EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
            k->addForcedDispatch(ets[0], cpus_big[0], 18);
            k->addForcedDispatch(ets[1], cpus_big[1], 18);
            k->addForcedDispatch(ets[2], cpus_big[2], 18);
            k->addForcedDispatch(ets[3], cpus_big[3], 18);
            k->addForcedDispatch(ets[4], cpus_big[3], 18); // ready

            for (CPU_BL* c : cpus_little)
                c->toggleDisabled();

            SIMUL.initSingleRun();
            
            SIMUL.run_to(150); // kill task on big0, task ready on big0 gets running
            ets[0]->killInstance();
            SIMUL.sim_step(); // t=150, but all events have been processed
            cout << "u active big 0 " << k->getUtilization_active(cpus_big[0]) << endl;
            cout << "idle evt " << ets[0]->getIdleEvent() << endl;
            cout << "server status " << ets[0]->getStatusString() << endl;
            REQUIRE (k->getUtilization_active(cpus_big[0]) > 0.85); // shall be 0.9
            REQUIRE ( (double) ets[0]->getIdleEvent() > 165);
            REQUIRE (ets[0]->getStatus() == ServerStatus::RELEASING);

            SIMUL.run_to(151);
            k->printState(true);
            // no migration, only schedule ready tasks
            REQUIRE (k->getRunningTask(cpus_big[0]) == NULL);
            REQUIRE (k->getReadyTasks(cpus_big[0]).size() == 0);

            SIMUL.run_to(169); // end vtime => migration
            REQUIRE (k->getRunningTask(cpus_big[0]) == ets[4]);
            REQUIRE (k->getUtilization_active(cpus_big[0]) == 0.0);

            cout << endl << "Scheduler state t=189:"<<endl;
            cout << k->getScheduler()->toString() << endl << endl;

            SIMUL.run_to(201);
            k->printState(true);

            SIMUL.run_to(451);
            k->printState(true);
            REQUIRE (k->getRunningTask(cpus_big[0]) == ets[4]);
            REQUIRE (k->getRunningTask(cpus_big[1]) == NULL);
            REQUIRE (k->getRunningTask(cpus_big[2]) == NULL);
            REQUIRE (k->getRunningTask(cpus_big[3]) == NULL);

            SIMUL.run_to(501); // all tasks are over, usual dispatch
            k->printState(true);
            for (CPU* c : k->getProcessors(IslandType::BIG))
                REQUIRE (k->getRunningTask(c) != NULL);
            REQUIRE (k->getProcessor(ets[0]) != NULL);
            ets[0]->endRun(); // since it goes scheduled somewhere
            REQUIRE (k->isDispatchable(ets[0], cpus_big[0])); // because task WCET 343 has stolen its utilization

            SIMUL.endSingleRun();

            cout << "--------------" << endl;
            cout << "Simulation finished" << endl;
            for (AbsRTTask *t : tasks)
                delete t;
            for (CBServerCallingEMRTKernel* cbs : ets)
                delete cbs;
            delete k;
            return 0;
        }
        if (TEST_NO == 22) {
            /**
                4th example. Is it true that, when tasks finish or are killed, next ready tasks are
                scheduled or, if no ready task is available on the core, there is a migration?
              */
            int wcets[] = { 10, 450, 450, 450, 20, 10 };
            int deads[] = { 500, 500, 500, 500, 500, 500 };
            vector<CBServerCallingEMRTKernel*> ets;
            for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
                task_name = "T" + to_string(TEST_NO) + "_task_BIG_" + to_string(j);
                cout << "Creating task: " << task_name;
                PeriodicTask* t = new PeriodicTask(deads[j], deads[j], 0, task_name);
                char instr[60] = "";
                sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
                cout << instr << endl;
                t->insertCode(instr);
                ets.push_back(kern->addTaskAndEnvelope(t, ""));
                ttrace.attachToTask(*t);
                tasks.push_back(t);
            }
            EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
            k->addForcedDispatch(ets[0], cpus_big[0], 18);
            k->addForcedDispatch(ets[1], cpus_big[1], 18);
            k->addForcedDispatch(ets[2], cpus_big[2], 18);
            k->addForcedDispatch(ets[3], cpus_big[3], 18);
            k->addForcedDispatch(ets[4], cpus_big[0], 18); // ready
            k->addForcedDispatch(ets[5], cpus_big[3], 18); // ready

            for (CPU_BL* c : cpus_little)
                c->toggleDisabled();

            SIMUL.initSingleRun();
            
            SIMUL.run_to(11);
            cout << "==================" << endl;
            cout << "t=" << time() << endl;
            cout << "state of kernel:" << endl; k->printState(true);
            cout << "server status " << ets[0]->getStatusString() << endl;
            // recharging VS releasing: if task reaches its WCET (=> budget ends), then recharging, else releasing?
            REQUIRE (ets[0]->getStatus() == ServerStatus::RECHARGING);
            REQUIRE (ets[4] == k->getRunningTask(cpus_big[0]));
            REQUIRE (k->getProcessorReady(ets[5]) == cpus_big[3]);

            SIMUL.run_to(31);
            cout << "==================" << endl;
            cout << "t=" << time() << endl;
            cout << "state of kernel:" << endl; k->printState(true);
            cout << "server status " << ets[0]->getStatusString() << endl;
            REQUIRE (k->getRunningTask(cpus_big[0]) == NULL);
            REQUIRE (k->getProcessorReady(ets[5]) == cpus_big[3]);

            SIMUL.run_to(501); // all tasks are over, usual dispatch
            k->printState(true);
            for (CBServerCallingEMRTKernel* e : ets)
                REQUIRE (k->getProcessor(e) != NULL);

            SIMUL.endSingleRun();

            cout << "--------------" << endl;
            cout << "Simulation finished" << endl;
            for (AbsRTTask *t : tasks)
                delete t;
            for (CBServerCallingEMRTKernel* cbs : ets)
                delete cbs;
            delete k;
            return 0;
        }

        else if (TEST_NO == 23) {
            /// Does killInstance() on CBS server enveloping periodic tasks work?
            int wcets[] = { 10 };
            int deads[] = { 200 };
            vector<CBServerCallingEMRTKernel*> ets;
            for (int j = 0; j < sizeof(wcets) / sizeof(wcets[0]); j++) {
                task_name = "T" + to_string(TEST_NO) + "_task_BIG_" + to_string(j);
                cout << "Creating task: " << task_name;
                PeriodicTask* t = new PeriodicTask(deads[j], deads[j], 0, task_name);
                char instr[60] = "";
                sprintf(instr, "fixed(%d, %s);", wcets[j], workload.c_str());
                cout << instr << endl;
                t->insertCode(instr);
                ets.push_back(kern->addTaskAndEnvelope(t, ""));
                ttrace.attachToTask(*t);
                tasks.push_back(t);
            }
            EnergyMRTKernel* k = dynamic_cast<EnergyMRTKernel*>(kern);
            k->addForcedDispatch(ets[0], cpus_big[0], 18);

            for (CPU_BL* c : cpus_little)
                c->toggleDisabled();

            SIMUL.initSingleRun();

            SIMUL.run_to(1);
            cout << "==================" << endl;
            ets[0]->killInstance();
            k->printState(true);
            REQUIRE (k->getRunningTask(cpus_big[0]) == NULL);
            REQUIRE (k->getReadyTasks(cpus_big[0]).empty());
            REQUIRE (k->getUtilization_active(cpus_big[0]) > 0.0);
            SIMUL.run_to(201);
            cout << "==================" << endl;
            cout << "t=" << time() << endl;
            cout << "state of kernel:" << endl; k->printState(true);
            REQUIRE (k->getProcessorRunning(ets[0])->getIslandType() == IslandType::BIG);

            SIMUL.endSingleRun();

            cout << "--------------" << endl;
            cout << "Simulation finished" << endl;
            for (AbsRTTask *t : tasks)
                delete t;
            for (CBServerCallingEMRTKernel* cbs : ets)
                delete cbs;
            delete k;
            return 0;
        }

        // end of tests requiring EMRTK_CBS_ENVELOPING_MIGRATE_AFTER_VTIME_END

        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
        cout << "Running simulation!" << endl;


        SIMUL.run(1000); // 5000
        dynamic_cast<EnergyMRTKernel*>(kernels[0])->dumpPowerConsumption(true, tasks);
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
        cout << "Simulation finished" << endl;
    } catch (BaseExc &e) {
        cout << e.what() << endl;
    }
}

void dumpSpeeds(CPUModelBP::ComputationalModelBPParams const & params) {
  for (unsigned int f = 200000; f <= 2000000; f += 100000) {
    std::cout << "Slowness of " << f << " is " << CPUModelBP::slownessModel(params, f) << std::endl;
  }
}

void dumpAllSpeeds() {
  std::cout << "LITTLE:" << std::endl;
  CPUModelBP::ComputationalModelBPParams bzip2_cp = {0.0256054, 2.9809e+6, 0.602631, 8.13712e+9};
  dumpSpeeds(bzip2_cp);
  std::cout << "BIG:" << std::endl;
  bzip2_cp = {0.17833, 1.63265e+6, 1.62033, 118803};
  dumpSpeeds(bzip2_cp);
}

/// Returns true if the value 'eval' and 'expected' are distant 'error'%
bool isInRange(int eval, int expected) {
    const unsigned int error = 5;

    int min = int(eval - eval * error/100);
    int max = int(eval + eval * error/100);

    return expected >= min && expected <= max; 
}

/// True if min <= eval <= max
bool isInRangeMinMax(double eval, const double min, const double max) {
    return min <= eval <= max;
}