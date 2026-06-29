#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.

from ctypes          import *
from fbb_sign_base   import FbbSignBase
from fbb_ecc_service import *
from fbb_sign_params import *

import hashlib

uint32_t = c_uint32
uint8_t  = c_uint8


class EcdsaKeyArea(Structure):
    _fields_ = [
        ("image_id", uint32_t),
        ("stru_version", uint32_t),
        ("stru_length", uint32_t),
        ("sign_length", uint32_t),
        ("key_owner_id", uint32_t),
        ("key_id", uint32_t),
        ("key_alg", uint32_t),
        ("ecc_curve_type", uint32_t),
        ("key_length", uint32_t),
        ("key_version", uint32_t),
        ("key_version_mask", uint32_t),
        ("msid", uint32_t),
        ("msid_mask", uint32_t),
        ("maintenance_mode", uint32_t),
        ("die_id", uint8_t * 16),
        ("info_addr", uint32_t),
        ("reserved", uint8_t * 52),
        ("sub_public_key", uint8_t * 64),
        ("sign_key_area", uint8_t * 64),
    ]


class EcdsaCodeInfo(Structure):
    _fields_ = [
        ("image_id", uint32_t),
        ("stru_version", uint32_t),
        ("stru_length", uint32_t),
        ("sign_length", uint32_t),
        ("version", uint32_t),
        ("version_mask", uint32_t),
        ("msid", uint32_t),
        ("msid_mask", uint32_t),
        ("area_addr", uint32_t),
        ("area_length", uint32_t),
        ("area_hash", uint8_t * 32),
        ("enc_flag", uint32_t),
        ("protection_key", uint8_t * 32),
        ("iv", uint8_t * 16),
        ("compressed_flag", uint32_t),
        ("uncompressed_length", uint32_t),
        ("text_segment_size", uint32_t),
        ("reserved", uint8_t * 248),
        ("sign_info_area", uint8_t * 64),
        ("sign_info_area_ex", uint8_t * 64),
    ]


class FbbSignEcdsa(FbbSignBase):

    def fill_key_area(self):
        key_area_bin = bytearray(sizeof(EcdsaKeyArea))
        key_area = EcdsaKeyArea.from_buffer(key_area_bin)
        key_area.image_id = int(self.params['image_id'][0], 16)
        key_area.stru_version = key_area_stru_ver
        key_area.stru_length = key_area_stru_len_ecc_sm2
        key_area.sign_length = key_area_sig_len_ecc_sm2
        key_area.key_owner_id = key_area_key_owner_id
        key_area.key_id = key_area_key_id
        key_area.key_alg = key_area_key_alg_ecc
        key_area.ecc_curve_type = key_area_ecc_curve
        key_area.key_length = key_area_key_len_ecc_sm2
        key_area.key_version = int(self.params['key_version'], 16)
        key_area.key_version_mask = int(self.params['key_ver_mask'], 16)
        key_area.msid = key_area_msid
        key_area.msid_mask = key_area_msid_mask
        key_area.sub_public_key = (uint8_t * 64)(*get_public_key(self.params['key_name'][1]))
        key_area.sign_key_area = (uint8_t * 64)(*self.sign_cmd(self.params['key_name'][0], key_area_bin[:-64]))

        return key_area_bin


    def fill_code_info(self, code_area_bin):
        code_info_bin = bytearray(sizeof(EcdsaCodeInfo))
        code_info = EcdsaCodeInfo.from_buffer(code_info_bin)
        code_info.image_id = int(self.params['image_id'][1], 16)
        code_info.stru_version = code_info_stru_ver
        code_info.stru_length = code_info_stru_len_ecc_sm2
        code_info.sign_length = code_info_sig_len_ecc_sm2
        code_info.version = int(self.params['version'], 16)
        code_info.version_mask = int(self.params['version_mask'])
        code_info.msid = code_info_msid
        code_info.msid_mask = code_info_msid_mask
        code_info.area_addr = 0
        code_info.area_length = len(code_area_bin)
        code_info.area_hash = (uint8_t * 32)(*bytes.fromhex(hashlib.sha256(code_area_bin).hexdigest()))
        code_info.enc_flag = code_info_enc_flag_disable
        code_info.compressed_flag = code_info_compress_disable
        code_info.uncompressed_length = code_info.area_length
        code_info.sign_info_area = (uint8_t * 64)(*self.sign_cmd(self.params['key_name'][1], code_info_bin[:-128]))
        return code_info_bin
