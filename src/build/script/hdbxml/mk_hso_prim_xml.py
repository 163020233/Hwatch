#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2026. All rights reserved.

import os
import re
import sys
import platform
import string
import shutil
import time
import json
import xml.etree.ElementTree as ET
from multiprocessing import Pool, cpu_count

py_version = platform.python_version()

def timestamp():
    return time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(time.time()))

##########################################################################
#
# 日志内容提取函数
#
##########################################################################
# 日志状态机处理流程：
# LOG_ENTRY_FINDING              -- 查找日志接口
# LOG_ENTRY_FOUNDED              -- 日志接口匹配，查找日志内容
# LOG_CONT_ENTRY_FINDING         -- 日志内容入口匹配，以“号开关，以”号结束
# LOG_CONT_END_FOUNING           -- 日志内容查找
# LOG_EXIT_FINDING               -- 日志线束符匹配

g_log_entry = [
    r"LOG_PRINT[0-4](\s*)\(",
    r"log_print[0-4x](\s*)\(",
    r"acore_log[0-6](\s*)\(",
    r"[printlog]{3,5}_alter(\s*)\(",
    r"[PRINTLOG]{3,5}_ALTER(\s*)\(",
    r"_LOG[0-9]{0,2}(\s*)\(",
    r"(DBG|INFO|ERR|WARN)_LOG(\s*)\(",
    # BTH
    r"(dbg|info|err|warn)_log(\s*)\(",
    r"(dbg|info|err|warn)_log[0-4](\s*)\(",
    # diag common
    r"diag_(error|warning|info|debug)_log(\d*)(\s*)\(",
    r"PRINT_(DEBUG|INFO|ERR|WARN)(\s*)\(",
    r"print_(debug|info|err|warn)(\s*)\(",
    r"_log_(err|warn|info|debug)(\s*)\(",
    r"oam_(err|warning|info)_log(\s*)\(",
    # GPU
    r"(vau|dpu|jpeg|gmmu|vg)_(err|info|dbg|print)(\s*)\(",
    # WIFI
    r"oam_(error|warning|info)_log[0-4](\s*)\(",
    r"alg_ar_dbg_log[0-4](\s*)\(",
    r"common_log_dbg[0-4](\s*)\(",
    r"oam_notice_log[0-4](\s*)\(",
    # GPU
    r"GRAPHIC_LOG(F|E|W|I|D|P)(\s*)\(",
    r"DISP_LOG(E|W|I|D)(\s*)\(",
    r"LV_LOG(E|W|I)(\s*)\(",
    # Meida
    r"ALOG(V|D|I|W|E)(\s*)\(",
    r"MEDIA_(DEBUG|ERR|WARNING|INFO|FATAL)_LOG(\s*)\(",
    r"HMF_LOG(F|E|W|I)(\s*)\(",
    r"MEDIA_HAL_LOG(V|D|I|W|E)(\s*)\(",
    r"CHECK_NULL_(RETURN|RETURN_NONE)(\s*)\(",
    r"CHECK_FAILED_(RETURN|RETURN_NONE|PRINT|RETURN_WITH_UNLOCK)(\s*)\(",
    r"CHK_NULL_(RETURN|RETURN_NONE|RETURN_WITH_UNLOCK)(\s*)\(",
    r"CHK_FAILED_(RETURN|NO_RETURN)(\s*)\(",
    r"(CHK_COND_RETURN|CHK_FALSE_RETURN|CHECK_STATE_SAME_RETURN)(\s*)\(",
    # OHOS
    r"HILOG_(DEBUG|ERROR|FATAL|INFO|WARN)(\s*)\(",
    r"SAMGR_LOG_(ERROR|FATAL|INFO|WARN)(\s*)\(",
    # app
    r"WEARABLE_LOG(D|I|W|E)(\s*)\(",
    # plat
    r"hcc_printf_err_log(\s*)\(",
    # carkey app
    r"carkey_log(_addr|_float(_7)?)?(\s*)\(",
    # audio
    r"audio_log[0-4x](\s*)\(",
    r"osal_log(\s*)\(",
]  # 日志关键字规则

# DSP日志关键字规则，不要改变顺序，慎重修改
g_audio_log_entry = [
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_(s32|bool)(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_u32(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_h32(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_float(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_void(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_ret(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_fun(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_null_pointer(\s*)\(",
    r"audio_func_enter(\s*)\(",
    r"audio_func_exit(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_info(\s*)\(",
    r"audio_log_(alert|fatal|err|warn|info|dbg|trace)(\s*)\(",
    r"sap_debug_module_handle(\s*)\(",
]  # DSP日志关键字规则，不要改变顺序，慎重修改


LOG_ENTRY_RE      = [re.compile(p) for p in g_log_entry]
AUDIO_LOG_RE      = [re.compile(p) for p in g_audio_log_entry]
RE_LOG_FULL_CONT  = re.compile(r'\"(.*)\"\s*[,|)]')
RE_COMMENT_FIND   = re.compile(r'^\/\/')
RE_LOG_EXIT       = re.compile(r'\)[\s]*\;')
RE_LOG_CONT_END   = re.compile(r'[\s]+(.+)\"')
RE_LOG_CONT_PART  = re.compile(r'\"(.+)')
RE_LOG_CONT_PART2 = re.compile(r'(.+)"\s*[,|)]')

# 全局变量
arch_name = ""
g_undefine_id = []  # 保存未定义的FileId
g_fsm_status = "LOG_ENTRY_FINDING"
g_log_key_find = 0  # （状态位）判断是否查找到日志关键字，0否1是
g_log_str = ""
g_log_str_full = ""

# 用于记录日志行数，日志多行时可能是第一行，有可能是最后一行；
# 0 表示第一行  1表示最后一行
g_log_entry_or_end_flag = -1

g_audio_log_key_index = -1
g_audio_log_addition_info = [" = %d",
                             " = %u",
                             " = 0x%08X",
                             " = %f",
                             " = %p",
                             "Error Code: [0x%08X]",
                             " Failed, Error Code: [0x%08X]",
                             " = %p,  Null Pointer!\\n",
                             "enter...",
                             "exit..."]

XML_PATH="build/config/target_config/<chip>/hdb_config/database_cfg/mk_hdb_xml.json"

g_db_conf_cache = {}

g_error_patterns = [
    r'LOG_LEVEL_ERROR|ERR_LOG|err_log|(GRAPHIC|DISP|LV)_LOGF|(GRAPHIC|DISP|LV)_LOGE|WEARABLE_LOGE|_ERR|print_err|FATAL|ERROR',
    r'(CHK|CHECK)_NULL_RETURN|(CHK|CHECK)_NULL_RETURN_NONE|(CHK|CHECK)_FAILED_RETURN|CHECK_FAILED_RETURN_NONE',
    r'CHECK_FAILED_PRINT|CHECK_FAILED_RETURN_WITH_UNLOCK|CHK_NULL_RETURN_WITH_UNLOCK|CHK_FAILED_NO_RETURN',
    r'CHK_COND_RETURN|CHK_FALSE_RETURN',
    r'_error_log|_log_err|_ERROR_|(vau|dpu|jpeg|gmmu|vg)_err|ALOGE|MEDIA_(ERR|FATAL)_LOG|HMF_LOG(F|E)|MEDIA_HAL_LOGE'
]
g_warning_patterns = [
    r'LOG_LEVEL_WARNING|WARN_LOG|warn_log|warning_log|(GRAPHIC|DISP|LV)_LOGW|WEARABLE_LOGW|_ALERT|_WARN|print_warn',
    r'alg_ar_dbg|common_log_dbg|oam_notice_log',
    r'_warning_log|_log_warn|_WARNING_|ALOGW|HMF_LOGW|MEDIA_HAL_LOGW'
]

g_info_patterns = [
    r'LOG_LEVEL_INFO|INFO_LOG|info_log|(GRAPHIC|DISP|LV)_LOGI|GRAPHIC_LOGP|WEARABLE_LOGI|_INFO|print_info|osal_log',
    r'CHECK_STATE_SAME_RETURN',
    r'_info_log|_log_info|_LOG_TEST|_LOG_IQ|_INFO_|(vau|dpu|jpeg|gmmu|vg)_info|ALOGI|MEDIA_INFO_LOG|HMF_LOGI|MEDIA_HAL_LOGI'
]

g_debug_patterns = [
    r'LOG_LEVEL_DBG|(DBG|DEBUG)_LOG|(dbg|debug)_log|(GRAPHIC|DISP|LV)_LOGD|WEARABLE_LOGD|_DEBUG|print_debug',
    r'_debug_log|_log_debug|(vau|dpu|jpeg|gmmu|vg)_(dbg|print)|ALOG(V|D)|MEDIA_DEBUG_LOG|MEDIA_HAL_LOG(V|D)'
]

g_audio_error_patterns = [
    r'_fatal|_err',
]
g_audio_warning_patterns = [
    r'_alert|_warn',
    r'sap_debug_module_handle',
]
g_audio_info_patterns = [
    r'_info|_dbg|_trace',
    r'_enter|_exit'
]

LOG_LEVEL_ERROR_RE     = [re.compile(p) for p in g_error_patterns]
LOG_LEVEL_WARN_RE      = [re.compile(p) for p in g_warning_patterns]
LOG_LEVEL_INFO_RE     = [re.compile(p) for p in g_info_patterns]
LOG_LEVEL_DBG_RE      = [re.compile(p) for p in g_debug_patterns]

AUDIO_LOG_LEVEL_ERROR_RE     = [re.compile(p) for p in g_audio_error_patterns]
AUDIO_LOG_LEVEL_WARN_RE      = [re.compile(p) for p in g_audio_warning_patterns]
AUDIO_LOG_LEVEL_INFO_RE     = [re.compile(p) for p in g_audio_info_patterns]

AUDIO_LOG_FOUND_RE          = re.compile(r'\(\s*(\S+)\s*\)')
AUDIO_LOG_FOUND_2_RE        = re.compile(r'\(\s*(\S+)\s*,')
AUDIO_LOG_NULL_FOUND_RE     = re.compile(r'\(\)')

def get_log_level(str):
    for idx, pat in enumerate(LOG_LEVEL_ERROR_RE):
        if pat.search(str):
            return 2,'ERROR'

    for idx, pat in enumerate(LOG_LEVEL_WARN_RE):
        if pat.search(str):
            return 3,'WARNING'

    for idx, pat in enumerate(LOG_LEVEL_INFO_RE):
        if pat.search(str):
            return 5,'INFO'

    for idx, pat in enumerate(LOG_LEVEL_DBG_RE):
        if pat.search(str):
            return 6,'DBG'

    print("no match for :%s" % str)
    return 7, 'no match'

# DSP日志级别识别，慎重修改
def get_audio_log_level(str):
    for idx, pat in enumerate(AUDIO_LOG_LEVEL_ERROR_RE):
        if pat.search(str):
            return 2,'ERROR'

    for idx, pat in enumerate(AUDIO_LOG_LEVEL_WARN_RE):
        if pat.search(str):
            return 3,'WARNING'

    for idx, pat in enumerate(AUDIO_LOG_LEVEL_INFO_RE):
        if pat.search(str):
            return 5,'INFO'
    print("audio log no match for :%s" % str)
    return 7, 'no match'

def add_content_to_xml(data, msg_root_element):
    prim_pri  = data["LEVEL_STR"]
    prim_sz =  data["FMT"]
    prim_line = data["LINE_NUM"]
    prim_file = data["FILE_NAME"]
    prim_file_id = data["FILE_ID"]
    args_count = data["ARGS_COUNT"]
    mod_id = data["MOD_ID"]
    level = data["LEVEL_NUM"]

    new_element = ET.Element('')
    new_element.tag = 'MSG'
    new_element.attrib['STRUCTURE'] = "%s%d" % ("diag_log_msg", args_count)
    new_element.attrib['NAME'] = "%s @%s(%d),%s" % (prim_sz, prim_file, prim_line, prim_pri)

    #sdt log_id to hso msg_id
    #log_flg  mod_id   file_id   line_num   level
    #1bit     4bit     10bit     14bit      3bit
    msg_id = (1 << 31) | (mod_id << 27) | (prim_file_id << 17) | (prim_line << 3) | level
    new_element.attrib['ID'] = hex(msg_id)
    new_element.tail = '\n\t\t\t'
    msg_root_element.append(new_element)

# 根据日志状态机流程逐行解析日志内容
def parse_log_from_line(line):
    global g_fsm_status, g_log_str, g_log_str_full, g_log_key_find
    global g_audio_log_key_index, g_log_entry_or_end_flag
    global g_log_entry

    # 匹配日志接口关键字
    if g_fsm_status == "LOG_ENTRY_FINDING":
        g_log_entry_or_end_flag = -1
        g_audio_log_key_index = -1
        g_log_str = ""
        g_log_str_full = ""

        for idx, pat in enumerate(AUDIO_LOG_RE):
            if pat.search(line):
                # 匹配到Audio日志关键字，直接返回
                g_fsm_status = "LOG_ENTRY_FOUNDED"
                g_log_str_full += line
                g_log_entry_or_end_flag = 0
                g_audio_log_key_index = idx
                g_log_key_find = 1
                parse_log_from_line(line)
                return

        for idx, pat in enumerate(LOG_ENTRY_RE):
            if pat.search(line):
                # 匹配到通用日志关键字
                g_fsm_status = "LOG_ENTRY_FOUNDED"
                g_log_str_full += line
                g_log_entry_or_end_flag = 0
                g_log_key_find = 1
                parse_log_from_line(line)

    # 日志接口匹配，确认是否完整日志；确认日志是否正常结束
    elif g_fsm_status == "LOG_ENTRY_FOUNDED":
        if 0 <= g_audio_log_key_index <= 4:   # such as "sap_alert_log_u32"
            m1 = AUDIO_LOG_FOUND_RE.search(line)
            if m1:
                g_log_str = m1.group(1) + g_audio_log_addition_info[g_audio_log_key_index] + '\0'
                g_fsm_status = "LOG_EXIT_FINDING"
                parse_log_from_line(line)
        elif g_audio_log_key_index == 5: # such as "sap_fatal_log_err_code"
            m1 = AUDIO_LOG_FOUND_RE.search(line)
            if m1:
                g_log_str = g_audio_log_addition_info[g_audio_log_key_index] + '\0'
                g_fsm_status = "LOG_EXIT_FINDING"
                parse_log_from_line(line)
        elif g_audio_log_key_index == 6: # such as "sap_err_log_fun"
            m1 = AUDIO_LOG_FOUND_2_RE.search(line)
            if m1:
                g_log_str = "Call " + m1.group(1) + g_audio_log_addition_info[g_audio_log_key_index] + '\0'
                g_fsm_status = "LOG_EXIT_FINDING"
                parse_log_from_line(line)
        elif g_audio_log_key_index == 7: # such as "sap_err_log_null_pointer"
            m1 = AUDIO_LOG_FOUND_RE.search(line)
            if m1:
                g_log_str = m1.group(1) + g_audio_log_addition_info[g_audio_log_key_index] + '\0'
                g_fsm_status = "LOG_EXIT_FINDING"
                parse_log_from_line(line)
        elif g_audio_log_key_index == 8 or g_audio_log_key_index == 9: # such as "audio_func_enter"
            m1 = AUDIO_LOG_NULL_FOUND_RE.search(line)
            if m1:
                g_log_str = g_audio_log_addition_info[g_audio_log_key_index] + '\0'
                g_fsm_status = "LOG_EXIT_FINDING"
                parse_log_from_line(line)
        else:
            # 通用日志
            m1 = RE_LOG_FULL_CONT.search(line)  # 完整日志格式
            m2 = RE_LOG_CONT_PART.search(line)  # 部分日志格式（以引号开头）
            if m1:
                # 当前行匹配到完整日志格式，继续查找结束符，完成解析
                g_log_str = m1.group(1) + '\0'
                g_fsm_status = "LOG_EXIT_FINDING"
                parse_log_from_line(line)
            elif m2:
                g_fsm_status = "LOG_CONT_END_FINDING"
                g_log_str = m2.group(1)  # 引号后面的内容为日志内容
                # 检查是否已经在同一行结束
                if RE_LOG_CONT_PART2.search(line):
                    g_fsm_status = "LOG_EXIT_FINDING"
                # 继续读取后续行
            else:
                # 没有匹配到日志内容（字符串），检查是否是空日志调用
                if RE_LOG_EXIT.search(line.strip()):
                    g_fsm_status = "LOG_ENTRY_FINDING"
                    g_log_entry_or_end_flag = -1
                    g_audio_log_key_index = -1
                    print("warning: can't find log str in line: %s" % line)
            if g_log_str_full.find(line) == -1:
                g_log_str_full += line
            return

    # 匹配部分日志接口
    elif g_fsm_status == "LOG_CONT_END_FINDING":
        m = RE_LOG_CONT_END.search(line)
        if m:
            temp = m.group(1)
            if not RE_LOG_CONT_PART2.search(line):  #部分日志可以跨多行
                g_log_str += temp
            else:
                g_log_str += (temp + '\0')
                g_fsm_status = "LOG_EXIT_FINDING"
                parse_log_from_line(line)
        else:
            g_log_str += line.strip()  # 纯日志内容 ，继续找日志
        if g_log_str_full.find(line) == -1:
            g_log_str_full += line
        return
    # 匹配日志结束符
    elif g_fsm_status == "LOG_EXIT_FINDING":
        if RE_LOG_EXIT.search(line.strip()):
            g_fsm_status = "LOG_ENTRY_FINDING"  # 继续逐行扫描下一条日志入口
            g_log_entry_or_end_flag = 1
        if g_log_str_full.find(line) == -1:
            g_log_str_full += line
        return

g_last_file_id_num = 0
g_moudle_id = {
    'wifi' : 0,
    'bt' : 1,
    'gnss' :2,
    'dsp' : 3,
    'pf' : 4,
    'media' : 5,
    'nfc': 6,
    'app' : 7,
    'gpu': 8,
    'gui' : 9,
    'slp' : 10,
    'bth' : 11,
    'ohos' : 12,
    'bgh' : 13,
    'alg' : 14,
}

# ----------------------------------------------------------------------
#   读取源码文件
# ----------------------------------------------------------------------
def parse_single_source_file(cfilepath, auto_def, module_name, file_id_dict):
    global g_log_key_find, g_fsm_status, g_audio_log_key_index
    global g_log_str
    curr_line_num = 0  # 当前行行号
    file_id_num = 0
    line_num_save = 0
    log_info_list = [] # 日志信息列表，【文件号， 行号， 日志内容】

    m = re.search(r"\w.[cC|hH]", cfilepath)
    if m is None:
        print("error: %s is not C or H file" % cfilepath)
        return []

    # -------------------------------------------------
    #   打开源文件，读取文件内容
    # -------------------------------------------------
    try:
        if py_version.startswith("3"):
            try:
                with open(cfilepath, 'r', encoding="UTF-8") as fd_src:
                    datalines = [line for line in fd_src]
            except Exception:
                with open(cfilepath, 'r', encoding="ISO-8859-1") as fd_src:
                    datalines = [line for line in fd_src]
        else:
            with open(cfilepath, 'r') as fd_src:
                datalines = [line for line in fd_src]
    except Exception as e:
        print(f"error: open {cfilepath} failed: {e}")
        return []

    if module_name not in file_id_dict:
        print("warning: not add log_def_%s.h into set(LOG_DEF)" % module_name)
        return []

    _, filename = os.path.split(cfilepath)
    if auto_def:
        file_id_str = filename.replace('.' , '_').upper()
        if file_id_str not in file_id_dict[module_name]:
            return []
        g_fsm_status = "LOG_ENTRY_FINDING"
    else:
        m = re.search(r"#define[\s]+THIS_FILE_ID[\s]+([\w]*)", '\n'.join(datalines))
        if m is None:
            return []
        file_id_str = m.group(1)
        g_fsm_status = "LOG_ENTRY_FINDING"

    # -------------------------------------------------
    #   行遍历
    # -------------------------------------------------
    for line in datalines:
        # 行号加1
        curr_line_num += 1

        # 跳过注释行
        if RE_COMMENT_FIND.search(line.strip()):
            continue

        # 解析当前行
        parse_log_from_line(line)

        # 判断是否找到日志关键字
        if g_log_key_find == 1:
            try:
                file_id_num = file_id_dict[module_name][file_id_str]
                moudle_id_num = g_moudle_id[module_name]
            except KeyError:
                print( "warning: Module:%s, %s not defined. fileName[%s]." %(module_name, file_id_str, filename))
                g_undefine_id.append((file_id_str, filename))
                break
            g_log_key_find = 0
            line_num_save = curr_line_num

        # 判断是否已找到完整日志，如果找到，存入log_info_list
        if g_log_entry_or_end_flag == 1:
            g_log_str = g_log_str.replace('\\r', "").replace('\\n', "").replace('\r', "").replace('\n', "").replace("\\", "").replace('""', "").replace("\000", "")
            g_log_str.strip()

            # 通过log打印字符串格式之前的字符串判断level
            g_log_level_str = g_log_str_full[0:g_log_str_full.find('"')]

            # 获取日志级别
            if (g_audio_log_key_index != -1):
                level_num, level_str = get_audio_log_level(g_log_level_str)
            else:
                level_num, level_str = get_log_level(g_log_level_str)

            data = {
                "MOD_ID": moudle_id_num,
                "FILE_ID": file_id_num,
                "LINE_NUM": line_num_save if arch_name == "cortex_m7" else curr_line_num,
                "FMT": g_log_str,
                "LEVEL_NUM": level_num,
                "LEVEL_STR": level_str,
                "ARGS_COUNT": g_log_str.count("%") - g_log_str.count("%%") * 2,
                "FILE_NAME": cfilepath.split('/')[-1],
            }

            log_info_list.append(data)
            g_log_key_find = 0
            g_audio_log_key_index = -1
    return log_info_list


# 保存FILD_ID
def save_file_id_dict(line_str, moudle_name, file_id_dict):
    global g_last_file_id_num

    file_id_str = ''
    file_id_num = 0
    if line_str == '':
        return 0

    m2 = re.search("[^A-Za-z0-9_=]", line_str)
    if m2 is None:
        m1 = line_str.split('=')  # XXXXXXX = dddd,
        if len(m1) == 2:
            file_id_str = m1[0]  # 获取file id字符串
            file_id_num = m1[1]
            file_id_dict[moudle_name][file_id_str] = int(file_id_num)
            g_last_file_id_num = int(file_id_num) + 1
        else:
            file_id_str = line_str
            file_id_dict[moudle_name][file_id_str] = g_last_file_id_num
            g_last_file_id_num += 1
    else:
        return -1
    return 0


# 打印fileid的映射字典
def print_file_id_dict(file_id_dict):
    print(timestamp() + " start to print FileIdDict::")
    for system_name in file_id_dict:
        for key in file_id_dict[system_name]:
            print(timestamp() + " key:%s value:%d" % (key, file_id_dict[system_name][key]))

# 获取FILE_ID枚举值
def get_file_id_enum_in_file(h_file_path, file_id_dict):
    global g_last_file_id_num
    g_last_file_id_num = 0

    moudle_name = os.path.basename(h_file_path).split('_')[-1][:-2]
    # btc将file_id头文件迁移后取名为log_def_btc.h 实际moudle_name任是bt
    # bth将file_id头文件迁移后取名为log_def_bth.h 实际moudle_name任是bt
    if moudle_name == 'btc':
        moudle_name = 'bt'
    if chip == 'bs25' and group_name == 'bt_core' and (moudle_name == "bth" or moudle_name == "rom"):
        moudle_name = 'bt'
    if moudle_name not in file_id_dict:
        file_id_dict[moudle_name] = {}
    m = re.search(r'\w+\.h$', h_file_path)
    if m is None:
        print(timestamp() + ' Wrong type file, please input head file path.')
        return -1

    if not os.path.exists(h_file_path):
        print(timestamp() + ' Head file [%s] did not exist.' % (h_file_path))
        return 0
    try:
        if py_version.startswith("3"):
            with open(h_file_path, 'r', encoding="UTF-8") as fd:
                id_file = fd.read()
        else:
            with open(h_file_path, 'r') as fd:
                id_file = fd.read()
    except Exception as e:
        print(e)
        return -1
    id_list = re.search(r"typedef enum[\s]*\{([\d\D]*?)\}", id_file).group(1)
    id_list = id_list.replace(',', '').replace(' ', '').split('\n')
    for line_str in id_list:
        line_str = line_str.strip().split("/")[0]
        save_file_id_dict(line_str, moudle_name, file_id_dict)
    return 0

def get_file_id_enum_in_paths(h_file_paths, file_id_dict):
    for h_file in h_file_paths:
        if not h_file:
            continue
        if get_file_id_enum_in_file(h_file, file_id_dict) != 0:
            return -1
    return 0

#mkdir ${target} ${root} ${chip} ${group_name}
def generate_db_temp_dir(sys_argv):
    root = sys_argv[2]
    chip = sys_argv[3]
    group_name = sys_argv[4]
    in_path = XML_PATH
    in_path = in_path.replace('<chip>', chip)

    # get global settings
    db_conf = None
    conf = os.path.join(root, in_path)
    with open(conf, 'r') as f:
        db_conf = json.load(f)

    base_file_dir = os.path.join(os.path.join(root, db_conf["HDB_XML_TEMP_BASE_DIR"]), group_name)
    if not os.path.exists(base_file_dir):
        os.makedirs(base_file_dir)

# ----------------------------------------------------------------------
#   并行化入口（使用 multiprocessing）
# ----------------------------------------------------------------------
def get_db_conf_cached(root, chip):
    cache_key = (root, chip)
    if cache_key not in g_db_conf_cache:
        conf_path = os.path.join(root, XML_PATH.replace('<chip>', chip))
        with open(conf_path, 'r') as f:
            g_db_conf_cache[cache_key] = json.load(f)
    return g_db_conf_cache[cache_key]

def _worker_task(args):
    src_file, auto_def, module_name, file_id_dict = args
    log_info_list = parse_single_source_file(src_file, auto_def, module_name, file_id_dict)
    # 复制当前进程的结果返回给主进程
    return src_file, log_info_list

def generate_all_xml_multi_process(root, group_name, auto_def, module_name, src_files, file_id_dict):
    # 读取全局配置一次
    db_conf = get_db_conf_cached(root, chip)

    pool = Pool(min(cpu_count(), 8))
    tasks = [(src.strip(), auto_def, module_name, file_id_dict) for src in src_files if src.strip()]

    for src_file, data in pool.imap_unordered(_worker_task, tasks):
        if not data:
            continue
        _, filename = os.path.split(src_file)
        dst_xml = os.path.join(
            root,
            db_conf["HDB_XML_TEMP_BASE_DIR"],
            group_name,
            f"{filename}.xml"
        )
        os.makedirs(os.path.dirname(dst_xml), exist_ok=True)

        msg_root_element = ET.Element('MSG_LOG')
        for d in data:
            add_content_to_xml(d, msg_root_element)

        tree = ET.ElementTree(msg_root_element)
        tree.write(dst_xml)

    pool.close()
    pool.join()

def generate_all_xml(root, group_name, auto_def, module_name, src_files, file_id_dict):
    # 读取全局配置一次
    db_conf = get_db_conf_cached(root, chip)

    for src_file in src_files:
        log_info_list = parse_single_source_file(src_file, auto_def, module_name, file_id_dict)
        if not log_info_list:
            continue

        _, filename = os.path.split(src_file)
        filename = filename + ".xml"
        base_file_dir = os.path.join(os.path.join(root, db_conf["HDB_XML_TEMP_BASE_DIR"]), group_name)

        if not os.path.exists(base_file_dir):
            os.makedirs(base_file_dir)

        dst_xml_full_name = os.path.join(base_file_dir, filename)

        msg_root_element = ET.Element('MSG_LOG')
        for data in log_info_list:
            add_content_to_xml(data, msg_root_element)

        tree = ET.ElementTree(msg_root_element)
        tree.write(dst_xml_full_name)

# main
if __name__ == "__main__":
    if (sys.argv[1] == "mkdir"):
        generate_db_temp_dir(sys.argv)
        sys.exit(0)
    root = sys.argv[1]
    chip = sys.argv[2]
    group_name = sys.argv[3]
    arch_name = sys.argv[4]
    auto_def = sys.argv[5]
    if auto_def == "TRUE" or auto_def == "True":
        auto_def = True
    else:
        auto_def = False
    module_name = sys.argv[6]
    info_file = sys.argv[7]

    try:
        with open(info_file, 'r') as f:
            text = f.read()
            file_id_dir, src_files = text.split("####")
            file_id_dir = file_id_dir.split(',') if file_id_dir else []
            src_files   = src_files.split(',')

        file_id_dict = {}
        get_file_id_enum_in_paths(file_id_dir, file_id_dict)

        if (len(file_id_dict) == 0):
            sys.exit(1)

        if platform.system().lower() == "linux":
            if (len(src_files) > 2):
                generate_all_xml_multi_process(root, group_name, auto_def, module_name, src_files, file_id_dict)
            else:
                generate_all_xml(root, group_name, auto_def, module_name, src_files, file_id_dict)
        elif platform.system().lower() == "windows":
                generate_all_xml(root, group_name, auto_def, module_name, src_files, file_id_dict)
    except Exception as e:
        print(f"[WARN] open {info_file} exception: {e}")
