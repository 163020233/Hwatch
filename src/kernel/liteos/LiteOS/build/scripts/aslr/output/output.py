#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
'''
Output result to file
'''
import os
import stat
from abc import ABC, abstractmethod
import logging


LOG_FORMAT = '[%(funcName)s:%(lineno)d, %(levelname)s]: %(message)s'
logging.basicConfig(level=logging.NOTSET, format=LOG_FORMAT)


class IOutputItem(ABC):
    @abstractmethod
    def generate(self) -> str:
        '''
        generate aslr table item source code
        '''
        raise NotImplementedError()


class OutputManager():
    '''
    Class for relocation information output
    '''

    HEADER_MAGIC = '0x5a5a5c5c'
    FILE_BEFORE = [
        "#include \"los_typedef.h\"\n",
        "typedef struct {\n",
        "    UINTPTR offset;\n",
        "    UINTPTR info;\n",
        "    INTPTR addend;\n",
        "    UINTPTR addr;\n",
        "} AslrRelInfo;\n",
        "typedef struct {\n",
        "    UINTPTR shType;\n",
        "    UINTPTR table;\n",
        "    UINTPTR num;\n",
        "    UINTPTR magic;\n",
        "} AslrTableInfo;\n",
        "#define ATTR_SECTION1 __attribute__((section(\".laslr_rel.1\")))\n",
        "#define ATTR_SECTION2 __attribute__((section(\".laslr_rel.2\")))\n",
        "#define ATTR_USED __attribute__((used))\n\n",
    ]
    REL_INFO_BEFORE = [
        "const AslrRelInfo g_aslrRelInfo[] ATTR_SECTION2 ATTR_USED = {\n",
    ]
    REL_INFO_AFTER = [
        "};\n",
    ]
    TABLE_INFO_BEFORE = [
        "const AslrTableInfo g_aslrTableInfo ATTR_SECTION1 ATTR_USED = {\n",
    ]
    TABLE_INFO_AFTER = [
        "};\n",
    ]
    FILE_AFTER = {
        "\n/* end of file */\n",
    }

    def __init__(self):
        self._contents = []
        self._relinfo_cnt = 0
        self._relinfo_start = 0

    def add_head(self):
        '''
        add head part
        '''
        self._contents += self.FILE_BEFORE

    def add_relinfo(self, relinfo: IOutputItem):
        '''
        add relinfo table item
        '''
        if self._relinfo_start == 0:
            self._contents += self.REL_INFO_BEFORE
            self._relinfo_start = len(self._contents)
            self._contents += self.REL_INFO_AFTER

        self._contents.insert(self._relinfo_start + self._relinfo_cnt, relinfo.generate())
        self._relinfo_cnt += 1

    def add_image_info(self, reloc_type: int):
        '''
        add image info part
        '''
        self._contents += self.TABLE_INFO_BEFORE
        self._contents.append("    .shType = {0},\n".format(reloc_type))
        self._contents.append("    .table = (UINTPTR)g_aslrRelInfo,\n")
        self._contents.append("    .num = {0},\n".format(self._relinfo_cnt))
        self._contents.append("    .magic = {0},\n".format(self.HEADER_MAGIC))
        self._contents += self.TABLE_INFO_AFTER

    def add_end(self):
        '''
        add end part
        '''
        self._contents += self.FILE_AFTER

    def output_to_file(self, out_file: str):
        '''
        output data to file for GSPR
        '''
        out_dir = os.path.dirname(out_file)
        if not os.path.exists(out_dir):
            os.makedirs(out_dir)
        # open with write only | open with creat | open with trunc
        # Write by owner | Read by owner | Read by group | Read by others
        with os.fdopen(os.open(out_file, os.O_WRONLY | os.O_CREAT | os.O_TRUNC, \
                stat.S_IWUSR | stat.S_IRUSR | stat.S_IRGRP | stat.S_IROTH), 'w') as file_handle:
            file_handle.writelines(self._contents)