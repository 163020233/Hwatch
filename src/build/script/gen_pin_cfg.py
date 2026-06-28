#!/usr/bin/env python3
# coding:utf-8
# Copyright Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.

import os
import sys
import zlib
import json
import struct
import re
import shutil
import openpyxl
import pandas as pd
from ctypes import *

td_char = c_char
td_u8 = c_ubyte
td_u16 = c_ushort
td_u32 = c_uint

class pin_cfg_data(Structure):
    _fields_ = [
        ("pin_mode", td_u32, 8),
        ("pin_ds", td_u32, 4),
        ("pin_pd", td_u32, 1),
        ("pin_pu", td_u32, 1),
        ("pin_ie", td_u32, 1),
        ("pin_st", td_u32, 1),
        ("pin_index", td_u32, 10),
        ("pin_reserved", td_u32, 5),
        ("pin_cfg_en", td_u32, 1),
    ]

class pinctrl_group(Structure):
    _fields_ = [
        ("head_magic", td_u32),
        ("crc", td_u32),
        ("pin_num", td_u16),
        ("pin_cfg_flag", td_u16),
    ]

class pin_cfg_image_info(Structure):
    _fields_ = [
        ("product_id", td_u32),
        ("addr", td_u32),
        ("load_addr", td_u32),
        ("length", td_u32),
    ]

class pin_cfg_package_header(Structure):
    _fields_ = [
        ("head_magic", td_u32),
        ("cfg_num", td_u32),
        ("offset", td_u32),
        ("length", td_u32),
    ]

pinctrl_cfg_map = {
    '管脚模式': 'pin_mode',
    '驱动能力': 'pin_ds',
    '上拉': 'pin_pu',
    '下拉': 'pin_pd',
    'IE': 'pin_ie',
    'ST': 'pin_st',
    'PIN_INDEX': 'pin_index',
    '配置使能': 'pin_cfg_en',
}

def get_product_id(data, product):
    id = 0
    for i, j in data:
        if j == product:
            id = int(i, 16)
            break
    if id != 0:
        return id
    else:
        print("[pin cfg] can't find idx of product[%s]" % product)
        sys.exit(1)

def get_product_name(data, idx):
    name = ""
    for i, j in data:
        if int(i, 16) == idx:
            name = j
            break
    if name:
        return name
    else:
        print("[pin cfg] can't find product name of idx[%d]" % idx)
        sys.exit(1)

def get_json_data(inputpath):
    json_path = os.path.join(inputpath, "pin_cfg", "product_id.json")
    with open(json_path, 'r', encoding='utf-8') as j:
        json_read = j.read()
        json_data = json.loads(json_read)
    return json_data.get("product_id_table")

def get_load_addr(chip, target, outputpath):
    var = 'g_pin_cfg_load_start'
    nm_file_path = os.path.join(outputpath, "{}.nm".format(target))
    nm_data = pd.read_csv(nm_file_path, sep='|', header=None, names=['Name', 'Value', 'Class', 'Type', 'Size', 'Line', 'Section'])
    if not var in nm_data["Name"].values:
        print("[pin cfg] cannot find g_pin_cfg_load_start symbol in nm file!")
        sys.exit(1)
    load_addr = nm_data.loc[nm_data['Name'] == var, 'Value'].values[0]
    return int("0x{}".format(load_addr), 16)

def cfg_data_check(inputpath, sheet, diff_data):
    default_path = os.path.join(inputpath, "pin_cfg", "default", "avaliable_cfg.xlsx")
    if not os.path.exists(default_path):
       print("[pin cfg] no need to check cfg data!")
       return
    check_data = pd.read_excel(default_path)
    for i in diff_data.index:
        col = "func{}".format(sheet.loc[i]["pin_mode"])
        if col not in check_data.columns or check_data.loc[i][col] == 0:
            print("[pin cfg] pin mode not avaliable, index = ", i)
            sys.exit(1)

def fill_cfg_crc(file_path):
    if os.path.getsize(file_path) <= 8:
        print("[pin cfg] pin file error!")
        sys.exit(1)
    length = os.path.getsize(file_path) - 8
    with open(file_path, 'rb') as f:
        f.seek(8)
        data = f.read(length)
        crc32_value = zlib.crc32(data) & 0xFFFFFFFF
    with open(file_path, 'rb+') as f:
        f.seek(4)
        f.write(struct.pack("I", crc32_value))

def fill_cfg_head(output_path, sheet_data, diff_data):
    tmp = pinctrl_group()
    tmp.head_magic = 0x5a5a5a5a
    tmp.pin_num = len(sheet_data.index)
    tmp.pin_cfg_flag = 0
    with open(output_path, 'wb') as f:
        f.write(tmp)

def fill_cfg_data(output_path, sheet_data, diff_data):
    with open(output_path, 'ab+') as f:
        for index in sheet_data.index:
            row = sheet_data.loc[index]
            tmp = pin_cfg_data()
            tmp.pin_mode = row["pin_mode"]
            tmp.pin_ds = row["pin_ds"]
            tmp.pin_pu = row["pin_pu"]
            tmp.pin_pd = row["pin_pd"]
            tmp.pin_ie = row["pin_ie"]
            tmp.pin_st = row["pin_st"]
            tmp.pin_index = row["pin_index"]
            tmp.pin_reserved = 0
            if index in diff_data.index:
                tmp.pin_cfg_en = 1
            else:
                tmp.pin_cfg_en = 0
            f.write(tmp)

def fill_pin_cfg(inputpath, output_file, default_sheet, custom_sheet):
    diff_data = custom_sheet.compare(default_sheet)
    cfg_data_check(inputpath, custom_sheet, diff_data)
    fill_cfg_head(output_file, custom_sheet, diff_data)
    fill_cfg_data(output_file, custom_sheet, diff_data)
    fill_cfg_crc(output_file)

def merge_all_product_pin_cfg(chip, target, json_data, outputpath, file_name):
    output_file_path = os.path.join(outputpath, "pin_cfg.bin")
    file_list = list()
    for file in file_name:
        cfg_file_path = os.path.join(outputpath, "{}_pin_cfg.bin".format(file))
        file_list.append(cfg_file_path)
    if not file_list:
        print("[pin cfg] cfg file list is null!")
        sys.exit(1)
    image_num = len(file_name)
    file_length = os.path.getsize(file_list[0])
    with open(output_file_path, "wb") as f:
        header = pin_cfg_package_header()
        header.head_magic = 0x5a5a5a5a
        header.cfg_num = image_num
        header.offset = 16
        header.length = file_length * image_num + 16 * image_num + 16
        f.write(header)

    load_addr = get_load_addr(chip, target, outputpath)
    with open(output_file_path, "rb+") as f1:
        for i, path in enumerate(file_list):
            node = pin_cfg_image_info()
            node.addr = 16 + image_num * 16 + i * file_length
            node.length = file_length
            node.product_id =  get_product_id(json_data, file_name[i])
            node.load_addr = load_addr
            f1.seek(16 + i * 16)
            f1.write(node)
            with open(path, "rb") as image:
                data = image.read()
            f1.seek(16 + image_num * 16 + i * file_length)
            f1.write(data)
    print("[pin cfg] packet all product pin cfg success!")

def create_pin_cfg(chip, target, target_name, inputpath, outputpath):
    print("[pin cfg] input_file_path:", inputpath)
    default_file_path = os.path.join(inputpath, 'pin_cfg', 'default', 'default.xlsx')
    default_sheet = pd.read_excel(default_file_path)
    default_sheet_data = default_sheet.rename(columns=pinctrl_cfg_map)

    sheet_list = dict()
    custom_file_path = os.path.join(inputpath, 'pin_cfg', 'product')
    if not os.path.exists(custom_file_path):
        print("[pin cfg] product excel file not found!")
        sys.exit(1)
    custom_file = os.listdir(custom_file_path)
    for file in custom_file:
        file_path = os.path.join(custom_file_path, file)
        sheet = pd.read_excel(file_path)
        if len(sheet.index) != len(default_sheet_data.index):
            print("[pin cfg] product excel file error!")
            sys.exit(1)
        sheet_data = sheet.rename(columns=pinctrl_cfg_map)
        sheet_list[file.split(".")[0]] = sheet_data

    json_data = get_json_data(inputpath)
    if "device_only" in target_name:
        product_name = "phone"
        sh = sheet_list[product_name]
        output_file_path = os.path.join(outputpath, "pin_cfg.bin")
        fill_pin_cfg(inputpath, output_file_path, default_sheet_data, sh)
        return

    for key, sh in sheet_list.items():
        product_file_name = key + "_pin_cfg.bin"
        output_file_path = os.path.join(outputpath, product_file_name)
        fill_pin_cfg(inputpath, output_file_path, default_sheet_data, sh)
    merge_all_product_pin_cfg(chip, target, json_data, outputpath, list(sheet_list.keys()))

def show_pin_cfg(chip, target, target_name, inputpath, outputpath):
    print("[pin cfg] show diff pinmux set!")

def create_default_xlsx(chip, target, target_name, inputpath, outputpath):   
    chip_excel_path = os.path.join(inputpath, "pin_cfg", chip + ".xlsx")
    tmp_path = os.path.join(inputpath, "pin_cfg", chip + "_tmp.xlsx")
    out_path = os.path.join(inputpath, "pin_cfg", "avaliable_cfg.xlsx")

    shutil.copyfile(chip_excel_path, tmp_path)
    wb = openpyxl.load_workbook(filename=tmp_path, data_only=True)
    ws = wb.active
    for row in ws.iter_rows():
        for cell in row:
            # 检查字体是否有删除线
            if cell.font.strike:
                cell.value = "No-Func"
    wb.save(tmp_path)
    wb.close()

    default_sheet = pd.read_excel(tmp_path)
    writer = pd.ExcelWriter(out_path)   
    column_names = default_sheet.columns
    mode_list = ["PAD Name"]
    pattern = r'func\d+'
    for i in column_names:
        matches = re.findall(pattern, i)
        if matches:
            mode_list.append(i)

    pin_num = 0
    for i in range(len(default_sheet.index)):
        if pd.isnull(default_sheet["PAD Name"].loc[i]):
            pin_num = i
            break

    if pin_num == 0:
        print("[pin cfg] chip excel file error!")
        sys.exit(1)

    default_sheet = default_sheet[mode_list][0:pin_num].copy()
    for col in default_sheet.columns[1:]:
        for i, row in enumerate(default_sheet[col]):
            if "No-Func" in row:
                default_sheet[col][i] = 0
            else:
                default_sheet[col][i] = 1
    default_sheet.to_excel(writer, sheet_name="avaliable_cfg", index=True)
    writer.save()
    os.remove(tmp_path)


if __name__ == '__main__':
    args = len(sys.argv)
    argv = sys.argv
    if(args != 7):
        print("[pin cfg] parma error!")
        sys.exit(1)
    chip = argv[2]
    target = argv[3]
    target_name = argv[4]
    inputpath = argv[5]
    outputpath = argv[6]
    if (argv[1] == '-create'):
        create_pin_cfg(chip, target, target_name, inputpath, outputpath)
    elif (argv[1] == '-show'):
        show_pin_cfg(chip, target, target_name, inputpath, outputpath)
    elif (argv[1] == '-check'):
        create_default_xlsx(chip, target, target_name, inputpath, outputpath)