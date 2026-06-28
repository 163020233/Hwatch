#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.

"""
脚本名称: gen_rom_keep_symbols.py
功能：从 asic.nm 中提取指定地址范围内的函数，生成 CMake 链接选项。
支持命令行参数：
  --inputfile=xxx.nm
  --output=xxx.cmake
  --addr_region=0x203000,0x2c0000   # 不带花括号！
"""

import sys
import argparse
import re
import os

def parse_nm_file(file_path):
    """
    解析 asic.nm 文件，提取 FUNC 类型且非 OUTLINED_FUNCTION_ 的符号
    """
    functions = []
    
    if not os.path.exists(file_path):
        print(f"❌ 错误：找不到输入文件 {file_path}")
        sys.exit(1)

    with open(file_path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            # 跳过空行和表头
            if not line or line.startswith('Name'):
                continue
            
            # 使用竖线 | 分割，这是最稳健的方式
            parts = [p.strip() for p in line.split('|')]
            if len(parts) < 6:
                continue
            
            try:
                name = parts[0]
                value = parts[1]
                type_str = parts[3]
                
                # 过滤条件 1: 必须是 FUNC
                if type_str != "FUNC":
                    continue
                
                # 过滤条件 2: 不能以 OUTLINED_FUNCTION_ 开头
                if name.startswith("OUTLINED_FUNCTION_"):
                    continue
                
                functions.append({
                    'name': name,
                    'value': value
                })
            except Exception as e:
                # 忽略解析错误的行
                continue
                
    return functions

def parse_address_range(addr_str):
    """
    解析形如 0x203000,0x2c0000 的字符串
    返回 (start_addr_int, end_addr_int)
    """
    # 去掉首尾空格
    addr_str = addr_str.strip()
    if not addr_str:
        raise ValueError("地址范围不能为空。")
    
    # 使用正则提取两个十六进制地址
    # 允许 0x 前缀，也允许不带（但必须是十六进制）
    pattern = r'^\s*((0x)?[a-fA-F0-9]+)\s*,\s*((0x)?[a-fA-F0-9]+)\s*$'
    match = re.match(pattern, addr_str)
    
    if not match:
        raise ValueError(f"无法解析地址范围格式：{addr_str}。期望格式: 0xstart,0xend (例如: 0x203000,0x2c0000)")

    start_hex, end_hex = match.groups()
    
    try:
        start_val = int(start_hex, 16)
        end_val = int(end_hex, 16)
    except ValueError as e:
        raise ValueError(f"无效的十六进制地址格式：{e}")
    if end_val >= start_val:
        raise ValueError(f"结束地址小于起始地址!")
    return start_val, end_val

def filter_by_address_range(functions, start_addr, end_addr):
    """
    根据地址范围筛选函数
    """
    filtered_names = []
    for func in functions:
        try:
            func_addr = int(func['value'], 16)
            if start_addr <= func_addr <= end_addr:
                filtered_names.append(func['name'])
        except ValueError:
            # 地址无法转换，跳过
            continue
    
    return filtered_names

def generate_link_options(func_names):
    """
    生成 -Wl,--undefined=xxx 格式
    """
    return [f"  -Wl,--undefined={name}" for name in func_names]

def write_to_cmake(output_file, content_lines):
    """
    将内容写入 CMake 文件（覆盖模式）
    """
    # 确保输出目录存在
    output_dir = os.path.dirname(output_file)
    if output_dir and not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write("target_link_options(${TARGET_NAME} PRIVATE" + '\n')
        for line in content_lines:
            f.write(line + '\n')
        f.write(")")
    
    print(f"✅ 成功生成链接选项到: {output_file}")
    print(f"   共找到 {len(content_lines)} 个函数")

def main():
    # 定义参数解析器
    parser = argparse.ArgumentParser(
        description="从 asic.nm 中提取指定地址范围内的函数，生成 CMake 链接选项。"
    )
    
    parser.add_argument(
        '--inputfile', 
        required=True, 
        help="输入的 asic.nm 符号表文件路径"
    )
    
    parser.add_argument(
        '--output', 
        required=True, 
        help="输出的 CMake 文件路径 (xxx.cmake)"
    )
    
    parser.add_argument(
        '--addr_region', 
        required=True, 
        help="地址范围，格式为 0xstart,0xend (例如: 0x203000,0x2c0000))"
    )

    # 解析参数
    args = parser.parse_args()

    # 1. 检查输入文件是否存在
    if not os.path.exists(args.inputfile):
        print(f"❌ 错误：找不到输入文件 {args.inputfile}")
        sys.exit(1)

    print(f"🔍 正在解析符号表文件: {args.inputfile}")
    functions = parse_nm_file(args.inputfile)
    print(f"📊 解析完成，共找到 {len(functions)} 个 FUNC 类型符号 (已过滤 OUTLINED_FUNCTION_)")

    # 2. 解析地址范围
    print(f"📍 正在解析地址范围: {args.addr_region}")
    try:
        start_addr, end_addr = parse_address_range(args.addr_region)
    except ValueError as e:
        print(f"❌ 错误：{e}")
        sys.exit(1)

    print(f"✅ 地址范围解析成功: 0x{start_addr:08x} ~ 0x{end_addr:08x}")

    # 3. 筛选函数
    print(f"🔍 正在筛选地址范围内的函数...")
    target_funcs = filter_by_address_range(functions, start_addr, end_addr)

    if not target_funcs:
        print("⚠️  在指定范围内未找到符合条件的函数。")
        # 生成空文件
        write_to_cmake(args.output, [])
        return

    print(f"✅ 找到 {len(target_funcs)} 个目标函数，正在生成链接选项...")
    link_opts = generate_link_options(target_funcs)

    # 4. 写入文件
    write_to_cmake(args.output, link_opts)

if __name__ == "__main__":
    main()
