#!/bin/bash
//===============================================================================
// @brief    source file
// Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
//===============================================================================

# @shell multi_binaries.sh
# copyright(c), 2020 - 2020, huawei tech. co., ltd. ALL RIGHTS RESERVED.
# @brief handle LiteOS head file(.h)
# @version 1.0
set -e

out_dir="$1"
cross_compile="$2"
path_libgcc="$3"
path_topdir="$4"
ar="${cross_compile}"ar
ld="${cross_compile}"ld

echo "out_dir: $1"

cd "$1"/lib

mkdir -p multi_binaries
mkdir -p multi_binaries/kernel
mkdir -p multi_binaries/libgcc
mkdir -p multi_binaries/libsec
mkdir -p multi_binaries/libc
mkdir -p multi_binaries/bsp
cp "$1"/lib/{libbase.a,libriscv.a} "$1"/lib/multi_binaries/kernel
cp $path_libgcc/libgcc.a "$1"/lib/multi_binaries/libgcc
cp "$1"/lib/libsec.a "$1"/lib/multi_binaries/libsec
cp "$1"/lib/{libc.a,libposix.a,libcsysdeps.a} "$1"/lib/multi_binaries/libc
cp "$1"/lib/{libtimer.a,libinterrupt.a,libarm_pl011.a} "$1"/lib/multi_binaries/bsp
rm -rf libbase.o
rm -rf libsec_rom.o
rm -rf libc_rom.o
rm -rf libbsp_rom.o

cd "$1"/lib/multi_binaries/kernel
$ar -x libbase.a
$ar -x libriscv.a

rm -rf fpb.o
rm -rf dynload.o
rm -rf reset_vector.o
all=($(find . -name "*.o"))
echo ${all[@]}

$ld -r -o libbase.o ${all[@]}
cp libbase.o ../

cd "$1"/lib/multi_binaries/libgcc
$ar -x libgcc.a
$ld -r -o libgcc_rom.o save-restore.o _clzsi2.o _udivdi3.o _ashldi3.o _ctzsi2.o divdf3.o muldf3.o floatsidf.o floatunsidf.o fixunsdfdi.o _divdi3.o _moddi3.o _clz.o _udivmoddi4.o floatdidf.o _umoddi3.o
cp libgcc_rom.o ../

cd "$1"/lib/multi_binaries/libsec
$ar -x libsec.a
$ld -r -o libsec_rom.o memcpy_s.o strncpy_s.o memset_s.o
cp libsec_rom.o ../

cd "$1"/lib/multi_binaries/libc
$ar -x libc.a
$ar -x libcsysdeps.a
$ar -x libposix.a
$ld -r -o libc_rom.o strlen.o memcpy.o memset.o errno.o rand.o time.o
cp libc_rom.o ../

cd "$1"/lib/multi_binaries/bsp
$ar -x libtimer.a
$ar -x libinterrupt.a
$ar -x libarm_pl011.a
$ld -r -o libbsp_rom.o arm_sp804_timer.o riscv_himideerv200_plic.o arm_pl011_core.o
cp libbsp_rom.o ../

cd "$1"/lib/multi_binaries
rm -rf ./kernel ./libgcc ./libsec ./libc ./bsp

cd "$1"/lib
cp multi_binaries/libbase.o libbase.o
cp multi_binaries/libgcc_rom.o libgcc_rom.o
cp multi_binaries/libsec_rom.o libsec_rom.o
cp multi_binaries/libc_rom.o libc_rom.o
cp multi_binaries/libbsp_rom.o libbsp_rom.o

cd "$path_topdir"
