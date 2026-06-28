#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.

'''
The lite-object file generator. This generator is used to generate a loadable module that LiteOS can use.
'''

import shutil
import logging
from argparse import ArgumentParser
import builder


def app():
    '''App Entry'''
    parser = ArgumentParser(description='LiteOS lite loadable module tools')
    parser.add_argument(dest='source', metavar='source', help='Loadable ELF file')
    parser.add_argument('-o', '--output', dest='output', metavar='output', help='Output filename', required=True)
    parser.add_argument('--objcopy', dest='objcopy', metavar='objcopy', help='objcopy executable')
    parser.add_argument('-v', '--verbose', dest='verbose', help='Verbose output', action='store_true')
    args = parser.parse_args()

    level = logging.DEBUG if args.verbose else logging.INFO
    logging.basicConfig(level=level)

    shutil.copyfile(args.source, args.output)
    if args.objcopy:
        builder.OBJCOPY = args.objcopy
    builder.build(args.output)
    logging.info('success output to %s', args.output)


if __name__ == '__main__':
    app()
