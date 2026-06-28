#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.

import logging
from typing import Optional
from targets.base import TargetsBase
from arch.base import IArchBase
from arch import LingLong
from arch.linglong import VLIW
from parse.parse import ParseElf


class AudiodspLL(TargetsBase, targets_name='audiodsp_linglong'):
    def __init__(self, parser: ParseElf, arch: IArchBase, safety: bool = False):
        def dump_image_info():
            logging.error('ddr_start=%x ddr_end=%x', ddr_start, ddr_end)
            logging.error('tcm_start=%x tcm_end=%x', tcm_start, tcm_end)
        if not isinstance(arch, LingLong):
            raise TypeError('arch not matched for linglong')
        super().__init__(parser, arch, safety)

        ddr_start = self._parser.get_one_symtab_value('__ddr_start')
        ddr_end = self._parser.get_one_symtab_value('__ddr_end')

        tcm_start = self._parser.get_one_symtab_value('__tcm_start')
        tcm_end = self._parser.get_one_symtab_value('__tcm_end')

        # check ddr should bigger than TCM/OCRAM, refers to [RESTRICTION 1]
        if ddr_end < ddr_start or tcm_end < tcm_start:
            dump_image_info()
            raise ValueError('invalid image range')
        if ddr_end > tcm_start:
            dump_image_info()
            raise ValueError('ddr larger than ocram/tcm is not allowed')

        self._ddr_region = range(ddr_start, ddr_end)

    @property
    def check_ldsize_symbol(self):
        return True

    def get_info(self, relinfo: dict) -> int:
        info = int(relinfo['info'], base=16) & 0xff
        vliw, slot = relinfo['mcode']
        if isinstance(vliw, VLIW):
            inst = vliw[slot]
            info |= inst.encoder.index << 8
            info |= (slot & 0x3) << 16
            info |= ((inst.ext_word.address - vliw.base) & 0x3f) << 18
        return info

    def _reloc(self, raw: dict) -> Optional[dict]:
        '''
        For this target, only DDR image loading with random offset, TCM holds in original position. OCRAM mapped to
        addresss within ddr, software does not know the difference.
        Therefore, following rules will apply for us:
        1. For absolute relocation: only it pointed to DDR should reloc.
        2. For relative relocation: if it pointed address region not same with the instruction located, do reloc.
            a. now the abs(reloc.value) only possible to decrease, due to TCM address is larger than DDR,
            therefore value bounds check is not necessary for this target. [RESTRICTION 1]
            b. consider TCM as same region due to these regions both need not to relocate.
        3. All other situation need not to reloc.
        '''
        raw = super()._reloc(raw)
        if not raw:
            return raw

        source_addr = int(raw['offset'], base=16)
        target_addr = raw['addr']
        source_in_ddr = source_addr in self._ddr_region
        target_in_ddr = target_addr in self._ddr_region
        if raw['reltype'] == 'abs':
            if target_in_ddr:
                return raw
        elif raw['reltype'] == 'rel':
            if source_in_ddr != target_in_ddr:
                return raw
        return None
