#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
# ============================================================================
# @brief    packet files
# ============================================================================

import os
import sys
import re
import shutil
import json
import subprocess
import struct
from xml.etree import ElementTree
from glob import glob

PY_PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.append(PY_PATH)
PKG_DIR = os.path.dirname(PY_PATH)
PKG_DIR = os.path.dirname(PKG_DIR)

from packet_create import create_sha_file
from packet_create import packet_bin

TOOLS_DIR = os.path.dirname(PKG_DIR)
SDK_DIR = os.path.dirname(TOOLS_DIR)
sys.path.append(os.path.join(SDK_DIR, "build", "script"))

from .database_script.mk_prim_xml_step1 import generate_db
from .database_script.mk_prim_xml_step2 import generate_db_xml
from .database_script.hdb_generator import generate_db_struct
from .database_script.database_create_kv import database_create_kv
from conf_parser import BuildConfParser, ParserError
from utils.build_utils import fn_str_to_int

CREATE_DATABASE_CFG_COMMAND_DICT = {}
DATABASE_MODULE_SRC_FILES = {}
ALL_DFX_INC_PATHS = []
G_UPGRADE_TEMP = "output/fhl2/upg_temp/"
G_UPGRADE_PACK = "output/package/fhl2/upgrade"


def parse_hdb_temp_file(temp_path):
    for dirname, _, files in os.walk(temp_path):
        for filename in files:
            module_name = filename.replace("_temp.txt", "")
            DATABASE_MODULE_SRC_FILES[module_name] = []
            with open(os.path.join(dirname, filename)) as f:
                value = f.read().strip()
                cfiles = value.split("####")[-1].split(",")
                for cfile in cfiles:
                    if cfile not in DATABASE_MODULE_SRC_FILES[module_name]:
                        DATABASE_MODULE_SRC_FILES[module_name].append(cfile)


def parse_compile_commands_json(json_path):
    with open(json_path, "r") as f:
        value = f.read()
        compile_commands_data = json.loads(value)
    for compile_command in compile_commands_data:
        module_name = ""
        dfx_header_paths = []
        if compile_command.get("file", ""):
            for k, v in DATABASE_MODULE_SRC_FILES.items():
                for cfile in v:
                    if cfile in compile_command.get("file"):
                        module_name = k
                        if module_name not in CREATE_DATABASE_CFG_COMMAND_DICT.keys():
                            CREATE_DATABASE_CFG_COMMAND_DICT[module_name] = {}
                            CREATE_DATABASE_CFG_COMMAND_DICT[module_name]["src"] = v
                        break
                if k == module_name:
                    break
        if module_name:
            if compile_command.get("directory", ""):
                dfx_header_path = os.path.join(compile_command.get("directory"), "dfx_include")
                if os.path.exists(dfx_header_path) :
                    if dfx_header_path not in dfx_header_paths:
                        dfx_header_paths.append("-I" + dfx_header_path)
                    if dfx_header_path not in ALL_DFX_INC_PATHS:
                        ALL_DFX_INC_PATHS.append("-I" + dfx_header_path)
            if compile_command.get("command", ""):
                compile_command = compile_command.get("command")
                ret = re.search("-o .*? -c .*", compile_command)
                compile_command = compile_command.replace(ret.group(0), "")
                compile_command_list = compile_command.split(" ")
                for i in compile_command_list:
                    i = i.strip()
                    if i == "":
                        continue
                    if i.endswith("clang-15"):
                         CREATE_DATABASE_CFG_COMMAND_DICT[module_name]["cc"] = i
                         CREATE_DATABASE_CFG_COMMAND_DICT[module_name]["inc"] = []
                         CREATE_DATABASE_CFG_COMMAND_DICT[module_name]["defs"] = []
                         CREATE_DATABASE_CFG_COMMAND_DICT[module_name]["xflags"] = []
                    elif i.startswith("-I"):
                        if i not in  CREATE_DATABASE_CFG_COMMAND_DICT[module_name]["inc"]:
                            CREATE_DATABASE_CFG_COMMAND_DICT[module_name]["inc"].append(i)
                        
                    elif i.startswith("-D"):
                        if i not in  CREATE_DATABASE_CFG_COMMAND_DICT[module_name]["defs"]:
                            CREATE_DATABASE_CFG_COMMAND_DICT[module_name]["defs"].append(i)

                    else:
                        if i not in  CREATE_DATABASE_CFG_COMMAND_DICT[module_name]["xflags"]:
                            CREATE_DATABASE_CFG_COMMAND_DICT[module_name]["xflags"].append(i)
                CREATE_DATABASE_CFG_COMMAND_DICT[module_name]["inc"].extend(dfx_header_paths)

def parse_cmake_cache(cmakecache_file_path):
    all_cflags = None
    all_defs = None
    all_inc = None
    with open(cmakecache_file_path, "r") as f:
        lines = f.readlines()
        for line in lines:
            if all([all_cflags, all_defs, all_inc]):
                break
            else:
                if all_cflags is None:
                    ret = re.search("^ALL_PUBLIC_CCFLAGS:INTERNAL=(.*)", line)
                    if ret:
                        all_cflags = []
                        all_cflags_str = ret.group(1)
                        all_cflags_temp = all_cflags_str.split(";")
                        for cflags in all_cflags_temp:
                            if cflags not in all_cflags and cflags != "" and 'SHELL' not in cflags and '-inline-threshold' not in cflags :
                                all_cflags.append(cflags)
                        continue
                if all_defs is None: 
                    ret = re.search("^ALL_PUBLIC_DEFINES:INTERNAL=(.*)", line)
                    if ret:
                        all_defs = []
                        all_defs_str = ret.group(1)
                        all_defs_temp = all_defs_str.split(";")
                        for defs in all_defs_temp:
                            if defs not in all_defs and defs != "":
                                all_defs.append(defs)
                        continue
                if all_inc is None:
                    ret = re.search("^ALL_PUBLIC_HEADER:INTERNAL=(.*)", line)
                    if ret:
                        all_inc = []
                        all_inc_str = ret.group(1)
                        all_inc_temp = all_inc_str.split(";")
                        for inc in all_inc_temp:
                            if inc not in all_inc and inc != "":
                                all_inc.append(inc)
                        continue
    return all_cflags, all_defs, all_inc

# fhl2
def create_database(pack_style_str):
    if pack_style_str.endswith("sta"):
        if pack_style_str.endswith("tsta"):
            acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-tsta")
            plc_nv_cfg = "fhl2_mcore_sg_tsta"
        else:
            acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-sta")
            plc_nv_cfg = "fhl2_mcore_sg_sta"
        database_output =  os.path.join(SDK_DIR, "output", 'database_fhl2', 'sta', 'system')
        database_cfg_path = os.path.join(SDK_DIR, "output", "fhl2", "build_hso_cfg", "hdb", "sta")
        database_cmd_xml_path = os.path.join(SDK_DIR, "drivers", "chips", "fhl2", "config", "database", "sta", "system", "hdbcfg", "mss_cmd_db.xml")
        database_temp_path = os.path.join(SDK_DIR, "drivers", "chips", "fhl2", "config", "database", "sta", "system")
        core = "sta"
    elif pack_style_str.endswith("cco"):
        acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-cco")
        database_output =  os.path.join(SDK_DIR, "output", 'database_fhl2', 'cco', 'system')
        database_cfg_path = os.path.join(SDK_DIR, "output", "fhl2", "build_hso_cfg", "hdb", "cco")
        database_cmd_xml_path = os.path.join(SDK_DIR, "drivers", "chips", "fhl2", "config", "database", "cco", "system", "hdbcfg", "mss_cmd_db.xml")
        database_temp_path = os.path.join(SDK_DIR,  "drivers", "chips", "fhl2", "config", "database", "cco", "system")
        core = "cco"
        plc_nv_cfg = "fhl2_mcore_sg_cco"
    elif pack_style_str.endswith("sta_ae"):
        acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-sta-ae")
        database_output =  os.path.join(SDK_DIR, "output", 'database_fhl2', 'sta', 'system')
        database_cfg_path = os.path.join(SDK_DIR, "output", "fhl2", "build_hso_cfg", "hdb", "sta")
        database_cmd_xml_path = os.path.join(SDK_DIR, "drivers", "chips", "fhl2", "config", "database", "sta", "system", "hdbcfg", "mss_cmd_db.xml")
        database_temp_path = os.path.join(SDK_DIR,  "drivers", "chips", "fhl2", "config", "database", "sta", "system")
        core = "sta"
        plc_nv_cfg = "fhl2_mcore_sg_sta_ae"
    elif pack_style_str.endswith("cco_ae"):
        acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-cco-ae")
        database_output =  os.path.join(SDK_DIR, "output", 'database_fhl2', 'cco', 'system')
        database_cfg_path = os.path.join(SDK_DIR, "output", "fhl2", "build_hso_cfg", "hdb", "cco")
        database_cmd_xml_path = os.path.join(SDK_DIR, "drivers", "chips", "fhl2", "config", "database", "cco", "system", "hdbcfg", "mss_cmd_db.xml")
        database_temp_path = os.path.join(SDK_DIR,  "drivers", "chips", "fhl2", "config", "database", "cco", "system")
        core = "cco"
        plc_nv_cfg = "fhl2_mcore_sg_cco_ae"
    elif pack_style_str.endswith("ndm"):
        acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-ndm")
        database_output =  os.path.join(SDK_DIR, "output", 'database_fhl2', 'ndm', 'system')
        database_cfg_path = os.path.join(SDK_DIR, "output", "fhl2", "build_hso_cfg", "hdb", "ndm")
        database_cmd_xml_path = os.path.join(SDK_DIR, "drivers", "chips", "fhl2", "config", "database", "ndm", "system", "hdbcfg", "mss_cmd_db.xml")
        database_temp_path = os.path.join(SDK_DIR,  "drivers", "chips", "fhl2", "config", "database", "ndm", "system")
        core = "ndm"
        plc_nv_cfg = "fhl2_mcore_sg_ndm"
    else:
        acore_binary_output = ""
    
    if 'csg' in pack_style_str:
        if pack_style_str.endswith("cco"):
            acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-csg-cco")
            plc_nv_cfg = "fhl2_mcore_csg_cco"
        elif pack_style_str.endswith("sta"):
            if pack_style_str.endswith("tsta"):
                acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-csg-tsta")
                plc_nv_cfg = "fhl2_mcore_csg_tsta"
            else:
                acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-csg-sta")
                plc_nv_cfg = "fhl2_mcore_csg_sta"
        elif pack_style_str.endswith("ndm"):
            acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-csg-ndm")
            plc_nv_cfg = "fhl2_mcore_csg_ndm"
        elif pack_style_str.endswith("cco_ae"):
            acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-csg-cco-ae")
            plc_nv_cfg = "fhl2_mcore_csg_cco_ae"
        elif pack_style_str.endswith("sta_ae"):
            acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-csg-sta-ae")
            plc_nv_cfg = "fhl2_mcore_csg_sta_ae"
    if 'dist' in pack_style_str:
        if pack_style_str.endswith("cco"):
            acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-dist-cco")
            plc_nv_cfg = "fhl2_mcore_sg_cco"
        elif pack_style_str.endswith("sta"):
            if pack_style_str.endswith("tsta"):
                acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-dist-tsta")
                plc_nv_cfg = "fhl2_mcore_sg_tsta"
            else:
                acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-dist-sta")
                plc_nv_cfg = "fhl2_mcore_sg_sta"
        elif pack_style_str.endswith("ndm"):
            acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-dist-ndm")
            plc_nv_cfg = "fhl2_mcore_sg_ndm"
        elif pack_style_str.endswith("cco_ae"):
            acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-dist-cco-ae")
            plc_nv_cfg = "fhl2_mcore_sg_cco_ae"
        elif pack_style_str.endswith("sta_ae"):
            acore_binary_output = os.path.join(SDK_DIR, "output", "fhl2", "acore", "application-fhl2-dist-sta-ae")
            plc_nv_cfg = "fhl2_mcore_sg_sta_ae"

    if acore_binary_output:
        temp_path = os.path.join(acore_binary_output, "hso_temp")
        parse_hdb_temp_file(temp_path)
        json_path = os.path.join(acore_binary_output, "compile_commands.json")
        parse_compile_commands_json(json_path)
        in_path = os.path.join(SDK_DIR, "build", "config", "target_config", "fhl2", "hdb_config", "database_cfg", "mk_hdb_xml.json")
        cc = None
        for k, v in CREATE_DATABASE_CFG_COMMAND_DICT.items():
            if cc is None:
                cc = v.get("cc")
            generate_db(k, v.get("cc"), v.get("src"), v.get("inc"), v.get("defs"), v.get("xflags"), SDK_DIR, in_path)
        generate_db_xml(SDK_DIR, in_path, pack_style_str)
        cmakecache_file_path = os.path.join(acore_binary_output, "CMakeCache.txt")
        mconfig_file_path = os.path.join(acore_binary_output, "mconfig.h")
        all_cflags, all_defs, all_inc = parse_cmake_cache(cmakecache_file_path)
        all_cflags.append("-include" + mconfig_file_path)
        generate_db_struct(cc, all_defs, all_cflags, all_inc, SDK_DIR, in_path, pack_style_str, ALL_DFX_INC_PATHS)

        if os.path.exists(database_output):
            shutil.rmtree(database_output)
        shutil.copytree(database_temp_path, database_output)
        shutil.copytree(database_cfg_path, database_output, dirs_exist_ok=True)
        database_cmd_xml_output_path = os.path.join(database_output, "hdbcfg", "mss_cmd_db.xml")
        shutil.copyfile(database_cmd_xml_path, database_cmd_xml_output_path)
        # kv database
        database_nv_output_path = os.path.join(SDK_DIR, "output", "database_fhl2")
        default_cfg = os.path.join(SDK_DIR, "middleware", "chips", "fhl2", "nv", "nv_config", "mcore_default.json")
        extern_cfg = glob(os.path.join(SDK_DIR, "middleware", "chips", "fhl2", "nv", "nv_config", plc_nv_cfg, "cfg", "acore", "*.json"))
        etypes_path = os.path.join(acore_binary_output, "nv_config", "acore.etypes")
        database_create_kv(database_nv_output_path, default_cfg, extern_cfg, etypes_path, core)

def add_efuse_info(pack_style_str):
    efuse_cfg = os.path.join(SDK_DIR, "output", "package", "fhl2", pack_style_str, "EFUSE.bin")
    efuse_cfg_gen = os.path.join(SDK_DIR, "tools", "bin", "sign_tool", "efuse_tool", "efuse_cfg_gen.py")
    efuse_oem = os.path.join(SDK_DIR, "tools", "bin", "sign_tool", "efuse_tool", "efuse_oem.csv")
    if os.system(sys.executable + " " + efuse_cfg_gen + " " + efuse_oem + " " + efuse_cfg) != 0:
        sys.exit(-5)

def make_upg(**upg_params):
    upg_cmd = ['python3', 'build/config/target_config/fhl2/make_upg_file.py', 
               '-app_name', upg_params.get("app_name"), 
               '-base', upg_params.get("base"),
               '-temp_dir', G_UPGRADE_TEMP, 
               '-input', upg_params.get("input"), 
               '-output', "{}/{}".format(os.getcwd(), G_UPGRADE_PACK),
               '-type', '0', 
               '-product_type', upg_params.get("product_type"), 
               '-param_ver', upg_params.get("param_ver"), 
               '-new_product_type', upg_params.get("new_product_type")]

    print("upg", upg_cmd)
    subprocess.run(upg_cmd, shell=False, check=True)

def get_param_ver(param_name):
    param_sector_dir = "build/config/target_config/fhl2/param_sector"
    param_sector_file = os.path.join(param_sector_dir, "param_sector_{}.json".format(param_name))
    partition_cfg = BuildConfParser(param_sector_file).get_conf_data().get("Partition_Tbl")
    if partition_cfg is None:
        msg = "[error] flash partiton config NULL:[%s]!" % param_sector_file
        raise ParserError(msg)
    version = partition_cfg.get("version")
    if version is None:
        msg = "[error] flash partiton config err:[%s]!" % param_sector_file
        raise ParserError(msg)
    return fn_str_to_int(version)

def get_nv_info(image_bin):
    length = image_bin[2:4]
    key_id = image_bin[6:8]
    str_length = "%d" % struct.unpack("H", length)
    int_length = int(str_length)
    int_key_id = int("%d" % struct.unpack("H", key_id))
    return int_length, int_key_id

def get_product_type(nv_bin):
    string_out = b''
    with open(nv_bin, 'rb') as fp:
        data = fp.read().hex()
        list_page = [data[i:i + 4096 * 2] for i in range(0, len(data), 4096 * 2)]
        for page in list_page:
            byte = "{}{}".format(page[32:][6:8], page[32:][4:6])
            if byte == "ffff":
                continue
            else:
                string_out += bytes.fromhex(page)
    tmp_len = 0
    while 1:
        # 跳过kv_page_header
        if tmp_len == 0 or tmp_len == 4096:
            tmp_len = tmp_len + 16
        length = 0
        int_key_id = 0
        # 获取kv_key_header
        length, int_key_id = get_nv_info(string_out[tmp_len:tmp_len + 16])
        if int_key_id == 0xF03:
            tmp_len = tmp_len + 16
            ftm_product_id_bin = string_out[tmp_len:tmp_len + length]
            product_type_bin = ftm_product_id_bin[8:12]
            product_type = int("%d" % struct.unpack("I", product_type_bin))
            return str(product_type)
        
        if tmp_len <= 4096 and (tmp_len + length + 4 + 16) > 4096:
            tmp_len = 4096
            continue
        tmp_len = tmp_len + length + 4 + 16
        if tmp_len >= 4096 * 2:
            break
    return ''

def get_new_product_type(param_name, product_type):
    for product in ['cco', 'ndm', 'cco_ae', 'dtt']:
        if product in param_name:
            return product_type
    return "500"


def get_param_info(param_info, bin_name):
    param_map = {
        "app" : "0x10",
        "flashboot_a" : "0x00",
        "flashboot_b" : "0x01",
        "nv" : "0x21",
        "rf" : "0x26",
    }
    if param_map.get(bin_name):
        for param_list in param_info:
            if param_map.get(bin_name) == param_list[0]:
                return hex(int(param_list[1], 16) + 0x400000), param_list[2]

def make_all_in_one_packet(pack_style_str, build_extr_defines):
    # make all in one packet
    create_database(pack_style_str)
    add_efuse_info(pack_style_str)
    if pack_style_str.endswith("sta") or pack_style_str.endswith("sta_fpga"):
        if pack_style_str.endswith("tsta") or pack_style_str.endswith("tsta_fpga"):
            if pack_style_str.endswith("csg_tsta"):
                fwpkg_name = "FengHuoLunV200_CSG_TSTA.fwpkg"
                param_name = "csg_tsta"
                app_name="FengHuoLunV200_CSG_TSTA"
            else:
                fwpkg_name = "FengHuoLunV200_TSTA.fwpkg"
                param_name = "tsta"
                app_name="FengHuoLunV200_TSTA"
        else:
            if pack_style_str.endswith("csg_sta"):
                fwpkg_name = "FengHuoLunV200_CSG_STA.fwpkg"
                param_name = "csg_sta"
                app_name="FengHuoLunV200_CSG_STA"
            else:
                fwpkg_name = "FengHuoLunV200_STA.fwpkg"
                param_name = "sta"
                app_name="FengHuoLunV200_STA"
    elif pack_style_str.endswith("sta_ae"):
        if pack_style_str.endswith("csg_sta_ae"):
            fwpkg_name = "FengHuoLunV200_CSG_STA_AE.fwpkg"
            param_name = "csg_sta_ae"
            app_name="FengHuoLunV200_CSG_STA_AE"
        else:
            fwpkg_name = "FengHuoLunV200_STA_AE.fwpkg"
            param_name = "sta_ae"
            app_name="FengHuoLunV200_STA_AE"
    elif pack_style_str.endswith("cco") or pack_style_str.endswith("cco_fpga"):
        if pack_style_str.endswith("csg_cco"):
            fwpkg_name = "FengHuoLunV200_CSG_CCO.fwpkg"
            param_name = "csg_cco"
            app_name="FengHuoLunV200_CSG_CCO"
        else:
            fwpkg_name = "FengHuoLunV200_CCO.fwpkg"
            param_name = "cco"
            app_name="FengHuoLunV200_CCO"
    elif pack_style_str.endswith("cco_ae"):
        if pack_style_str.endswith("csg_cco_ae"):
            fwpkg_name = "FengHuoLunV200_CSG_CCO_AE.fwpkg"
            param_name = "csg_cco_ae"
            app_name="FengHuoLunV200_CSG_CCO_AE"
        else:
            fwpkg_name = "FengHuoLunV200_CCO_AE.fwpkg"
            param_name = "cco_ae"
            app_name="FengHuoLunV200_CCO_AE"
    elif pack_style_str.endswith("ndm") or pack_style_str.endswith("ndm_fpga"):
        if pack_style_str.endswith("csg_ndm"):
            fwpkg_name = "FengHuoLunV200_CSG_NDM.fwpkg"
            param_name = "csg_ndm"
            app_name="FengHuoLunV200_CSG_NDM"
        else:
            fwpkg_name = "FengHuoLunV200_NDM.fwpkg"
            param_name = "ndm"
            app_name="FengHuoLunV200_NDM"
    else:
        if pack_style_str.endswith("test") or pack_style_str.endswith("test_fpga"):
            if "CONFIG_TESTSUITE_STA" in build_extr_defines:
                fwpkg_name = "FengHuoLunV200_TESTSUITE_STA.fwpkg"
                param_name = "sta"
            elif "CONFIG_TESTSUITE_TSTA" in build_extr_defines:
                fwpkg_name = "FengHuoLunV200_TESTSUITE_TSTA.fwpkg"
                param_name = "tsta"
            else:
                fwpkg_name = "FengHuoLunV200_TESTSUITE_CCO.fwpkg"
                param_name = "cco"
        else:
            fwpkg_name = "FengHuoLunV200.fwpkg"
            param_name = "default"
    if os.system("sh " + os.path.join(
        SDK_DIR, "build", "config", "target_config", "fhl2", "sign_config", "params_and_bin_sign.sh") + " " + pack_style_str + " " + param_name) != 0:
        sys.exit(-2)
    if pack_style_str.startswith("pack"):
        bin_dir = os.path.join(SDK_DIR, "output", "package", "fhl2", pack_style_str)
        application_dir = bin_dir
        protocol_dir = bin_dir
        ate_dir = bin_dir
        partition_dir = bin_dir
        nv_dir =  bin_dir
        efuse_dir = bin_dir
        fhl2_fwpkg = os.path.join(bin_dir, fwpkg_name)
    else:
        bin_dir = os.path.join(SDK_DIR, "interim_binary", "fhl2", "bin", "boot_bin")
        ate_dir = os.path.join(SDK_DIR, "output", "fhl2", "acore", pack_style_str)
        partition_dir = os.path.join(SDK_DIR, "interim_binary", "fhl2", "bin", "partition", pack_style_str)
        nv_dir =  os.path.join(SDK_DIR, "interim_binary", "fhl2", "bin", "nv", pack_style_str)
        application_dir = os.path.join(SDK_DIR, "output", "fhl2", "acore", pack_style_str)
        protocol_dir = os.path.join(SDK_DIR, "output", "fhl2", "acore", pack_style_str)
        efuse_dir = os.path.join(SDK_DIR, "output", "fhl2", "acore", pack_style_str)
        fhl2_fwpkg = os.path.join(SDK_DIR, "output", "fhl2", "fwpkg", pack_style_str, fwpkg_name)

    partition_a = os.path.join(partition_dir, "partition_sign_a.bin")
    partition_b = os.path.join(partition_dir, "partition_sign_b.bin")
    loadboot = os.path.join(bin_dir, "loaderboot_sign.bin")
    flashboot = os.path.join(bin_dir, "flashboot_sign_a.bin")
    flashboot_bak = os.path.join(bin_dir, "flashboot_sign_b.bin")
    nv = os.path.join(nv_dir, "fhl2_all_nv.bin")
    app = os.path.join(application_dir, "application_sign.bin")
    efuse = os.path.join(efuse_dir, "EFUSE.bin")
    rf = ""
    param_json_path = os.path.join(SDK_DIR, "build", "config", "target_config", "fhl2", "param_sector", "param_sector_%s.json" % param_name)

    with open(param_json_path, "r") as f:
        value = f.read()
        param_data = json.loads(value)
    param_info = param_data["Partition_Tbl"]["param_info"]

    loadboot_bx = loadboot + "|0x0|0x0|0"
    partition_a_bx = partition_a + "|0x400000|0x1000|1"
    partition_b_bx = partition_b + "|0x401000|0x1000|1"
    flashboot_bx = flashboot + "|%s|%s|8" % get_param_info(param_info, "flashboot_a")
    flashboot_bak_bx = flashboot_bak + "|%s|%s|8" % get_param_info(param_info, "flashboot_b")
    rf_bx = rf + "|%s|%s|13" % get_param_info(param_info, "rf")
    app_bx = app + "|%s|%s|12" % get_param_info(param_info, "app")
    efuse_bx = efuse + "|0x0|0x0|4"
    nv_bx =  nv + "|%s|%s|2" % get_param_info(param_info, "nv")

    try:
        packet_post_agvs = list()
        packet_post_agvs.append(loadboot_bx)
        packet_post_agvs.append(partition_a_bx)
        packet_post_agvs.append(partition_b_bx)
        packet_post_agvs.append(flashboot_bx)
        packet_post_agvs.append(flashboot_bak_bx)
        packet_post_agvs.append(app_bx)
        packet_post_agvs.append(nv_bx)
        if "EFUSE" in build_extr_defines:
            packet_post_agvs.append(efuse_bx)
        packet_bin(fhl2_fwpkg, packet_post_agvs)

        if not pack_style_str.startswith("ate") and not pack_style_str.startswith("pack"):
            packet_post_agvs = list()
            packet_post_agvs.append(loadboot_bx)
            packet_post_agvs.append(partition_bx)
            packet_post_agvs.append(flashboot_bx)
            packet_post_agvs.append(flashboot_bak_bx)
            packet_post_agvs.append(app_bx)
            packet_post_agvs.append(nv_bx)
            if "EFUSE" in build_extr_defines:
                packet_post_agvs.append(efuse_bx)
            packet_bin(fhl2_ide_fwpkg, packet_post_agvs)

    except Exception as e:
        print(e)
        exit(-1)

    # 先进行flashboot的升级包制作
    upg_params = {
        "app_name": "FengHuoLunV200_flashboot",
        "input": "{}:flashboot".format(flashboot),
        "param_ver": "65536",
        "product_type": '0',
        "new_product_type": '0',
        "base": ''
    }
    fota_cfg = "build/config/target_config/fhl2/fota/"
    if 'csg' in param_name:
        fota_cfg = "{}fota_csg.cfg".format(fota_cfg)
    elif 'dist' in param_name:
        fota_cfg = "{}fota_dist.cfg".format(fota_cfg)
    else:
        fota_cfg = "{}fota.cfg".format(fota_cfg)
    upg_params["base"] = fota_cfg
    if not os.path.exists(G_UPGRADE_TEMP) or not os.path.exists(G_UPGRADE_PACK):
        shutil.rmtree(G_UPGRADE_TEMP, ignore_errors=True)
        os.makedirs(G_UPGRADE_TEMP)
        shutil.rmtree(G_UPGRADE_PACK, ignore_errors=True)
        os.makedirs(G_UPGRADE_PACK)
    make_upg(**upg_params)
    
    # application的升级包制作，test不做升级包
    if param_name != "default" and not pack_style_str.endswith("test") and not pack_style_str.endswith("test_fpga"):
        input = "{}:application|{}:nv".format(app, nv)
        param_ver = str(get_param_ver(param_name))
        product_type = get_product_type(nv)
        if not product_type:
            print("error: product_type cannot be found in nv_bin!")
            exit(-1)
        new_product_type = get_new_product_type(param_name, product_type)
        upg_params.update(app_name=app_name, input=input, param_ver=param_ver, product_type=product_type,
            new_product_type=new_product_type)
        make_upg(**upg_params)

def is_packing_files_exist(soc, pack_style_str):
    """
    判断打包文件是否存在
    :return:
    """
    packing_files = get_packing_files(soc, pack_style_str)
    lost_files = list()
    for f_path in packing_files:
        if not os.path.isfile(f_path):
            lost_files.append(f_path)
    return lost_files

def get_packing_files(soc, pack_style_str):
    """
    直接添加需要打包的文件路径
    :return:
    """
    packing_files = list()            
    return packing_files
