#!/usr/bin/env python3
# coding=utf-8
# Copyright © Huawei Technologies Co., Ltd. 2010-2020. All rights reserved.

'''prepare/build_clean.py'''

from os import path, getcwd
import sys
import subprocess

HOME_PATH = path.dirname(path.dirname(path.dirname(path.abspath(__file__))))


def run_cmd(cmd, cwd=getcwd()):
    """
    执行脚本入口
    :param cmd:
    :param cwd:
    :return:
    """
    exit_code, _ = subprocess.getstatusoutput('cd %s && %s' % (cwd, cmd))
    return exit_code


if __name__ == "__main__":
    RET = run_cmd("python build.py clean", cwd=HOME_PATH)
    sys.exit(RET)
