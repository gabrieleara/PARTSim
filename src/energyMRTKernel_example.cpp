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
#include <tracepower.hpp>
#include <rttask.hpp>
#include <instr.hpp>
#include <powermodel.hpp>
#include <energyMRTKernel.hpp>

using namespace MetaSim;
using namespace RTSim;

/* ./energy [OPP little] [OPP big] [workload] */

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

int main(int argc, char *argv[])
{
    unsigned int OPP_little = 0; // Index of OPP in LITTLE cores
    unsigned int OPP_big = 0;    // Index of OPP in big cores
    string workload = "bzip2";
    std::vector<CPU*> cpus;

    dumpAllSpeeds();
    
    if (argc == 4) {
        OPP_little = stoi(argv[1]);
        OPP_big = stoi(argv[2]);
        workload = argv[3];
    }

    cout << "current OPPs indices: [" << OPP_little << ", " << OPP_big << "]" << endl;
    cout << "Workload: [" << workload << "]" << endl;

    try {
        SIMUL.dbg.enable("All");
        SIMUL.dbg.setStream("debug.txt");

        TextTrace ttrace("trace.txt");
        //JSONTrace jtrace("trace.json");

        vector<TracePowerConsumption *> ptrace;
        vector<EDFScheduler *> schedulers;
        vector<RTKernel *> kernels;
        vector<CPU *> cpus;

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
            CPU *c = new CPU(cpu_name, V_little, F_little, pm);
            c->setOPP(OPP_little);
            c->setWorkload("idle");
            c->setIsland(CPU::Island::LITTLE);
            pm->setFrequencyMax(max_frequency);
            TracePowerConsumption *power_trace = new TracePowerConsumption(c, 1, "power_" + cpu_name + ".txt");
            ptrace.push_back(power_trace);

            cpus.push_back(c);

            /*EDFScheduler *edfsched = new EDFScheduler;
            schedulers.push_back(edfsched);

            RTKernel *kern = new RTKernel(edfsched, "", c);
            kernels.push_back(kern);*/
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

            CPU *c = new CPU(cpu_name, V_big, F_big, pm);
            c->setOPP(OPP_big);
            c->setWorkload("idle");
            c->setIsland(CPU::Island::BIG);
            pm->setFrequencyMax(max_frequency);
            TracePowerConsumption *power_trace = new TracePowerConsumption(c, 1, "power_" + cpu_name + ".txt");
            ptrace.push_back(power_trace);

            cpus.push_back(c);

            /*EDFScheduler *edfsched = new EDFScheduler;
            schedulers.push_back(edfsched);

            RTKernel *kern = new RTKernel(edfsched, "", c);
            kernels.push_back(kern);*/
        }

        EDFScheduler *edfsched = new EDFScheduler;
        schedulers.push_back(edfsched);

        EnergyMRTKernel *kern = new EnergyMRTKernel(edfsched, cpus, "The sole kernel");
        kernels.push_back(kern);

        CPU::referenceFrequency = 2000; // BIG_3 frequency

        /*
         * Creating tasks
         */

        PeriodicTask *t;

        /* LITTLE */

        string task_name;
        int TEST_NO = 5;

        if (TEST_NO == 0) {
            cout << "workload "<<workload<<endl;
            task_name = "Task_LITTLE_0";
            cout << "Creating task: " << task_name << endl;
            t = new PeriodicTask(500, 500, 0, task_name);
            t->insertCode("fixed(100," + workload + ");");
            kernels[0]->addTask(*t, "");
            ttrace.attachToTask(*t);
            //jtrace.attachToTask(*t);

            task_name = "Task_LITTLE_1";
            cout << "Creating task: " << task_name << endl;
            t = new PeriodicTask(500, 500, 0, task_name);
            t->insertCode("fixed(100," + workload + ");");
            kernels[0]->addTask(*t, "");
            ttrace.attachToTask(*t);
            //jtrace.attachToTask(*t);

            task_name = "Task_LITTLE_2";
            cout << "Creating task: " << task_name << endl;
            t = new PeriodicTask(500, 500, 0, task_name);
            t->insertCode("fixed(100," + workload + ");");
            kernels[0]->addTask(*t, "");
            ttrace.attachToTask(*t);
            //jtrace.attachToTask(*t);

            task_name = "Task_LITTLE_3";
            cout << "Creating task: " << task_name << endl;
            t = new PeriodicTask(500, 500, 0, task_name);
            t->insertCode("fixed(100," + workload + ");");
            kernels[0]->addTask(*t, "");
            ttrace.attachToTask(*t);
            //jtrace.attachToTask(*t);

            task_name = "Task_big_0";
            cout << "Creating task: " << task_name << endl;
            t = new PeriodicTask(500, 500, 0, task_name);
            t->insertCode("fixed(100," + workload + ");");
            kernels[0]->addTask(*t, "");
            ttrace.attachToTask(*t);
            //jtrace.attachToTask(*t);

            // In Balsini's code, this is the content of trace.txt (cut at time=1000)
            /*
            [Time:0]	Task_LITTLE_0 arrived at 0
            [Time:0]	Task_big_0 arrived at 0
            [Time:0]	Task_LITTLE_0 scheduled on CPU LITTLE_0 200 abs WCET 100its arrival was 0
            [Time:0]	Task_big_0 scheduled on CPU BIG_0 200 abs WCET 100its arrival was 0
            [Time:7]	Task_LITTLE_0 ended, its arrival was 0
            [Time:12]	Task_big_0 ended, its arrival was 0
            [Time:500]	Task_LITTLE_0 arrived at 500
            [Time:500]	Task_big_0 arrived at 500
            [Time:500]	Task_LITTLE_0 scheduled on CPU LITTLE_0 200 abs WCET 100its arrival was 500
            [Time:500]	Task_big_0 scheduled on CPU BIG_0 200 abs WCET 100its arrival was 500
            [Time:507]	Task_LITTLE_0 ended, its arrival was 500
            [Time:512]	Task_big_0 ended, its arrival was 500
            [Time:1000]	Task_LITTLE_0 arrived at 1000
            [Time:1000]	Task_big_0 arrived at 1000
            [Time:1000]	Task_LITTLE_0 scheduled on CPU LITTLE_0 200 abs WCET 100its arrival was 1000
            [Time:1000]	Task_big_0 scheduled on CPU BIG_0 200 abs WCET 100its arrival was 1000

             As you can see , if Task_LITTLE_0 (bzip) is scheduled on little freq 200, its workload gets 7.
             Just as me.
             Task_big_0 is instead scheduled on CPU_LITTLE_3 after finishing the others since on LITTLE cores
             it would have the least consumption (and it cannot escape the chosen island in chooseCPU() )
             */
        }
        else if (TEST_NO == 1) {
            task_name = "task1";
            cout << "Creating task: " << task_name << endl;
            t = new PeriodicTask(500, 500, 0, task_name);
            //t->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
            t->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
            kernels[0]->addTask(*t, "");
            ttrace.attachToTask(*t);
            //jtrace.attachToTask(*t);

            // only task1 (500,500) => BIG max freq = 2000, with 500 the scaled WCET
        }
        else if (TEST_NO == 2) {
            task_name = "task1";
            cout << "Creating task: " << task_name << endl;
            t = new PeriodicTask(500, 500, 0, task_name);
            t->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
            kernels[0]->addTask(*t, "");
            ttrace.attachToTask(*t);
            //jtrace.attachToTask(*t);

            task_name = "task2";
            cout << "Creating task: " << task_name << endl;
            t = new PeriodicTask(500, 500, 0, task_name);
            t->insertCode("fixed(500," + workload + ");");
            kernels[0]->addTask(*t, "");
            ttrace.attachToTask(*t);

            // task1 (500,500) => BIG_3 max freq, task2 (500,500) => BIG_2 max freq
        }
        else if (TEST_NO == 3) {
            task_name = "task1";
            cout << "Creating task: " << task_name << endl;
            t = new PeriodicTask(500, 500, 0, task_name);
            t->insertCode("fixed(500," + workload + ");"); // WCET 500 at max frequency on big cores
            kernels[0]->addTask(*t, "");
            ttrace.attachToTask(*t);
            //jtrace.attachToTask(*t);

            task_name = "task2";
            cout << "Creating task: " << task_name << endl;
            t = new PeriodicTask(500, 500, 0, task_name);
            t->insertCode("fixed(250," + workload + ");");
            kernels[0]->addTask(*t, "");
            ttrace.attachToTask(*t);

            // task1 (500,500) => BIG_3 max freq, task2 (250,500) => LITTLE_3 max freq (WCET gets 358 = 250*2000/1400)
        }
        else if (TEST_NO == 4) {
            task_name = "task1";
            cout << "Creating task: " << task_name << endl;
            t = new PeriodicTask(500, 500, 0, task_name);
            t->insertCode("fixed(10," + workload + ");"); // WCET 10 at max frequency on big cores
            kernels[0]->addTask(*t, "");
            ttrace.attachToTask(*t);
            //jtrace.attachToTask(*t);

            // task1 (10,500) => LITTLE_3 freq 1400
            // if you only have a small task, in the formula for ideltapow the factors of the multiplication
            // compensate each other, so that even if one grows and the other sinks, the result remains the same.
            // Since I use a map to store <energy delta, <cpu, freq>>, the smaller energies are substituted.
            // Maybe I should tell that when the energy delta is already in the map associated with a smaller
            // freq of the same CPU island, then don't replace it?
        }
        else if(TEST_NO == 5) {
            for (int j = 0; j < 4; j++) {
                int wcet = 5; //* (j+1);
                task_name = "task" + std::to_string(j);
                cout << "Creating task: " << task_name;
                t = new PeriodicTask(500, 500, 0, task_name);
                char instr[60] = "";
                sprintf(instr, "fixed(%d, bzip2);", wcet);
                cout << " with abs. WCET " << wcet << endl;
                t->insertCode(instr);
                kernels[0]->addTask(*t, "");
                ttrace.attachToTask(*t);

                // LITTLE_0, _1, _2, _3 freq 1400.
            }

            // small tasks. They should be scheduled on a single little at some frequency
        }


        /*
         * Output execution time estimation for each workload on each OPP of
         * big and LITTLE cpus.
         */

        cout << "Dumping tasks' execution times" << endl;

        map<string, double> min_C;
        min_C["bzip2"] = 4.69799888;
        min_C["cachekiller"] = 0.518917331;
        min_C["hash"] = 0.656942014;
        min_C["encrypt"] = 0.746811798;
        min_C["decrypt"] = 0.754088207;

        /*for (string cpu_type : {"big", "LITTLE"}) {
            unsigned int cpu = cpu_type == "big" ? 5 : 1;
            unsigned int old_opp;
            auto opp_size = cpu_type == "big" ? F_big.size() : F_little.size();

            for (string wl : {"bzip2", "hash", "encrypt", "decrypt", "cachekiller"}) {
                cpus[cpu]->setWorkload(wl);

                string filename = "exec_" + wl + "_" + cpu_type + ".txt";
                ofstream computing_file(filename);

                old_opp = cpus[cpu]->getOPP();
                for (unsigned int opp=0; opp<opp_size; ++opp) {
                    cpus[cpu]->setOPP(opp);
                    computing_file << cpus[cpu]->getFrequency() * 1000 << " "
                                   << cpus[cpu]->getSpeed() * min_C[wl]
                                      << endl;
                }
                cpus[cpu]->setWorkload("idle");
                cpus[cpu]->setOPP(old_opp);
            }
        }*/

        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
        cout << "Running simulation!" << endl;

        SIMUL.run(1000); // 5000
    } catch (BaseExc &e) {
        cout << e.what() << endl;
    }
}
