#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2020. All rights reserved.

import os
import re
import json
from enum import Enum

project_path = os.path.abspath(os.path.join(os.getcwd(), "../../../"))
build_path = os.path.abspath(os.path.join(project_path, "tools"))
CONFIG_PATH = "check_size_config.json"
STATIC_START = "Linker script and memory map"
PATTEN_LIST = [
    r"(\.| )*[a-z\. _]+0x[0-9a-f]+ +0x[0-9a-f]+ +.*",
    r"^\.debug.*"
]
BLACKLIST = [".debug", ".head_ce", ".stack_ce", ".int_stack"]


class MapInfoType(Enum):
    SINGLE_SYMB_TRANK_INFO = 3
    SINGLE_SYMB_INFO = 4
    TOP_SEG_TRANK_INFO = 5
    TOP_SEG_INFO = 6


def get_map_real_info(info_list, map_info_type):
    size = 0
    lib = None
    src = None
    keys = None
    file_info = None
    top_flag = False
    if map_info_type == MapInfoType.SINGLE_SYMB_TRANK_INFO:
        file_info = info_list[2]
        size = info_list[1]
    elif map_info_type == MapInfoType.SINGLE_SYMB_INFO:
        file_info = info_list[3]
        size = info_list[2]
        keys = info_list[0]
    elif map_info_type == MapInfoType.TOP_SEG_TRANK_INFO:
        size = info_list[1]
        top_flag = True
    elif map_info_type == MapInfoType.TOP_SEG_INFO:
        keys = info_list[0]
        size = info_list[2]
        top_flag = True
    else:
        pass
    if file_info is not None:
        lib_idx_start = file_info.rfind("/")
        lib_idx_end = file_info.rfind("(")
        lib = file_info[lib_idx_start+1:lib_idx_end]
        src_idx_start = file_info.rfind("(")
        src_idx_end = file_info.rfind(")")
        src = file_info[src_idx_start+1:src_idx_end]
    return keys, size, lib, src, top_flag


def fill_symb(info, content):
    res = dict()
    key_count = dict()
    for k, v in info.items():
        if str(k).isdigit():
            keys = content[int(k) - 1].strip()
        else:
            keys = k
        if keys in res:
            if keys in key_count:
                key_count[keys] += 1
                new_key = f"{keys}#{key_count[keys]}"
            else:
                key_count[keys] = 1
                new_key = f"{keys}#1"
            res[new_key] = v
        else:
            res[keys] = v
            key_count[keys] = 0
    return res


def build_symb_layout(info, json_file):
    os.path.abspath(os.path.join(build_path, "scripts/{}".format(json_file)))
    config = None
    whitelist = [".init_call", ".sysinitcall.init", ".debug"]
    with open(json_file, "r") as jf:
        config = json.load(jf)
    if config is not None:
        for k, v in info.items():
            if v["istop"] and k.find(".text") == 0:
                for layout, layout_info in config["layout"].items():
                    for mod, _ in layout_info.items():
                        if k.find(mod) != -1:
                            info[k]["layout"] = layout
                            break
            elif v["istop"] and k.find("_ce") != -1:
                info[k]["layout"] = "sram_ce"
            elif v["istop"] and k.find("_po") != -1:
                info[k]["layout"] = "sram_po"
            else:
                for layout, layout_info in config["layout"].items():
                    for _, mod_info in layout_info.items():
                        if info[k]["lib"] in mod_info:
                            info[k]["layout"] = layout
                            break
                if "layout" not in info[k]:
                    info[k]["layout"] = None
            # In particular
            if k in whitelist:
                info[k]["layout"] = "sram_ce"
    return info


def get_map_info(config_path):
    with open(config_path, 'r') as file:
        data = json.load(file)
    map_filepath = os.path.abspath(os.path.join(project_path, data["map_path"].format(data["map_filename"])))
    map_info = dict()
    static_start = False
    with open(map_filepath, "r") as mf:
        lines = mf.readlines()
        for i in range(0, len(lines)):
            if (not static_start) and (lines[i].find(STATIC_START) == -1):
                continue
            else:
                static_start = True
            r = re.search(PATTEN_LIST[0], lines[i])
            if r is not None:
                line_info = lines[i].split()
                if line_info[0] == ".comment":
                    continue
                k, sz, l, sc, tf = get_map_real_info(line_info, MapInfoType(len(line_info)))
                if k is None:
                    k = i
                if k in map_info:
                    k = "{}#".format(k)
                    k_find = False
                    name = None
                    for kk, _ in map_info.items():
                        if str(kk).find(k) != -1:
                            k_find = True
                            idx = int(kk.split("#")[1]) + 1
                            name = kk.split("#")[0]
                            name = "{}#{}".format(name, idx)
                    if not k_find:
                        name = "{}0".format(k)
                        map_info[name] = dict()
                        map_info[name]["size"] = sz
                        map_info[name]["lib"] = l
                        map_info[name]["src"] = sc
                        map_info[name]["istop"] = tf
                    else:
                        map_info[name] = dict()
                        map_info[name]["size"] = sz
                        map_info[name]["lib"] = l
                        map_info[name]["src"] = sc
                        map_info[name]["istop"] = tf
                else:
                    map_info[k] = dict()
                    map_info[k]["size"] = sz
                    map_info[k]["lib"] = l
                    map_info[k]["src"] = sc
                    map_info[k]["istop"] = tf
            else:
                if lines[i].find("*fill*") != -1:
                    line_info = lines[i].split()
                    k = line_info[0]
                    size = line_info[2]
                    if k not in map_info:
                        map_info[k] = dict()
                        map_info[k]["size"] = size
                        map_info[k]["lib"] = None
                        map_info[k]["src"] = None
                        map_info[k]["istop"] = False
                        map_info[k]["layout"] = None
                    else:
                        map_info[k]["size"] = hex(int(size, 16) + int(map_info[k]["size"], 16))
        map_info = fill_symb(map_info, lines)
    return map_info, data


def static_layout_size(info, layout):
    print("######### layout size info #########")
    top_size = 0
    notop_size = 0
    for k, v in info.items():
        if v["layout"] == layout:
            if (k in BLACKLIST) or (re.match(PATTEN_LIST[1], k) is not None):
                continue
            else:
                if v["istop"]:
                    top_size += int(v["size"], 16)
                else:
                    notop_size += int(v["size"], 16)
    print("{} total top size is: {}({}KB)".format(layout, hex(top_size), top_size/1024))
    print("{} total notop size is: {}({}KB)".format(layout, hex(notop_size), notop_size/1024))


def static_top_size(info, top_name_list):
    print("######### top size info #########")
    top_valid = False
    top_change = False
    pre_top = None
    cur_top = None
    subsize = 0
    topsize = 0
    pre_topsize = 0
    pre_subsize = 0
    for k, v in info.items():
        if (cur_top is not None) and (v["istop"]):
            top_change = True
        else:
            top_change = False
        if k in top_name_list:
            top_valid = True
            if top_change:
                pre_top = cur_top
                pre_topsize = topsize
                pre_subsize = subsize
            cur_top = k
            subsize = 0
            topsize = int(v["size"], 16)
        elif v["istop"]:
            top_valid = False
        if (top_valid) and (top_change):
            print("{} topsize: {}({}KB)".format(pre_top, hex(pre_topsize), pre_topsize/1024))
            print("{} subsize: {}({}KB)".format(pre_top, hex(pre_subsize), pre_subsize/1024))
        if (top_valid) and (not v["istop"]):
            subsize += int(v["size"], 16)
    print("{} topsize: {}({}KB)".format(top_name_list[-1], hex(topsize), topsize/1024))
    print("{} subsize: {}({}KB)".format(top_name_list[-1], hex(subsize), subsize/1024))


def static_lib_size(info, lib_list):
    print("######### lib size info #########")
    for lib in lib_list:
        print("====== {} size info ======".format(lib))
        size = 0
        for _, v in info.items():
            if v["lib"] == lib:
                size += int(v["size"], 16)
        print("{} total size: {}({}KB)".format(lib, hex(size), size/1024))


def static_src_size(info):
    print("######### src size info #########")
    res = dict()
    for _, v in info.items():
        if v["src"] is None:
            continue
        if v["src"] not in res:
            res[v["src"]] = int(v["size"], 16)
        else:
            res[v["src"]] += int(v["size"], 16)
    for k, v in res.items():
        print("{} total size: {}({}KB)".format(k, hex(v), v/1024))


def find_lib(info):
    libs_list = []
    for _, v in info.items():
        lib = v["lib"]
        if isinstance(lib, str) and (lib.endswith('.a') or lib.endswith('.')):
            libs_list.append(lib)
    a_files = [lib for lib in libs_list]
    unique_a_files = list(set(a_files))
    return unique_a_files


def static_fill_size(info):
    print("######### fill size info #########")
    for k, v in info.items():
        if k == "*fill*":
            print("fill size is: {}".format(v["size"]))
            break


def debug_size_info(info):
    print("========= TOP START ========")
    for k, v in info.items():
        if v["istop"]:
            print("{}\n\t{}\n\t{}\n\t{}\n\t{}\n\t{}".format(k, v["size"], v["lib"], v["src"], v["istop"], v["layout"]))
    print("======== TOP END =========")
    print("========= NOTOP START ========")
    for k, v in info.items():
        if not v["istop"]:
            print("{}\n\t{}\n\t{}\n\t{}\n\t{}\n\t{}".format(k, v["size"], v["lib"], v["src"], v["istop"], v["layout"]))
    print("======== NOTOP END =========")


def total_mem_segment(info):
    print("==================== Summary ====================")
    keywords = [".text", ".rodata", ".data", "bss"]
    memory_sizes = []
    total_memory = 0
    for keys in keywords:
        total_size = sum(int(v["size"], 16) for k, v in info.items() if keys in k)
        memory_sizes.append(total_size)
        total_memory += total_size
    memory_sizes.append(total_memory)
    print(f"{keywords[0]:>10} {keywords[1]:>10} {keywords[2]:>10} {keywords[3]:>10} {'Total':>10}")
    mem_hex = [hex(size) for size in memory_sizes]
    print(f"{mem_hex[0]:>10} {mem_hex[1]:>10} {mem_hex[2]:>10} {mem_hex[3]:>10} {mem_hex[4]:>10}")

def total_lib_mem_segment(info, lib_list):
    print("==================== Sub Summary ====================")
    title = ["lib/field", ".text", ".rodata", ".data", ".bss", "total"]
    print(f"{title[0]:<15} | {title[1]:>7} {title[2]:>7} {title[3]:>7} {title[4]:>7} {title[5]:>7}")
    print("-" * 60)
    keywords = [".text", ".rodata", ".data", "bss"]
    lib_memory_info = []

    for lib in lib_list:
        memory_sizes = []
        totals = 0
        for keys in keywords:
            total_size = sum(int(v["size"], 16) for k, v in info.items() if keys in k and v["lib"] == lib)
            memory_sizes.append(total_size)
        if all(value == 0 for value in memory_sizes):
            continue
        totals = sum(size for size in memory_sizes)
        memory_sizes.append(totals)
        mem_hex = [hex(size) for size in memory_sizes]
        if lib[-1] == '.':
            lib = f"{lib}a"
        if len(lib) > 15:
            lib = f"{lib[:11]}*.a"
        lib_memory_info.append((lib, mem_hex))

    lib_memory_info.sort(key=lambda x: -int(x[1][4], 16))

    total_liteos = 0
    total_dict = {}

    for lib, mh in lib_memory_info:
        print(f"{lib:<15} | {mh[0]:>7} {mh[1]:>7} {mh[2]:>7} {mh[3]:>7} {mh[4]:>7}")
        mem_hex = [int(size, 16) for size in mh]

        if lib in compare_data["liteos"]:
            total_liteos += mem_hex[4]

        if lib in compare_data and mem_hex[4] > compare_data[lib]:
            total_dict[lib] = [mem_hex[4], compare_data[lib]]

    if total_liteos > compare_data["liteos.a"]:
        total_dict["liteos.a"] = [total_liteos, compare_data["liteos.a"]]

    if total_dict:
        print("WARNING: The compiled file is out of the target range!")
        for keys, values in total_dict.items():
            print(f"{keys:<15} | real values:{values[0]} | original values:{values[1]}")

if __name__ == "__main__":
    m_info, compare_data = get_map_info(CONFIG_PATH)
    total_mem_segment(m_info)
    lib_all = find_lib(m_info)
    total_lib_mem_segment(m_info, lib_all)
