#!/usr/bin/env python3
# coding=utf-8
# Copyright © Huawei Technologies Co., Ltd. 2010-2024. All rights reserved.

'''prepare/cut_code.py'''

import sys
from os import path, walk, listdir, remove, environ, rmdir
from sys import argv
import shutil
import logging
from cut_file import DIR2DEL_COMMON, FILE2DEL_COMMON, DIR2DEL_5G, FILE2DEL_5G, DIR2DEL_TRANS, DIR2DEL
from cut_file import FILE2DEL_TRANS, DIR2DEL_4G, FILE2DEL_4G, DIR2DEL_3G, FILE2DEL, FILE2DEL_3G
from cut_file_l2 import DIR2DEL_COMMON_L2, FILE2DEL_COMMON_L2, DIR2DEL_L2, FILE2DEL_L2
from cut_file_emru import DIR2DEL_COMMON_EMRU, FILE2DEL_COMMON_EMRU, DIR2DEL_EMRU, FILE2DEL_EMRU
from cut_file_h import DIR2DEL_H, FILE2DEL_H

logging.basicConfig(level=logging.DEBUG)

LIBCK_HOME = path.dirname(path.dirname(path.dirname(path.abspath(__file__))))

PRODUCTS = {
            'trans': [DIR2DEL_COMMON, FILE2DEL_COMMON, DIR2DEL_TRANS, FILE2DEL_TRANS],
            '5g': [DIR2DEL_COMMON, FILE2DEL_COMMON, DIR2DEL_5G, FILE2DEL_5G],
            'lte': [DIR2DEL_COMMON, FILE2DEL_COMMON, DIR2DEL_4G, FILE2DEL_4G],
            '4g': [DIR2DEL_COMMON, FILE2DEL_COMMON, DIR2DEL_4G, FILE2DEL_4G],
            '3g': [DIR2DEL_COMMON, FILE2DEL_COMMON, DIR2DEL_3G, FILE2DEL_3G],
            '2g': [DIR2DEL_COMMON, FILE2DEL_COMMON, DIR2DEL, FILE2DEL],
            'l2': [DIR2DEL_COMMON_L2, FILE2DEL_COMMON_L2, DIR2DEL_L2, FILE2DEL_L2],
            'emru': [DIR2DEL_COMMON_EMRU, FILE2DEL_COMMON_EMRU, DIR2DEL_EMRU, FILE2DEL_EMRU],
            'else': [DIR2DEL_COMMON, FILE2DEL_COMMON, DIR2DEL, FILE2DEL]
}

EXCEPTION_LIST = (
                  AssertionError, AttributeError, IOError,
                  ImportError, IndentationError,
                  IndexError, KeyError, NameError,
                  SyntaxError, TypeError, ValueError
)


def delete_dirs(dirs):
    """
    delete_dirs
    :param dirs:
    :return:
    """
    for delete_dir in dirs:
        if path.islink(path.join(LIBCK_HOME, delete_dir.strip())):
            remove(path.join(LIBCK_HOME, delete_dir.strip()))
        elif path.exists(path.join(LIBCK_HOME, delete_dir.strip())):
            shutil.rmtree(path.join(LIBCK_HOME, delete_dir.strip()))


def delete_files(files):
    """
    delete_files
    :param files:
    :return:
    """
    for file in files:
        if path.islink(path.join(LIBCK_HOME, file.strip())):
            remove(path.join(LIBCK_HOME, file.strip()))
        elif path.exists(path.join(LIBCK_HOME, file.strip())):
            remove(path.join(LIBCK_HOME, file.strip()))
            if not listdir(path.dirname(
                    path.join(LIBCK_HOME, file.strip()))):
                rmdir(path.dirname(path.join(
                    LIBCK_HOME, file.strip())))


def cut_code(product):
    """
    cut_code
    :param product:
    :return:
    """
    delete_dirs(PRODUCTS.get(product)[0])
    delete_files(PRODUCTS.get(product)[1])
    delete_dirs(PRODUCTS.get(product)[2])
    delete_files(PRODUCTS.get(product)[3])
    skip_h = environ.get("HB_FULLBUILD_SKIP_8261", "")
    if skip_h == "ON":
        delete_dirs(DIR2DEL_H)
        delete_files(FILE2DEL_H)


def cut_other():
    """
    cut_other
    :return:
    """
    for folder_name, subfolders, filenames in walk(LIBCK_HOME):
        for folder in subfolders:
            if folder in ['.git', '__pycache__']:
                folder_path = path.join(folder_name, folder)
                shutil.rmtree(path.relpath(folder_path))
        for filename in filenames:
            if filename.startswith('.git') or filename == 'Kconfig':
                file_path = path.join(folder_name, filename)
                remove(path.relpath(file_path))


def rm_empty_folder(path_):
    """
    rm_empty_folder
    :param path_:
    :return:
    """
    for local_dir, _, _ in walk(path_):
        while True:
            if not listdir(local_dir):
                up_dir = path.dirname(local_dir)
                rmdir(local_dir)
                local_dir = up_dir
            else:
                break


if __name__ == "__main__":
    PRODUCT = ""
    if len(argv) == 1:
        try:
            PRODUCT = environ["HB_FULLBUILD_PRODUCT"]
        except EXCEPTION_LIST:
            logging.error("get cut plat env error!")
            sys.exit(1)
        finally:
            pass
    elif len(argv) == 2:
        PRODUCT = argv[1]
    if PRODUCT.lower() not in PRODUCTS:
        cut_code('else')
    else:
        cut_code(PRODUCT.lower())
    cut_other()
    remove(path.join(LIBCK_HOME, 'build', 'prepare', 'cut_file_l2.py'))
    remove(path.join(LIBCK_HOME, 'build', 'prepare', 'cut_file_emru.py'))
    remove(path.join(LIBCK_HOME, 'build', 'prepare', 'cut_file_h.py'))
    remove(path.join(LIBCK_HOME, 'build', 'prepare', 'cut_file.py'))
    remove(path.join(LIBCK_HOME, 'build', 'prepare', 'cut_code.py'))
    rm_empty_folder(LIBCK_HOME)
