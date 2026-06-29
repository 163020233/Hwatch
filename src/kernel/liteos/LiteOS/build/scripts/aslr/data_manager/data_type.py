#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
'''
Data type define
'''

from collections import namedtuple
from enum import IntEnum


ElfHeaderStruct = namedtuple('ElfHeaderStruct', 'elfclass type machine address num endian')
ElfSectionStruct = namedtuple('ElfSectionStruct', 'index name type addr off size entsize link info align flags')
InstInfoStruct = namedtuple('InstInfoStruct', 'arch type is_multi reltype width')
RelInfoStruct = namedtuple('RelInfoStruct', 'mcode relocType shType position symAdd addend gotAddr thumbFunc')
SymbolInfoStruct = namedtuple('SymbolInfoStruct', 'num value size type buind Vis ndx name')


class ShType(IntEnum):
    '''
    Relocation type class
    '''
    # 4 LD_SHT_RELA
    RELA = 4
    # 9 LD_SHT_REL
    REL = 9
