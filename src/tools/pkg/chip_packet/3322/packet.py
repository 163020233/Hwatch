#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
# ============================================================================
# @brief    packet files
# ============================================================================

import os
import sys
import shutil
import json
import platform
import subprocess

PY_PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.append(PY_PATH)
PKG_DIR = os.path.dirname(PY_PATH)
PKG_DIR = os.path.dirname(PKG_DIR)
from packet_create import create_sha_file
from packet_create import packet_bin

TOOLS_DIR = os.path.dirname(PKG_DIR)
SDK_DIR = os.path.dirname(TOOLS_DIR)
BUILD_ROOT_DIR = SDK_DIR
if os.environ.get('FBB_BUILD_ROOT_PATH'):
    BUILD_ROOT_DIR = os.environ.get('FBB_BUILD_ROOT_PATH')

sys.path.append(os.path.join(SDK_DIR, "build", "script"))
sys.path.append(os.path.join(SDK_DIR, 'build', 'config'))
sys.path.append(os.path.join(SDK_DIR, "build", "script", "nv"))
from utils.build_utils import exec_shell
from enviroment import TargetEnvironment
from param_packet import gen_flash_part_bin
from nv_binary import nv_begin
sys.path.append(os.path.join(SDK_DIR, 'build', 'config', 'target_config', '3322', 'mk_emmc_image'))
import mkfatimgtool
sys.path.append(os.path.join(SDK_DIR, 'build', 'config', 'target_config', '3322', 'mk_single_norflash_image'))
import mklfsimgtool

g_sign_py = os.path.join(SDK_DIR, 'build', 'config', 'target_config', '3322', 'sign_config', 'change_sign_cfg.py')

g_lzma_tools_7zr_win = os.path.join('tools', 'bin', 'lzma_tool', '7zr.exe')
g_lzma_tools_7zr_linux = os.path.join('tools', 'bin', 'lzma_tool', '7zr')

def create_binary_file(file_path, file_size):
    with open(file_path, 'wb') as file:
        file.seek(file_size - 1)
        file.write(b'\0')

def concatenate_files(file_a, file_b, offset):
    with open(file_a, 'rb+') as file_a_obj, open(file_b, 'rb') as file_b_obj:
        file_a_obj.seek(offset)
        file_b_content = file_b_obj.read()
        file_a_obj.write(file_b_content)
        file_a_obj.close()
        file_b_obj.close()

def _build_root_key():
    sign_py = os.path.join(SDK_DIR, 'build', 'config', 'target_config', '3322', 'sign_config', 'change_sign_cfg.py')
    target = os.path.join(BUILD_ROOT_DIR, 'output', '3322', 'root_public_key.bin')
    pri_key = os.path.join(SDK_DIR, 'build', 'config', 'target_config', '3322', 'sign_config', 'ec_bp256_oem_root_private_key.pem')
    pubkey_config = os.path.join(SDK_DIR, 'build', 'config', 'target_config', '3322', 'sign_config', 'root_pubk.cfg')
    
    # 创建配置临时目录
    temp_conf_dir = os.path.join(BUILD_ROOT_DIR, 'output', '3322', 'conf')
    os.makedirs(temp_conf_dir, exist_ok=True)
    
    if os.path.exists(pri_key) and os.path.exists(pubkey_config):
        # 读取原始配置文件并替换硬编码路径
        with open(pubkey_config, 'r') as f:
            config_content = f.read()
        
        # 替换硬编码路径为基于SDK_DIR的相对路径
        config_content = config_content.replace(
            '{SDK_DIR}',
            SDK_DIR
        )

        # 写入临时配置文件
        temp_config = os.path.join(temp_conf_dir, 'root_pubk_temp.cfg')
        with open(temp_config, 'w') as f:
            f.write(config_content)
        
        cmd = "%s %s -base %s -temp_dir %s -input %s -output %s -type %d" % (
                sys.executable,
                sign_py,
                temp_config,  # 使用处理后的临时配置文件
                os.path.join(temp_conf_dir, 'root_pubk.cfg'),
                pri_key,
                target,
                1)
        if (exec_shell(cmd, None) != 0):
            raise Exception("%s Failed!!!"%cmd)
    else: # 私钥不存在
        empty_pri =bytearray(128)
        os.makedirs(os.path.join(BUILD_ROOT_DIR, 'output', '3322'), exist_ok=True)
        with open(target, 'wb') as f:
            f.write(empty_pri)

def _build_partition(pack_style_str):
    if pack_style_str == '3322-wstp-app-16m' or pack_style_str.split('-')[0] == 'standard' or pack_style_str.split('-')[0] == 'testsuite':
        cfg = os.path.join(SDK_DIR, 'build', 'config', 'target_config', '3322', 'param_sector', 'param_sector.json')
    elif pack_style_str == '3322-single-norflash':
        cfg = os.path.join(SDK_DIR, 'build', 'config', 'target_config', '3322', 'param_sector', 'param_16m_s_norflash_sector.json')
    elif pack_style_str == '3322-single-norflash-xts':
        cfg = os.path.join(SDK_DIR, 'build', 'config', 'target_config', '3322', 'param_sector', 'param_16m_s_norflash_sector.json')
    elif pack_style_str.split('-')[-1] == '16m' or pack_style_str.split('-')[1] == 'diting' or pack_style_str.split('-')[1] == 'community':
        cfg = os.path.join(SDK_DIR, 'build', 'config', 'target_config', '3322', 'param_sector', 'param_16m_sector.json')
    else:
        cfg = os.path.join(SDK_DIR, 'build', 'config', 'target_config', '3322', 'param_sector', 'param_8m_sector.json')

    op = os.path.join(BUILD_ROOT_DIR, 'output', '3322', 'partition_table.bin')
    gen_flash_part_bin(cfg, op)
    target = os.path.join(BUILD_ROOT_DIR, 'output', '3322', 'partition_table_signed.bin')
    cmd = "%s %s -base %s -temp_dir %s -input %s -output %s -type %d" % (
            sys.executable,
            g_sign_py,
            os.path.join(SDK_DIR, 'build', 'config', 'target_config', '3322', 'sign_config', 'params.cfg'),
            os.path.join(BUILD_ROOT_DIR, 'output', '3322', 'conf', 'params.cfg'),
            os.path.join(BUILD_ROOT_DIR, 'output', '3322', 'partition_table.bin'),
            target,
            0
        )
    if (exec_shell(cmd, None) != 0):
        raise Exception("%s Failed!!!"%cmd)

    root_key = os.path.join(BUILD_ROOT_DIR, 'output', '3322', 'root_public_key.bin')
    new_bin = os.path.join(BUILD_ROOT_DIR, 'output', '3322', 'rootkey_partition_cfg.bin')
    if (os.path.exists(root_key) is True):
        shutil.copy(root_key, new_bin)
    else:
        create_binary_file(new_bin, 128)
    concatenate_files(new_bin, target, 128)

    cfct = os.path.join(SDK_DIR, 'build', 'config', 'target_config', '3322', 'sign_config', 'cfct3.bin')
    concatenate_files(new_bin, cfct, 1920); # 0x80 + 0x700 = 1920

    return new_bin

def should_build_nand_fs_image(flash_env):

    defines = flash_env.get("defines", [])
    
    required_macros = {
        'CFG_DRIVERS_NANDFLASH'
    }
    
    return all(mac in defines for mac in required_macros)

def should_build_emmc_fs_image(flash_env):

    defines = flash_env.get("defines", [])
    
    required_macros = {
        'CFG_DRIVERS_MMC',
        'MMC_CODE_INNER_LOS',
    }
    
    return all(mac in defines for mac in required_macros)

def find_addr_len_by_name(id_name, pack_style_str):
    if pack_style_str == '3322-wstp-app-16m' or pack_style_str.split('-')[0] == 'standard' or pack_style_str.split('-')[0] == 'testsuite':
       partition_file = 'param_sector.json'
    elif pack_style_str == '3322-single-norflash':
        partition_file = 'param_16m_s_norflash_sector.json'
    elif pack_style_str == '3322-single-norflash-xts':
        partition_file = 'param_16m_s_norflash_sector.json'
    elif pack_style_str.split('-')[-1] == '16m' or pack_style_str.split('-')[1] == 'diting' or pack_style_str.split('-')[1] == 'community':
        partition_file = 'param_16m_sector.json'
    else:
        partition_file = 'param_8m_sector.json'
    partition_dir = os.path.join(SDK_DIR, "build", "config", "target_config", "3322", "param_sector", partition_file)

    # 读取并解析 JSON 文件
    with open(partition_dir, 'r') as file:
        data = json.load(file)

    # 提取 Partition_Tbl 部分
    partition_tbl = data.get('Partition_Tbl', {})

    # 提取 param_info 部分
    param_info = partition_tbl.get('param_info', [])

    # 提取 Fixed item id 部分
    fixed_item_id_list = partition_tbl.get('Fixed item id', [])

    param_flash_base_addr = partition_tbl.get('flash_base_addr', [])
    if id_name == "partition" :
        return param_flash_base_addr
    # 初始化一个字典来存储固定项 ID 到名称的映射
    fixed_item_id_map = {}

    # 解析 Fixed item id 列表，提取 ID 和名称
    for item in fixed_item_id_list:
        # 分割字符串以提取 ID 和名称
        id_hex, name = item.split(":")
        # 去除名称两端的空白字符
        name = name.strip()
        # 将 ID 从十六进制字符串转换为整数
        id_value = int(id_hex, 16)
        # 存储到字典中
        fixed_item_id_map[id_value] = name

    # 查找 ID
    id = None
    for id_value, name in fixed_item_id_map.items():
        if name == id_name:
            id = id_value
            break

    # 如果找到ID，则在 param_info 中查找对应的起始地址和长度
    result = "0x0"
    if id is not None:
        for entry in param_info:
            if int(entry[0], 16) == id:
                start_address = entry[1]
                length = entry[2]
                result = f"|{hex(int(start_address, 16) + int(param_flash_base_addr, 16))}|{length}|1"
                break

    # 输出结果
    if result:
        print(id_name,result)
    else:
       print(id_name," ID not found or its corresponding info not found in param_info.")
    return result

def should_build_spinor_fs_image(flash_env):

    defines = flash_env.get("defines", [])

    required_macros = {
        'CFG_DRIVERS_SPINOR',
    }

    return all(mac in defines for mac in required_macros)

def _generate_database_img(database_path):
    src_path = database_path
    database_name = os.path.basename(database_path)
    dst_file = os.path.join(os.path.dirname(database_path), '%s.7z'%database_name)

    if os.path.exists(dst_file):
        os.remove(dst_file)

    cmd_list0 = []
    if platform.system().lower() == "linux":
        compress_tool = g_lzma_tools_7zr_linux
        cmd_list0.append('chmod')
        cmd_list0.append('755')
        cmd_list0.append(compress_tool)
    elif platform.system().lower() == "windows":
        # 防止在Windows环境下，默认中文系统语言时，cacls返回中文导致乱码，设置utf-8编码
        compress_tool = g_lzma_tools_7zr_win
        cmd_list0.append('chcp 65001 > nul & ') # 65001: utf-8
        cmd_list0.append('cacls')
        cmd_list0.append(compress_tool)
        cmd_list0.append('/p everyone:f /e')
    str_cmd = ' '.join(cmd_list0)
    ret = subprocess.run(str_cmd, shell=True, encoding='utf-8', capture_output=True)
    if ret.returncode != 0:
        sys.exit("chmod compress tool failed: %s" % ret)
    print(ret.stdout)

    cmd_list = [
        compress_tool,
        'a',
        '-mtm=off',
        dst_file,
        os.path.join(".", src_path) # 压缩包中需包含database这一层级目录
    ]
    str_cmd =' '.join(cmd_list)
    ret = subprocess.run(str_cmd, shell=True, encoding='utf-8', capture_output=True)
    if ret.returncode != 0:
        sys.exit("run compress tool failed: %s" % ret)
    print(ret.stdout)
    return dst_file


def make_all_in_one_packet(pack_style_str):
    # make all in one packet
    _build_root_key()
    bin_dir = os.path.join(PKG_DIR, "bin", "3322")
    # loaderboot
    loaderboot_signed = os.path.join(BUILD_ROOT_DIR, "output", "3322", "acore", "3322-loaderboot", "loaderboot_signed.bin")
    loaderboot_signed_bx = loaderboot_signed + "|0|0x10000|0"
    # partition
    partition = _build_partition(pack_style_str)
    partition_bx =  f"{partition}|{find_addr_len_by_name('partition', pack_style_str)}|0x1000|1"
    # ssb
    ssb = os.path.join(BUILD_ROOT_DIR, "output", "3322", "acore", "3322-ssb", "ssb_signed.bin")
    if not os.path.exists(ssb):
        ssb = os.path.join(bin_dir, "ssb_signed.bin")
    print("ssb_path: ", ssb)
    ssb_bx = ssb + find_addr_len_by_name("ssb", pack_style_str)
    #ssb_back
    ssb_back = 'backup' + '3322-ssb'
    target = os.path.join(BUILD_ROOT_DIR, 'output', '3322', 'acore', ssb_back, 'ssb_backup_signed.bin')
    if (os.path.exists(os.path.dirname(target)) is False):
        os.makedirs(os.path.dirname(target))
    if (os.path.exists(ssb) is True):
        shutil.copy(ssb, target)
    ssb_back_bx = target + find_addr_len_by_name("ssb_backup", pack_style_str)
    #recovery
    if pack_style_str.split('-')[1] == 'emmc':
        recovery_bin = os.path.join(BUILD_ROOT_DIR,
                                    "output",
                                    "3322",
                                    "acore",
                                    "3322-emmc-recovery",
                                    "recovery_signed.bin")
        recovery_bx = recovery_bin + find_addr_len_by_name("recovery_image", pack_style_str)
    elif pack_style_str == '3322-single-norflash':
        recovery_bin = os.path.join(BUILD_ROOT_DIR,
                                    "output",
                                    "3322",
                                    "acore",
                                    "3322-single-norflash-recovery",
                                    "recovery_signed.bin")
        recovery_bx = recovery_bin + find_addr_len_by_name("recovery_image", pack_style_str)
    elif pack_style_str == '3322-single-norflash-xts':
        recovery_bin = os.path.join(BUILD_ROOT_DIR,
                                    "output",
                                    "3322",
                                    "acore",
                                    "3322-single-norflash-recovery",
                                    "recovery_signed.bin")
        recovery_bx = recovery_bin + find_addr_len_by_name("recovery_image", pack_style_str)
    else:
        recovery_bin = os.path.join(BUILD_ROOT_DIR,
                                    "output",
                                    "3322",
                                    "acore",
                                    "3322-recovery",
                                    "recovery_signed.bin")
        recovery_bx = recovery_bin + find_addr_len_by_name("recovery_image", pack_style_str)
    #seliteos
    seliteos = os.path.join(BUILD_ROOT_DIR, "output", "3322", "acore", "3322-seliteos-release", "seliteos_signed.bin")
    seliteos_bx = seliteos + find_addr_len_by_name("seliteos_image", pack_style_str)
    #application
    if pack_style_str == '3322-wstp-app-16m' or pack_style_str.split('-')[0] == 'standard' or pack_style_str.split('-')[0] == 'testsuite':
        application_dir = os.path.join(BUILD_ROOT_DIR,
                                    "output",
                                    "3322",
                                    "acore",
                                    pack_style_str)
        app = os.path.join(application_dir, "application_signed.bin")
        app_bx = app + find_addr_len_by_name("acpu_image", pack_style_str)
    elif pack_style_str.split('-')[-1] == '16m' or pack_style_str.split('-')[1] == 'diting' or pack_style_str.split('-')[1] == 'community':
        application_dir = os.path.join(BUILD_ROOT_DIR,
                                    "output",
                                    "3322",
                                    "acore",
                                    pack_style_str)
        app = os.path.join(application_dir, "application_signed.bin")
        app_bx = app + find_addr_len_by_name("acpu_image", pack_style_str)

    else:
        application_dir = os.path.join(BUILD_ROOT_DIR,
                                    "output",
                                    "3322",
                                    "acore",
                                    pack_style_str)
        app = os.path.join(application_dir, "application_signed.bin")
        app_bx = app + find_addr_len_by_name("acpu_image", pack_style_str)
    #nv
    if pack_style_str == '3322-wstp-app-16m' or pack_style_str.split('-')[0] == 'standard' or pack_style_str.split('-')[0] == 'testsuite':
        nv = os.path.join(BUILD_ROOT_DIR, 'output', '3322', 'acore', pack_style_str, '3322_all_nv.bin')
        nv_bx = nv + find_addr_len_by_name("nv_data", pack_style_str)
    elif pack_style_str.split('-')[-1] == '16m' or pack_style_str.split('-')[1] == 'diting' or pack_style_str.split('-')[1] == 'community':
        nv = os.path.join(BUILD_ROOT_DIR, 'output', '3322', 'acore', pack_style_str, '3322_all_nv.bin')
        nv_bx = nv + find_addr_len_by_name("nv_data", pack_style_str)
    else:
        nv = os.path.join(BUILD_ROOT_DIR, 'output', '3322', 'acore', pack_style_str, '3322_all_nv.bin')
        nv_bx = nv + find_addr_len_by_name("nv_data", pack_style_str)
    #fs
    flash_env = TargetEnvironment(pack_style_str)
    if flash_env.get("fs_image"):
        if should_build_nand_fs_image(flash_env):
            fs = os.path.join(BUILD_ROOT_DIR, "output", "3322", "file.bin")
            fs_bx = fs + "|0x36020000|0xFFFFFFFF|101"
        elif should_build_emmc_fs_image(flash_env):
            mkfatimgtool.main()
            fs = os.path.join(BUILD_ROOT_DIR, "output", "3322", "file.bin")
            fs_bx = fs + "|0x76000800|0xFFFFFFFF|101"
        if should_build_spinor_fs_image(flash_env):
            mklfsimgtool.main()
            fs = os.path.join(BUILD_ROOT_DIR, "output", "3322", "lfs_file.bin")
            fs_bx = None
            if os.path.exists(fs):
                fs_bx = fs + find_addr_len_by_name("filesystem", pack_style_str)
    #bt
    if pack_style_str.split('-')[0] == 'testsuite':
        bt = os.path.join(bin_dir, "testsuite_bt_signed.bin")
    else:
        bt = os.path.join(bin_dir, "bt_signed.bin")
    bt_bx = bt + find_addr_len_by_name("bt_image", pack_style_str)
    #dsp
    target_env = TargetEnvironment(pack_style_str)
    dsp_version = target_env.get("dsp_version")
    dsp_main = os.path.join(bin_dir, "dsp", dsp_version, "dsp_main.bin")
    dsp_main_bx = dsp_main + find_addr_len_by_name("dsp_main_image", pack_style_str)
    dsp_overlay = os.path.join(bin_dir, "dsp", dsp_version, "dsp_overlay.bin")
    dsp_overlay_bx = dsp_overlay + find_addr_len_by_name("dsp_overlay_image", pack_style_str)
    #database
    database_img_bx = ''
    database_path = os.path.join(BUILD_ROOT_DIR, 'output', '3322', 'database_evb')
    database_tmp_path = os.path.join(BUILD_ROOT_DIR,'output', '3322', 'database')
    if os.path.exists(database_path):
        if os.path.exists(database_tmp_path):
            shutil.rmtree(database_tmp_path)
        # 先将目录改为database, 因压缩包中必须包含database目录
        os.rename(database_path, database_tmp_path)
        database_img = _generate_database_img(database_tmp_path)
        database_img_bx = database_img + "|0xF000000|0|16"
        # 再将目录改为database_earphone
        os.rename(database_tmp_path, database_path)
    
    #fwpkg路径声明
    upload_app_fwpkg = os.path.join("output", "3322", "fwpkg", pack_style_str+".fwpkg")

    try:
        packet_post_agvs = list()
        packet_post_agvs.append(loaderboot_signed_bx)
        packet_post_agvs.append(partition_bx)
        packet_post_agvs.append(ssb_bx)
        packet_post_agvs.append(ssb_back_bx)
        packet_post_agvs.append(recovery_bx)
        packet_post_agvs.append(seliteos_bx)
        packet_post_agvs.append(app_bx)
        packet_post_agvs.append(nv_bx)
        if database_img_bx != '':
            packet_post_agvs.append(database_img_bx)
        if flash_env.get("fs_image") and fs_bx is not None:
            packet_post_agvs.append(fs_bx)
        if (pack_style_str.split('-')[0] == 'standard' or pack_style_str.split('-')[0] == 'testsuite' or
            pack_style_str == '3322-wstp-app-16m' or pack_style_str == '3322-single-norflash' or pack_style_str == '3322-single-norflash-xts'):
            packet_post_agvs.append(bt_bx)
            packet_post_agvs.append(dsp_main_bx)
            packet_post_agvs.append(dsp_overlay_bx)
        else:
            pass
        packet_bin(upload_app_fwpkg, packet_post_agvs)
    except Exception as e:
        print(e)
        exit(-1)

def is_packing_files_exist(soc, pack_style_str):
    """
    判断打包文件是否存在
    3322中存在只获取打包文件的情况,不做完整性校验
    :return:
    """
    packing_files = get_packing_files(soc, pack_style_str)
    lost_files = list()
    return lost_files

def get_packing_files(soc, pack_style_str):
    """
    直接添加需要打包的文件路径
    :return:
    """
    packing_files = list()
    packing_files.append(os.path.join(PKG_DIR, "bin", "3322", "ssb_signed.bin"))
    application_path = os.path.join(BUILD_ROOT_DIR,
                                    "output",
                                    "3322",
                                    "acore",
                                    pack_style_str,
                                    "application.bin")
    packing_files.append(application_path)
    packing_files.append(os.path.join(PKG_DIR, "bin", "3322", "bt_signed.bin"))

    target_env = TargetEnvironment(pack_style_str)
    dsp_version = target_env.get("dsp_version")
    packing_files.append(os.path.join(PKG_DIR, "bin", "3322", "dsp", dsp_version, "dsp_main.bin"))
    packing_files.append(os.path.join(PKG_DIR, "bin", "3322", "dsp", dsp_version, "dsp_overlay.bin"))
    return packing_files
