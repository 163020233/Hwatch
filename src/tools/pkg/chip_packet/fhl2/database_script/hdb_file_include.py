#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
# Description: extract strings from .c file, then, generate include .h file.

import os
import sys

sys.path.append(os.path.join('build', 'script'))
from conf_parser import ParserError
from .hdb_utils import file_open
from utils.build_utils import print_info


def check_path(dfx_include_path, file_path):
    '''
    判断参数路径是否存在，并返回矫正后的路径列表
    '''
    dfx_include = []
    for include_path in dfx_include_path:
        if not os.path.exists(file_path):  # 检查目标是否存在
            msg = "[error] [hdb_file_include] file_path %s does not exist!" % file_path
            raise ParserError(msg)
        else:
            if not os.path.isfile(file_path):  # 检查目标是否为文件
                msg = "[error] [hdb_file_include] file_path %s not is file!" % file_path
                raise ParserError(msg)
        if "-I" in include_path:  # 确保路径无"-I"后存到新列表
            include_path = include_path[2:]
            dfx_include.append(include_path)
        else:
            dfx_include.append(include_path)
        if not os.path.exists(include_path):  # 检查路径是否存在
            msg = "[error] [hdb_file_include] dfx_include_path %s does not exist!" % include_path
            raise ParserError(msg)
    return dfx_include


def read_file_include(file_path):
    '''
    解析文件中现有的依赖文件,并返回一个文件列表
    '''
    file_list = []  # 建立一个空列表，用于存储头文件名
    with open(file_path, 'r') as j:
        lines = j.readlines()  # 按行读取文件中的内容
    for line in lines:
        if line.startswith("#include"):  # 筛选引用头文件的行
            file_name = line[len("#include"):]  # 获取include后的内容
            file_name = file_name.strip()  # 去除前后空格
            file_name = file_name[1:-1]  # 切片获取文件名
            file_name = file_name.strip()  # 预防文件名前后有空格的情况
            file_list.append(file_name)
    return file_list


def extract_file_name(dfx_include, file_list):
    '''
    提取dfx_include路径文件名
    '''
    # 从dfx_include中循环提取路径，将每一个路径下的文件名依次提取并存储到列表
    for include_path in dfx_include:
        try:
            # 提取路径下的内容(文件名)
            files = os.listdir(include_path)
        except Exception as err:
            print_info(err)
            continue
        for file_name in files:  # 从目录的文件列表中取出文件
            if not file_name.endswith('.h'):  # 判断是否为.h文件
                continue
            if file_name not in file_list:  # 去重后存储到file_list列表中
                file_list.append(file_name)
    return file_list


def write_to_file(file_list, file_path):
    '''
    将指定路径的文件清空后，添加依赖路径
    '''
    file_head = \
        "#define __TD_BASE_H__\n#define EXT_START_HEADER\n#define EXT_END_HEADER\n"
    f = file_open(file_path, 'w', encoding='utf-8')
    f.write(file_head)
    for file_name in file_list:  # 在目标文件中的添加获取到的依赖文件
        include_str = '#include <%s>\n' % file_name
        f.write(include_str)
    f.close()


def add_file_include(dfx_include_path, file_path):
    dfx_include = check_path(dfx_include_path, file_path)
    file_list = read_file_include(file_path)
    file_list = extract_file_name(dfx_include, file_list)
    write_to_file(file_list, file_path)
