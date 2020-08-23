#!/bin/bash

mydir=$(dirname $0)

# Maximum number of tasks per taskset
NMAX=10
# Number of tasks (with vtime in the future) to be killed at random time
K=3

LD_LIBRARY_PATH="$LD_LIBRARY_PATH:../../"

e=0
for ((i=0; i<1000; i++)); do
    $mydir/edf-death-birth -n $NMAX -k $K > log.txt;
    if grep miss trace.txt > /dev/null; then
        echo "i=$i: ERR";
        mv log.txt log-$i.txt
        mv trace.txt trace-$i.txt
        e=$[$e+1]
    else
        echo "i=$i: OK";
    fi;
done

echo

if [ $e -eq 0 ]; then
    echo "All good, no misses detected!"
    exit 0
else
    echo "Error, $e misses detected!"
    exit 1
fi
