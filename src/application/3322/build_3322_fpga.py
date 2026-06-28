#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    build system entry, receive param & start to build
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
# ============================================================================
import os
import sys
import shutil

sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', '..', 'build', 'script'))
from utils.build_utils import exec_shell
from param_packet import gen_flash_part_bin

g_build_env = {
    "NAME" : {
        "testsuite" : "3322_testsuite",
        "standard"  : "3322_standard",
        "bt_standard"  : "3322_bt_standard",
        "bt_testsuite" : "3322_bt_testsuite",
        "wstp" : "3322_wstp",
    },
    "BINARY" : {
    },
    "NV" : {
    },
    "PARTITION" : {
        "partition" : os.path.join('build', 'config', 'target_config', '3322', 'param_sector', 'param_sector.json'),
    },
    "UPDATE" : {
    },

    "TOOL" : {
        "sign_cfg"  : os.path.join('build', 'config', 'target_config', '3322', 'sign_config'),
    },
    # FPGA上norflash只有16MB
    "FWPKG" : {
        "loader"      : "|0|0x10000|0",
        "partition"   : "|0x26000000|0x1000|1",
        "ssb"         : "|0x26001000|0x10000|1",
        "ssb_back"    : "|0x26011000|0x10000|1",
        "recovery"    : "|0x26021000|0x60000|1",
        "bt_core"     : "|0x26081000|0x200000|1",
        "dsp_main"    : "|0x26281000|0x100000|1",
        "dsp_overlay" : "|0x26381000|0x280000|1",
        "application" : "|0x26601000|0x5AF000|1",
        "seliteos"    : "|0x26C00000|0x40000|1",
        "nv"          : "|0x26C40000|0x6000|1",
        # "bt_rom"    : "|0x26C40000|0x60000|1",
    },

    # 8M norflash:no dsp
    "FWPKG_MINI" : {
        "loader"      : "|0|0x10000|0",
        "partition"   : "|0x26000000|0x1000|1",
        "ssb"         : "|0x26001000|0x10000|1",
        "ssb_back"    : "|0x26011000|0x10000|1",
        "recovery"    : "|0x26021000|0x60000|1",
        "bt_core"     : "|0x26081000|0x200000|1",
        "application" : "|0x26281000|0x4DF000|1",
        "seliteos"    : "|0x26760000|0x40000|1",
        "bt_rom"      : "|0x267A0000|0x60000|1",
    },
    "DSP_BIN" : {
        "dsp_main"    : os.path.join('interim_binary', '3322', 'audio', '3322_fpga', 'dsp_main.bin'),
        "dsp_overlay" : os.path.join('interim_binary', '3322', 'audio', '3322_fpga', 'dsp_overlay.bin'),
    },
}
g_sign_py = os.path.join(g_build_env.get('TOOL').get('sign_cfg'), 'change_sign_cfg.py')

def check_binary_exist():
    binaries = g_build_env.get("BINARY")
    for n,p in binaries.items():
        if (os.path.isfile(p) is False):
            raise Exception("Can NOT find %s from path %s"%(n, p))

def build_env_init():
    print("Running python at", sys.executable, " version:%d.%d.%d "%(sys.version_info[0], sys.version_info[1], sys.version_info[2]))
    if (sys.version_info[0] < 3):
        raise Exception("Required python3 version!")
    curr_dir = os.path.dirname(os.path.realpath(__file__))
    root_dir = os.path.join(curr_dir, '..', '..')
    os.chdir(root_dir)
    # check binary exist
    return curr_dir

def _build_target_with_define(target_name, macro):
    if '-c' in sys.argv:
        do_clean = '-c'
    else:
        do_clean = ''
    if '-nhso' in sys.argv:
        with_hso = '-nhso'
    else:
        with_hso = ''
    cmd = "%s %s %s %s -def=%s %s"%(sys.executable, 'build.py', do_clean, with_hso, macro, target_name)
    if (exec_shell(cmd, None) != 0):
        raise Exception("%s Failed!!!"%cmd)

def _build_any_target(target_name):
    if '-c' in sys.argv:
        do_clean = '-c'
    else:
        do_clean = ''
    if '-nhso' in sys.argv:
        with_hso = '-nhso'
    else:
        with_hso = ''

    cmd = "%s %s %s %s %s"%(sys.executable, 'build.py', do_clean, with_hso, target_name)
    if (exec_shell(cmd, None) != 0):
        raise Exception("%s Failed!!!"%cmd)

def _build_loader():
    build_target = '3322-loaderboot'
    if '-mini' in sys.argv:
        _build_target_with_define(build_target, 'CFG_FLASH_SPECS_MINI')
    else:
        _build_any_target(build_target)
    target = os.path.join('output', '3322', 'acore', build_target, 'loaderboot_signed.bin')
    return target

def _build_ssb(build_target):
    if '-mini' in sys.argv:
        _build_target_with_define(build_target, 'CFG_FLASH_SPECS_MINI')
    else:
        _build_any_target(build_target)
    target = os.path.join('output', '3322', 'acore', build_target, 'ssb_signed.bin')
    return target

def _copy_folder_with_overwrite(src, dst):
    if os.path.exists(dst):
        shutil.rmtree(dst)
    shutil.copytree(src, dst)

def _copy_dsp_bin():
    #拷贝dspbin到output
    dsp_main = g_build_env.get('DSP_BIN').get('dsp_main')
    dsp_overlay = g_build_env.get('DSP_BIN').get('dsp_overlay')
    destination_dir = os.path.join('output', '3322', 'dsp_core')
    os.makedirs(destination_dir, exist_ok=True)
    dsp_main_bp = os.path.join(destination_dir, 'dsp_main.bin')
    dsp_overlay_bp = os.path.join(destination_dir, 'dsp_overlay.bin')
    shutil.copy(dsp_main, dsp_main_bp)
    shutil.copy(dsp_overlay, dsp_overlay_bp)

def _build_seliteos(build_target):
    if '-mini' in sys.argv:
        _build_target_with_define(build_target, 'CFG_FLASH_SPECS_MINI')
    else:
        _build_any_target(build_target)
    target = os.path.join('output', '3322', 'acore', build_target, 'seliteos_signed.bin')
    return target
def _build_recovery(build_target):
    if '-mini' in sys.argv:
        _build_target_with_define(build_target, 'CFG_FLASH_SPECS_MINI')
    else:
        _build_any_target(build_target)
    target = os.path.join('output', '3322', 'acore', build_target, 'recovery_signed.bin')
    return target
def _build_ssb_back(ssb_target):
    build_target = 'backup' + ssb_target
    target = os.path.join('output', '3322', 'acore', build_target, 'ssb_backup_signed.bin')
    if (os.path.exists(os.path.dirname(target)) is False):
        os.makedirs(os.path.dirname(target))

    ssb_bin = os.path.join('output', '3322', 'acore', ssb_target, 'ssb_signed.bin')
    if (os.path.exists(ssb_bin) is True):
        shutil.copy(ssb_bin, target)

    return target

def _build_nv(nv_target):
    target = os.path.join('output', '3322', 'acore', nv_target, '3322_all_nv.bin')
    return target

def _build_application(build_target, wvt_flag):
    if wvt_flag :
        _build_target_with_define(build_target, 'COM_UART_USE_AS_BT_WVT')
    elif '-mini' in sys.argv:
        _build_target_with_define(build_target, 'CFG_FLASH_SPECS_MINI')
    else :
        _build_any_target(build_target)
    target = os.path.join('output', '3322', 'acore', build_target, 'application_signed.bin')
    return target

def _build_bt_core(build_target):
    if '-mini' in sys.argv:
        _build_target_with_define(build_target, 'CFG_FLASH_SPECS_MINI')
    else:
        _build_any_target(build_target)
    target = os.path.join('output', '3322', 'bt_core', build_target, 'bt_signed.bin')
    return target

def _build_bt_rom(build_target):
    target = os.path.join('output', '3322', 'bt_core', build_target, 'bt_rom.bin')
    return target

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
    sign_py = os.path.join(g_build_env.get('TOOL').get('sign_cfg'), 'change_sign_cfg.py')
    target = os.path.join('output', '3322', 'root_public_key.bin')
    cmd = "%s %s -base %s -temp_dir %s -input %s -output %s -type %d" % (
            sys.executable,
            sign_py,
            os.path.join(g_build_env.get('TOOL').get('sign_cfg'), 'root_pubk.cfg'),
            os.path.join('output', '3322', 'conf', 'root_pubk.cfg'),
            os.path.join(g_build_env.get('TOOL').get('sign_cfg'), 'ec_bp256_oem_root_private_key.pem'),
            target,
            1
        )
    if (exec_shell(cmd, None) != 0):
        raise Exception("%s Failed!!!"%cmd)
    return target

def _build_partition():
    cfg = os.path.join(g_build_env.get('PARTITION').get('partition'))
    op = os.path.join('output', '3322', 'partition_table.bin')
    gen_flash_part_bin(cfg, op)
    target = os.path.join('output', '3322', 'partition_table_signed.bin')
    cmd = "%s %s -base %s -temp_dir %s -input %s -output %s -type %d" % (
            sys.executable,
            g_sign_py,
            os.path.join(g_build_env.get('TOOL').get('sign_cfg'), 'params.cfg'),
            os.path.join('output', '3322', 'conf', 'params.cfg'),
            os.path.join('output', '3322', 'partition_table.bin'),
            target,
            0
        )
    if (exec_shell(cmd, None) != 0):
        raise Exception("%s Failed!!!"%cmd)

    root_key = os.path.join('output', '3322', 'root_public_key.bin')
    new_bin = os.path.join('output', '3322', 'rootkey_partition_cfg.bin')
    if (os.path.exists(root_key) is True):
        shutil.copy(root_key, new_bin)
    else:
        create_binary_file(new_bin, 128)
    concatenate_files(new_bin, target, 128)

    cfct = os.path.join(g_build_env.get('TOOL').get('sign_cfg'), 'cfct3.bin')
    concatenate_files(new_bin, cfct, 1920); # 0x80 + 0x700 = 1920

    return new_bin

def _build_pack(fwpkg_name, bin_map):
    pack_py = os.path.join('tools', 'pkg', 'packet_create.py')
    fwpkg_full_name = os.path.join('output', '3322', 'fwpkg', '%s.fwpkg'%fwpkg_name)
    loader = "%s%s"%(bin_map.get('loader'), g_build_env.get('FWPKG').get('loader'))
    ssb = "%s%s"%(bin_map.get('ssb'), g_build_env.get('FWPKG').get('ssb'))
    seliteos = "%s%s"%(bin_map.get('seliteos'), g_build_env.get('FWPKG').get('seliteos'))
    ssb_back = "%s%s"%(bin_map.get('ssb_back'), g_build_env.get('FWPKG').get('ssb_back'))
    recovery = "%s%s"%(bin_map.get('recovery'), g_build_env.get('FWPKG').get('recovery'))
    partition = "%s%s"%(bin_map.get('partition'), g_build_env.get('FWPKG').get('partition'))
    dsp_main = "%s%s"%(bin_map.get('dsp_main'), g_build_env.get('FWPKG').get('dsp_main'))
    dsp_overlay = "%s%s"%(bin_map.get('dsp_overlay'), g_build_env.get('FWPKG').get('dsp_overlay'))
    app = "%s%s"%(bin_map.get('app'), g_build_env.get('FWPKG').get('application'))
    bt = "%s%s"%(bin_map.get('bt'), g_build_env.get('FWPKG').get('bt_core'))
    #bt_rom = "%s%s"%(bin_map.get('bt_rom'), g_build_env.get('FWPKG').get('bt_rom'))
    nv = "%s%s"%(bin_map.get('nv'), g_build_env.get('FWPKG').get('nv'))
    cmd = [sys.executable, pack_py, '-packet',
           fwpkg_full_name, loader, partition, ssb, ssb_back, recovery, bt, dsp_main, dsp_overlay, app, seliteos, nv
           ]
    if (exec_shell(cmd, None) != 0):
        raise Exception("%s Failed!!!"%str(cmd))

def _build_pack_mini(fwpkg_name, bin_map):
    pack_py = os.path.join('tools', 'pkg', 'packet_create.py')
    fwpkg_full_name = os.path.join('output', '3322', 'fwpkg', '%s_mini.fwpkg'%fwpkg_name)
    loader = "%s%s"%(bin_map.get('loader'), g_build_env.get('FWPKG_MINI').get('loader'))
    ssb = "%s%s"%(bin_map.get('ssb'), g_build_env.get('FWPKG_MINI').get('ssb'))
    seliteos = "%s%s"%(bin_map.get('seliteos'), g_build_env.get('FWPKG_MINI').get('seliteos'))
    ssb_back = "%s%s"%(bin_map.get('ssb_back'), g_build_env.get('FWPKG_MINI').get('ssb_back'))
    recovery = "%s%s"%(bin_map.get('recovery'), g_build_env.get('FWPKG_MINI').get('recovery'))
    partition = "%s%s"%(bin_map.get('partition'), g_build_env.get('FWPKG_MINI').get('partition'))
    app = "%s%s"%(bin_map.get('app'), g_build_env.get('FWPKG_MINI').get('application'))
    bt = "%s%s"%(bin_map.get('bt'), g_build_env.get('FWPKG_MINI').get('bt_core'))
    #bt_rom = "%s%s"%(bin_map.get('bt_rom'), g_build_env.get('FWPKG_MINI').get('bt_rom'))
    cmd = [sys.executable, pack_py, '-packet',
           fwpkg_full_name, loader, partition, ssb, ssb_back, recovery, bt, app, seliteos
           ]
    if (exec_shell(cmd, None) != 0):
        raise Exception("%s Failed!!!"%str(cmd))


all_in_one_list = [
    "3322_all_of_list",
    "3322_wstp_fpga",
    "3322_lpci_fpga",
    "3322_standard_fpga",
    "3322_standard_device_only_fpga",
    "3322_testsuit_fpga",
    "3322_nand_fpga",
    #"3322_wstp_asic",
    #"3322_standard_asic",
    #"3322_standard_device_only_asic",
    #"3322_testsuit_asic",
]

target_combination = {
    "3322_all_of_list":["all", "all"],
    "3322_wstp":["3322-ssb", "3322-wstp-app", "standard-3322-bt", "3322-seliteos", "3322-recovery"],
    "3322_nand":["3322-ssb", "3322-nand-app", "standard-3322-bt", "3322-seliteos", "3322-nand-recovery"],
    "3322_lpci":["3322-ssb", "3322-lpci-app", "standard-3322-btc-only", "3322-seliteos", "3322-recovery"],
    "3322_standard":["3322-ssb", "standard-3322-app", "standard-3322-bt", "3322-seliteos", "3322-recovery"],
    "3322_standard_device_only":["3322-ssb", "standard-3322-app", "standard-3322-btc-only", "3322-seliteos", "3322-recovery"],
    "3322_testsuit":["3322-ssb", "testsuite-3322-app", "testsuite-3322-bt",  "3322-seliteos", "3322-recovery"],
}

def do_build(select_index):
    rootpubk = _build_root_key()
    loader = _build_loader()
    dsp_main = g_build_env.get('DSP_BIN').get('dsp_main')
    dsp_overlay = g_build_env.get('DSP_BIN').get('dsp_overlay')
    partition = _build_partition()
    start_index = select_index
    end_index = select_index + 1
    if select_index == 0:
        start_index = 1
        end_index = len(all_in_one_list)
    for i in range(start_index, end_index):
        # 找到最后一个下划线的位置
        last_index = all_in_one_list[i].rfind("_")
        # 使用切片将字符串分成两半
        fwpkg_name = all_in_one_list[i][:last_index]  # 从头到最后一个下划线前
        fwpkg_type = all_in_one_list[i][last_index + 1:]  # 从下划线后开始到结尾
        #ssb
        ssb = _build_ssb(target_combination[fwpkg_name][0] + "-" + fwpkg_type)
        #ssb_back
        ssb_back = _build_ssb_back(target_combination[fwpkg_name][0] + "-" + fwpkg_type)
        #app
        wvt_flag = False
        if fwpkg_name == '3322_standard_device_only' :
            wvt_flag = True
        if fwpkg_name == '3322_los_emmc' :
            app = _build_application('3322-for-emmc-update', False)
        else:
            app = _build_application(target_combination[fwpkg_name][1] + "-" + fwpkg_type, wvt_flag)
        #bt
        print("xx=%s" % (target_combination[fwpkg_name][2]))
        bt = _build_bt_core(target_combination[fwpkg_name][2]+ "-" + fwpkg_type)
        bt_rom = _build_bt_rom(target_combination[fwpkg_name][2]+ "-" + fwpkg_type)
        seliteos = _build_seliteos(target_combination[fwpkg_name][3]+ "-" + fwpkg_type)
        recovery = _build_recovery(target_combination[fwpkg_name][4]+ "-" + fwpkg_type)
        #nv
        nv = _build_nv(target_combination[fwpkg_name][1] + "-" + fwpkg_type)
        bin_map = {'loader':loader, 'partition':partition, 'ssb':ssb, 'ssb_back':ssb_back, 'recovery':recovery, 'dsp_main':dsp_main,
                'dsp_overlay':dsp_overlay, 'seliteos':seliteos, 'app':app, 'bt':bt, 'bt_rom':bt_rom, 'nv':nv}
        if '-mini' in sys.argv:
            _build_pack_mini(all_in_one_list[i], bin_map)
        else:
            _build_pack(all_in_one_list[i], bin_map)
    pass

if __name__ == "__main__":
    path = build_env_init()
    try:
        check_binary_exist()
        if '-s' in sys.argv:
            print("Here are the available target_names")
            cur_index = 0
            for index in range(len(all_in_one_list)):
                print("%-3d : %s" % (cur_index, all_in_one_list[index]))
                cur_index += 1
            print("Input index numbers for build or q for quit(num/q):", end='')
            # 输入序号或q/Q
            select_index = input()
            if select_index.lower() == 'q':
                sys.exit(0)
            do_build(int(select_index))
        else:
            do_build(0)
        _copy_dsp_bin() # 后续dsp代码全部开源后该处还需要优化，当前获取的bin不一定是最新的。
        os.chdir(path)
        sys.exit(0)
    except Exception as e:
        os.chdir(path)
        print("Build Failed! Exception:", e)
        sys.exit(1)
