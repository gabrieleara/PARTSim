/*
 * Filename: rtsim/rtsim_args.hpp
 * Path: rtsim
 * Created Date: Monday, May 16th 2022, 9:52:03 am
 * Author: Gabriele Ara
 *
 * Copyright (c) 2022 Gabriele Ara
 */

#pragma once
#ifndef RTSIM_ARGS_H_
#define RTSIM_ARGS_H_

#include <cmdarg.hpp>
#include <iostream>

static inline int list_append_unsafe(std::string *dest,
                                     const cmdarg::Argument &opt,
                                     const std::string &src) {
    *dest += "'" + src + "' ";
    return 0;
}

// TODO: escape ' characters
static inline int list_append_txt_json(std::string *dest,
                                       const cmdarg::Argument &opt,
                                       const std::string &src) {
    using cmdarg::actions::store_string;

    std::string ext = src.substr(src.length() - 4);
    if (ext == ".txt") {
        return list_append_unsafe(dest, opt, src);
    }

    ext = src.substr(src.length() - 5);
    if (ext == ".json") {
        return list_append_unsafe(dest, opt, src);
    }

    std::cerr << "Error: argument --" << opt.long_opt << "/-" << opt.short_opt
              << ": expected filename ending with 'txt' or '.json'!"
              << std::endl;
    return 1;
}

static inline cmdarg::Options parse_arguments(int argc, char *argv[]) {
    cmdarg::Parser parser{{
        .prog = argv[0],
        .description = "",
        .epilogue = "",
    }};

    parser.addArgument({
        .long_opt = "system",
        .required = true,
        .help = "The YAML file description for the system",
    });
    parser.addArgument({
        .long_opt = "taskset",
        .required = true,
        .help = "The file containing the taskset description",
    });
    parser.addArgument({
        .long_opt = "duration",
        .required = true,
        .help = "The duration of the simulation in time units",
    });
    parser.addArgument({
        .long_opt = "trace",
        .short_opt = 't',
        .required = false,
        .parameter_required = cmdarg::Argument::ParameterRequired::REQUIRED,
        .help = "The file name where to store a trace (either txt or json)",
        .action = list_append_txt_json,
    });
    parser.addArgument({
        .long_opt = "enable-debug",
        .short_opt = 'd',
        .required = false,
        .parameter_required = cmdarg::Argument::ParameterRequired::NO,
        .help = "Enables debug prints throughout the simulation",
        .default_value = "false",
        .action = cmdarg::actions::store_true,
    });

    return parser.parse(argc, argv);
}

#endif // RTSIM_ARGS_H_
