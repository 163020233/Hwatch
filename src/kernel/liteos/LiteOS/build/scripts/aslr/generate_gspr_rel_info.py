#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2024. All rights reserved.
'''
Generate aslr rel info file
'''

import sys
sys.path.append(sys.path[0]+"/../")
import os
import logging
import argparse
from data_manager.data import DataManager


LOG_FORMAT = '[%(funcName)s:%(lineno)d, %(levelname)s]: %(message)s'
logging.basicConfig(level=logging.NOTSET, format=LOG_FORMAT)

parser = argparse.ArgumentParser(description='LASLR GSPR Table Generation And Check')
parser.add_argument('op_type', type=str, choices=['GSPR', 'GSPR_check'], help='operation type')
parser.add_argument('cross_compile', type=str, help='cross compiler')
parser.add_argument('elf_path', type=str, help='elf path')
parser.add_argument('--config_path', metavar='config_path', type=str, help='config path', default=None)
args = parser.parse_args()


# argv[1]: process type(GSPR GSPR_check)
# argv[2]: process cmd(arm-none-eabi- riscv32-unknown-elf-)
# argv[3]: process filename (/usr1/xxx/out/vs_server)
if __name__ == '__main__':
    if not args.elf_path or not os.path.isfile(args.elf_path):
        logging.error("process file invalid %s", args.elf_path)
        sys.exit(1)
    if args.config_path is not None:
        if not os.path.isfile(args.config_path):
            logging.error("config file invalid %s", args.config_path)
            sys.exit(1)

    dir_name = os.path.dirname(os.path.realpath(args.elf_path))

    data_mgr = DataManager.create_from_elf(args.cross_compile, args.elf_path, args.config_path)
    if args.op_type == "GSPR_check":
        data_mgr.output_to_file(dir_name, DataManager.CHECK)
    elif args.op_type == "GSPR":
        data_mgr.output_to_file(dir_name, DataManager.GENERATE)
