#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
'''Description: LiteOS Building System'''

from __future__ import absolute_import
import os
import sys
import getopt
import subprocess

CONFIG_PATH = './self_src/tools/build/config'
LOG_FILE = ''
TIME_LABEL = '''awk 'BEGIN{IGNORECASE=1} {
    if (match($0, /error/)) {
        print strftime("%Y-%m-%d_%H:%M:%S"), "[Err ] ", $0;
    } else if (match($0, /warning/)) { \
        print strftime("%Y-%m-%d_%H:%M:%S"), "[Warn] ", $0;
    } else { \
        print strftime("%Y-%m-%d_%H:%M:%S"), "[Info] ", $0;
    }
}' '''


def helper():
    '''
    help info
    '''
    print('Usage : build.py [OPTION]...')
    print('  -b          board')
    print('  -v          version, including:')
    print('              1. debug')
    print('              2. release')
    print('              3. test')
    print('              4. test,dynload')
    print('  -e          extra tools, including:')
    print('              1. menuconfig')
    print('  -m          compile methods, including:')
    print('              1. makefile')
    print('              2. cmake')
    print('')
    print('Examples:')
    print('  ./build.py -b hi3556v200 -v release')
    print('  ./build.py -b npu_v200_be_got -v release -m cmake -o buildcmakeout')
    print('')


def dir_get(path):
    '''
    Get working directory, LiteOS
    '''
    name = "RTOS_Lite"
    idx = path.find(name)
    if idx == -1:
        print("Execution path error!")
        sys.exit(-1)
    else:
        path = path[:idx + len(name)]

    return path


def invalid(info):
    '''
    Prompt Help info
    '''
    print(info, 'not recognized\n')
    helper()


def makefile_build(path, is_dyn_build):
    '''
    Make Build
    '''
    global TIME_LABEL
    global LOG_FILE

    if is_dyn_build is True:
        subprocess.call("make dynload 2>&1 | {} | tee {}".format(TIME_LABEL, LOG_FILE), shell=True)
    else:
        # if not dynload build, clean dynload first to avoid impact on the following build.
        subprocess.call("make dynloadclean  2>&1 | {} | tee {}".format(TIME_LABEL, LOG_FILE), shell=True)

    subprocess.call(['cp', '-rf', path, '.config'])
    subprocess.call("make clean 2>&1 | {} | tee {}".format(TIME_LABEL, LOG_FILE), shell=True)
    subprocess.call("make -j 2>&1 | {} | tee {}".format(TIME_LABEL, LOG_FILE), shell=True)


def cmake_build(path, cmakeout):
    '''
    CMake Build
    '''
    subprocess.call(['cp', '-rf', path, '.config'])
    subprocess.call(['./build/build_cmake.sh', 'cleanall', cmakeout], shell=False)
    subprocess.call(['./build/build_cmake.sh', 'build', cmakeout], shell=False)


def build(path, is_dyn_build, methods, out):
    '''
    Choose Build Method
    '''
    if os.path.exists(path) is False:
        print(path, 'is not existed. Please check!!')
        return
    if methods == 'makefile':
        makefile_build(path, is_dyn_build)
    else:
        cmake_build(path, out)


def main(argv):
    '''
    Main Func
    '''
    global CONFIG_PATH
    global LOG_FILE
    board = ''
    version = 'debug'
    extra = ''
    out = ''
    methods = 'makefile'

    top_dir = dir_get(os.path.abspath(os.path.dirname(__file__)))
    os.chdir(top_dir)

    log_dir = os.path.join(top_dir, 'build', 'logs')
    LOG_FILE = os.path.join(log_dir, 'build.log')
    if not os.path.isdir(log_dir):
        os.mkdir(log_dir)

    # if no input arg, then print helper.
    if len(argv) == 0:
        helper()
        sys.exit(1)

    try:
        opts, _ = getopt.getopt(argv, "hb:v:m:o:e", ["board=", "version=", "methods=", "out=", "extra="])
    except getopt.GetoptError as err:
        # will print something like "option -a not recognized".
        print(str(err), '\n')
        helper()
        sys.exit(1)
    for opt, arg in opts:
        if opt == '-h':
            helper()
            sys.exit()
        elif opt in ("-b", "--board"):
            board = arg
        elif opt in ("-v", "--version"):
            version = arg
        elif opt in ("-e", "--extra"):
            extra = arg
        elif opt in ("-o", "--out"):
            out = arg
        elif opt in ("-m", "--methods"):
            methods = arg

    if extra == 'menuconfig':
        subprocess.call(['make', 'menuconfig'], shell=False)
    elif extra != '':
        invalid('{} {}'.format('option -e', extra))
        sys.exit(1)

    if board != '':
        if version == 'debug':
            path = '{}{}{}{}'.format(CONFIG_PATH, '/debug/', board, '.config')
            build(path, False, methods, out)
        elif version == 'release':
            path = '{}{}{}{}'.format(CONFIG_PATH, '/', board, '.config')
            build(path, False, methods, out)
        elif version == 'test':
            path = '{}{}{}{}'.format(CONFIG_PATH, '/testsuit/', board, '_testsuit.config')
            build(path, False, methods, out)
        elif version == 'test,dynload':
            path = '{}{}{}{}'.format(CONFIG_PATH, '/testsuit/', board, '_testsuit.config')
            build(path, True, methods, out)
        else:
            invalid('{} {}'.format('option -v', version))


if __name__ == "__main__":
    main(sys.argv[1:])
