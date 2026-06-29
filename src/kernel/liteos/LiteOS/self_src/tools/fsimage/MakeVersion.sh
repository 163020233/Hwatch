#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2022. All rights reserved.
# Description: make images for jffs2 and yaffs
set -e

yaffs_tool_dir="$1"
out_dir="$2"

if [ "$3" ]; then
    "${yaffs_tool_dir}"/mkyaffs2image "${yaffs_tool_dir}"/rootfs "${out_dir}"/rootfs_mn34220_30fps.yaffs2 $3 4 # yaffs2 filesystem image pagesize parameter base on caller
else
    "${yaffs_tool_dir}"/mkyaffs2image "${yaffs_tool_dir}"/rootfs "${out_dir}"/rootfs_mn34220_30fps.yaffs2 2 4 # yaffs2 filesystem image default pagesize 4k
fi
    "${yaffs_tool_dir}"/mkfs.ftl "${yaffs_tool_dir}"/rootfs/ 0x400000 0x10000 0x200 "${out_dir}"/appfs.ftl

    "${yaffs_tool_dir}"/mkfs.ftl "${yaffs_tool_dir}"/rootfs/ 0x400000 0x10000 0x200 "${out_dir}"/appfsd.ftl -d