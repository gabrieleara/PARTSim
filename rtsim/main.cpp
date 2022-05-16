#include "args.hpp"

/*
 * ╔═══════════════════════════════════════════════════════╗
 * ║                         Main                          ║
 * ╚═══════════════════════════════════════════════════════╝
 */

// LibMetasim
#include <simul.hpp>

// LibRTSim
#include <json_trace.hpp>
#include <system.hpp>
#include <texttrace.hpp>

int main(int argc, char *argv[]) {
    auto opts = parse_arguments(argc, argv);

    MetaSim::Simulation &simulation = MetaSim::Simulation::getInstance();

    if (opts["enable-debug"] == "true") {
        simulation.dbg.enable("All");
        simulation.dbg.setStream("debug.txt"); // FIXME: customizable
    }

    for (auto s : list_split(opts["trace"])) {
        if (string_endswith(s, ".txt")) {
            RTSim::TextTrace ttrace(s);
            // TODO: traces should be added to all tasks
        } else if (string_endswith(s, ".json")) {
            RTSim::JSONTrace jtrace(s);
            // TODO: traces should be added to all tasks
        }
    }

    RTSim::System sys{opts["system"]};
    // TODO: RTSim::TaskSet tset{opts.fname_tasks};

    try {
        simulation.run(std::stoi(opts["duration"]));
    } catch (std::exception &e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
        std::cerr << "TERMINATING!" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/*
 * ╔═══════════════════════════════════════════════════════╗
 * ║                       Includes                        ║
 * ╚═══════════════════════════════════════════════════════╝
 */

// PeriodicTask_ptr create_fixed_task(const string &workload,
//                                    const string &cpuname, int task_index) {
//     PeriodicTask_ptr t;

//     string task_name = "Task_" + cpuname + "_" + std::to_string(task_index);

//     // Parameters:
//     // - iat: inter-arrival-time
//     // - rdl: relative deadline
//     // - ph: initial phasing for inter arrival (?)
//     // - name: task name (that was easy)
//     // - qs: queue size (default value: 100)
//     t = make_shared<PeriodicTask>(500, 100, 0, task_name);
//     t->insertCode("fixed(100," + workload + ");");

//     return t;
// }

// using Task_ptr = RTSim::TaskSet::Task_ptr;

// TaskSet create_tasks_per_core(
//     System &s, TextTrace &ttrace, JSONTrace &jtrace, const string &workload,
//     int howmany = 1,
//     int skip = 0) { // last parameter can be used to skip cores in the same
//                     // island when all islands have the same size
//     TaskSet ts;

//     for (int i = 0; i < s.cpus.size(); i += 1 + skip) {
//         for (int j = 0; j < howmany; ++j) {
//             Task_ptr t =
//                 create_fixed_task(workload, "core" + std::to_string(i), j);

//             ts.tasks.push_back(t);
//             // Second parameter is called slice parameters, I don't know what
//             it
//             // does... yet!
//             // NOTICE: the addTask wants a reference but in reality it uses
//             // pointers inside!
//             s.kernels[i]->addTask(*t, "");
//             ttrace.attachToTask(*t);
//             jtrace.attachToTask(*t);
//         }
//     }

//     return ts;
// }
