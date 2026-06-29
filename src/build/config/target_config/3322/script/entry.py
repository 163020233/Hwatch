#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.

import os
import json
import sys
import platform
import subprocess
import shutil
import re

from utils.build_utils import root_path, exec_shell, compare_bin

from typing import Dict, Any


SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))


def do_cmd(target_name: str, hook_name: str, env: Dict[str, Any])->bool:
    if hook_name != 'build_post':
        return True

    if env.get('pke_rom_bin'):
        # gen pke_rom_bin
        gen_pke_rom_bin_sh = os.path.join(SCRIPT_DIR, 'pke_rom.sh')
        if os.path.exists(os.path.join(root_path, \
            'bootloader/bootrom_3322/drivers/hal/security_unified/hal_cipher/pke/rom_lib.c')):
            print("generate pke_rom_bin ing...")
            errcode = exec_shell(['sh', gen_pke_rom_bin_sh, root_path], None, True)
            if errcode != 0:
                print("generate pke_rom_bin failed!")
                return False
            print("generate pke_rom_bin done!")

            # verify pke rom bin
            if env.get('fixed_pke'):
                bin1 = os.path.join(root_path, "output", env.get('chip'), env.get('core'), 'pke_rom', 'pke_rom.bin')
                bin2 = env.get('fixed_pke_path', '').replace('<root>', root_path)
                if not compare_bin(bin1, bin2):
                    print(f"Verify pke rom bin ERROR! :{bin1} is not same with {bin2}")
                    return False

    if env.get('rom_in_one'):
        # gen rom_in_one
        rom_in_one = os.path.join(SCRIPT_DIR, 'rom_in_one.sh')
        print("generate rom_in_one ing...")
        errcode = exec_shell(['sh',
                             rom_in_one,
                             env.get('core'),
                             root_path,
                             env.get('bin_name'),
                             target_name],
                             None,
                             True)

        if errcode != 0:
            print("generate rom_in_one failed!")
            return False
        print("generate rom_in_one done!")

        # verify codepoint bin
        bin1 = os.path.join(root_path, "output", env.get('chip'), env.get('core'), \
                target_name, env.get('bin_name')+'_rompack.bin')
        if env.get('fixed_rom_in_one') and os.path.isfile(bin1):# only rompack bin exists
            bin2 = env.get('fixed_rom_in_one_path', '').replace('<root>', root_path)
            if not compare_bin(bin1, bin2):
                print(f"Verify rom_in_one bin ERROR! :{bin1} is not same with {bin2}")
                return False

    if env.get('fixed_romboot'):
        # verify romboot bin
        output_path = os.path.join(root_path, 'output/3322/acore/3322-romboot/')
        bin1 = os.path.join(output_path, '%s.bin' % env.get('bin_name'))
        bin2 = env.get('fixed_romboot_path').replace('<root>', root_path)
        if not compare_bin(bin1, bin2):
            print(f"ERROR! :{bin1} is not same with {bin2}")
            return False
        bin1 = os.path.join(root_path, 'output/3322/acore/pke_rom/pke_rom.bin')
        bin2 = env.get('fixed_pke_path').replace('<root>', root_path)
        if not compare_bin(bin1, bin2):
            print(f"ERROR! :{bin1} is not same with {bin2}")
            return False
        print("Romboot Same...")
    return True


def config_check(root_path, target_name):
    config_file_path = os.path.join(root_path, "build", "config", "target_config", "3322", "config.py")
    with open(config_file_path, "r", encoding="utf-8") as f:
        for i in f:
            if target_name in i:
                print(target_name, " in config.py.")
                return True
    return False
