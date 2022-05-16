#include <rtsim/system.hpp>
#include <rtsim/system_descriptor.hpp>

namespace RTSim {
    // TODO: lots of exception handling?
    System::System(const string &fname) {
        const SystemDescriptor sys_des{fname};

        // csv::CSVDocument pmtable{params.pm_descriptor};

        for (const auto &island : sys_des.islands) {
            // Construct the appropriate CPUModel for each island and then
            // create copies for each CPU

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

            for (int i = 0; i < island.numcpus; ++i) {
                const string cpuname = cpu_base_name + std::to_string(i);

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

                // TODO: a scheduler factory that has a number of different
                // schedulers to choose from starting from the value of
                // island.kernel.scheduler
                auto scheduler = std::make_shared<EDFScheduler>();
                auto kernel = std::make_shared<RTKernel>(
                    scheduler.get(), island.kernel.name, cpu.get());

                this->cpu_models.push_back(cpu_model);
                this->cpus.push_back(cpu);
                this->ptraces.push_back(ptrace);
                this->schedulers.push_back(scheduler);
                this->kernels.push_back(kernel);
            }
        }
    }

} // namespace RTSim
