#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
# Description: script to build and run pgnss decoder demo

set -e

rm -rf demo_data/output1
rm -rf demo_data/output2

mkdir -p demo_data/output1
mkdir -p demo_data/output2

exe_name=decode_demo

gcc -o ${exe_name} server_pgnss_decode_demo.c -ldl

chmod +x ${exe_name}

./${exe_name}

ret=$?

if [ $ret -eq 0 ]; then
    echo "run ${exe_name} succeed"
else
    echo "run ${exe_name} failed"
    exit 1
fi
