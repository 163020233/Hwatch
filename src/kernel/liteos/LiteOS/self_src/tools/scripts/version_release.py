#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
# Description: build the source to get lib

import os
import sys
import subprocess
import multiprocessing
import json
import tarfile

class Platform():
    def __init__(self, name, dir, cpu_type, config, lib_array):
        self.name = name
        self.rootpath = dir
        self.mkpath = dir
        self.cur_cpu = cpu_type
        self.config_file = config
        self.lib_array = lib_array
        self.miss = []

    def back_dir(self):
        '''
        For multi process only,
        change working directory to RTOS_Lite,
        and del files in RTOS_Lite_platform
        '''
        os.chdir(self.rootpath)
        subprocess.call("rm -rf {}".format(self.mkpath), shell = True)

    def mul_dir(self):
        '''
        For multi process only,
        change working directory to RTOS_Lite_platform,
        and copy project files to RTOS_Lite_platform
        '''
        dir = 'RTOS_Lite_{}'.format(self.name.replace('/', '_'))
        path = os.path.join(self.rootpath[:self.rootpath.find('RTOS_Lite')], dir)

        p = subprocess.call("cp {} {} -rf".format(self.rootpath, path), shell = True)

        self.mkpath = path
        os.chdir(self.mkpath)

    def move_lib(self):
        '''
        move libxxx.a from out/ to source dir
        '''
        for k, v in self.lib_array.items():
            v = os.path.join('self_src', v)
            if k == "libusb_device.a":
                continue

            if "self_src/arch_lite/arm" in v:
                if self.cur_cpu in v:
                    dir = os.path.join(self.rootpath, v)
                else:
                    continue
            else:
                dir = os.path.join(self.rootpath, v, self.cur_cpu)

            # check lib generated or not
            src = os.path.join(self.mkpath, 'out', self.name, 'lib', k)
            if not os.path.exists(src):
                self.miss.append(k)
                continue

            # create the lib dir
            if not os.path.exists(dir):
                os.makedirs(dir)

            dst = os.path.join(self.rootpath, dir, k)
            cmd_cp = "cp {} {} -f".format(src, dst)
            subprocess.call(cmd_cp, shell = True)

        subprocess.call("make clean >> /dev/null 2>&1", shell = True)

    def __make(self):
        '''
        make lib -j
        '''
        subprocess.call("make lib -j >> /dev/null", shell = True)
        dir = os.path.join(self.mkpath, 'out', self.name, 'lib')
        if os.path.exists(dir):
            print("{}: lib is existed".format(self.name))
            return 0
        else:
            print("{}: make error".format(self.name))
            return -1

    def make_usb_uvc_mode(self):
        k = 'libusb_device.a'

        config = os.path.join(self.mkpath, '.config')
        with open(config) as f:
            if 'LOSCFG_DRIVERS_USB_UVC_GADGET_BULK=y' in f.read():
                cat = 'uvc_bulk'
            else:
                cat = 'uvc_isoc'

        dir = os.path.join(self.rootpath, 'self_src/drivers/usb', self.cur_cpu, cat)

        # check lib generated or not
        src = os.path.join(self.mkpath, 'out', self.name, 'lib', k)
        if not os.path.exists(src):
            self.miss.append(k)
            return

        # create the lib dir
        if not os.path.exists(dir):
            os.makedirs(dir)

        dst = os.path.join(self.rootpath, dir, k)
        cmd_cp = "cp {} {} -f".format(src, dst)
        subprocess.call(cmd_cp, shell = True)

        subprocess.call("make clean >> /dev/null 2>&1", shell = True)

        if cat == "uvc_bulk":
            search_str = 'LOSCFG_DRIVERS_USB_UVC_GADGET_BULK'
            replace_str = 'LOSCFG_DRIVERS_USB_UVC_GADGET_ISOC'
        else:
            search_str = 'LOSCFG_DRIVERS_USB_UVC_GADGET_ISOC'
            replace_str = 'LOSCFG_DRIVERS_USB_UVC_GADGET_BULK'

        cmd = "sed -i 's/{}/{}/' {}".format(search_str, replace_str, config)
        subprocess.call(cmd, shell = True)

        print("{}: make savemenuconfig ...".format(self.name))
        subprocess.call("make savemenuconfig >> /dev/null 2>&1", shell = True)
        print("{}: make clean ...".format(self.name))
        subprocess.call("make clean >> /dev/null 2>&1", shell = True)

        print("{}: make ...".format(self.name))
        ret = self.__make()
        if ret == -1:
            print("{}: make error".format(self.name))
            exit(-1)

        if cat == "uvc_bulk":
            cat = 'uvc_isoc'
        else:
            cat = 'uvc_bulk'
        dir = os.path.join(self.rootpath, 'self_src/drivers/usb', self.cur_cpu, cat)

        # check lib generated or not
        src = os.path.join(self.mkpath, 'out', self.name, 'lib', k)
        if not os.path.exists(src):
            self.miss.append(k)
            return

        # create the lib dir
        if not os.path.exists(dir):
            os.makedirs(dir)

        dst = os.path.join(self.rootpath, dir, k)
        cmd_cp = "cp {} {} -f".format(src, dst)
        subprocess.call(cmd_cp, shell = True)

    def make_platform(self):
        '''
        make lib for the platform
        1. copy config
        2. make savemenuconfig
        3. make clean
        4. make yaffs2tool
        5. make lib
        '''
        dst = os.path.join(self.mkpath, '.config')
        src = os.path.join(self.mkpath, 'self_src', self.config_file)
        cmd_cp = "cp {} {} -f".format(src, dst)
        # print(cmd_cp)
        subprocess.call(cmd_cp, shell = True)
        print("{}: make savemenuconfig ...".format(self.name))
        subprocess.call("make savemenuconfig >> /dev/null 2>&1", shell = True)
        print("{}: make clean ...".format(self.name))
        subprocess.call("make clean >> /dev/null 2>&1", shell = True)
        print("{}: make yaffs2tool ...".format(self.name))
        subprocess.call("make yaffs2image >> /dev/null 2>&1", shell = True)

        print("{}: make ...".format(self.name))
        ret = self.__make()
        if ret == -1:
            print("{}: make error".format(self.name))
            exit(-1)

        # compile two USB libs
        self.make_usb_uvc_mode()

        file1 = os.path.join(self.mkpath, 'self_src', 'targets', 'board', self.name, 'include', 'menuconfig.h')
        file2 = os.path.join(self.mkpath, 'self_src', 'targets', 'board', self.name, 'board.ld')
        os.chdir(os.path.join(self.mkpath, 'self_src', 'targets'))

        # for bvt
        subprocess.call("git checkout {} >> /dev/null 2>&1".format(file1), shell = True)

        # for liteos
        subprocess.call("git clean -f {} >> /dev/null".format(file1), shell = True)
        subprocess.call("git clean -f {} >> /dev/null".format(file2), shell = True)
        os.chdir(os.path.join(self.mkpath))

def line_delete(filename, old_src, new_src):
    with open(filename,'r') as f:
        lines = []
        for line in f.readlines():
            if line != '\n':
                lines.append(line)
    f.close()
    with open(filename,'w') as f:
        for line in lines:
            if old_src in line:
                line = new_src
                f.write('%s\n' % line)
            else:
                f.write('%s' % line)

def src_del(top_dir, lib_array, extra_del_paths, lib_build):
    '''
    Del the source code
    '''
    if lib_build:
        for k, v in lib_array.items():
            v = os.path.join('self_src', v)
            if not os.path.exists(v):
                continue

            if k == "libusb_device.a":
                path = os.path.join('self_src', 'drivers', 'usb', 'gadget')
                cmd = 'find {} -name "*.[csS]" ! -name "*open*.[csS]" | xargs rm -rf'.format(path)
                subprocess.call(cmd, shell = True)
                path = os.path.join('self_src', 'drivers', 'usb', 'controller', 'usb_device')
                cmd = 'find {} -name "*.[csS]" | xargs rm -rf'.format(path)
                subprocess.call(cmd, shell = True)
            else:
                if 'self_src/arm/cortex_a_r' in v:
                    path = os.path.join('self_src', 'arch_lite', 'arm', 'cortex_a_r')
                else:
                    path = v

                cmd = 'find {} -name "*.[csS]" |xargs rm -rf'.format(path)
                subprocess.call(cmd, shell = True)
                subprocess.call("find {} -type d -empty -delete".format(path), shell = True)

    for i in extra_del_paths:
        i = i.rstrip('/')
        i = os.path.join('self_src', i)
        if os.path.islink(os.path.join(top_dir, i)):
            link_path = os.path.dirname(os.path.join(top_dir, i))
            i = os.path.join(link_path, os.readlink(i))
        subprocess.call("rm -rf {}".format(i), shell = True)

def libmk_move(lib_array):
    '''
    Move libmk to Makefile
    '''
    for k, v in lib_array.items():
        v = os.path.join('self_src', v)
        if not os.path.exists(v):
            continue

        if k == "libusb_device.a":
            src = os.path.join(v, 'gadget', 'libmk')
            dst = os.path.join(v, 'gadget', 'Makefile')
        elif 'self_src/arch_lite/arm/cortex_a_r' in v:
            path = v[:v.rfind('/')]
            src = os.path.join(path, 'libmk')
            dst = os.path.join(path, 'Makefile')
        else:
            src = os.path.join(v, 'libmk')
            dst = os.path.join(v, 'Makefile')

        if os.path.exists(src):
            cmd = 'mv -f {} {}'.format(src, dst)
            subprocess.call(cmd, shell = True)

def lib_print(lib_dict):
    '''
    Print the missed libs
    '''
    for k, v in lib_dict.items():
        print("platform: {}".format(k))
        print("dont generate as follow:")
        for i in v:
            print('\t{}'.format(i))
        print('\n')

def src_check(lib_array):
    '''
    Check the source code, but dont contain cpp
    '''
    src_left = []
    for k, v in lib_array.items():
        v = os.path.join('self_src', v)
        if not os.path.exists(v):
            continue

        if k == "libusb_device.a":
            path = os.path.join('self_src', 'drivers', 'usb', 'gadget')
            cmd = 'find {} -name "*.[csS]"'.format(path)
            str = os.popen(cmd).readlines()
            if str:
                src_left.extend(str)
            path = os.path.join('self_src', 'drivers', 'usb', 'controller', 'usb_device')
            cmd = 'find {} -name "*.[csS]"'.format(path)
            str = os.popen(cmd).readlines()
            if str:
                src_left.extend(str)
        else:
            if 'self_src/arch_lite/arm/cortex_a_r' in v:
                path = os.path.join('self_src', 'arch_lite', 'arm', 'cortex_a_r')
            else:
                path = v

            cmd = 'find {} -name "*.[csS]"'.format(path)
            str = os.popen(cmd).readlines()
            if str:
                src_left.extend(str)
    if len(src_left):
        print("some source code is left:")
        for i in src_left:
            print(i.strip())

def configin_change(top_dir, platform_paras):
    '''
    Del the config that the feature is not open
    '''
    path = os.path.join(top_dir, 'self_src/tools/menuconfig/config.in')

    for i in platform_paras['configin_del_list']:
        cmd = "sed -i '/{}/d' {}".format(i, path)
        subprocess.call(cmd, shell = True)

    path = os.path.join(top_dir, 'self_src/arch_lite/Kconfig')

    for i in platform_paras['arch_del_list']:
        cmd = "sed -i '/{}/d' {}".format(i, path)
        subprocess.call(cmd, shell = True)

def board_del(platforms, all_platforms, top_dir, paras, product):
    board_kconfig = ''
    if len(platforms) == 1:
        try:
            board_kconfig = paras[product]['platforms'][platforms[0]]['board_kconfig']
        except:
            board_kconfig = ''
    if board_kconfig != '':
        kconfig_name = board_kconfig
    else:
        kconfig_name = product
    for path, dir_list, file_list in os.walk(os.path.join(top_dir, 'self_src', 'targets')):
        for file in file_list:
            if os.path.islink(os.path.join(top_dir, 'self_src', 'targets', file)):
                isDel = True
                for platform in all_platforms:
                    if file.startswith(platform):
                        isDel = False
                        break
                if isDel:
                    subprocess.call("rm self_src/targets/{}".format(file), shell = True)
            if file != 'Kconfig' and file != 'Kconfig.{}'.format(kconfig_name) and \
                file != 'Kconfig.a{}'.format(kconfig_name) and file != 'bsp.mk' and file != 'Makefile' and \
                file != 'CMakeLists.txt' and \
                os.path.exists(os.path.join(top_dir, 'self_src', 'targets', file)):
                subprocess.call("rm self_src/targets/{}".format(file), shell = True)
        for dir in dir_list:
            isDel = True
            for platform in all_platforms:
                if dir.startswith(platform):
                    isDel = False
                    break
            if isDel:
                subprocess.call("rm -rf self_src/targets/{}".format(dir), shell = True)

def del_one_folder_config(platforms, top_dir, folder):
    for path, dir_list, file_list in os.walk(os.path.join(top_dir, folder)):
        for file in file_list:
            fname, ext = os.path.splitext(file)
            isDel = True
            for platform in platforms:
                if fname.startswith(platform):
                    isDel = False
                    break
            if isDel and os.path.exists(os.path.join(top_dir, folder, file)):
                subprocess.call("rm {}/{}".format(folder, file), shell = True)

def config_file_del(platforms, top_dir):
    del_one_folder_config(platforms, top_dir, 'self_src/tools/build/config')
    del_one_folder_config(platforms, top_dir, 'self_src/tools/build/config/debug')
    if not os.listdir('self_src/tools/build/config/debug'):
        subprocess.call("rm -rf self_src/tools/build/config/debug", shell = True)
    subprocess.call("rm -rf self_src/tools/build/config/testsuit", shell = True)

def process_entry(board, miss_dict):
    board.mul_dir()
    board.make_platform()
    board.move_lib()
    board.back_dir()
    miss_dict[board.name] = board.miss[:]

def get_relevant_platforms(platforms, paras, product):
    all_platforms = []
    for cur_platform in platforms:
        all_platforms.append(cur_platform)
        try:
            relevant_platforms = paras[product]['platforms'][cur_platform]['relevant_platform']
        except:
            relevant_platforms = []
        for i in relevant_platforms:
            all_platforms.append(i)
    return all_platforms

def make_lib(platforms, top_dir, paras, product, tar_name):
    subprocess.call("sh self_src/net/install.sh -d", shell = True)

    platforms_dict = []
    if len(platforms) == 1:
        if paras[product]['platforms'][platforms[0]]['lib_build'] == 'True':
            platforms_dict.append(platforms[0])
    elif len(platforms) > 1:
        for cur_platform in platforms:
            if paras[product]['platforms'][cur_platform]['lib_build'] == 'True':
                platforms_dict.append(cur_platform)
    lib_build = False
    if len(platforms_dict) > 0:
        lib_build = True

    lib_array = {}
    lib_array.update(paras['common']['lib_array'])
    lib_array.update(paras[product]['lib_array'])
    if len(platforms_dict) == 1:
        cur_platform = platforms_dict[0]
        board = Platform(cur_platform, top_dir, paras[product]['platforms'][cur_platform]['cpu_type'], paras[product]['platforms'][cur_platform]['config_file'], lib_array)
        board.make_platform()
        board.move_lib()
        # print missed library
        lib_print({board.name: board.miss})
    elif len(platforms_dict) > 1:
        # create multiple processes for multiple platforms
        # init multi-process pool and shared var
        pool = multiprocessing.Pool(len(platforms))
        miss_dict = multiprocessing.Manager().dict()

        for cur_platform in platforms_dict:
            # print("platform is : {}".format(cur_platform))
            board = Platform(cur_platform, top_dir, paras[product]['platforms'][cur_platform]['cpu_type'], paras[product]['platforms'][cur_platform]['config_file'], lib_array)
            pool.apply_async(process_entry, (board, miss_dict, ))

        pool.close()
        pool.join()

        # print missed library
        lib_print(miss_dict)

    if lib_build:
        # move libmk to Makefile
        libmk_move(lib_array)

    # delete the source files
    extra_del_paths = paras['common']['extra_del_paths'] + paras[product]['extra_del_paths']
    src_del(top_dir, lib_array, extra_del_paths, lib_build)

    # check source code
    src_check(lib_array)

    # delete the invalid config items
    # configin_change(top_dir, platform_paras)

    all_platforms = get_relevant_platforms(platforms, paras, product)

    # delete the board
    board_del(platforms, all_platforms, top_dir, paras, product)

    # delete the config files
    config_file_del(all_platforms, top_dir)

    subprocess.call("rm -rf out", shell = True)

    # delete .git
    subprocess.call('find  ./ -name ".git" |xargs rm -rf', shell = True)

    # delete .gitignore
    subprocess.call('find  ./ -name ".gitignore" |xargs rm -rf', shell = True)

    # delete .gitattributes
    subprocess.call('find  ./ -name ".gitattributes" |xargs rm -rf', shell = True)

def dir_change():
    '''
    Change working directory, such as RTOS_Lite
    '''
    path = os.path.abspath(sys.argv[0])
    dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(path))))
    os.chdir(dir)
    return dir

def dir_change_to_parent():
    '''
    Change working directory, parent of RTOS_Lite
    '''
    path = os.path.abspath(sys.argv[0])
    dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(path)))))
    os.chdir(dir)

def get_all_dict_platform(top_dir):
    with open(os.path.join(top_dir, 'self_src', 'tools', 'scripts', 'version_config.cfg'), 'r') as cfg_file:
        content = cfg_file.read()
    paras = json.loads(content)
    return paras

def main():
    top_dir = dir_change()
    paras = get_all_dict_platform(top_dir)

    try:
        bvt_platforms = paras['bvt']['platforms'].keys()
    except:
        bvt_platforms = []
    try:
        iot_platforms = paras['aiot']['platforms'].keys()
    except:
        iot_platforms = []
    try:
        secret_platforms = paras['secret']['platforms'].keys()
    except:
        secret_platforms = []

    isMakeTar = True
    if len(sys.argv) == 2 and (sys.argv[1] == 'bvt' or sys.argv[1] == 'aiot' or sys.argv[1] == 'secret'):
        if sys.argv[1] == 'bvt' and len(bvt_platforms) > 0:
            make_lib(bvt_platforms, top_dir, paras, sys.argv[1], sys.argv[1])
        elif sys.argv[1] == 'aiot' and len(iot_platforms) > 0:
            make_lib(iot_platforms, top_dir, paras, sys.argv[1], sys.argv[1])
        elif sys.argv[1] == 'secret' and len(secret_platforms) > 0:
            make_lib(secret_platforms, top_dir, paras, sys.argv[1], sys.argv[1])
        else:
            isMakeTar = False
            print('No platform in the \"' + sys.argv[1] + '\", please set it to the "version_config.cfg"')
    elif len(sys.argv) == 2 and (sys.argv[1] in bvt_platforms or sys.argv[1] in iot_platforms or sys.argv[1] in secret_platforms):
        if sys.argv[1] in bvt_platforms:
            product = 'bvt'
        elif sys.argv[1] in iot_platforms:
            product = 'aiot'
        elif sys.argv[1] in secret_platforms:
            product = 'secret'
        dst = os.path.join(top_dir, '.config')
        src = os.path.join(top_dir, 'self_src', paras[product]['platforms'][sys.argv[1]]['config_file'])
        cmd_cp = "cp {} {} -f".format(src, dst)
        subprocess.call(cmd_cp, shell = True)
        single_platform = []
        single_platform.append(sys.argv[1])
        make_lib(single_platform, top_dir, paras, product, sys.argv[1])
    else:
        isMakeTar = False
        print("#### Please input one supported platform: ")
        for i in sorted(bvt_platforms):
            print('{}'.format(i))
        for i in sorted(iot_platforms):
            print('{}'.format(i))
        for i in sorted(secret_platforms):
            print('{}'.format(i))
    if isMakeTar:
        dir_change_to_parent()
        subprocess.call('tar -czf liteos.tgz RTOS_Lite/', shell = True)
    return

if __name__ == '__main__':
    main()
