#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2015-2023. All rights reserved.
# Description: make bigo

import os
import re
import sys
import logging

image_symbols = []


def work(libs_symbols_path):
    '''
    write symbols path to object file
    '''
    logging.basicConfig(format='%(message)s', level=logging.NOTSET, stream=sys.stdout)
    cur_lib, cur_obj = '', ''
    last_lib, last_obj = '', ''
    pattern = re.compile(r'File: (\S+)\((\S+)\)')
    with open(libs_symbols_path, 'r') as file:
        for line in file.readlines():
            symbol = line.strip().split()
            match = pattern.match(line)
            if match:
                cur_lib, cur_obj = match.groups()
                continue
            if len(symbol) != 8 or symbol[0] == 'Num:':
                continue

            bind, ndx, name = symbol[4], symbol[6], symbol[7]
            is_bind_valid = bind in ('GLOBAL', 'WEAK')
            is_bind_ndx_name_valid = (is_bind_valid and ndx != 'UND' and name in image_symbols)
            if is_bind_ndx_name_valid and (cur_lib != last_lib or cur_obj != last_obj):
                logging.info('%s %s', os.path.split(cur_lib)[-1], cur_obj)
                last_lib, last_obj = cur_lib, cur_obj


def main():
    '''
    argv[1]:   symbol_list.(wow/scatter)
    argv[2]:   input file 'libs_symbols.list'
    '''
    if len(sys.argv) < 3:
        logging.basicConfig(format='%(levelname)s: %(funcName)s: %(message)s', level=logging.NOTSET, stream=sys.stderr)
        logging.info('Usage: %s image_symbols_list_path libs_symbols_list_path', sys.argv[0])
        return

    # load image symbols list
    abspath = os.path.realpath(sys.argv[1])
    with open(abspath, 'r') as file:
        for line in file.readlines():
            image_symbols.append(line.strip())

    abspath = os.path.realpath(sys.argv[2])
    work(abspath)

if __name__ == '__main__':
    main()
