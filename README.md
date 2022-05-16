# PARTSim

A Power-Aware Real-Time System Simulator.

The name derives from the fact that this is a fork of RTSim, originally designed and developed by Giuseppe Lipari.

## Dependencies

Apart from the build system dependencies like CMake, this library does not have any mandatory custom dependency.

### Optional Dependencies

The experimental thermal model implementation (excluded by default in the build process) depends on [Eigen3][eigen3]. Compiling it on your machine requires both library and headers installed.

On Debian-based distributions, you can install the Eigen3 library and headers by running:
```bash
sudo apt update
sudo apt install libeigen3-dev -y
```

## Building
Building PARTSim requires [CMake][cmake], optionally using [Ninja][ninja] as a CMake generator (default), making compilation time pretty fast. Ninja, however, is not required, with GNU Make set as fall back generator if Ninja is missing.

You don't have to issue CMake commands to build PARTSim. A (Python-based) builder script is provided to simplify the build process. The script invokes CMake commands with suitable options, handling all sorts of situations for you.

To build the simulator using the builder script with multi-threaded compilation enabled, run:
```bash
./tools/builder.py -c -J build
```

The builder script accepts many options and commands. Use the `--help` option (or run the script with no option/command) to see a list of accepted arguments ([see below](#build-options)).

### Using the EnergyMRTKernel

The EnergyMRTKernel is not supported anymore and maintained in the code base for now just for practicality. Users should not look at it or even get remotely close to it.

### Enabling the Thermal Model

If you want to build the experimental thermal model with the PARTSim library (provided you have installed the [Eigen3 dependency](#optional-dependencies)), you must pass the option `-D RTLIB_THERMAL=ON` to the builder script.

Example:
```bash
./tools/builder.py -D RTLIB_THERMAL=ON build # [...]
```

### Build Options

Following is a (mostly) up-to-date output of the builder help:
```
$ ./tools/builder.py -h
usage: ./tools/builder.py [-h] [-v] [-c] [-d] [-r] [-G {Ninja,Unix Makefiles}] [-J] [-j JOBS]
                          [-b {release,debug,release-wdebug}] [-p BUILD_PATH] [-D CMAKE_OPTION]
                          [COMMAND ...]

Multiple commands are executed in order, except 'help', which will always be the only one executed
if included.

Positional arguments:
  COMMAND                                         List of one or more commands to run sequentially

List of valid commands:
  help                                            Prints this help message and exits
  build                                           (Re-)Builds the project
  clean                                           Cleans the project build directory
  configure                                       (Re-)Configures the project
  install                                         (Re-)Installs the project
  package                                         Generates the desired packages (see options)
  uninstall                                       Removes the installed files from paths
  test                                            Runs automated testing

Valid options (all optional):
  -h, --help                                      Prints this help message and exits
  -v, --verbose                                   Prints more info during execution
  -c, --colorize                                  Forces compiler output to be ANSI-colored
  -d, --package-deb                               Enables the generation of the deb package
  -r, --package-rpm                               Enables the generation of the rpm package
  -G {Ninja,Unix Makefiles}, --generator {Ninja,Unix Makefiles}
                                                  Uses the provided CMake generator to build the
                                                  project (default: Ninja)
  -J, --parallel                                  Enables parallel compilation with 6 processes
  -j JOBS, --jobs JOBS                            Enables parallel compilation with JOBS processes
  -b {release,debug,release-wdebug}, --build-type {release,debug,release-wdebug}
                                                  Specifies which version of the project to build
                                                  (default: release)
  -p BUILD_PATH, --build-path BUILD_PATH          Specifies which path to use to build the project
                                                  (default: build)
  -D CMAKE_OPTION, --cmake-option CMAKE_OPTION    Accepts OPTION={ON|OFF} values to forward to
                                                  cmake
```

<!-- Links -->

[cmake]: https://cmake.org/cmake/help/latest/
[ninja]: https://ninja-build.org/
[eigen3]: https://eigen.tuxfamily.org/index.php?title=Main_Page
