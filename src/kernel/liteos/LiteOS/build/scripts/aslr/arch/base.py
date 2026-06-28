#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
'''
Arch base class
'''

import logging
from abc import ABC, abstractmethod
from typing import Optional, Tuple
from data_manager.data_type import RelInfoStruct
from parse.parse import ParseElf
from util.utils import Utils


class IArchBase(ABC):
    @property
    @abstractmethod
    def name(self):
        '''
        Get Arch Name
        '''
        raise NotImplementedError()

    @staticmethod
    @abstractmethod
    def relocinfo_mask(elfclass: str) -> int:
        '''
        The mask of the relocation type
        '''
        raise NotImplementedError()

    @classmethod
    @abstractmethod
    def create(cls, machine: str):
        '''
        create arch by machine name
        '''
        raise NotImplementedError()

    @classmethod
    @abstractmethod
    def get_reltype(cls, inst_type: str) -> Optional[str]:
        '''
        Get the relocation type
        '''
        raise NotImplementedError()

    @abstractmethod
    def reloc(self, info: RelInfoStruct, **kwargs) -> Tuple[bool, Optional[int]]:
        '''
        Do relocation
        '''
        raise NotImplementedError()

    @abstractmethod
    def get_mcode(self, parser: ParseElf, info: dict) -> object:
        '''
        get machine code by relinfo
        '''
        raise NotImplementedError()


class ISimpleInstArch:
    ''' arch that uses single instruction word, with fixed or variable length, eg. arm, riscv, etc... '''

    @staticmethod
    @abstractmethod
    def get_inst_width(inst: str) -> int:
        '''
        Get the instruction width by type
        '''


class ArchBase(IArchBase):
    '''
    Base class for arch
    '''
    arch_dict = {}

    def __init_subclass__(cls, arch_name, **kwargs):
        if arch_name in cls.arch_dict:
            logging.error(f'{arch_name} already registered')
            raise ValueError()
        super().__init_subclass__(**kwargs)
        cls.arch_name = arch_name
        cls.arch_dict[arch_name] = cls

    @property
    def name(self):
        return self.arch_name

    @staticmethod
    def relocinfo_mask(elfclass: str) -> int:
        '''
        The mask of the relocation type
        '''
        if elfclass == 'ELF32':  # ELF64 mask is 0xffffffff
            return 0xff
        raise NotImplementedError('not supported elfclass')

    @classmethod
    def create(cls, machine: str) -> IArchBase:
        if machine not in cls.arch_dict:
            logging.error(f'arch {machine} not registered')
            raise ValueError()
        return cls.arch_dict[machine]()

    def get_mcode(self, parser, info) -> int:
        if not isinstance(self, ISimpleInstArch):
            raise NotImplementedError('arch is not ISimpleInstArch')
        width = self.get_inst_width(info['type'])
        if not width:
            Utils.err_exit('not support inst type {}'.format(info['type']))
        mcode = parser.get_data(int(info['offset'], 16), width)
        mcode = int.from_bytes(mcode, byteorder=parser.endian, signed=False)
        return mcode
