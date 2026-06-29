#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.

__all__ = ['ArchBase', 'Arm', 'AArch64', 'Riscv', 'LingLong']

import importlib.util as imputil

from arch.base import ArchBase

if imputil.find_spec('arch.arm'):
    from arch.arm import Arm
if imputil.find_spec('arch.arm64'):
    from arch.arm64 import AArch64
if imputil.find_spec('arch.riscv'):
    from arch.riscv import Riscv
if imputil.find_spec('arch.linglong'):
    from arch.linglong import LingLong
