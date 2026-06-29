#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.

import os
import sys
import argparse
import fbb_sign_params
from fbb_sign_ecdsa   import FbbSignEcdsa
from fbb_ecc_root_key import FbbEccRootKey


def sign_tool_init():
    tmp_dir = fbb_sign_params.tmp_path
    if not os.path.exists(tmp_dir):
        os.mkdir(tmp_dir)


def handle_root_key(args):
    root_key = FbbEccRootKey(args.output)
    root_key.generate()


def handle_ecdsa_sign(args):
    sign = FbbSignEcdsa(args.type, args.input, args.output, args.agent)
    sign.sign()


def handle_param_areas(args):
    print('handle_param_areas: Not Support!')


def handle_fota_package(args):
    print('handle_fota_package: Not Support!')


def fbb_sign_handle(args):
    if args.type not in fbb_sign_params.sign_params:
        raise Exception(f'Not support type {args.type}')

    if args.type == 'root_key':
        handle_root_key(args)
    elif args.type == 'param_areas':
        handle_param_areas(args)
    elif args.type == 'fota_package':
        handle_fota_package(args)
    else:
        handle_ecdsa_sign(args)


def get_parameters(argv):
    type_ls = []
    for item in fbb_sign_params.sign_params:
        type_ls.append(f'\'{item}\'')

    type_help_ls = ', '.join(type_ls)
    type_help_str = f'Image type: {type_help_ls}.'

    parser = argparse.ArgumentParser()
    parser.add_argument('-t', '--type', type=str, required=True, help=type_help_str)
    parser.add_argument('-i', '--input',  type=str, help='Input file path.')
    parser.add_argument('-o', '--output', type=str, required=True, help='Output file path.')
    parser.add_argument('-a', '--agent', action='store_true', help='Sign use agent server flag.')
    return parser.parse_args(argv)


if __name__ == '__main__':
    try:
        sign_tool_init()
        args = get_parameters(sys.argv[1:])

        fbb_sign_handle(args)

        print('Success!')

    except Exception as e:
        print(f'[ERROR] {e}')
