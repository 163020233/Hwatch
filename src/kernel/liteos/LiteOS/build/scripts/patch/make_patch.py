#!/usr/bin/env python3
# coding=utf-8
# Copyright © Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
# Description: Make patch.
# Create: 2024-09-25

"""tools/make_patch.py"""


import sys
import os
import configparser
import codecs
import logging
from struct import pack

from elf import ElfInfo
from pat import NewPat
from pat import write_hphead_to_file
from pat import turn_unit_to_hp
from pat import write_head_to_file
from pat import PAT_HEAD_LEN
from pat import PAT_HPHEAD_LEN
from pat import PatchArea
from crc import make_crc
from rela import func_dic

logging.basicConfig(level=logging.DEBUG)

# 声明使用的全局变量
FUNC_SYM_TYPE = 2
# 解析的.o文件
ELF = ""
# 基础BIN中的符号HASH
ELF_SYM_HASH = {}
# .o文件的二进制数据，用于重定位等
ELF_BUFF = bytearray()
# 解析的需要打补丁的镜像文件
ELF_SYM = ""
# 当前文件目录
CURPATH = os.path.dirname(os.path.realpath(__file__))

# config文件解析字典
BASICINFO = dict()
OUTFILE = dict()

RET_OK = 1
RET_NOK = 0
# 退出代码
CODE_EXIT = -1
# 新pat文件数据
PAT_NEW = NewPat()
# 新pat文件使用的空间
PATCH_AREA = PatchArea()
# 新pat文件的句柄
OUT_PAT_FILE_HANDLE = ""
# 重定位后的ELF文件的句柄
OUT_ELF_FILE_HANDLE = ""
# 当前补丁号
G_CUR_PATCH_NUM = 0
# 入参：输出的文件夹
OUTPUT_DIR = ''
OUT_PAT_FILE = ""
# 重定位后的ELF文件
OUT_ELF_FILE = ""
# 重组后的pat文件主要信息
OUT_INFO_FILE = ""
# 函数的最小size
FUNC_MIN_SIZE = 16

# e_machine类型
ELF_EM_AARCH64 = 0xB7

# 节区头部表格中的特殊下标
OS_ELF_SHN_UNDEF = 0x0      # 标记未定义的、缺失的、不相关的，或者没有含义的
OS_ELF_SHN_ABS = 0xFFF1     # 包含对应引用量的绝对取值。这些值不会被重定位所影响
OS_ELF_SHN_COMMON = 0xFFF2  # 相对于此节区定义的符号是公共符号

# 符号的绑定类型, info的高四位
OS_ELF_STB_LOCAL = 0x0      # 局部符号
OS_ELF_STB_GLOBAL = 0x1     # 全局符号
OS_ELF_STB_WEAK = 0x2       # 弱引用

# 补丁函数白名单，在该名单中的函数不可打补丁;
# 白名单中添加成员方法如下： PATCH_FUNC_WHITE_LIST = [b"TspFuncNameA", b"TspFuncNameB"]
PATCH_FUNC_WHITE_LIST = []


def write_bytes_to_file(offset: int, byte_arr: bytearray):
    """
    write_bytes_to_file
    :param offset:
    :param byte_arr:
    :return:
    """
    OUT_PAT_FILE_HANDLE.seek(offset)
    OUT_PAT_FILE_HANDLE.write(byte_arr)
    return len(byte_arr)


def write_bytes_to_rela_elf(offset: int, byte_arr: bytearray):
    """
    write_bytes_to_elf
    重定位后的ELF_BUFF写入文件中，用于后续维测、回溯
    :param offset:
    :param byte_arr:
    :return:
    """
    OUT_ELF_FILE_HANDLE.seek(offset)
    OUT_ELF_FILE_HANDLE.write(byte_arr)
    return len(byte_arr)


def write_pat_info_to_file():
    """
    write_pat_info_to_file
    :return:
    """
    if os.path.exists(OUT_INFO_FILE):
        os.remove(OUT_INFO_FILE)
    with os.fdopen(os.open(OUT_INFO_FILE, os.O_RDWR | os.O_CREAT, 0o644), 'w') as file_handle:
        depen_sym = {}
        func_num = 0
        depen_sym['PatchNo'] = G_CUR_PATCH_NUM
        depen_sym['PatchTextStartAddr'] = hex(int(BASICINFO['patchtextstartaddr'], 16))
        depen_sym['UsedCodelen'] = hex(int(PAT_NEW.new_patch_hphead["CodeLength"]))
        depen_sym['PatchShareDataStart'] = hex(int(BASICINFO['patchsharedatastart'], 16))
        depen_sym['UsedDatalen'] = hex(int(PAT_NEW.new_patch_hphead["ShareDataLength"]))
        depen_sym['PatchCRC'] = hex(int(PAT_NEW.pat_header['PatchCRC']))
        depen_sym['HPCrc'] = hex(int(PAT_NEW.new_patch_hphead['HPCrc']))
        for key in PAT_NEW.patch_func_name_list:
            func_num += 1
            func_name = "func{0}_name".format(func_num)
            func_old_addr = "func{0}_old_addr".format(func_num)
            func_new_addr = "func{0}_new_addr".format(func_num)
            depen_sym[func_name] = key
            depen_sym[func_old_addr] = PAT_NEW.patch_old_func_addr_list[key]
            depen_sym[func_new_addr] = PAT_NEW.patch_new_func_addr_list[key]
        file_handle.write('{}'.format(depen_sym))
    return len(depen_sym)


def addr_align(addr, align):
    """
    addr_align
    :param addr:
    :param align:
    :return:
    """
    return (addr + align - 1) & ~(align - 1)


def init_pat_head():
    """
    init_pat_head
    :return:
    """
    hdr = dict()
    hdr['Ident'] = bytes(BASICINFO['patchtype'],
                               encoding="utf8").ljust(4, b'\x00')
    # 补丁包校验和
    hdr['PatchCRC'] = 52894
    # 补丁包类型，预留功能，默认为0
    hdr['PacketType'] = 0
    # 主机版本描述
    hdr['ProgVersion'] = bytes(BASICINFO['patchflag'],
                               encoding="utf8").ljust(48, b'\x00')
    # 补丁包中补丁文件个数
    hdr['PatchUnitNum'] = 1
    # 补丁单元索引表偏移
    hdr['IndexOffset'] = 0
    # 补丁单元索引表记录数
    hdr['EntryNum'] = 1
    # 保留位
    hdr['Reserved'] = 0
    # 保留主机版本的编译时间
    hdr['CompileTime'] = b'\x00' * 8

    PAT_NEW.pat_header = hdr

    return PAT_NEW.pat_header


def init_pat_hphead():
    """
    init_pat_hphead
    :return:
    """
    hphead = dict()
    hphead["HPVersion"] = b'\x02\x01\x01\x03'
    hphead["HPFlag"] = b'SP'
    hphead["HPFileName"] = bytes(OUTPUT_DIR, encoding="utf8")
    hphead["HPCrc"] = 0
    hphead["ndependFlag"] = 0
    hphead["InterPatchNo"] = G_CUR_PATCH_NUM
    hphead['ProgVersion'] = bytes(BASICINFO['patchflag'],
                               encoding="utf8").ljust(48, b'\x00')
    hphead["PatchVersion"] = bytes(BASICINFO['patchflag'],
                                   encoding="utf8").ljust(32, b'\x00')
    hphead["CreateTime"] = b'\x00\x00\x00\x00\x00\x00\x00\x00'
    hphead["NetID"] = 1
    hphead["ProgID"] = int(BASICINFO['ProgType'.lower()])
    # 需要重新
    hphead["CodeAddr"] = 0
    hphead["CodeLength"] = 0
    hphead["ShareDataAddr"] = 0
    hphead["ShareDataLength"] = 0
    hphead["DataAddr"] = 0
    hphead["DataLength"] = 0
    hphead["Funcs"] = 0
    hphead["Flag"] = b'CP'
    hphead["CrcLen"] = 0
    hphead["SegInfSize"] = 0
    PAT_NEW.new_patch_hphead = hphead

    return PAT_NEW.new_patch_hphead


def init_pat_hpunit():
    """
    init_pat_hpunit
    :return:
    """
    hp_unit = dict()
    hp_unit['UnitType'] = 0
    hp_unit['UnitOffset'] = 0
    hp_unit['UnitSize'] = 0
    hp_unit['UnitFlag'] = 0
    hp_unit['InterPatchNo'] = int(G_CUR_PATCH_NUM)
    PAT_NEW.new_patch_unit = hp_unit
    return hp_unit


def get_replace_func():
    """
    get_replace_func 获取补丁源文件中的global函数符号
    :return:
    """
    func_table = []
    for symstr in ELF.symtab:
        if (symstr['info'] & 0xf) == FUNC_SYM_TYPE \
                and symstr['shndx'] != OS_ELF_SHN_UNDEF \
                and ((symstr['info'] >> 4) & 0xf) == OS_ELF_STB_GLOBAL:
            func_table.append(symstr)
    return func_table


def handle_patch_area():
    """
    handle_patch_area
    从配置文件获取可用的空间
    :return:
    """
    # 从config文件获取
    PATCH_AREA.patch_code_area_start = \
        int(BASICINFO['patchtextstartaddr'], 16)
    PATCH_AREA.patch_code_area_length = \
        int(BASICINFO['patchtextarealength'], 16)
    PATCH_AREA.patch_share_data_area_start = \
        int(BASICINFO['patchsharedatastart'], 16)
    PATCH_AREA.patch_share_data_area_length = \
        int(BASICINFO['patchsharedataarealength'], 16)
    PATCH_AREA.patch_code_available_len = \
        int(BASICINFO['PatchTextAreaAvailableLength'.lower()], 16)
    PATCH_AREA.patch_share_data_available_len = \
        int(BASICINFO['PatchShareDataAreaAvailableLength'.lower()], 16)
    PATCH_AREA.patch_max_func_num_per_unit = \
        int(BASICINFO['MaxFuncInAUnit'.lower()], 16)

    # 校验可用内存是否足够
    if PATCH_AREA.patch_code_available_len < PATCH_AREA.patch_code_area_length:
        logging.error("[error  ] There's not enough text memory available for patch[%d] ", G_CUR_PATCH_NUM)
        return False
    if PATCH_AREA.patch_share_data_available_len < PATCH_AREA.patch_share_data_area_length:
        logging.error("[error  ] There's not enough share data memory available for patch[%d] ", G_CUR_PATCH_NUM)
        return False

    PAT_NEW.new_patch_hphead["CodeAddr"] = PATCH_AREA.patch_code_area_start
    PAT_NEW.new_patch_hphead["ShareDataAddr"] = \
        PATCH_AREA.patch_share_data_area_start

    ral_text_free_addr = PATCH_AREA.patch_code_area_start
    ral_share_data_free_addr = PATCH_AREA.patch_share_data_area_start
    # 处理每个段的地址
    handle_sections(ral_share_data_free_addr, ral_text_free_addr)

    return True


def handle_sections(ral_share_data_free_addr, ral_text_free_addr):
    """
    handle_sections
    :param ral_share_data_free_addr:
    :param ral_text_free_addr:
    :return:
    """
    sec_header_offset = ELF.header.get("e_shoff")
    loop = 0
    for section in ELF.sections:
        if section in ELF.sect_text:
            # text段
            ral_text_free_addr_align = addr_align(ral_text_free_addr, section['sh_addralign'])
            gap = ral_text_free_addr_align - ral_text_free_addr
            section["rel_sect_addr"] = ral_text_free_addr_align
            section["rel_sect_free_addr_at"] = section["rel_sect_addr"]
            section["sect_gap"] = gap
            section["sect_index"] = loop
            ral_text_free_addr += section['sh_size']
            ral_text_free_addr += gap

            # 写段头的shAddr，用于校正重定位后的.o中的符号地址，方便问题定位
            location = sec_header_offset + loop * ELF.header.get("e_shentsize") + 16
            ELF_BUFF[location: location + 8] = pack("<Q", section["rel_sect_addr"])
        loop += 1

    loop = 0
    for section in ELF.sections:
        if section in ELF.sect_rodata:
            # rodata段放在text内存区
            ral_text_free_addr_align = addr_align(ral_text_free_addr, section['sh_addralign'])
            gap = ral_text_free_addr_align - ral_text_free_addr
            section["rel_sect_addr"] = ral_text_free_addr_align
            section["rel_sect_free_addr_at"] = section["rel_sect_addr"]
            section["sect_gap"] = gap
            section["sect_index"] = loop
            ral_text_free_addr += section['sh_size']
            ral_text_free_addr += gap

            location = sec_header_offset + loop * ELF.header.get("e_shentsize") + 16
            ELF_BUFF[location: location + 8] = pack("<Q", section["rel_sect_addr"])
        loop += 1

    loop = 0
    for section in ELF.sections:
        if section in ELF.sect_data:
            # data段
            if section['sh_size'] == 0:
                section["rel_sect_addr"] = 0
                section["rel_sect_free_addr_at"] = section["rel_sect_addr"]
                loop += 1
                continue
            ral_share_data_free_addr_align = addr_align(ral_share_data_free_addr, section['sh_addralign'])
            gap = ral_share_data_free_addr_align - ral_share_data_free_addr
            section["rel_sect_addr"] = ral_share_data_free_addr_align
            section["rel_sect_free_addr_at"] = section["rel_sect_addr"]
            section["sect_gap"] = gap
            section["sect_index"] = loop
            ral_share_data_free_addr += section['sh_size']
            ral_share_data_free_addr += gap

            location = sec_header_offset + loop * ELF.header.get("e_shentsize") + 16
            ELF_BUFF[location: location + 8] = pack("<Q", section["rel_sect_addr"])
        loop += 1

    loop = 0
    for section in ELF.sections:
        if section in ELF.sect_bss:
            # BSS段
            if section['sh_size'] == 0:
                loop += 1
                continue
            ral_share_data_free_addr_align = addr_align(ral_share_data_free_addr, section['sh_addralign'])
            gap = ral_share_data_free_addr_align - ral_share_data_free_addr
            section["rel_sect_addr"] = ral_share_data_free_addr_align
            section["rel_sect_free_addr_at"] = section["rel_sect_addr"]
            section["sect_gap"] = gap
            section["sect_index"] = loop
            ral_share_data_free_addr += section['sh_size']
            ral_share_data_free_addr += gap

            location = sec_header_offset + loop * ELF.header.get("e_shentsize") + 16
            ELF_BUFF[location: location + 8] = pack("<Q", section["rel_sect_addr"])
        loop += 1

    for section in ELF.sections:
        # other
        in_falg = section in ELF.sect_text or section in ELF.sect_bss
        if in_falg or section in ELF.sect_data or section in ELF.sect_rodata:
            continue
        section["rel_sect_addr"] = 0
        section["rel_sect_free_addr_at"] = section["rel_sect_addr"]


def print_hex(byte):
    """
    print_hex
    :param byte:
    :return:
    """
    line = []
    for i in byte:
        line.append(hex(int(i)))
    return " ".join(line)


def handle_linkablefile():
    """
    handle_linkablefile
    :return:
    """
    for rela_sect in ELF.sect_rela:
        # 需要做重定位处理的代码段,rela_sect['sh_info']重定位段对应段的index
        section = ELF.sections[rela_sect['sh_info']]
        # 新补丁重定位后的实际地址
        in_falg = section in ELF.sect_text or section in ELF.sect_bss
        if in_falg or section in ELF.sect_data or section in ELF.sect_rodata:
            # 重定位处理
            ret = in_rela_table(rela_sect)
            if not ret:
                return ret
    # 把重定位后的ELF写入文件
    result = write_bytes_to_rela_elf(0, ELF_BUFF)
    if result == 0:
        logging.error("重定位后ELF内容写入失败,写入字节数为:[%d] ", result)
        return False
    return True


def in_rela_table(rela_sect):
    """
    in_rela_table
    :return:
    """
    for rela_table in ELF.get_rela_table(rela_sect):
        #sh_info 为重定位段时表示需要重定位符号所在的段的索引
        ret = handle_rela_table(rela_table, ELF.sections[rela_sect['sh_info']])
        if not ret:
            return ret
    return True


def solve_negative_number(value: int, replace_bytes: int):
    """
    solve_negative_number
    把负数转换成C语言上对应的形式
    """
    trans_value = -value
    ret = 0
    if replace_bytes == 2:
        ret = ((~trans_value) & 0xffff) + 1
    elif replace_bytes == 4:
        ret = ((~trans_value) & 0xffffffff) + 1
    elif replace_bytes == 8:
        ret = ((~trans_value) & 0xffffffffffffffff) + 1
    else:
        logging.error("[error]replace_bytes is : [%d] It's not legal, please check!", replace_bytes)
        ret = CODE_EXIT

    return ret


def reloc_value_write_to_elf(value, rela_table, section, sym_name):
    """
    reloc_value_write_to_elf
    """
    sh_offset = section['sh_offset']
    r_offset = rela_table['r_offset']
    # 地址须按照小端格式进行解析，然后再读取
    location = ELF_BUFF[sh_offset + r_offset: sh_offset + r_offset + 8]
    pre_addr_u64 = location[0] | (location[1] << 8) | (location[2] << 16) | (location[3] << 24) | \
                   (location[4] << 32) | (location[5] << 40) | (location[6] << 48) | (location[7] << 56)
    replace_bytes = 0 # elf中需要替换的字节数
    # rela_addr需要重定位的指令在板子上的地址，即armv8重定位所须的参数P
    rela_addr = section['rel_sect_addr'] + rela_table['r_offset']
    # 重定位符号信息，r_info高32位表示重定位入口的符号在符号表中的下标，低32位表示重定位类型
    elf64_r_type = rela_table['r_info'] & 0xffffffff

    if elf64_r_type in func_dic:
        function = func_dic.get(elf64_r_type)
        ret, symvalue, replace_bytes = function(value, rela_table['r_addend'], rela_addr, pre_addr_u64)
        if not ret:
            logging.error("[error]rela_func run error! rela_type:[%d], sym_name:[%s]", elf64_r_type, sym_name)
            return ret
    else:
        logging.error("[error]rela_type don't support: [%d]!", elf64_r_type)
        return False

    if symvalue < 0:
        neg_value = symvalue
        symvalue = solve_negative_number(neg_value, replace_bytes)
        if symvalue == CODE_EXIT:
            return False

    if replace_bytes == 2:
        ELF_BUFF[sh_offset + r_offset: sh_offset + r_offset + 2] = pack("<H", symvalue)
    elif replace_bytes == 4:
        ELF_BUFF[sh_offset + r_offset: sh_offset + r_offset + 4] = pack("<I", symvalue)
    elif replace_bytes == 8:
        ELF_BUFF[sh_offset + r_offset: sh_offset + r_offset + 8] = pack("<Q", symvalue)
    else:
        logging.error("[error]replace_bytes is : [%d] It's not legal, please check!", replace_bytes)
        return False

    return True


def handle_rela_table(rela_table, section):
    """
    handle_rela_table
    :param rela_table:
    :param section:
    :return:
    """
    # 重定位符号信息，r_info高32位表示重定位入口的符号在符号表中的下标，低32位表示重定位类型
    location_sym = ELF.symtab[rela_table['r_info'] >> 32]
    # 判断重定位符号是否需要替换，获取重定位符号实际的地址
    sym_list = []
    elf_sym_tal = dict()
    for sym in ELF_SYM.symtab:
        if (sym['name'] == location_sym['name']) and \
            (location_sym['shndx'] == OS_ELF_SHN_UNDEF):
            # 只有对与UNDEF类型的符号，且存在与基础bin的符号表中，才会使用基础bin中的符号值
            sym_list.append(sym)
    if sym_list:
        elf_sym_tal = dict(sym_list[0])
    # value 为符号的实际地址
    if location_sym['info'] & 0xf == 3: # local重定位
        value = ELF.sections[location_sym['shndx']]['rel_sect_addr'] \
                + location_sym['value'] # 符号所在的段地址+符号值；r_addend在重定位的时候加
    elif elf_sym_tal:
        value = elf_sym_tal['value']
    else:
        if location_sym['info'] & 0xf == 1 \
            and location_sym['shndx'] == 65522: # 未初始化的全局变量：符号与某个数据对象相关且在com段
            logging.error("[info]global object:[%s] need init!", location_sym['name'])
            return False
        value = ELF.sections[location_sym['shndx']]['rel_sect_addr'] \
                + location_sym['value'] # 补丁文件中的global重定位

    # 将重定位后的值写入ELF
    ret = reloc_value_write_to_elf(value, rela_table, section, location_sym['name'])
    if not ret:
        return ret
    return True


def getconfig(config_file):
    """
    getconfig
    :param config_file:
    :return:
    """
    config_path = os.path.join(config_file)

    # 创建管理对象
    conf = configparser.ConfigParser()

    conf.read(config_path)

    baseinfo = dict(conf.items('BASICINFO'))
    out = dict(conf.items("OUTFILE"))

    return baseinfo, out


def write_sect_text_to_hp(offset: int):
    """
    write_sect_text_to_hp
    :param offset:
    :return:
    """
    byte = bytearray()
    gap = bytearray()
    for sect in ELF.sect_text:
        # 按照sh_addralign对齐，gap填充0
        gap = bytearray(b'\x00') * sect["sect_gap"]
        byte.extend(gap)
        byte.extend(ELF_BUFF[sect['sh_offset']: sect['sh_offset'] + sect['sh_size']])

    result = write_bytes_to_file(offset, byte)
    if result == 0:
        logging.error("代码段内容写入失败，写入字节数为:[%d] ", result)
        return result
    return len(byte)


def write_relace_func_to_hp(file_offset: int):
    """
    write_relace_func_to_hp
    :param file_offset:
    :return:
    """
    buff = bytearray()
    replace_func = get_replace_func()
    patch_func_num = 0
    for func in replace_func:
        if PATCH_AREA.patch_max_func_num_per_unit <= patch_func_num:
            logging.error("替换函数个数已达到配置上限：[%d] ", PATCH_AREA.patch_max_func_num_per_unit)
            return CODE_EXIT
        # 补丁单元中的global符号在基础bin中能匹配到时才进行计数、写入文件
        ret = write_func(buff, func)
        if ret == CODE_EXIT:
            return ret
        if ret == RET_OK:
            patch_func_num += 1

    PAT_NEW.new_patch_hphead["Funcs"] = patch_func_num
    logging.info("当前补丁中替换函数的个数：[%d]", patch_func_num)
    ret = write_bytes_to_file(file_offset, buff)
    if ret == 0:
        logging.error("替换函数内容写入失败，写入字节数为:[%d] ", ret)
        return ret

    return len(buff)


def write_func(buff, func):
    """
    write_func
    :param buff:
    :param func:
    :return:
    """
    # 要替换的地址，段首地址+符号值
    free_addr = ELF.sections[func['shndx']]["rel_sect_free_addr_at"] + func['value']
    old_addr = 0
    exist_flag = False
    # bin文件被替换的函数地址
    for sym in ELF_SYM.symtab:
        if sym['name'] == func['name'] and (sym['info'] & 0xf) == FUNC_SYM_TYPE:
            if ELF_SYM_HASH[sym['name']] != 1:
                # 基础bin中的同名静态函数在符号表中符号名相同, 地址不同;
                # 不支持对同名的静态函数打补丁; 补丁制作报错, 须人工排查解决
                logging.error("Multiple static function [%s] in BIN file!", sym['name'])
                logging.error("function count in basic bin:[%d]", ELF_SYM_HASH[sym['name']])
                return CODE_EXIT

            # 基础Bin中符号唯一的函数可以打补丁
            old_addr = sym['value']
            exist_flag = True
            break
    if not exist_flag:
        return RET_NOK
    # 白名单中的函数不可打补丁
    if func['name'] in PATCH_FUNC_WHITE_LIST:
        logging.error("函数[%s] is in the whitelist that cannot be patched. please check!", func['name'])
        return CODE_EXIT
    for sym in ELF.symtab:
        if sym['name'] == func['name']:
            if sym['size'] < FUNC_MIN_SIZE:
                logging.error("[error] func[%s]: size[%d], Less than 16 bytes!!!", func['name'], sym['size'])
                return CODE_EXIT

    PAT_NEW.patch_func_name_list[func['name']] = func['name']
    PAT_NEW.patch_old_func_addr_list[func['name']] = hex(old_addr)
    PAT_NEW.patch_new_func_addr_list[func['name']] = hex(free_addr)
    buff.extend(pack('<Q', old_addr))
    buff.extend(pack('<Q', free_addr))
    logging.info("函数[%s]: %#x被替换成%#x", func['name'], old_addr, free_addr)

    return RET_OK


def write_sect_data_to_hp(offset: int):
    """
    write_sect_data_to_hp
    :param offset:
    :return:
    """
    byte = bytearray()
    gap = bytearray()
    for sect in ELF.sect_data:
        # 按照sh_addralign对齐，gap填充0
        gap = bytearray(b'\x00') * sect["sect_gap"]
        byte.extend(gap)
        byte.extend(ELF_BUFF[sect['sh_offset']: sect['sh_offset'] + sect['sh_size']])

    ret = write_bytes_to_file(offset, byte)
    return ret


def write_sect_rodata_to_hp(offset: int):
    """
    write_sect_rodata_to_hp
    :param offset:
    :return:
    """
    byte = bytearray()
    gap = bytearray()
    for sect in ELF.sect_rodata:
        # 按照sh_addralign对齐，gap填充0
        gap = bytearray(b'\x00') * sect["sect_gap"]
        byte.extend(gap)
        byte.extend(ELF_BUFF[sect['sh_offset']: sect['sh_offset'] + sect['sh_size']])

    ret = write_bytes_to_file(offset, byte)
    return ret


def write_sect_bss_to_hp(offset: int):
    """
    write_sect_bss_to_hp
    :param offset:
    :return:
    """
    byte = bytearray()
    for sect in ELF.sect_bss:
        # 按照sh_addralign对齐，gap填充0
        byte = bytearray(b'\x00') * (sect["sect_gap"] + sect["sh_size"])

    ret = write_bytes_to_file(offset, byte)
    return ret


def write_dependent_to_hp(offset: int):
    """
    write_dependent_to_hp
    :param offset:
    :return:
    """
    count = 0
    # 不依赖旧补丁，count为0
    byte_arr = bytearray(pack('<I', count))
    ret = write_bytes_to_file(offset, byte_arr)
    return ret


def write_static_patch(file_offset: int):
    """
    write_static_patch
    :param file_offset:
    :return:
    """
    offset = file_offset + PAT_HPHEAD_LEN
    crc_offset = offset

    # 写入替换函数
    temp_size = write_relace_func_to_hp(offset)
    if temp_size == CODE_EXIT:
        return False, 0
    offset += temp_size

    # 处理依赖
    offset += write_dependent_to_hp(offset)

    # 写代码段
    temp_size = write_sect_text_to_hp(offset)
    PAT_NEW.new_patch_hphead["CodeLength"] = temp_size
    offset += temp_size

    # 写rodata段
    temp_size = write_sect_rodata_to_hp(offset)
    PAT_NEW.new_patch_hphead["CodeLength"] += temp_size
    offset += temp_size

    if PAT_NEW.new_patch_hphead["CodeLength"] > PATCH_AREA.patch_code_available_len:
        logging.error("当前补丁代码段长度已超过了预留大小！")
        return False, 0

    temp_offset = offset
    # 写数据段
    temp_size = write_sect_data_to_hp(offset)
    offset += temp_size

    # 写bss段
    temp_size = write_sect_bss_to_hp(offset)
    offset += temp_size

    PAT_NEW.new_patch_hphead["ShareDataLength"] = offset - temp_offset

    if PAT_NEW.new_patch_hphead["ShareDataLength"] > PATCH_AREA.patch_share_data_available_len:
        logging.error("当前补丁数据段长度已超过了预留大小！")
        return False, 0

    # 写时间戳
    PAT_NEW.new_patch_hphead["year"] = 2023
    PAT_NEW.new_patch_hphead["month"] = 6
    PAT_NEW.new_patch_hphead["day"] = 12
    PAT_NEW.new_patch_hphead["hour"] = 9
    PAT_NEW.new_patch_hphead["minute"] = 41
    PAT_NEW.new_patch_hphead["second"] = 19
    PAT_NEW.new_patch_hphead["reserved"] = 0

    crc = 0
    OUT_PAT_FILE_HANDLE.seek(crc_offset)
    crc_buff = bytearray(OUT_PAT_FILE_HANDLE.read(offset - crc_offset))
    ret, crc = make_crc(crc_buff, crc)
    if not ret:
        return ret, 0
    # 对pat描述头做crc计算，offset为14
    OUT_PAT_FILE_HANDLE.seek(file_offset)
    crc_buff = write_hphead_to_file(PAT_NEW.new_patch_hphead)
    ret, crc_new = make_crc(crc_buff[14:], crc)
    if not ret:
        return ret, 0
    PAT_NEW.new_patch_hphead['HPCrc'] = crc_new
    # 写ShareDataLength
    ret = write_bytes_to_file(file_offset, write_hphead_to_file(PAT_NEW.new_patch_hphead))
    if ret == 0:
        logging.error("补丁内容写入失败，写入字节数为:[%d] ", ret)
        return ret, 0

    return True, offset - file_offset


def write_unit_to_hp(offset: int):
    """
    write_unit_to_hp
    :param offset:
    :return:
    """
    temlen = offset
    # 写新pat索引
    temlen += write_bytes_to_file(temlen,
                                  turn_unit_to_hp(PAT_NEW.new_patch_unit))
    return temlen


def make_new_patch():
    """
    make_new_patch
    :return:
    """
    offset = PAT_HEAD_LEN

    # 写pat重定位后的数据
    PAT_NEW.new_patch_unit["UnitOffset"] = offset
    ret, tmp_size = write_static_patch(offset)
    if not ret:
        return ret
    PAT_NEW.new_patch_unit["UnitSize"] = tmp_size
    offset += tmp_size

    # 写索引表, 索引表的位置
    PAT_NEW.pat_header['IndexOffset'] = offset
    ret = write_unit_to_hp(offset)
    if ret == 0:
        logging.error("内容写入失败，写入字节数为:[%d] ", ret)
        return ret

    # 写文件头
    PAT_NEW.pat_header['PatchUnitNum'] = 1
    # LiteOS独立补丁,仅包含一个补丁单元
    PAT_NEW.pat_header['EntryNum'] = 1

    # 制作pack_crc
    OUT_PAT_FILE_HANDLE.seek(PAT_NEW.pat_header.get('IndexOffset'))
    ret, pack_crc = make_crc(OUT_PAT_FILE_HANDLE.read(), 0)
    if not ret:
        return ret
    buf = write_head_to_file(PAT_NEW.pat_header)
    # 'PatchCRC'之后的crc制作，之前的长度为6。
    ret, pack_crc_new = make_crc(buf[6:], pack_crc)
    if not ret:
        return ret
    PAT_NEW.pat_header['PatchCRC'] = pack_crc_new

    result = write_bytes_to_file(0, write_head_to_file(PAT_NEW.pat_header))
    if result == 0:
        logging.error("pat_header内容写入失败，写入字节数为:[%d] ", result)
        return result
    return True


if __name__ == "__main__":
    if len(sys.argv) != 4:
        logging.error("[error ] need argv error!")
        sys.exit(1)
    PAT_OBJ_FILE = sys.argv[1]
    PAT_CONFIG_FILE = sys.argv[2]
    OUTPUT_DIR = sys.argv[3]

    if not os.path.exists(PAT_CONFIG_FILE):
        logging.error("[error  ] Don't has %s!!", PAT_CONFIG_FILE)
        sys.exit(1)
    if not os.path.exists(PAT_OBJ_FILE):
        logging.error("[error  ] Don't has %s!!", PAT_OBJ_FILE)
        sys.exit(1)

    # 初始化全局变量
    BASICINFO, OUTFILE = getconfig(PAT_CONFIG_FILE)
    G_CUR_PATCH_NUM = int(BASICINFO['DefaultPatchNo'.lower()])
    with codecs.open(PAT_OBJ_FILE, 'rb') as ELF_FILE_HANDLE:
        # 解析输入的elf可重定位文件
        ELF = ElfInfo(ELF_FILE_HANDLE)
        if ELF.header['e_machine'] != ELF_EM_AARCH64: # elf文件机器码校验
            logging.error("[error  ] This object file is not armv8 arch!!![%s] ", PAT_OBJ_FILE)
            sys.exit(1)
        with codecs.open(os.path.join(BASICINFO['SymFileBase'.lower()]), 'rb') as SYM_ELF_FILE_HANDLE:
            # 解析可执行镜像
            ELF_SYM = ElfInfo(SYM_ELF_FILE_HANDLE)
            ELF_FILE_HANDLE.seek(0)

            # 基础bin中的符号统计，用于拦截对重名静态函数打补丁的场景
            for sym_info in ELF_SYM.symtab:
                if sym_info["name"] not in ELF_SYM_HASH:
                    ELF_SYM_HASH[sym_info["name"]] = 1
                else:
                    ELF_SYM_HASH[sym_info["name"]] += 1

            # 读取elf可重定位文件到内存，用于重定向
            ELF_BUFF = bytearray(ELF_FILE_HANDLE.read())

            # 创建输出文件
            OUT_PAT_FILE = os.path.join(OUTPUT_DIR,
                                        "{0}SP{1:0>4d}.pat".format(
                                            BASICINFO['PatchFilePre'.lower()],
                                            G_CUR_PATCH_NUM))
            OUT_ELF_FILE = os.path.join(OUTPUT_DIR,
                                        "{0}SP{1:0>4d}.o".format(
                                            BASICINFO['PatchFilePre'.lower()],
                                            G_CUR_PATCH_NUM))
            OUT_INFO_FILE = os.path.join(OUTPUT_DIR,
                                        "{0}SP{1:0>4d}.info".format(
                                            BASICINFO['PatchFilePre'.lower()],
                                            G_CUR_PATCH_NUM))
            if not os.path.exists(OUTPUT_DIR):
                os.mkdir(OUTPUT_DIR)
            if os.path.exists(OUT_PAT_FILE):
                os.remove(OUT_PAT_FILE)
            if os.path.exists(OUT_ELF_FILE):
                os.remove(OUT_ELF_FILE)
            if os.path.exists(OUT_INFO_FILE):
                os.remove(OUT_INFO_FILE)
            with os.fdopen(os.open(OUT_PAT_FILE, os.O_RDWR | os.O_CREAT, 0o644), "wb+") as OUT_PAT_FILE_HANDLE:
                # 初始化文件头，pat索引
                PAT_HEAD_RET = init_pat_head()
                if not PAT_HEAD_RET:
                    logging.error("初始化pat_head文件头失败")
                    sys.exit(1)
                HP_HEAD_RET = init_pat_hphead()
                if not HP_HEAD_RET:
                    logging.error("初始化hp_head文件头失败")
                    sys.exit(1)
                HP_UNIT_RET = init_pat_hpunit()
                if not HP_UNIT_RET:
                    logging.error("初始化hp_unit文件头失败")
                    sys.exit(1)
                with os.fdopen(os.open(OUT_ELF_FILE, os.O_RDWR | os.O_CREAT, 0o644), "wb+") as OUT_ELF_FILE_HANDLE:
                    # 处理可重定向段
                    HANDLER_RET = handle_patch_area()
                    if not HANDLER_RET:
                        sys.exit(1)
                    HANDLER_RET = handle_linkablefile()
                    if not HANDLER_RET:
                        sys.exit(1)

                # 向新pat文件中输出二进制
                PATCH_RET = make_new_patch()
                if not PATCH_RET:
                    sys.exit(1)

    # 制作pat后信息写入
    WRITE_RET = write_pat_info_to_file()
    if not WRITE_RET:
        logging.error("pat后信息写入失败，写入字节数为:[%d] ", WRITE_RET)
