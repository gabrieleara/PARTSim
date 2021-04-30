#!/bin/bash

(
    compile="cc --std=c++14 -I. -c"

    green="$(tput setaf 2)"
    red="$(tput setaf 1)"
    normal="$(tput sgr0)"

    greentick="${green}\u2714${normal}"
    redcross="${red}\u2717${normal}"

    headers=()

    if [ $# -gt 0 ] ; then
        headers=($@)
    else
        headers=( $(find . -name '*.hpp') )
    fi

    for h in ${headers[*]}; do
        ${compile} "$h" -o /tmp/tmp.o &>/dev/null &&
            printf " [${greentick}] %-20s compilation succeded!\n" "$h" ||
            printf " [${redcross}] %-20s failed compilation!\n" "$h"
    done
)

rm -f /tmp/tmp.o
