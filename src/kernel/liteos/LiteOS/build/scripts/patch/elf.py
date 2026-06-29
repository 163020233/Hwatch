#!/usr/bin/env python3
# coding=utf-8
# Copyright © Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
# Description: Parse elf file.
# Create: 2024-09-25

"""tools/elf.py"""


from struct import unpack
from dataclasses import dataclass


@dataclass
class SectSegment:
    """
    用于存放从解析后的各个段的信息
    """
    sect_text: str
    sect_bss: str
    sect_data: str
    sect_rodata: str


class ElfInfo:
    """
    ElfInfo
    """
    # section flag
    SHF_WRITE = 1
    SHF_ALLOC = 2
    SHF_EXECINSTR = 4

    # section type
    SHT_PROGBITS = 1
    SHT_NOBITS = 8

    def __init__(self, file):
        self.file = file
        self.file.seek(0)
        self.endian = '<'
        self.header = self._parse_header()
        self.sections = self._parse_section()
        self.symtab = self._parse_symtab()
        self.sect_rela = self._parse_rela_tab()
        sect_segment = self._parse_sect_segment()
        self.sect_text = sect_segment.sect_text
        self.sect_bss = sect_segment.sect_bss
        self.sect_data = sect_segment.sect_data
        self.sect_rodata = sect_segment.sect_rodata

        # 获取做patch需要的段

    def get_rela_table(self, section):
        """
        get_rela_table
        :param section:
        :return:
        """
        self.file.seek(section['sh_offset'])
        rela_tab = []
        rela_tabs_bytes = bytearray(self.file.read(section["sh_size"]))
        rela_tabs_bytes_list = []
        for i in range(0, len(rela_tabs_bytes), 24):
            rela_tabs_bytes_list.append(rela_tabs_bytes[i:i + 24])
        for sym in rela_tabs_bytes_list:
            rela_tabs_tag = {}
            rela_tabs_tag['r_offset'], = unpack('<Q', sym[0:8])
            rela_tabs_tag['r_info'], = unpack('<Q', sym[8:16])
            rela_tabs_tag['r_addend'], = unpack('<q', sym[16:24])
            rela_tab.append(rela_tabs_tag)
        return rela_tab

    def _parse_header(self):
        """
        _parse_header
        :return:
        """
        hdr = {}
        hdr['e_ident'] = unpack('4sccccc6sc', self.file.read(16))

        # 文件类型
        hdr['e_type'], hdr['e_machine'] = unpack("{}{}".format(self.endian, 'HH'), self.file.read(4))
        hdr['e_version'], = unpack('<I', self.file.read(4))

        # Entry point virtual address
        hdr['e_entry'], = unpack('<Q', self.file.read(8))

        # segment/section header file offset
        hdr['e_phoff'], hdr['e_shoff'] = unpack('<QQ', self.file.read(16))

        # Processor-specific flags, ELF Header size in bytes
        hdr['e_flags'], hdr['e_ehsize'] = unpack('<IH', self.file.read(6))

        # Program header table entry size, Program header table entry count
        hdr['e_phentsize'], hdr['e_phnum'] = unpack('<HH', self.file.read(4))

        # Section header table entry size, Section header table entry count
        hdr['e_shentsize'], hdr['e_shnum'] = unpack('<HH', self.file.read(4))

        # Section header string table index
        hdr['e_shtrndx'], = unpack('<H', self.file.read(2))

        return hdr

    def _parse_section(self):
        """
        _parse_section
        :return:
        """
        self.file.seek(self.header.get('e_shoff'))
        sections = []
        for _ in range(self.header.get("e_shnum")):
            section = {}
            # Type of segment, Segment attributes
            section['sh_name'], = unpack('<I', self.file.read(4))
            section['sh_type'], = unpack('<I', self.file.read(4))

            # Offset in file
            section['sh_flags'], section['sh_addr'] = \
                unpack('<QQ', self.file.read(16))

            # Virtual address in memory, Reserved
            section['sh_offset'], = unpack('<Q', self.file.read(8))

            # Size of segment in file, Size of segment in memory
            section['sh_size'], section['sh_link'] = \
                unpack('<QI', self.file.read(12))

            # Alignment of segment
            section['sh_info'], = unpack('<I', self.file.read(4))
            section['sh_addralign'], = unpack('<Q', self.file.read(8))
            section['sh_entsize'], = unpack('<Q', self.file.read(8))

            sections.append(section)

        for section in sections:
            self.file.seek(sections[self.header.get('e_shtrndx')]['sh_offset']
                           + section['sh_name'])
            str1 = b""
            char = self.file.read(1)
            while char != b"\x00":
                str1 += char
                char = self.file.read(1)
            section['sh_name'] = str1

        return sections

    def _getsym(self, byte: bytearray):
        """
        _getsym
        :param byte:
        :return:
        """
        sym = {}
        sym['name'], = unpack('<I', byte[0:4])
        sym['info'], sym['st_other'], = unpack('<BB', byte[4:6])
        sym['shndx'], = unpack('<H', byte[6:8])
        sym['value'], = unpack('<Q', byte[8:16])
        sym['size'], = unpack('<Q', byte[16:24])

        section_list = []
        for section in self.sections:
            if section["sh_name"] == b'.strtab':
                section_list.append(section)
        str_section = section_list[0]
        self.file.seek(str_section['sh_offset'] + sym.get('name'))
        str1 = b""
        char = self.file.read(1)
        while char != b"\x00":
            str1 += char
            char = self.file.read(1)
        sym['name'] = str1
        return sym

    def _parse_symtab(self):
        """
        获取符号表
        :return:
        """
        symtab = []
        section_list = []
        for section in self.sections:
            if section["sh_name"] == b'.symtab':
                section_list.append(section)
        section = section_list[0]
        self.file.seek(section["sh_offset"])
        symtab_bytes = bytearray(self.file.read(section["sh_size"]))
        symtab_bytes_list = []
        for i in range(0, len(symtab_bytes), 24):
            symtab_bytes_list.append(symtab_bytes[i:i + 24])
        for sym in symtab_bytes_list:
            symtab.append(self._getsym(sym))
        return symtab

    def _parse_rela_tab(self):
        """
        _parse_rela_tab
        :return:
        """
        rela_tabs = []
        for section in self.sections:
            # 4表示为可重定位段RELA
            if section['sh_type'] == 4:
                rela_tabs.append(section)
        return rela_tabs

    def _parse_sect_segment(self):
        """
        _parse_sect_segment
        :return:
        """
        sect_text = []
        sect_bss = []
        sect_data = []
        sect_rodata = []
        for section in self.sections:
            if int(section['sh_type']) == self.SHT_PROGBITS:
                if int(section['sh_flags']) & self.SHF_ALLOC \
                        and int(section['sh_flags']) & self.SHF_EXECINSTR:
                    sect_text.append(section)
                elif int(section['sh_flags']) & self.SHF_ALLOC \
                        and int(section['sh_flags']) & self.SHF_WRITE:
                    if section['sh_size'] == 0:
                        continue
                    sect_data.append(section)
                elif int(section['sh_flags']) & self.SHF_ALLOC:
                    sect_rodata.append(section)
            elif int(section['sh_type']) == self.SHT_NOBITS:
                if int(section['sh_flags']) & self.SHF_ALLOC \
                        and int(section['sh_flags']) & self.SHF_WRITE:
                    if section['sh_size'] == 0:
                        continue
                    sect_bss.append(section)
        sect_segment = SectSegment(sect_text, sect_bss, sect_data, sect_rodata)
        return sect_segment
