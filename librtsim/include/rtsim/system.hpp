// TODO: organize includes

// STL
#include <metasim/memory.hpp>
#include <vector>

// Static system information
#include <rtsim/cpu.hpp>
#include <rtsim/scheduler/edfsched.hpp>
#include <rtsim/kernel.hpp>
#include <rtsim/mrtkernel.hpp>
#include <rtsim/powermodel.hpp>
#include <rtsim/tracepower.hpp>

// Tasks system information
#include <rtsim/instr.hpp>
#include <rtsim/rttask.hpp>

// Tracing
#include <rtsim/json_trace.hpp>
#include <rtsim/jtrace.hpp>
#include <rtsim/texttrace.hpp>
#include <rtsim/tracepower.hpp>

// File formats
#include <rtsim/csv.hpp>
#include <rtsim/yaml.hpp>

namespace RTSim {
    // TODO: all private, immutable and yadda yadda
    // TODO: the system has no notion of islands, should add that!
    class System {
    public:
        using TracePowerConsumption_ptr =
            std::shared_ptr<TracePowerConsumption>;
        using Scheduler_ptr = std::shared_ptr<Scheduler>;
        using RTKernel_ptr = std::shared_ptr<RTKernel>;
        using CPU_ptr = std::shared_ptr<CPU>;
        using CPUModel_ptr = std::shared_ptr<CPUModel>;

        // TODO: hide
    public:
        std::vector<CPUModel_ptr> cpu_models;
        std::vector<CPU_ptr> cpus;
        std::vector<TracePowerConsumption_ptr> ptraces;
        std::vector<Scheduler_ptr> schedulers;
        std::vector<RTKernel_ptr> kernels;

    public:
        System(const std::string &fname);
        System(std::string &&fname) : System(fname) {}
    };

    // TODO:
    class TaskSet {
    public:
        using Task_ptr = std::shared_ptr<Task>;

        std::vector<Task_ptr> tasks;
    };

    /*
    struct SystemParameters {
        struct KernelParameters {
            std::string name;
            std::string scheduler;
        };

        struct IslandParameters {
            // std::string name;
            std::string pm_type;
            size_t numcpus;
            KernelParameters kernel;
        };

        std::string pm_descriptor;
        std::vector<IslandParameters> cpu_islands;

        SystemParameters(yaml::Object_ptr &&description)
            : SystemParameters(description) {}

        // TODO: exceptions and all when mandatory parameters are missing
        SystemParameters(yaml::Object_ptr &description) {
            yaml::Object_ptr islands = description->get("cpu_islands");
            for (auto &island : *islands) {
                yaml::Object_ptr kernel = island->get("kernel");

                const KernelParameters kp = {
                    kernel->get("name")->get(),
                    kernel->get("scheduler")->get(),
                };

                const IslandParameters ip = {
                    island->get("pm_type")->get(),
                    (size_t)std::stol(island->get("numcpus")->get()),
                    kp,
                };

                this->cpu_islands.push_back(ip);
            }

            this->pm_descriptor = description->get("pm_descriptor")->get();
        };
    }; // struct SystemParameters
    */

} // namespace RTSim
