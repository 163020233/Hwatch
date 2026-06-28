#!/usr/bin/env python3
# coding=utf-8
# Copyright © Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
# Description: Make crc.
# Create: 2024-09-25

"""tools/crc.py"""


import logging

logging.basicConfig(level=logging.DEBUG)

G_CRC_TABLE = []
for i in range(256):
    value = i << 8
    for _ in range(8):
        if value & 0x8000 != 0:
            value = (value << 1) ^ 0x1021
        else:
            value = value << 1
    G_CRC_TABLE.append(value & 0xffff)


def make_crc(byte_arr: bytearray, crc: int):
    """
    make_crc
    :param byte_arr:
    :param crc:
    :return:
    """
    if crc > 0xffffffff or crc < 0:
        logging.error("[error  ]input crc is %s!", crc)
        return False, crc
    for byte in byte_arr:
        crc = ((crc << 8) ^ G_CRC_TABLE[(crc >> 8) ^ byte]) & 0xffff
    return True, crc
