#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
'''
Description: analysis and sort liba's size
'''

from __future__ import absolute_import
import sys
import os
import stat
import operator
import subprocess
from collections import namedtuple
if sys.version_info.major > 2:  # py3
    from functools import reduce

InfoStruct = namedtuple('InfoStruct', 'arname objname text data bss total')

FILE_LIST = []
edit_suffixs = ['.a', '.o', '.O']
obj_info = []
liba_info = []
dict_text = {}
dict_data = {}
dict_bss = {}
TOTAL_TEXT = 0
TOTAL_DATA = 0
TOTAL_BSS = 0
LIBA_SIZE_TXT = "liba_size.txt"


def do_work(size_list):
    '''
    Parses Size List
    '''
    for line in size_list:
        split_line = line.strip().split()
        if split_line[0] == 'text':
            continue
        if len(split_line) > 6:
            text, data, bss, dec, obj_name, ar_name = split_line[0], split_line[1], split_line[2], \
                                                    split_line[3], split_line[5], split_line[7]
            ar_name = ar_name[0:len(ar_name) - 1]
        else:
            text, data, bss, dec, obj_name = split_line[0], split_line[1], split_line[2], split_line[3], split_line[5]
            ar_name = obj_name
            obj_name = os.path.basename(obj_name)
        ar_name = os.path.basename(ar_name)
        info_item = InfoStruct(ar_name, obj_name, text, data, bss, dec)
        obj_info.append(info_item)


def sort_result():
    '''
    Sort Result
    '''
    global TOTAL_TEXT
    global TOTAL_DATA
    global TOTAL_BSS
    obj_info.sort(key=lambda x:(int(x.text) + int(x.data)), reverse=True)
    for var in obj_info:
        if var.arname in dict_text:
            dict_text[var.arname] = dict_text[var.arname] + int(var.text)
            dict_data[var.arname] = dict_data[var.arname] + int(var.data)
            dict_bss[var.arname] = dict_bss[var.arname] + int(var.bss)
        else:
            dict_text[var.arname] = int(var.text)
            dict_data[var.arname] = int(var.data)
            dict_bss[var.arname] = int(var.bss)
    for key, var in dict_text.items():
        TOTAL_TEXT += var
        TOTAL_DATA += dict_data[key]
        TOTAL_BSS += dict_bss[key]
        info_item = InfoStruct(key, ' ', str(var), str(dict_data[key]), str(dict_bss[key]), '0')
        liba_info.append(info_item)
    liba_info.sort(key=lambda x:(int(x.text) + int(x.data)), reverse=True)


def display_result():
    '''
    Display Result
    '''
    print('\nSizeTable sorted by text+data')
    print('{0:<20} {1:<30} {2:<10} {3:<10} {4:<}'.format( \
        'liba_file', ' ', 'text', 'data', 'bss'))
    print('{0:<20} {1:<30} {2:<10} {3:<10} {4:<}\n'.format( \
        'total', ' ', TOTAL_TEXT, TOTAL_DATA, TOTAL_BSS))
    for var in liba_info:
        print('{0:<20} {1:<30} {2:<10} {3:<10} {4:<}'.format( \
            var.arname, ' ', var.text, var.data, var.bss))
    print('')


def ouput_to_txt(output_dir):
    '''
    Write Results To File
    '''
    pwd = os.getcwd()
    output = os.path.realpath(output_dir)
    relative_path = output.replace(pwd, '.', 1)
    file_name = os.path.join(os.path.realpath(output_dir), LIBA_SIZE_TXT)
    flags = os.O_WRONLY | os.O_CREAT
    modes = stat.S_IWUSR | stat.S_IRUSR | stat.S_IRGRP | stat.S_IROTH
    with os.fdopen(os.open(file_name, flags, modes), 'w') as file_obj:
        file_obj.write('size table sorted by text+data\n')
        file_obj.write('{0:<20} {1:<30} {2:<10} {3:<10} {4:<}\n'.format( \
            'liba_file', 'obj_file', 'text', 'data', 'bss'))
        file_obj.write('\n')
        file_obj.write('{0:<20} {1:<30} {2:<10} {3:<10} {4:<}\n'.format( \
            'total', ' ', TOTAL_TEXT, TOTAL_DATA, TOTAL_BSS))
        file_obj.write('\n')
        for var in liba_info:
            file_obj.write('{0:<20} {1:<30} {2:<10} {3:<10} {4:<}\n'.format( \
                var.arname, ' ', var.text, var.data, var.bss))
        file_obj.write('\n')
        file_obj.write('Detail info:\n')
        for arg in liba_info:
            file_obj.write('{0:<20} {1:<30} {2:<10} {3:<10} {4:<}\n'.format( \
                arg.arname, ' ', dict_text[arg.arname], dict_data[arg.arname], dict_bss[arg.arname]))
            for var in obj_info:
                if var.arname == arg.arname:
                    file_obj.write('{0:<20} {1:<30} {2:<10} {3:<10} {4:<}\n'.format( \
                        ' ', var.objname, var.text, var.data, var.bss))
    print("Detail info in {0}\n".format(os.path.join(relative_path, LIBA_SIZE_TXT)))


def get_file(file_path):
    '''
    Recursively Obtain Dir Files.
    '''
    global FILE_LIST
    if not os.path.isdir(file_path): # is file
        print("error arg is not dir")
        sys.exit(1)

    for file_name in os.listdir(file_path):
        cur_path = os.path.join(file_path, file_name)
        if os.path.isdir(cur_path):
            get_file(cur_path)
        elif reduce(operator.ior, map(lambda x: bool(file_name.endswith(x)), edit_suffixs)):
            FILE_LIST.append(cur_path)


def run_shell_return(command):
    '''
    Run Shell Command, And Return Result
    '''
    pipe_handle = subprocess.Popen(command, stdout=subprocess.PIPE, \
            stderr=subprocess.PIPE, shell=False, universal_newlines=True)
    output = pipe_handle.stdout.read()
    pipe_handle.wait()
    errout = pipe_handle.stderr.read()
    if errout:
        print(errout, file=sys.stderr)
    pipe_handle.stdout.close()
    pipe_handle.stderr.close()
    return output, pipe_handle.returncode


def get_size(size_cmd):
    '''
    Get File Size
    '''
    global FILE_LIST
    cmd = [size_cmd] + FILE_LIST
    out_text, code = run_shell_return(cmd)
    if code != 0:
        print("error:{0}".format(size_cmd))
        sys.exit(1)
    size_list = out_text.splitlines(False)
    do_work(size_list)

# argv[1]:   size cmd
# argv[2]:   output dir
# argv[3]:   liba dirs
if __name__ == '__main__':
    print("run {0}".format(sys.argv[0]))
    if len(sys.argv) < 4:
        print("argv numbers error:{0}".format(len(sys.argv)))
        sys.exit(1)
    else:
        if "size" not in sys.argv[1]:
            print("size cmd error:{0}".format(sys.argv[1]))
            sys.exit(1)
        if not os.path.isdir(sys.argv[2]):
            print("output dir error:{0}".format(sys.argv[2]))
            sys.exit(1)
    if len(sys.argv[3]) == 1:
        get_file(os.path.realpath('.'))
    else:
        for path in sys.argv[3 : ]:
            get_file(os.path.realpath(path))

    get_size(sys.argv[1])
    sort_result()
    display_result()
    ouput_to_txt(sys.argv[2])
