#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
'''
Default target
'''

from targets.base import TargetsBase


class DefaultTarget(TargetsBase, targets_name='default'):
    @property
    def check_ldsize_symbol(self):
        return False

    def get_info(self, relinfo: dict) -> int:
        return int(relinfo['info'], 16) & self._arch.relocinfo_mask(self._parser.elfclass)
