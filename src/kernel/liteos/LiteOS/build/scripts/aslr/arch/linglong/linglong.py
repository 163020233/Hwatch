#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
'''
LingLong reloc implement
'''

import logging
from typing import Optional, Tuple, Union
from arch.base import ArchBase
from arch.linglong.inst import VLIW
from parse.parse import ParseElf
from data_manager.data_type import InstInfoStruct, RelInfoStruct


_LOG_FORMAT = '[%(funcName)s:%(lineno)d, %(levelname)s]: %(message)s'
logging.basicConfig(level=logging.NOTSET, format=_LOG_FORMAT)


class LingLong(ArchBase, arch_name='LingLong'):
    '''
    Class for LingLong architecture
    '''

    _INSTRUCTION_DICT = {
        'R_LINGLONG_NONE'        : InstInfoStruct('LingLong', 'None', 'single', 'noact', None),
        'R_LINGLONG_32'          : InstInfoStruct('LingLong', 'data', 'single', 'abs', None),
        'R_LINGLONG_ABS32_SLOT0' : InstInfoStruct('LingLong', 'func', 'single', 'abs', None),
        'R_LINGLONG_ABS32_SLOT1' : InstInfoStruct('LingLong', 'func', 'single', 'abs', None),
        'R_LINGLONG_ABS32_SLOT2' : InstInfoStruct('LingLong', 'func', 'single', 'abs', None),
        'R_LINGLONG_ABS32_SLOT3' : InstInfoStruct('LingLong', 'func', 'single', 'abs', None),
        'R_LINGLONG_PCREL'       : InstInfoStruct('LingLong', 'func', 'single', 'rel', None),
        'R_LINGLONG_LOOP_START'  : InstInfoStruct('LingLong', 'func', 'single', 'rel', None),
        'R_LINGLONG_LOOP_END'    : InstInfoStruct('LingLong', 'func', 'single', 'rel', None),
    }

    _SLOT_TBL = {
        'R_LINGLONG_ABS32_SLOT0' : 0,
        'R_LINGLONG_ABS32_SLOT1' : 1,
        'R_LINGLONG_ABS32_SLOT2' : 2,
        'R_LINGLONG_ABS32_SLOT3' : 3,
        'R_LINGLONG_PCREL' : 0,
        'R_LINGLONG_LOOP_START' : 0,
        'R_LINGLONG_LOOP_END' : 0,
    }

    @classmethod
    def get_reltype(cls, inst_type: str) -> Optional[str]:
        '''
        Get the relocation type
        '''
        item = cls._INSTRUCTION_DICT.get(inst_type, None)
        if item:
            return item.reltype
        return None

    def reloc(self, info: RelInfoStruct, **kwargs) -> Tuple[bool, Optional[int]]:
        '''
        Fix relocation address.
        For linglong, compiler may not generate correct address for us, thus the address must come from binary.
        1. relative-func: mcode.imm contains relative address, then use: mcode.imm + relinfo.position
        2. absolute-func: mcode.imm contains absolute address, then use: mcode.imm
        3. absolute-data: mcode is a 32bit value contains absolute address, then use mcode
        '''
        try:
            reloc_type_info = self._INSTRUCTION_DICT[info.relocType]
        except KeyError as e:
            raise ValueError() from e
        if reloc_type_info.type == 'data':
            return True, info.mcode[0]
        if reloc_type_info.type == 'func':
            vliw, slot = info.mcode
            if reloc_type_info.reltype == 'rel':
                # compiler may generate address larger than 32bit, so the final address can rewind to zero.
                return True, (int(info.position, base=16) + vliw[slot].imm) & 0xFFFFFFFF
            if reloc_type_info.reltype == 'abs':
                return True, vliw[slot].imm
        raise ValueError()

    def get_mcode(self, parser: ParseElf, info: dict) -> Union[Tuple[VLIW, int], Tuple[int, None]]:
        '''
        get machine code by relinfo
        '''
        try:
            reloc_type_info = self._INSTRUCTION_DICT[info['type']]
            if reloc_type_info.type == 'data':
                code = parser.get_data(int(info['offset'], base=16), 4)
                return int.from_bytes(code, byteorder='little'), None
            if reloc_type_info.type == 'func':
                slot = self._SLOT_TBL[info['type']]
                vliw = VLIW.create_from_va(int(info['offset'], base=16), parser)
                return vliw, slot  # we'll using it later, return vliw instead of int
            raise ValueError()
        except KeyError as e:
            raise ValueError() from e
