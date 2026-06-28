#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2015-2023. All rights reserved.
# Description: make liblist

import sys
import os
import logging

symbol_libs = {}


def do_work(full_path):
    '''
    write symbols path to object file
    '''
    cmd = "readelf -sW {}".format(full_path)
    for line in os.popen(cmd):
        symbol = line.strip().split()
        if len(symbol) != 8 or symbol[0] == 'Num:':
            continue
        bind, ndx, name = symbol[4], symbol[6], symbol[7]
        is_bind_valid = bind in ('GLOBAL', 'WEAK')
        if is_bind_valid and ndx != 'UND' and name in symbol_libs:
            symbol_libs.setdefault(name, []).append(full_path)
            return


def work(path):
    '''
    get symbols path
    '''
    if not os.path.isdir(path):
        do_work(path)
        return

    for file_name in os.listdir(path):
        cur_path = os.path.join(path, file_name)
        if os.path.isdir(cur_path):
            work(cur_path)
        elif file_name.endswith('.a'):
            do_work(cur_path)


def main():
    '''
    argv[1]:   symbol_list.(wow/scatter)
    argv[2]:   output lib dirs
    '''
    if len(sys.argv) < 3:
        logging.basicConfig(format='%(levelname)s: %(funcName)s: %(message)s', level=logging.NOTSET, stream=sys.stderr)
        logging.info('Usage: %s image_symbols_list_path libs_symbols_list_path', sys.argv[0])
        return

    # load symbol list
    with open(sys.argv[1], 'r') as file:
        for line in file.readlines():
            symbol_libs[line.strip()] = []

    for path in sys.argv[2 :]:
        abspath = os.path.realpath(path)
        work(abspath)

    logging.basicConfig(format='%(message)s', level=logging.NOTSET, stream=sys.stdout)
    for symbol, libs in sorted(symbol_libs.items(), key=lambda d: len(d[1])):
        if not libs:
            continue
        if len(libs) == 1:
            logging.info(libs[0])
            continue

        logging.info('++++++++++++++++++++++++++++++++++++++')
        logging.info('%s defined in %s libs:', symbol, str(len(libs)))
        for lib in libs:
            logging.info(lib)

if __name__ == '__main__':
    main()
