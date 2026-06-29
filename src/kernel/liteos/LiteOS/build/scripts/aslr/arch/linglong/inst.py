#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.

from typing import Optional, Iterable
from abc import ABC, abstractmethod
from parse.parse import ParseElf
from arch.linglong.machinecode import get_encoder


class ILingLongCode(ABC):
    '''
    LingLong Machine Code Basic Interface
    '''

    @property
    @abstractmethod
    def value(self) -> int:
        '''
        numeric value of machine code
        '''
        raise NotImplementedError()

    @property
    @abstractmethod
    def code(self) -> bytes:
        '''
        byte sequence of machine code
        '''
        raise NotImplementedError()

    @property
    @abstractmethod
    def address(self) -> int:
        '''
        memory begining of machine code
        '''
        raise NotImplementedError()


class ILingLongEncode(ILingLongCode):
    '''
    LingLong Encode Interface
    '''

    @classmethod
    @abstractmethod
    def create(cls, va: int, code: bytes):
        '''
        factory method of object creation
        '''
        raise NotImplementedError()


class LingLongEncoding(ILingLongEncode):
    '''
    LingLong Encoding Base Abstraction
    '''

    CODE_LEN = 0

    def __init__(self, va: int, code: bytes):
        self._code = code
        self._va = va

    def __len__(self):
        return len(self._code)

    @property
    def code(self) -> bytes:
        # convert it to big-endian, for easier use
        return bytes(self._code[::-1])

    @property
    def value(self) -> int:
        return int.from_bytes(self._code, byteorder='little')

    @property
    def address(self) -> int:
        return self._va

    @classmethod
    def create(cls, va: int, code: bytes):
        if len(code) != cls.CODE_LEN:
            raise ValueError('code len does not matched')
        decoded = cls.decode(va, code)
        return cls(va, bytes(decoded))

    @classmethod
    @abstractmethod
    def decode(cls, va: int, code: bytes) -> Iterable[int]:
        '''
        convert machine code order by address from input
        '''
        raise NotImplementedError()

    @classmethod
    @abstractmethod
    def encode(cls, va: int, code: bytes) -> Iterable[int]:
        '''
        convert machine code order by address to output
        '''
        raise NotImplementedError()


class SingleEncodingCode(LingLongEncoding):
    '''
    Single Byte Encoding
    '''

    CODE_LEN = 1

    @classmethod
    def decode(cls, va: int, code: bytes) -> Iterable[int]:
        return code

    @classmethod
    def encode(cls, va: int, code: bytes) -> Iterable[int]:
        return code


class ParityEncodingCode(LingLongEncoding):
    '''
    Parity Encoding
    '''

    CODE_LEN = 2

    @classmethod
    def decode(cls, va: int, code: bytes) -> Iterable[int]:
        decoded = list(code)
        if va % 2 != 0:
            decoded[0] = code[1]
            decoded[1] = code[0]
        return decoded

    @classmethod
    def encode(cls, va: int, code: bytes) -> Iterable[int]:
        encoded = list(code)
        if va % 2 != 0:
            encoded[0] = code[1]
            encoded[1] = code[0]
        return encoded


class RotateEncodingCode(LingLongEncoding):
    '''
    Rotate Encoding
    '''

    CODE_LEN = 4

    @classmethod
    def decode(cls, va: int, code: bytes) -> Iterable[int]:
        decoded = [0] * cls.CODE_LEN
        first = va % cls.CODE_LEN
        for i in range(cls.CODE_LEN):
            decoded[(first + i) % cls.CODE_LEN] = code[i]
        return decoded

    @classmethod
    def encode(cls, va: int, code: bytes) -> Iterable[int]:
        encoded = [0] * cls.CODE_LEN
        first = va % cls.CODE_LEN
        for i in range(cls.CODE_LEN):
            code[i] = encoded[(first + i) % cls.CODE_LEN]
        return encoded


class MainWord(ParityEncodingCode):
    '''
    Main word part of a instruction
    '''

    _EOV_MASK = 0x8000
    _INST_FORMAT_TBL = [
        {'name' : 'f16', 'len' : 2, 'suffix_mask' : 0x4000, 'suffix' : 0x0000},
        {'name' : 'f24', 'len' : 3, 'suffix_mask' : 0x6000, 'suffix' : 0x4000},
        {'name' : 'f32', 'len' : 4, 'suffix_mask' : 0x7000, 'suffix' : 0x6000},
        {'name' : 'f48', 'len' : 6, 'suffix_mask' : 0x7800, 'suffix' : 0x7000},
    ]

    @property
    def code(self) -> bytes:
        code = list(super().code)
        code[0] = code[0] & ~0x80
        return bytes(code)

    @property
    def eov(self) -> bool:
        '''
        end of VLIW
        '''
        return (self.value & self._EOV_MASK) != 0

    @property
    def ext_len(self) -> int:
        '''
        expected extend word width
        '''
        for inst_fmt in self._INST_FORMAT_TBL:
            if (self.value & inst_fmt['suffix_mask']) == inst_fmt['suffix']:
                return inst_fmt['len'] - self.CODE_LEN
        raise ValueError('invalid main code')


class ExtWord(ILingLongEncode):
    '''
    Extend word part of a instruction
    '''

    def __init__(self, ext_word: LingLongEncoding):
        self._ext_word = ext_word

    def __len__(self):
        return len(self._ext_word)

    @property
    def value(self) -> int:
        return self._ext_word.value

    @property
    def code(self) -> bytes:
        return self._ext_word.code

    @property
    def address(self) -> int:
        return self._ext_word.address

    @classmethod
    def create(cls, va: int, code: bytes):
        if not code:
            return None
        for code_cls in [SingleEncodingCode, ParityEncodingCode, RotateEncodingCode]:
            if len(code) == code_cls.CODE_LEN:
                break
        if code_cls.CODE_LEN != len(code):
            raise ValueError('cannot match code len')
        return cls(code_cls.create(va, code))


class InstWord(ILingLongCode):
    '''
    Linglong instruction. Linglong instrction contains a main word and maybe a extend word.
    '''

    def __init__(self, main_word: MainWord, ext_word: Optional[ExtWord]):
        if main_word.ext_len != 0 and (ext_word is None or main_word.ext_len != len(ext_word)):
            raise ValueError('main/ext word len not matched')
        self.main_word = main_word
        self.ext_word = ext_word
        self._encoder = None

    @property
    def imm(self):
        '''
        immediate value of a instruction.
        '''
        return self.encoder.get_value(self.code[::-1])

    @property
    def code(self) -> bytes:
        if not self.ext_word:
            return self.main_word.code
        return self.ext_word.code + self.main_word.code

    @property
    def value(self) -> int:
        return self.main_word.value << len(self.ext_word * 8) | self.ext_word.code

    @property
    def encoder(self):
        '''
        machine code encoder.
        '''
        # Encoder does not support all machine code, just try to get encoder will needed.
        if not self._encoder:
            self._encoder = get_encoder(self.code[::-1])
        return self._encoder

    @property
    def address(self) -> int:
        return self.main_word.address


class VLIW:
    '''
    A VLIW. VLIW may contains one or more instructions.
    '''

    def __init__(self):
        self._iw = []

    def __getitem__(self, idx: int):
        return self.slot(idx)

    @property
    def base(self):
        '''
        base address of VLIW
        '''
        return self[0].address

    @classmethod
    def create_from_va(cls, va: int, parser: ParseElf):
        '''
        factory method for object creation
        '''
        vliw = cls()
        curr_va = va
        main_word_list = []
        # get main word list first
        while True:
            main_word = parser.get_data(curr_va, MainWord.CODE_LEN)
            main_word = MainWord.create(curr_va, main_word)
            main_word_list.append(main_word)
            curr_va += 2
            if main_word.eov:
                break
        # use main word info to get extend word, then combine to inst word
        for main_word in main_word_list:
            ext_word = parser.get_data(curr_va, main_word.ext_len)
            ext_word = ExtWord.create(curr_va, ext_word)
            inst_word = InstWord(main_word, ext_word)
            vliw.add(inst_word)
            curr_va += main_word.ext_len
        return vliw

    def add(self, inst_word: InstWord):
        '''
        add a instruction to VLIW
        '''
        self._iw.append(inst_word)

    def slot(self, idx: int) -> InstWord:
        '''
        get instruction for idx of slots
        '''
        if idx < 0:
            raise ValueError('invalid slot index')
        return self._iw[idx]
