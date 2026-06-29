#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
# Description: Get Compiler Version String.

declare re="[0-9]+\.[0-9]+\.[0-9]+"
if [[ $($1 --version) =~ $re ]]; then
    echo ${BASH_REMATCH[0]}
fi