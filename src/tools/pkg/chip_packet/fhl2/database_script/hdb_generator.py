#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
# Description: extract strings from .c file, then, generate struct .txt file.

import os
import time
import string
import re
import shutil
import time
import hashlib
import binascii
import sys
import subprocess
import stat
import json

from .hdb_utils import get_core_name
from .hdb_utils import get_product_type
from .hdb_utils import get_struct_def_by_core
from .hdb_utils import get_cpp_env

sys.path.append(os.path.join('build', 'script'))
from conf_parser import ParserError
from .hdb_file_include import add_file_include
from .hdb_utils import file_open

G_FIRST_LINE = r'#include "base_datatype_def.txt"' + '\n'
G_PARAMS = {}


def c_2_txt_file(src_file, dst_file):
    with open(src_file, 'r') as src_f:
        dst_f = file_open(dst_file, 'w+', encoding='utf-8')
        dst_f.write(G_FIRST_LINE)
        for line in src_f:
            if (re.match(r"[ ]*#", line)):
                pass
            elif (re.match(r"[ ]*TD_EXTERN", line)):
                pass
            elif (re.match(r"[ ]*TD_PRV", line)):
                pass
            elif (re.match(r"[ ]*TD_EAPI", line)):
                pass
            elif (re.match(r"[ ]*extern", line)):
                pass
            else:
                dst_f.write(line)
        dst_f.close()


def check_key(json_conf, conf):
    check_item = ['HDB_TXT_TEMP_DIR', 'HDB_TXT_BASE_DATATYPE', 'HDB_TXT_DST_DIR', 'HDB_TXT_SRC_DIR']
    keys = dict.keys(json_conf)
    for check_key in check_item:
        if check_key not in keys:
            msg = "[error] [hdb_txt_simplyfy] need add ConfigMap (%s) in %s!" % (check_key, conf)
            raise ParserError(msg)


def c_2_txt_dir(src_dir, dst_dir):
    src_dir_list = os.listdir(src_dir)
    for src_f in src_dir_list:
        if os.path.splitext(src_f)[-1] == ".txt":
            src_file = os.path.join(src_dir, src_f)
            dst_file = os.path.join(dst_dir, src_f)
            c_2_txt_file(src_file, dst_file)


def process(g_src_dir, g_src_dir_tmp, core_name, dfx_path):
    cflag = G_PARAMS.get("CFLAGS")
    for src_file in os.listdir(g_src_dir):
        if src_file == get_struct_def_by_core(core_name):  # msg database
            struct_def_file = get_struct_def_by_core(core_name)  # cmd database
            flag = True
        elif src_file.startswith("mak_cmd_"):
            struct_def_file = src_file
            flag = True
        elif src_file.startswith("mak_cmd_"):
            struct_def_file = src_file
            flag = True
        elif src_file.startswith("fix_"):
            struct_def_file = src_file
            flag = False
        elif src_file.startswith("tool_fix_"):
            struct_def_file = src_file
            flag = False
        else:
            continue
        full_src_file_name = os.path.join(g_src_dir, struct_def_file)
        full_target_file_name = \
            os.path.join(g_src_dir_tmp, struct_def_file.replace(".txt", ".c"))

        if flag:
            shutil.copy(full_src_file_name, full_target_file_name)
            if '' not in dfx_path:
                add_file_include(dfx_path, full_target_file_name)
            cmd_line = [G_PARAMS.get('CC'), '-E', full_target_file_name] + cflag + \
                       ["-DMAKE_PRIM_XML_PROCESS_IN", '-P', '-o', os.path.join(g_src_dir_tmp, struct_def_file)]
            subprocess.run(cmd_line, check=True)
        else:
            shutil.copy(full_src_file_name, os.path.join(g_src_dir_tmp, struct_def_file))


def mkdir(g_src_dir_tmp, g_dst_dir):
    if not os.path.exists(g_src_dir_tmp):
        os.makedirs(g_src_dir_tmp)
    if not os.path.exists(g_dst_dir):
        os.makedirs(g_dst_dir)


def build_global_param(env_base, db_conf, base_dir):
    cpp_defines = env_base.get('CPPDEFINES')
    core_name = get_core_name(cpp_defines)
    cflags = get_cpp_env(env_base, base_dir)
    if core_name is None or cflags is None:
        return

    params_tmp = {}
    params_tmp['CORE_NAME'] = core_name
    params_tmp['CFLAGS'] = cflags
    params_tmp["HDB_TXT_TEMP_DIR"] = os.path.join(base_dir, db_conf["HDB_TXT_TEMP_DIR"])
    params_tmp["HDB_TXT_DST_DIR"] = os.path.join(base_dir, db_conf["HDB_TXT_DST_DIR"])
    params_tmp["HDB_TXT_BASE_DATATYPE"] = os.path.join(base_dir, db_conf["HDB_TXT_BASE_DATATYPE"])
    params_tmp["HDB_TXT_SRC_DIR"] = os.path.join(base_dir, db_conf["HDB_TXT_SRC_DIR"])
    params_tmp['CC'] = env_base.get("CC")
    return params_tmp


def check_include_path(include_path):
    includes = []
    '''判断路径前面是否加了-I'''
    for include in include_path:
        if not include.startswith('-I'):
            includes.append('-I%s' % include)
        else:
            includes.append(include)
    return includes


def generate_db_struct(cc, defs, xflags, header_path, base_dir, in_path, target, dfx_include_path):
    global G_PARAMS
    if os.path.isfile(in_path) is False:
        raise Exception("Can NOT found the database configure file:%s!\n", in_path)

    cpp_defines = []
    for define in defs:
        cpp_defines.append(define.replace("\\\"", "\""))

    core_name = "apps_core"
    cflags = xflags
    h_path = header_path
    dfx_path = dfx_include_path

    '''
    检查两个列表里面的路径是否带有-I，如果没有需要加上；
    并将两个列表合并为一个列表
    '''
    if len(dfx_include_path) == 0:
        includes = list(set(check_include_path(h_path)))
    else:
        includes = list(set(check_include_path(h_path)).union(set(check_include_path(dfx_path))))
    macros = []
    for macro in cpp_defines:
        if not macro.startswith('-D'):
            macros.append('-D%s' % macro)
        else:
            macros.append(macro)

    all_flags = macros
    all_flags.extend(cflags)
    all_flags.extend(includes)

    # get global settings
    db_conf = None
    conf = os.path.join(base_dir, in_path)
    with open(conf, 'r', encoding='utf-8') as f:
        db_conf = json.load(f)

    if core_name is None or all_flags is None or db_conf is None:
        return None

    check_key(db_conf, conf)

    G_PARAMS['CORE_NAME'] = core_name
    G_PARAMS['CFLAGS'] = all_flags
    G_PARAMS['CC'] = cc
    G_PARAMS["HDB_TXT_TEMP_DIR"] = os.path.join(base_dir, db_conf["HDB_TXT_TEMP_DIR"])
    G_PARAMS["HDB_TXT_DST_DIR"] = os.path.join(base_dir, db_conf["HDB_TXT_DST_DIR"])
    G_PARAMS["HDB_TXT_BASE_DATATYPE"] = os.path.join(base_dir, db_conf["HDB_TXT_BASE_DATATYPE"])
    G_PARAMS["MAK_PSEUDO_STRUCT"] = os.path.join(base_dir, db_conf["MAK_PSEUDO_STRUCT"])
    G_PARAMS["HDB_TXT_SRC_DIR"] = os.path.join(base_dir, db_conf["HDB_TXT_SRC_DIR"])

    g_src_dir = G_PARAMS.get("HDB_TXT_SRC_DIR")
    g_src_dir_tmp = G_PARAMS.get("HDB_TXT_TEMP_DIR")
    g_dst_dir = os.path.join(G_PARAMS.get("HDB_TXT_DST_DIR"), get_product_type(target), "diag")
    g_base_datatype_def_txt = G_PARAMS.get("HDB_TXT_BASE_DATATYPE")
    g_base_mak_pseudo_struct_txt = G_PARAMS.get("MAK_PSEUDO_STRUCT")
    if not os.path.exists(g_src_dir) or not os.path.exists(g_base_datatype_def_txt) or not os.path.exists(g_base_mak_pseudo_struct_txt):
        return
    mkdir(g_src_dir_tmp, g_dst_dir)
    process(g_src_dir, g_src_dir_tmp, G_PARAMS.get('CORE_NAME'), dfx_path)
    c_2_txt_dir(g_src_dir_tmp, g_dst_dir)
    shutil.copy(g_base_datatype_def_txt, g_dst_dir)
    shutil.copy(g_base_mak_pseudo_struct_txt, g_dst_dir)


if __name__ == "__main__":
    generate_db_struct()
