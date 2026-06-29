#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
'''
Data manager
'''

import os
import sys
import logging
from typing import Optional
from util.utils import Utils
from targets.base import TargetsBase
from parse.parse import ParseElf
from output.output import OutputManager, IOutputItem


class RelInfo(IOutputItem):
    '''
    Relocation info
    '''

    def __init__(self, targets, relinfo: dict):
        self._targets = targets
        self._raw_relinfo = relinfo

    def __str__(self):
        fmt = 'offset={},info={},addend={},value={},type={}'
        return fmt.format(self.offset, self.info, self.addend, self.value, self.type)

    @property
    def offset(self) -> int:
        '''
        offset address when loading
        '''
        return int(self._raw_relinfo['offset'], base=16)

    @property
    def info(self) -> int:
        '''
        relocation info value
        '''
        return self._targets.get_info(self._raw_relinfo)

    @property
    def addend(self) -> int:
        '''
        relocation addend
        '''
        if not isinstance(self._raw_relinfo['addend'], int):
            raise TypeError('invalid addend type')
        return self._raw_relinfo['addend']

    @property
    def value(self) -> int:
        '''
        relocation value
        '''
        addr = self._raw_relinfo['addr']
        if addr is not None:
            return addr
        else:
            return int(self._raw_relinfo['value'], base=16)

    @property
    def type(self) -> str:
        '''
        relocation type
        '''
        return self._raw_relinfo['type']

    def generate(self) -> str:
        '''
        generate c code of RelInfo
        '''
        reltype = self._raw_relinfo['reltype']
        if reltype == 'abs':
            fmt = "    {{0x{0:x}, 0x{1:x}, {2:}, 0x{3:x}}}, /* {4} */\n"
            return fmt.format(self.offset, self.info, self.addend, self.value, self.type)
        if reltype == 'rel':
            fmt = "    {{0x{0:x}, 0x{1:x}, {2:}, 0x{3:x}}}, /* {4}, {5} */\n"
            return fmt.format(self.offset, self.info, self.addend, self.value, self.type, 'REL')
        raise ValueError('bad reltype')


class DataManager():
    '''
    Class for relocation data
    '''

    CHECK = 'check'
    GENERATE = 'generate'

    def __init__(self, targets, parser):
        self.targets = targets
        self.parser = parser
        self._ldsize_symbols = self._build_ldsize_symbols()

    @classmethod
    def create_from_elf(cls, cmd: str, elffile: str, config_file: Optional[str] = None):
        '''
        create a new project by elf file and config file.
        '''
        def get_config_file_path(config_file):
            liteos_dir = os.path.dirname(os.path.realpath(__file__))
            liteos_dir = os.path.realpath(os.path.join(liteos_dir, '../../../..'))
            config_file_path = config_file if config_file else os.path.join(liteos_dir, '.config')
            if not os.path.isfile(config_file_path):
                raise ValueError('invalid config file')
            return config_file_path

        parser = ParseElf(cmd, elffile)

        file_path = get_config_file_path(config_file)
        targets = TargetsBase.create_from_config(file_path, parser)

        return cls(targets, parser)

    def output_to_file(self, dir_name: str, mode: str):
        '''
        generate gspr table source code and output to file
        '''
        def progress_update(curr, total):
            print("\r", end="")
            print("out progress: {}%: ".format(curr * 100 // total), \
                    "▋" * (curr * 100 // total), end="")
            sys.stdout.flush()

        def progress_finish():
            print("\r", end="")
            print("out progress: {}%: ".format(100), \
                "▋" * 100, end="")
            print("\n", end="")
            sys.stdout.flush()

        def iter_relinfo(relinfo_lst):
            total = len(relinfo_lst)
            for i, item in enumerate(relinfo_lst, start=1):
                progress_update(i, total)
                yield RelInfo(self.targets, item)
            progress_finish()

        def check_relinfo_not_ldsize(relinfo):
            '''
            check linkscript symbols contains size
            Check ldscript symbols shall not include 'size'. Using ldscript defined
            xxx_size in aslr is a bad usecase because of aslr may change the value
            of defined symbol.
            '''
            if relinfo.value in self._ldsize_symbols:
                sym = self._ldsize_symbols[relinfo.value]
                logging.warning("check if %s is size which defined in ldscripts should not shift, value %x", \
                                sym.name, sym.value)

        def build_relinfo():
            relinfo_lst = list(self.targets.iter_relinfo())
            if len(relinfo_lst) == 0:
                return relinfo_lst
            relinfo_lst.sort(key=lambda x : int(x['offset'], 16), reverse=False)
            return relinfo_lst

        relinfo_lst = build_relinfo()
        if not relinfo_lst:
            Utils.err_exit('relinfo is empty')

        if mode not in [self.CHECK, self.GENERATE]:
            raise ValueError('invalid mode')

        out_dir = os.path.join(dir_name, 'aslr', 'rel')
        out_file = 'aslr_gspr_info.c' if mode == self.GENERATE else 'aslr_gspr_info.c.check'
        out_file = os.path.join(out_dir, out_file)
        logging.info("%s file %s", mode, out_file)

        output_mgr = OutputManager()
        output_mgr.add_head()
        for relinfo in iter_relinfo(relinfo_lst):
            if mode == self.CHECK and self.targets.check_ldsize_symbol:
                check_relinfo_not_ldsize(relinfo)
            output_mgr.add_relinfo(relinfo)
        output_mgr.add_image_info(self.parser.reloc_type)
        output_mgr.add_end()

        output_mgr.output_to_file(out_file)

    def _build_ldsize_symbols(self):
        def is_ld_symbol(sym):
            return sym.name.startswith('__') and sym.type == 'NOTYPE'
        _ldsize_symbols = dict()
        for sym in [s for s in self.parser.iter_symbol_list() if is_ld_symbol(s)]:
            for keyword in ['size', 'SIZE', 'Size']:
                if keyword in sym.name:
                    _ldsize_symbols[sym.value] = sym
        return _ldsize_symbols
