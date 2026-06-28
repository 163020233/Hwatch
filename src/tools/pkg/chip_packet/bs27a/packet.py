#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
# ============================================================================
# @brief    packet files
# ============================================================================

import os
import sys
import json
import shutil
import subprocess

PY_PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.append(PY_PATH)
PKG_DIR = os.path.dirname(PY_PATH)
PKG_DIR = os.path.dirname(PKG_DIR)

from packet_create import create_sha_file
from packet_create import packet_bin

TOOLS_DIR = os.path.dirname(PKG_DIR)
SDK_DIR = os.path.dirname(TOOLS_DIR)
sys.path.append(os.path.join(SDK_DIR, "build", "script"))
sys.path.append(os.path.join(SDK_DIR, "build", "script", "utils"))
sys.path.append(os.path.join(SDK_DIR, "build", "config", "target_config", "bs27a"))
from config import target, target_group
from create_hex import create_hex_file

sector_cfg_type = {
    'flashboot':     '0x00',
    'flashboot_bak': '0x01',
    'upgrade':       '0x02',
    'tee':           '0x03',
    'bcore':         '0x04',
    'app':           '0x06',
    'fota':          '0x20',
    'nv':            '0x21'
}

flash_start_addr = 0x00400000

def search_sector_info(region, sector_config_list, sector_cfg):
    index = sector_cfg_type[region]
    for info in sector_config_list:
        if info[0] == index:
            region_addr = "0x%x" % (int(info[1], 16) + flash_start_addr)
            region_len = info[2]
            return f"|{region_addr}|{region_len}|", int(region_len, 16)
    print(f'error: {region} not found in {sector_cfg}, index is {index}!')
    sys.exit(-1)

def pkg_error_log(path, image_size, sector_size):
    print("packet error: image size overflow.")
    print(f"image path: {path}.")
    print(f"image size: {hex(image_size)} > max pkg size: {hex(sector_size)}.")
    sys.exit(-1)

def full_bin(bin_file_path, target_size):
    with open(bin_file_path, 'rb') as f:
        bin_data = f.read()
    current_size = len(bin_data)
    if current_size > target_size:
        print(f"Error: File {bin_file_path} size {current_size} exceeds limit {target_size}")
        return
    pad_size = target_size - current_size
    if pad_size > 0:
        bin_data += bytes([0x00] * pad_size)
    with open(bin_file_path, 'wb') as f:
        f.write(bin_data)

def packet_ate_bin(output_path, input_files):
    with open(output_path, 'wb') as output_file:
        for file_path in input_files:
            with open(file_path, 'rb') as input_file:
                output_file.write(input_file.read())

def pack_flash_ate_boot_bin(pkg_soc, pack_style_str):
    # ate 启动代码和ate.bin拼接后签名
    ate_startup = os.path.join(SDK_DIR, "tools", "pkg", "bin", pkg_soc, "ate", "ate_startup.bin")
    ate_dir = os.path.join(SDK_DIR, "output", pkg_soc, "acore", pack_style_str)
    ate_bin = os.path.join(ate_dir, "ate.bin")
    with open(ate_bin, 'rb') as f:
        ate_data = f.read()
    with open(ate_startup, 'rb') as f:
        startup_data = f.read()
    ate_all_data = startup_data + ate_data
    ate = os.path.join(ate_dir, "ate_flashboot.bin")
    with open(ate, 'wb') as f:
        f.write(ate_all_data)
    sign_tool_path = os.path.join(SDK_DIR, "tools", "bin", "sign_tool", "sign_tool_pltuni")
    sign_config_path = os.path.join(SDK_DIR, "build", "config", "target_config", pkg_soc, "sign_config", "ate_flashboot_bs27a.cfg")
    try:
        subprocess.run([sign_tool_path, "0", sign_config_path], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        print("sign ate succ")
    except subprocess.CalledProcessError as e:
        print(f"sign ate fail: {e.stderr.decode()}")
        raise
    # rootkey和ate_sign_bin拼接
    ate_sign_bin = os.path.join(ate_dir, "ate_flashboot_sign.bin")
    ate_rootkey = os.path.join(SDK_DIR, "tools", "pkg", "bin", pkg_soc, "ate", "ate_rootkey.bin")
    with open(ate_rootkey, 'rb') as f:
        rootkey_data = f.read()
    with open(ate_sign_bin, 'rb') as f:
        sign_data = f.read(0x780)
    combined_data = rootkey_data + sign_data
    with open(ate_sign_bin, 'wb') as f:
        f.write(combined_data)
    # 分别制作ate实际使用版本app_ate.hex和老化上电版本ate_burntool.bin
    ate_partition = os.path.join(SDK_DIR, "interim_binary", pkg_soc, "bin", "partition", pack_style_str, "partition.bin")
    full_bin(ate_partition, 0x1000)
    full_bin(ate_bin, 0xD800)
    packet_post_agvs = list([ate_bin, ate_sign_bin, ate_partition, ate_partition])
    flash_ate_boot_bin = os.path.join(ate_dir, "app_ate.bin")
    flash_ate_boot_hex = os.path.join(ate_dir, "app_ate.hex")
    packet_ate_bin(flash_ate_boot_bin, packet_post_agvs)
    create_hex_file(flash_ate_boot_bin, flash_ate_boot_hex)
    packet_post_agvs = list([ate_partition, ate_partition, ate_sign_bin, ate_bin])
    ate_burntool_bin = os.path.join(ate_dir, "ate_burntool.bin")
    packet_ate_bin(ate_burntool_bin, packet_post_agvs)

# bs27a
def make_all_in_one_packet(pkg_soc, pack_style_str, sector_cfg):
    if "ate-application" in pack_style_str:
        pack_flash_ate_boot_bin(pkg_soc, pack_style_str)
        ate_dir = os.path.join(SDK_DIR, "output", pkg_soc, "acore", pack_style_str)
        bs27a_ate_fwpkg = os.path.join(ate_dir, "%s_app_ate.fwpkg" %pkg_soc)
        ate = os.path.join(ate_dir, "ate_burntool.bin")
        ate_bx = ate + f"|{flash_start_addr:08X}|0x10000|1"
        loadboot_ate = os.path.join(SDK_DIR, "interim_binary", pkg_soc, "bin", "boot_bin", "loaderboot-bs27a", "loaderboot_sign.bin")
        loadboot_ate_bx = loadboot_ate + "|0x0|0x0|0"
        # pack ate fwpkg.
        packet_post_agvs = list([loadboot_ate_bx, ate_bx])
        packet_bin(bs27a_ate_fwpkg, packet_post_agvs)
        return

    if sector_cfg:
        sector_cfg_path = os.path.join(SDK_DIR, "build", "config", "target_config", pkg_soc, "flash_sector_config")
        sector_cfg_json = os.path.join(sector_cfg_path, sector_cfg + ".json")
        if not os.path.exists(sector_cfg_json):
            print("packet error: sector config %s not found in %s." % (sector_cfg, sector_cfg_path))
            exit(-1)
        with open(sector_cfg_json, 'r') as f:
            sector = json.load(f)
        sector_config_list = sector['Partition_Tbl']['param_info']
        info = {}
        for key, value in sector_cfg_type.items():
            info[key] = search_sector_info(key, sector_config_list, sector_cfg)

        # get pkg dir.
        interim_binary_dir = os.path.join(SDK_DIR, "interim_binary", pkg_soc, "bin")
        if pack_style_str.startswith("pack"):
            bin_dir = os.path.join(SDK_DIR, "output", "package", pkg_soc, pack_style_str)
            partition_dir = bin_dir
            boot_dir = os.path.join(interim_binary_dir, "boot_bin")
            application_dir = bin_dir
            protocol_dir = os.path.join(interim_binary_dir, "protocol")
            nv_dir =  bin_dir
            glp_dir =  bin_dir
            output_dir = bin_dir
            tee_dir = os.path.join(interim_binary_dir, "boot_bin")
            upgrade_dir = bin_dir
        else:
            partition_dir = os.path.join(interim_binary_dir, "partition", pack_style_str)
            boot_dir = os.path.join(interim_binary_dir, "boot_bin")
            application_dir = os.path.join(SDK_DIR, "output", pkg_soc, "acore", pack_style_str)
            protocol_dir = os.path.join(interim_binary_dir, "protocol")
            nv_dir =  os.path.join(interim_binary_dir, "nv", pack_style_str)
            glp_dir =   os.path.join(interim_binary_dir, "slp", pack_style_str, "file_array_ota")
            output_dir = os.path.join(SDK_DIR, "output", pkg_soc, "fwpkg", pack_style_str)
            tee_dir = os.path.join(interim_binary_dir, "boot_bin")
            upgrade_dir = os.path.join(interim_binary_dir, "boot_bin", "bs27a-upgrade")

        fota_dir = application_dir
        bs27a_fwpkg_custom = None
        # 检查 target_config 是否以 'pack' 开头
        if pack_style_str.startswith('pack'):
            target_config = target_group.get(pack_style_str, {})
            # 如果是 pack 开头的配置，遍历子 target
            for sub_target in target_config:
                # 获取每个子 target 的 output_pkg_name
                target_config = target.get(sub_target, {})
                output_pkg_name = target_config.get('output_pkg_name', None)
                if output_pkg_name:
                    bs27a_fwpkg_custom = os.path.join(output_dir, f"{output_pkg_name}.fwpkg")
                    break
                else:
                    continue
        else:
            target_config = target.get(pack_style_str, {})
            # 如果不是 pack 开头的配置，保持原有逻辑
            output_pkg_name = target_config.get('output_pkg_name', None)
            if output_pkg_name:
                bs27a_fwpkg_custom = os.path.join(output_dir, f"{output_pkg_name}.fwpkg")

        bs27a_fwpkg = os.path.join(output_dir, "%s_all_in_one.fwpkg" %pkg_soc)
        bs27a_flashboot_only_fwpkg = os.path.join(output_dir, "%s_flashboot_only.fwpkg" %pkg_soc)
        bs27a_loadapp_only_fwpkg = os.path.join(output_dir, "%s_loadapp_only.fwpkg" %pkg_soc)
        bs27a_nv_fwpkg = os.path.join(output_dir, "%s_nv_only.fwpkg" %pkg_soc)
        bs27a_ide_fwpkg = os.path.join(SDK_DIR, "tools", "pkg", "fwpkg", pkg_soc, "%s_all.fwpkg" %pkg_soc)
        bs27a_efuse_fwpkg = os.path.join(output_dir, "%s_efuse.fwpkg" %pkg_soc)
        fota = None
        if os.path.isfile(os.path.join(fota_dir, "fota.fwpkg")):
            fota = os.path.join(fota_dir, "fota.fwpkg")
            fota_fwpkg = os.path.join(fota_dir, "fota_loadboot.fwpkg")

        # bin pkg dir
        partition = os.path.join(partition_dir, "partition.bin")
        partition_bk = os.path.join(partition_dir, "partition_bk.bin")
        # 拷贝文件
        if os.path.exists(partition_bk):
            os.remove(partition_bk)
        shutil.copy2(partition, partition_bk)

        loadboot = os.path.join(boot_dir, "loaderboot_sign.bin")
        flashboot = os.path.join(boot_dir, "flashboot_sign_a.bin")
        flashboot_bak = os.path.join(boot_dir, "flashboot_sign_b.bin")
        tee = os.path.join(tee_dir, "tee_sign.bin")
        upgrade = os.path.join(upgrade_dir, "upgrade_sign.bin")
        app = os.path.join(application_dir, "application_sign.bin")
        if os.path.exists(os.path.join(protocol_dir, "protocol_sign.bin")):
            protocol = os.path.join(protocol_dir, "protocol_sign.bin")
        else:
            protocol = os.path.join(interim_binary_dir, "protocol", "protocol_sign.bin")
        if pack_style_str.startswith("pack"):
            if os.path.exists(loadboot):
                cp_target = os.path.join(output_dir, "loadboot_sign.bin")
                shutil.copy2(loadboot, cp_target)
            if os.path.exists(tee):
                cp_target = os.path.join(output_dir, "tee_sign.bin")
                shutil.copy2(tee, cp_target)
            if os.path.exists(protocol):
                cp_target = os.path.join(output_dir, "protocol_sign.bin")
                shutil.copy2(protocol, cp_target)
        nv = os.path.join(nv_dir, "%s_all_nv.bin" %pkg_soc)
        slp = os.path.join(glp_dir, "slp.bin")
        efuse = os.path.join(interim_binary_dir, "boot_bin" , "efuse_cfg.bin")
        # pkg bin addr
        loadboot_bx = loadboot + "|0x0|0x0|0"
        partition_bx = partition + f"|{flash_start_addr:08X}|0x1000|1"
        partition_bk_addr = flash_start_addr + 0x1000
        partition_bk_bx = partition_bk + f"|{partition_bk_addr:08X}|0x1000|1"
        if os.path.isfile(efuse):
            efuse_bx = efuse + "|0x0|0x0|3"

        # get flashboot info.
        if os.path.getsize(flashboot) <= info["flashboot"][1]:
            flashboot_bx = flashboot + info["flashboot"][0] + "1"
        else:
            pkg_error_log(flashboot, os.path.getsize(flashboot), info["flashboot"][1])
        # get flashboot bak info.
        if info["flashboot_bak"][1] == 0: # don't pkg flashbootb.
            flashboot_bak_bx = None
        elif os.path.getsize(flashboot_bak) <= info["flashboot_bak"][1]:
            flashboot_bak_bx = flashboot_bak + info["flashboot_bak"][0] + "1"
        else:
            pkg_error_log(flashboot_bak, os.path.getsize(flashboot_bak), info["flashboot_bak"][1])
        # get tee info.
        if os.path.getsize(tee) <= info["tee"][1]:
            tee_bx = tee + info["tee"][0] + "1"
        else:
            pkg_error_log(tee, os.path.getsize(tee), info["tee"][1])
        # get upgrade info.
        if info["upgrade"][1] != 0 and os.path.exists(upgrade):
            if os.path.getsize(upgrade) <= info["upgrade"][1]:
                upgrade_bx = upgrade + info["upgrade"][0] + "1"
            else:
                pkg_error_log(upgrade, os.path.getsize(upgrade), info["upgrade"][1])
        # get app info.
        if os.path.getsize(app) <= info["app"][1]:
            app_bx = app + info["app"][0] + "1"
        else:
            pkg_error_log(app, os.path.getsize(app), info["app"][1])
        # get protocol info.
        if os.path.getsize(protocol) <= info["bcore"][1]:
            protocol_bx = protocol + info["bcore"][0] + "1"
        else:
            pkg_error_log(app, os.path.getsize(app), info["app"][1])
        # get nv info.
        if os.path.getsize(nv) <= info["nv"][1]:
            nv_bx = nv + info["nv"][0] + "1"
        else:
            pkg_error_log(nv, os.path.getsize(nv), info["nv"][1])
        # get slp info.
        if os.path.isfile(slp):
            if info["slp"][1] == 0:
                print("no need to pack slp.")
            elif os.path.getsize(slp) <= info["slp"][1]:
                slp_bx = slp + info["slp"][0] + "1"
            else:
                pkg_error_log(slp, os.path.getsize(slp), info["slp"][1])
        # get fota info.
        if fota is not None and os.path.isfile(fota):
            if os.path.getsize(fota) <= info["fota"][1]:
                fota_bx = fota + info["fota"][0] + "1"
            else:
                pkg_error_log(fota, os.path.getsize(fota), info["fota"][1])
    else:
        print("packet error: no sector config.")
        exit(-1)

    try:
        # pack all in one fwpkg.
        packet_post_agvs = list()
        packet_post_agvs.append(loadboot_bx)
        packet_post_agvs.append(partition_bx)
        packet_post_agvs.append(partition_bk_bx)
        packet_post_agvs.append(flashboot_bx)
        if flashboot_bak_bx is not None:
            packet_post_agvs.append(flashboot_bak_bx)
        packet_post_agvs.append(tee_bx)
        if info["upgrade"][1] != 0 and os.path.exists(upgrade):
            packet_post_agvs.append(upgrade_bx)
        packet_post_agvs.append(app_bx)
        if protocol_bx is not None:
            packet_post_agvs.append(protocol_bx)
        packet_post_agvs.append(nv_bx)
        if os.path.isfile(slp) and info["slp"][1] > 0:
            packet_post_agvs.append(slp_bx)
        packet_bin(bs27a_fwpkg, packet_post_agvs)
        if bs27a_fwpkg_custom:
            packet_bin(bs27a_fwpkg_custom, packet_post_agvs)
        packet_bin(bs27a_ide_fwpkg, packet_post_agvs)

        # pack flashboot only fwpkg.
        packet_post_agvs = list()
        packet_post_agvs.append(loadboot_bx)
        packet_post_agvs.append(flashboot_bx)
        packet_post_agvs.append(tee_bx)
        if info["upgrade"][1] != 0 and os.path.exists(upgrade):
            packet_post_agvs.append(upgrade_bx)
        if flashboot_bak_bx is not None:
            packet_post_agvs.append(flashboot_bak_bx)
        packet_bin(bs27a_flashboot_only_fwpkg, packet_post_agvs)

        # pack application only fwpkg.
        packet_post_agvs = list()
        packet_post_agvs.append(loadboot_bx)
        packet_post_agvs.append(app_bx)
        packet_bin(bs27a_loadapp_only_fwpkg, packet_post_agvs)

        # pack nv only fwpkg.
        packet_post_agvs = list()
        packet_post_agvs.append(loadboot_bx)
        packet_post_agvs.append(nv_bx)
        packet_bin(bs27a_nv_fwpkg, packet_post_agvs)

        # pack efuse only fwpkg.
        if os.path.isfile(efuse):
            packet_post_agvs = list()
            packet_post_agvs.append(loadboot_bx)
            packet_post_agvs.append(efuse_bx)
            packet_bin(bs27a_efuse_fwpkg, packet_post_agvs)

        if fota is not None and os.path.isfile(fota):
            packet_post_agvs = list()
            packet_post_agvs.append(loadboot_bx)
            packet_post_agvs.append(fota_bx)
            packet_bin(fota_fwpkg, packet_post_agvs)

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
        bin_dir = os.path.join(SDK_DIR, "output", "package", soc, pack_style_str)
        if pack_style_str.endswith("ate"):
            ate_path = os.path.join(bin_dir, "ate.bin")
            packing_files.append(ate_path)
        else:
            application_path = os.path.join(bin_dir, "application.bin")
            packing_files.append(application_path)
    else:
        bin_dir = os.path.join(SDK_DIR, "output", soc, "acore", pack_style_str)
        if pack_style_str.startswith("ate"):
            ate_path = os.path.join(bin_dir, "ate.bin")
            packing_files.append(ate_path)
        else:
            application_path = os.path.join(bin_dir, "application.bin")
            packing_files.append(application_path)
    return packing_files
