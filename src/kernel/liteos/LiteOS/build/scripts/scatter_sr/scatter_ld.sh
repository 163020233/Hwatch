#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
# Description: scatter ld

set -e

ld_dir="$1"
scatter_dir="$2"

# add include
sed -i '/\.rodata/d' "${ld_dir}"/scatter.ld
sed -i '/\.text/d' "${ld_dir}"/scatter.ld
sed -i '/\.data/d' "${ld_dir}"/scatter.ld

if [ -f "${scatter_dir}"/scatter.O ]; then
# fast_rodata
sed -i '/.fast_rodata ALIGN(0x1000) : /a\scatter.O(.rodata*); scatter.O(.constdata*);' "${ld_dir}"/scatter.ld

# scatter_text
sed -i '/.fast_text ALIGN(0x4) : /a\scatter.O(.text*); scatter.O(ch07_3);' "${ld_dir}"/scatter.ld

# fast_data
sed -i '/.fast_data ALIGN(0x4) : /a\ . = ALIGN(0x4); KEEP(scatter.O(SORT(.liteos.table.*.wow.*))); KEEP(scatter.O(SORT(.liteos.table.*.scatter.*))); . = ALIGN(0x4); scatter.O(.data*);' "${ld_dir}"/scatter.ld
fi
