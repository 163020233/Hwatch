#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
# Description: build libc for compiler

set -e

arch="$1"
compile="$2"
out_dir="$3"
install_dir="$4"
cur_dir=$(readlink -f $(dirname $0))

help_info()
{
    echo "eg: ./compiler_lib.sh arm arm-himix410-linux- libout libinstall"
    echo "eg: ./compiler_lib.sh aarch64 aarch64-himix210-linux- libout libinstall"
}

if [ -z "${arch}" ] || [ -z "${compile}" ] || [ -z "${out_dir}" ] || [ -z "${install_dir}" ]; then
    help_info
    exit 1
fi

arch_list="${cur_dir}/config/${arch}/arch_list.txt"
if [ ! -f "${arch_list}" ]; then
    echo "input arch ${arch} not support!"
    exit 1
fi


build_list=$(cat ${arch_list} | xargs echo)
make TARGET_OUT=${out_dir} TARGET_INSTALL=${install_dir} clean_lib;
for build_name in ${build_list}; do
    echo "building ${arch} : ${build_name}"
    make CPU_CONFIG_OPTIONS=${build_name} TARGET_OUT=${out_dir} clean_obj;
    make TARGET_COMPILER=${compile} CPU_CONFIG_OPTIONS=${build_name} TARGET_OUT=${out_dir} MAKE_COMPILER_LIBS="y" build_lib -j;
done
echo "build ${arch} done! start install_lib"
make CPU_CONFIG_OPTIONS=${arch}_generic TARGET_OUT=${out_dir} TARGET_INSTALL=${install_dir} install_lib;
echo "install_lib ${arch} done!"
