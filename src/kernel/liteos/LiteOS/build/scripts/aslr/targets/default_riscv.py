#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
'''
Default Riscv Targets.
'''

from targets.base import RiscvTargets


class DefaultRiscvTarget(RiscvTargets, targets_name='default-riscv'):
    @property
    def check_ldsize_symbol(self):
        return False

    def get_info(self, relinfo: dict) -> int:
        return int(relinfo['info'], 16) & self._arch.relocinfo_mask(self._parser.elfclass)

    def _reloc_addr(self, raw):
        if raw['reltype'] == 'rel':
            if not self._safety:
                return None
        return super()._reloc_addr(raw)
