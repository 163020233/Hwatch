#===================================================================================
# @brief   Python script
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===================================================================================
#!/usr/bin/env python3
# coding=utf-8

import struct

# 宏定义
ADDR_OFFSET_SIZE = 0x8000
MAX_DEEP_TRACE = 50

# 结构体定义
class UserFrame:
    def __init__(self):
        self.pc = 0
        self.ps = 0
        self.sar = 0
        self.vpri = 0
        self.a2 = 0
        self.a3 = 0
        self.a4 = 0
        self.a5 = 0
        self.exccause = 0
        self.lcount = 0
        self.lbeg = 0
        self.lend = 0

    def unpack(self, data):
        self.pc, self.ps, self.sar, self.vpri, \
        self.a2, self.a3, self.a4, self.a5, \
        self.exccause, self.lcount, self.lbeg, self.lend = struct.unpack("<12I", data)

class DumpInfo:
    def __init__(self):
        self.uf_size = 0
        self.cause = 0
        self.uf = UserFrame()

    def unpack(self, data):
        self.uf_size, self.cause = struct.unpack("<2I", data[:8])
        self.uf.unpack(data[8:])  # UserFrame 占 48 字节 (12 * 4)

class TraceInfo:
    def __init__(self):
        self.cur_ra = 0
        self.cur_sp = 0
        self.val_sp = 0

    def unpack(self, data):
        self.cur_ra, self.cur_sp, self.val_sp = struct.unpack("<3I", data)

class PanicInfo:
    def __init__(self):
        self.epc = 0
        self.depc = 0
        self.exc_cause = 0
        self.exc_vaddr = 0
        self.dump = DumpInfo()
        self.trace = [TraceInfo() for _ in range(MAX_DEEP_TRACE)]
        self.trace_count = 0

    def unpack(self, data):
        self.epc, self.depc, self.exc_cause, self.exc_vaddr = struct.unpack("<4I", data[:16])
        self.dump.unpack(data[16:16+56])  # DumpInfo 占 56 字节 (8 + 48)

        offset = 72  # 16 + 56
        for i in range(MAX_DEEP_TRACE):
            ti_data = data[offset:offset+12]  # 每个 TraceInfo 占 12 字节
            self.trace[i].unpack(ti_data)
            offset += 12

        self.trace_count = struct.unpack("<I", data[offset:offset+4])[0]

# 全局变量
g_in_file = "dsp_dtcm.bin"
g_out_file = "core_panic.txt"
g_panic_info = PanicInfo()

def main():
    info = g_panic_info

    try:
        with open(g_in_file, "rb") as in_file:
            # 获取文件长度
            in_file.seek(0, 2)
            file_len = in_file.tell()
            in_file.seek(0)

            # 判断文件长度是否足够
            info_size = 16 + 8 + 48 + MAX_DEEP_TRACE * 12 + 4  # 16 + 8 + 48 + 600 + 4 = 676
            if file_len < (ADDR_OFFSET_SIZE + info_size):
                print("File size is too small.\n")
                return -2

            # 跳过偏移并读取 panic info 数据
            in_file.seek(ADDR_OFFSET_SIZE)
            data = in_file.read(info_size)
            info.unpack(data)

        trace_count = min(info.trace_count, MAX_DEEP_TRACE)

        with open(g_out_file, "w") as out_file:
            # 写入输出文件
            out_file.write("***Core Panic***\n")
            out_file.write(f"epc=0x{info.epc:x}; depc=0x{info.depc:x}\n")
            out_file.write(f"exccause: {info.exc_cause}; excvaddr=0x{info.exc_vaddr:x}\n")

            out_file.write("/***Exception Dump Beg***/\n")
            out_file.write(f"sizeof(*uf) {info.dump.uf_size}; cause {info.dump.cause}\n")
            out_file.write(f"pc=0x{info.dump.uf.pc:x}; ps=0x{info.dump.uf.ps:x}; sar=0x{info.dump.uf.sar:x} vpri=0x{info.dump.uf.vpri:x}\n")
            out_file.write(f"a2=0x{info.dump.uf.a2:x}; a3=0x{info.dump.uf.a3:x}; a4=0x{info.dump.uf.a4:x}; a5=0x{info.dump.uf.a5:x}\n")
            out_file.write(f"lcount=0x{info.dump.uf.lcount:x}; lbeg=0x{info.dump.uf.lbeg:x} lend=0x{info.dump.uf.lend:x}\n")
            out_file.write("/***Exception Dump End***/\n")

            out_file.write("/***Trace Beg***/\n")
            for i in range(trace_count):
                ti = info.trace[i]
                out_file.write(f"trace{i} ra:0x{ti.cur_ra:x} sp:0x{ti.cur_sp:x}, 0x{ti.val_sp:x}\n")
            out_file.write("/***Trace End***/\n")

        with open(g_out_file, "r") as out_file:
            for line in out_file:
                print(line, end='')

    except Exception as e:
        print(f"Error: {e}\n")
        return -1

    return 0

if __name__ == "__main__":
    main()
    input("按回车退出...")
