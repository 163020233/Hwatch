#!/usr/bin/env python3
# encoding=utf-8
#===============================================================================
# @brief    ROM Patch script.
# Copyright Huawei Technologies Co., Ltd. 2019-2025. All rights reserved.
#===============================================================================
import shutil
import struct
import ctypes
import sys
import os

class SymbolInfo:
    def __init__(self, sym_addr, sym_size) -> None:
        self.addr = 0
        self.size = 0
        if sym_addr != '':
            self.addr = int(sym_addr, 16)
        if sym_size != '':
            self.size = int(sym_size, 16)

class PatchBase:
    def __init__(self, nm:str, patch_ini:str) -> None:
        self.sym_addr = {}
        self.patch_settings = {}
        self.patch_function = {}
        self.parse_nm(nm)
        self.parse_ini(patch_ini)
        self.init_env()

    def init_env(self):
        self.reg_num = int(self.patch_settings['Reg_Number'])
        self.cmp_head_len = 3
        self.cmp_reg_count = self.reg_num
        self.cmp_total_len = self.cmp_head_len + self.cmp_reg_count
        self.cmp_reg_size = 4
        # self.jump_reg_size = self.patch_settings['patch_jump_reg_size']

        # Version1: 外挂patch(cmp地址 8字节对齐)
        # Version2: Himideer自带flash patch
        self.version = self.patch_settings['Device_Code_Version']
        # riscv jal指令(4字节)最多支持20位偏移地址跳转，即1M, 超过1M时需要使用tail伪指令(8字节).
        # rom 和 patch地址之间的距离大于1M, 使用long_jump, 否则使用short_jump.
        self.jump_version = self.patch_settings['Jump_Version']

        if self.version == "Version1":
            self.table_reg_size = 8
            self.data_patch_count = 0
        elif self.version == "Version2":
            self.table_reg_size = 4 if self.jump_version == "Short" else 8
            # 该参数(data_patch_count)通过PatchInfo.ini得到,表示支持的load地址补丁,一般叫做只读数据patch; linx131支持两个datapatch
            self.data_patch_count = int(self.patch_settings["Data_Patch_Count"], 10)

        self.patch_file_addr = \
            self.sym_addr[self.patch_settings['Patch_File_Address_Symbol']].addr
        self.patch_table_addr = \
            self.sym_addr[self.patch_settings['Patch_TBL_Address_Symbol']].addr
        self.patch_code_addr = \
            self.sym_addr[self.patch_settings['Patch_Code_Address_Symbol']].addr
        self.rom_addr = \
            self.sym_addr[self.patch_settings['ROM_Address']].addr
        self.rom_size = \
            self.sym_addr[self.patch_settings['ROM_Size']].addr

        if self.jump_version == "Short":
            if ((self.patch_table_addr%1024) != 0):
                print("ERROR: Short jump require 1K-aligned remap addresses.")
                sys.exit(1)
        else:
            if ((self.patch_table_addr%2048) != 0):
                print("ERROR: Long jump require 2K-aligned remap addresses.")
                sys.exit(1)
        self.cmp_offset = self.patch_table_addr - self.patch_file_addr + self.reg_num * self.table_reg_size
        self.tbl_offset = self.patch_table_addr - self.patch_file_addr + self.data_patch_count * self.table_reg_size


    def parse_patch_item(self, item:str):
        [item_key , item_value] = item.split('$patch$')
        if item.endswith('hard'):
            self.patch_function[item_key] = item_value[0 : -len('_hard')]
        elif item.endswith('soft'):
            self.patch_function[item_key] = item_value[0 : -len('_soft')]
        else:
            print("ERROR: unsupport key %s." % item_key)
            sys.exit(1)

    def read_file(self, name):
        with open(name, 'r') as f:
            return f.readlines()

    def parse_nm(self, nm):
        nm_lines = self.read_file(nm)
        for line in nm_lines:
            arr = line.split('|')
            if len(arr) < 2:
                continue
            sym_name = arr[0].strip()
            addr = arr[1].strip()
            size = arr[4].strip()
            self.sym_addr[sym_name] = SymbolInfo(addr, size)
            if "$patch$" in sym_name:
                self.parse_patch_item(sym_name)
                continue

    def parse_ini(self, ini):
        ini_lines = self.read_file(ini)
        for line in ini_lines:
            if '=' not in line:
                continue
            key = line.split('=')[0].strip()
            value = line.split('=')[1].strip()
            self.patch_settings[key] = value

    def get_sym_addr(self, symbol_name):
        if symbol_name not in self.sym_addr:
            print("Error: %s Symbol Can't Find in Map File !" % symbol_name)
            sys.exit(1)
        return self.sym_addr[symbol_name].addr

    def get_sym_size(self, symbol_name):
        if symbol_name not in self.sym_addr:
            print("Error: %s Symbol Can't Find in Map File !" % symbol_name)
            sys.exit(1)
        return self.sym_addr[symbol_name].size


class PatchGenerator(PatchBase):
    def __init__(self, nm, patch_ini, rom_file, patch_file):
        super().__init__(nm, patch_ini)
        self.current_work_dir = os.getcwd()
        self.dir_name = os.path.dirname(os.path.realpath(__file__))
        self.rom_file = rom_file
        self.patch_file = patch_file

    def get_dir(self, dir_in):
        return os.path.join(self.current_work_dir, dir_in)

    def remove_bin_file(self):
        for bin_file in ["RW.bin"]:
            if os.path.exists(self.get_dir(bin_file)):
                os.remove(self.get_dir(bin_file))

    # Table or Cmp
    def create_bin_file(self, file_name, contents):
        try:
            with open(file_name, "wb") as file_o:
                for content in contents:
                    byte = struct.pack('I', content)
                    file_o.write(byte)
        except Exception as e:
            print("Error: %s Can't Open!" % file_name)
            print("Exception: %s" % e.message)
            self.remove_bin_file()
            sys.exit(1)

    def create_txt_file(self, outfile):
        with open(outfile, "w") as file_out:
            file_out.write("Patch Table Address\t\t= 0x%x\n" % self.patch_table_addr)
            file_out.write("Patch Code Address\t\t= 0x%x\n" % self.patch_code_addr)
            file_out.write("Device Code Version\t\t= %s\n" % self.version)

    def create_patch_file(self, old_func_names, new_func_names, outfile):
        with open(outfile, "w+") as file_out:
            for old_func, new_func in zip(old_func_names, new_func_names):
                file_out.write('%s %s\n' % (old_func, new_func))

    def copy_bin_file(self, str_src, str_dst):
        try:
            shutil.copy(str_src, str_dst)
        except Exception as e:
            print(f"Exception: {e}")
            self.remove_bin_file()
            sys.exit(1)

    def merge_output_file(self):
        try:
            with open(self.get_dir('RW.bin'), "rb+") as file_rw:
                file_rw.seek(self.tbl_offset, 0)
                for data in self.tbl_content:
                    byte1 = struct.pack('<I', data)
                    file_rw.write(byte1)

                file_rw.seek(self.cmp_offset, 0)
                for data in self.cmp_content:
                    byte2 = struct.pack('<I', data)
                    file_rw.write(byte2)
        except Exception as e:
            print(f"Exception: {e}")
            self.remove_bin_file()
            sys.exit(1)

    # 取函数在bin文件中的二进制码
    def get_func_addr_bin_content(self, old_func_addr, align_byte, bin_content):
        rom_address = self.rom_addr
        rom_size = self.rom_size
        if rom_address <= old_func_addr < rom_address + rom_size:
            rom_file_path = self.rom_file
        else:
            print(f"addr {hex(int(str(old_func_addr), 10))} not in rom range, please check rom addr/size in PatchInfo ini")
            sys.exit(1)
        offset_in_bin = old_func_addr - rom_address - align_byte
        read_size = 4 if align_byte == 6 else 2

        if self.rom_size - offset_in_bin < read_size * 4:
            print("Can not Patch: func_addr:%x" % old_func_addr)
            sys.exit(1)

        with open(rom_file_path, "rb") as file_rom:
            self.rom_bin = file_rom.read()
            file_rom.seek(offset_in_bin, 0)
            for _ in range(read_size):
                func_code = file_rom.read(4)
                bin_content.append(struct.unpack('<I', func_code)[0])

    def get_func_addr(self, func_names, new_func_flag):
        func_addrs = []
        for func_name in func_names:
            func_addr = self.get_sym_addr(func_name)
            func_size = self.get_sym_size(func_name)
            if not new_func_flag:
                if func_size < 4:
                    print(f"Warning: {func_name} Length is {func_size}, Ensure func is aligned by 4 bytes!")
                elif func_size == 4:
                    print(f"Warning: {func_name} Length is 4 bytes, it may be unsafe!")
            else:
                if func_addr < self.patch_code_addr and self.patch_settings["Func_File"] != "None":
                    print(f"Error: {func_name} Function Addr must be more than {self.patch_code_addr}")
                    sys.exit(1)
            func_addrs.append(func_addr)
        return func_addrs

    # Ctrl寄存器 +  Remap寄存器 + CMP数量 + CMP表
    def get_cmp_content(self, func_addrs):
        cmp_content = [0, self.patch_table_addr, 0]
        if self.version == "Version1":
            # 6字节对齐的放前面
            func_addrs.sort(key=lambda x:0 if x % 8 == 6 else 1)
            for addr in func_addrs:
                align = addr % 8
                addr = (addr - align) | 1
                if addr not in cmp_content:
                    cmp_content.append(addr)
                if align == 6 and (addr + 8) not in cmp_content:
                    cmp_content.append(addr + 8)
        else:
            for addr in func_addrs:
                cmp_content.append(addr | 1)
        cmp_content[2] = len(cmp_content) - 3
        cmp_content.extend([0] * (self.cmp_total_len - len(cmp_content)))
        return cmp_content

    def get_jump_bitcode_for_short_jump(self, func_addr, func_patch_addr):
        off_addr = func_patch_addr - func_addr
        if off_addr >= 0x100000:
            raise ValueError("short jump只能跳转1M范围, patch_addr:%x, func_addr:%x" % (func_patch_addr, func_addr))
        bit0_to6 = 0x6F
        bit7_to11 = 0x5 << 7

        off_bit1_to10 = (off_addr & 0x7fe) >> 1
        off_bit12_to19 = (off_addr & 0xff000) >> 12
        off_bit11 = (off_addr & 0x800) >> 11
        off_bit20 = (off_addr & 0x100000) >> 20

        bit_code = bit0_to6 + bit7_to11 + (off_bit12_to19 << 12) + (off_bit11 << 20) + (off_bit1_to10 << 21) + (off_bit20 << 31)
        return [bit_code]

    def get_jump_bitcode_for_long_jump(self, func_addr, func_patch_addr):
        off_addr = func_patch_addr - func_addr
        auipc_opt_bits = 0x17
        jalr_opt_bits = 0x67
        base_addr_bits = 0x6
        jalr_bit12_to14 = 0x0 << 12
        jalr_bit7_to11 = 0x0 << 7

        off_bit12_to31 = off_addr & 0xfffff000
        off_bit0_to11 = off_addr & 0xfff
        if off_bit0_to11 > 0x7FF:
            off_bit12_to31 = off_bit12_to31 + 0x1000
            off_bit0_to11 = 0x1000 - off_bit0_to11
            off_bit0_to11 = (~off_bit0_to11 + 1) & 0xfff

        auipc_bit_code = auipc_opt_bits + (base_addr_bits << 7) + off_bit12_to31
        jalr_bit_code = jalr_opt_bits + jalr_bit7_to11 + jalr_bit12_to14 + (base_addr_bits << 15) + (off_bit0_to11 << 20)
        return [auipc_bit_code, jalr_bit_code]

    def get_index_in_vector(self, value1, vector):
        try:
            return vector.index(value1)
        except ValueError:
            return -1

    def get_table_content(self, func_addrs, func_patch_addrs, cmp_content=[]):
        # uint32_t table_content[]
        table_content = []

        if self.jump_version == "Short":
            bitcode_func = self.get_jump_bitcode_for_short_jump
        elif self.jump_version == "Long":
            bitcode_func = self.get_jump_bitcode_for_long_jump

        for index, func_addr in enumerate(func_addrs):
            bin_content = []                                # ROM二进制内容
            temp_tbl_content = [0, 0, 0, 0]                 # 如果已存在TBL，读取TBL原值
            func_patch_addr = func_patch_addrs[index]
            bitcodes = bitcode_func(func_addr, func_patch_addr) # 跳转指令

            if self.version == "Version1":
                align = func_addr % 8    # 外挂patch(cmp地址 8字节对齐)
                self.get_func_addr_bin_content(func_addr, align, bin_content)    # 取函数在bin文件中的二进制码
                addr_cmp_idx = 0
                addr_tbl_idx = 0
                addr_cmp_idx = self.get_index_in_vector((func_addr | 0x01) - align, cmp_content)
                addr_tbl_idx = (addr_cmp_idx - self.cmp_head_len) * 2
                # 原函数地址6字节对齐时TBL文件中对应2个64位数据:
                if align == 6:
                    if self.cmp_head_len <= addr_cmp_idx and addr_tbl_idx < len(table_content):
                        # 已存在TBL中，修改TBL
                        temp_tbl_content[1] = ctypes.c_uint32(bitcodes[0] << 16).value + (ctypes.c_uint32(table_content[addr_tbl_idx + 1] << 16).value >> 16)
                        table_content[addr_tbl_idx + 1] = temp_tbl_content[1]
                    else:
                        # 不存在TBL中，添加TBL
                        # 第一个64位数据高16位填充跳转指令低16位，其它48位保留原值；
                        table_content.append(bin_content[0])
                        table_content.append(ctypes.c_uint32(bitcodes[0] << 16).value + (ctypes.c_uint32(bin_content[1] << 16).value >> 16))
                    # 6字节对齐需占用两个TBL表，此处开始处理第二个TBL表
                    addr_cmp_idx = self.get_index_in_vector((func_addr | 0x01) - align + 8, cmp_content)
                    addr_tbl_idx = (addr_cmp_idx - self.cmp_head_len) * 2
                    if self.cmp_head_len <= addr_cmp_idx and addr_tbl_idx < len(table_content):
                        # 已存在TBL中，修改TBL
                        temp_tbl_content[2] = ((table_content[addr_tbl_idx] >> 16) << 16) + (bitcodes[0] >> 16)
                        table_content[addr_tbl_idx] = temp_tbl_content[2]
                    else:
                        # 不存在TBL中，添加TBL
                        # 第二个64位数据低16位填充跳转指令高16位，其它48位保留原值
                        table_content.append(((bin_content[2] >> 16) << 16) + (bitcodes[0] >> 16))
                        table_content.append(bin_content[3])
                elif align == 2:
                    # Version1 函数地址非6字节对齐时,不进行添加，前面只添加6字节对齐部分，添加结束后再添加非6字节对齐部分
                    continue
                elif align == 4:
                    # Version1 函数地址非6字节对齐时,不进行添加，前面只添加6字节对齐部分，添加结束后再添加非6字节对齐部分
                    continue
                elif align == 0:
                    # Version1 函数地址非6字节对齐时,不进行添加，前面只添加6字节对齐部分，添加结束后再添加非6字节对齐部分
                    continue
                else:
                    print(f"Error: instruction illegal alignment, Need to check dis, Patch Failed !")
                    sys.exit(1)
            elif self.version == "Version2":
                table_content.append(bitcodes[0])
                if self.jump_version == "Long":
                    table_content.append(bitcodes[1])

        if self.version == "Version1":
            for index, func_addr in enumerate(func_addrs):
                bin_content = []                                # ROM二进制内容
                temp_tbl_content = [0, 0, 0, 0]                 # 如果已存在TBL，读取TBL原值
                func_patch_addr = func_patch_addrs[index]
                bitcodes = bitcode_func(func_addr, func_patch_addr) # 跳转指令
                align = func_addr % 8                           # 外挂patch(cmp地址 8字节对齐)
                self.get_func_addr_bin_content(func_addr, align, bin_content)    # 取函数在bin文件中的二进制码
                addr_cmp_idx = 0
                addr_tbl_idx = 0
                addr_cmp_idx = self.get_index_in_vector((func_addr | 0x01) - align, cmp_content)
                addr_tbl_idx = (addr_cmp_idx - self.cmp_head_len) * 2
                if align == 6:
                    # 原函数地址6字节对齐时已处理，此处不做处理:
                    continue
                elif align == 2:
                    if self.cmp_head_len <= addr_cmp_idx and addr_tbl_idx < len(table_content):
                        # 已存在TBL中，修改TBL
                        temp_tbl_content[0] = ctypes.c_uint32(bitcodes[0] << 16).value + (ctypes.c_uint32(table_content[addr_tbl_idx] << 16).value >> 16)
                        temp_tbl_content[1] = ((table_content[addr_tbl_idx + 1] >> 16) << 16) + (bitcodes[0] >> 16)
                        table_content[addr_tbl_idx] = temp_tbl_content[0]
                        table_content[addr_tbl_idx + 1] = temp_tbl_content[1]
                    else:
                        # 不存在TBL中，添加TBL
                        # 原函数地址2字节对齐时TBL文件中对应2个32位数据，第一个32位数据高16位填充跳转指令低16位，其它16位保留原值;
                        table_content.append(ctypes.c_uint32(bitcodes[0] << 16).value + (ctypes.c_uint32(bin_content[0] << 16).value >> 16))
                        # 第二个32位数据低16位填充跳转指令高16位，其它16位保留原值
                        table_content.append(((bin_content[1] >> 16) << 16) + (bitcodes[0] >> 16))
                elif align == 4:
                    if self.cmp_head_len <= addr_cmp_idx and addr_tbl_idx < len(table_content):
                        # 已存在TBL中，修改TBL
                        table_content[addr_tbl_idx + 1] = bitcodes[0]
                    else:
                        # 不存在TBL中，添加TBL
                        # 原函数地址4字节对齐时TBL文件中对应64位数据高32位填充跳转指令低32位保留原值
                        table_content.append(bin_content[0])
                        table_content.append(bitcodes[0])
                elif align == 0:
                    if self.cmp_head_len <= addr_cmp_idx and addr_tbl_idx < len(table_content):
                        # 已存在TBL中，修改TBL
                        table_content[addr_tbl_idx] = bitcodes[0]
                    else:
                        # 不存在TBL中，添加TBL
                        # 原函数地址0字节对齐时TBL文件中对应64位数据低32位填充跳转指令高32位保留原值
                        table_content.append(bitcodes[0])
                        table_content.append(bin_content[1])
                else:
                    print(f"Error: instruction illegal alignment, Need to check dis, Patch Failed !")
                    sys.exit(1)

        table_count = len(table_content)
        word_per_reg = self.table_reg_size / 4
        if table_count > self.reg_num * word_per_reg:  # 128个比较表
            print("Error: TABLE Packet is larger than CMP Reg Capacitance")
            sys.exit(1)
        while table_count < self.reg_num * word_per_reg:
            table_content.append(0)
            table_count += 1

        return table_content

    # 将跳转表带入比较表对应的rom地址，得到打了patch后的rom_bin
    def get_patched_rom_bin(self, cmp_content:list, tbl_content):
        patched_rom_bin = bytearray(self.rom_bin)
        for i, cmp_addr in enumerate(cmp_content[self.cmp_head_len:]):
            addr = (cmp_addr & ~1) - self.rom_addr
            if self.version == "Version1" or self.jump_version == "Long":
                tbl_index = (i) * 2
                if tbl_content[tbl_index] == 0 and tbl_content[tbl_index+1] == 0:
                    break
                tbl_bytes = struct.pack('<II', tbl_content[tbl_index], tbl_content[tbl_index+1])
            else:
                tbl_index = (i)
                if tbl_content[tbl_index] == 0:
                    break
                tbl_bytes = struct.pack('<I', tbl_content[tbl_index])

            for j in range(len(tbl_bytes)):
                patched_rom_bin[addr + j] = tbl_bytes[j]
        return bytes(patched_rom_bin)

    # 通过源函数和patch函数计算出跳转指令，替换原函数地址所在的指令，得出预期的rom_bin
    def get_wanted_patched_rom_bin(self, func_addrs, patch_func_addrs):
        patched_rom_bin = bytearray(self.rom_bin)
        for i in range(len(func_addrs)):
            if self.jump_version == "Short":
                jump_code = struct.pack('<I', self.get_jump_bitcode_for_short_jump(func_addrs[i], patch_func_addrs[i])[0])
            else:
                jump_code = self.get_jump_bitcode_for_long_jump(func_addrs[i], patch_func_addrs[i])
                jump_code = struct.pack('<2I', jump_code[0], jump_code[1])
            addr = func_addrs[i] - self.rom_addr
            for j in range(len(jump_code)):
                patched_rom_bin[addr + j] = jump_code[j]
        return bytes(patched_rom_bin)

    def check_patch_valide(self, func_addrs, patch_func_addrs, cmp_content, tbl_content):
        patched_rom_bin = self.get_patched_rom_bin(cmp_content, tbl_content)
        wanted_patched_rom_bin = self.get_wanted_patched_rom_bin(func_addrs, patch_func_addrs)

        if patched_rom_bin != wanted_patched_rom_bin:
            print("Patch ERROR! patch result is not as excepted")
            self.remove_bin_file()
            sys.exit(1)

    def generate_patch(self):
        old_func_names = self.patch_function.keys()
        new_func_names = self.patch_function.values()
        # 不存在patch函数, 则不执行后续脚本
        if len(new_func_names) == 0:
            print("Patch INFO! no patch func")
            return

        old_func_addrs = self.get_func_addr(old_func_names, False)
        new_func_addrs = self.get_func_addr(new_func_names, True)

        self.cmp_content = self.get_cmp_content(old_func_addrs)
        self.tbl_content = self.get_table_content(old_func_addrs, new_func_addrs, self.cmp_content)
        self.check_patch_valide(old_func_addrs, new_func_addrs, self.cmp_content, self.tbl_content)
        self.create_txt_file('Patch.txt')
        self.create_patch_file(old_func_names, new_func_names, 'Patch.list')
        # 保留未打patch的原文件
        self.copy_bin_file(self.patch_file, "unpatch.bin")
        self.copy_bin_file(self.patch_file, "RW.bin")
        self.merge_output_file()
        self.copy_bin_file("RW.bin", self.patch_file)
        self.remove_bin_file()

if __name__ == "__main__":
    patch_ini = sys.argv[1]
    nm_file = sys.argv[2]
    patch_file = sys.argv[3]
    rom_file = sys.argv[4]

    # 使用示例
    patch_generator = PatchGenerator(nm_file, patch_ini, rom_file, patch_file)
    patch_generator.generate_patch()