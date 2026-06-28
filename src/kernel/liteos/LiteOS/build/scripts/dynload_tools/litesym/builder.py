#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.

'''
Object builder.
'''

import os
import subprocess
import shlex
import logging
from elftools.elf.elffile import ELFFile
from tools import ELFFileExtendedTool


OBJCOPY = 'objcopy'


def _add_section(elfpath: str, section: str, filename: str):
    '''Add a section to elf file.
    '''
    cmdline = [
        OBJCOPY,
        '--add-section', '{}={}'.format(section, filename),
        '--set-section-flags', '{}=noload,readonly'.format(section),
        elfpath
    ]
    _execute_cmdline(cmdline)

    cmdline = [
        OBJCOPY,
        '--set-section-alignment', '{}={}'.format(section, 4),  # align 4 bytes
        elfpath
    ]
    _execute_cmdline(cmdline)


def _add_empty_section(elfpath: str, section: str, length: int):
    '''Create a empty section to elf file
    '''
    cmdline = 'dd if=/dev/zero of={} bs={} count=1'.format('tmp.bin', length)
    _execute_cmdline(cmdline)
    _add_section(elfpath, section, 'tmp.bin')


def _execute_cmdline(cmdline):
    if not isinstance(cmdline, str):
        cmdline = ' '.join(cmdline)
    logging.debug('cmd:%s', cmdline)
    args = shlex.split(cmdline)
    subprocess.run(args, check=True)


def build(filename):
    '''Build lite-object and output to file'''
    with ELFFile.load_from_path(filename) as elffile:
        # check input file
        for sec_name in ['.liteos.sym.table']:
            if elffile.get_section_by_name(sec_name):
                raise Exception('liteos custom table already exist')
        # add empty .liteos.sym.table place holder
        elftool = ELFFileExtendedTool(elffile)
        lite_symtab = elftool.create_lite_symtab()
        lite_symtab_len = len(lite_symtab.to_bytes())
        logging.debug('lite symtab len is %u', lite_symtab_len)
        _add_empty_section(filename, '.liteos.sym.table', lite_symtab_len)

    # memery layout of the output elf is fixed, now calulate .liteos.sym.table content
    with ELFFile.load_from_path(filename) as elffile:
        symtab_offset = elffile.get_section_by_name('.liteos.sym.table')['sh_offset']
        elftool = ELFFileExtendedTool(elffile)
        lite_symtab = elftool.create_lite_symtab()
        content = lite_symtab.to_bytes()

    # final output
    with open(filename, 'r+b') as output:
        output.seek(symtab_offset, os.SEEK_SET)
        output.write(content)
