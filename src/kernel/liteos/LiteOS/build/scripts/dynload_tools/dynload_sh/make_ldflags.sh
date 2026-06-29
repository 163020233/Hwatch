#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
# Description: make ldflags

set -e
build_dir="$1"
sym_dir="$2"
mkdir -p "${build_dir}"

echo "LITEOS_DYNLDFLAGS += \$(OUT)/dynload_dir/dynload_sym/los_dynload_gsymbol.o" >"${build_dir}"/dynload_ld.mk
echo "LITEOS_DYNLOADOPTS := -ug_liteGlobalSymTab" >>"${build_dir}"/dynload_ld.mk

echo "set(LITEOS_DYNLDFLAGS \${OUT}/dynload_dir/dynload_sym/los_dynload_gsymbol.o)" >"${build_dir}"/dynload_ld.cmake
echo "set(LITEOS_DYNLOADOPTS -ug_liteGlobalSymTab)" >>"${build_dir}"/dynload_ld.cmake
