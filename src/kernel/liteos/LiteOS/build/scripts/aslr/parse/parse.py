#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
'''
Parse elf method
'''

import re
import logging
import tempfile
from typing import Union, Iterator, Optional
from util.utils import Utils
from data_manager.data_type import ElfHeaderStruct, ElfSectionStruct, ShType, SymbolInfoStruct


LOG_FORMAT = '[%(funcName)s:%(lineno)d, %(levelname)s]: %(message)s'
logging.basicConfig(level=logging.NOTSET, format=LOG_FORMAT)


class ELFInfo:
    def __init__(self, reloc_type, header_info, section_data):
        if reloc_type not in ['REL', 'RELA']:
            raise ValueError('reloc_type should be REL or RELA')
        if (('little' not in header_info.endian) and \
            ('big' not in header_info.endian)):
            raise ValueError('unsupported endian')

        self.header_info = header_info
        self.section_data = section_data

        self._reloc_type = reloc_type

    @property
    def endian(self) -> str:
        '''
        archtecture endian type.
        '''
        if 'little' in self.header_info.endian:
            return 'little'
        if 'big' in self.header_info.endian:
            return 'big'
        raise ValueError('invalid endian info')

    @property
    def reloc_type(self) -> int:
        '''
        relocation type
        '''
        if self._reloc_type == 'REL':
            return ShType.REL
        return ShType.RELA

    @property
    def elfclass(self) -> str:
        '''
        elfclass
        '''
        return self.header_info.elfclass

    @property
    def machine(self) -> str:
        '''
        machine type name
        '''
        return self.header_info.machine

    def get_data(self, vaddr: int, size: int) -> Optional[bytes]:
        '''
        Get machine code of addr
        '''
        if size == 0:
            return None
        for section in self.section_data:
            if vaddr in section:
                return section[vaddr : vaddr + size]
        return None


class SectionData:
    def __init__(self, name: str, content: bytes, va_range: range):
        self.name = name
        self.content = content
        self.va_range = va_range

    def __contains__(self, address: int):
        return address in self.va_range

    def __getitem__(self, index: Union[slice, int]):
        if isinstance(index, slice):
            start = index.start
            end = index.stop
        elif isinstance(index, int):
            start = index
            end = index + 1
        else:
            raise TypeError("Invalid index type")

        if start >= end:
            raise ValueError('Invalid range, range should contain at least one data')
        if start not in self.va_range or (end - 1) not in self.va_range:
            raise ValueError('Invalid index for this section va')
        offset = start - self.va_range.start
        size = end - start
        return self.content[offset : offset + size]


class ParseElf():
    '''
    Class for parsing elf file
    '''
    # example:
    # [Nr] Name       Type     Addr     Off    Size   ES  Lk Inf Al  Flags
    # [ 0]            NULL     00000000 000000 000000 00  0  0   0  [00000000]:
    # [ 1] .laslr     PROGBITS 007fc000 000080 0007fc 00  0  0  32  [00000007]: WRITE, ALLOC, EXEC
    # [ 2] .rel.laslr REL      00000000 149950 000260 08 20  1   4  [00000040]: INFO LINK
    # [ 3] .startup   PROGBITS 007fc800 000880 000490 00  0  0  32  [00000006]: ALLOC, EXEC
    rule_section_start = r'^\s*\[\s*[0-9]+\]\s+\.'
    rule_section_info = r'^\s*\[\s*([0-9]+)\]\s+([\.\w\-]+)\s+([a-zA-Z_]+)\s+' \
        '([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+' \
        '([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+' \
        '\[([0-9a-fA-F]+)\]:([\w\,\s]+)'
    # example:
    # Relocation section '.rel.laslr' at offset
    # Relocation section '.rela.laslr' at offset
    rule_rel_section_start = r'^\s*Relocation\s+section\s+\'(\.rela*\.[\w\.]+)\'\s+at\s+offset\s+'

    # example:
    # 008135f4  00000028 R_ARM_V4BX
    # c000291a  00000033 R_RISCV_RELAX                     8
    rule_rel_half_match = r'^\s*([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+(\w+)\s+[\s\w\.\-\_]*$'
    # example:
    # 00ffe2f4  00048902 R_ARM_ABS32       0081160c   HalIrqPending
    rule_rel_info = r'^\s*([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+(\w+)\s+([0-9a-fA-F]+)\s+([\w\.\-]+)[\s\w\.\-\_]*$'
    # example:
    # 000007b4  00041f1a R_RISCV_HI20      8000008e   nmi_handler.LOGSTR_ris + 0
    # 00000080  00d94f1a R_RISCV_HI20      20030000   __data_copy_dest__ - 20000000
    # 00000858  000fc717 R_RISCV_PCREL_HI20     20080000   __global_pointer$ + 0
    rule_rela_info = r'^\s*([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+(\w+)\s+([0-9a-fA-F]+)\s+' \
        '([\w\.\-\$]+)\s+([\+\-])\s+([0-9a-fA-F]+)\s*$'

    # example:
    #  Offset     Info    Type
    rule_relheader_match = r'^\s* Offset\s+Info\s+Type\s*[\s\w\.\-\_\']*$'

    #example
    #82329: 00000000     0 NOTYPE  GLOBAL DEFAULT    1 __image_start
    rule_symtab_info = r'^\s*([0-9]+):\s+([0-9a-fA-F]+)\s+([0-9]+)\s+([a-zA-Z]+)\s+' \
        '([a-zA-Z]+)\s+([a-zA-Z]+)\s+([0-9a-zA-Z]+)\s+(.+)\s*$'

    def __init__(self, cmd: str, elffile: str):
        self._cmd = cmd
        self._elffile = elffile

        reloc_type = self._get_section_header_type()
        header_info = self._get_header_info()
        section_data = self._get_all_section()
        self._elfinfo = ELFInfo(reloc_type, header_info, section_data)

    @property
    def reloc_type(self):
        '''
        {0}
        '''.format(self._elfinfo.reloc_type.__doc__)
        return self._elfinfo.reloc_type

    @property
    def elfclass(self):
        '''
        {0}
        '''.format(self._elfinfo.elfclass.__doc__)
        return self._elfinfo.elfclass

    @property
    def machine(self):
        '''
        {0}
        '''.format(self._elfinfo.machine.__doc__)
        return self._elfinfo.machine

    @property
    def endian(self):
        '''
        {0}
        '''.format(self._elfinfo.endian.__doc__)
        return self._elfinfo.endian

    def get_data(self, vaddr: int, size: int) -> Optional[bytes]:
        '''
        {0}
        '''.format(self._elfinfo.get_data.__doc__)
        return self._elfinfo.get_data(vaddr, size)

    def get_one_symtab_value(self, symbol_name: str) -> int:
        '''
        get symbol value
        '''
        cmd_list = ' '.join([self._cmd + 'readelf', '-s', '-W', self._elffile, '|', 'grep', symbol_name])
        out_text, return_code = Utils.run_shell_return(cmd_list, None)
        if return_code != 0:
            Utils.err_exit("{0} return {1}".format(cmd_list, return_code))
        pattern_sym = re.compile(self.rule_symtab_info)
        groups_sym = pattern_sym.match(out_text.splitlines(False)[0])
        return int(groups_sym.group(2), 16)

    def iter_symbol_list(self) -> Iterator[SymbolInfoStruct]:
        '''
        iterate symbol list
        '''
        cmd_list = ' '.join([self._cmd + 'readelf', '-s', '-W', self._elffile])
        symbols, return_code = Utils.run_shell_return(cmd_list, None)
        if return_code != 0:
            Utils.err_exit("{0} return {1}".format(cmd_list, return_code))
        symbols_lines = symbols.splitlines()

        for line in symbols_lines:
            pattern_sym = re.compile(self.rule_symtab_info)
            groups_sym = pattern_sym.match(line)
            if not groups_sym:
                continue

            sym_num = int(groups_sym[1], base=10)
            sym_value = int(groups_sym[2], base=16)
            sym_size = int(groups_sym[3], base=10)
            sym_type = groups_sym[4]
            sym_bind = groups_sym[5]
            sym_vis = groups_sym[6]
            try:
                sym_ndx = int(groups_sym[7], base=10)
            except ValueError:
                sym_ndx = groups_sym[7]
            sym_name = groups_sym[8]

            yield SymbolInfoStruct(sym_num, sym_value, sym_size, sym_type, sym_bind, sym_vis, sym_ndx, sym_name)

    def iter_relinfo(self) -> Iterator[dict]:
        '''
        Parse relocation section
        '''
        def is_valid_rel_section(sec_name):
            if not sec_name:
                return False
            for excluded_sec_name in ['debug', 'laslr_rel']:
                if sec_name.find(excluded_sec_name) != -1:
                    return False
            if sec_name in ['.rel.laslr', '.rela.laslr', '.rela.laslr.data', '.rel.laslr.data']:
                return False
            return True

        def line_match_section_start(line):
            section_name = None
            pattern = re.compile(self.rule_rel_section_start)
            groups = pattern.match(line)
            if groups:
                section_name = groups.group(1)
            return section_name

        cmd_list = [''.join([self._cmd, 'readelf'])]
        cmd_list.append("-Wr")
        cmd_list.append(self._elffile)
        out_text, return_code = Utils.run_shell_return(cmd_list, None)
        current_section = None
        if return_code != 0:
            Utils.err_exit("{0} return {1}".format(cmd_list, return_code))
        for line in out_text.splitlines(False):
            tmp = line_match_section_start(line)
            current_section = tmp if tmp else current_section
            if is_valid_rel_section(current_section):
                relinfo = self._parse_one_relinfo(line, self.reloc_type)
                if relinfo:
                    yield relinfo

    def _get_header_info(self):
        '''
        Parse elf header
        '''
        cmd_list = [''.join([self._cmd, 'readelf'])]
        cmd_list.append("-Wh")
        cmd_list.append(self._elffile)
        out_text, return_code = Utils.run_shell_return(cmd_list, None)
        if return_code != 0:
            Utils.err_exit("{0} return {1}".format(cmd_list, return_code))
        for line in out_text.splitlines(False):
            line_string = line.strip().split(':')
            if line_string[0].strip() == 'Class':
                header_class = line_string[1].strip().split()[0]
            elif line_string[0].strip() == 'Type':
                header_type = line_string[1].strip().split()[0]
            elif line_string[0].strip() == 'Machine':
                header_machine = line_string[1].strip()
                if header_machine == 'Huawei LingLong Processor':
                    header_machine = 'LingLong'
                else:
                    header_machine = header_machine.split()[0]
            elif line_string[0].strip() == 'Entry point address':
                header_address = line_string[1].strip().split()[0]
            elif line_string[0].strip() == 'Number of section headers':
                header_num = line_string[1].strip().split()[0]
            elif line_string[0].strip() == 'Data':
                header_endian = line_string[1].strip()

        elf_header_info = ElfHeaderStruct(header_class, header_type, \
            header_machine, header_address, header_num, header_endian)
        logging.info("elf header info is: %s %s %s %s %s %s", elf_header_info.elfclass, elf_header_info.type, \
            elf_header_info.machine, elf_header_info.address, elf_header_info.num, elf_header_info.endian)
        return elf_header_info

    def _get_all_section(self):
        '''
        Parse section data
        '''
        section_lst = self._get_section_info()
        sec_data_lst = []
        for sec_info in section_lst:
            if (sec_info.type in ['PROGBITS', 'FINI_ARRAY']) and ('ALLOC' in sec_info.flags):
                sec_data = self._get_section_data(sec_info.name)
                sec_start = int(sec_info.addr, 16)
                sec_end = sec_start + int(sec_info.size, 16)
                sec = SectionData(sec_info.name, sec_data, range(sec_start, sec_end))
                sec_data_lst.append(sec)
        return sec_data_lst

    def _get_section_header_type(self):
        section_lst = self._get_section_info()
        sh_type = None
        lst = [sec for sec in section_lst if sec.type in ('REL', 'RELA')]
        if lst:
            sh_type = lst[0].type
        for sec in lst:
            if sh_type != sec.type:
                Utils.err_exit("sh_type change")
        return sh_type

    def _parse_one_relinfo(self, line, sh_type) -> Optional[dict]:
        if sh_type == ShType.REL:
            groups_rel = re.match(self.rule_rel_info, line)
            if groups_rel:
                return {
                    'offset' : groups_rel.group(1), 'info' : groups_rel.group(2), \
                    'type' : groups_rel.group(3), 'value' : groups_rel.group(4), \
                    'name' : groups_rel.group(5), 'addend' : 0, 'mcode' : 0, \
                    'addr' : None
                }
        elif sh_type == ShType.RELA:
            groups_rela = re.match(self.rule_rela_info, line)
            if groups_rela:
                addend = int(groups_rela.group(7), 16)
                if groups_rela.group(6) == '-':
                    addend = -addend
                return {
                    'offset' : groups_rela.group(1), 'info' : groups_rela.group(2), \
                    'type' : groups_rela.group(3), 'value' : groups_rela.group(4), \
                    'name' : groups_rela.group(5), 'addend' : addend, 'mcode' : 0, \
                    'addr' : None
                }

        groups_half = re.match(self.rule_rel_half_match, line)
        if groups_half:
            return {
                'offset' : groups_half.group(1),
                'info' : groups_half.group(2),
                'type' : groups_half.group(3)
            }

        groups_relheader = re.match(self.rule_relheader_match, line)
        groups_empty = re.match(r'^\s*', line)
        if (not groups_relheader) and (not groups_empty):
            Utils.err_exit('error. not match line: {}'.format(line))

        return None

    def _get_section_info(self):
        section_lst = []
        cmd_list = [''.join([self._cmd, 'readelf']), "-t", self._elffile]
        out_text, return_code = Utils.run_shell_return(cmd_list, None)
        if return_code != 0:
            Utils.err_exit("{0} return {1}".format(cmd_list, return_code))
        line_lst = []
        pattern_start = re.compile(self.rule_section_start)
        pattern_info = re.compile(self.rule_section_info)

        for line in out_text.splitlines(False):
            groups_start = pattern_start.match(line)
            if groups_start:
                # process multi lines here
                merge_line = ' '.join(line_lst)
                groups_info = pattern_info.match(merge_line)
                if groups_info:
                    item = ElfSectionStruct(groups_info.group(1), groups_info.group(2),
                    groups_info.group(3), groups_info.group(4), groups_info.group(5),
                    groups_info.group(6), groups_info.group(7), groups_info.group(8),
                    groups_info.group(9), groups_info.group(10), groups_info.group(12))
                    section_lst.append(item)
                line_lst[:] = [] # clear list adapt to py2
            line_lst.append(line) # merge lines

        # process the last section header
        merge_line = ' '.join(line_lst)
        groups_end = pattern_info.match(merge_line)
        if groups_end:
            item = ElfSectionStruct(groups_end.group(1), groups_end.group(2),
            groups_end.group(3), groups_end.group(4), groups_end.group(5),
            groups_end.group(6), groups_end.group(7), groups_end.group(8),
            groups_end.group(9), groups_end.group(10), groups_end.group(12))
            section_lst.append(item)
        return section_lst

    def _get_section_data(self, section_name):
        objcopy_cmd = [''.join([self._cmd, 'objcopy']), '-O', 'binary', '--only-section']
        with tempfile.NamedTemporaryFile() as temp_file:
            cmd_list = []
            cmd_list.extend(objcopy_cmd)
            cmd_list.append(section_name)
            cmd_list.append(self._elffile)
            cmd_list.append(temp_file.name)
            _, return_code = Utils.run_shell_return(cmd_list, None)
            if return_code != 0:
                Utils.err_exit("{0} return {1}".format(cmd_list, return_code))
            return temp_file.read()  # temp_file updated by external executable
