#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.

import os
import re
import sys
import platform
import string
import shutil
import time
import json
import xml.etree.ElementTree as ET
from xml.etree.ElementTree import ElementTree

XML_PATH="build/config/target_config/<chip>/hdb_config/database_cfg/mk_hdb_xml.json"

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

def get_subsystem_by_name(tree, name):
    root = tree.getroot()
    for child in root:
        if child.attrib["NAME"] == name:
            return child
    return None

def merge_db_xml(root, chip):
    xml_src_file = G_PARAMS['HDB_PRIM_XML_SRC_FILE'].strip()
    dst_xml_file = G_PARAMS['HDB_PRIM_XML_DST_FILE'].strip()
    base_dir     = G_PARAMS['HDB_XML_TEMP_BASE_DIR']

    dst_xml_dir = os.path.abspath(os.path.join(dst_xml_file, ".."))
    os.makedirs(dst_xml_dir, exist_ok=True)

    # 1. 目标文件不存在时拷贝模板
    if not os.path.exists(dst_xml_file):
        shutil.copy(xml_src_file, dst_xml_file)

    # 2. 解析一次目标 XML
    tree_dst = ET.parse(dst_xml_file)
    root_dst = tree_dst.getroot()

    # 3. 建立子系统 name → <SUBSYSTEM> 节点的映射（一次遍历根节点）
    subsystem_map = {}
    for sub in root_dst:
        name = sub.attrib.get("NAME")
        if name:
            if name in subsystem_map:
                print(f"Warning: Duplicate subsystem name '{name}' detected")
            subsystem_map[name] = sub

    # --------------------------------------------------------------
    # 4. 预读取所有子模块列表
    # --------------------------------------------------------------
    mod_list = sorted(
        [m for m in os.listdir(base_dir) if os.path.isdir(os.path.join(base_dir, m))]
    )
    print("merge_db_xml: mod_list =", mod_list)

    # --------------------------------------------------------------
    # 5. 循环合并子模块
    # --------------------------------------------------------------
    for mod in mod_list:
        subsystem = subsystem_map.get(mod)
        if subsystem is None:
            print(f"Error: subsystem {mod} is not in {dst_xml_file}")
            continue

        # 取得该子系统下的 MSG_LOG 节点并清空
        msg_root_element = get_msg_root(subsystem)
        msg_root_element.clear()

        # 读取该子模块下的所有临时 XML
        mod_path = os.path.join(base_dir, mod)
        filelist = sorted(os.listdir(mod_path))
        for filename in filelist:
            tmp_xml_file = os.path.join(mod_path, filename)

            tree_src = ET.parse(tmp_xml_file)
            root_src = tree_src.getroot()
            # 开头是MSG_LOG的情况，对应文件是单个文件的xml的情况
            if root_src.tag == 'MSG_LOG':
                for child in root_src:
                    msg_root_element.append(child)
            else:
                # 开头不是MSG_LOG的情况，对应文件是mss_prim_db.xml的情况
                subsystem_src = get_subsystem_by_name(tree_src, mod)
                if subsystem_src is None:
                    print(f"Error: subsystem {mod} not found in {filename}")
                    continue
                msg_element_src = get_msg_root(subsystem_src)
                for child in msg_element_src:
                    msg_root_element.append(child)
        # 子模块合并完毕后，继续下一个子模块（暂不写目标）

    # --------------------------------------------------------------
    # 6. 循环结束后一次性写回目标 XML
    # --------------------------------------------------------------
    tree_dst.write(dst_xml_file, encoding='utf-8', xml_declaration=True)

# main
if __name__ == "__main__":
    global G_PARAMS

    root = sys.argv[1]
    chip = sys.argv[2]

    in_path = XML_PATH
    in_path = in_path.replace('<chip>', chip)

    db_conf = None
    conf = os.path.join(root, in_path)
    with open(conf, 'r') as f:
        db_conf = json.load(f)

    G_PARAMS = {}
    G_PARAMS['HDB_XML_TEMP_BASE_DIR'] = os.path.join(root, db_conf["HDB_XML_TEMP_BASE_DIR"])
    G_PARAMS['HDB_PRIM_XML_DST_FILE'] = os.path.join(root, db_conf["HDB_PRIM_XML_DST_FILE"])
    G_PARAMS['HDB_PRIM_XML_SRC_FILE'] = os.path.join(root, db_conf["HDB_PRIM_XML_SRC_FILE"])

    merge_db_xml(root, chip)