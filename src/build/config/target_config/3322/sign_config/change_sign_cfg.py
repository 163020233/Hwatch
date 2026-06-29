#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.

import argparse
import os
import sys

def get_parameters():
    parser = argparse.ArgumentParser()

    parser.add_argument('-base', type=str, default='D:\\Code\\PythonCode\\projects\\change_sign_cfg\\acpu_ecc.cfg',
                        help='基础配置文件路径')

    parser.add_argument('-temp_dir', type=str, default='D:\\Code\\PythonCode\\projects\\change_sign_cfg\\temp.cfg',
                        help='临时配置文件路径')

    parser.add_argument('-input', type=str, default='D:\\Code\\PythonCode\\projects\\change_sign_cfg',
                        help='源文件路径')

    parser.add_argument('-output', type=str, default='D:\\Code\\PythonCode\\projects\\change_sign_cfg',
                        help='输出文件路径')

    parser.add_argument('-type', type=str, default='0',
                        help='签名选项')

    parser.add_argument('-sign_tool', type=str, default='0',
                        help='签名工具选择')

    config = parser.parse_args()
    return config

class Properties:
    def getProperties(self, dir):
        properties = {}
        with open(dir, 'r') as pro_file:
            for line in pro_file:
                if line.find('=') > 0 and '#' not in line:
                    strs = line.replace('\n', '').split('=')
                    properties[strs[0]] = strs[1]
        return properties
    def setProperties(self, dir, properties):
        with open(dir, 'w') as pro_file:
            for item in properties:
                pro_file.write(item+'='+properties[item]+'\n')

if __name__ == "__main__":
    user_input = get_parameters()
    temp_path = os.path.split(os.path.realpath(user_input.temp_dir))[0]
    if not os.path.exists(temp_path):
        os.makedirs(temp_path)
    p = Properties()
    properties = p.getProperties(user_input.base)
    properties['SrcFile'] = user_input.input
    properties['DstFile'] = user_input.output
    sign_tool = user_input.sign_tool
    p.setProperties(user_input.temp_dir, properties)
    root_dir = os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', '..')
    if sign_tool == '0':
        if sys.platform == 'win32':
            cmd = os.path.join(root_dir, 'tools', 'bin', 'sign_tool', 'sign_tool_pltuni.exe') + ' ' + user_input.type + ' ' + user_input.temp_dir
        else:
            cmd = os.path.join(root_dir, 'tools', 'bin', 'sign_tool', 'sign_tool_pltuni') + ' ' + user_input.type + ' ' + user_input.temp_dir
    else:
        cmd = "%s %s"%(sys.executable, os.path.join(root_dir, 'tools', 'bin', 'sign_tool', 'melody_script', 'sign_tool_melody.py') + ' ' + user_input.type + ' ' + user_input.temp_dir)
    print(cmd)
    os.system(cmd)
