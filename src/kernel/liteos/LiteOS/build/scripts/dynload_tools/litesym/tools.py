#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.

'''
ELF parsing tools
'''

import logging
from collections import OrderedDict
from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection
from elftools.elf.constants import SH_FLAGS
from tables import LiteSymTab


class ELFFileExtendedTool:
    '''ELF File Handing Tool'''

    E_MACHINE_LINGLONG = 292

    def __init__(self, elffile: ELFFile, memview=True):
        self.elffile = elffile
        self._symbols = sorted(list(self._iter_visible_symbols()), key=lambda x: x.name)
        self._ptr_size = self.elffile.elfclass // 8
        self._alignment = self.elffile.elfclass // 8
        if self.elffile['e_machine'] == self.E_MACHINE_LINGLONG:
            self._alignment = 16
        if memview:
            self._memview = self._build_memview()
        else:
            self._memview = None

    @property
    def symbols(self):
        '''Visible symbol list'''
        return list(self._symbols)

    @staticmethod
    def _symbol_visible(entry):
        st_shndx = entry['st_shndx']
        st_bind = entry['st_info']['bind']
        st_type = entry['st_info']['type']
        return st_shndx != 'SHN_UNDEF' and (st_bind in ['STB_GLOBAL', 'STB_WEAK']) and \
            (st_type in ['STT_FUNC', 'STT_OBJECT'])

    def create_lite_symtab(self):
        '''Create lite symtab object'''
        return LiteSymTab.create_from_symbols(self.get_offset, self.symbols)

    def get_offset(self, symbol: str):
        '''Query symbol address in memory layout after dynload'''
        if not self._memview:
            return 0
        sec = self.elffile.get_section(symbol.entry['st_shndx'])
        return self._memview[sec.name] + symbol.entry['st_value']

    def _iter_visible_symbols(self):
        symtab = self._get_section_by_type(SymbolTableSection)
        for sym in symtab.iter_symbols():
            if self._symbol_visible(sym.entry):
                if (sym.entry['st_shndx'] & 0xFF00) != 0:
                    # COMMON symbols is forbidden to use
                    raise NotImplementedError("special section symbol parsing not implemented")
                yield sym

    def _get_section_by_type(self, wanted_type: type):
        for section in self.elffile.iter_sections():
            if isinstance(section, wanted_type):
                return section
        raise ValueError('section type not found')

    def _build_memview(self):
        def align(size: int):
            return (size + (self._alignment - 1)) & ~(self._alignment - 1)

        def section_loadable(section):
            return section['sh_flags'] & SH_FLAGS.SHF_ALLOC and section['sh_size'] != 0

        memview = OrderedDict()
        offset = 0
        for idx, section in enumerate(self.elffile.iter_sections()):
            if not section_loadable(section):
                continue
            logging.debug('%s(%u) load offset : %x', section.name, idx, offset)
            memview[section.name] = offset
            offset += align(section['sh_size'])
        return memview
