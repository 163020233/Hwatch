#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
'''
RISCV reloc implement
'''


import logging
from typing import Optional, Tuple
from arch.base import ArchBase
from arch.base import ISimpleInstArch
from data_manager.data_type import InstInfoStruct, RelInfoStruct
from data_manager.data_type import ShType
from util.utils import Utils
from parse.parse import ParseElf


_LOG_FORMAT = '[%(funcName)s:%(lineno)d, %(levelname)s]: %(message)s'
logging.basicConfig(level=logging.NOTSET, format=_LOG_FORMAT)


class Riscv(ArchBase, ISimpleInstArch, arch_name='RISC-V'):
    '''
    Class for riscv architecture
    '''
    _INSTRUCTION_DICT = {
        # c.j
        'R_RISCV_RVC_JUMP' : InstInfoStruct('RISC-V', 'func', 'single', 'rel', 2),
        # c.beq c.bne
        'R_RISCV_RVC_BRANCH' : InstInfoStruct('RISC-V', 'func', 'single', 'rel', 2),
        # jal j
        'R_RISCV_JAL' : InstInfoStruct('RISC-V', 'func', 'single', 'rel', 4),
        # beqz bnez bltu bgeu
        'R_RISCV_BRANCH' : InstInfoStruct('RISC-V', 'func', 'single', 'rel', 4),
        # bgei bgeui beqi
        'R_RISCV_BRANCHI' : InstInfoStruct('RISC-V', 'func', 'single', 'rel', 4),
        # auipc jalr
        'R_RISCV_CALL' : InstInfoStruct('RISC-V', 'func', 'multi', 'rel', 4),
        'R_RISCV_RELAX' : InstInfoStruct('RISC-V', 'None', 'single', 'noact', 0),
        'R_RISCV_NONE' : InstInfoStruct('RISC-V', 'None', 'single', 'noact', 0),
        # la data(addi rd, pc, (imm20 << 12))
        'R_RISCV_PCREL_HI20' : InstInfoStruct('RISC-V', 'data', 'multi', 'rel', 4),
        # la data (addi rd, rs, imm12)
        'R_RISCV_PCREL_LO12_I' : InstInfoStruct('RISC-V', 'data', 'multi', 'rel', 4),
        # sw data
        'R_RISCV_PCREL_LO12_S' : InstInfoStruct('RISC-V', 'data', 'multi', 'rel', 4),
        # la data (addi rd, gp, imm12)
        'R_RISCV_GPREL_I' : InstInfoStruct('RISC-V', 'data', 'single', 'rel', 4),
        'R_RISCV_GPREL_S' : InstInfoStruct('RISC-V', 'data', 'single', 'rel', 4),
        # lui data (load rd, (imm20 << 12))
        'R_RISCV_HI20' : InstInfoStruct('RISC-V', 'data', 'multi', 'abs', 4),
        # la data (addi rd, rs, imm12)
        'R_RISCV_LO12_I' : InstInfoStruct('RISC-V', 'data', 'multi', 'abs', 4),
        # sw data (imm12)
        'R_RISCV_LO12_S' : InstInfoStruct('RISC-V', 'data', 'multi', 'abs', 4),
        # data imm32
        'R_RISCV_32' : InstInfoStruct('RISC-V', 'data', 'single', 'abs', 4),
        # bne rs1, rs2, offset (if rs1 != rs2) pc += offset
        'R_RISCV_BEQ_X' : InstInfoStruct('RISC-V', 'func', 'single', 'rel', 4),
        # c.lui rd, imm (load rd, (imm[17:12]) << 12)
        'R_RISCV_RVC_LUI' : InstInfoStruct('RISC-V', 'data', 'multi', 'abs', 2),
        # BNEI rs1, cmpimm, if (rs1 != cmpimm) pc += offset
        'R_RISCV_BEQI_YK' : InstInfoStruct('RISC-V', 'func', 'single', 'rel', 4),
        # sw rs2, offset(gp)
        'R_RISCV_GPREL17_S' : InstInfoStruct('RISC-V', 'data', 'single', 'rel', 4),
        # lw rs2, offset(gp)
        'R_RISCV_GPREL17_I' : InstInfoStruct('RISC-V', 'data', 'single', 'rel', 4),
        # lui data (load rd, (imm20 << 12))
        'R_RISCV_GP_HI20' : InstInfoStruct('RISC-V', 'data', 'multi', 'abs', 4),
        # lw rs2, offset(rs1)
        'R_RISCV_GP_LO12_I' : InstInfoStruct('RISC-V', 'data', 'multi', 'abs', 4),
        #lw rs2, offset(pc)
        'R_RISCV_GOT_HI20' : InstInfoStruct('RISC-V', 'data', 'multi', 'rel', 4),
        # sw rs2, offset(rs1)
        'R_RISCV_GP_LO12_S' : InstInfoStruct('RISC-V', 'data', 'multi', 'abs', 4),
        # BLTUI rs1, cmpimm, offset
        'R_RISCV_BEQI_YJ' : InstInfoStruct('RISC-V', 'func', 'single', 'rel', 4),
        # auipc rd, imm
        'R_RISCV_CALL_PLT' : InstInfoStruct('RISC-V', 'func', 'single', 'rel', 4),
        # bgtu rs1, rs2, offset
        'R_RISCV_BEQ_GB' : InstInfoStruct('RISC-V', 'func', 'single', 'rel', 4),
        # beqi rs1,imm, offset
        'R_RISCV_BEQI_L' : InstInfoStruct('RISC-V', 'func', 'single', 'rel', 4),
        # same as BEQ_GB
        'R_RISCV_BEQ_GD' : InstInfoStruct('RISC-V', 'func', 'single', 'rel', 4),

        'R_RISCV_ADD8' : InstInfoStruct('RISC-V', 'data', 'single', 'rel', 1),
        'R_RISCV_SUB8' : InstInfoStruct('RISC-V', 'data', 'single', 'rel', 1),
        'R_RISCV_ADD16' : InstInfoStruct('RISC-V', 'data', 'single', 'rel', 2),
        'R_RISCV_SUB16' : InstInfoStruct('RISC-V', 'data', 'single', 'rel', 2),
        'R_RISCV_ADD32' : InstInfoStruct('RISC-V', 'data', 'single', 'rel', 4),
        'R_RISCV_SUB32' : InstInfoStruct('RISC-V', 'data', 'single', 'rel', 4),
    }
    # record hi20 to assist lo12 relocation
    rel_hi20_sym_val = 0
    rel_hi20_loc = 0
    got_reloc = False
    # got relocation has indirect address
    rel_hi20_got_val = 0

    def __init__(self):
        super().__init__()
        self.reloc_dict = dict()

    @staticmethod
    def get_inst_width(inst):
        '''
        Get the instruction width
        '''
        item = Riscv._INSTRUCTION_DICT.get(inst, None)
        if item:
            return item.width
        else:
            return None

    @staticmethod
    def check_random_overflow(item, max_random_value):
        '''
        check max possible randomized value fits this instruction
        '''
        offset = int(item['value'], 16) + item['addend'] - int(item['offset'], 16)
        r_riscv_jal = 17
        r_riscv_j = 45
        # jal has limited access range: 2MB
        if r_riscv_jal == (int(item['info'], 16) & 0xff) and abs(offset) + max_random_value > 0x100000:
            Utils.err_exit('{} offset overflow random value'.format(item['offset']))
        # c.jal has limited access range: 4KB
        if r_riscv_j == (int(item['info'], 16) & 0xff) and abs(offset) + max_random_value > 0x800:
            Utils.err_exit('{} offset overflow random value'.format(item['offset']))
        return True

    @staticmethod
    def _r_riscv_32(info):
        addr = info.mcode - info.addend
        return True, addr

    @staticmethod
    def _r_riscv_hi20(info):
        # lui pcrel_hi|hi(symbol) (U-Type):
        # |31                               12|11                  |6      |0
        # |imm[31:12]                         |rd                  |opcode
        # *position[31:12] = imm20 << 12
        ret = True
        addr = int(info.symAdd, 16)
        addend = info.addend
        mcode = info.mcode
        addr_in_mcode = mcode & 0xfffff000
        imm20 = ((addr + addend) + 0x800) & 0xfffff000  # U_TYPE_OFFSET 0x800
        if addr_in_mcode != imm20:
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_riscv_lo12_s(info):
        # sw pcrel_lo|lo(pcrel_hi20_address) (S-Type):
        # |31     25|24      |19       |14     |11      7|6      |0
        # imm[11:5] |rs2     |rs1      |funct3 |imm[4:0] |opcode
        # *position[31:25 11:7] = imm7[11:5] imm5[4:0]
        ret = True
        addr = int(info.symAdd, 16)
        addend = info.addend
        mcode = info.mcode
        addr_in_mcode = (((mcode & 0xfe000000) >> 25) << 5) | ((mcode & 0x00000f80) >> 7)
        # 0x800: U_TYPE_OFFSET
        imm12 = -((((addr + addend) + 0x800) & 0xfffff000) - (addr + addend))
        imm12 &= 0x00000fff
        if addr_in_mcode != imm12:
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_riscv_lo12_i(info):
        # addi lw pcrel_lo|lo(pcrel_hi20_address) (I-Type):
        # |31                        20|19    |14     |11      |6      |0
        # |imm[11:0]                   |rs1   |funct3 |rd      |opcode
        # *position[11:0] = imm12
        ret = True
        addr = int(info.symAdd, 16)
        addend = info.addend
        mcode = info.mcode
        addr_in_mcode = (mcode & 0xfff00000) >> 20
        # 0x800: U_TYPE_OFFSET
        imm12 = -((((addr + addend) + 0x800) & 0xfffff000) - (addr + addend))
        imm12 &= 0x00000fff
        if addr_in_mcode != imm12:
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_riscv_rvc_lui(info):
        # c.lui rd, imm
        # 15 13 | 12       | 11 7 | 6 2        | 1 0
        # 011   | imm[17]  | rd   | imm[16:12] | 01
        ret = True
        addr = int(info.symAdd, 16)
        addend = info.addend
        mcode = info.mcode
        addr_in_mcode = (mcode & 0x7C) << 10
        singedflag = mcode & 0x1000
        # cpro gspr range 0-4096
        maxloadaddr = addr + 4096
        # 0x800: U_TYPE_OFFSET
        imm17 = (addr + addend + 0x800) & 0x3F000
        # c.lui code range is 0~128k, ensure that the offset address is also in this range.
        if ((addr_in_mcode | singedflag) != imm17 or (singedflag == 0 and maxloadaddr >= 0x1F800)):
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_riscv_gp_hi20(info):
        # lui pcrel_hi|hi(symbol) (U-Type):
        # |31                               12|11                  |6      |0
        # |imm[31:12]                         |rd                  |opcode
        # *position[31:12] = imm20 << 12
        ret = True
        addr = int(info.symAdd, 16)
        addend = info.addend
        mcode = info.mcode
        addr_in_mcode = mcode & 0xfffff000
        # determine whether is lui(0x37) code
        if mcode & 0x7F != 0x37:
            logging.warning("%s in rel table code type is wrong", info.position)
            return False, None

        imm20 = ((addr + addend) + 0x800) & 0xfffff000 # U_TYPE_OFFSET 0x800
        if addr_in_mcode != imm20:
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_riscv_gp_lo12_i(info):
        # lbu lhu addi lw pcrel_lo|lo(pcrel_hi20_address) (I-Type):
        # |31                        20|19    |14     |11      |6      |0
        # |imm[11:0]                   |rs1   |funct3 |rd      |opcode
        # *position[11:0] = imm12
        ret = True
        addr = int(info.symAdd, 16)
        addend = info.addend
        mcode = info.mcode
        addr_in_mcode = (mcode & 0xfff00000) >> 20
        # determine whether is lw(0x2003)/lbu(0x4003)/lhu(0x5003) code
        if (mcode & 0x707F != 0x2003 and mcode & 0x707F != 0x4003 and mcode & 0x707F != 0x5003):
            logging.warning("%s in rel table code type is wrong", info.position)
            return False, None

        # 0x800: U_TYPE_OFFSET
        imm12 = -((((addr + addend) + 0x800) & 0xfffff000) - (addr + addend))
        imm12 &= 0x00000fff
        if addr_in_mcode != imm12:
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_riscv_gp_lo12_s(info):
        # sw  rs2, offset(rs1)
        # | 31    25   | 24 20 | 19 15 | 14 12 | 11       7 | 6    0 |
        # offset[11:5] |  rs2  | rs1   |  010  | offset[4:0]| 0100011|
        ret = True
        addr = int(info.symAdd, 16)
        addend = info.addend
        mcode = info.mcode
        addr_in_mcode = ((mcode & 0xFE000000) >> 20) | ((mcode & 0xF80) >> 7)

        # 0x800: U_TYPE_OFFSET
        imm12 = -((((addr + addend) + 0x800) & 0xfffff000) - (addr + addend))
        imm12 &= 0x00000fff
        if addr_in_mcode != imm12:
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_riscv_gprel(info):
        # addi rd, irs1, imm r_riscv_gprel_i
        # 31 20       | 19       15| 14 12 | 11         7| 6    0
        # imm[11:0]   | rs1        | 000   | rd          | 0010011
        # lw rd, rs1, imm r_riscv_gprel_i
        # 31 20       | 19       15| 14 12 | 11         7| 6    0
        # imm[11:0]   | rs1        | 000   | rd          | 0000011
        # sw rd, offset(rs1) r_riscv_gprel_s
        # 31 25       | 24       20| 19 15 | 14   12| 11       7| 6   0
        # off[11:5]   | rs2        | rs1   | 010    |  off[0:4] |0100011
        # sb rd, offset(rs1) r_riscv_gprel_s
        # 31 25       | 24       20| 19 15 | 14   12| 11       7| 6   0
        # off[11:5]   | rs2        | rs1   | 000    |  off[0:4] |0100011
        ret = True
        mcode = info.mcode

        # check rs1 == gp(x3)
        if mcode & 0xF8000 != 0x18000:
            logging.warning("%s is not gp rel", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_riscv_jal(info):
        # jal ra, imm
        # |31      |30       21|20       |19       12|11     |6      |0
        # |imm[20] |imm[10:1]  |imm[11]  |imm[19:12] |rd     |opcode
        ret = True
        imm20 = 0
        addr = int(info.symAdd, 16)
        addend = info.addend
        mcode = info.mcode
        position = int(info.position, 16)
        addr_in_mcode = Riscv._r_riscv_set_bit(mcode, 12, 20)
        offset = addr + addend - position

        imm20 |= Riscv._r_riscv_move_bit(offset, 12, 12, 8)
        imm20 |= Riscv._r_riscv_move_bit(offset, 11, 20, 1)
        imm20 |= Riscv._r_riscv_move_bit(offset, 1, 21, 10)
        imm20 |= Riscv._r_riscv_move_bit(offset, 20, 31, 1)

        if imm20 != addr_in_mcode:
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_riscv_pcrel_hi20(info):
        # auipc rs1, imm
        # |31                               12|11                  |6      |0
        # |imm[31:12]                         |rd                  |opcode
        ret = True
        imm20 = 0
        addr = int(info.symAdd, 16)
        addend = info.addend
        mcode = info.mcode
        position = int(info.position, 16)
        addr_in_mcode = Riscv._r_riscv_set_bit(mcode, 12, 20)
        offset = addr + addend - position + 0x800
        imm20 = offset & 0xfffff000
        Riscv.rel_hi20_sym_val = addr + addend
        Riscv.rel_hi20_loc = position
        if imm20 != addr_in_mcode:
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        Riscv.got_reloc = False
        return ret, None

    @staticmethod
    def _r_riscv_got_hi20(info):
        # auipc rs1, imm
        # |31                               12|11                  |6      |0
        # |imm[31:12]                         |rd                  |opcode
        mcode = info.mcode
        position = int(info.position, 16)
        addr_in_mcode = Riscv._r_riscv_set_bit(mcode, 12, 20)
        Riscv.rel_hi20_sym_val = addr_in_mcode + position
        Riscv.rel_hi20_loc = position
        Riscv.rel_hi20_got_val = int(info.symAdd, 16)
        Riscv.got_reloc = True
        return True, None

    @staticmethod
    def _r_riscv_jump(info):
        # j imm
        # |15     13|    12    |    11    |10       9|    8    |    7    |    6    |5        3|   2   |1   0
        # |101      | imm[11]  | imm[4]   | imm[9:8] | imm[10] | imm[6]  | imm[7]  | imm[3:1] | imm[5]|01
        ret = True
        imm = 0
        addr = int(info.symAdd, 16)
        addend = info.addend
        mcode = info.mcode
        position = int(info.position, 16)
        addr_in_mcode = Riscv._r_riscv_set_bit(mcode, 2, 11)
        offset = addr + addend - position
        imm |= Riscv._r_riscv_move_bit(offset, 11, 12, 1)
        imm |= Riscv._r_riscv_move_bit(offset, 4, 11, 1)
        imm |= Riscv._r_riscv_move_bit(offset, 8, 9, 2)
        imm |= Riscv._r_riscv_move_bit(offset, 10, 8, 1)
        imm |= Riscv._r_riscv_move_bit(offset, 6, 7, 1)
        imm |= Riscv._r_riscv_move_bit(offset, 7, 6, 1)
        imm |= Riscv._r_riscv_move_bit(offset, 1, 3, 3)
        imm |= Riscv._r_riscv_move_bit(offset, 5, 2, 1)

        if imm != addr_in_mcode:
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_riscv_branch(info):
        # SB-type:
        # |31      |30      25|24    |19   |14      |11       8|7       |6      |0
        # |imm[12] |imm[10:5] |rs2   |rs1  |funct3  |imm[4:1]  |imm[11] |opcode
        ret = True
        imm = 0
        addr = int(info.symAdd, 16)
        addend = info.addend
        mcode = info.mcode
        position = int(info.position, 16)

        addr_in_mcode = Riscv._r_riscv_set_bit(mcode, 7, 1)
        addr_in_mcode |= Riscv._r_riscv_set_bit(mcode, 8, 4)
        addr_in_mcode |= Riscv._r_riscv_set_bit(mcode, 25, 6)
        addr_in_mcode |= Riscv._r_riscv_set_bit(mcode, 31, 1)
        offset = addr + addend - position

        imm |= Riscv._r_riscv_move_bit(offset, 12, 31, 1)
        imm |= Riscv._r_riscv_move_bit(offset, 5, 25, 6)
        imm |= Riscv._r_riscv_move_bit(offset, 1, 8, 4)
        imm |= Riscv._r_riscv_move_bit(offset, 11, 7, 1)

        if imm != addr_in_mcode:
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_riscv_rvc_branch(info):
        ret = True
        imm = 0
        addr = int(info.symAdd, 16)
        addend = info.addend
        mcode = info.mcode
        position = int(info.position, 16)

        addr_in_mcode = Riscv._r_riscv_set_bit(mcode, 12, 1)
        addr_in_mcode |= Riscv._r_riscv_set_bit(mcode, 5, 2)
        addr_in_mcode |= Riscv._r_riscv_set_bit(mcode, 2, 1)
        addr_in_mcode |= Riscv._r_riscv_set_bit(mcode, 10, 2)
        addr_in_mcode |= Riscv._r_riscv_set_bit(mcode, 3, 2)
        offset = addr + addend - position

        imm |= Riscv._r_riscv_move_bit(offset, 8, 12, 1)
        imm |= Riscv._r_riscv_move_bit(offset, 6, 5, 2)
        imm |= Riscv._r_riscv_move_bit(offset, 5, 2, 1)
        imm |= Riscv._r_riscv_move_bit(offset, 3, 10, 2)
        imm |= Riscv._r_riscv_move_bit(offset, 1, 3, 2)

        if imm != addr_in_mcode:
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_riscv_gprel17(info):
        ret = True
        mcode = info.mcode
        if mcode & 0x7F != 0x7B:
            logging.warning("%s is not gp17 rel", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_riscv_call(info):
        ret = True
        addr = int(info.symAdd, 16)
        addend = info.addend
        mcode = info.mcode
        position = int(info.position, 16)
        addr_in_mcode = mcode & 0xfffff000
        imm20 = ((addr + addend - position) + 0x800) & 0xfffff000  # U_TYPE_OFFSET 0x800
        if imm20 != addr_in_mcode:
            logging.warning("%s in rel table info is wrong", info.position)
            ret = False
        return ret, None

    @staticmethod
    def _r_riscv_move_bit(offset, bit_from, bit_to, width):
        if bit_from > bit_to:
            return (offset >> (bit_from - bit_to)) & ((pow(2, width) - 1) << bit_to)
        return (offset << (bit_to - bit_from)) & ((pow(2, width) - 1) << bit_to)

    @staticmethod
    def _r_riscv_set_bit(mcode, bit_start, width):
        return mcode & ((pow(2, width) - 1) << bit_start)

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
        parser = kwargs.get('parser')
        if not parser or not isinstance(parser, ParseElf):
            raise ValueError('invalid parser argument')
        item = self._INSTRUCTION_DICT.get(info.relocType, None)
        if not item:
            Utils.err_exit('not support relocType {}'.format(info.relocType))
        if info.shType == ShType.REL:
            Utils.err_exit('RISCV not support REL now')
        elif item.reltype == 'abs':
            ret, addr = self._abs_process(info)
        elif item.reltype == 'rel':
            ret, addr = self._rel_process(parser, info)
        elif item.reltype == 'noact':
            ret, addr = True, None
        else:
            ret, addr = False, None

        if ret and addr == 0:
            addr = None
        return ret, addr

    def _r_riscv_pcrel_lo12_i(self, parser, info):
        # addi rs1, imm
        # |31                        20|19    |14     |11      |6      |0
        # |imm[11:0]                   |rs1   |funct3 |rd      |opcode

        ret = True
        imm12 = 0
        got_position = 0
        got_value = 0
        hi20_addr = int(info.symAdd, 16)
        if hi20_addr != Riscv.rel_hi20_loc:
            logging.warning("%s in rel table info is wrong", info.position)
            return False, None
        addr = Riscv.rel_hi20_sym_val
        mcode = info.mcode
        position = Riscv.rel_hi20_loc
        addr_in_mcode = Riscv._r_riscv_set_bit(mcode, 20, 12)
        offset = addr - position
        imm12 = offset - ((offset + 0x800) & 0xfffff000)
        imm12 = imm12 << 20
        imm12 &= 0xfff00000
        if not Riscv.got_reloc:
            if imm12 != addr_in_mcode:
                logging.warning("%s in rel table info is wrong", info.position)
                ret = False
        else:
            addr_in_mcode = (addr_in_mcode >> 20) & 0xfff
            if 0x800 & addr_in_mcode:
                got_position = Riscv.rel_hi20_sym_val + (addr_in_mcode - 0x1000)
            else:
                got_position = Riscv.rel_hi20_sym_val + addr_in_mcode
            got_bytes = parser.get_data(got_position, 4)
            got_value = int.from_bytes(got_bytes, byteorder=parser.endian, signed=False)
            if got_value != Riscv.rel_hi20_got_val:
                logging.warning("%s in rel table info is wrong", info.position)
                ret = False
            self.reloc_dict[Riscv.rel_hi20_loc] = got_position
        return ret, None

    def _r_riscv_pcrel_lo12_s(self, parser, info):
        # |31     25|24      |19       |14     |11      7|6      |0
        # imm[11:5] |rs2     |rs1      |funct3 |imm[4:0] |opcode
        ret = True
        imm12 = 0
        got_position = 0
        got_value = 0
        imm = 0
        hi20_addr = int(info.symAdd, 16)
        if hi20_addr != Riscv.rel_hi20_loc:
            logging.warning("%s in rel table info is wrong", info.position)
            return False, None
        addr = Riscv.rel_hi20_sym_val
        mcode = info.mcode
        position = Riscv.rel_hi20_loc
        addr_in_mcode = Riscv._r_riscv_set_bit(mcode, 7, 5)
        addr_in_mcode |= Riscv._r_riscv_set_bit(mcode, 25, 7)
        offset = addr - position
        imm12 = offset - ((offset + 0x800) & 0xfffff000)
        imm |= Riscv._r_riscv_move_bit(imm12, 0, 7, 5)
        imm |= Riscv._r_riscv_move_bit(imm12, 5, 25, 7)
        if not Riscv.got_reloc:
            if imm != addr_in_mcode:
                logging.warning("%s in rel table info is wrong", info.position)
                ret = False
        else:
            addr_in_mcode = ((addr_in_mcode >> 7) | (addr_in_mcode >> 20)) & 0xfff
            if 0x800 & addr_in_mcode:
                got_position = Riscv.rel_hi20_sym_val + (addr_in_mcode - 0x1000)
            else:
                got_position = Riscv.rel_hi20_sym_val + addr_in_mcode
            got_bytes = parser.get_data(got_position, 4)
            got_value = int.from_bytes(got_bytes, byteorder=parser.endian, signed=False)
            if got_value != Riscv.rel_hi20_got_val:
                logging.warning("%s in rel table info is wrong", info.position)
                ret = False
            self.reloc_dict[Riscv.rel_hi20_loc] = got_position
        return ret, None

    def _abs_process(self, info):
        if info.relocType == 'R_RISCV_32':
            return self._r_riscv_32(info)
        elif info.relocType == 'R_RISCV_HI20':
            return self._r_riscv_hi20(info)
        elif info.relocType == 'R_RISCV_LO12_S':
            return self._r_riscv_lo12_s(info)
        elif info.relocType == 'R_RISCV_GP_LO12_S':
            return self._r_riscv_gp_lo12_s(info)
        elif info.relocType == 'R_RISCV_LO12_I':
            return self._r_riscv_lo12_i(info)
        elif info.relocType == 'R_RISCV_GP_HI20':
            return self._r_riscv_gp_hi20(info)
        elif info.relocType == 'R_RISCV_GP_LO12_I':
            return self._r_riscv_gp_lo12_i(info)
        elif info.relocType == 'R_RISCV_RVC_LUI':
            return self._r_riscv_rvc_lui(info)
        else:
            Utils.err_exit('not support abs relocType {}'.format(info.relocType))

    # Prevent GP relative access from being relaxed(Relative access becomes absolute access).
    # For example
    # +-------------------+                      +-------------------+
    # |auipc   a0, 0      |     relax to         |                   |
    # |addi   a0, a0, 616 |     ---------->      | li t0, 616        |
    # +-------------------+                      +-------------------+
    def _rel_process(self, parser, info):
        if info.relocType in ('R_RISCV_GPREL_I', 'R_RISCV_GPREL_S'):
            ret, addr = self._r_riscv_gprel(info)
        elif info.relocType == 'R_RISCV_JAL':
            ret, addr = self._r_riscv_jal(info)
        elif info.relocType == 'R_RISCV_PCREL_HI20':
            ret, addr = self._r_riscv_pcrel_hi20(info)
        elif info.relocType == 'R_RISCV_GOT_HI20':
            ret, addr = self._r_riscv_got_hi20(info)
        elif info.relocType == 'R_RISCV_PCREL_LO12_I':
            ret, addr = self._r_riscv_pcrel_lo12_i(parser, info)
        elif info.relocType == 'R_RISCV_PCREL_LO12_S':
            ret, addr = self._r_riscv_pcrel_lo12_s(parser, info)
        elif info.relocType == 'R_RISCV_RVC_JUMP':
            ret, addr = self._r_riscv_jump(info)
        elif info.relocType == 'R_RISCV_BRANCH':
            ret, addr = self._r_riscv_branch(info)
        elif info.relocType == 'R_RISCV_RVC_BRANCH':
            ret, addr = self._r_riscv_rvc_branch(info)
        elif info.relocType in ('R_RISCV_GPREL17_S', 'R_RISCV_GPREL17_I'):
            ret, addr = self._r_riscv_gprel17(info)
        elif info.relocType in ('R_RISCV_CALL', 'R_RISCV_CALL_PLT'):
            ret, addr = self._r_riscv_call(info)
        elif info.relocType in ('R_RISCV_BEQI_L', 'R_RISCV_BEQ_GB', 'R_RISCV_SUB8', 'R_RISCV_ADD8', \
            'R_RISCV_SUB16', 'R_RISCV_ADD16', 'R_RISCV_SUB32', 'R_RISCV_ADD32', 'R_RISCV_BEQ_GD'):
            return True, None
        else:
            return False, None
        return ret, addr
