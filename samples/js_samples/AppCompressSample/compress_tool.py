#!/usr/bin/env python3
"""
文件压缩工具 - 支持 zlib、lz4、zip 三种压缩算法
用法示例:
    python compress_tool.py input.txt output.bin --method zlib
    python compress_tool.py input.txt output.lz4 --method lz4
    python compress_tool.py input.txt output.zip --method zip
"""

import sys
import os
import argparse
import zlib
import zipfile

# 尝试导入可选压缩库
try:
    import lz4.frame
except ImportError:
    lz4 = None

def compress_zlib(data: bytes) -> tuple:
    """使用 zlib 压缩，返回 (状态码, 压缩后数据或错误信息)"""
    try:
        compressed = zlib.compress(data, level=9)
        return 0, compressed
    except Exception as e:
        return -1, str(e)


def compress_lz4(data: bytes) -> tuple:
    """使用 lz4 压缩，返回 (状态码, 压缩后数据或错误信息)"""
    if lz4 is None:
        return -1, "lz4 not installed."
    try:
        compressed = lz4.frame.compress(data)
        return 0, compressed
    except Exception as e:
        return -1, str(e)

def compress_zip(file_path, zip_name):
    try:
        # 检查源文件是否存在
        if not os.path.exists(file_path):
            return -1, "file_path not exist"
        # 确保输出目录存在？zipfile会创建父目录吗？需要手动创建
        zip_dir = os.path.dirname(zip_name)
        if zip_dir and not os.path.exists(zip_dir):
            os.makedirs(zip_dir, exist_ok=True)
        with zipfile.ZipFile(zip_name, 'w', zipfile.ZIP_DEFLATED) as zf:
            zf.write(file_path, arcname=os.path.basename(file_path))
        return 0, "compress zip success"
    except Exception as e:
        return -1, "compress_zip : {str(e)}"


def main() -> int:
    parser = argparse.ArgumentParser(
        description="success 0，fail -1"
    )
    parser.add_argument("input_file", help="input_file")
    parser.add_argument("output_file", help="output_file")
    parser.add_argument(
        "--method", "-m",
        choices=["zlib", "lz4", "zip"],
        default="zip",
    )
    args = parser.parse_args()

    # zip 单独处理，不操作文件
    if args.method == "zip":
        status, msg = compress_zip(args.input_file, args.output_file)
        if status == 0:
            print(msg)
            return 0
        else:
            print(f"Error: {msg}")
            return -1

    # 检查输入文件
    if not os.path.exists(args.input_file):
        print(f"inputfile '{args.input_file}' not exist")
        return -1

    # 读取原始数据
    try:
        with open(args.input_file, "rb") as f:
            original_data = f.read()
    except Exception as e:
        print(f"read inputfile: {e}")
        return -1

    # 执行压缩
    if args.method == "zlib":
        status, result = compress_zlib(original_data)
    elif args.method == "lz4":
        status, result = compress_lz4(original_data)
    else:
        print(f"Unsupported compress method.: {args.method}")
        return -1

    if status != 0:
        print(f"compress fail: {result}")
        return -1

    compressed_data = result

    # 写入输出文件
    try:
        with open(args.output_file, "wb") as f:
            f.write(compressed_data)
    except Exception as e:
        print(f"fail write outputfile : {e}")
        return -1

    return 0

if __name__ == "__main__":
    sys.exit(main())
