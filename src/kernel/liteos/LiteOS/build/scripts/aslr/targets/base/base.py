#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
'''
Targets base class
'''

import re
from typing import Optional, Iterable
from abc import ABC, abstractmethod
from arch.base import ArchBase
from parse.parse import ParseElf
from util.utils import Utils
from data_manager.data_type import RelInfoStruct


class ITargets(ABC):
    @property
    @abstractmethod
    def check_ldsize_symbol(self) -> bool:
        '''
        get target check symbol address enabled
        '''

    @property
    @abstractmethod
    def image_begin(self) -> int:
        '''
        begin address of image va
        '''

    @property
    @abstractmethod
    def image_end(self) -> int:
        '''
        end address of image va
        '''

    @property
    @abstractmethod
    def machine(self) -> str:
        '''
        target machine name
        '''

    @classmethod
    @abstractmethod
    def create_from_config(cls, config_file: str, parser: ParseElf):
        '''
        create target object from LiteOS config file.
        '''

    @abstractmethod
    def get_info(self, relinfo: dict) -> int:
        '''
        get reloc info field from relinfo
        '''

    @abstractmethod
    def iter_relinfo(self) -> Iterable[dict]:
        '''
        iter relinfos that required reloc
        '''


class TargetsBase(ITargets):
    '''
    Abstract base class for arch
    '''
    _targets_dict = {}
    _PLATFORM_REGEX = re.compile(r'^LOSCFG_PLATFORM="(.*)"$')
    _SAFETY_REGEX = re.compile(r'^LOSCFG_SAFETY_KERNEL=y$')

    def __init__(self, parser: ParseElf, arch: ArchBase, safety: bool = False):
        self._arch = arch
        self._safety = safety
        self._parser = parser

        va_start = self._parser.get_one_symtab_value('__image_start')
        va_stop = self._parser.get_one_symtab_value('__image_end')
        self._va_range = range(va_start, va_stop + 1)

    def __init_subclass__(cls, targets_name, **kwargs):
        if targets_name in cls._targets_dict:
            raise ValueError()
        super().__init_subclass__(**kwargs)
        cls._targets_name = targets_name
        cls._targets_dict[targets_name] = cls

    @property
    def image_begin(self) -> int:
        return self._va_range.start

    @property
    def image_end(self) -> int:
        return self._va_range.stop - 1

    @property
    def machine(self) -> str:
        return self._parser.machine

    @classmethod
    def create_from_config(cls, config_file: str, parser: ParseElf):
        def get_platform_name(config_file: str):
            with open(config_file, 'rt') as f:
                for line in f:
                    groups = cls._PLATFORM_REGEX.match(line)
                    if groups:
                        return groups.group(1)
            return None

        def get_isolated_os_enabled(config_file: str):
            with open(config_file, 'rt') as f:
                for line in f:
                    groups = cls._SAFETY_REGEX.match(line)
                    if groups:
                        return True
                return False

        targets_name = get_platform_name(config_file)
        if not targets_name:
            raise RuntimeError('platform not found')
        safety = get_isolated_os_enabled(config_file)
        arch = ArchBase.create(parser.machine)
        targets_cls = cls._targets_dict.get(targets_name)
        if not targets_cls:
            if arch.name == 'RISC-V':
                targets_cls = cls._targets_dict.get('default-riscv')
            else:
                targets_cls = cls._targets_dict.get('default')
        return targets_cls(parser, arch, safety)

    def iter_relinfo(self) -> Iterable[dict]:
        for raw in self._parser.iter_relinfo():
            raw = self._reloc(raw)
            if raw:
                yield raw

    def _reloc(self, raw: dict) -> Optional[dict]:
        '''
        Which relocType should do reloc and how to do reloc is a targets related behavier.
        By default, delegate all reloc request to arch, and each targets can override this
        behavier to customize reloc rules.
        '''
        def reltype_ok(reltype):
            if not reltype:
                Utils.err_exit('not support rel type: {}'.format(raw['type']))
            if 'value' not in raw:
                if (not reltype) or (reltype != 'noact'):
                    Utils.err_exit('not support {} no action rel {}'.format(self._parser.reloc_type, reltype))
                return False
            return True

        reltype = self._arch.get_reltype(raw['type'])
        if not reltype_ok(reltype):
            return None
        mcode = self._arch.get_mcode(self._parser, raw)
        new_relinfo = dict(raw)
        new_relinfo.update({'mcode' : mcode})
        new_relinfo.update({'reltype' : reltype})
        return self._reloc_addr(new_relinfo)

    def _reloc_addr(self, raw) -> Optional[dict]:
        # mcode relocType shType position symAdd addend gotAddr thumbFunc
        relinfo = RelInfoStruct(mcode=raw['mcode'], relocType=raw['type'], shType=self._parser.reloc_type,
                                position=raw['offset'], symAdd=raw['value'], addend=raw['addend'],
                                gotAddr=0, thumbFunc=0)
        ret, addr = self._arch.reloc(relinfo, parser=self._parser)
        if not ret:
            Utils.err_exit('inst reloc result not same with mcode {}'.format(raw))
        raw.update({'addr' : addr})
        return raw
