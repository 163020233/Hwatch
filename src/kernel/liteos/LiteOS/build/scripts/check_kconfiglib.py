#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.

"""Module check kconfiglib is install or not."""


def import_kconfiglib():
    """import kconfiglib and return result of execution."""

    try:
        import kconfiglib
        return 1
    except ImportError:
        return 0


def print_output(param):
    """print result of execution import_kconfiglib."""

    if param == 1:
        print(True)
    else:
        print(False)


if __name__ == '__main__':
    FLAG = import_kconfiglib()
    print_output(FLAG)
