#include <rtsim/system.hpp>
#include <rtsim/system_descriptor.hpp>

#include <rtsim/mrtkernel.hpp>

#include <rtsim/scheduler/edfsched.hpp>
#include <rtsim/scheduler/fifosched.hpp>
#include <rtsim/scheduler/rmsched.hpp>
#include <rtsim/scheduler/rrsched.hpp>

namespace RTSim {
    uptr<Scheduler> make_scheduler(const std::string &name) {
        // TODO: support RR and other scheduler parameters
        if (name == "edf") {
            return std::make_unique<EDFScheduler>();
            // } else if (name == "rr") {
            //     return std::make_unique<RRScheduler>();
        } else if (name == "rm") {
            return std::make_unique<RMScheduler>();
        } else if (name == "fifo") {
            return std::make_unique<FIFOScheduler>();
        }

        throw BaseExc("Unsupported scheduler class: " + name);
    }

    uptr<CPUModel>
        make_model(const std::map<std::string, CPUMDescriptor> &models,
                   const std::vector<OPP> &opps, const std::string &name_power,
                   const std::string &name_speed) {
        auto model_power = models.find(name_power);
        if (model_power == models.cend())
            throw BaseExc("Could not find model: " + name_power);

        auto model_speed = models.find(name_speed);
        if (model_speed == models.cend())
            throw BaseExc("Could not find model: " + name_speed);

        // Last OPP is expected to be the maximum frequency
        return CPUModel::create(model_power->second, model_speed->second,
                                opps.back(), opps.back().frequency);
    }

    uptr<CPUIsland> make_island(const std::string &name,
                                const std::vector<OPP> &opps, CPUModel *model) {
        return std::make_unique<CPUIsland>(0, CPUIsland::Type::GENERIC, name,
                                           opps, model);
    }

    uptr<CPU> make_cpu(const std::string &name) {
        return std::make_unique<CPU>(name, nullptr);
    }

    uptr<RTKernel> make_kernel_global(Scheduler *scheduler,
                                      const std::string &name) {
        return std::make_unique<MRTKernel>(scheduler, std::set<CPU *>{}, name);
    }

    uptr<RTKernel> make_kernel_partitioned(Scheduler *scheduler,
                                           const std::string &name, CPU *cpu) {
        return std::make_unique<RTKernel>(scheduler, name, cpu);
    }

    // TODO: lots of exception handling?
    System::System(const string &fname) {
        // TODO: default values for all attributes
        const SystemDescriptor sys_des{fname};

        int cnt_cpus = 0;
        int cnt_islands = 0;

        // Fill up the islands
        for (const auto &island_des : sys_des.islands) {
            sptr<CPUModel> model;
            sptr<CPUIsland> island;
            sptr<CPU> cpu;
            sptr<TracePowerConsumption> ptrace;
            sptr<Scheduler> scheduler;
            sptr<RTKernel> kernel;

            auto opps = OPP::fromVectors(island_des.volts, island_des.freqs);
            if (opps.size() < 1) {
                throw BaseExc("The specified island '" + island_des.name +
                              "' has no OPPs!");
            }

            // TODO: check that multiple islands do not have the same name

            // CPU Model and Island (could probably share even more CPU Models,
            // since they are pretty muvch stateless, but oh well)
            model = make_model(sys_des.power_models, opps,
                               island_des.power_model, island_des.speed_model);
            island = make_island(island_des.name, opps, model.get());

            cpu_models.emplace_back(model);
            islands.emplace_back(island);

            // TODO: island initial (or fixed) frequency

            const std::string basename_kernel = island_des.name + "-kernel";
            const std::string basename_cpu = island_des.name + "-";

            // For global scheduling: one single MRTKernel with one
            // Scheduler
            if (island_des.kernel.placement == "global") {
                scheduler = make_scheduler(island_des.kernel.scheduler);
                kernel = make_kernel_global(scheduler.get(), basename_kernel);
                this->schedulers.emplace_back(scheduler);
                this->kernels.emplace_back(kernel);
            }

            for (int i = 0; i < island_des.numcpus; ++i, ++cnt_cpus) {
                const std::string cpuname =
                    basename_cpu + std::to_string(cnt_cpus);

                // TODO: fix CPU constructor
                cpu = make_cpu(cpuname);
                cpu->setIsland(island.get());
                cpu->setWorkload("idle");

                if (island_des.kernel.placement == "partitioned") {
                    // One RTKernel for each CPU, with its own scheduler
                    scheduler = make_scheduler(island_des.kernel.scheduler);
                    kernel = make_kernel_partitioned(
                        scheduler.get(),
                        basename_kernel + std::to_string(cnt_cpus), cpu.get());
                    this->schedulers.emplace_back(scheduler);
                    this->kernels.emplace_back(kernel);
                } else {
                    std::dynamic_pointer_cast<MRTKernel>(kernel)->addCPU(
                        cpu.get());
                }

                // TODO: fix ptrace
                ptrace = std::make_unique<TracePowerConsumption>(
                    cpu.get(), 1, "power_" + cpuname + ".txt");

                this->cpus.emplace_back(cpu);
                this->ptraces.emplace_back(ptrace);
            }

            // TODO: change default frequency for an island, for now they all
            // start with speed maximized
            island->setOPP(opps.size() - 1);

            ++cnt_islands;
        }
    }

} // namespace RTSim
