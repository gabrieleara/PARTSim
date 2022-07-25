#include "args.hpp"

/*
 * ╔═══════════════════════════════════════════════════════╗
 * ║                         Main                          ║
 * ╚═══════════════════════════════════════════════════════╝
 */

// LibMetasim
#include <metasim/simul.hpp>

// LibRTSim
#include <rtsim/cbserver.hpp>
#include <rtsim/json_trace.hpp>
#include <rtsim/resource/fcfsresmanager.hpp>
#include <rtsim/system.hpp>
#include <rtsim/texttrace.hpp>
#include <rtsim/waitinstr.hpp>

using Task_ptr = std::shared_ptr<RTSim::Task>;
using Server_ptr = std::shared_ptr<RTSim::Server>;

class ServerTask {
public:
    ServerTask(Task_ptr task, Server_ptr server) : task(task), server(server) {
        if (server)
            server->addTask(*task);
    }

    RTSim::AbsRTTask &getTaskForKernel() {
        return *server;
    }

    RTSim::AbsRTTask &getTaskForTracer() {
        return *task;
    }

private:
    Task_ptr task;
    Server_ptr server;
};

struct placement_t {
    ServerTask task;
    int initial_cpu;

    placement_t(Task_ptr task, Server_ptr server, int initial_cpu) :
        task(task, server),
        initial_cpu(initial_cpu) {}

    placement_t(ServerTask task, int initial_cpu) :
        task(task),
        initial_cpu(initial_cpu) {}
};

using TaskSet = std::vector<placement_t>;

TaskSet read_taskset(const std::string &tset_file) {
    yaml::Object_ptr tset_spec = yaml::parse(tset_file);

    TaskSet taskset;

    int i = 0;

    // TODO: assuming periodic task, ask for task type in YML
    for (const auto &task_spec : *(tset_spec->get("taskset"))) {
        auto str_name = task_spec->get("name")->get();
        auto str_iat = task_spec->get("iat")->get();
        auto str_startcpu = task_spec->get("startcpu")->get();
        auto str_runtime = task_spec->get("runtime")->get();
        auto str_rdl = task_spec->get("rdl")->get();
        auto str_ph = task_spec->get("ph")->get();
        auto str_qs = task_spec->get("qs")->get();
        auto code = task_spec->get("code");

        using Tick = MetaSim::Tick;

        int startcpu = str_startcpu.length() ? std::stoi(str_startcpu) : 0;

        auto iat = str_iat.length() ? Tick(std::stol(str_iat)) : Tick(0);
        auto runtime =
            str_runtime.length() ? Tick(std::stol(str_runtime)) : Tick(0);
        auto rdl = str_rdl.length() ? Tick(std::stol(str_rdl)) : iat;
        auto ph = str_ph.length() ? Tick(std::stol(str_ph)) : Tick(0);
        auto qs = str_qs.length() ? std::stol(str_qs) : 100L;

        auto task_ptr =
            std::make_shared<RTSim::PeriodicTask>(iat, rdl, ph, str_name, qs);

        for (const auto &instr : (*code)) {
            auto str_instr = instr->get();
            if (str_instr.length() < 1) {
                continue;
            }

            if (str_instr[str_instr.length() - 1] != ';') {
                str_instr += ";";
            }

            task_ptr->insertCode(str_instr);
        }

        // Use Hard CBS
        auto server_ptr = std::make_shared<RTSim::CBServer>(
            runtime, iat, rdl, true, "cbserver_" + str_name);

        taskset.emplace_back(task_ptr, server_ptr, startcpu);
    }

    return taskset;
}

std::unique_ptr<RTSim::ResManager>
    read_resources(const std::string &tset_file) {
    yaml::Object_ptr tset_spec = yaml::parse(tset_file);

    auto resources = std::make_unique<RTSim::FCFSResManager>();

    for (const auto &res_spec : *(tset_spec->get("resources"))) {
        auto str_name = res_spec->get("name")->get();
        auto str_initial_state = res_spec->get("initial_state")->get();

        // TODO: more general specification in YML for any kind of resource
        int n_initial = str_initial_state == "locked" ? 0 : 1;

        if (!resources->hasResource(str_name)) {
            resources->addResource(str_name, 1, n_initial);
        } else {
            std::cerr << "Cannot specify resource twice: " << str_name
                      << std::endl;
            throw std::exception{};
        }
    }

    return resources;
}

struct Tracer {
    std::unique_ptr<RTSim::TextTrace> ttrace;
    std::unique_ptr<RTSim::JSONTrace> jtrace;

    class UnrecognizedTracerException : public std::exception {
        const std::string _what;

    public:
        UnrecognizedTracerException(const std::string &fname) :
            _what("Unrecognized tracer type: " + fname + "!") {}

        const char *what() const noexcept override {
            return _what.c_str();
        }
    };

    Tracer(const std::string &fname) {
        if (string_endswith(fname, ".txt")) {
            ttrace = std::make_unique<RTSim::TextTrace>(fname);
        } else if (string_endswith(fname, ".json")) {
            jtrace = std::make_unique<RTSim::JSONTrace>(fname);
        } else {
            throw UnrecognizedTracerException(fname);
        }
    }

    void attachToTask(RTSim::AbsRTTask &task) {
        if (ttrace)
            ttrace->attachToTask(task);
        if (jtrace)
            jtrace->attachToTask(task);
    }
};

int main(int argc, char *argv[]) {
    auto opts = parse_arguments(argc, argv);

    MetaSim::Simulation &simulation = MetaSim::Simulation::getInstance();

    if (opts["debug"] == "true") {
        simulation.dbg.enable("All");
        simulation.dbg.setStream(opts["debug-out"]);
    }

    std::vector<Tracer> tracers;
    for (auto fname : list_split(opts["trace"])) {
        tracers.emplace_back(fname);
    }

    RTSim::System sys{opts["system"]};

    auto resmanager = read_resources(opts["taskset"]);
    for (auto &kernel : sys.kernels) {
        kernel->setResManager(resmanager.get());
    }

    TaskSet taskset = read_taskset(opts["taskset"]);
    for (auto &[task, cpu] : taskset) {
        sys.cpus[cpu]->getKernel()->addTask(task.getTaskForKernel());

        for (auto &tracer : tracers) {
            tracer.attachToTask(task.getTaskForTracer());
        }
    }

    try {
        simulation.run(std::stoi(opts["duration"]));
    } catch (std::exception &e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
        std::cerr << "TERMINATING!" << std::endl;
        return EXIT_FAILURE;
    }

    resmanager->getID();

    return EXIT_SUCCESS;
}
