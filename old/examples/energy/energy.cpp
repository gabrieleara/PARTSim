/*
  In this example, a simple system is simulated, to provide
  an evaluation of different workloads running on an big.LITTLE
  Odroid-XU3 embedded board.
*/

#include <cstring>
#include <fstream>
#include <string>

#include <rtsim/cpu.hpp>
#include <rtsim/instr.hpp>
#include <rtsim/json_trace.hpp>
#include <rtsim/jtrace.hpp>
#include <rtsim/mrtkernel.hpp>
#include <rtsim/powermodel.hpp>
#include <rtsim/rttask.hpp>
#include <rtsim/scheduler/edfsched.hpp>
#include <rtsim/system_descriptor.hpp>
#include <rtsim/texttrace.hpp>
#include <rtsim/tracepower.hpp>

using namespace MetaSim;
using namespace RTSim;

/* ./energy [OPP little] [OPP big] [workload] */

int main(int argc, char *argv[]) {
    unsigned int OPP_little = 0;
    unsigned int OPP_big = 0;
    string workload = "bzip2";

    if (argc == 4) {
        OPP_little = stoi(argv[1]);
        OPP_big = stoi(argv[2]);
        workload = argv[3];
    }

    std::cout << "OPPs: [" << OPP_little << ", " << OPP_big << "]" << std::endl;
    std::cout << "Workload: [" << workload << "]" << std::endl;

    try {
        SIMUL.dbg.enable("All");
        SIMUL.dbg.setStream("debug.txt");

        TextTrace ttrace("trace.txt");
        JSONTrace jtrace("trace.json");

        vector<TracePowerConsumption *> ptrace;
        vector<EDFScheduler *> schedulers;
        vector<RTKernel *> kernels;
        vector<CPU *> cpus;

        vector<volt_type> V_little = {
            0.92,   0.919643, 0.919357, 0.918924, 0.95625, 0.9925, 1.02993,
            1.0475, 1.08445,  1.12125,  1.15779,  1.2075,  1.25625};
        vector<freq_type> F_little = {200, 300,  400,  500,  600,  700, 800,
                                      900, 1000, 1100, 1200, 1300, 1400};

        vector<volt_type> V_big = {
            0.916319, 0.915475, 0.915102, 0.91498, 0.91502, 0.90375, 0.916562,
            0.942543, 0.96877,  0.994941, 1.02094, 1.04648, 1.05995, 1.08583,
            1.12384,  1.16325,  1.20235,  1.2538,  1.33287};
        vector<freq_type> F_big = {200,  300,  400,  500,  600,  700,  800,
                                   900,  1000, 1100, 1200, 1300, 1400, 1500,
                                   1600, 1700, 1800, 1900, 2000};

        if (OPP_little >= V_little.size() || OPP_big >= V_big.size())
            exit(-1);

        unsigned long int max_frequency =
            max(F_little[F_little.size() - 1], F_big[F_big.size() - 1]);

        /* ---------------------- Creating CPU Models ----------------------- */

        CPUMDescriptor big_desc;
        CPUMDescriptor little_desc;

        big_desc.name = "balsini_pannocchi_big";
        little_desc.name = "balsini_pannocchi_little";

        big_desc.type = CPUModelBPParams::key;
        little_desc.type = CPUModelBPParams::key;

        /* --------------------- Creating LITTLE Models --------------------- */

        std::unique_ptr<CPUModelBPParams> bpp;

        bpp = std::make_unique<CPUModelBPParams>();
        bpp->workload = "idle";
        bpp->params.power = {0.00134845, 1.76307e-5, 124.535, 1.00399e-10};
        bpp->params.speed = {1, 0, 0, 0};
        little_desc.params.push_back(std::move(bpp));

        bpp = std::make_unique<CPUModelBPParams>();
        bpp->workload = "idle";
        bpp->params.power = {0.00134845, 1.76307e-5, 124.535, 1.00399e-10};
        bpp->params.speed = {1, 0, 0, 0};
        little_desc.params.push_back(std::move(bpp));

        bpp = std::make_unique<CPUModelBPParams>();
        bpp->workload = "bzip2";
        bpp->params.power = {0.00775587, 33.376, 1.54585, 9.53439e-10};
        bpp->params.speed = {0.0256054, 2.9809e+6, 0.602631, 8.13712e+9};
        little_desc.params.push_back(std::move(bpp));

        bpp = std::make_unique<CPUModelBPParams>();
        bpp->workload = "hash";
        bpp->params.power = {0.00624673, 176.315, 1.72836, 1.77362e-10};
        bpp->params.speed = {0.00645628, 3.37134e+6, 7.83177, 93459};
        little_desc.params.push_back(std::move(bpp));

        bpp = std::make_unique<CPUModelBPParams>();
        bpp->workload = "encrypt";
        bpp->params.power = {0.00676544, 26.2243, 5.6071, 5.34216e-10};
        bpp->params.speed = {6.11496e-78, 3.32246e+6, 6.5652, 115759};
        little_desc.params.push_back(std::move(bpp));

        bpp = std::make_unique<CPUModelBPParams>();
        bpp->workload = "decrypt";
        bpp->params.power = {0.00629664, 87.1519, 2.93286, 2.80871e-10};
        bpp->params.speed = {5.0154e-68, 3.31791e+6, 7.154, 112163};
        little_desc.params.push_back(std::move(bpp));

        bpp = std::make_unique<CPUModelBPParams>();
        bpp->workload = "cachekiller";
        bpp->params.power = {0.0126737, 67.9915, 1.63949, 3.66185e-10};
        bpp->params.speed = {1.20262, 352597, 2.03511, 169523};
        little_desc.params.push_back(std::move(bpp));

        /* ---------------------- Creating BIG Models ----------------------- */

        bpp = std::make_unique<CPUModelBPParams>();
        bpp->workload = "idle";
        bpp->params.power = {0.0162881, 0.00100737, 55.8491, 1.00494e-9};
        bpp->params.speed = {1, 0, 0, 0};
        big_desc.params.push_back(std::move(bpp));

        bpp = std::make_unique<CPUModelBPParams>();
        bpp->workload = "bzip2";
        bpp->params.power = {0.0407739, 12.022, 3.33367, 7.4577e-9};
        bpp->params.speed = {0.17833, 1.63265e+6, 1.62033, 118803};
        big_desc.params.push_back(std::move(bpp));

        bpp = std::make_unique<CPUModelBPParams>();
        bpp->workload = "hash";
        bpp->params.power = {0.0388215, 16.3205, 4.3418, 5.07039e-9};
        bpp->params.speed = {0.017478, 1.93925e+6, 4.22469, 83048.3};
        big_desc.params.push_back(std::move(bpp));

        bpp = std::make_unique<CPUModelBPParams>();
        bpp->workload = "encrypt";
        bpp->params.power = {0.0348728, 8.14399, 5.64344, 7.69915e-9};
        bpp->params.speed = {8.39417e-34, 1.99222e+6, 3.33002, 96949.4};
        big_desc.params.push_back(std::move(bpp));

        bpp = std::make_unique<CPUModelBPParams>();
        bpp->workload = "decrypt";
        bpp->params.power = {0.0320508, 25.8727, 3.27135, 4.11773e-9};
        bpp->params.speed = {9.49471e-35, 1.98761e+6, 2.65652, 109497};
        big_desc.params.push_back(std::move(bpp));

        bpp = std::make_unique<CPUModelBPParams>();
        bpp->workload = "cachekiller";
        bpp->params.power = {0.086908, 9.17989, 2.5828, 7.64943e-9};
        bpp->params.speed = {0.825212, 235044, 786.368, 25622.1};
        big_desc.params.push_back(std::move(bpp));

        /* ------------------- Instantiating actual CPUs -------------------- */

        for (unsigned int i = 0; i < 4; ++i) {
            /* Create LITTLE CPUs */
            string cpu_name = "LITTLE_" + to_string(i);
            std::cout << "Creating CPU: " << cpu_name << std::endl;

            CPUModel *pm = CPUModel::create(little_desc, little_desc,
                                            OPP{F_little[F_little.size() - 1],
                                                V_little[V_little.size() - 1]},
                                            max_frequency)
                               .release();

            CPU *c = new CPU(cpu_name, V_little, F_little, pm);
            // pm->setCPU(c);
            cpus.push_back(c);
            c->setOPP(OPP_little);
            c->setWorkload("idle");
            // pm->setFrequencyMax(max_frequency);
            TracePowerConsumption *power_trace =
                new TracePowerConsumption(c, 1, "power_" + cpu_name + ".txt");
            ptrace.push_back(power_trace);

            EDFScheduler *edfsched = new EDFScheduler;
            schedulers.push_back(edfsched);

            RTKernel *kern = new RTKernel(edfsched, "", c);
            kernels.push_back(kern);
        }

        for (unsigned int i = 0; i < 4; ++i) {
            /* Create big CPUs */
            string cpu_name = "BIG_" + to_string(i);
            std::cout << "Creating CPU: " << cpu_name << std::endl;

            CPUModel *pm = CPUModel::create(big_desc, big_desc,
                                            OPP{F_big[F_big.size() - 1],
                                                V_big[V_big.size() - 1]},
                                            max_frequency)
                               .release();

            CPU *c = new CPU(cpu_name, V_big, F_big, pm);
            // pm->setCPU(c);
            cpus.push_back(c);
            c->setOPP(OPP_big);
            c->setWorkload("idle");
            // pm->setFrequencyMax(max_frequency);
            TracePowerConsumption *power_trace =
                new TracePowerConsumption(c, 1, "power_" + cpu_name + ".txt");
            ptrace.push_back(power_trace);

            EDFScheduler *edfsched = new EDFScheduler;
            schedulers.push_back(edfsched);

            RTKernel *kern = new RTKernel(edfsched, "", c);
            kernels.push_back(kern);
        }

        /*
         * Creating tasks
         */

        PeriodicTask *t;

        /* LITTLE */

        string task_name;

        task_name = "Task_LITTLE_0";
        std::cout << "Creating task: " << task_name << std::endl;
        t = new PeriodicTask(500, 100, 0, task_name);
        t->insertCode("fixed(100," + workload + ");");
        kernels[0]->addTask(*t, "");
        ttrace.attachToTask(*t);
        jtrace.attachToTask(*t);

        /* big */

        task_name = "Task_big_0";
        std::cout << "Creating task: " << task_name << std::endl;
        t = new PeriodicTask(500, 100, 0, task_name);
        t->insertCode("fixed(100," + workload + ");");
        kernels[4]->addTask(*t, "");
        ttrace.attachToTask(*t);
        jtrace.attachToTask(*t);

        /*
         * Output execution time estimation for each workload on each OPP of
         * big and LITTLE cpus.
         */

        std::cout << "Dumping tasks' execution times" << std::endl;

        map<string, double> min_C;
        min_C["bzip2"] = 4.69799888;
        min_C["cachekiller"] = 0.518917331;
        min_C["hash"] = 0.656942014;
        min_C["encrypt"] = 0.746811798;
        min_C["decrypt"] = 0.754088207;

        for (string cpu_type : {"big", "LITTLE"}) {
            unsigned int cpu = cpu_type == "big" ? 5 : 1;
            unsigned int old_opp;
            auto opp_size = cpu_type == "big" ? F_big.size() : F_little.size();

            for (string wl :
                 {"bzip2", "hash", "encrypt", "decrypt", "cachekiller"}) {
                cpus[cpu]->setWorkload(wl);

                string filename = "exec_" + wl + "_" + cpu_type + ".txt";
                ofstream computing_file(filename);

                old_opp = cpus[cpu]->getOPP();
                for (unsigned int opp = 0; opp < opp_size; ++opp) {
                    cpus[cpu]->setOPP(opp);
                    computing_file << cpus[cpu]->getFrequency() * 1000 << " "
                                   << cpus[cpu]->getSpeed() * min_C[wl]
                                   << std::endl;
                }
                cpus[cpu]->setWorkload("idle");
                cpus[cpu]->setOPP(old_opp);
            }
        }

        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
        std::cout << "Running simulation!" << std::endl;

        SIMUL.run(5000);
    } catch (BaseExc &e) {
        std::cout << e.what() << std::endl;
    }
}
