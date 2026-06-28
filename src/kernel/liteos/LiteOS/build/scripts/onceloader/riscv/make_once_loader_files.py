#!/usr/bin/env python
# coding=utf-8
# Copyright Huawei Technologies Co., Ltd. 2010-2019. All rights reserved.
# Python2 and Python3 compatible update finished. Date:2020/02/09
'''
Description: riscv onceload Script
'''

from __future__ import unicode_literals
from __future__ import print_function
import logging
import struct
import os
import sys
from io import open
import subprocess
from copy import deepcopy

LOG_FORMAT = '[%(funcName)s:%(lineno)d, %(levelname)s]: %(message)s'
logging.basicConfig(level=logging.INFO, format=LOG_FORMAT)

UINT32_MAX = 0xFFFFFFFF
U_TYPE_OFFSET = 0x800

# 重定位项的定位
AREA_IDLE = 0  # 初始值
AREA_TCM = 1   # 待重定位的符号位于tcm中
AREA_DDR = 2   # 待重定位的符号位于ddr中
AREA_IMG_POS = 3  # 待重定位的符号位于sensorhub.elf image中，且是位置相关的
AREA_IMG = 4   # 待重定位的符号位于sensorhub.elf image中，且是强符号
# AREA_WEAK = 5   待重定位的符号位于sensorhub.elf image中，且是弱符号
AREA_IGNORE = 6  # 该重定位项无需重定位
AREA_IMG_STRLOG = 7 # 待重定位的符号位于strlog 段
AREA_ITCM = 8   # 待重定位的符号位于tcm中
AREA_DTCM = 9   # 待重定位的符号位于tcm中
AREA_DDR_TEXT = 10   # 待重定位的符号位于ddr中
AREA_DDR_DATA = 11   # 待重定位的符号位于ddr中

ELF_LOCAL = 0
ELF_GLOBAL = 1
ELF_WEAK = 2

HEAD = ''
IMG_FILE = None
ELF_FILE = None
IMG_SIZE = 0
ELF_OBJ = None
OUTFILES = []
FILES = []
OBJS = []
NOT_FOUND_SYM = []
IS_SAFE_KERNEL = False
CODE_IS_SEPARATION = False

RELOC_IN_SCRIPTS = 0
RELOC_IN_RUNTIME = 1


def replace_uint32(string, uint32, index):
    '''
    替换4字节数据
    '''
    string = list(string)
    string[index] = struct.pack('B', (uint32 & 0xFF)).decode("Latin-1")
    string[index + 1] = struct.pack('B', ((uint32 >> 8) & 0xFF)).decode("Latin-1")
    string[index + 2] = struct.pack('B', ((uint32 >> 16) & 0xFF)).decode("Latin-1")
    string[index + 3] = struct.pack('B', ((uint32 >> 24) & 0xFF)).decode("Latin-1")
    return ''.join(string)


def replace_uint16(string, uint16, index):
    '''
    替换2字节数据
    '''
    string = list(string)
    string[index] = struct.pack('B', (uint16 & 0xFF)).decode("Latin-1")
    string[index + 1] = struct.pack('B', ((uint16 >> 8) & 0xFF)).decode("Latin-1")
    return ''.join(string)


def little_int32(context, offset):
    '''
    读取小端4字节有符号数据
    '''
    return struct.unpack('<l', context[offset: offset + 4].encode("Latin-1"))[0], offset + 4


def little_uint32(context, offset):
    '''
    读取小端4字节无符号数据
    '''
    return struct.unpack('<L', context[offset: offset + 4].encode("Latin-1"))[0], offset + 4


def little_uint16(context, offset):
    '''
    读取小端2字节无符号数据
    '''
    return struct.unpack('<H', context[offset: offset + 2].encode("Latin-1"))[0], offset + 2


def little_uint8(context, offset):
    '''
    读取小端1字节无符号数据
    '''
    return struct.unpack('<B', context[offset: offset + 1].encode("Latin-1"))[0], offset + 1


def little_str(context, offset, str_len):
    '''
    读取str_len长度的字符串并更新字符串下标
    '''
    return context[offset: offset + str_len], offset + str_len


def clear_bits_area(data, start_bit, bit_wide, max_val, var_len):
    '''
    清除指定区域的bit
    '''
    data = data & ~((max_val >> (var_len - bit_wide)) << start_bit)
    return data


def get_bits_area(data, start_bit, bit_wide, max_val, var_len):
    '''
    获取指定区域的bit
    '''
    data = data & ((max_val >> (var_len - bit_wide)) << start_bit)
    return data


def ld_move_bits_area16(imm, mcode, bit_from, bit_to, width):
    '''
    移动imm指定长度区域至mcode指定长度区域（2字节）
    '''
    mcode = clear_bits_area(mcode, bit_to, width, 0xFFFF, 16)
    if bit_from < bit_to:
        mcode = mcode | (get_bits_area(imm, bit_from, width, 0xFFFF, 16) << (bit_to - bit_from))
    else:
        mcode = mcode | (get_bits_area(imm, bit_from, width, 0xFFFF, 16) >> (bit_from - bit_to))
    return mcode


def ld_move_bits_area32(imm, mcode, bit_from, bit_to, width):
    '''
    移动imm指定长度区域至mcode指定长度区域（4字节）
    '''
    mcode = clear_bits_area(mcode, bit_to, width, 0xFFFFFFFF, 32)
    if bit_from < bit_to:
        mcode = mcode | (get_bits_area(imm, bit_from, width, 0xFFFFFFFF, 32) << (bit_to - bit_from))
    else:
        mcode = mcode | (get_bits_area(imm, bit_from, width, 0xFFFFFFFF, 32) >> (bit_from - bit_to))
    return mcode


def str_to_bool(value):
    '''
    Converting Customized Strings to Bool.
    '''
    if value.lower() == "true":
        return True
    return False


class DynMapInfo:
    '''
    struct dyn_map {
        UINT64 offset:16;
        UINT64 file_idx:8;
        UINT64 value:32;
        UINT64 info:6;
        UINT64 type:2;
    };

    struct dyn_map {
        UINT64 offset:16;
        UINT64 file_idx:8;
        UINT64 value:32;
        UINT64 info:8;
        UINT8  type;
    };
    '''
    OFFSET_WIDTH = 16
    FILE_IDX_WIDTH = 8
    VALUE_WIDTH = 32
    INFO_WIDTH = 6
    TYPE_WIDTH = 2

    OFFSET_SHIFT = 0
    FILE_IDX_SHIFT = 16
    VALUE_SHIFT = 24
    INFO_SHIFT = 56
    TYPE_SHIFT = 62


class SymbolTable:
    '''
    symbal table
    name, name_str, value, size, info, other, shndx
    name :符号名。这个成员包含了该符号在字符串表中的下标
    value:符号对应的值，可能是一个绝对值也可能是一个地址等，不同的符号，对应的值含义不同。如果符号是一个函数或者变量的定义，那么就是函数或者变量的地址
          符号该符号不是common块的类型则value表示该符号在段中的偏移
    size:符号大小，对于包含数据的符号，这个值是该数据类型的大小，比如一个double类型的符号，它占用0字节，如果是0表示大小为0或者未知
    info:符号类型和绑定信息
    other:目前为0，无用
    shndx:符号所在的段
    '''
    def __init__(self, context):
        if len(context) < 16:
            raise Exception('ELF file\'s symtable is error')
        offset = 0
        self.st_name, offset = little_uint32(context, offset)
        self.st_name_str = ''
        self.st_value, offset = little_uint32(context, offset)
        self.st_size, offset = little_uint32(context, offset)
        self.st_info, offset = little_uint8(context, offset)
        self.st_other, offset = little_uint8(context, offset)
        self.st_shndx, offset = little_uint16(context, offset)


class ElfHeader:
    '''
    Parses ELF Head
    ident, type, machine, version, entry, phoff, shoff, flags, ehsize, phentsize, phum, shentsize, shum, shstrndx
    '''
    def __init__(self, context):
        if len(context) < 52:
            raise Exception('ELF file\'s head is error')
        offset = 0
        self.e_ident, offset = little_str(context, offset, 16)
        self.e_type, offset = little_uint16(context, offset)
        self.e_machine, offset = little_uint16(context, offset)
        self.e_version, offset = little_uint32(context, offset)
        self.e_entry, offset = little_uint32(context, offset)
        self.e_phoff, offset = little_uint32(context, offset)
        self.e_shoff, offset = little_uint32(context, offset)      # section信息的起始偏移
        self.e_flags, offset = little_uint32(context, offset)
        self.e_ehsize, offset = little_uint16(context, offset)
        self.e_phentsize, offset = little_uint16(context, offset)
        self.e_phum, offset = little_uint16(context, offset)
        self.e_shentsize, offset = little_uint16(context, offset)
        self.e_shum, offset = little_uint16(context, offset)       # section个数
        self.e_shstrndx, offset = little_uint16(context, offset)   # section名称的字符串段索引
        logging.debug(self.__dict__)


class ElfSection:
    '''
    Parses ELF Section
    name, name_str, type, flags, addr, offset, size, link, info, addralign, entsize
    '''
    def __init__(self, context):
        if len(context) < 40:
            raise Exception('ELF file\'s section is error')
        offset = 0
        self.sh_index = 0
        self.sh_name, offset = little_uint32(context, offset)
        self.sh_name_str = ''
        self.sh_type, offset = little_uint32(context, offset)
        self.sh_flags, offset = little_uint32(context, offset)
        self.sh_addr, offset = little_uint32(context, offset)
        self.sh_offset, offset = little_uint32(context, offset)
        self.sh_size, offset = little_uint32(context, offset)
        self.sh_link, offset = little_uint32(context, offset)
        self.sh_info, offset = little_uint32(context, offset)
        self.sh_addralign, offset = little_uint32(context, offset)
        self.sh_entsize, offset = little_uint32(context, offset)
        self.sh_context = ''
        logging.debug(self.__dict__)

    def add_context(self, context):
        '''
        Append Context
        '''
        if self.sh_type == 8:  # 8表示该段无实际内容 NOBITS
            self.sh_context = '\0' * self.sh_size
        else:
            start = self.sh_offset
            end = start + self.sh_size
            self.sh_context = context[start:end]

    def add_name(self, shstr_context):
        '''
        add name
        '''
        self.sh_name_str = shstr_context[self.sh_name:].split('\0')[0]


class ElfFile:
    '''
    ELF文件类
    '''
    def __init__(self, filepath):
        self.file_path = filepath
        with open(filepath, 'rb') as elf_file:
            context = elf_file.read().decode("Latin-1")
            elf_file.close()
        #解析ELF头
        self.head = ElfHeader(context)
        #先获取全量section
        self.parse_sections(context)
        #再解析其中的符号表
        self.parse_base_sym()
        logging.debug(self.__dict__)


    def parse_sections(self, context):
        '''
        解析单个文件的所有section
        '''
        self.sections = []

        # 解析每个section
        section_entry_size = 40
        self.shstr_context = ''
        for index in range(self.head.e_shum):
            start = self.head.e_shoff + index * section_entry_size
            end = start + section_entry_size
            sec_info = ElfSection(context[start: end])
            sec_info.sh_index = index
            # 保存每个section的内容
            sec_info.add_context(context)
            self.sections.append(sec_info)
            if index == self.head.e_shstrndx:
                self.shstr_context = sec_info.sh_context

        for sec_info in self.sections:
            # 添加每个section的描述名
            sec_info.add_name(self.shstr_context)

            # 每个section对齐
            if sec_info.sh_type == 1 and '.text' in sec_info.sh_name_str: # 1表示PROGBITS 程序段
                logging.debug(sec_info.__dict__)
                if sec_info.sh_size % 4 == 2:
                    # add nop_s instruction after text section, which is not 4 byte aligned
                    sec_info.sh_context += struct.pack('H', 0x0001).decode("Latin-1")
                    sec_info.sh_size += 2
            elif sec_info.sh_size % 4 != 0:
                sec_info.sh_context += '\0' * (4 - sec_info.sh_size % 4)
                sec_info.sh_size += 4 - sec_info.sh_size % 4


    def parse_sym_section(self, symtab, strtab):
        '''
        解析单个文件的符号表
        '''
        # 查找.symtab段和.strtab，并获取该段的内容信息
        symbol_table = None
        string_table = None
        logstr_sec = None
        for sec_info in self.sections:
            if sec_info.sh_name_str.startswith(symtab):
                symbol_table = sec_info
            if sec_info.sh_name_str.startswith(strtab):
                string_table = sec_info
            if sec_info.sh_name_str.startswith('.logstr'):
                logstr_sec = sec_info

        if symbol_table is not None and string_table is not None:
            one_symbol_size = 16
            symtab = []
            for i in range(symbol_table.sh_size // one_symbol_size):
                symbol_info = SymbolTable(symbol_table.sh_context[i * one_symbol_size: (i + 1) * one_symbol_size])
                idx = string_table.sh_context.find("\0", symbol_info.st_name) # 从下表symbol_info.name开始，查找\0字符，并返回其索引
                if idx == -1:
                    symbol_info.st_name_str = string_table.sh_context[symbol_info.st_name:]
                else:
                    symbol_info.st_name_str = string_table.sh_context[symbol_info.st_name:idx]
                symtab.append(symbol_info)
                logging.debug(symbol_info.__dict__)
        else:
            raise Exception('Symbol table or string table is NULL\n')
        return symtab, logstr_sec

    def parse_base_sym(self):
        '''
        parse section(.symtab, .strtab) sym
        '''
        self.symtab, self.logstr_sec = self.parse_sym_section('.symtab', '.strtab')


class RelEntryInfo():
    '''
    Parses Rel Entry Info
    '''
    def __init__(self, context):
        if len(context) < 12:
            raise Exception('ELF REL section is error\n')
        self.r_sec_name = ''
        self.r_base_offset = 0
        self.r_area = AREA_IDLE
        self.r_name_str = ''
        offset = 0
        self.r_offset, offset = little_uint32(context, offset)
        self.r_info, offset = little_uint32(context, offset)
        self.r_addend, offset = little_uint32(context, offset)
        self.r_value = 0
        logging.debug(self.__dict__)


class SimpleSection():
    '''
    Get Simple Section info
    '''
    def __init__(self, reoffset, size, name_str):
        self.reoffset = reoffset
        self.size = size
        self.name_str = name_str
        self.index = 0


class SimpleObj():
    '''
    Get Simple Obj info
    '''
    def __init__(self, sections, symtab, speclist):
        self.speclist = speclist
        # 获取指定节信息，并生成其REL节的信息
        self._get_specified_section_info(sections, symtab)


    def parse_rel_table(self, rel_section, symtab, base_offset):
        '''
        解析单个REL表
        '''
        rel_entry_size = 12
        for i in range(rel_section.sh_size // rel_entry_size):
            onerel = RelEntryInfo(rel_section.sh_context[i * rel_entry_size : (i + 1) * rel_entry_size])
            if onerel.r_info != 0:
                # 添加有效的REL信息
                onerel.r_sec_name = rel_section.sh_name_str
                onerel.r_base_offset = base_offset
                onerel.r_offset += base_offset
                onerel.r_area = AREA_IDLE
                onerel.r_name_str = symtab[onerel.r_info >> 8].st_name_str
                self.reltable.append(onerel)
                logging.debug(onerel.__dict__)

    # 获取指定的节信息，以及其REL信息
    def _get_specified_section_info(self, sections, symtab):
        reset_offset = 0
        self.reltable = []
        self.simplist = []
        self.new_context = ''
        self.sec_reoffset_dict = {}
        for key in self.speclist:
            for index, sec_info in enumerate(sections):
                if not sec_info.sh_name_str.startswith(key):
                    continue
                # 记录需要保存的section
                simple_section = SimpleSection(reset_offset, sec_info.sh_size, sec_info.sh_name_str)
                simple_section.index = index
                self.sec_reoffset_dict[simple_section.index] = simple_section.reoffset
                self.simplist.append(simple_section)
                # 一般RELA节都在对应section的后一个
                if index + 1 < (len(sections)) and \
                    sections[index + 1].sh_name_str.startswith(''.join(('.rela', sec_info.sh_name_str))):
                    self.parse_rel_table(sections[index + 1], symtab, reset_offset)
                # 拼接待输出的二进制数据
                if sec_info.sh_name_str.endswith('.bss'):
                    logging.debug(sec_info.__dict__)
                    self.new_context += sec_info.sh_size * '\0'
                else:
                    self.new_context += sec_info.sh_context
                # 计算简化后的节偏移位置
                reset_offset += sec_info.sh_size


reloc_type_dict = {
    1 : 'R_RISCV_32',
    16 : 'R_RISCV_BRANCH',
    17 : 'R_RISCV_JAL',
    18 : 'R_RISCV_CALL',
    19 : 'R_RISCV_CALL_PLT',
    26 : 'R_RISCV_HI20',
    27 : 'R_RISCV_LO12_I',
    28 : 'R_RISCV_LO12_S',
    33 : 'R_RISCV_ADD8',
    34 : 'R_RISCV_ADD16',
    37 : 'R_RISCV_SUB8',
    38 : 'R_RISCV_SUB16',
    44 : 'R_RISCV_RVC_BRANCH',
    45 : 'R_RISCV_RVC_JUMP',
    51 : 'R_RISCV_RELAX',
    59 : 'R_RISCV_GP_HI20',
    60 : 'R_RISCV_GP_LO12_I',
    61 : 'R_RISCV_GP_LO12_S',
    67 : 'R_RISCV_BEQ_X',
    68 : 'R_RISCV_BEQI_YJ',
}

no_reloc_type_list = {
    16, # 'R_RISCV_BRANCH'
    17, # 'R_RISCV_JAL'
    44, # 'R_RISCV_RVC_BRANCH'
    45, # 'R_RISCV_RVC_JUMP'
    51, # 'R_RISCV_RELAX'
    67, # 'R_RISCV_BEQ_X'
    68, # 'R_RISCV_BEQI_YJ'
}


def get_relocation_type(info):
    '''
    Get Relocation Type
    '''
    key = info & 0xFF
    return reloc_type_dict.get(key)


def run_shell_return(command):
    '''
    Run Shell Command, And Return Result
    '''
    pipe_handle = subprocess.Popen(command, stdout=subprocess.PIPE, \
            stderr=subprocess.PIPE, shell=False, universal_newlines=True)
    output = pipe_handle.stdout.read()
    pipe_handle.wait()
    errout = pipe_handle.stderr.read()
    if errout:
        logging.info(errout)
    pipe_handle.stdout.close()
    pipe_handle.stderr.close()
    return output, pipe_handle.returncode


class ObjFile(ElfFile):
    '''
    Parses Obj File
    '''
    def __init__(self, filepath):
        ElfFile.__init__(self, filepath)
        self.reloc_obj_dict = {}

        if CODE_IS_SEPARATION:
            self.fileid = 0xFFFF
        else:
            self.fileid = 0xFFFFFFFF

        self.initflag = 0xFFFF # 表示启动类型：APP(1) or Sensor(0)；需先调用Sensor的初始化函数，再调用APP的初始化函数
        self.itcm = None
        self.dtcm = None
        self.ddr_text = None
        self.ddr_data = None
        self.tcm = None
        self.ddr = None

        # 0:drv box :peripheral_initcall
        # 1:APP BOX :sensor_init + app_init
        # 2:AI BOX
        # 3:AOD BOX
        if IS_SAFE_KERNEL:
            cmd = ['grep', '-q', 'peripheral_initcall', filepath]
            _, return_code = run_shell_return(cmd)
            self.boxid = 0 if return_code == 0 else 1
        else:
            self.boxid = 0

    def init_simple_section_list(self, code_is_separation):
        '''
        Get Tcm/Ddr Section
        '''
        self._get_tcm_section_list(code_is_separation)
        self._get_ddr_section_list(code_is_separation)

    def relocate_elf_symbol(self, code_is_separation):
        '''
        Relocate Elf Symbol Tab
        '''
        if not code_is_separation:
            self._replace_elf_symbol(self.tcm)
            self._replace_elf_symbol(self.ddr)
            return
        self._replace_elf_symbol(self.itcm)
        self._replace_elf_symbol(self.dtcm)
        self._replace_elf_symbol(self.ddr_text)
        self._replace_elf_symbol(self.ddr_data)


    def relocate_local_symbol(self, code_is_separation):
        '''
        Relocate Local Symbol Tab
        '''
        if not code_is_separation:
            self._replace_obj_symbol(self.tcm)
            self._replace_obj_symbol(self.ddr)
            return
        self._replace_obj_symbol(self.itcm)
        self._replace_obj_symbol(self.dtcm)
        self._replace_obj_symbol(self.ddr_text)
        self._replace_obj_symbol(self.ddr_data)

    def set_initflag_and_file_id(self):
        '''
        Set The Initialization Flag And File ID.
        '''
        for sec_info in self.sections:
            if sec_info.sh_name_str.startswith('.dyn_app_initcall') or \
               sec_info.sh_name_str.startswith('.dyn_sensor_initcall') or \
               sec_info.sh_name_str.startswith('.dyn_peripheral_initcall'):
                if sec_info.sh_name_str.startswith('.dyn_app_initcall'):
                    self.initflag = 1
                else:
                    self.initflag = 0
                self.fileid = struct.unpack('<L',
                    sec_info.sh_context[4:8].encode("Latin-1"))[0]

    def adjust_elf_symbol(self, elf_dic):
        '''
        Update Elf Symbol Value
        '''
        logstr_sym = None
        for sym in self.symtab:
            if sym.st_shndx < len(self.sections) and \
                self.sections[sym.st_shndx].sh_name_str.startswith('.logstr'):
                logstr_sym = sym
                logging.debug(logstr_sym.__dict__)
                break
        for sym in self.symtab:
            bind = ELF_LOCAL
            # 高28表示绑定信息，值为1，即为GLOBAL， 低4位表示符号类型，值为0，即为NOTYPE。shndx=0表示该符号未定义
            if sym.st_info >> 4 == 1 and sym.st_shndx == 0:
                logging.debug(sym.__dict__)
                # 去elf中查找 global属性对应的符号
                value, bind = find_sym_in_elf(sym.st_name_str, ELF_GLOBAL, elf_dic)
                if value == 0:
                    # 去elf中查找 WEAK属性对应的符号
                    value, bind = find_sym_in_elf(sym.st_name_str, ELF_WEAK, elf_dic)
            # 或者是.logstr节的变量, 且高28位的绑定信息部位SECTION，即3
            # .logstr节并不在tcm或者ddr 这两种simple section List中
            # 将obj中的字典重定位到elf
            elif (logstr_sym is not None and sym.st_shndx == logstr_sym.st_shndx and \
                sym.st_info & 0xF != 3):
                logging.debug(sym.__dict__)
                value, bind = find_sym_in_elf(sym.st_name_str, ELF_LOCAL, elf_dic)
            else:
                value = 0
            if value != 0:
                if logstr_sym is not None and sym.st_shndx == logstr_sym.st_shndx and logstr_sym.st_value == 0:
                    # 沒看懂这个分支的含义
                    logstr_sym.st_value = value - sym.st_value
                    logging.debug(logstr_sym.__dict__)
                sym.st_value = value
            sym.st_img_bind = bind

    def adjust_obj_symbol(self, code_is_separation):
        '''
        Update Obj Symbol Value
        '''
        for sym in self.symtab:
            # 绑定类型是否为有效值
            if sym.st_info >> 4 != 0 and sym.st_info >> 4 != 1 and sym.st_info >> 4 != 2:
                continue
            if not code_is_separation:
                if sym.st_shndx in self.tcm.sec_reoffset_dict:
                    sym.st_value += self.tcm.sec_reoffset_dict[sym.st_shndx]
                elif sym.st_shndx in self.ddr.sec_reoffset_dict:
                    sym.st_value += self.ddr.sec_reoffset_dict[sym.st_shndx]
            else:
                if sym.st_shndx in self.itcm.sec_reoffset_dict:
                    sym.st_value += self.itcm.sec_reoffset_dict[sym.st_shndx]
                elif sym.st_shndx in self.dtcm.sec_reoffset_dict:
                    sym.st_value += self.dtcm.sec_reoffset_dict[sym.st_shndx]
                elif sym.st_shndx in self.ddr_text.sec_reoffset_dict:
                    sym.st_value += self.ddr_text.sec_reoffset_dict[sym.st_shndx]
                elif sym.st_shndx in self.ddr_data.sec_reoffset_dict:
                    sym.st_value += self.ddr_data.sec_reoffset_dict[sym.st_shndx]


    def get_link_global_sym(self, sym_list):
        '''
        Get Notype sym
        '''
        for sym in self.symtab:
            # 高28表示绑定信息，值为1，即为GLOBAL， 低4位表示符号类型，值为0，即为NOTYPE
            if sym.st_info == 0x10:
                sym_list.append(sym.st_name_str)

    def _get_tcm_section_list(self, code_is_separation):
        if not code_is_separation:
            self.tcm = SimpleObj(self.sections, self.symtab, ['.text', '.data', '.rodata', '.bss', '.sdata', '.sbss'])
        else:
            self.itcm = SimpleObj(self.sections, self.symtab, ['.text'])
            self.dtcm = SimpleObj(self.sections, self.symtab, ['.data', '.rodata', '.bss', '.sdata', '.sbss'])

    def _get_ddr_section_list(self, code_is_separation):
        if not code_is_separation:
            self.ddr = SimpleObj(self.sections, self.symtab, \
                ['.dyn_', '.ddr.text', '.ddr.data', '.ddr.rodata', '.ddr.bss'])
        else:
            self.ddr_text = SimpleObj(self.sections, self.symtab, ['.ddr.text'])
            self.ddr_data = SimpleObj(self.sections, self.symtab, ['.dyn_', '.ddr.data', '.ddr.rodata', '.ddr.bss'])

    def _replace_elf_symbol(self, simp_obj):
        for item in simp_obj.reltable:
            # 符号表中的绑定信息为GLOBAL, 且不在本.o文件定义的符号
            if self.symtab[item.r_info >> 8].st_info >> 4 == 1 and self.symtab[item.r_info >> 8].st_shndx == 0:
                # 对于本.o文件的未定义符号，如果并未在elf中找到任何实现，包括弱函数，那么img_bind就是ELF_LOCAL
                if self.symtab[item.r_info >> 8].st_img_bind == ELF_LOCAL:
                    continue
                func = self.reloc_obj_dict.get(item.r_info & 0xFF)
                if func is not None:
                    func(simp_obj, item, is_area_img_sym=1)
                else:
                    raise Exception('File({}), Section({}), Variable({}) is {} type'.format(
                        self.file_path.rsplit('\\')[-1], \
                        self.sections[self.symtab[item.r_info >> 8].st_shndx].sh_name_str,
                        self.symtab[item.r_info >> 8].st_name_str, item.r_info & 0xFF))

            # 或者是LOGSTR开头的静态全局变量，虽然是LOCAL属性，但是其链接在elf中。
            elif self.sections[self.symtab[item.r_info >> 8].st_shndx].sh_name_str.startswith('.logstr'):
                func = self.reloc_obj_dict.get(item.r_info & 0xFF)
                if func is not None:
                    func(simp_obj, item, is_area_img_sym=1)
                else:
                    raise Exception('File({}), Section({}), Variable({}) is {} type'.format(
                        self.file_path.rsplit('\\')[-1], \
                        self.sections[self.symtab[item.r_info >> 8].st_shndx].sh_name_str,
                        self.symtab[item.r_info >> 8].st_name_str, item.r_info & 0xFF))

    def _replace_obj_symbol(self, simp_obj):
        for item in simp_obj.reltable:
            # 如果已经全局变量符号调整过，则不做处理
            if item.r_area != AREA_IDLE:
                continue
            func = self.reloc_obj_dict.get(item.r_info & 0xFF)
            if func is not None:
                func(simp_obj, item, is_area_img_sym=0)
            else:
                raise Exception('File({}), Section({}), Variable({}) is {} type'.format(
                    self.file_path.rsplit('\\')[-1], self.sections[self.symtab[item.r_info >> 8].st_shndx].sh_name_str,
                    self.symtab[item.r_info >> 8].st_name_str, item.r_info & 0xFF))

    def _set_rel_item_area(self, item, is_area_img_sym, dyn_reloc_stage, is_relative_jump):
        if item.r_area != AREA_IDLE:
            return
        if dyn_reloc_stage == RELOC_IN_SCRIPTS:
            item.r_area = AREA_IGNORE
            return
        if is_area_img_sym == 1:
            if is_relative_jump == 1:
                item.r_area = AREA_IMG_POS
                return
            # ELF_WEAK : 对于__stack_chk_fail，elf符号表，有定义和weak属性，但在.o中是未定义和Global属性
            if self.symtab[item.r_info >> 8].st_img_bind in (ELF_GLOBAL, ELF_WEAK):
                item.r_area = AREA_IMG
            # 此情况只有strlog段中的符号
            elif self.symtab[item.r_info >> 8].st_img_bind == ELF_LOCAL and \
                self.sections[self.symtab[item.r_info >> 8].st_shndx].sh_name_str.startswith('.logstr'):
                item.r_area = AREA_IMG_STRLOG
            else:
                raise Exception('Wrong symbol bind property:{}'.format(item.__dict__))
            return
        # 若在tcm的simplist有shndx，说明符号在tcm中
        if self.tcm and self.symtab[item.r_info >> 8].st_shndx in self.tcm.sec_reoffset_dict:
            item.r_area = AREA_TCM
        # 否则在ddr中
        elif self.ddr and self.symtab[item.r_info >> 8].st_shndx in self.ddr.sec_reoffset_dict:
            item.r_area = AREA_DDR
        # 若在tcm的simplist有shndx，说明符号在tcm中
        elif self.itcm and self.symtab[item.r_info >> 8].st_shndx in self.itcm.sec_reoffset_dict:
            item.r_area = AREA_ITCM
        elif self.dtcm and self.symtab[item.r_info >> 8].st_shndx in self.dtcm.sec_reoffset_dict:
            item.r_area = AREA_DTCM
        # 否则在ddr中
        elif self.ddr_text and self.symtab[item.r_info >> 8].st_shndx in self.ddr_text.sec_reoffset_dict:
            item.r_area = AREA_DDR_TEXT
        elif self.ddr_data and self.symtab[item.r_info >> 8].st_shndx in self.ddr_data.sec_reoffset_dict:
            item.r_area = AREA_DDR_DATA
        else:
            logging.debug(item.__dict__)

    def _set_rel_item_value(self, item, dyn_reloc_stage):
        item.r_value = self.symtab[item.r_info >> 8].st_value + item.r_addend
        if dyn_reloc_stage == RELOC_IN_SCRIPTS:
            item.r_value = item.r_value - item.r_offset


class ArchObjFile(ObjFile):
    '''
    Architecture-related relocation methods
    '''
    def __init__(self, filepath):
        ObjFile.__init__(self, filepath)
        self.reloc_obj_dict = {
            1  : self.relocate_riscv32,    # R_RISCV_32
            16 : self.relocate_branch,     # R_RISCV_BRANCH
            17 : self.relocate_rvc_jal,    # R_RISCV_JAL
            18 : self.relocate_call,       # R_RISCV_CALL
            19 : self.relocate_call,       # R_RISCV_CALL_PLT
            26 : self.relocate_hi20,       # R_RISCV_HI20
            27 : self.relocate_lo12i,      # R_RISCV_LO12_I
            28 : self.relocate_lo12s,      # OS_R_RISCV_LO12_S
            33 : self.relocate_add8,       # R_RISCV_ADD8
            34 : self.relocate_add16,      # R_RISCV_ADD16
            37 : self.relocate_sub8,       # R_RISCV_SUB8
            38 : self.relocate_sub16,      # R_RISCV_SUB16
            44 : self.relocate_rvc_branch, # R_RISCV_RVC_BRANCH
            45 : self.relocate_rvc_jump,   # R_RISCV_RVC_JUMP
            51 : self.relocate_relax,      # R_RISCV_RELAX
            61 : self.relocate_hi20,       # R_RISCV_GP_HI20
            62 : self.relocate_lo12i,      # R_RISCV_GP_LO12_I
            63 : self.relocate_lo12s,      # R_RISCV_GP_LO12_S
            66 : self.relocate_beqi,       # R_RISCV_BEQI_L
            71 : self.relocate_branch      # R_RISCV_BEQ_GB
        }

    # linx170 OBJ file
    def relocate_sub8(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_SUB8
        '''
        self._set_rel_item_value(item, dyn_reloc_stage=RELOC_IN_RUNTIME)
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_RUNTIME, is_relative_jump=0)

    def relocate_sub16(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_SUB16
        '''
        self._set_rel_item_value(item, dyn_reloc_stage=RELOC_IN_RUNTIME)
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_RUNTIME, is_relative_jump=0)


    def relocate_add8(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_ADD8
        '''
        self._set_rel_item_value(item, dyn_reloc_stage=RELOC_IN_RUNTIME)
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_RUNTIME, is_relative_jump=0)

    def relocate_add16(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_ADD16
        '''
        self._set_rel_item_value(item, dyn_reloc_stage=RELOC_IN_RUNTIME)
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_RUNTIME, is_relative_jump=0)


    def relocate_riscv32(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_32
        '''
        self._set_rel_item_value(item, dyn_reloc_stage=RELOC_IN_RUNTIME)
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_RUNTIME, is_relative_jump=0)

    def relocate_hi20(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_HI20
        '''
        self._set_rel_item_value(item, dyn_reloc_stage=RELOC_IN_RUNTIME)
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_RUNTIME, is_relative_jump=0)
        if is_area_img_sym == 0:
            return
        # lui(U-type):
        # |31        12|11    7|6     0
        # |imm[31:12]  |rd     |0110111
        imm = item.r_value + U_TYPE_OFFSET
        instruct_before, _ = little_uint32(simp_obj.new_context, item.r_offset)
        instruction = ld_move_bits_area32(imm, instruct_before, 12, 12, 20)
        simp_obj.new_context = replace_uint32(simp_obj.new_context, instruction, item.r_offset)

    def relocate_lo12i(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_LO12_I
        '''
        self._set_rel_item_value(item, dyn_reloc_stage=RELOC_IN_RUNTIME)
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_RUNTIME, is_relative_jump=0)
        if is_area_img_sym == 0:
            return
        # addi/ori/slti/andi (I-Type):
        # |31                        20|19    |14     |11      |6      0
        # |imm[11:0]                   |rs1   |funct3 |rd      |0010011
        imm = int(-(((item.r_value + U_TYPE_OFFSET) & 0xfffff000) - item.r_value))
        instruct_before, _ = little_uint32(simp_obj.new_context, item.r_offset)
        instruction = ld_move_bits_area32(imm, instruct_before, 0, 20, 12)
        simp_obj.new_context = replace_uint32(simp_obj.new_context, instruction, item.r_offset)

    def relocate_lo12s(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_LO12_S
        '''
        self._set_rel_item_value(item, dyn_reloc_stage=RELOC_IN_RUNTIME)
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_RUNTIME, is_relative_jump=0)
        if is_area_img_sym == 0:
            return
        # sw/sh/sb (S-Type):
        # |31      25|24      |19       |14     |11      7|6     0
        # |imm[11:5] |rs2     |rs1      |funct3 |imm[4:0] |0100011
        imm = int(-(((item.r_value + U_TYPE_OFFSET) & 0xfffff000) - item.r_value))
        instruct_before, _ = little_uint32(simp_obj.new_context, item.r_offset)
        instruction = ld_move_bits_area32(imm, instruct_before, 5, 25, 7)
        instruction = ld_move_bits_area32(imm, instruction, 0, 7, 5)
        simp_obj.new_context = replace_uint32(simp_obj.new_context, instruction, item.r_offset)

    def relocate_call(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_CALL or R_RISCV_CALL_PLT
        '''
        self._set_rel_item_value(item, dyn_reloc_stage=RELOC_IN_RUNTIME)
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_RUNTIME, is_relative_jump=1)

    def relocate_branch(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_BRANCH or R_RISCV_BEQ_X
        '''
        self._set_rel_item_value(item, dyn_reloc_stage=RELOC_IN_SCRIPTS)
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_SCRIPTS, is_relative_jump=1)
        # bne/beq/beqz/bltu/bgeu/blt/bge(B-type):
        # |31      |30      25|24    |19   |14      |11       8|7       |6     0
        # |imm[12] |imm[10:5] |rs2   |rs1  |funct3  |imm[4:1]  |imm[11] |1100011
        imm = item.r_value
        instruct_before, _ = little_uint32(simp_obj.new_context, item.r_offset)
        instruction = ld_move_bits_area32(imm, instruct_before, 12, 31, 1)
        instruction = ld_move_bits_area32(imm, instruction, 5, 25, 6)
        instruction = ld_move_bits_area32(imm, instruction, 1, 8, 4)
        instruction = ld_move_bits_area32(imm, instruction, 11, 7, 1)
        simp_obj.new_context = replace_uint32(simp_obj.new_context, instruction, item.r_offset)
        if instruct_before != instruction:
            raise Exception('wrong relocation context')

    def relocate_rvc_branch(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_RVC_BRANCH
        '''
        self._set_rel_item_value(item, dyn_reloc_stage=RELOC_IN_SCRIPTS)
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_SCRIPTS, is_relative_jump=1)
        # B-type:
        # |15  13|12       10|9   7|6            2|1 0
        # |1 1 0 |imm[8|4:3] |rs1-8|imm[7:6|2:1|5]|0 1
        imm = item.r_value
        instruct_before, _ = little_uint16(simp_obj.new_context, item.r_offset)
        instruction = ld_move_bits_area16(imm, instruct_before, 8, 12, 1)
        instruction = ld_move_bits_area16(imm, instruction, 3, 10, 2)
        instruction = ld_move_bits_area16(imm, instruction, 6, 5, 2)
        instruction = ld_move_bits_area16(imm, instruction, 1, 3, 2)
        instruction = ld_move_bits_area16(imm, instruction, 5, 2, 1)
        simp_obj.new_context = replace_uint16(simp_obj.new_context, instruction, item.r_offset)
        if instruct_before != instruction:
            raise Exception('wrong relocation context')

    def relocate_rvc_jump(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_RVC_JUMP
        '''
        self._set_rel_item_value(item, dyn_reloc_stage=RELOC_IN_SCRIPTS)
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_SCRIPTS, is_relative_jump=1)
        # B-type:
        # |15  13|12                        2|1 0
        # |1 0 1 |imm[11|4|9:8|10|6|7|3:1|5] |0 1
        imm = item.r_value
        instruct_before, _ = little_uint16(simp_obj.new_context, item.r_offset)
        instruction = ld_move_bits_area16(imm, instruct_before, 11, 12, 1)
        instruction = ld_move_bits_area16(imm, instruction, 4, 11, 1)
        instruction = ld_move_bits_area16(imm, instruction, 8, 9, 2)
        instruction = ld_move_bits_area16(imm, instruction, 10, 8, 1)
        instruction = ld_move_bits_area16(imm, instruction, 6, 7, 1)
        instruction = ld_move_bits_area16(imm, instruction, 7, 6, 1)
        instruction = ld_move_bits_area16(imm, instruction, 1, 3, 3)
        instruction = ld_move_bits_area16(imm, instruction, 5, 2, 1)
        simp_obj.new_context = replace_uint16(simp_obj.new_context, instruction, item.r_offset)
        if instruct_before != instruction:
            raise Exception("wrong relocation context")

    def relocate_rvc_jal(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_JAL
        '''
        self._set_rel_item_value(item, dyn_reloc_stage=RELOC_IN_SCRIPTS)
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_SCRIPTS, is_relative_jump=1)
        # jal(J-type):
        # |31                  12|11    7|6     0
        # |imm[20|10:1|11|19:12] |rd     |1101111
        imm = item.r_value
        instruct_before, _ = little_uint32(simp_obj.new_context, item.r_offset)
        instruction = ld_move_bits_area32(imm, instruct_before, 20, 31, 1)
        instruction = ld_move_bits_area32(imm, instruction, 1, 21, 10)
        instruction = ld_move_bits_area32(imm, instruction, 11, 20, 1)
        instruction = ld_move_bits_area32(imm, instruction, 12, 12, 8)
        simp_obj.new_context = replace_uint32(simp_obj.new_context, instruction, item.r_offset)
        if instruct_before != instruction:
            raise Exception("wrong relocation context")

    def relocate_beqi(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_BEQI_YJ
        '''
        # no need to relocate for this type
        self._set_rel_item_value(item, dyn_reloc_stage=RELOC_IN_SCRIPTS)
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_SCRIPTS, is_relative_jump=1)
        # bltui/bgeui/blti(B-type):

    def relocate_relax(self, simp_obj, item, is_area_img_sym):
        '''
        R_RISCV_RELAX
        '''
        # no need to relocate for this type
        self._set_rel_item_area(item, is_area_img_sym, dyn_reloc_stage=RELOC_IN_SCRIPTS, is_relative_jump=0)
        item.r_value = 0


def find_sym_in_file(file, name, sym_bind):
    '''
    Find Sym In Obj File
    '''
    for sym in file.symtab:
        if sym.st_name_str == name and sym.st_shndx != 0:
            if sym.st_info >> 4 == sym_bind:
                return sym.st_value, sym.st_shndx, (sym.st_info >> 4)
    return 0, 0, 0


def quick_search_in_elf_dict(elf_dic, name, sym_bind):
    '''
    Quick Search Using The Elf Dictionary
    '''
    if name in elf_dic:
        logging.debug(name)
        for sym in elf_dic[name]:
            if sym.st_shndx != 0 and sym.st_info >> 4 == sym_bind:
                return sym.st_value, sym.st_shndx, (sym.st_info >> 4)
    return 0, 0, 0


def find_sym_in_elf(name, sym_bind, elf_dic):
    '''
    Find Sym In Elf
    '''
    global ELF_OBJ
    if elf_dic:
        value, _, elf_bind = quick_search_in_elf_dict(elf_dic, name, sym_bind)
    else:
        value, _, elf_bind = find_sym_in_file(ELF_OBJ, name, sym_bind)
    return value, elf_bind


def parse_elf_file():
    '''
    Parse Elf
    '''
    global ELF_OBJ
    ELF_OBJ = ElfFile(ELF_FILE)


def parse_obj_file():
    '''
    Parse Obj
    '''
    global OBJS
    global FILES
    global ELF_OBJ
    elf_dic = {}
    for sym in ELF_OBJ.symtab:
        logging.debug(sym.__dict__)
        if sym.st_name_str in elf_dic:
            elf_dic[sym.st_name_str].append(sym)
        else:
            elf_dic[sym.st_name_str] = [sym]

    for file_path in FILES:
        # 解析一个OBJ文件
        obj = ArchObjFile(file_path)
        # 获取特定节列表
        obj.init_simple_section_list(CODE_IS_SEPARATION)
        #刷新在elf的符号值
        obj.adjust_elf_symbol(elf_dic)
        #刷新obj内的符号值
        obj.adjust_obj_symbol(CODE_IS_SEPARATION)
        # 替换本文件内的elf全局符号
        obj.relocate_elf_symbol(CODE_IS_SEPARATION)
        # 替换本文件内的符号
        obj.relocate_local_symbol(CODE_IS_SEPARATION)
        # 配置文件启动类型（APP or SENSOR），文件ID
        obj.set_initflag_and_file_id()
        OBJS.append(obj)


def adjust_simple_obj_addr(simp_obj, index, dep_index, file, symtab):
    '''
    Adjust Sym In Simple Obj
    '''
    if simp_obj is None:
        return dep_index
    for rel_item in simp_obj.reltable:
        if rel_item.r_area != AREA_IDLE:
            rel_item.r_dep_index = index
            continue
        #遍历所有其他文件
        for dep, dep_obj in enumerate(OBJS):
            #同文件过滤
            if dep == index:
                continue
            value, shndx, bind = find_sym_in_file(dep_obj, rel_item.r_name_str, ELF_GLOBAL)
            #若未找到符号，则继续
            if shndx == 0:
                continue
            rel_item.r_dep_index = dep
            if dep_index.count(dep) == 0:
                dep_index.append(dep)

            # 找到符号后，判断在DDR还是TCM
            if dep_obj.tcm and shndx in dep_obj.tcm.sec_reoffset_dict:
                rel_item.r_area = AREA_TCM
                rel_item.r_value = (value + rel_item.r_addend) & UINT32_MAX
                logging.debug(rel_item.__dict__)
                break
            elif dep_obj.ddr and shndx in dep_obj.ddr.sec_reoffset_dict:
                rel_item.r_area = AREA_DDR
                rel_item.r_value = (value + rel_item.r_addend) & UINT32_MAX
                logging.debug(rel_item.__dict__)
                break
            elif dep_obj.itcm and shndx in dep_obj.itcm.sec_reoffset_dict:
                rel_item.r_area = AREA_ITCM
                rel_item.r_value = (value + rel_item.r_addend) & UINT32_MAX
                logging.debug(rel_item.__dict__)
                break
            elif dep_obj.dtcm and shndx in dep_obj.dtcm.sec_reoffset_dict:
                rel_item.r_area = AREA_DTCM
                rel_item.r_value = (value + rel_item.r_addend) & UINT32_MAX
                logging.debug(rel_item.__dict__)
                break
            elif dep_obj.ddr_text and shndx in dep_obj.ddr_text.sec_reoffset_dict:
                rel_item.r_area = AREA_DDR_TEXT
                rel_item.r_value = (value + rel_item.r_addend) & UINT32_MAX
                logging.debug(rel_item.__dict__)
                break
            elif dep_obj.ddr_data and shndx in dep_obj.ddr_data.sec_reoffset_dict:
                rel_item.r_area = AREA_DDR_DATA
                rel_item.r_value = (value + rel_item.r_addend) & UINT32_MAX
                logging.debug(rel_item.__dict__)
                break
            else:
                raise Exception('Not Found Sym {} value({:x}) shndx({}) bind({}) in file({})'.format(
                    rel_item.r_name_str, value, shndx, bind, dep_obj.file_path))
        # 在所有文件都没有找到符号，则打印失败
        else:
            raise Exception('Error: no find file({}) {} variable'.format(file.rsplit('/')[-1],
                symtab[rel_item.r_info >> 8].st_name_str))
    return dep_index


def adjust_addr_from_other_obj():
    '''
    Adjust Sym In Obj
    '''
    global OBJS

    for index, obj in enumerate(OBJS):
        obj.dep_index = []
        obj.dep_index = adjust_simple_obj_addr(obj.tcm, index, obj.dep_index, obj.file_path, obj.symtab)
        obj.dep_index = adjust_simple_obj_addr(obj.ddr, index, obj.dep_index, obj.file_path, obj.symtab)
        obj.dep_index = adjust_simple_obj_addr(obj.itcm, index, obj.dep_index, obj.file_path, obj.symtab)
        obj.dep_index = adjust_simple_obj_addr(obj.dtcm, index, obj.dep_index, obj.file_path, obj.symtab)
        obj.dep_index = adjust_simple_obj_addr(obj.ddr_text, index, obj.dep_index, obj.file_path, obj.symtab)
        obj.dep_index = adjust_simple_obj_addr(obj.ddr_data, index, obj.dep_index, obj.file_path, obj.symtab)
        if len(obj.dep_index) >= 4:
            raise Exception('Error: Dep file too much!!!')
        obj.dep_index += [index] * (4 - len(obj.dep_index))


def count_rel_entry_num(simp_obj):
    '''
    Calculate The Number Of Rel Entry
    '''
    if simp_obj is None:
        return
    count = 0
    for item in simp_obj.reltable:
        if item.r_area in \
            (AREA_DDR_TEXT, AREA_DDR_DATA, AREA_DTCM, AREA_ITCM, AREA_DDR, AREA_TCM, AREA_IMG_POS, AREA_IMG):
            count += 1
    simp_obj.relcnt = count


def pack_rel_table_info(simp_obj):
    '''
    Packing Relocation Table Information
    '''
    items = []
    for item in simp_obj.reltable:
        if item.r_area in \
            (AREA_DDR_TEXT, AREA_DDR_DATA, AREA_ITCM, AREA_DTCM, AREA_DDR, AREA_TCM, AREA_IMG_POS, AREA_IMG):
            logging.debug(item.__dict__)
            # 脚本中AREA_DDR是2，AREA_TCM是1，但是对应到实际链接过程，代码判断时，1是DDR， 0是TCM，所有要减1
            if item.r_offset > 0xffff:
                raise Exception('relocation entry offset too large!!!')
            if item.r_dep_index > 0xff:
                raise Exception('dep file index too large!!!')
            if item.r_info & 0xff > 0x3f:
                raise Exception('relocation entry info(type) number too large!!!')
            if item.r_area > (1 << DynMapInfo.TYPE_WIDTH):
                raise Exception('relocation area number too large!!!')

            if not CODE_IS_SEPARATION:
                items.append(struct.pack('<1Q', \
                        item.r_dep_index << DynMapInfo.FILE_IDX_SHIFT | item.r_offset | \
                        (item.r_area - 1) << DynMapInfo.TYPE_SHIFT | (item.r_info & 0x3f) << DynMapInfo.INFO_SHIFT | \
                        item.r_value << DynMapInfo.VALUE_SHIFT).decode("Latin-1"))
            else:
                items.append(struct.pack('<1Q', \
                            item.r_dep_index << DynMapInfo.FILE_IDX_SHIFT | item.r_offset | \
                            (item.r_info & 0xff) << DynMapInfo.INFO_SHIFT | item.r_value << DynMapInfo.VALUE_SHIFT
                ).decode("Latin-1"))
                items.append(struct.pack('<1B', (item.r_area - 1)).decode("Latin-1"))
    return ''.join(items)


def gen_obj_head(obj):
    '''
    Generate Simple Obj Header Info.
    '''
    if not CODE_IS_SEPARATION:
        count_rel_entry_num(obj.ddr)
        count_rel_entry_num(obj.tcm)
        # UINT32 FILEID
        # UINT16 DDR REL COUNT
        # UINT16 TCM REL COUNT
        # UINT16 DDR SIZE
        # UINT16 TCM SIZE
        # UINT32 INITFLAG
        obj.new_context = struct.pack('<L', obj.fileid).decode("Latin-1")
        obj.new_context += struct.pack('<2H2L', obj.ddr.relcnt, obj.tcm.relcnt, \
                                len(obj.ddr.new_context), \
                                len(obj.tcm.new_context)).decode("Latin-1")
        obj.new_context += struct.pack('<L', obj.initflag).decode("Latin-1")

        # DDR REMAP TABLE
        obj.new_context += pack_rel_table_info(obj.ddr)
        # TCM REMAP TABLE
        obj.new_context += pack_rel_table_info(obj.tcm)

        # DDR CONTEXT
        obj.new_context += obj.ddr.new_context
        # TCM CONTEXT
        obj.new_context += obj.tcm.new_context
    else:
        count_rel_entry_num(obj.ddr_text)
        count_rel_entry_num(obj.ddr_data)
        count_rel_entry_num(obj.itcm)
        count_rel_entry_num(obj.dtcm)
        # UINT32 FILEID
        # UINT16 DDR TEXT REL COUNT
        # UINT16 DDR DATA REL COUNT
        # UINT16 ITCM REL COUNT
        # UINT16 DTCM REL COUNT
        # UINT16 DDR TEXT SIZE
        # UINT16 DDR DATA SIZE
        # UINT16 ITCM SIZE
        # UINT16 DTCM SIZE
        # UINT32 INITFLAG
        obj.new_context = struct.pack('<2H', obj.boxid, obj.fileid).decode("Latin-1")
        obj.new_context += struct.pack('<4H4L', obj.ddr_text.relcnt, obj.ddr_data.relcnt, \
                                    obj.itcm.relcnt, obj.dtcm.relcnt, \
                                    len(obj.ddr_text.new_context), len(obj.ddr_data.new_context), \
                                    len(obj.itcm.new_context), len(obj.dtcm.new_context)).decode("Latin-1")
        obj.new_context += struct.pack('<L', obj.initflag).decode("Latin-1")

        # DDR REMAP TABLE
        obj.new_context += pack_rel_table_info(obj.ddr_text)
        obj.new_context += pack_rel_table_info(obj.ddr_data)
        # TCM REMAP TABLE
        obj.new_context += pack_rel_table_info(obj.itcm)
        obj.new_context += pack_rel_table_info(obj.dtcm)

        # DDR CONTEXT
        obj.new_context += obj.ddr_text.new_context
        obj.new_context += obj.ddr_data.new_context
        # TCM CONTEXT
        obj.new_context += obj.itcm.new_context
        obj.new_context += obj.dtcm.new_context

    # 4 ALGN
    context_len = len(obj.new_context)
    align_size = ((context_len + 4 - 1) // 4) * 4
    obj.new_size = align_size
    obj.new_context += (align_size - context_len) * '\0'


def reassemble_new_obj():
    '''
    Generate All Obj Header Info.
    '''
    global OBJS
    for obj in OBJS:
        gen_obj_head(obj)


def get_symbol_type_bind(info):
    '''
    Get Symbol Type And Bind
    '''
    sym_type = ''
    bind = ''
    if info & 0xF == 0:
        sym_type = 'NOTYPE'
    elif info & 0xF == 1:
        sym_type = 'OBJECT'
    elif info & 0xF == 2:
        sym_type = 'FUNC'
    elif info & 0xF == 3:
        sym_type = 'SECTION'
    elif info & 0xF == 4:
        sym_type = 'FILE'
    else:
        sym_type = 'NONE'

    if info >> 4 == 0:
        bind = 'LOCAL'
    elif info >> 4 == 1:
        bind = 'GLOBAL'
    elif info >> 4 == 2:
        bind = 'WEAK'
    else:
        sym_type = 'NONE'

    return sym_type, bind


def get_rel_area_and_idx(area, idx):
    '''
    Get Single Rel Area Name
    '''
    if area == AREA_TCM:
        location = 'TCM'
        idx += 1
        str_idx = str(idx)
    elif area == AREA_DDR:
        location = 'DDR'
        idx += 1
        str_idx = str(idx)
    elif area == AREA_ITCM:
        location = 'ITCM'
        idx += 1
        str_idx = str(idx)
    elif area == AREA_DTCM:
        location = 'DTCM'
        idx += 1
        str_idx = str(idx)
    elif area == AREA_DDR_TEXT:
        location = 'DDR_TEXT'
        idx += 1
        str_idx = str(idx)
    elif area == AREA_DDR_DATA:
        location = 'DDR_DATA'
        idx += 1
        str_idx = str(idx)
    elif area == AREA_IMG:
        location = 'IMG'
        idx += 1
        str_idx = str(idx)
    elif area == AREA_IMG_POS:
        location = 'IMG_POS'
        idx += 1
        str_idx = str(idx)
    elif area == AREA_IGNORE:
        location = 'IGNORE'
        str_idx = '--'
    elif area == AREA_IMG_STRLOG:
        location = 'STRLOG'
        str_idx = '--'
    else:
        location = 'NULL'
        str_idx = '--'
    return location, idx, str_idx


def gen_rel_entry_info(reltable, items, obj):
    '''
    Generate Relocation Entry Information.
    '''
    idx = -1
    for item in reltable:
        location, idx, str_idx = get_rel_area_and_idx(item.r_area, idx)
        rel_type = get_relocation_type(item.r_info)
        if item.r_info & 0xFF in no_reloc_type_list:
            continue
        name = item.r_name_str
        if name == '':
            name = obj.sections[obj.symtab[item.r_info >> 8].st_shndx].sh_name_str
        symbal_type, symbal_bind = get_symbol_type_bind(obj.symtab[item.r_info >> 8].st_info)
        items.append('\t\t\t(%-3s  0x%-4x  0x%-5x  %-18s  %-3d  0x%-8x  %-8s)  %-7s  %-7s  [%-24s]  [%s]\n' % \
        (str_idx, item.r_offset, item.r_info, rel_type, item.r_dep_index, item.r_value, location, symbal_type,        \
        symbal_bind, name, item.r_sec_name.split('.rela')[-1]))


def get_obj_box_name(box_id):
    '''
    Get Box Name
    '''
    box_name = ''
    if box_id == 0:
        box_name = 'DRIVER BOX'
    elif box_id == 1:
        box_name = 'APP BOX '
    else:
        box_name = 'UNKOWN BOX'
    return box_name


def gen_section_title(mem_type, data_type, context_addr, simp_obj, items):
    '''
    Generating section title
    '''
    if mem_type == 'DDR':
        items.append('\t\tDDR %s Size:  %d, DDR %s Remap Count: %d, Address(0x%x)\n' % \
            (data_type, len(simp_obj.new_context), data_type, simp_obj.relcnt, context_addr))
        items.append('\t\t%s %s Section List: \n' % (mem_type, data_type))
        items.append('\t\t\t%-50s  %-6s  %-6s %-6s\n' % ('Seciton', 'Offset', 'Size', 'Shndx'))
        for simp in simp_obj.simplist:
            items.append('\t\t\t[%-48s]  0x%-4x  0x%-4x 0x%-4x %d\n' % \
                (simp.name_str, simp.reoffset, simp.size, simp.index, simp.index))
        items.append('\t\t%s %s Remap List: \n' % (mem_type, data_type))
        items.append('\t\t\t(%-3s  %-6s  %-7s  %-18s  %-3s  %-10s  %-8s)  %-7s  %-7s  %-26s  %s\n' % \
            ('Idx', 'Offset', 'Info', 'Type', 'Dep', 'Value', 'Location', 'SymType', 'SymBind', 'Symbol', 'Section'))
    else:
        items.append('\t\tTCM %s Size: %d, TCM %s Remap Count: %d, Address(0x%x)\n' % \
            (data_type, len(simp_obj.new_context), data_type, simp_obj.relcnt, context_addr))
        items.append('\t\t%s %s Section List: \n' % (mem_type, data_type))
        items.append('\t\t\t%-50s  %-6s  %-6s %-6s\n' % ('Seciton', 'Offset', 'Size', 'Shndx'))
        for simp in simp_obj.simplist:
            items.append('\t\t\t[%-48s]  0x%-4x  0x%-4x 0x%-4x %d\n' % \
                (simp.name_str, simp.reoffset, simp.size, simp.index, simp.index))
        items.append('\t\t%s %s Remap List: \n' % (mem_type, data_type))
        items.append('\t\t\t(%-3s  %-6s  %-7s  %-18s  %-3s  %-10s  %-8s)  %-7s  %-7s  %-26s  %s\n' % \
            ('Idx', 'Offset', 'Info', 'Type', 'Dep', 'Value', 'Location', 'SymType', 'SymBind', 'Symbol', 'Section'))


def gen_debug_info_se(target):
    '''
    Generating Debugging Information
    '''
    global OBJS
    global IMG_SIZE
    global HEAD
    file_nums = len(OBJS)
    head_start = IMG_SIZE + len('ONCE_LOADER_FILE_LIST_BEGINS')
    # 4 : FILE_NUM
    # 12 : FILE_IDX + FILE_OFFSET + 4 * DEP
    body_start = head_start + 4 + 12 * file_nums
    items = [
        'ONCE_LOAD: start_addr(0x%x), head_start(0x%x), body_start(0x%x), file_nums(%d)\n\n' % \
        (IMG_SIZE, head_start, body_start, file_nums)
    ]

    for index, obj in enumerate(OBJS):
        # output current obj header info
        if obj.initflag == 1:
            initflag = 'APP'
        else:
            initflag = 'SENSOR'
        obj_addr = obj.new_offset + body_start
        items.append('BoxID(%d-%s) FileID(0x%x-%d), Offset(0x%x), Address(0x%x) File Name: %s \n' % \
            (obj.boxid, get_obj_box_name(obj.boxid), \
            obj.fileid, obj.fileid, obj.new_offset, obj_addr, obj.file_path.rsplit('\\')[-1]))
        items.append('\tIndex(%d), InitFlag(%d-%s), Dep_Index(%d, %d, %d, %d) \n' % \
            (index, obj.initflag, initflag, obj.dep_index[0], obj.dep_index[1], obj.dep_index[2], obj.dep_index[3]))

        rel_count = obj.itcm.relcnt + obj.dtcm.relcnt + obj.ddr_data.relcnt + obj.ddr_text.relcnt
        # output ddr section relocation entry info
        # 32 : BOX_FILE_ID + DDR_REL_COUNT + TCM_REL_COUNT + DDR_T_LEN + DDR_D_LEN + ITCM_LEN + DTCM_LEN + INIT_FLAG
        context_addr = obj_addr + 32 + (8 * rel_count)
        gen_section_title('DDR', 'TEXT', context_addr, obj.ddr_text, items)
        gen_rel_entry_info(obj.ddr_text.reltable, items, obj)

        context_addr = context_addr + len(obj.ddr_text.new_context)
        gen_section_title('DDR', 'DATA', context_addr, obj.ddr_data, items)
        gen_rel_entry_info(obj.ddr_data.reltable, items, obj)

        # output tcm section relocation entry info
        context_addr = context_addr + len(obj.ddr_data.new_context)
        gen_section_title('TCM', 'TEXT', context_addr, obj.itcm, items)
        gen_rel_entry_info(obj.itcm.reltable, items, obj)

        # output tcm section relocation entry info
        context_addr = context_addr + len(obj.itcm.new_context)
        gen_section_title('TCM', 'DATA', context_addr, obj.dtcm, items)
        gen_rel_entry_info(obj.dtcm.reltable, items, obj)

        # output current obj end info
        items.append('\n\n')

    debug = ''.join(items)

    with open(target + '.debug.txt', 'w+') as outputfile:
        outputfile.write(debug)
        outputfile.close()


def gen_debug_info(target):
    '''
    Generating Debugging Information
    '''
    global OBJS
    global IMG_SIZE
    global HEAD
    file_nums = len(OBJS)
    head_start = IMG_SIZE + len('ONCE_LOADER_FILE_LIST_BEGINS')
    # 4 : FILE_NUM
    # 12 : FILE_IDX + FILE_OFFSET + 4 * DEP
    body_start = head_start + 4 + 12 * file_nums
    items = [
        'ONCE_LOAD: start_addr(0x%x), head_start(0x%x), body_start(0x%x), file_nums(%d)\n\n' % \
        (IMG_SIZE, head_start, body_start, file_nums)
    ]

    for index, obj in enumerate(OBJS):
        # output current obj header info
        if obj.initflag == 1:
            initflag = 'APP'
        else:
            initflag = 'SENSOR'
        obj_addr = obj.new_offset + body_start
        items.append('FileID(0x%x-%d), Offset(0x%x), Address(0x%x) File Name: %s \n' % \
            (obj.fileid, obj.fileid, obj.new_offset, obj_addr, obj.file_path.rsplit('\\')[-1]))
        items.append('\tIndex(%d), InitFlag(%d-%s), Dep_Index(%d, %d, %d, %d) \n' % \
            (index, obj.initflag, initflag, obj.dep_index[0], obj.dep_index[1], obj.dep_index[2], obj.dep_index[3]))

        # output ddr section relocation entry info
        ddr_addr = obj_addr + 20 + 8 * (obj.tcm.relcnt + obj.ddr.relcnt)
        gen_section_title('DDR', '', ddr_addr, obj.ddr, items)
        gen_rel_entry_info(obj.ddr.reltable, items, obj)

        # output tcm section relocation entry info
        tcm_addr = ddr_addr + len(obj.ddr.new_context)
        gen_section_title('TCM', '', tcm_addr, obj.tcm, items)
        gen_rel_entry_info(obj.tcm.reltable, items, obj)

        # output current obj end info
        items.append('\n\n')

    debug = ''.join(items)

    with open(target + '.debug.txt', 'w+') as outputfile:
        outputfile.write(debug)
        outputfile.close()


def stitch_files(output_bin_file, mode, is_safe_kernel):
    '''
    Generate The Onceload Format IMG.
    '''
    global OBJS
    global IMG_SIZE
    global HEAD
    global ELF_OBJ

    obj_offset = 0
    items_head = ['ONCE_LOADER_FILE_LIST_BEGINS%s' % struct.pack('<L', len(OBJS)).decode("Latin-1")]
    items_body = []

    for obj in OBJS:
        if not CODE_IS_SEPARATION:
            items_head.append(struct.pack('<2L4B', obj.fileid, obj_offset, \
                    obj.dep_index[0], obj.dep_index[1], obj.dep_index[2], \
                    obj.dep_index[3]).decode("Latin-1"))
        else:
            items_head.append(struct.pack('<2HL4B', obj.boxid, obj.fileid, obj_offset, \
                    obj.dep_index[0], obj.dep_index[1], obj.dep_index[2], \
                    obj.dep_index[3]).decode("Latin-1"))
        obj.new_offset = obj_offset
        obj_offset += obj.new_size
        items_body.append(obj.new_context)

    # for normal
    if mode == 1:
        # try to split logstr from outputfile (only for seliteos).
        if is_safe_kernel:
            with open(output_bin_file, 'ab+') as outputfile:
                file_len = os.path.getsize(output_bin_file)
                outputfile.seek(file_len - ELF_OBJ.logstr_sec.sh_size)
                outputfile.truncate()
                outputfile.close
        # try to append onceloader objs to outputfile.
        with open(output_bin_file, 'ab+') as outputfile:
            outputfile.seek(0, 2)
            size = outputfile.tell()
            # 4字节对齐
            IMG_SIZE = ((size + 4 - 1) // 4) * 4
            outputfile.write((IMG_SIZE - size) * b'\0')
            HEAD = ''.join(items_head)
            outputfile.write(HEAD.encode("Latin-1"))
            body = ''.join(items_body)
            outputfile.write(body.encode("Latin-1"))
            outputfile.close()
    elif mode == 0:
        with open(output_bin_file, 'rb') as outputfile:
            context = outputfile.read()
            outputfile.close()
        items_context = [context]
        items_context.append(HEAD)
        items_context.append(body)
        context = ''.join(items_context)
        with open('%stest.img' % output_bin_file, 'w') as outputfile:
            outputfile.write(context)
            outputfile.close()


def g_sym_is_new(one_g_sym):
    '''
    Check Whether The Symbol Table Is Processed.
    '''
    for obj in OBJS:
        for l_sym in obj.symtab:
            if l_sym.st_name_str == one_g_sym and l_sym.st_shndx != 0 and l_sym.st_info >> 4 == 1:
                return False
    return True


def get_obj_global_sym():
    '''
    Gets The Symbol Of the Global Type.
    '''
    global OBJS

    global_sym = []
    for file_path in FILES:
        # 解析一个OBJ文件
        obj = ArchObjFile(file_path)
        obj.get_link_global_sym(global_sym)
        OBJS.append(obj)
    new_sym = []
    for g_sym in global_sym:
        if g_sym_is_new(g_sym) is True:
            new_sym.append(g_sym)
    return list(set(new_sym))


def gen_reserved_c_file(out_file, sym_list):
    '''
    Generate Reserved.c File
    '''
    head_context = '''#define AT_DDR_DATA __attribute__((__used__, section(".ol_sym")))
#define RESERVED(x)     extern void (x)(void); AT_DDR_DATA void *p##x = &x;
'''
    built_in_funcs = ['memset', 'memcpy', 'memset_s', 'memcpy_s', 'sqrt', 'strcmp']

    undef_sym = list(set(sym_list))
    undef_sym.sort()
    undef_sym = [i for i in undef_sym if i not in built_in_funcs]
    with open(out_file, 'w+') as res_file:
        res_file.write(head_context)
        for undef_symbol in undef_sym:
            res_file.write("RESERVED(%s);\n" % undef_symbol)
        res_file.close()


class ObjSizeInfo(ObjFile):
    '''
    Records the size information of each obj.
    '''
    def __init__(self, filepath):
        ObjFile.__init__(self, filepath)
        self.itcm_size = self.get_specified_section_size(self.sections, ['.text'])
        self.dtcm_size = self.get_specified_section_size(self.sections, \
                                                        ['.data', '.rodata', '.bss', '.sdata', '.sbss'])
        self.ddr_text_size = self.get_specified_section_size(self.sections, ['.ddr.text'])
        self.ddr_data_size = self.get_specified_section_size(self.sections, \
                                                             ['.dyn_', '.ddr.data', '.ddr.rodata', '.ddr.bss'])

    def get_specified_section_size(self, sections, speclist):
        '''
        Obtains the specified segment size.
        '''
        size = 0
        for key in speclist:
            for _, sec_info in enumerate(sections):
                if not sec_info.sh_name_str.startswith(key):
                    continue
                size += sec_info.sh_size
        return size


class OnceloadSizeInfo:
    '''
    Records the size information of the entire obj.
    '''
    def __init__(self):
        self.itcm_size = 0
        self.dtcm_size = 0
        self.ddr_text_size = 0
        self.ddr_data_size = 0
        self.module_dict = {}

    def add(self, size_info):
        '''
        onceloader size add.
        '''
        self.itcm_size += size_info.itcm_size
        self.dtcm_size += size_info.dtcm_size
        self.ddr_text_size += size_info.ddr_text_size
        self.ddr_data_size += size_info.ddr_data_size

    def calc_max_size(self):
        '''
        Calculate the maximum size.
        '''
        keys = deepcopy(list(self.module_dict.keys()))
        while keys:
            key = keys.pop()
            obj_size = OnceloadSizeInfo()
            for info in self.module_dict[key]:
                if isinstance(info, ObjSizeInfo):
                    obj_size.itcm_size = obj_size.itcm_size if obj_size.itcm_size > info.itcm_size else info.itcm_size
                    obj_size.dtcm_size = obj_size.dtcm_size if obj_size.dtcm_size > info.dtcm_size else info.dtcm_size
                    obj_size.ddr_text_size = obj_size.ddr_text_size \
                                             if obj_size.ddr_text_size > info.ddr_text_size else info.ddr_text_size
                    obj_size.ddr_data_size = obj_size.ddr_data_size \
                                             if obj_size.ddr_data_size > info.ddr_data_size else info.ddr_data_size
            if "dual" in key.lower():
                obj_size.add(obj_size)

            self.add(obj_size)


def gen_head_include_file(out_file):
    '''
    Generate Include File
    '''
    global FILES

    onceload_size = [OnceloadSizeInfo(), OnceloadSizeInfo()]

    for file_path in FILES:
        driver_info = ObjSizeInfo(file_path)
        el = os.path.basename(driver_info.file_path).split('_')
        module = el[-1][:-len('.o')]
        onceload_size[driver_info.boxid].module_dict.setdefault(module, [])
        onceload_size[driver_info.boxid].module_dict[module].append(driver_info)

    # kernel or drv box
    onceload_size[0].calc_max_size()
    # app box
    onceload_size[1].calc_max_size()

    with open(out_file, 'w+') as include_file:
        include_file.write("#define DRVBOX_ONCELOAD_ITCM_SIZE 0x%x\n" % onceload_size[0].itcm_size)
        include_file.write("#define DRVBOX_ONCELOAD_DTCM_SIZE 0x%x\n" % onceload_size[0].dtcm_size)
        include_file.write("#define DRVBOX_ONCELOAD_DDR_TEXT_SIZE 0x%x\n" % onceload_size[0].ddr_text_size)
        include_file.write("#define DRVBOX_ONCELOAD_DDR_DATA_SIZE 0x%x\n" % onceload_size[0].ddr_data_size)

        include_file.write("#define APPBOX_ONCELOAD_ITCM_SIZE 0x%x\n" % onceload_size[1].itcm_size)
        include_file.write("#define APPBOX_ONCELOAD_DTCM_SIZE 0x%x\n" % onceload_size[1].dtcm_size)
        include_file.write("#define APPBOX_ONCELOAD_DDR_TEXT_SIZE 0x%x\n" % onceload_size[1].ddr_text_size)
        include_file.write("#define APPBOX_ONCELOAD_DDR_DATA_SIZE 0x%x\n" % onceload_size[1].ddr_data_size)

        include_file.write("#define ONCELOAD_ITCM_SIZE 0x%x\n" % \
                            (onceload_size[0].itcm_size + onceload_size[1].itcm_size))
        include_file.write("#define ONCELOAD_DTCM_SIZE 0x%x\n" % \
                            (onceload_size[0].dtcm_size + onceload_size[1].dtcm_size))
        include_file.write("#define ONCELOAD_DDR_TEXT_SIZE 0x%x\n" % \
                            (onceload_size[0].ddr_text_size + onceload_size[1].ddr_text_size))
        include_file.write("#define ONCELOAD_DDR_DATA_SIZE 0x%x\n" % \
                            (onceload_size[0].ddr_data_size + onceload_size[1].ddr_data_size))


if __name__ == '__main__':
    run_mode = int(sys.argv[1])
    if run_mode == 2:
        reserve_file = sys.argv[2]
        for file_idx in range(3, len(sys.argv)):
            if sys.argv[file_idx].endswith('.o'):
                FILES.append(sys.argv[file_idx])
        NOT_FOUND_SYM = get_obj_global_sym()
        gen_reserved_c_file(reserve_file, NOT_FOUND_SYM)
    elif run_mode == 3:
        CODE_IS_SEPARATION = str_to_bool(sys.argv[2])
        IS_SAFE_KERNEL = str_to_bool(sys.argv[3])
        head_file = sys.argv[4]
        if not CODE_IS_SEPARATION:
            logging.info("No need to generate onceloader size information")
        for file_idx in range(4, len(sys.argv)):
            if sys.argv[file_idx].endswith('.o'):
                FILES.append(sys.argv[file_idx])
        gen_head_include_file(head_file)
    elif run_mode == 1:
        CODE_IS_SEPARATION = str_to_bool(sys.argv[2])
        IS_SAFE_KERNEL = str_to_bool(sys.argv[3])
        IMG_FILE = sys.argv[4]
        ELF_FILE = sys.argv[5]

        if CODE_IS_SEPARATION:
            DynMapInfo.INFO_WIDTH = 8
            DynMapInfo.TYPE_WIDTH = 8
            DynMapInfo.TYPE_SHIFT = 64

        for file_idx in range(6, len(sys.argv)):
            if sys.argv[file_idx].endswith('.o'):
                FILES.append(sys.argv[file_idx])
        parse_elf_file()
        parse_obj_file()

        adjust_addr_from_other_obj()

        reassemble_new_obj()

        stitch_files(IMG_FILE, run_mode, IS_SAFE_KERNEL)

        if CODE_IS_SEPARATION:
            gen_debug_info_se(IMG_FILE)
        else:
            gen_debug_info(IMG_FILE)

        NOT_FOUND_SYM = list(set(NOT_FOUND_SYM))
        if 0 == len(NOT_FOUND_SYM):
            logging.info('Success For Make Once Loader Files, OBJS(%d)', len(FILES))
        else:
            raise Exception('Error: Symbol not found {} in OBJS{}'.format(' '.join(NOT_FOUND_SYM), len(FILES)))
