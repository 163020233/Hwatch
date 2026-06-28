#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
# Description: extract strings from .c file, then, generate .cfg file.

import os
import time
import string
import re
import shutil
import hashlib
import binascii
import sys
import io
import subprocess
import copy
import json
from .hdb_utils import get_core_name
from .hdb_utils import get_cpp_env
from .hdb_utils import file_open

from conf_parser import ParserError
from utils.build_utils import print_warning


G_PARAMS = {}
G_FILE_ID_DIC = {}
G_LAST_FILE_ID_NUM = 0


def check_key(json_conf, conf):
    check_item = ['LOG_FILE_NAME', 'HDB_XML_TEMP_ROOT_DIR', 'HDB_XML_TEMP_BASE_DIR', 'HDB_XML_FILE_ID']
    keys = dict.keys(json_conf)
    for check_key in check_item:
        if check_key not in keys:
            msg = "[error] [mk_prim_xml_step1] need add ConfigMap (%s) in %s!" % (check_key, conf)
            raise ParserError(msg)


def save_file_id_dict(line):
    global G_FILE_ID_DIC
    global G_LAST_FILE_ID_NUM
    file_id_str = ''
    file_id_num = 0

    mod_1 = re.search("^(DIAG_FILE_ID_[\w]*).+=\s+(\d*)", line.strip())
    mod_2 = re.search("^(DIAG_FILE_ID_[\w]*)", line.strip())
    if mod_1 is not None:
        file_id_str = mod_1.groups()[0]  # 获取file id字符串
        file_id_num = mod_1.groups()[1]
        G_FILE_ID_DIC[file_id_str] = int(file_id_num)
        G_LAST_FILE_ID_NUM = int(file_id_num)
    elif mod_2 is not None:
        file_id_str = mod_2.group()
        G_LAST_FILE_ID_NUM += 1
        G_FILE_ID_DIC[file_id_str] = G_LAST_FILE_ID_NUM
    else:
        print_warning("save_file_id_dict search file id failed.")
        return -1
    return 0


def create_file_id_dic():
    file_name = G_PARAMS['HDB_XML_FILE_ID'].strip(' ')
    if not os.path.exists(file_name):
        print_warning("getFileIdEnum::file" + file_name + "not exist!!!")
        return

    with open(file_name, 'r') as file_d:
        lines = file_d.readlines()

    fsm_status = 0
    for line in lines:
        if 0 == fsm_status:
            mod = re.search("^typedef enum {", line.strip())
            if mod is not None:
                fsm_status = 1
        elif 1 == fsm_status:
            mod_1 = re.search("^DIAG_FILE_ID_[\w]*", line.strip())
            mod_2 = re.search("^}", line.strip())
            if mod_1 is not None:
                save_file_id_dict(line.strip())
            elif mod_2 is not None:
                fsm_status = 2
        elif 2 == fsm_status:
            break



def get_file_id_str(src_file_name):
    if not os.path.isfile(src_file_name):
        return None
    with open(src_file_name, 'r', encoding='utf-8', errors='replace') as file:
        for line in file:
            mod = re.search( \
                "^#define[\s]+THIS_FILE_ID[\s]+(DIAG_FILE_ID_[\w]*)", \
                line.strip())
            if mod is None:
                continue
            return mod.groups()[0]


# pre-compile .c to .i
def conver_c_2_i(full_file_name_list):
    dst_full_file_name_list = []
    temp_dir = G_PARAMS['I_FILE_DIR'].strip()
    if os.path.isdir(temp_dir):
        shutil.rmtree(temp_dir)
    os.makedirs(temp_dir)
    cc_name = G_PARAMS['CC'].strip()
    cflag = G_PARAMS['CFLAGS']
    if '-m-push-pop' in cflag : # remove unsupport flags
        cflag.remove('-m-push-pop')
    for src_full_file_name in full_file_name_list:
        file_id_str = get_file_id_str(src_full_file_name)
        if file_id_str is None:
            continue
        src_file_name = src_full_file_name.split('/')[-1]
        dst_file_name = src_file_name
        dst_file_name = dst_file_name.replace('.c', '.i')
        dst_full_file_name = os.path.join(temp_dir, dst_file_name)
        
        file_id = G_FILE_ID_DIC.get(file_id_str)
        if file_id is None:
            raise Exception(file_id_str + "not in " + str(G_FILE_ID_DIC))
        cmd_line = [cc_name, '-E', src_full_file_name] + cflag + \
                   ["-DMAKE_PRIM_XML_PROCESS_IN", \
                    '-D__FILE_IDX__=%s' % file_id, '-P', '-o', dst_full_file_name]
        subprocess.run(cmd_line, check=True)
        dst_full_file_name_list.append(dst_full_file_name)
    return dst_full_file_name_list


def cp_effect_content_on_file(cfg_fp, file_name):
    with open(file_name, 'r', encoding='utf-8', errors='replace') as src_fp:
        for line in src_fp:
            match = re.search('_PRIM_ST', line)
            if match:
                cfg_fp.write(line.strip(" "))


def cp_effect_content(dst_full_file_name_list, core_name):
    base_folder = \
        os.path.join(G_PARAMS['HDB_XML_TEMP_BASE_DIR'].strip(), core_name)
    key_word = G_PARAMS['PRIM_XML_KEY_WORD'].strip()
    if not os.path.isdir(base_folder):
        try:
            os.makedirs(base_folder)
        except FileExistsError as e:
            pass

    cfg_file_name = key_word + ".cfg"
    cfg_file = os.path.join(base_folder, cfg_file_name)
    if os.path.isfile(cfg_file):
        os.remove(cfg_file)
    cfg_fp = file_open(cfg_file, 'w+', encoding='UTF-8')
    for src_file_name in dst_full_file_name_list:
        time_out_sec = 3
        cur_wati_sec = 0
        b_file_exist = 0
        while cur_wati_sec < time_out_sec:
            if os.path.isfile(src_file_name):
                b_file_exist = 1
                break
            time.sleep(0.1)
            cur_wati_sec = cur_wati_sec + 0.1
        if b_file_exist:
            cp_effect_content_on_file(cfg_fp, src_file_name)
    cfg_fp.close()



def del_char(list):
    list_after = []
    for del_macro in list:
        if '\\' in del_macro:
            del_macro = del_macro.strip('\\')
        list_after.append(del_macro)
    return list_after


def generate_db(module_name, cc, src, inc, defs, xflags, root, in_path):
    global G_PARAMS
    if os.path.isfile(in_path) is False:
        raise Exception("Can NOT found the database configure file:%s!\n", in_path)

    cpp_defines = []
    for define in defs:
        cpp_defines.append(define.replace("\\\"", "\""))

    core_name = "apps_core"
    cflags = xflags
    inc_path = inc
    source = src
    if "" in cflags:
        cflags.remove('')
    if "" in inc_path:
        inc_path.remove('')

    all_flags = cpp_defines
    all_flags.extend(cflags)
    all_flags.extend(inc_path)

    log_file_name = module_name + '.log'

    # get global settings
    db_conf = None
    conf = os.path.join(root, in_path)
    with open(conf, 'r') as f:
        db_conf = json.load(f)

    if core_name is None or all_flags is None or db_conf is None:
        return None

    check_key(db_conf, conf)

    G_PARAMS['CORE_NAME'] = core_name
    G_PARAMS['CFLAGS'] = all_flags
    G_PARAMS['SOURCES'] = source
    G_PARAMS['HDB_XML_TEMP_ROOT_DIR'] = os.path.join(root, db_conf["HDB_XML_TEMP_ROOT_DIR"])
    G_PARAMS['HDB_XML_TEMP_BASE_DIR'] = os.path.join(root, db_conf["HDB_XML_TEMP_BASE_DIR"])
    G_PARAMS['HDB_XML_FILE_ID'] = os.path.join(root, db_conf["HDB_XML_FILE_ID"])
    G_PARAMS['LOG_FILE_NAME'] = os.path.join(root, db_conf["LOG_FILE_NAME"], log_file_name)
    G_PARAMS['PRIM_XML_KEY_WORD'] = module_name
    G_PARAMS['CC'] = cc

    log_file_name = G_PARAMS['LOG_FILE_NAME'].strip()
    log_file_dir = os.path.dirname(log_file_name)
    core_name = G_PARAMS['CORE_NAME']
    tmp_core_dir = \
        os.path.join(G_PARAMS['HDB_XML_TEMP_ROOT_DIR'].strip(), core_name)
    G_PARAMS['I_FILE_DIR'] = \
        os.path.join(tmp_core_dir, G_PARAMS['PRIM_XML_KEY_WORD'])
    if not os.path.isdir(log_file_dir):
        try:
            os.makedirs(log_file_dir)
        except FileExistsError as e:
            pass
    log_file_fp = file_open(log_file_name, 'w+')
    last_print_target = sys.stdout
    sys.stdout = log_file_fp
    create_file_id_dic()
    full_file_name_list = G_PARAMS.get('SOURCES')
    dst_full_file_name_list = conver_c_2_i(full_file_name_list)
    if dst_full_file_name_list is None:
        return -1
    cp_effect_content(dst_full_file_name_list, core_name)
    sys.stdout = last_print_target
    log_file_fp.close()
