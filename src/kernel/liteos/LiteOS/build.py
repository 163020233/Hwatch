#!/usr/bin/env python3
# coding=utf-8
# Copyright © Huawei Technologies Co., Ltd. 2010-2020. All rights reserved.

'''build.py'''
import os
import shutil
import sys
import time
from sys import argv
from os import path, getenv
import json
import subprocess

LIBCK_HOME = path.dirname(path.abspath(__file__))

sys.path.append('{}/build/libck_lib'.format(LIBCK_HOME))

import globle
from libck_build import Compile
from build_cmake import BuildCmake

SPEC_DEMO = """
#
# spec file for package libck
#

Summary:libck {0} packages
Name:libck-{0}
Version:1
Release:1.0
License:Commercial
Group:Development/Tools
Autoreqprov:0

%description
libck {0}

%files
%defattr(-,root,root,-)
/cmake
/libck

"""


def package_output(home_path, product):
    """
    修改文件最后一次访问时间并进行压缩打包
    """
    out_put_path = os.path.join(home_path, "output")
    if not os.path.exists(os.path.join(home_path, "build/libck_cmake/common/pack_conf.json")):
        return False
    with open(os.path.join(home_path, "build/libck_cmake/common/pack_conf.json"), encoding='utf-8') as conf_json:
        pack_conf = json.load(conf_json)
    model_time = int(time.mktime(time.strptime(pack_conf['pack_time'], "%Y%m%d%H%M%S")))
    change_file_time(out_put_path, model_time)
    zip_name = f"libck-{pack_conf['version']}-{pack_conf['pack_time']}.zip"
    spec_file = os.path.join(LIBCK_HOME, "build", "logs", "libck.spec")
    if product == "balong":
        zip_cmd = "cd {} && zip -o -X {} $(find . | LC_ALL=C sort | tr '\n' ' ')".format(out_put_path, zip_name)
    elif product in globle.RPM_PORDUCT:
        with os.fdopen(os.open(spec_file, os.O_RDWR | os.O_CREAT, 0o644), "w") as file_handle:
            file_handle.write(SPEC_DEMO.format(product))
        zip_name = f"libck-{product}-{pack_conf['version']}-{pack_conf['pack_time']}.noarch.rpm"
        zip_cmd = f"source {libck_build.bep_env_path}/bep_env.sh -s \
            {LIBCK_HOME}/build/libck_cmake/common/bep_time/bep_env.conf && \
            rpmbuild -vv -bb --nodeps --noclean --target noarch-euler-linux --buildroot={out_put_path} {spec_file} \
            --define '_rpmdir {out_put_path}' --define '_build_name_fmt {zip_name}'"
    else:
        zip_cmd = "cd {} && zip -o -X {} $(find . | sort | tr '\n' ' ')".format(out_put_path, zip_name)
    subprocess.getstatusoutput(zip_cmd)
    os.utime(os.path.join(out_put_path, zip_name), (model_time, model_time))
    out_dirs = ["cmake", "libck", "RTOS_Lite"]
    for out_dir in out_dirs:
        if path.exists(os.path.join(out_put_path, out_dir)):
            shutil.rmtree(os.path.join(out_put_path, out_dir))
    if path.exists(spec_file):
        os.remove(spec_file)
    return True


def change_file_time(out_put_path, model_time):
    """
    修改生成文件的创建和访问时间等，保证zip包bep一致性
    """
    for parent, dirnames, filenames in os.walk(out_put_path):
        for filename in filenames:
            pathfile = os.path.join(parent, filename)
            os.utime(pathfile, (model_time, model_time))
        for dirname in dirnames:
            pathdir = os.path.join(parent, dirname)
            os.utime(pathdir, (model_time, model_time))


if __name__ == "__main__":
    # argv[1]: cpu_type 表示要编译的平台 类似sd6157， default：
    # argv[2]: compile_option 控制编译选项，调用不同的cmake参数，目前只有normal
    #          hllt single(是否编译安全c，组件化独立构建需求)
    # argv[3]: lib_run_type lib库要跑的平台 faga sim等
    # argv[4]: cpu_plat 类似于'sd6157': 'dsp220', 'risc220'中的risc220,或者all
    # argv[5]: make_phase 全量构建选项
    # argv[6]: libck_packing_path lib库的安装位置
    DEFAULT_PARA = ("all", "normal", "FPGA", "ALL", "ALL", "")
    PARA = []
    if len(argv) == 1:
        for _, para_data in enumerate(DEFAULT_PARA):
            PARA.append(para_data)
    else:
        for para_i, para_data in enumerate(DEFAULT_PARA):
            if para_i < len(argv) - 1:
                PARA.append(argv[para_i + 1])
            else:
                PARA.append(para_data)
    CPU_TYPE = PARA[0].lower()
    COMPILE_OPTION = PARA[1].lower()
    LIB_RUN_TYPE = PARA[2].upper()
    CPU_PLAT = PARA[3].lower()
    MAKE_PHASE = PARA[4].upper()
    LIBCK_PACKING_PATH = PARA[5]

    CPUS = {}
    for cpu in globle.PRODUCT.get(CPU_TYPE):
        CPUS[cpu] = globle.CPU_PLAT.get(cpu) if CPU_PLAT == 'all' else [CPU_PLAT]
        run_type_list = globle.CPU_RUN_TYPE.get(cpu, [LIB_RUN_TYPE]) if len(argv) < 4 else [LIB_RUN_TYPE]
        cpu_name = globle.CPU_ALIAS.get(cpu, cpu)
        for plat in CPUS.get(cpu):
            for run_type in run_type_list:
                bc_instance = BuildCmake(cpu_name, COMPILE_OPTION, run_type,
                                     plat, MAKE_PHASE, LIBCK_PACKING_PATH)
                if not bc_instance.generate_cmake_file():
                    sys.exit(1)
                libck_build = Compile(cpu_name, COMPILE_OPTION, run_type,
                                    plat, MAKE_PHASE, LIBCK_PACKING_PATH)
                if cpu == "clean":
                    libck_build.libck_clean()
                    continue

                if not libck_build.prepare_env():
                    sys.exit(1)

                if not libck_build.libck_compile():
                    sys.exit(1)

    CMD = "bgf -h"
    output, _ = subprocess.getstatusoutput(CMD)
    output_path = libck_build.libck_packing_path
    swbom_file = path.join(LIBCK_HOME, "build", "libck_lib", "create_swbom.py")
    if output == 0 and getenv('SWBOM_LIBCK', "") == "1" and path.exists(swbom_file):
        ret, msg = subprocess.getstatusoutput(f"python {swbom_file} {output_path}")
    if CPU_TYPE in globle.ZIP_PRODUCT:
        if not package_output(globle.HOME_PATH, CPU_TYPE):
            sys.exit(1)
    sys.exit(0)
