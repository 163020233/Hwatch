#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
# Description: sym

set -e
so_dir="$1"
sym_dir="$2"
reverse="$3"
isolated="$4"
cur_root="$(pwd)"
build_dir="${sym_dir}"/../build

mkdir -p "${sym_dir}"
"${cur_root}"/dynload_sh/make_symlst.py "${so_dir}" > "${sym_dir}"/symbol.list
"${cur_root}"/dynload_sh/make_symcfile.sh "${sym_dir}"/symbol.list "${sym_dir}"/los_dynload_gsymbol.c "$isolated"
"${cur_root}"/dynload_sh/make_ldflags.sh "${build_dir}" "${sym_dir}"
