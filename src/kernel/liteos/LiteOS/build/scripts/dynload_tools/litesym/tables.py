#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.

'''
LiteOS Table Structures
'''

import logging
import struct


class LiteSymTab:
    '''LiteOS Symbol Table Structure'''

    def __init__(self, symlist, strtab):
        self.symlist = symlist
        self.strtab = strtab

    @classmethod
    def create_from_symbols(cls, get_offset, syms, ptr_len=4):
        '''Create LiteSymTab from symbols'''
        symlist = SymList(ptr_len)
        strtab = StrTab()
        for sym in syms:
            name_offset = strtab.add(sym.name)
            sym_offset = get_offset(sym)
            symlist.add(name_offset, sym_offset)
            logging.debug('added symbol %s(%x,%x)', sym.name, name_offset, sym_offset)
        return cls(symlist, strtab)

    def to_bytes(self):
        '''Serialize object'''
        header = self._make_header()
        symlist = self.symlist.to_bytes()
        strtab = self.strtab.to_bytes()
        return header + symlist + strtab

    def _make_header(self):
        return struct.pack('<I12x', self.symlist.num)


class SymList:
    '''LiteOS Symbol List Structure'''

    def __init__(self, ptr_len=4):
        self.num = 0
        self.ptr_len = ptr_len
        self._content = bytearray()

    def add(self, name_offset, addr_offset):
        '''Add a symbol'''
        fmt = '<II' if self.ptr_len == 4 else '<QQ'
        item_data = struct.pack(fmt, name_offset, addr_offset)
        self._content.extend(item_data)
        self.num += 1

    def to_bytes(self):
        '''Get bytes'''
        return bytes(self._content)


class StrTab:
    '''LiteOS String Table Structure'''

    def __init__(self):
        self._content = bytearray()

    @property
    def size(self):
        '''Total size of strtab'''
        return len(self._content)

    def add(self, symbol_name) -> int:
        '''Add a string'''
        curr = self.size
        self._content.extend(symbol_name.encode('ascii'))
        self._content.append(0)
        return curr

    def to_bytes(self):
        '''Get bytes'''
        return bytes(self._content)
