#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#
# Description: projects rom targets rom_bin compare, just befor rom fixed \n
#
# History: \n
# 2023-03-22, Create file. \n
import sys
import os
import json
from build_utils import root_path, compare_bin, output_root

output_root = os.path.abspath(output_root)
build_path = os.path.join(os.path.split(__file__)[0], '..', '..')
target_config_path = os.path.abspath(os.path.join(build_path, 'config', 'target_config'))

def replace_file_path(file_path, chip):
    file_path = file_path.replace('<out_root>', output_root)
    file_path = file_path.replace('<chip>', chip)
    return file_path

def compare(chip, compare_key):
    chip_config_path =  os.path.join(target_config_path, chip)
    target_compare_config = os.path.join(chip_config_path, 'compare', 'compare.json')
    if not os.path.exists(target_compare_config):
        return
    with open(target_compare_config, 'r') as fp:
        temp = fp.read()
    configs = json.loads(temp)
    if compare_key:
        configs = configs[compare_key]
    for config in configs:
        file_name = config['name']
        basefile = replace_file_path(os.path.join(config["targets"][0], file_name), chip)
        if not os.path.exists(basefile):
            continue
        for targets in config["targets"][1:]:
            target_filee = replace_file_path(os.path.join(targets, file_name), chip)
            if not os.path.exists(target_filee):
                continue
            if not compare_bin(basefile, target_filee):
                return False
    return True

if __name__ == "__main__":
    if len(sys.argv) > 3:
        print("Compare Param Error")
        sys.exit(1)
    chip = sys.argv[1]
    compare_key = sys.argv[2] if len(sys.argv) == 3 else ""
    if compare(chip, compare_key):
        sys.exit(0)
    sys.exit(1)