#!/usr/bin/env python3
# encoding=utf-8
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

from utils.build_utils import exec_shell

# hi2132
def create_database(apps_hdb_xml_dir, protocol_hdb_xml_dir, pack_style_str):
    acore_message_xml_path = os.path.join(SDK_DIR, "output", "hi2132", "acore", apps_hdb_xml_dir, "acore_messages.xml")
    pcore_message_xml_path = os.path.join(SDK_DIR, "output", "hi2132", "protocol_core", protocol_hdb_xml_dir, "protocol_core_messages.xml")
    message_xml_path = os.path.join(SDK_DIR, "output", "hi2132", "messages.xml")
    merge_message_script_path = os.path.join(SDK_DIR, "build", "script", "hdbxml_custom", "MessageXmlGen", "merge_messages.py")
    create_database_script_path = os.path.join(SDK_DIR, "build", "script", "hdbxml_custom", "database_create", "database_create.py")
    mss_prim_db_old_xml_path = os.path.join(SDK_DIR, "build", "script", "hdbxml_custom", "database_create", "mss_prim_db_old.xml")
    database_output_path = os.path.join(SDK_DIR, "output", "hi2132", "database_create_hi2132")
    if exec_shell([sys.executable, merge_message_script_path, message_xml_path, acore_message_xml_path, pcore_message_xml_path], cmd_dump=True) != 0:
        sys.exit(-6)
    if exec_shell([sys.executable, create_database_script_path, message_xml_path, mss_prim_db_old_xml_path, database_output_path], cmd_dump=True) != 0:
        sys.exit(-7)
    database_evb_mss_prim_db_xml_path = os.path.join(SDK_DIR, "output", "hi2132", "database_hi2132_evb", "cco", "system", "hdbcfg", "mss_prim_db.xml")
    database_evb_diag_path = os.path.join(SDK_DIR, "output", "hi2132", "database_hi2132_evb", "cco", "system", "diag")
    database_evb_nv_out_path = os.path.join(SDK_DIR, "output", "hi2132", "database_hi2132_evb", "cco", "system", "nv")
    database_evb_hdbcfg_out_path = os.path.join(SDK_DIR, "output", "hi2132", "database_hi2132_evb", "cco", "system", "hdbcfg")
    database_hi2132_mss_prim_db_xml_path = os.path.join(SDK_DIR, "output", "hi2132", "database_create_hi2132", "cco", "system", "hdbcfg", "mss_prim_db.xml")
    database_hi2132_diag_path = os.path.join(SDK_DIR, "output", "hi2132", "database_create_hi2132", "cco", "system", "diag")
    database_hi2132_nv_path = os.path.join(SDK_DIR, "output", "hi2132", "database_hi2132_nv", "cco", "system", "nv")
    database_hi2132_hdbcfg_path = os.path.join(SDK_DIR, "output", "hi2132", "database_hi2132_nv", "cco", "system", "hdbcfg")
    tree = ElementTree.parse(database_evb_mss_prim_db_xml_path)
    root = tree.getroot()
    data = ElementTree.parse(database_hi2132_mss_prim_db_xml_path).getroot()
    sub_data = data.findall("SUBSYSTEM")
    root.extend(sub_data)
    with open(database_evb_mss_prim_db_xml_path, 'wb') as output_file_handle:
        tree.write(output_file_handle, encoding='utf-8', method='xml', xml_declaration=True)
    for dirname, _, files in os.walk(database_hi2132_diag_path):
        for filename in files:
            shutil.copy(os.path.join(dirname, filename), os.path.join(dirname.replace(database_hi2132_diag_path, database_evb_diag_path), filename))
    for dirname, _, files in os.walk(database_hi2132_nv_path):
        for filename in files:
            shutil.copy(os.path.join(dirname, filename), os.path.join(dirname.replace(database_hi2132_nv_path, database_evb_nv_out_path), filename))
    for dirname, _, files in os.walk(database_hi2132_hdbcfg_path):
        for filename in files:
            shutil.copy(os.path.join(dirname, filename), os.path.join(dirname.replace(database_hi2132_hdbcfg_path, database_evb_hdbcfg_out_path), filename))
    if os.path.exists(os.path.join(SDK_DIR, "output", "hi2132", "database_hi2132_nv")):
        shutil.rmtree(os.path.join(SDK_DIR, "output", "hi2132", "database_hi2132_nv"))
    if os.path.exists(os.path.join(SDK_DIR, "output", "hi2132", "database_create_hi2132")):
        shutil.rmtree(os.path.join(SDK_DIR, "output", "hi2132", "database_create_hi2132"))
    database_cat_path = os.path.join(SDK_DIR, "output", "hi2132", "database_hi2132_evb")
    database_ver_path = os.path.join(SDK_DIR, "build", "config", "target_config", "hi2132", "hdb_config", "database_cfg", "database_ver.txt")
    shutil.copy(database_ver_path, os.path.join(database_cat_path, "database_ver.txt"))
    database_path = os.path.join(SDK_DIR, "output", "package", "hi2132", pack_style_str, "database")
    if os.path.exists(database_path):
        shutil.rmtree(database_path)
    shutil.copytree(database_cat_path, database_path)
    database_7z_path = os.path.join(SDK_DIR, "output", "package", "hi2132", pack_style_str, "database.7z")
    z7_path = os.path.join(SDK_DIR, "tools", "bin", "7z2301-linux-x64", "7zz")
    if exec_shell([z7_path, "a", "-mtm=off", database_7z_path, database_path]) != 0:
        sys.exit(-8)

def add_efuse_info(pack_style_str):
    efuse_cfg = os.path.join(SDK_DIR, "output", "package", "hi2132", pack_style_str, "EFUSE.bin")
    efuse_cfg_gen = os.path.join(SDK_DIR, "tools", "bin", "sign_tool", "efuse_tool", "efuse_cfg_gen.py")
    efuse_oem = os.path.join(SDK_DIR, "tools", "bin", "sign_tool", "efuse_tool", "efuse_oem.csv")
    if exec_shell([sys.executable, efuse_cfg_gen, efuse_oem, efuse_cfg]) != 0:
        sys.exit(-9)

def check_flash_size(bin_path, remaining_min_flash_size, flash_size):
    image_size = os.path.getsize(bin_path)
    print("INFO: bin_path: %s" % bin_path)
    print("INFO: bin_size: %d" % image_size)
    print("INFO: flash_size: %d" % flash_size)
    print("INFO: remaining_flash_size: %d" % (flash_size - image_size))
    print("INFO: remaining_min_flash_size: %d" % remaining_min_flash_size)
    if flash_size - image_size >=  remaining_min_flash_size:
        print("INFO: Succeeded in checking the remaining size of the flash memory.")
        return True
    else:
        print("ERROR: The size of the %s file is greater than %dB!!!" % (bin_path, (flash_size - remaining_min_flash_size)))
        return False

def make_all_in_one_packet(pack_style_str, build_extr_defines):
    # apps_hdb_xml_dir, protocol_hdb_xml_dir, otp_flag, support_4m_flag,\
    # remaining_min_flash_size, version_string, sign_key_flag, build_defines = args
    # make all in one packet
    # add_efuse_info(pack_style_str)
    # if not 'testsuite-hi2132' in apps_hdb_xml_dir:
    #     create_database(apps_hdb_xml_dir, protocol_hdb_xml_dir, pack_style_str)
    if pack_style_str.endswith("tts") or pack_style_str.endswith("tts_fpga"):
        fwpkg_name = "Hi2132V100.fwpkg"
    elif pack_style_str.endswith("ims") or pack_style_str.endswith("ims_fpga"):
        fwpkg_name = "Hi2132EV100.fwpkg"
    elif pack_style_str.endswith("tts-xts"):
        fwpkg_name = "Hi2132V100_XTS.fwpkg"
    elif pack_style_str.endswith("ims-xts"):
        fwpkg_name = "Hi2132EV100_XTS.fwpkg"
    elif pack_style_str.endswith("xts"):
        fwpkg_name = "Hi2132CV100_XTS.fwpkg"
    else:
        fwpkg_name = "Hi2132CV100.fwpkg"
    params_sign_config = os.path.join(SDK_DIR, "build", "config", "target_config", "hi2132", "params_sign_config.py")
    if exec_shell([sys.executable, params_sign_config, pack_style_str], cmd_dump=True) != 0:
        sys.exit(-2)
    # tlv_table_process = os.path.join(SDK_DIR, "tools", "rf_calibration_and_test_tools", "user_tools", "tools.py")
    # if exec_shell([sys.executable, tlv_table_process, "-build"], cmd_dump=True) != 0:
    #     sys.exit(-3)
    if pack_style_str.startswith("pack"):
        bin_dir = os.path.join(SDK_DIR, "output", "package", "hi2132", pack_style_str)
        tlv_dir = os.path.join(SDK_DIR, "output")
        application_dir = bin_dir
        protocol_dir = bin_dir
        ate_dir = bin_dir
        partition_dir = bin_dir
        nv_dir =  bin_dir
        efuse_dir = bin_dir
        customloader_dir = bin_dir
        hi2132_fwpkg = os.path.join(bin_dir, fwpkg_name)
    else:
        bin_dir = os.path.join(SDK_DIR, "interim_binary", "hi2132", "bin", "boot_bin")
        ate_dir = os.path.join(SDK_DIR, "output", "hi2132", "acore", pack_style_str)
        partition_dir = os.path.join(SDK_DIR, "interim_binary", "hi2132", "bin", "partition", pack_style_str)
        nv_dir =  os.path.join(SDK_DIR, "interim_binary", "hi2132", "bin", "nv", pack_style_str)
        application_dir = os.path.join(SDK_DIR, "output", "hi2132", "acore", pack_style_str)
        protocol_dir = os.path.join(SDK_DIR, "output", "hi2132", "acore", pack_style_str)
        efuse_dir = os.path.join(SDK_DIR, "output", "hi2132", "acore", pack_style_str)
        hi2132_fwpkg = os.path.join(SDK_DIR, "output", "hi2132", "fwpkg", pack_style_str, fwpkg_name)

    partition = os.path.join(partition_dir, "partition_sign.bin")
    loadboot = os.path.join(bin_dir, "loaderboot_sign.bin")
    # if "SUPPORT_CUSTOM_LOADER" in build_defines:
    #     if sign_key_flag == "True":
    #         flashboot = os.path.join(SDK_DIR, "build", "precompiled", "flashboot", "customer", "key", "flashboot_sign_a.bin")
    #     else:
    #         flashboot = os.path.join(SDK_DIR, "build", "precompiled", "flashboot", "customer", "normal", "flashboot_sign_a.bin")
    #     customloader = os.path.join(customloader_dir, "customloader.bin")
    # elif "SUPPORT_SOFT_CONTROL_UPGRADE" in build_defines:
    #     if sign_key_flag == "True":
    #         flashboot = os.path.join(SDK_DIR, "build", "precompiled", "flashboot", "upgrade", "key", "flashboot_sign_a.bin")
    #     else:
    #         flashboot = os.path.join(SDK_DIR, "build", "precompiled", "flashboot", "upgrade", "normal", "flashboot_sign_a.bin")
    # else:
    #     if sign_key_flag == "True":
    #         flashboot = os.path.join(SDK_DIR, "build", "precompiled", "flashboot", "original", "key", "flashboot_sign_a.bin")
    #     else:
    #         flashboot = os.path.join(SDK_DIR, "build", "precompiled", "flashboot", "original", "normal", "flashboot_sign_a.bin")        
     
    flashboot = os.path.join(bin_dir, "flashboot_sign_a.bin")
    nv = os.path.join(nv_dir, "hi2132_all_nv.bin")
    app = os.path.join(application_dir, "application_sign.bin")
    pro = os.path.join(protocol_dir, "protocol_sign.bin")
    # efuse = os.path.join(efuse_dir, "EFUSE.bin")
    # rf_bom = os.path.join(tlv_dir, "RadioConfig.tlv")
    # database = os.path.join(bin_dir, "database.7z")

    # if remaining_min_flash_size.startswith("0x") or remaining_min_flash_size.startswith("0X"):
    #     remaining_min_flash_size = int(remaining_min_flash_size, 16)
    # else:
    #     remaining_min_flash_size = int(remaining_min_flash_size)

    # if support_4m_flag == "True":
    #     # if not pack_style_str.endswith("xts"):
    #     #     if not check_flash_size(app, remaining_min_flash_size, 0x331000):
    #     #         sys.exit(-4)
    #     loadboot_bx = loadboot + "|0x0|0x0|0"
    #     partition_bx = partition + "|0x400000|0x1000|1"
    #     flashboot_bx = flashboot + "|0x401000|0x18000|8"
    #     if "SUPPORT_CUSTOM_LOADER" in build_defines:
    #         customloader_bx = customloader + "|0x411000|0x8000|9"
    #     pro_bx = pro + "|0x431000|0x50000|11"
    #     rf_bom_bx = rf_bom + "|0x481000|0x8000|13"
    #     app_bx = app + "|0x489000|0x362000|12"
    #     nv_bx =  nv + "|0x13EF000|0x9000|2"
    #     efuse_bx = efuse + "|0x0|0x0|4"
    #     database_bx = database + "|0xF0000000|0x0|16"
    # else:
        # if not pack_style_str.endswith("xts"):
        #     if not check_flash_size(app, remaining_min_flash_size, 0x145000):
        #         sys.exit(-5)
    loadboot_bx = loadboot + "|0x0|0x0|0"
    partition_bx = partition + "|0x1000000|0x1000|1"
    flashboot_bx = flashboot + "|0x1002000|0x1F000|8"
    pro_bx = pro + "|0x1040000|0x87000|11"
    # rf_bom_bx = rf_bom + "|0x481000|0x8000|13"
    app_bx = app + "|0x10D0000|0x160000|12"
    nv_bx =  nv + "|0x13EF000|0x9000|2"
    # efuse_bx = efuse + "|0x0|0x0|4"
    # database_bx = database + "|0xF0000000|0x0|16"

    try:
        packet_post_agvs = list()
        packet_post_agvs.append(loadboot_bx)
        packet_post_agvs.append(partition_bx)
        packet_post_agvs.append(flashboot_bx)       
        packet_post_agvs.append(app_bx)
        packet_post_agvs.append(pro_bx)
        packet_post_agvs.append(nv_bx)
        # packet_post_agvs.append(rf_bom_bx)
        # if otp_flag == "True":
        #     packet_post_agvs.append(efuse_bx)
        # if not 'testsuite-hi2132' in apps_hdb_xml_dir:
        #     packet_post_agvs.append(database_bx)
        # packet_post_agvs.append("-version:" + version_string)
        packet_bin(hi2132_fwpkg, packet_post_agvs)

        if not pack_style_str.startswith("ate") and not pack_style_str.startswith("pack"):
            packet_post_agvs = list()
            packet_post_agvs.append(loadboot_bx)
            packet_post_agvs.append(partition_bx)
            packet_post_agvs.append(flashboot_bx)
            packet_post_agvs.append(app_bx)
            packet_post_agvs.append(pro_bx)
            packet_post_agvs.append(nv_bx)
            # packet_post_agvs.append(rf_bom_bx)
            packet_bin(hi2132_ide_fwpkg, packet_post_agvs)

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
