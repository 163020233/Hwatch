#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2023 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import tarfile
import argparse
import os
import subprocess
import sys
import shutil
from pathlib import Path


def untar_file(tar_file_path, extract_path):
    try:
        os.makedirs(extract_path, exist_ok=True)
        with tarfile.open(tar_file_path, 'r:gz') as tar:
            members = tar.getmembers()
            for member in members:
                # 去除第一层路径
                if member.name.count('/') > 0:
                    member.name = member.name.split('/', 1)[1]
                else:
                    continue  # 跳过根目录条目
                # 提取文件
                tar.extract(member, extract_path)
    except Exception as e:
        print(f"tar error! {e}")
        return


def move_file(src_path, dst_path):
    files = [
        "CVE-2019-6129.patch",
        "huawei_libpng_CMakeList.patch",
        "libpng-fix-arm-neon.patch",
        "libpng-multilib.patch",
        "backport-libpng-1.6.37-enable-valid.patch",
        "pnglibconf.h",
        "CVE-2018-14048.patch",
        "libpng_optimize.patch"
    ]
    for file in files:
        src_file = os.path.join(src_path, file)
        dst_file = os.path.join(dst_path, file)
        shutil.copy(src_file, dst_file)


def apply_patch(patch_file, target_dir, patch_exe_dir):
    try:
        if not os.path.exists(target_dir):
            return
        patch_cmd = [patch_exe_dir, '-p1', "--fuzz=0", "--no-backup-if-mismatch", '-i', patch_file, '-d', target_dir]
        subprocess.run(patch_cmd, check=True)
    except Exception as e:
        print(f"apply_patch error! {e}")
        print(' '.join(patch_cmd))
        return


def do_patch(target_dir, patch_exe_dir):
    patch_file = [
        "CVE-2019-6129.patch",
        "huawei_libpng_CMakeList.patch",
        "libpng-fix-arm-neon.patch",
        "libpng-multilib.patch",
        "backport-libpng-1.6.37-enable-valid.patch",
        "CVE-2018-14048.patch",
        "libpng_optimize.patch"
    ]

    for patch in patch_file:
        apply_patch(patch, target_dir, patch_exe_dir)


def main():
    path = argparse.ArgumentParser()
    path.add_argument('--gen-dir', help='generate path of log', required=True)
    path.add_argument('--source-dir', help='generate path of log', required=True)
    path.add_argument('--patch-dir', help='generate path of log', required=True)
    args = path.parse_args()
    tar_file_path = os.path.join(args.source_dir, "libpng-1.6.38.tar.gz")
    target_dir = os.path.join(args.gen_dir, "libpng-1.6.38")
    target_dir = target_dir.replace("\\", "/")
    
    patch_exe_dir = str(Path(args.patch_dir))

    untar_file(tar_file_path, target_dir)
    move_file(args.source_dir, target_dir)
    do_patch(target_dir, patch_exe_dir)
    return 0

if __name__ == '__main__':
    sys.exit(main())
