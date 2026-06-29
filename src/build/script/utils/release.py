#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
#Description: CI 构建版本打包工具入口

import os
import sys
import re
import argparse
import configparser
import shutil

def post_merge_clean(config, path, version):
    ini_image_path = config['build_post_copy.prepare']['image_path']
    image_path = os.path.join(path, ini_image_path)
    # 清空文件夹
    if os.path.exists(image_path):
        for ver in version.split(","):
            chip_version_path = os.path.join(image_path, ver)
            if os.path.exists(chip_version_path):
                shutil.rmtree(chip_version_path)
            os.mkdir(chip_version_path)
    else:
        print('image path not find %s' % image_path)
        sys.exit(1)

def post_merge_copy(config, path, project, version):
    for ver in version.split(","):
        label = 'build_post_copy' + '.' + ver
        for copy_dir in config[label]:
            if copy_dir.strip() == "":
                continue
            dest_dir = os.path.join(path, copy_dir)

            # 防呆处理，此处避免误处理其他的产品
            if not re.match(r'.*\/(%s)\/.*' % project, dest_dir):
                print('match project path')
                sys.exit(1)

            os.makedirs(dest_dir, exist_ok=True)
            for file in config[label][copy_dir].splitlines():
                if file.strip() == "":
                    continue
                dest_file = os.path.join(path, file)
                if os.path.isfile(dest_file):
                    shutil.copy(dest_file, dest_dir)
                else:
                    print('file %s not find' % dest_file)
                    sys.exit(1)

def wdk_merge_copy(config, path, project, version):
    for ver in version.split(","):
        label = 'merge_total_wdk' + '.' + ver
        if not config.has_section(label):
            print('label %s not find' % label)
            continue

        tool_path = os.path.join(path, config['build_post_copy.prepare']['wdk_tool'])
        output_dir = os.path.join(path, config[label]['output_dir'])
        output_temp_dir = os.path.join(output_dir, 'temp')
        if os.path.exists(output_dir):
            shutil.rmtree(output_dir)
        os.mkdir(output_dir)
        os.mkdir(output_temp_dir)

        wdk_index = 0
        for copy_file in config[label]['src_file'].splitlines():
            if copy_file.strip() == "":
                continue
            src_file = os.path.join(path, copy_file)
            if not os.path.isfile(src_file):
                print('file %s not find' % copy_file)
                continue
            print(src_file)
            dest_file = os.path.join(output_temp_dir, str(wdk_index) + '_' + 'Total.wdk')
            print(dest_file)
            shutil.copy(src_file, dest_file)
            wdk_index += 1

        os.system(tool_path + " merge " + output_temp_dir)

        shutil.copy(os.path.join(output_temp_dir, 'Total.wdk'), os.path.join(output_dir, 'Total.wdk'))
        shutil.rmtree(output_temp_dir)
        conv_tool_dir = os.path.join(path, 'main', 'build', 'mpxx', 'custom', 'script', 'wdk_convert', 'build_xml.sh')
        os.system(conv_tool_dir + " " + output_dir + " " + path + " " + project)
        if os.path.exists(os.path.join(output_dir, '..', 'database')):
            shutil.rmtree(os.path.join(output_dir, '..', 'database'))
        shutil.move(os.path.join(output_dir, 'database'), os.path.join(output_dir, '..', 'database'))

def compress_build(config, path, project, version):
    for ver in version.split(","):
        label = 'compress_build' + '.' + ver
        if not config.has_section(label):
            print('label %s not find' % label)
            continue
        dest_position = os.path.join(path, 'output', 'firmware_symbols_' + project)
        if os.path.exists(dest_position):
            shutil.rmtree(dest_position)
        for copy_dir in config[label]['compress_dir'].splitlines():
            if copy_dir.strip() == "":
                continue
            copy_dir = os.path.join(path, copy_dir)
            if os.path.exists(copy_dir):
                shutil.copytree(copy_dir, dest_position)
            else:
                print('dir %s not find' % copy_dir)
                sys.exit(1)
        current_dir = os.getcwd()
        os.chdir(os.path.join(path, 'output'))
        output_filename = 'firmware_symbols_' + project
        # 创建一个压缩文件对象
        shutil.make_archive(output_filename, 'gztar', dest_position)
        shutil.rmtree(dest_position)
        os.chdir(current_dir)

def main():
    parser = argparse.ArgumentParser(
                    prog='CI 生成自动化工具',
                    description='用于合入后构建打包',
                    epilog='---------------')
    parser.add_argument('--package_path')
    parser.add_argument('-prj', '--project', choices=['mp18', 'mp17c', 'mp12'], required=True)
    parser.add_argument('-proc', '--processer', choices=['codehub', 'couldgrgon', 'gerrit'], required=True)
    parser.add_argument('-ver', '--chip_version', help='one more combine of pilot, fpga, mpw, cs1, cs2', required=True)
    parser.add_argument('-a', '--action', help='one more combine of copy, build, show, merge_wdk', required=True)
    parser.add_argument('-s', '--stage',  choices=['pre_merge', 'post_merge'])
    parser.add_argument('-ini', '--ini_path',  help='release ini path', required=True)

    args = parser.parse_args()

    iniconfig = configparser.ConfigParser()

    path = args.package_path
    if path is None:
        path = os.getcwd()
    ini_path = args.ini_path

    if not os.path.isfile(ini_path):
        print('not find path %s' % ini_path)
        sys.exit(1)
    iniconfig.read(ini_path)

    action = args.action.split(",")
    print(action)

    if "copy" in action and args.stage == 'post_merge':
        post_merge_clean(iniconfig, path, args.chip_version)
        post_merge_copy(iniconfig, path, args.project, args.chip_version)
        print("project [%s] image firmware copy success"% args.project)

    if "merge_wdk" in action and args.stage == 'post_merge':
        wdk_merge_copy(iniconfig, path, args.project, args.chip_version)
        print("project [%s] total wdk merge success"% args.project)

    if "compress_build" in action and args.stage == 'post_merge':
        compress_build(iniconfig, path, args.project, args.chip_version)
        print("project [%s] total compress_build success"% args.project)

    sys.exit(0)
if __name__ == "__main__":
    exit(main())