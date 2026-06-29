#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
# Description: Diff files.

set -e
src="$1"
dst="$2"

if [ ! -f "${src}" ];then
    echo "file ${src} not exist"
    exit 1;
fi

if [ ! -f "${dst}" ];then
    echo "file ${dst} not exist"
    exit 1;
fi

if diff -q "${dst}" "${src}"; then
    exit 0;
else
    echo "different file: ${src} ${dst}";
    exit 1;
fi
