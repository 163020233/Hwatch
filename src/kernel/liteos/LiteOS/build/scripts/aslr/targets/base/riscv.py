#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
'''
Riscv targets base class
'''

from arch.riscv import Riscv
from targets.base import TargetsBase


class RiscvTargets(TargetsBase, targets_name='riscv-base'):
    def __init__(self, parser, arch, safety):
        if not isinstance(arch, Riscv):
            raise TypeError('incompatible machine type')
        super().__init__(parser, arch, safety)
