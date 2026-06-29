#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.

'''
LiteOS dynload gsymbol checking tool
'''

import sys
import struct
import logging
from typing import List, Tuple
from argparse import ArgumentParser
from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection
from elftools.elf.constants import SH_FLAGS


class GlobalSymbolTableParser:
    ''' Parsing gsymbols from elf file.
    '''

    def __init__(self, elffile):
        if not isinstance(elffile, ELFFile):
            raise ValueError('invalid elffile')
        self.elffile = elffile

        self._cached_strtab = None
        self._cached_strtab_range = None
        self._cached_strtab_content = None

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.elffile.close()

    @staticmethod
    def _get_section_range(section):
        return range(section['sh_addr'], section['sh_addr'] + section.data_size)

    @classmethod
    def load(cls, filepath):
        '''load with file path
        '''
        elffile = ELFFile.load_from_path(filepath)
        return cls(elffile)

    def get_gsymbols(self) -> List[str]:
        '''get gsymbol name list
        '''
        gsymbol_beg, gsymbol_end = self._get_gsymbol_range()
        if gsymbol_beg == gsymbol_end:
            logging.info('gsymbol is empty, please check gsymbol is compiled and linked correctly.')
            return []
        section = self._get_section_by_vaddr(gsymbol_beg)
        offset_beg = gsymbol_beg - section['sh_addr']
        offset_end = gsymbol_end - section['sh_addr']
        content = section.data()
        content = content[offset_beg : offset_end]
        return self._parse_gsymbol_content(content)

    def _get_gsymbol_range(self) -> Tuple[int, int]:
        def get_symtab_section():
            for section in self.elffile.iter_sections():
                if isinstance(section, SymbolTableSection):
                    return section
            raise ValueError('no symtab found')

        # find symbol table segment
        section = get_symtab_section()
        # find gsymbol range
        gsymbol_beg = section.get_symbol_by_name('g_dynloadGlobalSymbolBegin')
        gsymbol_end = section.get_symbol_by_name('g_dynloadGlobalSymbolEnd')
        if gsymbol_beg is None or gsymbol_end is None:
            logging.debug('symbol not found, ignored')
            return 0, 0  # if no dynload API calling, symbol may be striped by the linker
        return gsymbol_beg[0].entry['st_value'], gsymbol_end[0].entry['st_value']

    def _parse_gsymbol_content(self, content: bytes) -> List[str]:
        symbols = []
        fmt = '<II' if self.elffile.elfclass == 32 else '<QQ'
        item_size = self.elffile.elfclass // 8 * 2
        logging.debug('item_size = %u', item_size)
        sym_addr_map = dict()
        for section in self.elffile.iter_sections(type='SHT_SYMTAB'):
            for sym in section.iter_symbols():
                try:
                    sym_list = sym_addr_map[sym['st_value']]
                except KeyError:
                    sym_list = []
                    sym_addr_map[sym['st_value']] = sym_list
                sym_list.append(sym.name)  # some symbols may share same address, add them to list
        for i in range(0, len(content), item_size):
            name_vaddr, symbol_vaddr = struct.unpack(fmt, content[i : i + item_size])
            logging.debug('item: (%x %x)', name_vaddr, symbol_vaddr)

            name = self._get_string_by_vaddr(name_vaddr)
            logging.debug('name string: %s', name)

            try:
                sym_list = sym_addr_map[symbol_vaddr]
            except KeyError as ex:
                raise ValueError('symbol address not found') from ex
            if name not in sym_list:  # check name and symbol is matched
                raise ValueError('table content invalid')
            symbols.append(name)
        return symbols

    def _get_section_by_vaddr(self, vaddr):
        logging.debug('find section vaddr=0x%x', vaddr)
        for section in self.elffile.iter_sections():
            if section['sh_flags'] & SH_FLAGS.SHF_ALLOC != 0:
                if vaddr in self._get_section_range(section):
                    return section
        raise ValueError('vaddr not found')

    def _get_string_by_vaddr(self, vaddr) -> str:
        if not self._cached_strtab or vaddr not in self._cached_strtab_range:
            if self._cached_strtab:
                logging.debug('string table (%s) cache missed, re-caching for 0x%x', self._cached_strtab.name, vaddr)
            self._cached_strtab = self._get_section_by_vaddr(vaddr)
            self._cached_strtab_range = self._get_section_range(self._cached_strtab)
            self._cached_strtab_content = self._cached_strtab.data()
        cstring_start = vaddr - self._cached_strtab['sh_addr']
        cstring_end = self._cached_strtab_content.find(0, cstring_start)
        cstring = self._cached_strtab_content[cstring_start : cstring_end]
        return cstring.decode('ascii')


def _check_gsymbol_sorted(gsymbols: List[str]):
    if sorted(gsymbols) != gsymbols:
        raise ValueError('gsymbols not sorted')


def _check_file(filename):
    logging.debug('filename=%s', filename)
    with GlobalSymbolTableParser.load(filename) as gsym_parser:
        gsymbols = gsym_parser.get_gsymbols()
        _check_gsymbol_sorted(gsymbols)


if __name__ == '__main__':
    parser = ArgumentParser(description='Validate LiteOS gsymbol section content.')
    parser.add_argument(dest='filename', metavar='filename', help='Executable ELF file')
    parser.add_argument('-v', '--verbose', dest='verbose', help='Verbose output', action='store_true')
    args = parser.parse_args()
    logging.basicConfig(level=(logging.DEBUG if args.verbose else logging.INFO))
    _check_file(args.filename)
    logging.info('CHECK PASSED')
    sys.exit(0)
