#include "args.hpp"

/*
 * ╔═══════════════════════════════════════════════════════╗
 * ║                         Main                          ║
 * ╚═══════════════════════════════════════════════════════╝
 */

// LibMetasim
#include <metasim/simul.hpp>

// LibRTSim
#include <rtsim/fcfsresmanager.hpp>
#include <rtsim/json_trace.hpp>
#include <rtsim/system.hpp>
#include <rtsim/texttrace.hpp>
#include <rtsim/waitinstr.hpp>

using TaskSet = std::vector<std::shared_ptr<RTSim::Task>>;

TaskSet read_taskset(const std::string &tset_file) {
    yaml::Object_ptr tset_spec = yaml::parse(tset_file);

    TaskSet taskset;

    // FIXME: assuming periodic task, ask for task type in YML
    for (const auto &task_spec : *(tset_spec->get("taskset"))) {
        auto str_name = task_spec->get("name")->get();
        auto str_iat = task_spec->get("iat")->get();
        auto str_rdl = task_spec->get("rdl")->get();
        auto str_ph = task_spec->get("ph")->get();
        auto str_qs = task_spec->get("qs")->get();
        auto code = task_spec->get("code");

        using Tick = MetaSim::Tick;

        auto iat = str_iat.length() ? Tick(std::stol(str_iat)) : Tick(0);
        auto rdl = str_rdl.length() ? Tick(std::stol(str_rdl)) : iat;
        auto ph = str_ph.length() ? Tick(std::stol(str_ph)) : Tick(0);
        auto qs = str_qs.length() ? std::stol(str_qs) : 100L;

        auto task_ptr =
            std::make_shared<RTSim::PeriodicTask>(iat, rdl, ph, str_name, qs);

        for (const auto &instr : (*code)) {
            auto str_instr = instr->get();
            if (str_instr.length() < 1) {
                // TODO:
                throw std::exception{};
            }

            if (str_instr[str_instr.length() - 1] != ';') {
                str_instr += ";";
            }

            task_ptr->insertCode(str_instr);
        }

        taskset.push_back(task_ptr);
    }

    return taskset;
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

    void attachToTask(RTSim::Task &task) {
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
    TaskSet taskset = read_taskset(opts["taskset"]);

    RTSim::FCFSResManager resmanager;
    sys.kernels[0]->setResManager(&resmanager);

    for (auto &t : taskset) {
        sys.kernels[0]->addTask(*t, "");
        for (auto &tracer : tracers) {
            tracer.attachToTask(*t);
        }

        for (const auto &instr : t->getInstrQueue()) {
            auto waitInstr =
                dynamic_cast<const RTSim::WaitInstr *>(instr.get());

            // If instance of WaitInstr, check that the resoure is present
            if (waitInstr) {
                auto res = waitInstr->getResource();
                if (!resmanager.hasResource(res)) {
                    resmanager.addResource(res);
                }

                continue;
            }

            // Same thing for SignalInstr
            auto signalInstr =
                dynamic_cast<const RTSim::SignalInstr *>(instr.get());

            // If instance of SignalInstr, check that the resoure is present
            if (signalInstr) {
                auto res = signalInstr->getResource();
                if (!resmanager.hasResource(res)) {
                    resmanager.addResource(res);
                }

                continue;
            }
        }
    }

    try {
        simulation.run(std::stoi(opts["duration"]));
    } catch (std::exception &e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
        std::cerr << "TERMINATING!" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
