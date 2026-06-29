#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
'''
Check aslr image
'''

import sys
import os
import re
import stat
import shutil
import logging
from collections import namedtuple
sys.path.append(sys.path[0]+"/../")
from util.utils import Utils

LOG_FORMAT = '[%(funcName)s:%(lineno)d, %(levelname)s]: %(message)s'
logging.basicConfig(level=logging.NOTSET, format=LOG_FORMAT)

AslrObjectInfo = namedtuple('AslrObjectInfo', 'type section addr size liba object temp')
ASLR_OBJECT_INFO = []
OUT_DIR = ''
TMP_DIR = ''
LD_FLAGS = []

MAP_TYPE = 'gcc'
MAP_START_RULE = r'Linker script and memory map'
MAP_END_RULE = r'START GROUP'

# reg rules for path & object name: a-z A-Z 0-9 and (special char like % $)
# archive info in section line like:
# (line_begin).laslr          0x0000000000000000      0x490
MAP_FILE_SECTION_LINE = r'(\.\w+)\s+0x([a-fA-F0-9]+)\s+0x([a-fA-F0-9]+)\s*'
# (line_begin)          800000           800000      3dc    32 .laslr
MAP_FILE_LLVM_SECTION_LINE = r'\s+([a-fA-F0-9]+)\s+([a-fA-F0-9]+)\s+([a-fA-F0-9]+)\s+([a-fA-F0-9]+)\s+(\.\w+)'
# reg rules for path & object name: a-z A-Z 0-9 and (special char like % $)
# archive info in one line like:
# (line_begin) .laslr.abc  0x1234abcd 0x1f5 /home/usr2/x/out/libtest.a(a.o)(line_end)
MAP_AFILE_IN_ONE_LINE = r'^\s*(\.laslr\.\w+)\s+0x([a-fA-F0-9]+)\s+0x([a-fA-F0-9]+)\s+(\S+\.a)\((\S+\.o)\)\s*$'
# object info in one line like:
# (line_begin) .laslr.abc  0x1234abcd 0x1f5 /home/usr2/x/out/libtest.o(line_end)
MAP_OFILE_IN_ONE_LINE = r'^\s*(\.laslr\.\w+)\s+0x([a-fA-F0-9]+)\s+0x([a-fA-F0-9]+)\s+(\S+\.o)\s*$'
# object info in one line like:
#          8002a0           8002a0       20     8         /out/libtargets.a(aslr_func.o):(.laslr.abc)
MAP_LLVM_IN_ONE_LINE = r"\s+([a-fA-F0-9]+)\s+([a-fA-F0-9]+)\s+([a-fA-F0-9]+)\s+" \
    "([a-fA-F0-9]+)\s+(\S+\.a)\((\S+\.o)\)\:\((\.laslr\.\w+)\)\s*"
# archive info in two line like:
# (line1_begin) .laslr.abc                                        (line1_end)
# (line2_begin)   0x1234abcd 0x1f5 /home/usr2/x/out/libtest.a(a.o)(line2_end)
MAP_AFILE_IN_TWO_LINE = r'^\s*(\.laslr\.\w+)\s*$'


def clean_dir(directory):
    '''
    Clean temporary directory
    '''
    image_path = os.path.join(directory, 'image')
    object_path = os.path.join(directory, 'object')
    temp_path = os.path.join(directory, 'temp')
    remkdir(image_path)
    remkdir(object_path)
    remkdir(temp_path)


def remkdir(temp_dir):
    '''
    Create temporary directory
    '''
    if os.path.exists(temp_dir): # delete tmp dir
        shutil.rmtree(temp_dir)
    os.makedirs(temp_dir)


def check_first_section(start_line_num, lines):
    '''
    Check aslr section
    '''
    pattern = re.compile(MAP_FILE_SECTION_LINE)
    for line_num in range(start_line_num, len(lines)):
        groups = pattern.match(lines[line_num])
        if groups:  # is section line info
            break
    logging.info("map first section: %s", groups[1])
    if groups[1] != '.laslr':
        logging.error("The first section is not .laslr.")
        sys.exit(1)


def check_first_section_llvm(start_line_num, lines):
    '''
    Check aslr section
    '''
    pattern = re.compile(MAP_FILE_LLVM_SECTION_LINE)
    for line_num in range(start_line_num, len(lines)):
        groups = pattern.match(lines[line_num])
        if groups:  # is section line info
            break
    logging.info("map first section: %s", groups[5])
    if groups[5] != '.laslr':
        logging.error("The first section is not .laslr.")
        sys.exit(1)


def info_in_twoline(line):
    '''
    Match two rows in map file
    '''
    pattern1 = re.compile(MAP_AFILE_IN_TWO_LINE)
    groups1 = pattern1.match(line)
    return bool(groups1)


def record_liba_object(line):
    '''
    Record object list for aslr image
    '''
    global ASLR_OBJECT_INFO
    pattern2 = re.compile(MAP_AFILE_IN_ONE_LINE)
    groups2 = pattern2.match(line)
    if groups2: # .a(.o)
        # groups in MAP_AFILE_IN_ONE_LINE rule is:
        # 1.section 2.addr 3.size 4.liba path 5.object name
        logging.info(" ".join(['liba', groups2.group(1), groups2.group(2), \
            groups2.group(3), groups2.group(4), groups2.group(5)]))
        info = AslrObjectInfo('liba', groups2.group(1), groups2.group(2), \
            groups2.group(3), groups2.group(4), groups2.group(5), '')
        ASLR_OBJECT_INFO.append(info)
        return
    pattern3 = re.compile(MAP_OFILE_IN_ONE_LINE)
    groups3 = pattern3.match(line)
    if groups3: # .o
        # groups in MAP_OFILE_IN_ONE_LINE rule is:
        # 1.section 2.addr 3.size 4.object path
        logging.info(" ".join(['object', groups3.group(1), groups3.group(2), \
            groups3.group(3), groups3.group(4)]))
        info = AslrObjectInfo('object', groups3.group(1), groups3.group(2), \
            groups3.group(3), '', '', groups3.group(4))
        ASLR_OBJECT_INFO.append(info)
        return

    pattern4 = re.compile(MAP_LLVM_IN_ONE_LINE)
    groups4 = pattern4.match(line)
    if groups4: # .o
        # groups in MAP_LLVM_IN_ONE_LINE rule is:
        # 7.section 1.addr 3.size 5.liba path 6.object name
        logging.info(" ".join(['liba', groups4.group(7), groups4.group(1), \
            groups4.group(3), groups4.group(5), groups4.group(6)]))
        info = AslrObjectInfo('liba', groups4.group(7), groups4.group(1), \
            groups4.group(3), groups4.group(5), groups4.group(6), '')
        ASLR_OBJECT_INFO.append(info)
        return
    return


def get_aslr_file(map_file):
    '''
    Get aslr image information from map file
    '''
    is_start = False
    is_end = False
    multi_line = []
    with open(map_file, 'r') as file_handle:
        lines = file_handle.readlines()
        for line_num in range(len(lines)):
            line = lines[line_num]
            if len(multi_line) > 0:
                multi_line.append(line)
                merge_line = re.sub(r'\n', '', ''.join(multi_line))
                record_liba_object(merge_line)
                multi_line[:] = [] # clear list adapt to py2
            if MAP_TYPE == 'llvm' and not is_start:
                is_start = True
                check_first_section_llvm(line_num + 1, lines)
            if re.match(MAP_START_RULE, line):
                is_start = True
                check_first_section(line_num + 1, lines) # first section is laslr?
            elif re.match(MAP_END_RULE, line):
                is_end = True
            elif is_start and (not is_end):
                if info_in_twoline(line):
                    multi_line.append(line)
                    continue
                record_liba_object(line)


def copy_object_in_liba(temp_dir):
    '''
    Copy object for making aslr image
    '''
    global ASLR_OBJECT_INFO
    obj_path = os.path.join(temp_dir, 'object')
    tmp_path = os.path.join(temp_dir, 'temp')

    lst_no_addr = []
    for item in ASLR_OBJECT_INFO: # delete duplicate file name
        info = AslrObjectInfo(item.type, '', '', \
           '', item.liba, item.object, item.temp)
        if info not in lst_no_addr:
            lst_no_addr.append(info)

    ld_objects = []
    for item in lst_no_addr:
        if item.type == 'object':
            ld_objects.append(item.temp)
        elif item.type == 'liba':
            if os.path.exists(tmp_path): # delete tmp dir
                shutil.rmtree(tmp_path)
            os.makedirs(tmp_path)

            shutil.copy(item.liba, tmp_path) # copy to temp dir: OUT/aslr/object/
            liba_name = os.path.basename(item.liba)
            liba_name_lst = os.path.splitext(liba_name)
            liba_name_pre = liba_name_lst[-2] # -2: get liba name
            cmd_list = ['ar', '-x', liba_name]
            _, return_code = Utils.run_shell_return(cmd_list, tmp_path)
            if return_code != 0:
                logging.error("%s return %s", cmd_list, return_code)
                sys.exit(1)
            obj_copy_dst = os.path.join(obj_path, '_'.join([liba_name_pre, item.object]))
            cmd_list = ['cp', '-r', item.object, obj_copy_dst]
            _, return_code = Utils.run_shell_return(cmd_list, tmp_path)
            if return_code != 0:
                logging.error("%s return %s", cmd_list, return_code)
                sys.exit(1)
            ld_objects.append(obj_copy_dst)
    return ld_objects


def link_objects(compiler, ldflags, objects, directory):
    '''
    Link object to generate aslr image
    '''
    # link cmd like:
    # ld -o image -Map=image.map --start-group a.o b.o c.o --end-group
    ld_cmd = ''.join([compiler, 'ld'])
    if MAP_TYPE == 'llvm':
        ld_cmd = 'ld.lld'
    dump_cmd = ''.join([compiler, 'objdump'])
    aslr_image = os.path.join(directory, 'image', 'aslr_image')
    aslr_map = os.path.join(directory, 'image', 'aslr_image.map')
    aslr_asm = os.path.join(directory, 'image', 'aslr_image.asm')
    cmd_list = [ld_cmd]
    cmd_list.extend(ldflags)
    cmd_list.extend(['-o', aslr_image, f'-Map={aslr_map}'])
    cmd_list.append('--start-group')
    cmd_list.extend(objects)
    cmd_list.append('--end-group')

    out_text, return_code = Utils.run_shell_return(cmd_list, None)
    if return_code != 0:
        logging.error("%s return %s", cmd_list, return_code)
        logging.error(out_text)
        sys.exit(1)

    dump_list = [dump_cmd, '-dh', aslr_image]
    out_text, return_code = Utils.run_shell_return(dump_list, None)
    if return_code != 0:
        logging.error("%s return %s", dump_list, return_code)
        logging.error(out_text)
        sys.exit(1)
    # open with write only | open with creat | open with trunc
    flags = os.O_WRONLY | os.O_CREAT | os.O_TRUNC
    # Write by owner | Read by owner | Read by group | Read by others
    modes = stat.S_IWUSR | stat.S_IRUSR | stat.S_IRGRP | stat.S_IROTH
    with os.fdopen(os.open(aslr_asm, flags, modes), 'w') as file_handle:
        file_handle.writelines(out_text)
    return aslr_image


def cmp_bin(vs_bin_file, aslr_bin_file, cmp_size):
    '''
    Byte comparison for two image
    '''
    stat1 = os.stat(vs_bin_file)
    stat2 = os.stat(aslr_bin_file)

    if stat1.st_size != stat2.st_size:
        return False

    size = min(cmp_size, 4096) # compare 4k
    with open(vs_bin_file, 'rb') as fp1, \
        open(aslr_bin_file, 'rb') as fp2:
        while cmp_size > 0:
            buf1 = fp1.read(size)
            buf2 = fp2.read(size)
            cmp_size = cmp_size - size
            if buf1 != buf2:
                return False
            if not buf1:
                return False
        return True


def compare_two_image(compiler, aslr_image, vs_server):
    '''
    Compare two image
    '''
    global ASLR_OBJECT_INFO
    bin_dir = os.path.dirname(os.path.realpath(aslr_image))
    aslr_bin = os.path.join(bin_dir, 'aslr_image.bin')
    vs_bin = os.path.join(bin_dir, 'vs_image.bin')
    objcopy = ''.join([compiler, 'objcopy'])

    cmd_list = [objcopy, '-O', 'binary', '--only-section', '.laslr']

    cmd_list.append(aslr_image)
    cmd_list.append(aslr_bin)
    out_text, return_code = Utils.run_shell_return(cmd_list, None)
    if return_code != 0:
        logging.error("%s return %s", cmd_list, return_code)
        logging.error(out_text)
        sys.exit(1)

    cmd_list.pop()
    cmd_list.pop()
    cmd_list.append(vs_server)
    cmd_list.append(vs_bin)
    out_text, return_code = Utils.run_shell_return(cmd_list, None)
    if return_code != 0:
        logging.error("%s return %s", cmd_list, return_code)
        logging.error(out_text)
        sys.exit(1)

    # get different section .laslr.data size
    data_section_size = 0
    for item in ASLR_OBJECT_INFO:
        if item.section in ('.laslr.init', '.laslr.text'):
            data_section_size = data_section_size + int(item.size, 16)
    is_same = cmp_bin(vs_bin, aslr_bin, data_section_size)
    if not is_same:
        logging.error('section .laslr.init or .laslr.text of aslr image invalid')
        sys.exit(1)


# argv[1]: elf file with path (create aslr dir to store tmp file)
# argv[2]: map file
# argv[3]: compile path
# argv[4...]: ld flags
if __name__ == '__main__':
    logging.info("enter %s", sys.argv[0])
    if len(sys.argv) < 5:
        logging.error("argv number error: need 5 argv at least, pass %s", len(sys.argv))
        for index in range(len(sys.argv)):
            logging.error("%s %s", index, sys.argv[index])
        sys.exit(1)
    else:
        OUT_DIR = os.path.dirname(os.path.realpath(sys.argv[1]))
        if not ((len(sys.argv[1]) > 0) and (os.path.isdir(OUT_DIR)) \
            and (os.path.isfile(sys.argv[1]))):
            logging.error("elf file invalid %s", sys.argv[1])
            sys.exit(1)
        if not ((len(sys.argv[2]) > 0) and (os.path.isfile(sys.argv[2]))):
            logging.error("map file invalid %s", sys.argv[2])
            sys.exit(1)
        TMP_DIR = os.path.join(OUT_DIR, 'aslr')
        LD_FLAGS = sys.argv[4:]
        clean_dir(TMP_DIR)
        if sys.argv[3] == 'llvm-':
            MAP_TYPE = 'llvm'
        get_aslr_file(sys.argv[2])
        obj_name = copy_object_in_liba(TMP_DIR)
        aslr_file = link_objects(sys.argv[3], LD_FLAGS, obj_name, TMP_DIR)
        compare_two_image(sys.argv[3], aslr_file, sys.argv[1])
