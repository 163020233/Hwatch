#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.

import re
from collections import OrderedDict


class EncodeType:
    '''
    Encode type
    '''

    _instance_mapping = dict()

    def __new__(cls, *args, **kwargs):
        name = args[0]
        if name not in cls._instance_mapping:
            cls._instance_mapping[name] = super().__new__(cls)
        return cls._instance_mapping[name]

    def __init__(self, name):
        self._name = name
        matched = re.match('.*(S|M|U)([0-9]{1,2}).*', self._name)
        if not matched:
            raise ValueError('no type info found')
        self._sign_type = matched[1]
        self._imm_width = int(matched[2])

    @property
    def name(self):
        '''
        encode name
        '''
        return self._name

    @property
    def imm_signed(self):
        '''
        immediate signed
        '''
        return self._sign_type == 'S'

    @property
    def imm_width(self):
        '''
        immediate value width
        '''
        return self._imm_width


class EncodeReader:
    '''
    Linglong machine code reader
    '''

    def __init__(self, main_start, main_width, ext_start, ext_width, shift):
        self._main_start = main_start
        self._main_width = main_width
        self._ext_start = ext_start
        self._ext_width = ext_width
        self._shift = shift

    def get_raw_value(self, code: bytes, byteorder='little') -> int:
        '''
        raw immediate value without sign
        '''
        def get_bitfield(value, startbit, bitwidth):
            mask = (1 << bitwidth) - 1
            shifted_value = value >> startbit
            return shifted_value & mask
        main_word = int.from_bytes(code[0:2], byteorder)
        ext_word = int.from_bytes(code[2:], byteorder)
        val_main = get_bitfield(main_word, self._main_start, self._main_width)
        val_ext = get_bitfield(ext_word, self._ext_start, self._ext_width)
        imm = val_main | (val_ext << self._main_width)
        return imm << self._shift


class Encoder:
    '''
    Linglong instruction encoder and decoder.
    '''

    _ENCODER_READER_TABLE = OrderedDict([
        # ----,----,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,x---
        ('TRG__X__U32_D__X__F48', EncodeReader(8, 3, 0, 29, 0)),
        # ----,----,----,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx
        ('TRG__X__U32__X__F48', EncodeReader(0, 0, 0, 32, 0)),
        # ----,----,--x-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx-
        ('TRG__X__U32_B__X__F48', EncodeReader(10, 1, 0, 31, 0)),
        # ----,---x,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx-,----
        ('U31_SHIFT1_B__X__F48', EncodeReader(7, 4, 0, 27, 1)),
        # ----,----,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx-,----
        ('U30_SHIFT2_B__X__F48', EncodeReader(8, 3, 0, 27, 2)),
        # ----,--xx,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx-,----
        ('U32_B__X__F48', EncodeReader(6, 5, 0, 27, 0)),
        # ----,--xx,xxx-,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxx-,----
        ('M32__X__F48', EncodeReader(6, 5, 0, 27, 0)),
        # ----,-xxx,xxxx,xx--
        ('TRG__X__REL_S9__X__F16', EncodeReader(5, 9, 0, 0, 0)),
        # ----,---x,xxxx,x---,xxxx,xxxx
        ('TRG__X__REL_S14__X__F24', EncodeReader(7, 6, 0, 8, 0)),
        # ----,--xx,xxxx,----,xxxx,xxxx,xxxx,xxxx
        ('TRG__X__REL_S22__X__F32', EncodeReader(6, 6, 0, 16, 0)),
        # ----,--xx,xxxx,x---,xxx-,----
        ('TRG__X__REL_S10_C__X__F24', EncodeReader(6, 7, 0, 3, 0)),
        # ----,---x,xxxx,----,xxxx,xxxx,xxx-,----
        ('TRG__X__REL_S16_C__X__F32', EncodeReader(7, 5, 0, 11, 0)),
        # ----,----,----,----,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx
        ('TRG__X__REL_S32__X__F48', EncodeReader(0, 0, 0, 32, 0)),
        # ----,----,----,----,----,xxxx,xxxx,xxxx
        ('TRG__X__REL_U12__X__F32', EncodeReader(0, 0, 4, 12, 0)),
        # ----,----,----,----,----,----,----,----,---x,xxxx,xxxx,xxxx
        ('TRG__X__REL_U13__X__F48', EncodeReader(0, 0, 19, 13, 0)),
        # ----,----,----,----,----,xxxx,xxxx,xxx-,----,----,----,----
        ('REL_HWL_U11_BEG__X__F48', EncodeReader(0, 0, 4, 11, 0)),
        # ----,----,----,----,----,----,----,---x,xxxx,xxxx,xx--,----
        ('REL_HWL_U11_END__X__F48', EncodeReader(0, 0, 15, 11, 0)),
        # ----,----,----,----,-xxx,xxxx,xxxx,----,----,----,----,----
        ('REL_HWL_U11_B_BEG__X__F48', EncodeReader(0, 0, 1, 11, 0)),
        # ----,----,----,----,----,----,----,xxxx,xxxx,xxx-,----,----
        ('REL_HWL_U11_B_END__X__F48', EncodeReader(0, 0, 12, 11, 0)),
    ])

    _ENCODER_NAME_LIST = list(_ENCODER_READER_TABLE.keys())

    def __init__(self, name: str):
        if name not in self._ENCODER_READER_TABLE:
            raise ValueError('unknown encoder name')
        self.reader = self._ENCODER_READER_TABLE[name]
        self.type = EncodeType(name)

    @property
    def index(self):
        '''
        index of encoder
        '''
        return self._ENCODER_NAME_LIST.index(self.type.name)

    def get_value(self, code: bytes, byteorder='little') -> int:
        '''
        get immediate value with sign
        '''
        def sign_extend(value, bits):
            sign_bit = 1 << (bits - 1)
            return (value & (sign_bit - 1)) - (value & sign_bit)
        raw_value = self.reader.get_raw_value(code, byteorder=byteorder)
        if self.type.imm_signed:
            return sign_extend(raw_value, self.type.imm_width)
        return raw_value


MACHINE_CODE_TBL = [
    # call.intra.val@pcu.f48 0000,010-,---0,1110,----,----,----,----,----,----,----,----
    {'code' : '207000000000', 'mask' : '7f7800000000', 'encoding' : 'TRG__X__U32_D__X__F48'},
    # call.intra.val@pcu.f48 0000,0010,---0,1110,----,----,----,----,----,----,----,----
    {'code' : '407000000000', 'mask' : 'ff7800000000', 'encoding' : 'TRG__X__U32_D__X__F48'},
    # call.val@pcu.f48 0000,011-,---0,1110,----,----,----,----,----,----,----,----
    {'code' : '607000000000', 'mask' : '7f7800000000', 'encoding' : 'TRG__X__U32_D__X__F48'},
    # call.val@pcu.f48 0000,0011,---0,1110,----,----,----,----,----,----,----,----
    {'code' : 'c07000000000', 'mask' : 'ff7800000000', 'encoding' : 'TRG__X__U32_D__X__F48'},
    # j@pcu.f48 0000,0000,1110,1110,----,----,----,----,----,----,----,----
    {'code' : '007700000000', 'mask' : 'ff7f00000000', 'encoding' : 'TRG__X__U32__X__F48'},
    # j.cf@pcu.f48 0000,0000,0110,1110,----,----,----,----,----,----,----,----
    {'code' : '007600000000', 'mask' : 'ff7f00000000', 'encoding' : 'TRG__X__U32__X__F48'},
    # j.ct@pcu.f48 0000,0000,1000,1110,----,----,----,----,----,----,----,----
    {'code' : '007100000000', 'mask' : 'ff7f00000000', 'encoding' : 'TRG__X__U32__X__F48'},
    # j.intra.cf@pcu.f48 0000,0000,1010,1110,----,----,----,----,----,----,----,----
    {'code' : '007500000000', 'mask' : 'ff7f00000000', 'encoding' : 'TRG__X__U32__X__F48'},
    # j.intra.ct@pcu.f48 0000,0000,1100,1110,----,----,----,----,----,----,----,----
    {'code' : '007300000000', 'mask' : 'ff7f00000000', 'encoding' : 'TRG__X__U32__X__F48'},
    # lbeginit@pcu.f48 0000,0001,00-0,1110,----,----,----,----,----,----,----,----
    {'code' : '807000000000', 'mask' : 'ff7b00000000', 'encoding' : 'TRG__X__U32_B__X__F48'},
    # ld.a16.u16.aa@agx.f48 0000,110-,---0,1110,----,----,----,----,----,----,----,----
    {'code' : '307000000000', 'mask' : '7f7800000000', 'encoding' : 'U31_SHIFT1_B__X__F48'},
    # ld.a32.aa@agx.f48 0000,1001,---0,1110,----,----,----,----,----,----,----,----
    {'code' : '907000000000', 'mask' : 'ff7800000000', 'encoding' : 'U30_SHIFT2_B__X__F48'},
    # ld.a8.u8.aa@agx.f48 0001,00--,---0,1110,----,----,----,----,----,----,----,----
    {'code' : '087000000000', 'mask' : '3f7800000000', 'encoding' : 'U32_B__X__F48'},
    # lendinit@pcu.f48 0000,0001,01-0,1110,----,----,----,----,----,----,----,----
    {'code' : '807200000000', 'mask' : 'ff7b00000000', 'encoding' : 'TRG__X__U32_B__X__F48'},
    # mov.i@agx.f48 0001,01--,---0,1110,----,----,----,----,----,----,----,----
    {'code' : '287000000000', 'mask' : '3f7800000000', 'encoding' : 'M32__X__F48'},
    # mov.i@sau.f48 0010,00--,---0,1110,----,----,----,----,----,----,----,----
    {'code' : '047000000000', 'mask' : '3f7800000000', 'encoding' : 'M32__X__F48'},
    # st.a16.aa@agx.f48 0000,111-,---0,1110,----,----,----,----,----,----,----,----
    {'code' : '707000000000', 'mask' : '7f7800000000', 'encoding' : 'U31_SHIFT1_B__X__F48'},
    # st.a32.aa@agx.f48 0000,1010,---0,1110,----,----,----,----,----,----,----,----
    {'code' : '507000000000', 'mask' : 'ff7800000000', 'encoding' : 'U30_SHIFT2_B__X__F48'},
    # st.a8.aa@agx.f48 0001,10--,---0,1110,----,----,----,----,----,----,----,----
    {'code' : '187000000000', 'mask' : '3f7800000000', 'encoding' : 'U32_B__X__F48'},
    # b@pcu.f16 0010,1---,----,--00
    {'code' : '140000000000', 'mask' : '1f4000000000', 'encoding' : 'TRG__X__REL_S9__X__F16'},
    # b@pcu.f24 0000,101-,----,-010,----,----
    {'code' : '504000000000', 'mask' : '7f6000000000', 'encoding' : 'TRG__X__REL_S14__X__F24'},
    # b@pcu.f32 0001,11--,----,0110,----,----,----,----
    {'code' : '386000000000', 'mask' : '3f7000000000', 'encoding' : 'TRG__X__REL_S22__X__F32'},
    # b.cf@pcu.f16 0000,1---,----,--00
    {'code' : '100000000000', 'mask' : '1f4000000000', 'encoding' : 'TRG__X__REL_S9__X__F16'},
    # b.cf@pcu.f24 0000,001-,----,-010,----,----
    {'code' : '404000000000', 'mask' : '7f6000000000', 'encoding' : 'TRG__X__REL_S14__X__F24'},
    # b.cf@pcu.f32 0000,11--,----,0110,----,----,----,----
    {'code' : '306000000000', 'mask' : '3f7000000000', 'encoding' : 'TRG__X__REL_S22__X__F32'},
    # b.ct@pcu.f16 0001,0---,----,--00
    {'code' : '080000000000', 'mask' : '1f4000000000', 'encoding' : 'TRG__X__REL_S9__X__F16'},
    # b.ct@pcu.f24 0000,010-,----,-010,----,----
    {'code' : '204000000000', 'mask' : '7f6000000000', 'encoding' : 'TRG__X__REL_S14__X__F24'},
    # b.ct@pcu.f32 0001,00--,----,0110,----,----,----,----
    {'code' : '086000000000', 'mask' : '3f7000000000', 'encoding' : 'TRG__X__REL_S22__X__F32'},
    # b.eqz@pcu.f24 0001,00--,----,-010,----,----
    {'code' : '084000000000', 'mask' : '3f6000000000', 'encoding' : 'TRG__X__REL_S10_C__X__F24'},
    # b.eqz@pcu.f32 0000,001-,----,0110,----,----,----,----
    {'code' : '406000000000', 'mask' : '7f7000000000', 'encoding' : 'TRG__X__REL_S16_C__X__F32'},
    # b.intra.cf@pcu.f16 0001,1---,----,--00
    {'code' : '180000000000', 'mask' : '1f4000000000', 'encoding' : 'TRG__X__REL_S9__X__F16'},
    # b.intra.cf@pcu.f24 0000,011-,----,-010,----,----
    {'code' : '604000000000', 'mask' : '7f6000000000', 'encoding' : 'TRG__X__REL_S14__X__F24'},
    # b.intra.cf@pcu.f32 0001,01--,----,0110,----,----,----,----
    {'code' : '286000000000', 'mask' : '3f7000000000', 'encoding' : 'TRG__X__REL_S22__X__F32'},
    # b.intra.ct@pcu.f16 0010,0---,----,--00
    {'code' : '040000000000', 'mask' : '1f4000000000', 'encoding' : 'TRG__X__REL_S9__X__F16'},
    # b.intra.ct@pcu.f24 0000,100-,----,-010,----,----
    {'code' : '104000000000', 'mask' : '7f6000000000', 'encoding' : 'TRG__X__REL_S14__X__F24'},
    # b.intra.ct@pcu.f32 0001,10--,----,0110,----,----,----,----
    {'code' : '186000000000', 'mask' : '3f7000000000', 'encoding' : 'TRG__X__REL_S22__X__F32'},
    # b.intra.eqz@pcu.f24 0001,01--,----,-010,----,----
    {'code' : '284000000000', 'mask' : '3f6000000000', 'encoding' : 'TRG__X__REL_S10_C__X__F24'},
    # b.intra.eqz@pcu.f32 0000,010-,----,0110,----,----,----,----
    {'code' : '206000000000', 'mask' : '7f7000000000', 'encoding' : 'TRG__X__REL_S16_C__X__F32'},
    # b.intra.neqz@pcu.f24 0001,10--,----,-010,----,----
    {'code' : '184000000000', 'mask' : '3f6000000000', 'encoding' : 'TRG__X__REL_S10_C__X__F24'},
    # b.intra.neqz@pcu.f32 0000,011-,----,0110,----,----,----,----
    {'code' : '606000000000', 'mask' : '7f7000000000', 'encoding' : 'TRG__X__REL_S16_C__X__F32'},
    # b.neqz@pcu.f24 0001,11--,----,-010,----,----
    {'code' : '384000000000', 'mask' : '3f6000000000', 'encoding' : 'TRG__X__REL_S10_C__X__F24'},
    # b.neqz@pcu.f32 0000,100-,----,0110,----,----,----,----
    {'code' : '106000000000', 'mask' : '7f7000000000', 'encoding' : 'TRG__X__REL_S16_C__X__F32'},
    # call@pcu.f24 0000,111-,----,-010,----,----
    {'code' : '704000000000', 'mask' : '7f6000000000', 'encoding' : 'TRG__X__REL_S14__X__F24'},
    # call@pcu.f32 0010,01--,----,0110,----,----,----,----
    {'code' : '246000000000', 'mask' : '3f7000000000', 'encoding' : 'TRG__X__REL_S22__X__F32'},
    # call@pcu.f48 0000,0000,0100,1110,----,----,----,----,----,----,----,----
    {'code' : '007200000000', 'mask' : 'ff7f00000000', 'encoding' : 'TRG__X__REL_S32__X__F48'},
    # call.intra@pcu.f24 0000,110-,----,-010,----,----
    {'code' : '304000000000', 'mask' : '7f6000000000', 'encoding' : 'TRG__X__REL_S14__X__F24'},
    # call.intra@pcu.f32 0010,00--,----,0110,----,----,----,----
    {'code' : '046000000000', 'mask' : '3f7000000000', 'encoding' : 'TRG__X__REL_S22__X__F32'},
    # call.intra@pcu.f48 0000,0000,0010,1110,----,----,----,----,----,----,----,----
    {'code' : '007400000000', 'mask' : 'ff7f00000000', 'encoding' : 'TRG__X__REL_S32__X__F48'},
    # lbreak.cf@pcu.f32 0000,0000,0000,0110,0001,----,----,----
    {'code' : '006008000000', 'mask' : 'ff7f0f000000', 'encoding' : 'TRG__X__REL_U12__X__F32'},
    # lbreak.ct@pcu.f32 0000,0000,0000,0110,0010,----,----,----
    {'code' : '006004000000', 'mask' : 'ff7f0f000000', 'encoding' : 'TRG__X__REL_U12__X__F32'},
    # lcont.cf@pcu.f32 0000,0000,0000,0110,0011,----,----,----
    {'code' : '00600c000000', 'mask' : 'ff7f0f000000', 'encoding' : 'TRG__X__REL_U12__X__F32'},
    # lcont.ct@pcu.f32 0000,0000,0000,0110,0100,----,----,----
    {'code' : '006002000000', 'mask' : 'ff7f0f000000', 'encoding' : 'TRG__X__REL_U12__X__F32'},
    # lskip.eqz@pcu.f48 0000,0000,0000,1110,0000,0000,0000,0000,000-,----,----,----
    {'code' : '007000000000', 'mask' : 'ff7fffff0700', 'encoding' : 'TRG__X__REL_U13__X__F48'},
    # lskip.leqz@pcu.f48 0000,0000,0000,1110,0000,0000,0000,0000,001-,----,----,----
    {'code' : '007000000400', 'mask' : 'ff7fffff0700', 'encoding' : 'TRG__X__REL_U13__X__F48'},
    # lallinit.a@pcu.f48 0000,0000,0000,1110,0001,----,----,----,----,----,----,----
    {'code' : '007008000000', 'mask' : 'ff7f0f000000', 'encoding' : 'REL_HWL_U11_BEG__X__F48'},
    # lallinit.i@pcu.f48 0000,0000,0000,1110,1---,----,----,----,----,----,----,----
    {'code' : '007001000000', 'mask' : 'ff7f01000000', 'encoding' : 'REL_HWL_U11_B_BEG__X__F48'},
    # lallinit.a@pcu.f48 0000,0000,0000,1110,0001,----,----,----,----,----,----,----
    {'code' : '007008000000', 'mask' : 'ff7f0f000000', 'encoding' : 'REL_HWL_U11_END__X__F48'},
    # lallinit.i@pcu.f48 0000,0000,0000,1110,1---,----,----,----,----,----,----,----
    {'code' : '007001000000', 'mask' : 'ff7f01000000', 'encoding' : 'REL_HWL_U11_B_END__X__F48'},
]

_cached_machine_code_table = None


def _init():
    global _cached_machine_code_table
    _cached_machine_code_table = list()
    for rule in MACHINE_CODE_TBL:
        item = dict()
        item['code'] = bytearray.fromhex(rule['code'])
        item['mask'] = bytearray.fromhex(rule['mask'])
        item['encoder'] = Encoder(rule['encoding'])
        _cached_machine_code_table.append(item)


def get_encoder(code: bytes) -> Encoder:
    '''
    get instruction encoder from machine code
    '''
    if not _cached_machine_code_table:
        _init()
    code = code.ljust(6, b'\0')
    for item in _cached_machine_code_table:
        found = True
        for input_byte, mask_byte, code_byte in zip(code, item['mask'], item['code']):
            if input_byte & mask_byte != code_byte:
                found = False
                break
        if found:
            return item['encoder']
    raise ValueError('encoder not found')
