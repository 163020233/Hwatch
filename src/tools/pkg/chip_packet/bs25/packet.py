#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
# ============================================================================
# @brief    packet files
# ============================================================================

import os
import sys
import shutil

PY_PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.append(PY_PATH)
PKG_DIR = os.path.dirname(PY_PATH)
PKG_DIR = os.path.dirname(PKG_DIR)

from packet_create import create_sha_file
from packet_create import packet_bin

TOOLS_DIR = os.path.dirname(PKG_DIR)
SDK_DIR = os.path.dirname(TOOLS_DIR)
sys.path.append(os.path.join(SDK_DIR, "build", "script"))
FOTA_PATH = os.path.join(SDK_DIR, "build", "config", "target_config", "bs25", "fota")
sys.path.append(FOTA_PATH)

from fota_create import pack_fota
pack_fota_in_all = False

def make_fota(PKG_DIR, pack_style_str, fota_bin):
    out_dir = os.path.join(PKG_DIR, "fwpkg", "bs25", "fota_temp")
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
    for item in fota_bin:
        shutil.copy(item, out_dir)
    fota = os.path.join(PKG_DIR, "fwpkg", "bs25", "ota_dfu_fota.bin")

    if pack_style_str.endswith("flash"):
        if "in_ex" in pack_style_str:  # SUPPORT_IN_EX_FLASH
            flash_type = "in_ex_flash"
        else:
            flash_type = "double_flash"
    else:
        flash_type = "embed_flash"
    pack_fota(flash_type, bin_root_path=out_dir, out_bin_path=fota)  # packet to FOTA.bin
    return fota

# bs25
def make_all_in_one_packet(pack_style_str):
    # make all in one packet
    if pack_style_str.startswith("pack"):
        bin_dir = os.path.join(SDK_DIR, "output", "package", "bs25", pack_style_str)
        application_path = os.path.join(bin_dir, "application.bin")
        partition = os.path.join(bin_dir, "partition.bin")
    else:
        application_path = os.path.join(SDK_DIR,
                                    "output",
                                    "bs25",
                                    "acore",
                                    pack_style_str,
                                    "application.bin")
        bin_dir = os.path.join(SDK_DIR, "interim_binary", "bs25", "bin")
        partition = os.path.join(bin_dir, 'partition', pack_style_str, "partition.bin")
    evb_all_fwpkg = os.path.join(PKG_DIR, "fwpkg", "bs25", "evb_all.fwpkg")
    evb_app_fwpkg = os.path.join(PKG_DIR, "fwpkg", "bs25", "evb_app.fwpkg")
    evb_nv_fwpkg = os.path.join(PKG_DIR, "fwpkg", "bs25", "evb_nv_bin.fwpkg")
    evb_partition_fwpkg = os.path.join(PKG_DIR, "fwpkg", "bs25", "evb_partition_bin.fwpkg")

    ssb = os.path.join(bin_dir, "ssb.bin")
    app = application_path
    bt = os.path.join(bin_dir, "bt.bin")
    nv = os.path.join(SDK_DIR, "interim_binary", "bs25", "bin", "bs25_all_nv.bin")

    #create fota
    fota_bin = [ssb, app, bt, nv]
    fota = make_fota(PKG_DIR, pack_style_str, fota_bin)
    fota_size = hex(os.path.getsize(fota))

    if pack_style_str.endswith("flash"):
        if "in" in pack_style_str and "ex" in pack_style_str:  # SUPPORT_IN_EX_FLASH
            ssb_bx = ssb + "|0x98000000|0x18000|0"
            app_bx = app + "|0x9807c000|0x84000|18"
            bt_bx = bt + "|0x9802c000|0x50000|20"
            nv_bx = nv + "|0x8c5fc000|0x4000|100"
            app_dfu_bx = app + "|0x9807c000|0x84000|32"
            fota_addr = "0x8c401000"
        else:  # SUPPORT_TWO_FLASH
            ssb_bx = ssb + "|0x98000000|0x18000|0"
            app_bx = app + "|0x9807c000|0x84000|18"
            bt_bx = bt + "|0x9802c000|0x50000|20"
            nv_bx = nv + "|0x900fc000|0x4000|100"
            app_dfu_bx = app + "|0x9807c000|0x84000|32"
            fota_addr = "0x90001000"
    elif "keyboard" in pack_style_str:
        ssb_bx = ssb + "|0x8c400000|0x18000|0"
        app_bx = app + "|0x8c485000|0x7B000|18"
        bt_bx = bt + "|0x8c435000|0x50000|20"
        nv_bx = nv + "|0x8c5fc000|0x4000|100"
        partition_bx = partition + "|0x8c5fb000|0x1000|100"
    else:
        ssb_bx = ssb + "|0x8c400000|0x18000|0"
        app_bx = app + "|0x8c47c000|0x84000|18"
        bt_bx = bt + "|0x8c42c000|0x50000|20"
        nv_bx = nv + "|0x8c5fc000|0x4000|100"
        fota_addr = "0x8c501000"
    try:
        packet_post_agvs = list()
        packet_post_agvs.append(ssb_bx)
        if "keyboard" in pack_style_str:
            packet_post_agvs.append(partition_bx)

        if pack_fota_in_all:
            fota_bx = fota + "|" + fota_addr + "|" + fota_size + "|" + "100"
            packet_post_agvs.append(fota_bx)
        packet_post_agvs.append(bt_bx)
        packet_post_agvs.append(app_bx)
        packet_post_agvs.append(nv_bx)
        packet_bin(evb_all_fwpkg, packet_post_agvs)

        packet_post_agvs = list()
        packet_post_agvs.append(ssb_bx)
        packet_post_agvs.append(app_bx)
        packet_bin(evb_app_fwpkg, packet_post_agvs)

        packet_post_agvs = list()
        packet_post_agvs.append(ssb_bx)
        packet_post_agvs.append(nv_bx)
        packet_bin(evb_nv_fwpkg, packet_post_agvs)

        if pack_style_str.startswith("pack"):
            shutil.copy(evb_all_fwpkg, bin_dir)
            shutil.copy(evb_app_fwpkg, bin_dir)
            shutil.copy(fota, bin_dir)

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
    if pack_style_str.startswith("pack"):
        pack_path = os.path.join(SDK_DIR, "output", "package", "bs25", pack_style_str)
        packing_files = [
            os.path.join(pack_path, "ssb.bin"),
            os.path.join(pack_path, "application.bin"),
            os.path.join(pack_path, "bt.bin")
        ]
    else:
        packing_files.append(os.path.join(SDK_DIR, "interim_binary", "bs25", "bin", "ssb.bin"))
        application_path = os.path.join(SDK_DIR,
                                        "output",
                                        "bs25",
                                        "acore",
                                        pack_style_str,
                                        "application.bin")
        packing_files.append(application_path)
        packing_files.append(os.path.join(SDK_DIR, "interim_binary", "bs25", "bin", "bt.bin"))
    return packing_files