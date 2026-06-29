#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
# ============================================================================
# @brief    packet files
# ============================================================================

import os
import sys
import re
import shutil
from xml.etree import ElementTree


PY_PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.append(PY_PATH)
PKG_DIR = os.path.dirname(PY_PATH)
PKG_DIR = os.path.dirname(PKG_DIR)

from packet_create import create_sha_file
from packet_create import packet_bin

TOOLS_DIR = os.path.dirname(PKG_DIR)
SDK_DIR = os.path.dirname(TOOLS_DIR)
sys.path.append(os.path.join(SDK_DIR, "build", "script"))

# cat1
def create_database():
    acore_message_xml_path = os.path.join(SDK_DIR, "output", "cat1", "acore", "application-cat1", "acore_messages.xml")
    pcore_message_xml_path = os.path.join(SDK_DIR, "output", "cat1", "protocol_core", "protocol-cat1", "protocol_core_messages.xml")
    message_xml_path = os.path.join(SDK_DIR, "output", "cat1", "messages.xml")
    merge_message_script_path = os.path.join(SDK_DIR, "build", "script", "hdbxml_custom", "MessageXmlGen", "merge_messages.py")
    create_database_script_path = os.path.join(SDK_DIR, "build", "script", "hdbxml_custom", "database_create", "database_create.py")
    mss_prim_db_old_xml_path = os.path.join(SDK_DIR, "build", "script", "hdbxml_custom", "database_create", "mss_prim_db_old.xml")
    database_output_path = os.path.join(SDK_DIR, "output", "cat1", "database_create_cat1")
    if os.system(sys.executable + " " + merge_message_script_path + " " + message_xml_path + " " + acore_message_xml_path + " " + pcore_message_xml_path) != 0:
        sys.exit(-3)
    if os.system(sys.executable + " " + create_database_script_path + " " + message_xml_path + " " + mss_prim_db_old_xml_path + " " + database_output_path) != 0:
        sys.exit(-4)
    database_evb_mss_prim_db_xml_path = os.path.join(SDK_DIR, "output", "cat1", "database_cat1_evb", "cco", "system", "hdbcfg", "mss_prim_db.xml")
    database_evb_diag_path = os.path.join(SDK_DIR, "output", "cat1", "database_cat1_evb", "cco", "system", "diag")
    database_cat1_mss_prim_db_xml_path = os.path.join(SDK_DIR, "output", "cat1", "database_create_cat1", "cco", "system", "hdbcfg", "mss_prim_db.xml")
    database_cat1_diag_path = os.path.join(SDK_DIR, "output", "cat1", "database_create_cat1", "cco", "system", "diag")
    tree = ElementTree.parse(database_evb_mss_prim_db_xml_path)
    root = tree.getroot()
    data = ElementTree.parse(database_cat1_mss_prim_db_xml_path).getroot()
    sub_data = data.findall("SUBSYSTEM")
    root.extend(sub_data)
    with open(database_evb_mss_prim_db_xml_path, 'wb') as output_file_handle:
        tree.write(output_file_handle, encoding='utf-8', method='xml', xml_declaration=True)
    for dirname, _, files in os.walk(database_cat1_diag_path):
        for filename in files:
            shutil.copy(os.path.join(dirname, filename), os.path.join(dirname.replace(database_cat1_diag_path, database_evb_diag_path), filename))

def make_all_in_one_packet(pack_style_str):
    # make all in one packet
    create_database()
    if pack_style_str.endswith("tts"):
        fwpkg_name = "Hi2131V100.fwpkg"
    elif pack_style_str.endswith("ims"):
        fwpkg_name = "Hi2131EV100.fwpkg"
    else:
        fwpkg_name = "Hi2131LV100.fwpkg"
    if os.system("sh " + os.path.join(
        SDK_DIR, "build", "config", "target_config", "cat1", "sign_config", "params_and_bin_sign.sh") + " " + pack_style_str) != 0:
        sys.exit(-2)
    if pack_style_str.startswith("pack"):
        bin_dir = os.path.join(SDK_DIR, "output", "package", "cat1", pack_style_str)
        application_dir = bin_dir
        protocol_dir = bin_dir
        ate_dir = bin_dir
        partition_dir = bin_dir
        nv_dir =  bin_dir
        cat1_fwpkg = os.path.join(bin_dir, fwpkg_name)
    else:
        bin_dir = os.path.join(SDK_DIR, "interim_binary", "cat1", "bin", "boot_bin")
        ate_dir = os.path.join(SDK_DIR, "output", "cat1", "acore", pack_style_str)
        partition_dir = os.path.join(SDK_DIR, "interim_binary", "cat1", "bin", "partition", pack_style_str)
        nv_dir =  os.path.join(SDK_DIR, "interim_binary", "cat1", "bin", "nv", pack_style_str)
        application_dir = os.path.join(SDK_DIR, "output", "cat1", "acore", pack_style_str)
        protocol_dir = os.path.join(SDK_DIR, "output", "cat1", "acore", pack_style_str)
        cat1_fwpkg = os.path.join(SDK_DIR, "output", "cat1", "fwpkg", pack_style_str, fwpkg_name)

    partition = os.path.join(partition_dir, "partition_sign.bin")
    loadboot = os.path.join(bin_dir, "loaderboot_sign.bin")
    flashboot = os.path.join(bin_dir, "flashboot_sign_a.bin")
    flashboot_bak = os.path.join(bin_dir, "flashboot_sign_b.bin")
    nv = os.path.join(nv_dir, "cat1_all_nv.bin")
    app = os.path.join(application_dir, "application_sign.bin")
    pro = os.path.join(protocol_dir, "protocol_sign.bin")
    rf = ""

    loadboot_bx = loadboot + "|0x0|0x0|0"
    partition_bx = partition + "|0x400000|0x1000|1"
    flashboot_bx = flashboot + "|0x401000|0xF000|8"
    flashboot_bak_bx = flashboot_bak + "|0x410000|0xF000|8"
    pro_bx = pro + "|0x41F000|0x50000|11"
    rf_bx = rf + "|0x46F000|0x8000|13"
    app_bx = app + "|0x477000|0x145000|12"
    nv_bx =  nv + "|0x5F5000|0x6000|2"

    try:
        packet_post_agvs = list()
        packet_post_agvs.append(loadboot_bx)
        packet_post_agvs.append(partition_bx)
        packet_post_agvs.append(flashboot_bx)
        packet_post_agvs.append(flashboot_bak_bx)
        packet_post_agvs.append(app_bx)
        packet_post_agvs.append(pro_bx)
        packet_post_agvs.append(nv_bx)
        packet_bin(cat1_fwpkg, packet_post_agvs)

        if not pack_style_str.startswith("ate") and not pack_style_str.startswith("pack"):
            packet_post_agvs = list()
            packet_post_agvs.append(loadboot_bx)
            packet_post_agvs.append(partition_bx)
            packet_post_agvs.append(flashboot_bx)
            packet_post_agvs.append(flashboot_bak_bx)
            packet_post_agvs.append(app_bx)
            packet_post_agvs.append(pro_bx)
            packet_post_agvs.append(nv_bx)
            packet_bin(cat1_ide_fwpkg, packet_post_agvs)

    except Exception as e:
        print(e)
        exit(-1)


def is_packing_files_exist(soc, pack_style_str):
    """
    判断打包文件是否存在
    :return:
    """
    packing_files = get_packing_files(soc, pack_style_str)
    lost_files = list()
    for f_path in packing_files:
        if not os.path.isfile(f_path):
            lost_files.append(f_path)
    return lost_files

def get_packing_files(soc, pack_style_str):
    """
    直接添加需要打包的文件路径
    :return:
    """
    packing_files = list()            
    return packing_files
