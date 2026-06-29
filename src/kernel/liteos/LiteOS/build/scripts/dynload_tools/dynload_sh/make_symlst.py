#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2015-2019. All rights reserved.
'''
Description: generates a list of external symbols (undefined symbols)
             that the library under the specified path depends on
'''

from __future__ import absolute_import
import os
import operator
from functools import reduce
from argparse import ArgumentParser

edit_suffixs = ['.o', 'so', 'so.53', 'so.51', 'so.54', 'so.56']
namesExternModule = []
namesUserModule = []


def do_work(full_path):
    '''
    Read Symbol
    '''
    cmd = "readelf -sW {}".format(full_path)
    for line in os.popen(cmd):
        info = line.strip().split()
        if len(info) != 8 or info[0] == 'Num:':
            continue
        bind, ndx, name = info[4], info[6], info[7]
        name = name.split('@')[0]
        if (bind in ('GLOBAL', 'WEAK')) and (ndx != 'UND') and (name not in namesUserModule):
            namesUserModule.append(name)
            if name in namesExternModule:
                namesExternModule.remove(name)
        not_appended = (name not in namesExternModule) and (name not in namesUserModule)
        if (bind in ('GLOBAL', 'WEAK')) and (ndx == 'UND') and not_appended:
            namesExternModule.append(name)


def work(full_path):
    '''
    Traversing Dir
    '''
    if not os.path.isdir(full_path):
        do_work(full_path)
        return

    for file_name in os.listdir(full_path):
        cur_path = os.path.join(full_path, file_name)
        if os.path.isdir(cur_path):
            work(cur_path)
        elif reduce(operator.ior, map(lambda x: bool(file_name.endswith(x)), edit_suffixs)):
            do_work(cur_path)


if __name__ == '__main__':
    parser = ArgumentParser(description='gsymbol table generator')
    parser.add_argument(dest='path', metavar='path', nargs='*', help='File or path')
    args = parser.parse_args()

    if not args.path:
        work(os.path.realpath('.'))
    else:
        for path in args.path:
            realpath = os.path.realpath(path)
            work(realpath)

    NAMES = 0
    namesExternModule.sort()
    for NAMES in namesExternModule:
        print(NAMES)
