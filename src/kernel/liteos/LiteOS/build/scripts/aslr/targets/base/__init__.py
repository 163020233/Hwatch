#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.

__all__ = ['ITargets', 'TargetsBase', 'RiscvTargets']

import importlib.util as imputil

from targets.base.base import ITargets, TargetsBase

if imputil.find_spec('targets.base.riscv'):
    from targets.base.riscv import RiscvTargets
