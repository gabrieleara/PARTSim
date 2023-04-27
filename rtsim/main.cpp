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
#include <rtsim/exeinstr.hpp>
#include <rtsim/task.hpp>
#include <rtsim/grubserver.hpp>

using Task_ptr = std::shared_ptr<RTSim::Task>;
using Server_ptr = std::shared_ptr<RTSim::Server>;
using UManager_ptr = std::shared_ptr<RTSim::UtilizationManager>;

class ServerTask {
public:
    ServerTask(Task_ptr task, Server_ptr server) : task(task), server(server) {
        if (server)
            server->addTask(*task);
    }

    Server_ptr getServer() {
        return server;
    }

    RTSim::AbsRTTask &getTask() {
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

    // Check that we do not mix different types of servers in the same task set
    std::string global_server_type = ""; 
    
    // TODO: assuming periodic task, ask for task type in YML
    for (const auto &task_spec : *(tset_spec->get("taskset"))) {
        auto str_name = task_spec->get("name")->get();
        auto str_iat = task_spec->get("iat")->get();
        auto str_startcpu = task_spec->get("startcpu")->get();
        auto str_deadline = task_spec->get("deadline")->get();

        // by default, no server is used
        std::string str_server_type = "";
        std::string str_cbs_runtime = "0";
        std::string str_cbs_period = "0";
        std::string str_cbs_deadline = "0";

        // (glipari) Now you can have CBS or grub. In principles,
        // other kind of servers are possible.  Notice that I changed
        // the semantic : now it is mandatory to specify a server_type
        // if we want to use a server
        if (task_spec->has("server_type")) {
            str_server_type = task_spec->get("server_type")->get();
            str_cbs_runtime = task_spec->get("cbs_runtime")->get();
            str_cbs_period = task_spec->get("cbs_period")->get();
            str_cbs_deadline = task_spec->get("cbs_deadline")->get();
        }
        // else { // by default, I assume cbs
        //     str_server_type = "cbs";
        //     str_cbs_runtime = task_spec->has("cbs_runtime") ? task_spec->get("cbs_runtime")->get() : "0";
        //     str_cbs_period = task_spec->has("cbs_period") ? task_spec->get("cbs_period")->get() : "0";
        //     str_cbs_deadline = task_spec->has("cbs_deadline") ? task_spec->get("cbs_deadline")->get() : "0";
        // }

        auto str_ph = task_spec->get("ph")->get();
        auto str_qs = task_spec->get("qs")->get();
        auto code = task_spec->get("code");

        using Tick = MetaSim::Tick;

        int startcpu = str_startcpu.length() ? std::stoi(str_startcpu) : 0;

        auto iat = str_iat.length() ? Tick(std::stol(str_iat)) : Tick(0);
        auto deadline = str_deadline.length() ? Tick(std::stol(str_deadline)) : iat;
        auto cbs_runtime =
            str_cbs_runtime.length() ? Tick(std::stol(str_cbs_runtime)) : Tick(0);
        auto cbs_period = str_cbs_period.length() ? Tick(std::stol(str_cbs_period)) : iat;
        auto cbs_deadline = str_cbs_deadline.length() ? Tick(std::stol(str_cbs_deadline)) : cbs_period;
        auto ph = str_ph.length() ? Tick(std::stol(str_ph)) : Tick(0);
        auto qs = str_qs.length() ? std::stol(str_qs) : 100L;

        auto task_ptr = std::make_shared<RTSim::PeriodicTask>(iat, deadline, ph,
                                                              str_name, qs);

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

        if (str_server_type != "" and
            (cbs_period == 0 or cbs_deadline == 0 or cbs_runtime == 0)) {
            std::cerr << "Error : unvalid CBS parameters, period="
                      << cbs_period << ", deadline="
                      << cbs_deadline << ", runtime="
                      << cbs_runtime << std::endl;
            exit(EXIT_FAILURE);
        }

        if (str_server_type == "cbs") {
            if (global_server_type != "" and global_server_type != "cbs") {
                std::cerr << "Cannot mix different types of server on the same task set" << std::endl;
                exit(EXIT_FAILURE);
            }
            global_server_type = str_server_type;
            // Use Hard CBS
            auto server_ptr = std::make_shared<RTSim::CBServer>(
                cbs_runtime, cbs_period, cbs_deadline, true, "cbserver_" + str_name);
            
            taskset.emplace_back(task_ptr, server_ptr, startcpu);
        } else if (str_server_type == "grub") {
            if (global_server_type != "" and global_server_type != "grub") {
                std::cerr << "Cannot mix different types of servers on the same task set" << std::endl;
                exit(EXIT_FAILURE);
            }
            global_server_type = str_server_type;
            
            auto server_ptr = std::make_shared<RTSim::Grub>(
                cbs_runtime, cbs_period, "grub_" + str_name);
            taskset.emplace_back(task_ptr, server_ptr, startcpu);
        } else if (str_server_type == "") {
            if (global_server_type == "grub") {
                std::cerr << "All tasks must be handled by a GRUB server" << std::endl;
                exit(EXIT_FAILURE);
            }
            taskset.emplace_back(task_ptr, Server_ptr(), startcpu);
        }
        
        // if (cbs_period > 0) {
        //     auto server_ptr = std::make_shared<RTSim::CBServer>(
        //         cbs_runtime, cbs_period, cbs_deadline, true, "cbserver_" + str_name);

        //     taskset.emplace_back(task_ptr, server_ptr, startcpu);
        // } else {
        //   taskset.emplace_back(task_ptr, Server_ptr(), startcpu);
        // }
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
        RTSim::Task *t = dynamic_cast<RTSim::Task *>(&task);
        const std::vector<std::unique_ptr<RTSim::Instr>> &instrs =
            t->getInstrQueue();
        for (auto i = instrs.begin(); i != instrs.end(); ++i) {
            RTSim::ExecInstr *ei = dynamic_cast<RTSim::ExecInstr *>(i->get());
            if (ei != 0) {
                if (ttrace)
                    ei->setTrace(*ttrace.get());
            }
        }
    }
};

int main(int argc, char *argv[]) {
    auto opts = parse_arguments(argc, argv);

    MetaSim::Simulation &simulation = MetaSim::Simulation::getInstance();
    simulation.dbg.enable(_TASK_DBG_LEV);
    
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

    std::vector<UManager_ptr> umanagers;
    
    // Create the UtilizationManagers, one per CPU
    for (auto &cpu : sys.cpus) 
        umanagers.push_back(std::make_shared<RTSim::UtilizationManager>(std::string("umgr_") + cpu->getName()));

    TaskSet taskset = read_taskset(opts["taskset"]);
    for (auto &[tasksrv, cpu] : taskset) {
        if (tasksrv.getServer()) {
            sys.cpus[cpu]->getKernel()->addTask(*tasksrv.getServer());

            // if it's a grub, add the server to the UtilizationManager
            // @todo (glipari) tpo be generalized to any server later on
            if (dynamic_cast<RTSim::Grub *>(tasksrv.getServer().get())) 
                umanagers[cpu]->addServer(tasksrv.getServer().get());
            
            for (auto &tracer : tracers) {
                tracer.attachToTask(tasksrv.getTask());
                if (tracer.ttrace)
                    tasksrv.getServer()->setTrace(*tracer.ttrace.get());
                if (tracer.jtrace)
                    tasksrv.getServer()->setTrace(*tracer.jtrace.get());
            }
        } else {
            sys.cpus[cpu]->getKernel()->addTask(tasksrv.getTask());
            for (auto &tracer : tracers)
                tracer.attachToTask(tasksrv.getTask());
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
