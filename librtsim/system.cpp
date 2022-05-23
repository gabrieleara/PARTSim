#include <rtsim/system.hpp>
#include <rtsim/system_descriptor.hpp>

#include <rtsim/mrtkernel.hpp>

#include <rtsim/scheduler/edfsched.hpp>
#include <rtsim/scheduler/fifosched.hpp>
#include <rtsim/scheduler/rmsched.hpp>
#include <rtsim/scheduler/rrsched.hpp>

namespace RTSim {

    std::shared_ptr<Scheduler> make_scheduler(const std::string &name) {
        // TODO: support RR and other scheduler parameters
        if (name == "edf") {
            return std::make_shared<EDFScheduler>();
        // } else if (name == "rr") {
        //     return std::make_shared<RRScheduler>();
        } else if (name == "rm") {
            return std::make_shared<RMScheduler>();
        } else if (name == "fifo") {
            return std::make_shared<FIFOScheduler>();
        }

        throw BaseExc("Unsupported scheduler class: " + name);
    }

    // TODO: lots of exception handling?
    System::System(const string &fname) {
        const SystemDescriptor sys_des{fname};

        // csv::CSVDocument pmtable{params.pm_descriptor};

        int cpu_cnt = 0;
        int island_cnt = -1;

        for (const auto &island : sys_des.islands) {
            // Construct the appropriate CPUModel for each island and then
            // create copies for each CPU
            ++island_cnt;

            auto cpu_pmodel_des = sys_des.power_models.find(island.power_model);
            if (cpu_pmodel_des == sys_des.power_models.cend())
                throw std::exception{};

            auto cpu_smodel_des = sys_des.power_models.find(island.power_model);
            if (cpu_smodel_des == sys_des.power_models.cend())
                throw std::exception{};

            // TODO: default OPP and fmax for the island
            std::shared_ptr<CPUModel> cpu_model_base_ptr = CPUModel::create(
                cpu_pmodel_des->second, cpu_smodel_des->second);

            const string cpu_base_name = "CPU_" + island.name + "_";

            // Frequencies are already sorted in non-decreasing order
            const unsigned int fmax = *(island.freqs.end() - 1);

            Scheduler_ptr scheduler;
            RTKernel_ptr kernel;

            if (island.kernel.placement == "global") {
                // One single MRTKernel with one single Scheduler
                scheduler = make_scheduler(island.kernel.scheduler);
                kernel = std::make_shared<MRTKernel>(scheduler.get(),
                                                     std::set<CPU *>{},
                                                     island.name + "-kernel");

                this->schedulers.push_back(scheduler);
                this->kernels.push_back(kernel);
            }

            for (int i = 0; i < island.numcpus; ++i, ++cpu_cnt) {
                const string cpuname = cpu_base_name + std::to_string(cpu_cnt);

                // Clone the base model to create the CPU model
                std::shared_ptr<CPUModel> cpu_model =
                    cpu_model_base_ptr->clone();
                std::shared_ptr<CPU> cpu = std::make_shared<CPU>(
                    cpuname, island.volts, island.freqs, cpu_model.get());

                cpu->setOPP(
                    0); // TODO: let user choose default OPP for each island
                cpu->setWorkload(
                    "idle"); // TODO: do islands always start as idle?

                auto ptrace = std::make_shared<TracePowerConsumption>(
                    cpu.get(), 1, "power_" + cpuname + ".txt");

                if (island.kernel.placement == "partitioned") {
                    // One RTKernel for each CPU, with its own scheduler
                    scheduler = make_scheduler(island.kernel.scheduler);
                    kernel = std::make_shared<RTKernel>(
                        scheduler.get(),
                        island.name + "-kernel" + std::to_string(cpu_cnt),
                        cpu.get());

                    this->schedulers.push_back(scheduler);
                    this->kernels.push_back(kernel);
                } else {
                    std::dynamic_pointer_cast<MRTKernel>(kernel)->addCPU(
                        cpu.get());
                }

                this->cpu_models.push_back(cpu_model);
                this->cpus.push_back(cpu);
                this->ptraces.push_back(ptrace);
            }
        }
    }

} // namespace RTSim
