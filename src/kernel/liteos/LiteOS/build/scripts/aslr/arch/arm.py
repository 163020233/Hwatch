#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
'''
ARM reloc implement
'''

import logging
from typing import Optional, Tuple
from arch.base import ArchBase, ISimpleInstArch
from data_manager.data_type import InstInfoStruct, RelInfoStruct
from data_manager.data_type import ShType
from util.utils import Utils


_LOG_FORMAT = '[%(funcName)s:%(lineno)d, %(levelname)s]: %(message)s'
logging.basicConfig(level=logging.NOTSET, format=_LOG_FORMAT)


class Arm(ArchBase, ISimpleInstArch, arch_name='ARM'):
    '''
    Class for arm architecture
    '''
    _INSTRUCTION_DICT = {
        'R_ARM_ABS32' :  InstInfoStruct('ARM', 'data', 'single', 'abs', 4),
        'R_ARM_MOVW_ABS_NC' : InstInfoStruct('ARM', 'data', 'multi', 'abs', 4),
        'R_ARM_MOVT_ABS' : InstInfoStruct('ARM', 'data', 'multi', 'abs', 4),
        'R_ARM_CALL' : InstInfoStruct('ARM', 'func', 'single', 'rel', 4),
        'R_ARM_JUMP24' : InstInfoStruct('ARM', 'func', 'single', 'rel', 4),
        'R_ARM_V4BX' : InstInfoStruct('ARM', 'func', 'single', 'noact', 4),
        'R_ARM_NONE' : InstInfoStruct('ARM', 'None', 'single', 'noact', 0),
    }

    @staticmethod
    def get_inst_width(inst):
        '''
        Get the instruction width
        '''
        item = Arm._INSTRUCTION_DICT.get(inst, None)
        if item:
            return item.width
        else:
            return None

    @staticmethod
    def _r_arm_abs32(info):
        addr = info.mcode
        return True, addr

    @staticmethod
    def _r_arm_movt_abs(info):
        # MOVT in 'ARM Architecture Reference Manual ARMv7-A and ARMv7-R edition'
        # |31    28|27       24|23       20|19        16|15     12|11        0|
        # |cond    |0b0011     |0b0100     |imm4[15:12] |Rd       |imm12[11:0]|
        # *position[19:16 11:0] = imm4:imm12
        ret = True
        addr = int(info.symAdd, 16)
        mcode = info.mcode
        addr_in_mcode = ((mcode & 0x000f0000) >> 4) | (mcode & 0x00000fff)
        if (addr & 0xffff0000) != (addr_in_mcode << 16):
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_arm_movw_abs_nc(info):
        # MOVW in 'ARM Architecture Reference Manual ARMv7-A and ARMv7-R edition'
        # |31    28|27       24|23       20|19       16 |15     12|11        0|
        # |cond    |0b0011     |0b0000     |imm4[15:12] |Rd       |imm12[11:0]|
        # *position = ZeroExtend(imm4:imm12, 32)
        ret = True
        addr = int(info.symAdd, 16)
        mcode = info.mcode
        addr_in_mcode = ((mcode & 0x000f0000) >> 4) | (mcode & 0x00000fff)
        if (addr & 0x0000ffff) != addr_in_mcode:
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _abs_process(info):
        if info.relocType == 'R_ARM_ABS32':
            return Arm._r_arm_abs32(info)
        elif info.relocType == 'R_ARM_MOVT_ABS':
            return Arm._r_arm_movt_abs(info)
        elif info.relocType == 'R_ARM_MOVW_ABS_NC':
            return Arm._r_arm_movw_abs_nc(info)
        else:
            Utils.err_exit('not support abs relocType {}'.format(info.relocType))

    @classmethod
    def get_reltype(cls, inst_type: str) -> Optional[str]:
        '''
        Get the relocation type
        '''
        item = cls._INSTRUCTION_DICT.get(inst_type, None)
        if item:
            return item.reltype
        else:
            return None

    def reloc(self, info: RelInfoStruct, **kwargs) -> Tuple[bool, Optional[int]]:
        '''
        Do relocation
        '''
        item = self._INSTRUCTION_DICT.get(info.relocType, None)
        if not item:
            Utils.err_exit('not support relocType {}'.format(info.relocType))
        if info.shType == ShType.RELA:
            Utils.err_exit('ARM not support RELA now')
        if item.reltype == 'abs': # only process abs now for GSPR
            return self._abs_process(info)
        elif item.reltype == 'rel':
            return True, None
        elif item.reltype == 'noact':
            return True, None
        else:
            return None, None
