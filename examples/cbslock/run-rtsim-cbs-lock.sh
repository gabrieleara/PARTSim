#!/bin/bash

cat > /tmp/hw.yml <<EOF
cpu_islands:
  - name: island0
    numcpus: 1
    kernel:
      scheduler: edf
      task_placement: partitioned
    volts:
      [1.000000]
    freqs:
      [1000]
    base_freq: 1000
    power_model: island0_psm
    speed_model: island0_psm

power_models:
  - name: island0_psm
    type: table_based
    filename: /tmp/hw_psm.csv
EOF

cat > /tmp/hw_psm.csv <<EOF
model,freq,workload,power,speed
island0_psm,1000,bzip2,0.504400,1.000000
island0_psm,1000,idle,0.099480,1.000000
EOF

cat > /tmp/sw.yml <<EOF
taskset:
  - name: task_0_0
    iat: 100
    runtime: 10
    rdl: 20
    startcpu: 0
    code:
      - lock(L_DAG_0)
      - fixed(15,bzip2)
      - unlock(L_0_0_1)
  - name: task_0_1
    iat: 100
    runtime: 10
    rdl: 20
    startcpu: 0
    code:
      - lock(L_0_0_1)
      - fixed(15,bzip2)
      - unlock(L_DAG_0)
resources:
  - name: L_DAG_0
    initial_state: unlocked
  - name: L_0_0_1
    initial_state: locked
EOF

rtsim -d -t cbs-lock_trace.txt /tmp/hw.yml /tmp/sw.yml 200
