#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    Build Update Pkg File
# Copyright HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
# ============================================================================

import os
import sys

file_dir = os.path.dirname(os.path.realpath(__file__))
g_root = os.path.realpath(os.path.join(file_dir, "..", "..", "..", "..", ".."))
sys.path.append(os.path.join(g_root, 'build', 'script'))

files = [[
        {
            'file_path': os.path.join(g_root, "output", "3322", "rootkey_partition_cfg.bin"),
            'max_length': 0x1000
        },
        {
            'file_path': os.path.join(g_root, "output", "3322", "acore", "3322-ssb", "ssb_signed.bin"),
            'max_length': 0xf000
        },
        {
            'file_path': os.path.join(g_root, "output", "3322", "acore", "backup3322-ssb", "ssb_backup_signed.bin"),
            'max_length': 0xf000
        },
        {
            'file_path': os.path.join(g_root, "output", "3322", "acore", "3322-recovery", "recovery_signed.bin"),
            'max_length': 0x61000
        },
        {
            'file_path': os.path.join(g_root, "output", "3322", "acore", "3322-seliteos-release", "seliteos_signed.bin"),
            'max_length': 0x40000
        },
        {
            'file_path': os.path.join(g_root, "output", "3322", "acore", "3322-diting", "application_signed.bin"),
            'max_length': 0x800000
        },
        {
            'file_path': os.path.join(g_root, "output", "3322", "acore", "3322-diting", "3322_all_nv.bin"),
            'max_length': 0x6000
        }
    ],
    [
        {
            'file_path': os.path.join(g_root, "output", "3322", "rootkey_partition_cfg.bin"),
            'max_length': 0x1000
        },
        {
            'file_path': os.path.join(g_root, "output", "3322", "acore", "3322-ssb", "ssb_signed.bin"),
            'max_length': 0xf000
        },
        {
            'file_path': os.path.join(g_root, "output", "3322", "acore", "backup3322-ssb", "ssb_backup_signed.bin"),
            'max_length': 0xf000
        },
        {
            'file_path': os.path.join(g_root, "output", "3322", "acore", "3322-recovery", "recovery_signed.bin"),
            'max_length': 0x61000
        },
        {
            'file_path': os.path.join(g_root, "output", "3322", "acore", "3322-seliteos-release", "seliteos_signed.bin"),
            'max_length': 0x40000
        },
        {
            'file_path': os.path.join(g_root, "output", "3322", "acore", "3322-native-js", "application_signed.bin"),
            'max_length': 0x6e3000
        },
        {
            'file_path': os.path.join(g_root, "output", "3322", "acore", "3322-native-js", "3322_all_nv.bin"),
            'max_length': 0x6000
        }
    ]]

all_in_one_list = [
    "3322-diting",
    "3322-native-js"
]


class base_info:
    def __init__(self, select_index):
        self.root_path = g_root
        self.target_index = select_index
        # 产品镜像输出路径
        self.output = os.path.join(self.root_path, "output", "3322")
        print("self root:", self.root_path)
        # 文件映射表：文件路径、最大长度
        self.file_mapping = files[select_index]

    def read_and_pad_file(self, file_path, max_length):
        """
        读取文件内容并填充到指定长度
        :param file_path: 文件路径
        :param max_length: 最大长度
        :return: 填充后的字节数据
        """
        try:
            with open(file_path, 'rb') as f:
                data = f.read()
                current_length = len(data)
                if current_length < max_length:
                    # 计算需要填充的字节数
                    pad_length = max_length - current_length
                    # 填充0xff
                    data += bytes([0xff] * pad_length)
                return data
        except FileNotFoundError:
            # 文件不存在时，打印文件名并报错退出
            print(f"Build fail! File {file_path} not exist, please build it first!")
            sys.exit(1)

    def generate_combined_file(self, output_file):
        """
        生成拼接后的文件
        :param output_file: 输出文件路径
        """
        # 拼接所有文件
        combined_data = bytearray()
        for file_info in self.file_mapping:
            file_path = file_info['file_path']
            max_length = file_info['max_length']
            data = self.read_and_pad_file(file_path, max_length)
            if "application_signed.bin" in file_path:
                if all_in_one_list[self.target_index] == "3322-diting":
                    print("application will add 0x73A000 byte 0xFF")
                    data += bytes([0xff] * 0x73A000)
                if all_in_one_list[self.target_index] == "3322-native-js":
                    print("application will add 0x57000 byte 0xFF")
                    data += bytes([0xff] * 0x57000)
            print(f"merge file {file_info['file_path']} success!")
            combined_data.extend(data)

        '''
        converted_data = bytearray()
        for i in range(0, len(combined_data), 4):
            chunk = combined_data[i:i+4]
            converted_chunk = chunk[::-1]
            converted_data.extend(converted_chunk)
        '''

        # 写入输出文件
        with open(output_file, 'wb') as f:
            f.write(combined_data)

def do_build(select_index):
    # 创建base_info实例
    info = base_info(select_index)
    # 生成拼接后的文件，输出到指定路径
    file_name = all_in_one_list[select_index] + "_norflash_image.bin"
    info.generate_combined_file(os.path.join(info.output, file_name))

def target_input():
    print("Here are the available target_names")
    cur_index = 0
    for index in range(len(all_in_one_list)):
        print("%-3d : %s" % (cur_index, all_in_one_list[index]))
        cur_index += 1
    print("Input index numbers for build or q for quit(num/q):", end='')
        # 输入序号或q/Q
    select_index = input()
    if select_index.lower() == 'q':
        sys.exit(0)
    return int(select_index)

if __name__ == '__main__':
    do_build(target_input())
    print(f"Generate norflash_image.bin success!")