#!/usr/local/bin/python3.7
# -*- coding: utf-8 -*-
#===============================================================================
# @brief   Python script
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================

import os
import sys
import time
import json
import struct
import shutil
from distutils.dir_util import copy_tree

"""
介绍：
    本脚本通过调用mklittlefs工具生成文件镜像，请提前准备好放入本目录
    请注意使用本脚本时支持的python版本，本脚本只支持python3版本。
"""

root_path = os.path.join(os.path.split(os.path.realpath(__file__))[0], '..', '..', '..', '..', '..')
cur_dir = os.path.dirname(os.path.abspath(__file__))
image_tool_path = os.path.join(cur_dir, 'mklittlefs')   # 需要将镜像工具mklittlefs放入当前目录
out_path = os.path.join(root_path, 'output', '3322')

# gnss
tools_bin_path = os.path.join(root_path, 'tools', 'pkg', 'bin', '3322')
gnss_bin_path = os.path.join(tools_bin_path, 'gnss', 'firmwares')

# 配置生成文件镜像工具路径
image_name = "lfs_file.bin"

# nor页大小配置
cfg_page_size = "0x1000"

# 从分区表中获取文件系统的大小
def get_partition_len_by_name(id_name):
    partition_dir = os.path.join(root_path, "build", "config", "target_config", "3322", "param_sector", "param_16m_s_norflash_sector.json")

    try:
        # 加载JSON数据
        with open(partition_dir, 'r') as file:
            data = json.load(file)

        # 查找id_name对应的序号
        fixed_item_ids = data['Partition_Tbl']['Fixed item id']
        index = None
        for item in fixed_item_ids:
            id_hex, name = item.split(': ')
            if name == id_name:
                index = id_hex
                break

        print("index:", index)
        if index is None:
            return None

        # 获取param_info列表
        param_info = data['Partition_Tbl']['param_info']

        # 查找对应的起始地址和长度
        for item in param_info:
            if item[0] == index:
                return item[2]

    except Exception as e:
        print(f"Error: {e}")
    return None


def copy_file():
    #清除gnss文件
    if os.path.exists(os.path.join(cur_dir, 'fs', 'gnss')):
        shutil.rmtree(os.path.join(cur_dir, 'fs', 'gnss'), ignore_errors = True)
    os.makedirs(os.path.join(cur_dir, 'fs'), exist_ok=True)

    bin_mapping = [
        (os.path.join(gnss_bin_path, 'gnss_config.bin'), os.path.join(cur_dir, 'fs', 'gnss', 'firmware')),
    ]

    for src_path, dst_dir in bin_mapping:
        if os.path.exists(src_path):
            if not os.path.exists(dst_dir):
                os.makedirs(dst_dir)
            file_name = os.path.basename(src_path)
            dst_path = os.path.join(dst_dir, file_name)
            shutil.copyfile(src_path, dst_path)
            print(f"成功拷贝: {src_path} -> {dst_path}")
        else:
            print(f"文件不存在: {src_path}")

def main():
    copy_file()

    image_file = os.path.join(out_path, image_name)
    cfg_image_size = get_partition_len_by_name("filesystem")
    print("image file:", image_file, "image_size:", cfg_image_size)
    if cfg_image_size is None:
        print("not get partition info, mklfsimgtool stopped.")
        return

    if os.path.exists(image_file):
        os.remove(image_file)

    if not os.path.exists(image_tool_path):
        print(image_tool_path + " not exist, mklfsimgtool stopped.")
        return

    if sys.platform == 'linux2' or sys.platform == 'linux':
        command = image_tool_path + ' -c ' + os.path.join(cur_dir, 'fs') + ' -b ' + cfg_page_size + ' -s ' + cfg_image_size + ' ' + image_file
        print(command)
        os.system(command)

    print("make lfs image file over!")

if __name__ == "__main__":
    main()
