#!/bin/bash

# -------------------------------------------------------- #

# Returns the script path
function get_script_path() {
    realpath "$(dirname "${BASH_SOURCE[0]}")"
}

# Returns a default value for parallel compilation equal to
# 6/8 the number of CPUs available (if more than 1)
function get_cpus_j() {
    local j
    j=$(($(nproc) * 6 / 8))
    if [ $j -lt 1 ]; then
        echo 1
    else
        echo $j
    fi
}

# ======================================================== #
# ---------------------- ARGUMENTS ----------------------- #
# ======================================================== #

# Format: short-option [long-option] [colon]
readonly ARGUMENTS_MAPPING=(
    "h help"
    "v verbose"
    "c colorize"
    "d deb"
    "r rpm"
    "J parallel"
    "j jobs :"
    "b build-type :"
    "p build-path :"
    "G generator :"
)

# Returns a string to be passed to getopt
# Argument: 0 for short, 1 for long
function optstring() {
    local idx

    case "$1" in
    short) idx=1 ;;
    long) idx=2 ;;
    esac

    local out=""
    local a
    local c
    for arg in "${ARGUMENTS_MAPPING[@]}"; do
        a="$(echo "$arg" | cut -d' ' -f "$idx")"
        c="$(echo "$arg" | rev | cut -d' ' -f "1")"

        if [ "$c" != ":" ]; then
            c=""
        fi

        if [ -n "$a" ] && [ "$a" != ":" ] && [ "$a" != "-" ]; then
            out="${out},${a}${c}"
        fi
    done

    echo "${out:1}"
}

function options_parse_jobs() {
    PARALLEL="$1"
    if [ "$PARALLEL" -lt 1 ] 2>/dev/null; then
        echo "${BASH_SOURCE[0]}: option 'j': expected an integer, found '$1'" >&2
        false
    fi
}

function BUILD_TYPE() {
    case "$1" in
    debug) BUILD_TYPE='Debug' ;;
    release) BUILD_TYPE='Release' ;;
    release-wdebug) BUILD_TYPE='RelWithDebInfo' ;;
    *)
        echo "${BASH_SOURCE[0]}: option 'b': unexpected value '$1'" >&2
        false
        ;;
    esac
}

function options_parse_build_path() {
    BUILD_PATH="$1"
    if [ -z "$BUILD_PATH" ] || [[ "$BUILD_PATH" == -* ]]; then
        echo "${BASH_SOURCE[0]}: option 'p': unexpected value '$1'" >&2
        false
    fi
}

function options_parse_generator() {
    GENERATOR="$1"
    if [ -z "$GENERATOR" ] || [[ "$GENERATOR" == -* ]]; then
        echo "${BASH_SOURCE[0]}: option 'G': unexpected value '$1'" >&2
        false
    fi

    if [ "$GENERATOR" = 'Ninja' ] && ! command -v ninja &>/dev/null; then
        # Fallback to Unix Makefiles on Linux
        echo "Ninja not found, falling back on Linux Makefiles..."
        GENERATOR='Unix Makefiles'
    fi
}

function options_parse() {
    local VALID_ARGS

    set +e
    VALID_ARGS="$(getopt \
        --name "${BASH_SOURCE[0]}" \
        -o "$(optstring short)" \
        --long "$(optstring long)" \
        -- "$@")"

    # If something goes wrong, getopt will print the message
    # error first, so we just need to print usage and exit
    # with an error
    if [[ $? -ne 0 ]]; then
        set -e
        echo "" >&2
        do_usage
        false
    fi

    # After this command, VALID_ARGS contains only valid
    # arguments in the format: all options -- all commands
    set -e
    eval set -- "$VALID_ARGS"

    # Variables that will contain all command-line arguments
    COMMANDS=()
    VERBOSE=OFF
    COLORIZE=OFF
    PARALLEL=1

    PACKAGE_DEB=OFF
    PACKAGE_RPM=OFF

    BUILD_TYPE=Release
    BUILD_PATH="$DIR_SCRIPT/build"
    GENERATOR=
    options_parse_generator "Ninja"

    OPTIONS_ATLEASTONE=n

    while true; do
        case "$1" in
        -h | --help)
            COMMANDS=("help")
            return 0
            ;;
        -v | --verbose)
            VERBOSE=ON
            ;;
        -c | --colorize)
            COLORIZE=ON
            ;;
        -d | --deb)
            PACKAGE_DEB=ON
            ;;
        -r | --rpm)
            PACKAGE_RPM=ON
            ;;
        -J | --parallel)
            PARALLEL="$(get_cpus_j)"
            ;;
        -j | --jobs)
            options_parse_jobs "$2"
            shift
            ;;
        -b | --build-type)
            BUILD_TYPE "$2"
            shift
            ;;
        -p | --build-path)
            options_parse_build_path "$2"
            shift
            ;;
        -G | --generator)
            options_parse_generator "$2"
            shift
            ;;
        '--')
            # End of option arguments
            shift
            break
            ;;
        *)
            echo "Error: unrecognized option $1" >&2
            do_usage
            false
            ;;
        esac
        OPTIONS_ATLEASTONE=y
        shift
    done

    # Remaining arguments are positional
    COMMANDS+=("$@")
}

function options_commit() {
    readonly COMMANDS
    readonly VERBOSE
    readonly COLORIZE
    readonly PARALLEL

    readonly PACKAGE_DEB
    readonly PACKAGE_RPM

    readonly BUILD_TYPE
    readonly BUILD_PATH
    readonly GENERATOR
}

# ======================================================== #
# ----------------------- COMMANDS ----------------------- #
# ======================================================== #

function do_usage() {
    cat <<EOF
usage: ${BASH_SOURCE[0]} [options] COMMAND [...COMMANDS]

Runs the specified list of commands using the given arguments

Valid commands:
    help                            Prints this help message and exits
    build                           (Re-)Builds the project
    clean                           Cleans the project build directory
    configure                       (Re-)Configures the project
    install                         (Re-)Installs the project
    package                         Generates the desired packages (see options)
    uninstall                       Removes the installed files from paths
    test                            Runs automated testing

Multiple commands are executed in order, except 'help',
which will always be the only one executed if included.

Valid options (all optional):
    -h, --help                      Prints this help message and exits
    -v, --verbose                   Prints more info during execution
    -c, --colorize                  Forces compiler output to be ANSI-colored

    -d, --deb                       Enables the generation of the deb package
    -r, --rpm                       Enables the generation of the rpm package

    -G [gen], --generator [gen]     Uses the provided CMake generator to build
                                    the project

    -J, --parallel                  Enables parallel compilation with $(get_cpus_j)
                                    processes
    -j [jobs], --jobs [jobs]        Enables parallel compilation with [jobs]
                                    processes

    -b [target], --build-type [target]
                                    Specifies which version of the project to
                                    build (default: release, see below)

    -p [buildpath], --build-path [buildpath]
                                    Specifies which path to use to build the
                                    project (default: build)

Available build types:
    release                         release build (default)
    debug                           debug build
    release-wdebug                  release build with debug information

EOF

    # TODO: multiple-verbosity levels?
}

function do_reset_ran() {
    RAN_BUILD=0
    RAN_CONFIGURE=0
    RAN_INSTALL=0
    RAN_PACKAGE=0
    RAN_UNINSTALL=0
    RAN_TEST=0
}

function do_build() {
    if [ "$RAN_BUILD" = 1 ]; then
        return 0
    fi

    do_configure
    cmake --build "$BUILD_PATH" --parallel "$PARALLEL"
    RAN_BUILD=1
}

function do_clean() {
    rm -rf "$BUILD_PATH"
    do_reset_ran
}

function do_configure() {
    if [ "$RAN_CONFIGURE" = 1 ]; then
        return 0
    fi

    cmake -S "$DIR_PROJ" -B "$BUILD_PATH" \
        -G "$GENERATOR" \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DCMAKE_VERBOSE_MAKEFILE:BOOL="$VERBOSE" \
        -DCPACK_ENABLE_DEB="$PACKAGE_DEB" \
        -DCPACK_ENABLE_RPM="$PACKAGE_RPM" \
        -DCMAKE_FORCE_COLORED_OUTPUT="$COLORIZE"

    RAN_CONFIGURE=1
}

function do_install() {
    if [ "$RAN_INSTALL" = 1 ]; then
        return 0
    fi

    do_build
    sudo cmake --build "$BUILD_PATH" --target install
    RAN_INSTALL=1
}

function do_uninstall() {
    if [ "$RAN_UNINSTALL" = 1 ]; then
        return 0
    fi

    # This is a bit counter-intuitive, to uninstall we need to install first!
    # What we want actually is the install manifest to be present in the build
    # path!
    local install_manifest="${BUILD_PATH}/install_manifest.txt"
    if [ ! -f "$install_manifest" ]; then
        do_install
    fi

    echo "Removing the following files:"
    cat "$install_manifest"
    echo ""
    cat "$install_manifest" | sudo xargs rm -f
    RAN_UNINSTALL=1
}

function do_package() {
    if [ "$RAN_PACKAGE" = 1 ]; then
        return 0
    fi

    do_configure
    do_build

    (
        set -e
        cd "$BUILD_PATH"
        sudo cpack
        sudo chown -R "$USER:$USER" .
    )

    # if [ "$PACKAGE_DEB" = 'ON' ]; then
    #     # TODO: sign the deb package
    #     :
    # fi

    RAN_PACKAGE=1
}

function do_runtest() {
    if [ "$RAN_TEST" = 1 ]; then
        return 0
    fi

    do_build

    (
        set -e
        cd "$BUILD_PATH"

        testargs=()

        if [ "$VERBOSE" = "ON" ]; then
            testargs+=("-V")
        else
            testargs+=("--progress")
        fi

        GTEST_COLOR=1 ctest -C "$BUILD_TYPE" "${testargs[@]}"
    )

    RAN_TEST=1
}

# ======================================================== #
# ------------------------- MAIN ------------------------- #
# ======================================================== #

function do_main() {
    # Base directories
    DIR_CUR="$(realpath "$(pwd)")"
    DIR_SCRIPT="$(get_script_path)"
    DIR_PROJ="$DIR_SCRIPT"
    # DIR_UTIL="$DIR_SCRIPT/util"

    readonly DIR_CUR
    readonly DIR_SCRIPT
    readonly DIR_PROJ
    # readonly DIR_UTIL

    # # Include util scripts (looping over find output
    # # separated by '\0')
    # while IFS= read -r -d '' infile; do
    #     source "$infile"
    # done < <(find "$DIR_UTIL" -name '*.sh' -print0)

    # # Parse env variables first
    # options_parse_env

    # Parse command line arguments then
    options_parse "$@"

    # Set in stone all options and configuration
    options_commit

    # If at least one option, but no commands are specified,
    # bad command
    if [ "${#COMMANDS[@]}" -lt 1 ] && [ "$OPTIONS_ATLEASTONE" = y ]; then
        echo "${BASH_SOURCE[0]}: no command specified!" >&2
        echo "" >&2
        do_usage
        false
    fi

    # If help specified, do that only
    if [ "${#COMMANDS[@]}" -lt 1 ] || [[ " ${COMMANDS[*]} " =~ " help " ]]; then
        do_usage
        return 0
    fi

    do_reset_ran

    local cmd
    for cmd in "${COMMANDS[@]}"; do
        case "$cmd" in
        build)
            do_build
            ;;
        clean)
            do_clean
            ;;
        configure)
            do_configure
            ;;
        install)
            do_install
            ;;
        package)
            do_package
            ;;
        test)
            do_runtest
            ;;
        uninstall)
            do_uninstall
            ;;
        usage)
            do_usage
            ;;
        *)
            echo "${BASH_SOURCE[0]}: invalid command -- '$cmd'" >&2
            echo "" >&2
            do_usage
            false
            ;;
        esac
    done
}

(
    set -e
    do_main "$@"
)
