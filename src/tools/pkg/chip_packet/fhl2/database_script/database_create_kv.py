#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.

# ===============================================================================
# Description: Create KV Database.
# ===============================================================================
import sys
import os
import json
import xml.etree.ElementTree as ET
import ctypes
import pycparser

from .parse_msgdefs import Visitor
from .hdb_utils import file_open

binary_operators = {
    "&": lambda x, y: x & y,
    "|": lambda x, y: x | y,
    "*": lambda x, y: x * y,
    "+": lambda x, y: x + y,
    "-": lambda x, y: x - y,
    "/": lambda x, y: x // y,
    "<<": lambda x, y: x << y,
    ">>": lambda x, y: x >> y,
    "<=": lambda x, y: int(x <= y),
    ">=": lambda x, y: int(x >= y),
    "==": lambda x, y: int(x == y),
    "&&": lambda x, y: int(bool(x and y)),
    "||": lambda x, y: int(bool(x or y)),
}

unary_operators = {
    "-": lambda x: -x,
    "!": lambda x: 1 if x else 0,
    "sizeof": lambda x: ctypes.sizeof(x),
}

fundamental_types = {
    "": ctypes.c_int,
    "_Bool": ctypes.c_bool,
    "char unsigned": ctypes.c_ubyte,
    "char": ctypes.c_byte,
    "double": ctypes.c_double,
    "double long": ctypes.c_longdouble,
    "float": ctypes.c_float,
    "long long unsigned": ctypes.c_ulonglong,
    "long long": ctypes.c_longlong,
    "long unsigned": ctypes.c_ulong,
    "long": ctypes.c_long,
    "short unsigned": ctypes.c_ushort,
    "short": ctypes.c_short,
    "unsigned": ctypes.c_uint,
    "void": None,
}


def get_typename(names):
    specifiers = []
    for name in sorted(names):
        if name in ["signed", "int"]:
            continue
        specifiers.append(name)
    return " ".join(specifiers)


def create_enum_type(enumname, enums):
    smallest = min(enums.values())
    largest = max(enums.values())
    if smallest < 0:
        if -128 <= smallest and largest <= 127:
            base = ctypes.c_byte
        elif -32768 <= smallest and largest <= 32767:
            base = ctypes.c_short
        else:
            base = ctypes.c_int
    else:
        if largest < 256:
            base = ctypes.c_ubyte
        elif largest < 65536:
            base = ctypes.c_ushort
        else:
            base = ctypes.c_uint
    return type(enumname, (base,), {"members": enums})


def get_core_by_name(name):
    (filename, extension) = os.path.splitext(name)
    if filename == 'application_core':
        return 'Application'
    elif filename == 'protocol_core':
        return 'Protocol'
    elif filename == 'security_core':
        return 'Security'
    return ''



def get_file_name_by_name(name):
    (filename, extension) = os.path.splitext(name)
    if filename == 'application_core':
        return 'nv_app_struct_def.txt'
    elif filename == 'protocol_core':
        return 'nv_protocol_struct_def.txt'
    elif filename == 'security_core':
        return 'nv_security_struct_def.txt'


def int_prim_xml_layer_tree(tree, core):
    root = tree.getroot()
    layer_root_element = ET.Element('')
    for child_1 in root:
        if child_1.tag == 'GROUP' and child_1.attrib['NAME'] == core:
            layer_root_element = child_1
            break

    if layer_root_element.tag == 'GROUP':
        temp_attrib = layer_root_element.attrib
        temp_text = layer_root_element.text
        temp_tail = layer_root_element.tail
        layer_root_element.clear()
        layer_root_element.attrib = temp_attrib
        layer_root_element.text = temp_text
        layer_root_element.tail = temp_tail
    return layer_root_element


def add_element_to_xml(root_element):
    global G_ID_TO_NAME
    global G_ID_TO_STRUCTURE
    sorted_list = sorted(G_ID_TO_NAME.items(), key=lambda e: e[1])

    for key, value in sorted_list:
        new_element = ET.Element('')
        new_element.tag = 'NV'
        new_element.attrib['CATEGORY'] = 'FTM'
        new_element.attrib['DESCRIPTION'] = 'FTM'
        new_element.attrib['ID'] = str(key)
        new_element.attrib['NAME'] = value
        new_element.attrib['PARAM_NAME'] = G_ID_TO_STRUCTURE.get(key)
        new_element.attrib['PARAM_VALUE'] = ''
        new_element.tail = '\n\t'
        root_element.append(new_element)


def write_prim_xml_tree(tree, output_path):
    tree.write(output_path, encoding="UTF-8", xml_declaration='<?xml version="1.0" encoding="utf-8" ?>', method="xml")


G_ID_TO_NAME = {}
G_ID_TO_STRUCTURE = {}

G_ID_TO_NAME = {}
G_ID_TO_STRUCTURE = {}


def database_create_kv(output_path, mcore_cfg, extern_cfg, etype_path, core):
    target_xml = os.path.join(output_path, core, 'system', 'hdbcfg', 'mss_nvi_db.xml')
    etype_path_list = etype_path.split("|")
    key_definitions_list = []
    key_values_list = []
    etype_data = ''
    target_txt = os.path.join(output_path, core, 'system', 'nv', 'nv_app_struct_def.txt')
    extern_txt = os.path.join(output_path, core, 'system', 'nv', 'extern_datatype_def.txt')

    with file_open(target_txt, 'w') as target:
        target.write('#include \"base_datatype_def.txt\"\r\n')
        target.write('#include \"extern_datatype_def.txt\"\r\n')
    
    with file_open(extern_txt, 'w') as target:
        target.seek(0, 0)
        target.truncate()

    for etype_path in etype_path_list:
        with open(etype_path, 'r') as etype:
            with file_open(target_txt, 'a') as target:
                for line in etype.readlines():
                    if line[0] != '#':
                        target.write(line)
            etype.close()
        
        code = pycparser.parse_file(etype_path)
        v = Visitor()
        v.visit(code)
        
        with file_open(extern_txt, 'a') as target:
            for structname, fields in sorted(v.typedefs.items()):
                if fields is not None and hasattr(fields, "members"):
                    if ctypes.sizeof(fields) == 1:
                        line = ''.join([structname, ' 8 0\r\n'])
                    elif ctypes.sizeof(fields) == 2:
                        line = ''.join([structname, ' 16 0\r\n'])
                    elif ctypes.sizeof(fields) == 4:
                        line = ''.join([structname, ' 32 0\r\n'])
                    else:
                        line = ''.join([structname, ' 8 0\r\n'])
                    target.write(line)

    if os.path.exists(mcore_cfg):
        extern_cfg.append(mcore_cfg)
    for extern_cfg_json in extern_cfg:
        with open(extern_cfg_json, 'r+') as stream:
            test = json.load(stream)
            for i in test:
                for j in test[i]:
                    if isinstance(test[i][j], dict):
                        if '0x' in str(test[i][j]['key_id']):
                            G_ID_TO_NAME[int(test[i][j]['key_id'], 16)] = j
                            G_ID_TO_STRUCTURE[int(test[i][j]['key_id'], 16)] = test[i][j]['structure_type']
                        else:
                            G_ID_TO_NAME[int(test[i][j]['key_id'])] = j
                            G_ID_TO_STRUCTURE[int(test[i][j]['key_id'])] = test[i][j]['structure_type']

    target_txt = 'nv_app_struct_def.txt'
    target_txt = os.path.join(output_path, core, 'system', 'nv', os.path.basename(target_txt))
    with file_open(target_txt, 'a') as target:
        sorted_list = sorted(G_ID_TO_NAME.items(), key=lambda e: e[1])
        for key, value in sorted_list:
            target.write(''.join(
                ["typedef struct { \r\n    ", G_ID_TO_STRUCTURE.get(key), " body;\r\n} ", G_ID_TO_NAME.get(key),
                    "_nv;\r\n"]))
            G_ID_TO_STRUCTURE[key] = ''.join([G_ID_TO_NAME.get(key), "_nv"])
    tree = ET.parse(target_xml)
    root = tree.getroot()
    add_element_to_xml(int_prim_xml_layer_tree(tree, 'Application'))
    write_prim_xml_tree(tree, target_xml)
    G_ID_TO_NAME.clear()
    G_ID_TO_STRUCTURE.clear()
    key_definitions_list.clear()
