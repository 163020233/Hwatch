#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.

from fbb_ecc_service import *
from fbb_sign_params import *


SEGMENT_SIZE = 16

class FbbSignBase:
    '''Base class for FBB sign tool'''
    def __init__(self, img_id, src_file, dst_file, agent_flag):
        self.src_file = src_file
        self.dst_file = dst_file
        self.agent    = agent_flag

        if img_id not in sign_params:
            raise Exception("Not support image type.")

        self.params = sign_params[img_id]


    def fill_key_area(self):
        return bytearray()


    def fill_code_info(self, code_area_bin):
        return bytearray()


    def fill_code_area(self):
        try:
            with open(self.src_file, 'rb') as f:
                code_area_bin = f.read()

            length = len(code_area_bin)
            remain = length % SEGMENT_SIZE
            if (remain > 0):
                code_area_bin = code_area_bin + bytearray(SEGMENT_SIZE - remain)

            return code_area_bin

        except Exception as e:
            raise Exception('Fill code area error!')


    def sign_cmd(self, key_id, hash_bin):
        return sign_service(key_id, hash_bin, self.agent)


    def sign(self):
        try:
            code_area_bin = self.fill_code_area()
            key_area_bin = self.fill_key_area()
            code_info_bin = self.fill_code_info(code_area_bin)
            with open(self.dst_file, 'wb') as f:
                f.write(key_area_bin)
                f.write(code_info_bin)
                f.write(code_area_bin)
        except Exception as e:
            raise Exception(f'sign error! {e}')
