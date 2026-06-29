#!/usr/bin/env python3
# coding=utf-8
# Copyright © Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
# Description: Parse patch file.
# Create: 2024-09-25

"""tools/pat.py"""


from struct import unpack
from struct import pack
import logging

logging.basicConfig(level=logging.DEBUG)

PAT_HEAD_LEN = 80
PAT_HPHEAD_LEN = 176
PAT_UNIT_LEN = 20


class PatInfo:
    """
    PatInfo
    """

    def __init__(self, file):
        self.pat_file = file
        self.pat_header = self._parse_head()
        self.pat_hpuint_index = self._parse_hp_index()
        self.pat_hpfile_info = self._get_hp_file_header_info()

    @staticmethod
    def _get_hpfile(hp_index_byte: bytearray):
        """
        _get_hpfile
        :param hp_index_byte:
        :return:
        """
        hp_index = {
                    'HPVersion': b'\x02\x01\x01\x03',
                    'HPFlag': b'SP',
                    'HPFileName': b'SP0001',
                    'HPCrc': 19046,
                    'ndependFlag': 0,
                    'InterPatchNo': 1,
                    'ProgVersion': b'LiteOS\x00\x00\x00\x00\x00\x00\x00'
                                   b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
                                   b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
                                   b'\x00\x00\x00\x00\x00\x00\x00\x00\x00',
                    'PatchVersion': b'HUAWEIASP\x00\x00\x00\x00\x00\x00\x00'
                                    b'\x00\x00\x00\x00\x00\x00\x00\x00\x00'
                                    b'\x00\x00\x00\x00\x00\x00\x00',
                    'CreateTime': b'\x00\x00\x00\x00\x00\x00\x00\x00',
                    'NetID': 1, 'ProgID': 58, 'CodeAddr': 268959744,
                    'CodeLength': 64, 'ShareDataAddr': 268967936,
                    'ShareDataLength': 0, 'DataAddr': 268972032,
                    'DataLength': 0,
                    'Funcs': 2, 'Flag': b'CP', 'CrcLen': 0, 'SegInfSize': 0
        }
        hp_index["HPVersion"], = unpack('<4s', hp_index_byte[0:4])
        hp_index["HPFlag"], = unpack('<2s', hp_index_byte[4:6])
        hp_index["HPFileName"], = unpack('<6s', hp_index_byte[6:12])
        hp_index["HPCrc"], = unpack('<H', hp_index_byte[12:14])
        hp_index["ndependFlag"], = unpack('<H', hp_index_byte[14:16])
        hp_index["InterPatchNo"], = unpack('<I', hp_index_byte[16:20])
        hp_index["ProgVersion"], = unpack('<48s', hp_index_byte[20:68])
        hp_index["PatchVersion"], = unpack('<32s', hp_index_byte[68:100])
        hp_index["CreateTime"], = unpack('<8s', hp_index_byte[100:108])
        hp_index["NetID"], = unpack('<I', hp_index_byte[108:112])
        hp_index["ProgID"], = unpack('<I', hp_index_byte[112:116])
        hp_index["CodeAddr"], = unpack('<Q', hp_index_byte[116:124])
        hp_index["CodeLength"], = unpack('<Q', hp_index_byte[124:132])
        hp_index["ShareDataAddr"], = unpack('<Q', hp_index_byte[132:140])
        hp_index["ShareDataLength"], = unpack('<Q', hp_index_byte[140:148])
        hp_index["DataAddr"], = unpack('<Q', hp_index_byte[148:156])
        hp_index["DataLength"], = unpack('<Q', hp_index_byte[156:164])
        hp_index["Funcs"], = unpack('<I', hp_index_byte[164:168])
        hp_index["Flag"], = unpack('<2s', hp_index_byte[168:170])
        hp_index["CrcLen"], = unpack('<H', hp_index_byte[170:172])
        hp_index["SegInfSize"], = unpack('<I', hp_index_byte[172:176])
        return hp_index

    @staticmethod
    def _get_hp_index(hp_index_byte: bytearray):
        """
        _get_hp_index
        :param hp_index_byte:
        :return:
        """
        hp_index = {}
        hp_index["UnitType"], = unpack('<I', hp_index_byte[0:4])
        hp_index["UnitOffset"], = unpack('<I', hp_index_byte[4:8])
        hp_index["UnitSize"], = unpack('<I', hp_index_byte[8:12])
        hp_index["UnitFlag"], = unpack('<I', hp_index_byte[12:16])
        hp_index["InterPatchNo"], = unpack('<I', hp_index_byte[16:20])
        return hp_index

    def _parse_head(self):
        self.pat_file.seek(0)
        hdr = {}
        hdr['Ident'], = unpack('<4s', self.pat_file.read(4))

        # 补丁包校验和
        hdr['PatchCRC'], = unpack('<H', self.pat_file.read(2))

        # 补丁包类型
        hdr['PacketType'], = unpack('<H', self.pat_file.read(2))

        # 主机版本描述
        hdr['ProgVersion'], = unpack('<48s', self.pat_file.read(48))

        # 补丁包中补丁文件个数
        hdr['PatchUnitNum'], = unpack('<I', self.pat_file.read(4))

        # 补丁单元索引表偏移
        hdr['IndexOffset'], = unpack('<I', self.pat_file.read(4))

        # 补丁单元索引表记录数，独立式补丁EntryNum=1
        hdr['EntryNum'], = unpack('<I', self.pat_file.read(4))

        # 保留位
        hdr['Reserved'], = unpack('<I', self.pat_file.read(4))

        # 保留主机版本的编译时间
        hdr['CompileTime'], = unpack('<8s', self.pat_file.read(8))

        return hdr

    def _get_hp_file_header_info(self):
        """
        _get_hp_file_header_info
        :return:
        """
        hp_indexs = []
        for hp_index in self.pat_hpuint_index:
            self.pat_file.seek(hp_index['UnitOffset'])
            hp_index_bytes = bytearray(self.pat_file.read(PAT_HPHEAD_LEN))
            hp_index_byte_list = []
            for hp_i in range(0, len(hp_index_bytes), PAT_HPHEAD_LEN):
                hp_index_byte_list.append(hp_index_bytes[hp_i:hp_i + PAT_HPHEAD_LEN])
            for hp_index_byte in hp_index_byte_list:
                hp_indexs.append(PatInfo._get_hpfile(hp_index_byte))
        return hp_indexs

    def _parse_hp_index(self):
        """
        _parse_hp_index
        :return:
        """
        hp_indexs = []
        self.pat_file.seek(self.pat_header.get('IndexOffset'))
        hp_index_bytes = bytearray(self.pat_file.read())
        hp_index_bytes_list = []
        for hp_i in range(0, len(hp_index_bytes), PAT_UNIT_LEN):
            hp_index_bytes_list.append(hp_index_bytes[hp_i:hp_i + PAT_UNIT_LEN])
        for hp_index_byte in hp_index_bytes_list:
            hp_indexs.append(PatInfo._get_hp_index(hp_index_byte))
        return hp_indexs


class NewPat:
    """
    NewPat
    """
    pat_header = dict()
    new_patch_hphead = dict()
    new_patch_unit = dict()
    patch_func_name_list = dict()
    patch_old_func_addr_list = dict()
    patch_new_func_addr_list = dict()


class PatchArea:
    """
    PatchArea
    """
    patch_code_area_start = 0
    patch_code_area_length = 0
    patch_share_data_area_start = 0
    patch_share_data_area_length = 0
    patch_data_area_start = 0
    patch_data_area_length = 0
    patch_code_available_len = 0
    patch_share_data_available_len = 0


def write_head_to_file(hdr: dict):
    """
    write_head_to_file
    :param hdr:
    :return:
    """
    write_buffer = bytearray()
    write_buffer.extend(pack('<4s', hdr['Ident']))
    write_buffer.extend(pack("<H", hdr['PatchCRC']))
    write_buffer.extend(pack('<H', hdr['PacketType']))
    write_buffer.extend(pack('<48s', hdr['ProgVersion']))
    write_buffer.extend(pack('<I', hdr['PatchUnitNum']))
    write_buffer.extend(pack('<I', hdr['IndexOffset']))
    write_buffer.extend(pack('<I', hdr['EntryNum']))
    write_buffer.extend(pack('<I', hdr['Reserved']))
    write_buffer.extend(pack('<8s', hdr['CompileTime']))
    return write_buffer


def write_hphead_to_file(hp_index: dict):
    """
    write_hphead_to_file
    :param hp_index:
    :return:
    """
    write_buffer = bytearray()
    write_buffer.extend(pack('<4s', hp_index["HPVersion"]))
    write_buffer.extend(pack('<2s', hp_index["HPFlag"]))
    write_buffer.extend(pack('<6s', hp_index["HPFileName"]))
    write_buffer.extend(pack('<H', hp_index["HPCrc"]))
    write_buffer.extend(pack('<H', hp_index["ndependFlag"]))
    write_buffer.extend(pack('<I', hp_index["InterPatchNo"]))
    write_buffer.extend(pack('<48s', hp_index["ProgVersion"]))
    write_buffer.extend(pack('<32s', hp_index["PatchVersion"]))
    write_buffer.extend(pack('<H', hp_index["year"]))
    write_buffer.extend(pack('<B', hp_index["month"]))
    write_buffer.extend(pack('<B', hp_index["day"]))
    write_buffer.extend(pack('<B', hp_index["hour"]))
    write_buffer.extend(pack('<B', hp_index["minute"]))
    write_buffer.extend(pack('<B', hp_index["second"]))
    write_buffer.extend(pack('<B', hp_index["reserved"]))
    write_buffer.extend(pack('<I', hp_index["NetID"]))
    write_buffer.extend(pack('<I', hp_index["ProgID"]))
    write_buffer.extend(pack('<Q', hp_index["CodeAddr"]))
    write_buffer.extend(pack('<Q', hp_index["CodeLength"]))
    write_buffer.extend(pack('<Q', hp_index["ShareDataAddr"]))
    write_buffer.extend(pack('<Q', hp_index["ShareDataLength"]))
    write_buffer.extend(pack('<Q', hp_index["DataAddr"]))
    write_buffer.extend(pack('<Q', hp_index["DataLength"]))
    write_buffer.extend(pack('<I', hp_index["Funcs"]))
    write_buffer.extend(pack('<2s', hp_index["Flag"]))
    write_buffer.extend(pack('<H', hp_index["CrcLen"]))
    write_buffer.extend(pack('<I', hp_index["SegInfSize"]))
    return write_buffer


def turn_unit_to_hp(hp_index: dict):
    """
    turn_unit_to_hp
    :param hp_index:
    :return:
    """
    write_buffer = bytearray()
    write_buffer.extend(pack('<I', hp_index["UnitType"]))
    write_buffer.extend(pack('<I', hp_index["UnitOffset"]))
    write_buffer.extend(pack('<I', hp_index["UnitSize"]))
    write_buffer.extend(pack('<I', hp_index["UnitFlag"]))
    write_buffer.extend(pack('<I', hp_index["InterPatchNo"]))
    return write_buffer
