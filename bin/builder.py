#!/usr/bin/env python3
"""
Runs the specified list of commands using the given arguments.
"""

import argparse
import sys
import os

# -------------------------------------------------------- #
#                   Classes for Argparse                   #
# -------------------------------------------------------- #


class DefaultNotNoneHelpFormatter(
        argparse.ArgumentDefaultsHelpFormatter, ):

    def _get_help_string(self, action):
        help = action.help
        if '%(default)' not in action.help:
            if action.default is not argparse.SUPPRESS:
                defaulting_nargs = [argparse.OPTIONAL, argparse.ZERO_OR_MORE]
                if action.option_strings or action.nargs in defaulting_nargs:
                    if action.default is not None and action.default is not False:
                        help += ' (default: %(default)s)'
        return help


# Source: https://stackoverflow.com/a/49999185
class NoAction(argparse.Action):

    def __init__(self, **kwargs):
        kwargs.setdefault('default', argparse.SUPPRESS)
        kwargs.setdefault('nargs', 0)
        super(NoAction, self).__init__(**kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        pass


# +--------------------------------------------------------+
# |                        Actions                         |
# +--------------------------------------------------------+

RAN = []


def runcmd(cmd, quiet=False, exit=True):
    if isinstance(cmd, list):
        cmd = ' '.join(cmd)
    if quiet:
        cmd += ' >/dev/null'
    e = os.system(cmd)
    if exit and e:
        sys.exit(int(e))
    return e


def do_build(options):
    do_configure(options)

    cmd = [
        "cmake",
        "--build '{}'".format(options.build_path),
        "--parallel '{}'".format(options.jobs),
    ]
    runcmd(cmd)


def do_clean(options):
    global RAN
    cmd = "rm -rf '{}'".format(options.build_path)
    runcmd(cmd)
    RAN.clear()


def do_configure(options):
    cmd = [
        "cmake",
        "-S '{}'".format(options.dir_project),
        "-B '{}'".format(options.build_path),
        "-G '{}'".format(options.generator),
        "-DCMAKE_BUILD_TYPE='{}'".format(options.build_type),
        "-DCMAKE_VERBOSE_MAKEFILE:BOOL='{}'".format(options.verbose),
        "-DCPACK_ENABLE_DEB='{}'".format(options.package_deb),
        "-DCPACK_ENABLE_RPM='{}'".format(options.package_rpm),
        "-DCMAKE_FORCE_COLORED_OUTPUT='{}'".format(options.colorize),
    ]

    runcmd(cmd)


def do_install(options):
    do_build(options)

    cmd = [
        "sudo",
        "cmake",
        "--build '{}'".format(options.build_path),
        "--target install",
    ]
    runcmd(cmd)


def do_uninstall(options):
    # This is a bit counter-intuitive, to uninstall we need to install first!
    # What we want actually is the install manifest to be present in the build
    # path!
    manifest_fname = '{}/install_manifest.txt'.format(options.build_path)
    from pathlib import Path
    path = Path(manifest_fname)
    if not path.is_file():
        do_install(options)

    print("Removing the following files:")
    with open(manifest_fname, 'r') as f:
        print(f.read())

    # Clearly this would not work!
    cmd = [
        "cat",
        manifest_fname,
        "|",
        "sudo xargs rm -f",
    ]

    runcmd(cmd)


def do_package(options):
    do_build(options)

    user = os.getenv('USER')

    cwd = os.getcwd()
    os.chdir(options.build_path)
    runcmd("sudo cpack")
    runcmd("sudo chown -R {}:{}".format(user, user))
    os.chdir(cwd)

    # if [ "$PACKAGE_DEB" = 'ON' ]; then
    #     # TODO: sign the deb package
    #     :
    # fi


def do_test(options):
    do_build(options)

    cmd = [
        '{} ctest'.format('GTEST_COLOR=1' if options.colorize else ''),
        "-C '{}".format(options.build_type),
        '-V' if options.verbose else '--progress',
    ]

    cwd = os.getcwd()
    os.chdir(options.build_path)
    runcmd(cmd)
    os.chdir(cwd)


COMMANDS = {
    'build': {
        'help': '(Re-)Builds the project',
        'action': do_build
    },
    'clean': {
        'help': 'Cleans the project build directory',
        'action': do_clean
    },
    'configure': {
        'help': '(Re-)Configures the project',
        'action': do_configure
    },
    'install': {
        'help': '(Re-)Installs the project',
        'action': do_install
    },
    'package': {
        'help': 'Generates the desired packages (see options)',
        'action': do_package
    },
    'uninstall': {
        'help': 'Removes the installed files from paths',
        'action': do_uninstall
    },
    'test': {
        'help': 'Runs automated testing',
        'action': do_test
    }
}

# +--------------------------------------------------------+
# |                  Command-Line Checks                   |
# +--------------------------------------------------------+


def max_cpus():
    return len(os.sched_getaffinity(0))


def check_cpus(v):
    v = int(v)
    if v <= 0:
        raise argparse.ArgumentTypeError(
            "'{}' is not a valid number of CPUs".format(v))

    if v > max_cpus():
        return max_cpus()
    return v


def get_cpus_j():
    return int(max_cpus() * 6 / 8)


# +--------------------------------------------------------+
# |          Command-line Arguments Configuration          |
# +--------------------------------------------------------+


def arguments_parser():
    fmter = lambda prog: DefaultNotNoneHelpFormatter(
        prog,
        max_help_position=50,
        # width=100,
    )

    parser = argparse.ArgumentParser(
        # description=config['description'],
        # usage=config['usage'],
        description=
        'Multiple commands are executed in order, except \'help\', which will always be the only one executed if included.',
        formatter_class=fmter,
        add_help=False,
    )

    # Used for the commands in the list
    parser.register('action', 'none', NoAction)

    global COMMANDS

    # Append help at the beginning of the list
    help_help = 'Prints this help message and exits'
    COMMANDS = {
        'help': {
            'help': help_help,
            'action': None,
        },
        **COMMANDS,
    }

    # Generate commands list
    commands_only_group = parser.add_argument_group('Positional arguments')
    commands_only_group.add_argument(
        'commands',
        nargs='*',
        metavar='COMMAND',
        choices=COMMANDS.keys(),
        help='List of one or more commands to run sequentially',
    )

    # Append each command to list its help
    commands_group = parser.add_argument_group(title='List of valid commands')
    for cmdname in COMMANDS.keys():
        commands_group.add_argument(
            cmdname,
            help=COMMANDS[cmdname]['help'],
            action='none',
        )

    # All optional parameters are listed here
    optional = parser.add_argument_group('Valid options (all optional)')
    optional.add_argument(
        '-h',
        '--help',
        help=help_help,
        action='help',
        default=argparse.SUPPRESS,
    )
    optional.add_argument(
        '-v',
        '--verbose',
        help='Prints more info during execution',
        action='count',
    )
    optional.add_argument(
        '-c',
        '--colorize',
        help='Forces compiler output to be ANSI-colored',
        action='store_true',
    )
    optional.add_argument(
        '-d',
        '--package-deb',
        help='Enables the generation of the deb package',
        action='store_true',
    )
    optional.add_argument(
        '-r',
        '--package-rpm',
        help='Enables the generation of the rpm package',
        action='store_true',
    )
    optional.add_argument(
        '-G',
        '--generator',
        help='Uses the provided CMake generator to build the project',
        type=str,
        choices=[
            'Ninja',
            'Unix Makefiles',
        ],
        default='Ninja',
    )
    optional.add_argument(
        '-J',
        '--parallel',
        help='Enables parallel compilation with {} processes'.format(
            get_cpus_j()),
        action='store_true',
    )
    optional.add_argument(
        '-j',
        '--jobs',
        help='Enables parallel compilation with JOBS processes',
        type=check_cpus)
    optional.add_argument(
        '-b',
        '--build-type',
        help='Specifies which version of the project to build',
        choices=[
            'release',
            'debug',
            'release-wdebug',
        ],
        default='release',
    )
    optional.add_argument(
        '-p',
        '--build-path',
        help='Specifies which path to use to build the project',
        default='build')

    return parser


# +--------------------------------------------------------+
# |                          Main                          |
# +--------------------------------------------------------+


def onoff(v):
    return 'ON' if v else 'OFF'


def check_generator(g):
    if g == 'Ninja' and runcmd('command -v ninja', quiet=True, exit=False):
        print('Ninja not found, falling back to Unix Makefiles generator...')
        return 'Unix Makefiles'
    return g


def main():
    parser = arguments_parser()

    if len(sys.argv) == 1:
        parser.print_help(sys.stdout)
        sys.exit(0)

    options = parser.parse_args()

    if 'help' in options.commands:
        parser.print_help(sys.stdout)
        sys.exit(0)

    if options.parallel and not options.jobs:
        options.jobs = get_cpus_j()

    if not options.jobs:
        options.jobs = 1

    options.verbose = onoff(options.verbose)
    options.colorize = onoff(options.colorize)
    options.package_deb = onoff(options.package_deb)
    options.package_rpm = onoff(options.package_rpm)
    options.generator = check_generator(options.generator)

    options.dir_project = os.path.realpath(
        os.path.dirname(os.path.realpath(__file__)) + '/..')

    global RAN
    for c in options.commands:
        if c == 'clean' or c not in RAN:
            print(" +------- Running step " + c)
            print('')
            COMMANDS[c]['action'](options)

            RAN.append(c)
    return 0


if __name__ == "__main__":
    main()
