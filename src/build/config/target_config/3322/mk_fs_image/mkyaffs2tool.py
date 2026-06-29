#!/usr/local/bin/python3.7
# -*- coding: utf-8 -*-
#===============================================================================
# @brief   Python script
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================

import os
import sys
import time
import struct
import shutil
from distutils.dir_util import copy_tree

"""
介绍：
    本脚本主要实现调用mkyaffs2image工具生成文件镜像
    请注意使用本脚本时支持的python版本，本脚本只支持python3版本。
    请根据实际情况配置对应信息，主要是nandflash的信息，以及分区信息
    目前配置了user分区

"""

root_path = os.path.join(os.path.split(os.path.realpath(__file__))[0], '..', '..', '..', '..', '..')
cur_dir = os.path.dirname(os.path.abspath(__file__))
image_tool_path = os.path.realpath(os.path.join(root_path, 'tools', 'bin', 'platform_tool', 'mkyaffs2image'))
out_path = os.path.join(root_path, 'output', '3322')
tools_bin_path = os.path.join(root_path, 'tools', 'pkg', 'bin', '3322')
dsp_bin_path = os.path.join(tools_bin_path, 'dsp', 'normal')
gnss_bin_path = os.path.join(tools_bin_path, 'gnss', 'firmwares')
nfc_bin_path = os.path.join(tools_bin_path, 'nfc')

# 配置生成文件镜像工具路径
mkyaffstool = os.path.join(image_tool_path, 'mkyaffs2image')
image_name = "file"

# 配置nandflash信息
cfg_page_size = 2048
cfg_obb_size = 64

# nand分区{5，25，70}
# 配置分区信息，请根据实际情况进行配置，添加格式为[分区路径， 分区大小]
# 分区大小需要根据按照百分系统分配好后的blocks数进行计算，大小=blocks（文件块个数）*pages_per_block（每个文件块包含的页数）*bytes_per_page（每页包含字节数）
# 前面按照比例分好分区，最后分区置成null，即代表剩下所有blocks
partition_list_normal = [
    [os.path.join(cur_dir, 'fs', 'boot'), "0x1980000"],
    [os.path.join(cur_dir, 'fs', 'system'), "0x8000000"],
    [os.path.join(cur_dir, 'fs', 'user'), "null"],
]

# 产测版本文件系统制作时只预制boot目录
partition_list_ft = [
    [os.path.join(cur_dir, 'fs', 'boot'), "null"],
]

def get_page_type(page_size):
    if page_size == 2048:
        return " 1"
    elif page_size == 4096:
        return " 2"
    else:
        return " 0"

def get_obb_type(obb_size):
    if obb_size == 64:
        return " 2"
    elif obb_size == 128:
        return " 4"
    else:
        return " 0"

def copy_file(ft_ver):
    #清除fs文件
    if os.path.exists(os.path.join(cur_dir, 'fs')):
        shutil.rmtree(os.path.join(cur_dir, 'fs'), ignore_errors = True)
    os.makedirs(os.path.join(cur_dir, 'fs', 'boot', 'data'), exist_ok=True)
    os.makedirs(os.path.join(cur_dir, 'fs', 'system'), exist_ok=True)
    os.makedirs(os.path.join(cur_dir, 'fs', 'user'), exist_ok=True)

    bin_mapping = [
        (os.path.join(tools_bin_path, 'bt_signed.bin'), os.path.join(cur_dir, 'fs', 'boot', 'bt')),
        (os.path.join(dsp_bin_path, 'dsp_main.bin'), os.path.join(cur_dir, 'fs', 'boot', 'dsp')),
        (os.path.join(dsp_bin_path, 'dsp_overlay.bin'), os.path.join(cur_dir, 'fs', 'boot', 'dsp')),
        (os.path.join(gnss_bin_path, 'gnss_config.bin'), os.path.join(cur_dir, 'fs', 'boot', 'gnss')),
        (os.path.join(nfc_bin_path, 'fw.img'), os.path.join(cur_dir, 'fs', 'boot', 'nfc')),
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

    #拷贝res
    resource_mapping = [
        (os.path.join('res'), os.path.join(cur_dir, 'fs', 'user', 'res')),
        (os.path.join('music'), os.path.join(cur_dir, 'fs', 'user', 'music')),
        (os.path.join('video'), os.path.join(cur_dir, 'fs', 'user', 'video')),
        (os.path.join('dial'), os.path.join(cur_dir, 'fs', 'user', 'dial')),
        (os.path.join('dial', '466x466'), os.path.join(cur_dir, 'fs', 'user', 'dial')),
    ]
    for resource_dir, target_path in resource_mapping:
        source_path = os.path.join(root_path, 'application', 'wearable', 'res', resource_dir)
        if not os.path.exists(source_path):
            print(f"Source path {source_path} does not exist. Skipping.")
            continue
        if os.path.exists(target_path):
            shutil.rmtree(target_path)
        os.makedirs(target_path, exist_ok=True)
        copy_tree(source_path, target_path)

    res_path = os.path.join(root_path, 'application', 'wearable', 'res')
    if os.path.exists(os.path.join(res_path, 'logo', 'OPEN_LOGO.bin')):
        shutil.copyfile(os.path.join(res_path, 'logo', 'OPEN_LOGO.bin'), os.path.join(cur_dir, 'fs', 'boot', 'data', 'OPEN_LOGO.bin'))
    if os.path.exists(os.path.join(res_path, 'logo', 'OPEN_LOGO_QSPI.bin')):
        shutil.copyfile(os.path.join(res_path, 'logo', 'OPEN_LOGO_QSPI.bin'), os.path.join(cur_dir, 'fs', 'boot', 'data', 'OPEN_LOGO_QSPI.bin'))
    #产线音频测试预置资源
    if ft_ver:
        shutil.copyfile(os.path.join(res_path, 'ft', 'AAA_16k_16bit_1ch.pcm'), os.path.join(cur_dir, 'fs', 'boot', 'AAA_16k_16bit_1ch.pcm',))
    
    # 字体文件映射关系（源文件名 -> 目标文件名）
    font_rename_mapping = {
        "HarmonyOS_Sans_SC.ttf": "HarmonyOS_Sans_SC_Regular.ttf",
        "HarmonyOS_Sans_Naskh_Arabic_UI.ttf": "HarmonyOS_Sans_Naskh_Arabic_Light.ttf",
        "HarmonyOS_Sans_Italic.ttf": "HarmonyOS_Sans_Light_Italic.ttf",
        "HarmonyOS_Sans.ttf": "HarmonyOS_Sans_Regular.ttf"
    }

    fonts_src_dir = os.path.join(root_path, 'ohos', 'base', 'global', 'system_resources', 'fonts')
    fonts_dst_dir = os.path.join(cur_dir, 'fs', 'user', 'res')


    for src_file in os.listdir(fonts_src_dir):
        src_path = os.path.join(fonts_src_dir, src_file)
    
        if not src_file.lower().endswith('.ttf') or not os.path.isfile(src_path):
            continue

        # 查看是否在映射表中
        if src_file in font_rename_mapping:
            dst_filename = font_rename_mapping[src_file]
            dst_path = os.path.join(fonts_dst_dir, dst_filename)
        
            shutil.copyfile(src_path, dst_path)
            print(f"字体文件已成功复制并重命名: {src_path} -> {dst_path}")

    print("所有字体文件复制完成（含重命名）！")

    # 新增路径变更文件的复制
    path_change_mapping = [
        (os.path.join(root_path, 'ohos', 'foundation', 'arkui', 'ui_lite', 'tools', 'qt', 'simulator', 'config', 'faces', 'default', 'B024_002.bin'), os.path.join(cur_dir, 'fs', 'user', 'res', 'B024_002.bin')),
        (os.path.join(root_path, 'ohos', 'foundation', 'arkui', 'ui_lite', 'tools', 'qt', 'simulator', 'config', 'images', 'blue_rgb888.bin'), os.path.join(cur_dir, 'fs', 'user', 'res', 'blue_rgb888.bin')),
        (os.path.join(root_path, 'ohos', 'foundation', 'arkui', 'ui_lite', 'tools', 'qt', 'simulator', 'font', 'line_cj.brk'), os.path.join(cur_dir, 'fs', 'user', 'res', 'line_cj.brk')),
        (os.path.join(root_path, 'ohos', 'base', 'global', 'i18n_lite', 'frameworks', 'i18n', 'i18n.dat'), os.path.join(cur_dir, 'fs', 'user', 'res', 'i18n.dat')),
        (os.path.join(root_path, 'ohos', 'foundation', 'arkui', 'ui_lite', 'tools', 'qt', 'simulator', 'config', 'images', 'image_animator_0.bin'), os.path.join(cur_dir, 'fs', 'user', 'res', 'image_animator_1.bin')),
        (os.path.join(root_path, 'ohos', 'foundation', 'arkui', 'ui_lite', 'tools', 'qt', 'simulator', 'config', 'images', 'image_animator_1.bin'), os.path.join(cur_dir, 'fs', 'user', 'res', 'image_animator_2.bin')),
        (os.path.join(root_path, 'ohos', 'foundation', 'arkui', 'ui_lite', 'tools', 'qt', 'simulator', 'config', 'images', 'image_animator_2.bin'), os.path.join(cur_dir, 'fs', 'user', 'res', 'image_animator_3.bin')),
        (os.path.join(root_path, 'ohos', 'foundation', 'arkui', 'ui_lite', 'tools', 'qt', 'simulator', 'config', 'images', 'image_animator_3.bin'), os.path.join(cur_dir, 'fs', 'user', 'res', 'image_animator_4.bin')),
    ]

    for src_path, dst_path in path_change_mapping:
        if os.path.exists(src_path):
            dst_dir = os.path.dirname(dst_path)
            if not os.path.exists(dst_dir):
                os.makedirs(dst_dir)
            shutil.copyfile(src_path, dst_path)
            print(f"路径变更文件已成功复制: {src_path} -> {dst_path}")
        else:
            print(f"路径变更文件不存在: {src_path}")

if __name__ == "__main__":
    target_name = os.path.basename(sys.argv[1])
    if target_name == "3322-diting-ft":
        partition_list = partition_list_ft
        ft_ver = True
    else:
        partition_list = partition_list_normal
        ft_ver = False

    copy_file(ft_ver)
    image_file = os.path.join(cur_dir, "%s.bin" %image_name)
    print("image file", image_file)

    if sys.platform == 'linux2' or sys.platform == 'linux':
        partition_total = 0
        page_type =  get_page_type(cfg_page_size)
        obb_type = get_obb_type(cfg_obb_size)

        if os.path.exists(image_file):
            os.remove(image_file)

        for partition in partition_list:
            if (partition[1] != "null"):
                partition_total = partition_total + int(partition[1][2:], 16)
                command = mkyaffstool + " " + partition[0] + " " + image_file + page_type + obb_type + (" 0x%x" %partition_total)
            else:
                command = mkyaffstool + " " + partition[0] + " " + image_file + page_type + obb_type
            print(command)
            os.system(command)

    if (os.path.exists(image_file)):
        shutil.copyfile(image_file, os.path.join(out_path, "%s.bin" %image_name))

    print("make yaffs image file over!")
