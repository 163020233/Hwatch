#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.

from ctypes          import *
from fbb_ecc_service import *
from fbb_sign_base   import FbbSignBase
from fbb_sign_params import *
import hashlib

uint32_t = c_uint32
uint8_t  = c_uint8

class EcdsaRootPublicKey(Structure):
    _fields_ = [
        ("image_id", uint32_t),
        ("stru_version", uint32_t),
        ("stru_length", uint32_t),
        ("key_owner_id", uint32_t),
        ("key_id", uint32_t),
        ("key_alg", uint32_t),
        ("ecc_curve_type", uint32_t),
        ("key_length", uint32_t),
        ("reserved", uint8_t * 32),
        ("public_key", uint8_t * 64),
    ]


class FbbEccRootKey:

    '''Class for root public key'''
    def __init__(self, dst_file):
        self.dst_file = dst_file
        self.params = sign_params['root_key']

    def generate(self):
        print(f'{sizeof(EcdsaRootPublicKey)}')

        root_key_bin = bytearray(sizeof(EcdsaRootPublicKey))
        root_key = EcdsaRootPublicKey.from_buffer(root_key_bin)
        root_key.image_id = int(self.params['image_id'][0], 16)
        root_key.stru_version = key_area_stru_ver
        root_key.stru_length = sizeof(EcdsaRootPublicKey)
        root_key.key_alg = key_area_key_alg_ecc
        root_key.ecc_curve_type = key_area_ecc_curve
        root_key.key_length = key_area_key_len_ecc_sm2
        root_key.public_key = (uint8_t * 64)(*get_public_key(self.params['key_name'][0]))

        with open(self.dst_file, 'wb') as f:
            f.write(root_key_bin)

        with open(f'{self.dst_file}.hash', 'w') as f:
            hash_val = hashlib.sha256(root_key_bin).hexdigest().zfill(64).upper()
            f.write(hash_val)
            print(f'hash: {hash_val}')
