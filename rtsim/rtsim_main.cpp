#include "rtsim_args.hpp"

/*
 * ╔═══════════════════════════════════════════════════════╗
 * ║                         Main                          ║
 * ╚═══════════════════════════════════════════════════════╝
 */

#include <simul.hpp>
#include <system.hpp>

int main(int argc, char *argv[]) {
    auto opts = parse_arguments(argc, argv);

    MetaSim::Simulation &simulation = MetaSim::Simulation::getInstance();

    if (opts["enable-debug"] == "true") {
        simulation.dbg.enable("All");
        simulation.dbg.setStream("debug.txt");
    }

    for (auto s : list_split_unsafe(opts["trace"])) {
        if (string_endswith(s, ".txt")) {
            RTSim::TextTrace ttrace(s);
            // TODO: traces should be added to all tasks
        } else if (string_endswith(s, ".json")) {
            RTSim::JSONTrace jtrace(s);
            // TODO: traces should be added to all tasks
        }
    }

    // TODO: tracing options

    RTSim::System sys{opts["system"]};
    // TODO: RTSim::TaskSet tset{opts.fname_tasks};

    try {
        simulation.run(std::stoi(opts["length"])); // TODO: let user decide for how long
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

// // LibMetasim
// #include <simul.hpp>

// // LibRTSim
// #include <system.hpp>

// // Tracing
// #include <json_trace.hpp>
// #include <texttrace.hpp>

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

int main(int argc, char *argv[]) {
    // string taskset_descriptor = "tasks.yaml";
    vector<unsigned int> default_opps = {0, 0};

    try {
        Simulation &simulation = Simulation::getInstance();

        simulation.dbg.enable("All");
        simulation.dbg.setStream("debug.txt");

        TextTrace ttrace("trace.txt");
        JSONTrace jtrace("trace.json");

        // Create the system on which the taskset will run
        // TODO: let user choose which system to use
        System sys(system_descriptor);

        const string workload = "bzip2";

        // Create the corresponding taskset
        // TODO: proper taskset generation

        /*
        TaskSet tset =
            create_tasks_per_core(sys, ttrace, jtrace, workload, 1, 3);
        */

        simulation.run(5000); // TODO: let user decide for how long
    } catch (std::exception &e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
        std::cerr << "TERMINATING!" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
