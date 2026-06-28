#===============================================================================
# @brief   Python script
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
import argparse
import os

def get_temp_output(path):
    rets = ''
    if not os.path.isfile(path):
        print(f"File not found: {path}")
        return path
    with open(path, 'r') as file:
        for line in file:
            line = line.replace(" ", "") 
            if line.startswith("DstFile"):
                # 提取 "DstFile=" 后面的部分
                rets = line.split("=", 1)[1].strip() 
    return rets

def merge_files(path1, path2):
    if os.path.exists(path1): # build/config/target_config/3322/sign_config/root_pubk.cfg
        key_file_path = get_temp_output(path1)
    else:
        empty_pubk = bytearray(128)
        os.makedirs(os.path.join('output', '3322', 'acore', '3322-loaderboot'), exist_ok=True)
        key_file_path = os.path.join('output', '3322', 'acore', '3322-loaderboot', 'public_root_pubk.bin')
        with open(key_file_path, 'wb') as f:
            f.write(empty_pubk)
        f.close()
    loader_file_path = get_temp_output(path2)
    print(key_file_path)
    # 定义要合并的文件路径
    output_path = os.path.join(os.path.dirname(loader_file_path), 'loaderboot_signed.bin')
    with open(output_path, 'wb') as output_file:
        with open(key_file_path, 'rb') as file1:
            output_file.write(file1.read())
        with open(loader_file_path, 'rb') as file2:
            output_file.write(file2.read())


def main():
    parser = argparse.ArgumentParser(description="Read lines starting with 'Test' from two files.")
    parser.add_argument("path1", help="The path to the key config file")
    parser.add_argument("path2", help="The path to the loader config file")
    args = parser.parse_args()
    merge_files(args.path1, args.path2)

if __name__ == "__main__":
    main()
