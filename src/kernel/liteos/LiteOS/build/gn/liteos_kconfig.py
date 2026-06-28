#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
'''Description: generating GN and menuconfig header files'''
import re
import sys
import io
import shutil
import logging


def generate_menuconfig_headr(prepend_config, valid_config_input, config_output_file):

    '''
    generating menuconfig header files based on .config
    '''
    config_dict = {}
    with io.open(valid_config_input, "r", encoding="utf-8") as f:
        lines = f.readlines()

    for line in lines:
        line = line.strip()
        if not line.startswith(prepend_config):
            continue

        if "=" not in line:
            continue

        var_name, var_val = line.split("=", 1)
        var_name = var_name.strip()
        var_val = var_val.strip()
        if var_val == 'y':
            var_val = '1'
        elif var_val == 'n':
            var_val = '0'

        config_dict[var_name] = var_val
        if config_output_file:
            with io.open(config_output_file, "a", encoding="utf-8") as f_out:
                if var_val == '1':
                    f_out.write(u"#define {} 1\n".format(var_name))
                else:
                    f_out.write(u"#define {} {}\n".format(var_name, var_val))

    return config_dict


def insert_content_infirstline(file_path, line_content):

    '''
    insert y=true in the first line
    '''
    try:
        with open(file_path, 'r') as f:
            original_content = f.read()

        with open(file_path, 'w') as f:
            f.write("{}\n{}".format(line_content, original_content))

    except Exception as e:
        logging.error(e)
        sys.exit(1)


def remove_0x_fromfile(filePath):

    '''
    delete 0x from file
    '''
    with open(filePath, 'r') as file:
        content = file.read()
    modified_content = re.sub(r'=\s*0x', '=', content)
    with open(filePath, 'w') as file:
        file.write(modified_content)

if __name__ == "__main__":
    if len(sys.argv) != 4:
        sys.exit(1)

    inputFilePath = sys.argv[1]
    menuconfigHead = sys.argv[2]
    liteOsConfigGni = sys.argv[3]
    configs = generate_menuconfig_headr(
        prepend_config="LOSCFG_",
        valid_config_input=inputFilePath,
        config_output_file=menuconfigHead
    )
    shutil.copy(inputFilePath, liteOsConfigGni)

    insert_content_infirstline(liteOsConfigGni, "y=true")

    remove_0x_fromfile(liteOsConfigGni)
