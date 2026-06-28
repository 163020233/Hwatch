#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
'''
Check image reloc info
'''

import sys
import os
import logging
sys.path.append(sys.path[0]+"/../")
from util.utils import Utils
from data_manager.data import DataManager

LOG_FORMAT = '[%(funcName)s:%(lineno)d, %(levelname)s]: %(message)s'
logging.basicConfig(level=logging.NOTSET, format=LOG_FORMAT)

DATA_MANAGER = None
elf_got_rel_dict = {
    'ARM' : ['R_ARM_RELATIVE'],
    'AArch64' : ['R_AARCH64_RELATIVE'],
    'RISC-V' : ['R_RISCV_RELATIVE'],
}


def parse_elf_header(cmd, process_file, config_file):
    '''
    Parse elf header
    '''
    global DATA_MANAGER

    DATA_MANAGER = DataManager.create_from_elf(cmd, process_file, config_file)
    if elf_got_rel_dict.get(DATA_MANAGER.targets.machine, 'default') == 'default':
        logging.error("arch type not support")
        sys.exit(1)


def process_got(cmd, process_file):
    '''
    Verification procedure for GOT
    '''
    cmd_list = [''.join([cmd, 'readelf'])]
    cmd_list.append("-Wr")
    cmd_list.append(process_file)
    out_text, return_code = Utils.run_shell_return(cmd_list, None)
    if return_code != 0:
        logging.error("%s return %s", cmd_list, return_code)
        sys.exit(1)
    last_rel_offset = DATA_MANAGER.targets.image_begin
    for line in out_text.splitlines(False):
        line_string = line.strip().split()
        if len(line_string) < 3 or line_string[0] == 'Relocation' or line_string[0] == 'Offset':
            continue
        rel_offset = int(line_string[0], 16)
        rel_type = line_string[2]
        if (rel_offset < DATA_MANAGER.targets.image_begin) or (rel_offset > DATA_MANAGER.targets.image_end):
            logging.error("Relocation list offset is out of %s to %s",
                DATA_MANAGER.targets.image_begin, DATA_MANAGER.targets.image_end)
            sys.exit(1)
        if last_rel_offset > rel_offset:
            logging.error("Relocation list offset is not ascending order, offset %s", rel_offset)
            sys.exit(1)
        last_rel_offset = rel_offset
        if elf_got_rel_dict.get(DATA_MANAGER.targets.machine, 'default') == 'default':
            logging.error("rel type %s for %s not support", rel_type, DATA_MANAGER.targets.machine)
            sys.exit(1)

# argv[1]: process type(got or gspr)
# argv[2]: process cmd(arm-none-eabi- riscv32-unknown-elf-)
# argv[3]: process filename
# argv[4]: (optional) config filename
if __name__ == '__main__':
    logging.info("enter %s", sys.argv[0])
    if (len(sys.argv) != 4) and (len(sys.argv) != 5):
        logging.error("argv number error: need 4 or 5 argv, pass %s", len(sys.argv))
        for index in range(len(sys.argv)):
            logging.error("%s %s", index, sys.argv[index])
        sys.exit(1)
    else:
        if not ((len(sys.argv[3]) > 0) and (os.path.isfile(sys.argv[3]))):
            logging.error("process file invalid %s", sys.argv[3])
            sys.exit(1)
        if sys.argv[1] == "GOT":
            _config_file = None if (len(sys.argv) < 5) else sys.argv[4]
            parse_elf_header(sys.argv[2], sys.argv[3], _config_file)
            process_got(sys.argv[2], sys.argv[3])
        elif sys.argv[1] == "GSPR":
            logging.error("not support GSPR now")
        else:
            logging.error("aslr type invalid %s", sys.argv[1])
