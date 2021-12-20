# PARTSim

A Power Aware Real-Time System Simulator.

The name derives from the fact that this is a fork of RTSim, originally designed
and developed by Giuseppe Lipari.

## Dependencies

Apart from the build system dependencies like CMake, this library does not have
any custom mandatory dependency.

If you wish to use in your system the thermal model implementation included
(experimental) you do need to have Eigen3 library and headers installed on your
machine. Also, you need to enable the option to build it in
`rtlib/CMakeLists.txt` by setting `RTLIB_THERMAL` to `ON`. For now, the builder
script does not support setting this option from the command line (that is,
unless you want to run `cmake` commands on your own).

On Debian-based distributions, you can install Eigen3 library and headers by
running:
```bash
sudo apt update
sudo apt install libeigen3-dev -y
```

## Building

Building PARTSim requires CMake. If you also have Ninja Build System installed
on your machine it will use that one to be faster by default, but it can
automatically fall-back to GNU Make as a backend if not available.

To build the simulator on Linux use
```sh
./m -c -J build
```

Use the `--help` flag (or simply `./m`) to see the different options of the
`build` command.
