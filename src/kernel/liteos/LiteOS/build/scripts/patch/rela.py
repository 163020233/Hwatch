#!/usr/bin/env python3
# coding=utf-8
# Copyright © Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
# Description: ARMv8 relocation.
# Create: 2024-09-25

"""tools/rela.py"""


import logging

MASK_32_BITS = 0xffffffff


def reloc_check_symval_signed(left, right, value):
    """
    reloc_check_symval_signed
    对重定位symVal有效性校验
    """
    if value >= -2 ** left and value < 2 ** right:
        return True

    logging.error("[error]reloc_check_symval_signed error! left :%s right: %s value is: %s!", left, right, hex(value))

    return False


def reloc_check_symval_unsigned(right, value):
    """
    reloc_check_symval_unsigned
    对重定位symVal有效性校验
    """
    if value >= 0 and value < 2 ** right:
        return True

    logging.error("[error]reloc_check_symval_unsigned error! right: %s value is: %s!", right, hex(value))

    return False


def reloc_move_imm(pre_addr, imm):
    """
    reloc_move_imm
    mov指令重定位立即数替换
    """
    replace_bytes = 4
    local_pre_addr = pre_addr & MASK_32_BITS
    value = local_pre_addr
    value &= 0xffe0001f
    value |= ((imm & 0xffff) << 5)

    return value, replace_bytes


def reloc_ld_st_imm(pre_addr, imm):
    """
    reloc_ld_st_imm
    LD or ST指令重定位立即数替换
    """
    replace_bytes = 4
    local_pre_addr = pre_addr & MASK_32_BITS
    value = local_pre_addr
    value &= 0xffc003ff
    value |= ((imm & 0xfff) << 10)

    return value, replace_bytes


def reloc_movw_inst(pre_addr, symvalue, imm):
    """
    reloc_movw_inst
    Movw重定位指令码基于立即数正负性变更逻辑
    """
    local_pre_addr = pre_addr & MASK_32_BITS
    value = local_pre_addr
    value &= (~(3 << 29))
    if not symvalue & (1 << 63):
        value |= (2 << 29)
        return value, imm

    return value, ~imm


def reloc_call_or_b(symvalue, raddend, rela_addr, pre_addr):
    """
    reloc_call_or_b
    立即数替换指令 call 或者 b 类型
    """
    value = symvalue + raddend - rela_addr
    replace_bytes = 4
    # 须加-mlong-calls编译选项，强制使用长跳转
    ret = reloc_check_symval_signed(27, 27, value)
    if not ret:
        logging.error("[error]reloc_call_or_b error! symvalue:[%#x] rela_addr:[%#x] value:[%#x]", \
                       symvalue, rela_addr, value)
        return False, ret, replace_bytes
    local_pre_addr = pre_addr & MASK_32_BITS
    imm = (value & 0xffffffc) >> 2
    value = local_pre_addr
    value &= 0xfc000000
    value |= imm

    return True, value, replace_bytes


class Rela:
    """
    Rela 处理armv8重定位项
    """
    @classmethod
    def rela_type_257(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_ABS64
        """
        symvalue = value + raddend
        replace_bytes = 8

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_258(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_ABS32
        """
        symvalue = value + raddend
        replace_bytes = 4
        ret = reloc_check_symval_signed(31, 32, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_259(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_ABS16
        """
        symvalue = value + raddend
        replace_bytes = 2
        ret = reloc_check_symval_signed(15, 16, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_260(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_PREL64
        """
        symvalue = value + raddend - rela_addr
        replace_bytes = 8

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_261(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_PREL32
        """
        symvalue = value + raddend - rela_addr
        replace_bytes = 4
        ret = reloc_check_symval_signed(31, 32, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_262(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_PREL16
        """
        symvalue = value + raddend - rela_addr
        replace_bytes = 2
        ret = reloc_check_symval_signed(15, 16, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_263(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_UABS_G0
        """
        symvalue = value + raddend
        replace_bytes = 0
        ret = reloc_check_symval_unsigned(16, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes

        imm = symvalue & 0xffff
        symvalue, replace_bytes = reloc_move_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_264(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_UABS_G0_NC
        """
        symvalue = value + raddend
        replace_bytes = 0
        imm = symvalue & 0xffff
        symvalue, replace_bytes = reloc_move_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_265(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_UABS_G1
        """
        symvalue = value + raddend
        replace_bytes = 0
        ret = reloc_check_symval_unsigned(32, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes

        imm = (symvalue >> 16) & 0xffff
        symvalue, replace_bytes = reloc_move_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_266(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_UABS_G1_NC
        """
        symvalue = value + raddend
        replace_bytes = 0
        imm = (symvalue >> 16) & 0xffff
        symvalue, replace_bytes = reloc_move_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_267(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_UABS_G2
        """
        symvalue = value + raddend
        replace_bytes = 0
        ret = reloc_check_symval_unsigned(48, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes

        imm = (symvalue >> 32) & 0xffff
        symvalue, replace_bytes = reloc_move_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_268(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_UABS_G2_NC
        """
        symvalue = value + raddend
        replace_bytes = 0
        imm = (symvalue >> 32) & 0xffff
        symvalue, replace_bytes = reloc_move_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_269(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_UABS_G3
        """
        symvalue = value + raddend
        replace_bytes = 0
        imm = (symvalue >> 48) & 0xffff
        symvalue, replace_bytes = reloc_move_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_270(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_SABS_G0
        """
        symvalue = value + raddend
        replace_bytes = 0
        ret = reloc_check_symval_signed(16, 16, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes

        imm = symvalue & 0xffff
        symvalue, replace_bytes = reloc_move_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_271(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_SABS_G1
        """
        symvalue = value + raddend
        replace_bytes = 0
        ret = reloc_check_symval_signed(32, 32, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes

        imm = (symvalue >> 16) & 0xffff
        symvalue, replace_bytes = reloc_move_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_272(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_SABS_G2
        """
        symvalue = value + raddend
        replace_bytes = 0
        ret = reloc_check_symval_signed(48, 48, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes

        imm = (symvalue >> 32) & 0xffff
        symvalue, replace_bytes = reloc_move_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_273(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_LD_PREL_LO19
        """
        symvalue = value + raddend - rela_addr
        replace_bytes = 4
        local_pre_addr = pre_addr & MASK_32_BITS
        ret = reloc_check_symval_signed(20, 20, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes

        imm = (symvalue & 0x1ffffc) >> 2
        symvalue = local_pre_addr
        symvalue &= 0xff00001f
        symvalue |= (imm << 5)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_274(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_ADR_PREL_LO21
        """
        symvalue = value + raddend - rela_addr
        replace_bytes = 4
        local_pre_addr = pre_addr & MASK_32_BITS
        ret = reloc_check_symval_signed(20, 20, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes

        # Set an ADR immediate value to bits [20:0] of X
        imm = symvalue & 0x1fffff
        # ADRP immediate value : 1 immlo[30:29] 1 0000 immhi[23:5] x xxxx
        symvalue = local_pre_addr
        symvalue &= 0x9f00001f
        symvalue |= ((imm & 0x3) << 29)
        symvalue |= (((imm & 0x1ffffc) >> 2) << 5)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_275(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_ADR_PREL_PG_HI21
        """
        symvalue = ((value + raddend) & (~0xfff)) - (rela_addr & (~0xfff))
        local_pre_addr = pre_addr & MASK_32_BITS
        replace_bytes = 4
        ret = reloc_check_symval_signed(32, 32, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes
        imm = (symvalue & 0x1fffff000) >> 12
        symvalue = local_pre_addr
        symvalue &= 0x9f00001f
        symvalue |= ((imm & 0x3) << 29)
        symvalue |= (((imm & 0x1ffffc) >> 2) << 5)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_276(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_ADR_PREL_PG_HI21_NC
        """
        symvalue = ((value + raddend) & (~0xfff)) - (rela_addr & (~0xfff))
        replace_bytes = 4
        local_pre_addr = pre_addr & MASK_32_BITS
        imm = (symvalue & 0x1fffff000) >> 12
        symvalue = local_pre_addr
        symvalue &= 0x9f00001f
        symvalue |= ((imm & 0x3) << 29)
        symvalue |= (((imm & 0x1ffffc) >> 2) << 5)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_277(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_ADD_ABS_LO12_NC
        """
        symvalue = value + raddend
        replace_bytes = 4
        imm = symvalue & 0xfff
        local_pre_addr = pre_addr & MASK_32_BITS
        symvalue = local_pre_addr
        symvalue &= 0xffc003ff
        symvalue |= (imm << 10)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_278(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_LDST8_ABS_LO12_NC
        """
        symvalue = value + raddend
        replace_bytes = 0
        imm = symvalue & 0xfff
        symvalue, replace_bytes = reloc_ld_st_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_279(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_TSTBR14
        """
        symvalue = value + raddend - rela_addr
        replace_bytes = 4
        local_pre_addr = pre_addr & MASK_32_BITS
        ret = reloc_check_symval_signed(15, 15, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes
        imm = (symvalue & 0xfffc) >> 2
        symvalue = local_pre_addr
        symvalue &= 0xfff8001f
        symvalue |= (imm << 5)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_280(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_CONDBR19
        """
        symvalue = value + raddend - rela_addr
        replace_bytes = 4
        local_pre_addr = pre_addr & MASK_32_BITS
        ret = reloc_check_symval_signed(20, 20, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes
        imm = (symvalue & 0x1ffffc) >> 2
        symvalue = local_pre_addr
        symvalue &= 0xff00001f
        symvalue |= (imm << 5)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_282(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_JUMP26
        """
        return reloc_call_or_b(value, raddend, rela_addr, pre_addr)


    @classmethod
    def rela_type_283(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_CALL26
        """
        return reloc_call_or_b(value, raddend, rela_addr, pre_addr)


    @classmethod
    def rela_type_284(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_LDST16_ABS_LO12_NC
        """
        symvalue = value + raddend
        replace_bytes = 0
        imm = (symvalue >> 1) & 0x7ff
        symvalue, replace_bytes = reloc_ld_st_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_285(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_LDST32_ABS_LO12_NC
        """
        symvalue = value + raddend
        replace_bytes = 0
        imm = (symvalue >> 2) & 0x3ff
        symvalue, replace_bytes = reloc_ld_st_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_286(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_LDST64_ABS_LO12_NC
        """
        symvalue = value + raddend
        replace_bytes = 0
        imm = (symvalue >> 3) & 0x1ff
        symvalue, replace_bytes = reloc_ld_st_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_287(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_PREL_G0
        """
        symvalue = value + raddend - rela_addr
        replace_bytes = 0
        ret = reloc_check_symval_signed(16, 16, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes
        imm = symvalue & 0xffff
        symvalue, imm = reloc_movw_inst(pre_addr, symvalue, imm)
        symvalue, replace_bytes = reloc_move_imm(symvalue, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_288(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_PREL_G0_NC
        """
        symvalue = value + raddend - rela_addr
        replace_bytes = 0
        imm = symvalue & 0xffff
        symvalue, replace_bytes = reloc_move_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_289(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_PREL_G1
        """
        symvalue = value + raddend - rela_addr
        replace_bytes = 0
        ret = reloc_check_symval_signed(32, 32, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes
        imm = (symvalue >> 16) & 0xffff
        symvalue, imm = reloc_movw_inst(pre_addr, symvalue, imm)
        symvalue, replace_bytes = reloc_move_imm(symvalue, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_290(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_PREL_G1_NC
        """
        symvalue = value + raddend - rela_addr
        replace_bytes = 0
        imm = (symvalue >> 16) & 0xffff
        symvalue, imm = reloc_movw_inst(pre_addr, symvalue, imm)
        symvalue, replace_bytes = reloc_move_imm(symvalue, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_291(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_PREL_G2
        """
        symvalue = value + raddend - rela_addr
        replace_bytes = 0
        ret = reloc_check_symval_signed(48, 48, symvalue)
        if not ret:
            return ret, symvalue, replace_bytes
        imm = (symvalue >> 32) & 0xffff
        symvalue, imm = reloc_movw_inst(pre_addr, symvalue, imm)
        symvalue, replace_bytes = reloc_move_imm(symvalue, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_292(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_PREL_G2_NC
        """
        symvalue = value + raddend - rela_addr
        replace_bytes = 0
        imm = (symvalue >> 32) & 0xffff
        symvalue, replace_bytes = reloc_move_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_293(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_MOVW_PREL_G3
        """
        symvalue = value + raddend - rela_addr
        replace_bytes = 0
        imm = (symvalue >> 48) & 0xffff
        symvalue, replace_bytes = reloc_move_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


    @classmethod
    def rela_type_299(cls, value, raddend, rela_addr, pre_addr):
        """
        重定位类型 R_AARCH64_LDST128_ABS_LO12_NC
        """
        symvalue = value + raddend
        replace_bytes = 0
        imm = (symvalue >> 4) & 0xff
        symvalue, replace_bytes = reloc_ld_st_imm(pre_addr, imm)

        return True, symvalue, replace_bytes


func_dic = {
    257: Rela.rela_type_257,
    258: Rela.rela_type_258,
    259: Rela.rela_type_259,
    260: Rela.rela_type_260,
    261: Rela.rela_type_261,
    262: Rela.rela_type_262,
    263: Rela.rela_type_263,
    264: Rela.rela_type_264,
    265: Rela.rela_type_265,
    266: Rela.rela_type_266,
    267: Rela.rela_type_267,
    268: Rela.rela_type_268,
    269: Rela.rela_type_269,
    270: Rela.rela_type_270,
    271: Rela.rela_type_271,
    272: Rela.rela_type_272,
    273: Rela.rela_type_273,
    274: Rela.rela_type_274,
    275: Rela.rela_type_275,
    276: Rela.rela_type_276,
    277: Rela.rela_type_277,
    278: Rela.rela_type_278,
    279: Rela.rela_type_279,
    280: Rela.rela_type_280,
    282: Rela.rela_type_282,
    283: Rela.rela_type_283,
    284: Rela.rela_type_284,
    285: Rela.rela_type_285,
    286: Rela.rela_type_286,
    287: Rela.rela_type_287,
    288: Rela.rela_type_288,
    289: Rela.rela_type_289,
    290: Rela.rela_type_290,
    291: Rela.rela_type_291,
    292: Rela.rela_type_292,
    293: Rela.rela_type_293,
    299: Rela.rela_type_299
}
