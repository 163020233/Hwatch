#!/bin/sh
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
set -e

root_dir=$1
out_dir=${root_dir}/output/3322/acore/pke_rom
create_hex_py=${root_dir}/build/script/utils/create_hex.py
objcopy_tool=${root_dir}/tools/bin/compiler/linx/bisheng_mobile_mcu_std/bisheng_mobile_mcu_std_202_0_0_b005/linx-llvm-binary-release-musl/bin/riscv32/riscv32-linux-musl-objcopy
gcc_tool=${root_dir}/tools/bin/compiler/linx/bisheng_mobile_mcu_std/bisheng_mobile_mcu_std_202_0_0_b005/linx-llvm-binary-release-musl/bin/clang-15
rom_lib_c=${root_dir}/bootloader/bootrom_3322/drivers/hal/security_unified/hal_cipher/pke/rom_lib.c
td_type_h=${root_dir}/bootloader/bootrom_3322/middleware/utils/common_headers/native/td_type.h

if [ ! -d ${out_dir} ];then
    mkdir ${out_dir}
fi

cat ${rom_lib_c}|grep -Pzo ".*g_instr_rom\[\]\s*=\s*\{[\s\S]+\}\s*;"|sed 's/\x0//g' > ${out_dir}/rom_lib.c
rom_lib_file_path=${out_dir}/rom_lib.o
${gcc_tool} -include ${td_type_h} -c ${out_dir}/rom_lib.c -o ${rom_lib_file_path}
if [ -f ${rom_lib_file_path} ]; then
    ${objcopy_tool} -O binary -j "\.rodata" ${rom_lib_file_path} ${out_dir}/pke_rom_raw.bin
    tr '\000' '\377' < /dev/zero|dd of=${out_dir}/pke_rom.bin bs=4k count=1
    dd if=${out_dir}/pke_rom_raw.bin of=${out_dir}/pke_rom.bin seek=0 conv=notrunc

    # generate hex
    python3 ${create_hex_py} ${out_dir}/pke_rom.bin ${out_dir}/pke_rom.hex
fi


exit 0
