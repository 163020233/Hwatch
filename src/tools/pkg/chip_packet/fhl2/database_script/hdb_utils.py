#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
# Description: generation of mak database utils

import os
import stat

def get_core_name(cpp_defines):
    if cpp_defines is None:
        return ""
    if "CONFIG_APPS_CORE" in cpp_defines or "-DCONFIG_APPS_CORE" in cpp_defines:
        return "apps_core"
    if "CONFIG_PROT_CORE" in cpp_defines or "-DCONFIG_PROT_CORE" in cpp_defines:
        return "prot_core"
    if "CONFIG_MAIN_CORE" in cpp_defines or "-DCONFIG_MAIN_CORE" in cpp_defines:
        return "main_core"
    return ''


def get_product_type(target):
    if target is None:
        return ""
    if "cco" in target:
        return "cco"
    elif "sta" in target:
        return "sta"
    elif "ndm" in target:
        return "ndm"
    else:
        return "test"

def get_struct_def_by_core(core_name):
    if core_name == "apps_core":
        return "apps_core_hso_msg_struct_def.txt"
    elif core_name == "prot_core":
        return "prot_core_mak_msg_struct_def.txt"
    elif core_name == "main_core":
        return "main_core_mak_msg_struct_def.txt"
    return ''


def get_cpp_env(env_base, base_dir):
    cpp_defines = []
    cpp_path = []

    tmp_cpp_defines = env_base.get('CPPDEFINES')
    if tmp_cpp_defines is None:
        return ""
    for ele in tmp_cpp_defines:
        if ele.startswith("-D"):
            cpp_defines.append(ele)
            continue
        cpp_defines.append("-D{}".format(ele))

    tmp_cpp_path = env_base.get("CPPPATH")
    if tmp_cpp_path is None:
        return ""
    for ele in tmp_cpp_path:
        cpp_path.append(ele.replace('#', '-I{}'.format(base_dir)))

    cflag = env_base.get('CCFLAGS')
    if cflag is None:
        return ""

    return cflag + cpp_defines + cpp_path

def file_open(file_path, flag, encoding=None):
    encoding = encoding if encoding else None
    flags = {
        'r': os.O_RDONLY,
        'rb': os.O_RDONLY,
        'w': os.O_WRONLY | os.O_CREAT,
        'w+': os.O_RDWR | os.O_CREAT,
        'a': os.O_WRONLY | os.O_CREAT | os.O_APPEND,
        'a+': os.O_RDWR | os.O_CREAT | os.O_APPEND,
        'wb': os.O_WRONLY | os.O_CREAT,
        'wb+': os.O_RDWR | os.O_CREAT,
        'ab': os.O_WRONLY | os.O_CREAT | os.O_APPEND,
        'ab+': os.O_RDWR | os.O_CREAT | os.O_APPEND
    }
    if not os.path.isfile(file_path):
        modes = stat.S_IWUSR | stat.S_IRUSR | stat.S_IRGRP | stat.S_IROTH
        fd = os.open(file_path, flags[flag], modes)
        os.close(fd)
    return open(file_path, flag, encoding=encoding)
