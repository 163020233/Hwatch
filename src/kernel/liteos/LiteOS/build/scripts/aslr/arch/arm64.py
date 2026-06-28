#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
'''
ARM reloc implement
'''

from typing import Optional, Tuple
from arch.base import ArchBase
from data_manager.data_type import RelInfoStruct


class AArch64(ArchBase, arch_name='AArch64'):
    '''
    Class for arm architecture
    '''

    @classmethod
    def get_reltype(cls, inst_type: str) -> Optional[str]:
        raise NotImplementedError()

    def reloc(self, info: RelInfoStruct, **kwargs) -> Tuple[bool, Optional[int]]:
        raise NotImplementedError()
