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
    deadline: 100
    cbs_runtime: 10
    cbs_period: 20
    startcpu: 0
    code:
      - fixed(15,bzip2)
  - name: task_0_1
    iat: 100
    deadline: 100
    cbs_runtime: 10
    cbs_period: 20
    startcpu: 0
    code:
      - suspend(10)
      - fixed(15,bzip2)
EOF

rtsim -d -t cbs-susp_trace.txt /tmp/hw.yml /tmp/sw.yml 300
