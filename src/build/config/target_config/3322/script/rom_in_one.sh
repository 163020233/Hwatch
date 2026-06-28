#!/bin/sh
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
set -e

core=$1
root_dir=$2
bin_name=$3
target_name=$4

out_dir=${root_dir}/output/3322/${core}/${target_name}
romboot_bin=${root_dir}/output/3322/acore/3322-romboot/romboot.bin
create_8byte_align_hex_py=${root_dir}/build/config/target_config/3322/script/create_hex_8byte_align.py
create_4byte_align_hex_py=${root_dir}/build/script/utils/create_hex.py

bt_rom_bin=${out_dir}/bt_rom.bin

if [ "${core}" = "acore" -a "${target_name}" = "3322-romboot" ];then
    if [ ! -f ${romboot_bin} ];then
        echo "[*] romboot not found, codepoint files will not be one!"
        exit 0
    fi

    pushd ${out_dir}
    # Fill romboot bin to 64K
    dd if=${romboot_bin} of=romboot.64K.bin bs=64K seek=0 count=1 conv=sync

    # All in one rompack file
    python3 ${create_8byte_align_hex_py} romboot.64K.bin ${bin_name}_rompack.hex

    popd
fi

if [ "${core}" = "bt_core" ];then
    if [ ! -f ${bt_rom_bin} ];then
        echo "[*] bt_rom not found, codepoint files will not be one!"
        exit 0
    fi
    pushd ${out_dir}
    # Fill bt_rom bin to 448K
    dd if=${bt_rom_bin} of=bt_rom.448K.bin bs=448K seek=0 count=1 conv=sync

    # All in one rompack file
    python3 ${create_4byte_align_hex_py} bt_rom.448K.bin ${bin_name}_rompack.hex

    popd

fi

