#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.

import os
import subprocess
import socket
import hashlib
import argparse
import re
from fbb_sign_params import tmp_path

public_key_hi1a21_dict = {
    'wearalbe_secboot_ecc_brainpool_rootkey' :
    '55458381878946D910534B5D6753DB2DC67FB9708D587FCDA726C9CBE33472B0'\
    '48E5532E1459A663D4980D384D6797DAEB0B9E9085F8DE008706A6764450909F',
    'wearalbe_secboot_ecc_brainpool_subkey' :
    '522D6C472B38E92AEE8F4A0241775CFC4A2F1EF2E16768B7AA51EB10D87926A6'\
    '79CB20E9F517A9A95BB24D98FE6A5E58CBDAB296FD360A1E0E9AD03914895FA2',
    'wearalbe_secboot_ecc_brainpool_devlprkey' :
    '5FBCED957579A34B036F1BF04212435E5E98984441FC06A9B03395218356AF62'\
    '8ADAD563B2AF1F4C618F2567034A0C33F7864CBF35D762681D7A201B85CEEBC9',
}
public_key_dict = public_key_hi1a21_dict

server_name = '10.30.61.85'
server_port = 8193
seg_size    = 8192


def sigdecode_der(sig_der):

    if sig_der[0] != 0x30:
        raise Exception('wanted type \'sequence\' (0x30)')

    len_rs = sig_der[1]
    if (len_rs > 0x46):
        raise Exception('sig_der len too long')

    if sig_der[2] != 0x02:
        raise Exception('sig_der format error')

    len_r = sig_der[3]
    if len_r + 2 >= len_rs:
        raise Exception('sig_der format error')

    if (sig_der[len_r + 4]) != 0x02:
        raise Exception('sig_der format error')

    len_s = sig_der[len_r + 5]
    if len_rs != len_r + len_s + 4:
        raise Exception('sig_der format error')

    r = int(bytes.hex(sig_der[4:len_r + 4]), 16)
    s = int(bytes.hex(sig_der[len_r + 6:]), 16)

    return r, s


def sign_service_local(key_id, data_bin):
    '''签名服务'''
    data_file = os.path.join(tmp_path, 'tmp_data.bin')
    sig_file  = os.path.join(tmp_path, 'tmp_sig.bin')

    with open(data_file, 'wb') as f:
        f.write(data_bin)

    jar_path = os.path.abspath(__file__)
    # 获取脚本所在目录
    jar_dir = os.path.join(os.path.dirname(jar_path), 'Rsagen.jar')
    sig_cmd = '-signAlg SHA256WITHECDSA https://pki-sign.cbg.huawei.com/sign-service/v1/signature'
    env_usr = '$domain_user'
    env_pwd = '$domain_pwd'

    cmd = f'java -jar {jar_dir} {sig_cmd} {env_usr} {env_pwd} {key_id} {data_file} {sig_file}'
    print(cmd)

    result = subprocess.getoutput(cmd)
    print(result)
    if 'ERROR' in result:
        raise Exception('Sign error.')

    with open(sig_file, 'rb') as f:
        sig_data = f.read()

    r, s = sigdecode_der(sig_data)
    sig_bin = bytes.fromhex(f'{r:064X}{s:064X}')
    return sig_bin


def parse_sign_result(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('--error', type=str, default='')
    parser.add_argument('--sig', type=str, default='')
    return parser.parse_args(argv)


def is_hex_string(s):
    return bool(re.match(r'^[0-9a-fA-F]+$', s))


def socket_send_file(tcp_socket, data_bin):
    file_size = len(data_bin)
    send_size = 0

    while send_size < file_size:
        blk_size = min(file_size - send_size, seg_size)
        tcp_socket.send(data_bin[send_size:(send_size + blk_size)])
        send_size += blk_size


def sign_service_agent(key_id, data_bin):
    # 创建socket
    tcp_client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_client_socket.connect((server_name, server_port))

    try:
        file_size = len(data_bin)
        cmd_str = f'sign_request --key_id {key_id} --file_size {file_size}'
        tcp_client_socket.send(cmd_str.encode('utf-8'))
        req_res = tcp_client_socket.recv(seg_size).decode('utf-8')
        if req_res != 'ACK':
            raise Exception('Sign request error!')

        socket_send_file(tcp_client_socket, data_bin)

        res_str = tcp_client_socket.recv(seg_size).decode('utf-8')

        res_argv = res_str.split()
        if res_argv[0] != 'sign_response':
            raise Exception('Return invalid format')

        res_args = parse_sign_result(res_argv[1:])
        if len(res_args.error) > 0:
            raise Exception(f'Sign error: {res_args.error}')

        if not is_hex_string(res_args.sig):
            raise Exception(f'Sign result error')

        if len(res_args.sig) != 128:
            raise Exception(f'Sign result length error')

        return bytes.fromhex(res_args.sig)

    except Exception as e:
        raise Exception('Sign error!')
    finally:
        tcp_client_socket.close()


def sign_service(key_id, data_bin, agent):
    if agent:
        return sign_service_agent(key_id, data_bin)
    else:
        return sign_service_local(key_id, data_bin)


def get_public_key(key_id):
    '''获取公钥数据'''
    if key_id not in public_key_dict:
        raise Exception('get_public_key: invalid key_id.')

    return bytes.fromhex(public_key_dict[key_id])