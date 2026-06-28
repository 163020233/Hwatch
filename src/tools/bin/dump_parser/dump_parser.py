#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.

"""
用法: python3 dump_parser.py -d DEFINE -s STRUCT -i INPUT [-l LST] [-o OUTPUT] [-c CC] [-e {little,big}]
  -d DEFINE, --define DEFINE
                        包含结构体定义的 C 头文件路径
  -s STRUCT, --struct STRUCT
                        要解析的结构体名
  -i INPUT, --input INPUT
                        要解析的二进制文件
  -l LST, --lst LST     要解析的 lst 文件
  -p LST PATH, --path LST PATH     要解析的 lst的目录
  -o OUTPUT, --output OUTPUT
                        将解析结果保存到文件中 (默认: 无)
  -c CC, --cc CC        要使用的 C 语言预处理器 (默认: 不使用)
  -e {little,big}, --endian {little,big}
                        大小端序 (默认: little)
例如: python3 dump_parser.py -d melody.h -s exc_info_save_t -i dump.bin

注意: 为避免平台不一致导致的解析错误, 建议头文件中声明的结构体使用 uintXX_t, intXX_t 等类型
"""

from enum import Enum
import sys
import os
import ctypes
import re
import pycparser
import pycparser.c_ast

basic_types = {
    "": ctypes.c_int,
    "void": None,
    "char": ctypes.c_byte,
    "unsigned char": ctypes.c_ubyte,
    "short": ctypes.c_short,
    "unsigned short": ctypes.c_ushort,
    "int": ctypes.c_int,
    "unsigned int": ctypes.c_uint,
    "unsigned": ctypes.c_uint,
    "long": ctypes.c_long,
    "unsigned long": ctypes.c_ulong,
    "long long": ctypes.c_longlong,
    "unsigned long long": ctypes.c_ulonglong,
    "float": ctypes.c_float,
    "double": ctypes.c_double,
    "long double": ctypes.c_longdouble,
    "_Bool": ctypes.c_bool,
    "int8_t": ctypes.c_int8,
    "uint8_t": ctypes.c_uint8,
    "int16_t": ctypes.c_int16,
    "uint16_t": ctypes.c_uint16,
    "int32_t": ctypes.c_int32,
    "uint32_t": ctypes.c_uint32,
    "int64_t": ctypes.c_int64,
    "uint64_t": ctypes.c_uint64,
}
hex_types = [ctypes.c_uint, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint32]
hex_types = [(t.__ctype_be__, t.__ctype_le__) for t in hex_types]
hex_types = [t for sublist in hex_types for t in sublist]
functions = [] # 函数名称及其地址范围(符号表)
stack_size = -1 # HACK 自动读取调用栈大小的临时解决方案
traceback = [] # 调用栈函数名称
deep_len = 0
functions_a = [] # A核函数名称及其地址范围(符号表)
functions_bt = [] # BT核函数名称及其地址范围(符号表)

class Visitor(pycparser.c_ast.NodeVisitor):
    def __init__(self, endian):
        self.is_big_endian = endian == "big"
        self.types = dict(basic_types)
        self.id_context = {}

    def generic_visit(self, node):
        raise RuntimeError(f"Unvisit AST element at {node.coord}: {node}")

    def visit_FileAST(self, node):
        for c in node:
            self.visit(c)

    def visit_Decl(self, node):
        return self.visit(node.type)

    def visit_FuncDef(self, node):
        return

    def visit_FuncDecl(self, node):
        return

    def visit_Typename(self, node):
        return self.visit(node.type)

    def visit_TypeDecl(self, node):
        return self.visit(node.type)

    def visit_ArrayDecl(self, node):
        length = self.visit(node.dim)
        type = self.visit(node.type)
        if length is None or length < 0:
            raise RuntimeError(f"Invalid array length '{length}' at {node.dim.coord}")
        if length == 0:
            length = int(input(f"Input the size of array '{node.type.declname}': "))
        if type is None:
            raise RuntimeError(f"Invalid array type '{type}' at {node.type.coord}")
        return type * length

    def visit_PtrDecl(self, node):
        pointer = self.visit(node.type)
        return ctypes.POINTER(pointer)

    def visit_Typedef(self, node):
        if node.name in self.types:
            return
        type = self.visit(node.type)
        self.types[node.name] = type

    def visit_IdentifierType(self, node):
        try:
            if "signed" in node.names:
                node.names.remove("signed")
            if ("short" in node.names or "long" in node.names) and "int" in node.names:
                node.names.remove("int")
            name = " ".join(node.names)
            return self.types[name]
        except KeyError:
            raise RuntimeError(f"Invalid type specifier '{name}' at {node.coord}")

    def visit_Enum(self, node):
        if node.values is None:
            return self.types[node.name]
        enums = {}
        value = -1
        for enum in node.values:
            if enum.value is None:
                value += 1
            else:
                value = self.visit(enum.value)
            self.id_context[enum.name] = enums[enum.name] = value
        enum_name = node.name if node.name is not None else "enum_anon"
        enum_type = type(enum_name, (ctypes.c_uint,), {"members": enums})
        self.types[enum_name] = enum_type
        return enum_type

    def visit_Struct(self, node):
        if node.decls is None:
            return self.types[node.name]
        fields = []
        for decl in node.decls:
            value = self.visit(decl.type)
            if value is None:
                return None
            if decl.bitsize is not None:
                fields.append((decl.name, value, int(decl.bitsize.value)))
            else:
                fields.append((decl.name, value))
        struct_name = node.name if node.name is not None else "struct_anon"
        struct_class = ctypes.BigEndianStructure if self.is_big_endian else ctypes.LittleEndianStructure
        struct_type = type(struct_name, (struct_class,), {"_fields_": fields}) # TODO _pack_ 暂时无修改对齐的需求
        self.types[struct_name] = struct_type
        return struct_type

    def visit_Union(self, node):
        if node.decls is None:
            return self.types[node.name]
        fields = []
        for decl in node.decls:
            value = self.visit(decl.type)
            if value is None:
                return None
            fields.append((decl.name, value))
        union_name = node.name if node.name is not None else "union_anon"
        union_class = ctypes.Union
        if sys.version_info > (3, 11): # New in python 3.11
            union_class = ctypes.BigEndianUnion if self.is_big_endian else ctypes.LittleEndianUnion
        union_type = type(union_name, (union_class,), {"_fields_": fields})
        self.types[union_name] = union_type
        return union_type

    def visit_ID(self, node):
        try:
            return self.id_context[node.name]
        except KeyError:
            raise RuntimeError(f"Failed to resolve identifier '{node.name}' at {node.coord}")

    def visit_Constant(self, node):
        if node.type not in ["int", "unsigned int"]:
            raise RuntimeError(f"Unhandled Constant node type '{node.type}' at {node.coord}")
        value = node.value.rstrip("LlUu")
        if value.startswith(("0x", "0X")):
            return int(value, 16)
        return int(value)

def parse_c(src, struct, cc, endian):
    node = pycparser.parse_file(src, use_cpp=False) if cc is None else \
            pycparser.parse_file(src, use_cpp=True, cpp_path=cc, cpp_args=["-E"])
    v = Visitor(endian)
    v.visit(node)
    return v.types[struct]

def parse_bin(struct, f):
    bytearr = f.read()
    obj = struct()
    ctypes.memmove(ctypes.addressof(obj), bytearr, ctypes.sizeof(obj))
    return obj

def parse_lst(f):
    functions = []
    is_symbol_table = False
    while True:
        line = f.readline()
        if not line:
            break
        line = line.strip()
        if len(line) == 0:
            continue
        if not is_symbol_table:
            if line == "SYMBOL TABLE:":
                is_symbol_table = True
            continue
        if "Disassembly of section" in line:
            break
        item = line.split()
        if item[2] != "F":
            continue
        addr = int(item[0], base=16)
        length = int(item[4], base=16)
        functions.append((addr, addr + length, item[-1]))
    functions.sort(key=lambda x: x[0])
    return functions

def find_function(start, end, addr):
    if start >= end:
        return "???"
    mid = (start + end) // 2
    func = functions[mid]
    if addr < func[0]:
        return find_function(start, mid, addr)
    if addr >= func[1]:
        return find_function(mid + 1, end, addr)
    return func[2]

def parse_stack_arm(stack):
    traceback = []
    for item in stack:
        traceback.append(find_function(0, len(functions), item.fp))
    return traceback

def parse_stack_riscv(ra, stack):
    traceback = []
    traceback.append(find_function(0, len(functions), ra))
    for item in stack:
        traceback.append(find_function(0, len(functions), item.ra))
    return traceback

def print_obj(obj, out=sys.stdout):
    global stack_size
    global deep_len
    global traceback
    global functions

    if isinstance(obj, ctypes.Structure): # 结构体
        deep_len += 1
        if hasattr(obj, "stack") and len(functions) > 0:
            if hasattr(obj, "ra"):
                traceback = parse_stack_riscv(obj.mepc, obj.stack)
            else:
                traceback = parse_stack_arm(obj.stack)
        for field in obj._fields_:
            if (len(obj._fields_) > 2):
                out.write("\n")
                for deep in range(1, deep_len):
                    out.write(f"    ")
            else:
                out.write("\t")

            name = field[0]
            value = getattr(obj, name)

            if re.match(r'^hi3322_(unused|stack)', name):
                continue

            out.write(f"{name}:")

            if len(functions)==0:
                if (name == "crash_data_a_core"):
                    functions = functions_a
                elif (name == "crash_data_bt_core"):
                    functions = functions_bt

            if isinstance(value, (int, float, bool, str)):
                value = field[1](value)
            if name == "stack":
                stack_size = getattr(obj, "stack_size")
            elif name == "backtrace":
                stack_size = getattr(obj, "backtrace_size")
            print_obj(value, out)
        deep_len -= 1
    elif isinstance(obj, ctypes.Union): # 共用体
        deep_len += 1
        print("\nSelect the type of this union:")
        i = 1
        for field in obj._fields_:
            print(f"{i}. {field[0]}")
            i += 1
        num = int(input("Input number: "))
        for deep in range(1, deep_len):
            out.write(f"    ")
        field = obj._fields_[num - 1]
        out.write("\n")
        name = field[0]
        value = getattr(obj, name)
        out.write(f"{name}:")
        if isinstance(value, (int, float, bool, str)):
            value = field[1](value)
        print_obj(value, out)
        deep_len -= 1
    elif isinstance(obj, ctypes.Array): # 数组
        if obj._type_ == ctypes.c_byte: # C风格字符串
            byte_arr = bytes(obj)
            byte_arr = byte_arr[0:byte_arr.find(b"\x00")]
            print_obj(byte_arr.decode("utf-8", "ignore"), out)
        else:
            prefix = issubclass(obj._type_, ctypes.Structure) or issubclass(obj._type_, ctypes.Union)
            i = 0
            for element in obj:
                if stack_size >= 0 and i >= stack_size:
                    break
                if prefix:
                    out.write(f"\n")
                    for deep in range(1, deep_len):
                        out.write(f"    ")
                    out.write(f"[{i}]:")
                if isinstance(element, (int, float, bool, str)):
                    element = obj._type_(element)
                print_obj(element, out)
                if stack_size >= 0 and i < len(traceback):
                    out.write(f"\t<{traceback[i]}>")
                i += 1
            stack_size = -1
    elif hasattr(obj, "members"): # 枚举
        enums = [item[0] for item in obj.members.items() if item[1] == obj.value]
        enum = enums[0] if len(enums) > 0 else "UNKNOWN"
        out.write(f" {enum} ({hex(obj.value)})")
    elif hasattr(obj, "value"): # 基本类型
        t = type(obj)
        if t in hex_types: # 以十六进制显示的无符号数
            out.write(f" 0x{obj.value:08x}")
            if stack_size == -1:
                out.write(f" ({obj.value})")
        else:
            out.write(f" {obj.value}")
    else: # 其它类型直接输出
        out.write(f" {obj}")

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-d", "--define", type=str, required=True, help="C header file path")
    parser.add_argument("-s", "--struct", type=str, required=True, help="The name of struct to be parsed")
    parser.add_argument("-i", "--input", type=str, required=True, help="The bin file to be parsed")
    parser.add_argument("-l", "--lst", type=str, help="The lst file to be parsed")
    parser.add_argument("-p", "--path", type=str, help="The directory of lst files to be parsed")
    parser.add_argument("-o", "--output", type=str, help="Save the parse result into file (default: None)")
    parser.add_argument("-c", "--cc", default=None, type=str, help="C preprocessor path (default: Not use)")
    parser.add_argument("-e", "--endian", default="little", type=str, choices=["little", "big"],
                                            help="Choose little or big endian (default: little)")
    args = parser.parse_args()

    if args.lst:
        with open(args.lst, "r", encoding="utf-8") as f:
            functions = parse_lst(f)

    if args.path:
        app_lst_path = os.path.join(args.path, "application.lst")
        with open(app_lst_path, "r", encoding="utf-8") as fa:
            functions_a = parse_lst(fa)

        bt_lst_path = os.path.join(args.path, "bt.lst")
        with open(bt_lst_path, "r", encoding="utf-8") as fbt:
            functions_bt = parse_lst(fbt)

    header_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), args.define)
    root_struct = parse_c(header_path, args.struct, args.cc, args.endian)
    with open(args.input, "rb") as f:
        obj = parse_bin(root_struct, f)
        # if hasattr(obj, "stack") and len(functions) > 0:
        #     if hasattr(obj, "ra"):
        #         traceback = parse_stack_riscv(obj.mepc, obj.stack)
        #     else:
        #         traceback = parse_stack_arm(obj.stack)
        if args.output is not None:
            out = open(args.output, "w")
            out.write("Crash info:")
            print_obj(obj, out)
            out.close()
        else:
            print("Crash info:", end="")
            print_obj(obj)
    print("\n========== finished! ==========")
