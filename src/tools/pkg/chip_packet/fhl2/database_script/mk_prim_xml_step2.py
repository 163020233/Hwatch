#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
# Description: extract strings from .cfg , then, generate .xml file.

import os
import time
import string
import re
import shutil
import time
import hashlib
import subprocess
import binascii
import sys
import json
import fnmatch

import xml.etree.ElementTree as ET
from .hdb_utils import get_core_name
from .hdb_utils import file_open

from conf_parser import ParserError


G_PARAMS = {}


def get_file_sha256(file_name):
    if os.path.isfile(file_name):
        m = hashlib.sha256()
        with open(file_name, 'rb') as fp:
            data = fp.read()
        m.update(data)
        return m.hexdigest()
    return 0


def check_key(json_conf, conf):
    target_key = str()
    if 'cco' in sys.argv[4].lower():
        target_key = 'CCO_HDB_CMD_XML_DST_FILE'
    elif 'sta' in sys.argv[4].lower():
        target_key = 'STA_HDB_CMD_XML_DST_FILE'
    elif 'ndm' in sys.argv[4].lower():
        target_key = 'NDM_HDB_CMD_XML_DST_FILE'
    else:
        target_key = 'HDB_CMD_XML_DST_FILE'
    check_item = ['LOG_FILE_NAME', 'HDB_XML_TEMP_BASE_DIR', 'HDB_PRIM_XML_DST_FILE', \
                  'HDB_PRIM_XML_SRC_FILE', target_key, 'HDB_CMD_XML_SRC_FILE']
    keys = dict.keys(json_conf)
    for check_key in check_item:
        if check_key not in keys:
            msg = "[error] [mk_prim_xml_step2] need add ConfigMap (%s) in %s!" % (check_key, conf)
            raise ParserError(msg)


def get_msg_root(subsystem):
    msg_root_element = ET.Element('')
    for child in subsystem:
        if child.tag == 'MSG_LOG':
            msg_root_element = child
            break
    if msg_root_element.tag == 'MSG_LOG':
        temp_attrib = msg_root_element.attrib
        temp_text = msg_root_element.text
        temp_tail = msg_root_element.tail
        msg_root_element.attrib = temp_attrib
        msg_root_element.text = temp_text
        msg_root_element.tail = temp_tail
    return msg_root_element


def write_prim_xml_tree(tree, dst_xml_file):
    dst_xml_file_dir = os.path.abspath(os.path.join(dst_xml_file, ".."))
    if not os.path.exists(dst_xml_file_dir):
        os.makedirs(dst_xml_file_dir)
    tree.write(dst_xml_file, encoding="UTF-8", \
               xml_declaration='<?xml version="1.0" encoding="utf-8" ?>', \
               method="xml", short_empty_elements=True)


def add_content_to_xml(file_name, msg_root_element):
    with open(file_name, encoding='UTF-8') as src_fp:
        for line in src_fp:
            match_st = re.search('_PRIM_ST', line)
            match_pri = re.search(', _PRIM_PRI_ = ', line)
            match_id = re.search(', _PRIM_ID_ = ', line)
            match_sz = re.search(', _PRIM_SZ_ = ', line)
            match_line = re.search(', _PRIM_LINE_ = ', line)
            match_file = re.search(', _PRIM_FILE_ = ', line)
            match_file_id = re.search(', _PRIM_FILE_ID_ = ', line)
            match_type = re.search(', _PRIM_TYPE_ = ', line)
            match_end = re.search(', _PRIM_END_', line)
            if match_st and match_pri and match_id and match_sz and \
                    match_line and match_file and match_file_id and \
                    match_type and match_end:

                prim_pri = line[match_pri.end():match_id.start()]
                prim_id = line[match_id.end():match_sz.start()]
                prim_sz = line[match_sz.end():match_line.start()].strip(r'"')
                prim_line = line[match_line.end():match_file.start()]
                prim_file = line[match_file.end():match_file_id.start()]
                prim_file_id = line[match_file_id.end():match_type.start()]
                prim_match_type = \
                    line[match_type.end():match_end.start()].strip(r'"')

                if int(prim_pri) == 0:
                    real_pri = int(prim_pri)
                    pri_str = 'ALERT'
                elif int(prim_pri) == 1:
                    real_pri = int(prim_pri)
                    pri_str = 'FATAL'
                elif int(prim_pri) == 2:
                    real_pri = int(prim_pri)
                    pri_str = 'ERROR'
                elif int(prim_pri) == 3:
                    real_pri = int(prim_pri)
                    pri_str = 'WARNING'
                elif int(prim_pri) == 4:
                    real_pri = int(prim_pri)
                    pri_str = 'NOTICE'
                elif int(prim_pri) == 5:
                    real_pri = int(prim_pri)
                    pri_str = 'INFO'
                elif int(prim_pri) == 6:
                    real_pri = int(0)
                    pri_str = 'DBG'
                elif int(prim_pri) == 7:
                    real_pri = int(0)
                    pri_str = 'TRACE'

                if prim_id.isdigit():
                    if int(prim_id) == 0:
                        real_id = int(prim_line)
                    else:
                        real_id = int(prim_id)
                else:
                    i = 1
                    while 1:
                        if not prim_id[-i:].isdigit():
                            break
                        i = i + 1

                    if (i == 1):
                        real_id = 0
                    else:
                        i = i - 1
                        real_id = int(prim_id[-i:], 16)

                file_id_bit = int(G_PARAMS['HDB_PRIM_XML_FILE_ID_BIT'])
                line_id_bit = int(G_PARAMS['HDB_PRIM_XML_LINE_ID_BIT'])
                print_level_bit = int(G_PARAMS['HDB_PRIM_XML_PRINT_LEVEL_BIT'])
                if (file_id_bit + line_id_bit + print_level_bit) != 32:
                    msg = "[error] [mk_prim_xml_step2] log ID not is 32bit!"
                    raise ParserError(msg)
                line_bit = 1 << print_level_bit

                i = 1
                while i < line_id_bit:
                    line_bit = line_bit | (line_bit << 1)
                    i = i + 1

                xml_id = \
                    (int(prim_file_id) << (print_level_bit + line_id_bit)) | (
                            (real_id << print_level_bit) & line_bit) | (real_pri)
                new_element = ET.Element('')
                new_element.tag = 'MSG'
                new_element.attrib['STRUCTURE'] = prim_match_type
                new_element.attrib['NAME'] = \
                    '{} @{}({}),{}'.format(prim_sz, prim_file, prim_line, pri_str)
                new_element.attrib['ID'] = hex(xml_id)
                new_element.tail = '\n\t\t\t'
                msg_root_element.append(new_element)
            else:
                pass


def add_content_files_to_xml(base_file_dir, msg_root_element):
    if os.path.isdir(base_file_dir):
        file_name_list = os.listdir(base_file_dir)
        for file_name in file_name_list:
            full_file_name = os.path.join(base_file_dir, file_name)
            add_content_to_xml(full_file_name, msg_root_element)


def get_subsystem_by_name(tree, name):
    root = tree.getroot()
    for child in root:
        if child.attrib["NAME"] == name:
            return child


def write_to_target(tree_target, subsystem, name):
    root = tree_target.getroot()
    for child in root:
        if child.attrib["NAME"] == name:
            root.remove(child)
            root.append(subsystem)
            break

def bak_db_xml(merge_src_xml, merge_bak_xml):
    merge_src_xml_hash = get_file_sha256(merge_src_xml)
    merge_back_xml_hash = get_file_sha256(merge_bak_xml)
    if merge_src_xml_hash != merge_back_xml_hash:
        cmd_list = []
        cmd_list.append('cp')
        cmd_list.append(merge_src_xml)
        cmd_list.append(merge_bak_xml)
        subprocess.run(cmd_list, shell=False)


def merge_db_xml(merge_src_xml, merge_bak_xml):
    core_name = G_PARAMS.get('CORE_NAME').strip()
    if not os.path.isfile(merge_bak_xml):
        return
    tree_src_xml = ET.parse(merge_src_xml)
    tree_bak_xml = ET.parse(merge_bak_xml)
    subsystem_src = get_subsystem_by_name(tree_src_xml, core_name)
    subsystem_bak = get_subsystem_by_name(tree_bak_xml, core_name)
    msg_src_element = get_msg_root(subsystem_src)
    msg_bak_element = get_msg_root(subsystem_bak)
    no_need_merge_file = set()
    need_merge_file = set()
    is_need_merge = True

    for child in msg_bak_element:
        file_nam_bak = re.search(r'@.+?\.c', child.get("NAME"), re.DOTALL)
        if not file_nam_bak:
            continue
        if file_nam_bak.group()[1:-2] in need_merge_file:
            continue
        is_need_merge = True
        if file_nam_bak not in no_need_merge_file:
            for child_src in msg_src_element:
                file_name_src = re.search(r'@.+?\.c', child_src.get("NAME"), re.DOTALL)
                if file_name_src and file_nam_bak.group()[1:-2] == file_name_src.group()[1:-2]:
                    no_need_merge_file.add(file_nam_bak.group()[1:-2])
                    is_need_merge = False
                    break
        if is_need_merge:
            need_merge_file.add(file_nam_bak.group()[1:-2])
    if len(need_merge_file) > 0:
        for child in msg_bak_element:
            file_nam_bak = re.search(r'@.+?\.c', child.get("NAME"), re.DOTALL)
            if not file_nam_bak:
                continue
            if file_nam_bak.group()[1:-2] in need_merge_file:
                new_element = ET.Element('')
                new_element.tag = 'MSG'
                new_element.attrib['STRUCTURE'] = child.get("STRUCTURE")
                new_element.attrib['NAME'] = child.get("NAME")
                new_element.attrib['ID'] = child.get("ID")
                new_element.tail = '\n\t\t\t'
                msg_src_element.append(new_element)
        write_prim_xml_tree(tree_src_xml, merge_src_xml)


def merge_or_bak_db_xml():
    base_dir = G_PARAMS.get('BASE_DIR')
    source_dir = os.path.join(base_dir, "utils/net/plc")
    component_file_num = 0
    for root, _dirs, files in os.walk(source_dir):
        for file in fnmatch.filter(files, "component.json"):
            component_file_num = component_file_num + 1
    bak_xml_dir = G_PARAMS.get('HDB_PRIM_XML_BAK_FILE').strip()
    dst_xml_dir = G_PARAMS.get('HDB_PRIM_XML_DST_FILE').strip()
    
    cores = G_PARAMS.get('DATABASE_VERSION_CORE').split()
    for core in cores:
        # 编译新生成的XML
        merge_src_xml = os.path.join(dst_xml_dir, core, 'hdbcfg', 'mss_prim_db.xml')
        # 源码编译备份的XML
        merge_bak_xml = os.path.join(bak_xml_dir, core, 'mss_prim_db.xml')
        if component_file_num > 0: # source code compilation
            bak_db_xml(merge_src_xml, merge_bak_xml)
        else:
            merge_db_xml(merge_src_xml, merge_bak_xml)


def generate_db_xml(base_dir, in_path, target):
    global G_PARAMS
    core_name = "apps_core"
    if os.path.isfile(in_path) is False:
        raise Exception("Can NOT found the database configure file:%s!\n", in_path)

    # get global settings
    db_conf = None
    conf = os.path.join(base_dir, in_path)
    with open(conf, 'r') as f:
        db_conf = json.load(f)

    if core_name is None or db_conf is None:
        return

    check_key(db_conf, conf)

    G_PARAMS = {}
    G_PARAMS['CORE_NAME'] = core_name
    G_PARAMS['BASE_DIR'] = base_dir
    G_PARAMS['HDB_XML_TEMP_BASE_DIR'] = os.path.join(base_dir, db_conf["HDB_XML_TEMP_BASE_DIR"])
    G_PARAMS['HDB_PRIM_XML_DST_FILE'] = os.path.join(base_dir, db_conf["HDB_PRIM_XML_DST_FILE"])
    G_PARAMS
    keys = dict.keys(db_conf)
    if 'HDB_PRIM_XML_BAK_FILE' in keys:
        G_PARAMS['HDB_PRIM_XML_BAK_FILE'] = os.path.join(base_dir, db_conf["HDB_PRIM_XML_BAK_FILE"])
    else :
        G_PARAMS['HDB_PRIM_XML_BAK_FILE'] = base_dir
    G_PARAMS['HDB_PRIM_XML_SRC_FILE'] = db_conf["HDB_PRIM_XML_SRC_FILE"]
    G_PARAMS['HDB_CMD_XML_SRC_FILE'] = os.path.join(base_dir, db_conf["HDB_CMD_XML_SRC_FILE"])
    G_PARAMS['HDB_PRIM_XML_FILE_ID_BIT'] = db_conf["HDB_PRIM_XML_FILE_ID_BIT"]
    G_PARAMS['HDB_PRIM_XML_LINE_ID_BIT'] = db_conf["HDB_PRIM_XML_LINE_ID_BIT"]
    G_PARAMS['HDB_PRIM_XML_PRINT_LEVEL_BIT'] = db_conf["HDB_PRIM_XML_PRINT_LEVEL_BIT"]
    G_PARAMS['LOG_FILE_NAME'] = os.path.join(base_dir, db_conf["LOG_FILE_NAME"], "base.log")

    if 'cco' in target.lower():
        G_PARAMS['HDB_CMD_XML_DST_FILE'] = os.path.join(base_dir, db_conf["CCO_HDB_CMD_XML_DST_FILE"])
        G_PARAMS['DATABASE_VERSION_CORE'] = 'cco'
    elif 'sta' in target.lower():
        G_PARAMS['HDB_CMD_XML_DST_FILE'] = os.path.join(base_dir, db_conf["STA_HDB_CMD_XML_DST_FILE"])
        G_PARAMS['DATABASE_VERSION_CORE'] = 'sta'
    elif 'ndm' in target.lower():
        G_PARAMS['HDB_CMD_XML_DST_FILE'] = os.path.join(base_dir, db_conf["NDM_HDB_CMD_XML_DST_FILE"])
        G_PARAMS['DATABASE_VERSION_CORE'] = 'ndm'
    else:
        G_PARAMS['TARGET'] = None
        G_PARAMS['HDB_CMD_XML_DST_FILE'] = os.path.join(base_dir, db_conf["HDB_CMD_XML_DST_FILE"])
        G_PARAMS['DATABASE_VERSION_CORE'] = db_conf["DATABASE_VERSION_CORE"]

    xml_src_dir = G_PARAMS.get('HDB_PRIM_XML_SRC_FILE').strip()
    dst_xml_dir = G_PARAMS.get('HDB_PRIM_XML_DST_FILE').strip()
    cores = G_PARAMS['DATABASE_VERSION_CORE'].split()
    for core in cores:
        xml_src_file = os.path.join(xml_src_dir, core, "system", "hdbcfg", "mss_prim_db.xml")
        dst_xml_dir = os.path.join(dst_xml_dir, core, "hdbcfg")
        if not os.path.isdir(dst_xml_dir):
            os.makedirs(dst_xml_dir)
        dst_xml_file = os.path.join(dst_xml_dir, "mss_prim_db.xml")
        if not os.path.exists(dst_xml_file):
            shutil.copy(xml_src_file, dst_xml_file)
        core_name = G_PARAMS['CORE_NAME']
        base_file_dir = \
            os.path.join(G_PARAMS['HDB_XML_TEMP_BASE_DIR'].strip(), core_name)

        log_file_name = G_PARAMS['LOG_FILE_NAME'].strip()
        log_file_dir = os.path.dirname(log_file_name)
        if not os.path.isdir(log_file_dir):
            os.makedirs(log_file_dir)

        log_file_fp = file_open(log_file_name, 'w+')
        last_print_target = sys.stdout
        sys.stdout = log_file_fp
        tree_src = ET.parse(xml_src_file)
        subsystem = get_subsystem_by_name(tree_src, core_name)
        msg_root_element = get_msg_root(subsystem)
        add_content_files_to_xml(base_file_dir, msg_root_element)
        tree_target = ET.parse(dst_xml_file)
        write_to_target(tree_target, subsystem, core_name)
        write_prim_xml_tree(tree_target, dst_xml_file)
        sys.stdout = last_print_target
        log_file_fp.close()

