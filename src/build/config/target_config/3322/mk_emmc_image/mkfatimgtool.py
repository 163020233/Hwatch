#===============================================================================
# @brief   Python script
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
#!/usr/local/bin/python3.7
# -*- coding: utf-8 -*-

import os
import sys
import time
import struct
import shutil
from distutils.dir_util import copy_tree

"""
介绍：
    本脚本主要实现调用工具生成文件镜像（支持 Windows 和 Linux）
    支持一次性生成多个分区镜像（目前支持 5 个分区）
    并自动将 DSP 和 BT 二进制文件复制到 user 分区中。
    所有生成的 .bin 镜像文件统一输出到 out_path 目录。

    支持：
      - Windows：使用 mkfatimg.exe
      - Linux：使用 dd + mkfs.vfat + 挂载方式（需 sudo）

    依赖（Linux）：
        sudo apt-get update
        sudo apt-get install dosfstools
    请注意使用本脚本时支持的python版本，本脚本只支持python3版本。
    请根据实际情况配置对应信息，主要是emmc的信息，以及分区信息

"""

cur_dir = os.path.dirname(os.path.abspath(__file__))

# 是否开启裁剪(删除末尾全零扇区，只保留有效数据)
ENABLE_TRIMMING = False
# 是否启用拼接生成 file.bin 
ENABLE_BUILD_EMMC_IMAGE = True  # 默认开启

root_path = os.path.join(cur_dir, '..', '..', '..', '..', '..')
root_path = os.path.abspath(root_path)  # 确保是绝对路径

out_path = os.path.join(cur_dir)

tools_bin_path = os.path.join(root_path, 'tools', 'pkg', 'bin', '3322')
dsp_bin_path = os.path.join(tools_bin_path, 'dsp', 'normal')      # 源：dsp/normal/
bt_bin_path = os.path.join(tools_bin_path, 'bt_signed.bin')       # 源：bt_signed.bin
gnss_bin_path = os.path.join(tools_bin_path, 'gnss', 'firmwares')
nfc_bin_path = os.path.join(tools_bin_path, 'nfc')

target_user_dir = os.path.join(cur_dir, 'fs', 'boot')             # 源：fs/user/
target_dsp_dir = os.path.join(target_user_dir, 'dsp')             # 源：fs/user/dsp/
target_bt_dir = os.path.join(target_user_dir, 'bt')               # 源：fs/user/bt/

mkfatimgtool = os.path.join(cur_dir, 'mkfatimg.exe')

cfg_sector_size = 512  # 扇区大小（字节）

# 【核心】多分区配置列表
# 结构：[源路径, 分区大小(KB), 输出文件名]
partition_list = [
    [os.path.join(cur_dir, 'fs', 'boot'), "6000", "file_boot.bin"],   # boot, 分区大小不能大于6000KB
    [os.path.join(cur_dir, 'fs', 'system'), "1572833", "file_system.bin"],   # system, 分区大小不能大于1572833KB
    [os.path.join(cur_dir, 'fs', 'inner'), "131072", "file_inner.bin"],     # inner, 分区大小不能大于131072KB
    [os.path.join(cur_dir, 'fs', 'systembk'), "262144", "file_systembk.bin"],   # systembk, 分区大小不能大于262144KB
    [os.path.join(cur_dir, 'fs', 'user'), "263000", "file_user.bin"],       # user, 分区大小不能大于emmc剩余大小
]

# 自动复制 DSP/BT 文件
def copy_dsp_and_bt_files():
    os.makedirs(target_dsp_dir, exist_ok=True)
    os.makedirs(target_bt_dir, exist_ok=True)

    file_mappings = [
        (os.path.join(dsp_bin_path, 'dsp_main.bin'), os.path.join(target_dsp_dir, 'dsp_main.bin')),
        (os.path.join(dsp_bin_path, 'dsp_overlay.bin'), os.path.join(target_dsp_dir, 'dsp_overlay.bin')),
        (bt_bin_path, os.path.join(target_bt_dir, 'bt_signed.bin')),
        (os.path.join(gnss_bin_path, 'gnss_config.bin'), os.path.join(cur_dir, 'fs', 'boot', 'gnss')),
        (os.path.join(nfc_bin_path, 'fw.img'), os.path.join(cur_dir, 'fs', 'boot', 'nfc')),
    ]

    all_copied = True
    for src, dst in file_mappings:
        if not os.path.exists(src):
            print(f"源文件不存在：{src}")
            all_copied = False
            continue

        shutil.copy2(src, dst)  # copy2 保留元数据

    if all_copied:
        print("DSP 和 BT 文件全部复制成功！")
    else:
        print("有部分文件未找到，但将继续生成镜像（可能导致镜像缺失关键文件）")

def copy_res_files():
    res_path = os.path.join(root_path, 'application', 'wearable', 'res')

    target_res_dir = os.path.join(cur_dir, 'fs', 'user', 'res')
    target_logo_dir = os.path.join(cur_dir, 'fs', 'boot', 'data')

    if os.path.exists(target_res_dir):
        shutil.rmtree(target_res_dir)

    os.makedirs(target_res_dir, exist_ok=True)
    os.makedirs(target_logo_dir, exist_ok=True)

    if not os.path.exists(res_path):
        print(f"警告：res 资源目录不存在！路径：{res_path}")
        return

    resource_mapping = [
        (os.path.join('res'), os.path.join(cur_dir, 'fs', 'user', 'res')),
        (os.path.join('music'), os.path.join(cur_dir, 'fs', 'user', 'music')),
        (os.path.join('video'), os.path.join(cur_dir, 'fs', 'user', 'video')),
        (os.path.join('dial'), os.path.join(cur_dir, 'fs', 'user', 'dial')),
        (os.path.join('dial', '466x466'), os.path.join(cur_dir, 'fs', 'user', 'dial')),
    ]
    for resource_dir, target_path in resource_mapping:
        source_path = os.path.join(root_path, 'application', 'wearable', 'res', resource_dir)
        if not os.path.exists(source_path):
            print(f"Source path {source_path} does not exist. Skipping.")
            continue
        if os.path.exists(target_path):
            shutil.rmtree(target_path)
        os.makedirs(target_path, exist_ok=True)
        copy_tree(source_path, target_path)

    logo_files = {
        'OPEN_LOGO.bin': os.path.join(res_path, 'logo', 'OPEN_LOGO.bin'),
        'OPEN_LOGO_QSPI.bin': os.path.join(res_path, 'logo', 'OPEN_LOGO_QSPI.bin')
    }

    for name, src in logo_files.items():
        if os.path.exists(src):
            dst = os.path.join(target_logo_dir, name)
            shutil.copyfile(src, dst)
        else:
            print(f"Logo 文件不存在，跳过：{src}")

    print("所有 res 资源文件复制完成！")

    # 字体文件映射关系（源文件名 -> 目标文件名）
    font_rename_mapping = {
        "HarmonyOS_Sans_SC.ttf": "HarmonyOS_Sans_SC_Regular.ttf",
        "HarmonyOS_Sans_Naskh_Arabic_UI.ttf": "HarmonyOS_Sans_Naskh_Arabic_Light.ttf",
        "HarmonyOS_Sans_Italic.ttf": "HarmonyOS_Sans_Light_Italic.ttf",
        "HarmonyOS_Sans.ttf": "HarmonyOS_Sans_Regular.ttf"
    }

    fonts_src_dir = os.path.join(root_path, 'ohos', 'base', 'global', 'system_resources', 'fonts')
    fonts_dst_dir = os.path.join(cur_dir, 'fs', 'user', 'res')


    for src_file in os.listdir(fonts_src_dir):
        src_path = os.path.join(fonts_src_dir, src_file)
    
        if not src_file.lower().endswith('.ttf') or not os.path.isfile(src_path):
            continue

        # 查看是否在映射表中
        if src_file in font_rename_mapping:
            dst_filename = font_rename_mapping[src_file]
            dst_path = os.path.join(fonts_dst_dir, dst_filename)
        
            shutil.copyfile(src_path, dst_path)
            print(f"字体文件已成功复制并重命名: {src_path} -> {dst_path}")

    print("所有字体文件复制完成（含重命名）！")


    # 新增路径变更文件的复制
    path_change_mapping = [
        (os.path.join(root_path, 'ohos', 'foundation', 'arkui', 'ui_lite', 'tools', 'qt', 'simulator', 'config', 'faces', 'default', 'B024_002.bin'), os.path.join(cur_dir, 'fs', 'user', 'res', 'B024_002.bin')),
        (os.path.join(root_path, 'ohos', 'foundation', 'arkui', 'ui_lite', 'tools', 'qt', 'simulator', 'config', 'images', 'blue_rgb888.bin'), os.path.join(cur_dir, 'fs', 'user', 'res', 'blue_rgb888.bin')),
        (os.path.join(root_path, 'ohos', 'foundation', 'arkui', 'ui_lite', 'tools', 'qt', 'simulator', 'font', 'line_cj.brk'), os.path.join(cur_dir, 'fs', 'user', 'res', 'line_cj.brk')),
        (os.path.join(root_path, 'ohos', 'base', 'global', 'i18n_lite', 'frameworks', 'i18n', 'i18n.dat'), os.path.join(cur_dir, 'fs', 'user', 'res', 'i18n.dat')),
        (os.path.join(root_path, 'ohos', 'foundation', 'arkui', 'ui_lite', 'tools', 'qt', 'simulator', 'config', 'images', 'image_animator_0.bin'), os.path.join(cur_dir, 'fs', 'user', 'res', 'image_animator_1.bin')),
        (os.path.join(root_path, 'ohos', 'foundation', 'arkui', 'ui_lite', 'tools', 'qt', 'simulator', 'config', 'images', 'image_animator_1.bin'), os.path.join(cur_dir, 'fs', 'user', 'res', 'image_animator_2.bin')),
        (os.path.join(root_path, 'ohos', 'foundation', 'arkui', 'ui_lite', 'tools', 'qt', 'simulator', 'config', 'images', 'image_animator_2.bin'), os.path.join(cur_dir, 'fs', 'user', 'res', 'image_animator_3.bin')),
        (os.path.join(root_path, 'ohos', 'foundation', 'arkui', 'ui_lite', 'tools', 'qt', 'simulator', 'config', 'images', 'image_animator_3.bin'), os.path.join(cur_dir, 'fs', 'user', 'res', 'image_animator_4.bin')),
    ]

    for src_path, dst_path in path_change_mapping:
        if os.path.exists(src_path):
            dst_dir = os.path.dirname(dst_path)
            if not os.path.exists(dst_dir):
                os.makedirs(dst_dir)
            shutil.copyfile(src_path, dst_path)
            print(f"路径变更文件已成功复制: {src_path} -> {dst_path}")
        else:
            print(f"路径变更文件不存在: {src_path}")

# 裁剪镜像函数（删除末尾全零扇区）
def trim_image(image_file, output_file, sector_size=512):
    if not os.path.exists(image_file):
        print(f"裁剪失败：输入镜像文件不存在！{image_file}")
        return False

    file_size = os.path.getsize(image_file)
    if file_size == 0:
        print(f"裁剪成功：输入文件为空，输出空文件。")
        open(output_file, 'wb').close()
        return True

    sector_count = file_size // sector_size
    remainder = file_size % sector_size

    if remainder != 0:
        print(f"警告：镜像文件大小 ({file_size} bytes) 不是扇区大小 ({sector_size}) 的整数倍，可能存在问题。")
        sector_count -= 1
        file_size = sector_count * sector_size

    with open(image_file, 'rb') as f:
        for i in range(sector_count - 1, -1, -1):
            f.seek(i * sector_size)
            sector_data = f.read(sector_size)
            if any(b != 0 for b in sector_data):
                effective_size = (i + 1) * sector_size
                break
        else:
            print("警告：整个镜像文件都是全零扇区，输出空文件。")
            effective_size = 0

    if effective_size == 0:
        with open(output_file, 'wb') as f:
            pass
        print(f"裁剪成功：输出空镜像文件 → {output_file}")
    else:
        with open(image_file, 'rb') as f_in, open(output_file, 'wb') as f_out:
            f_in.seek(0)
            data = f_in.read(effective_size)
            f_out.write(data)
        print(f"裁剪成功：输出裁剪后镜像文件 → {output_file}")

    return True

# 拼接多个 .bin 文件为 file.bin
def generate_combined_file():
    output_file = os.path.join(out_path, "file.bin")

    files_list = []
    for i, (src_path, part_size_kb, output_filename) in enumerate(partition_list):
        file_path = os.path.join(out_path, output_filename)
        max_length = int(part_size_kb) * 1024  # 转换为字节

        should_pad = (output_filename != "file_user.bin")

        files_list.append({
            'file_path': file_path,
            'max_length': max_length,
            'should_pad': should_pad
        })

    combined_data = bytearray()

    for file_info in files_list:
        file_path = file_info['file_path']
        max_length = file_info['max_length']
        should_pad = file_info['should_pad']

        try:
            with open(file_path, 'rb') as f:
                data = f.read()
                current_length = len(data)

                if should_pad:
                    if current_length < max_length:
                        pad_length = max_length - current_length
                        data += bytes([0x00] * pad_length)
                        print(f"{os.path.basename(file_path)}: 填充 {pad_length} 字节 (0x00) → 总大小: {current_length + pad_length}B")
                    else:
                        print(f"{os.path.basename(file_path)}: 文件大小 {current_length}B ≥ max_length {max_length}B，无需填充")
                else:
                    if current_length > max_length:
                        print(f"严重警告：{os.path.basename(file_path)} 文件大小 {current_length}B > 配置最大长度 {max_length}B！")
                        print(f"可能导致镜像溢出，建议检查源文件并重新生成！")
                        sys.exit(1)
                    print(f"{os.path.basename(file_path)}: 原样读取，大小: {current_length}B")

                combined_data.extend(data)

        except FileNotFoundError:
            print(f"Build fail! 文件不存在: {file_path}，请先生成该文件！")
            sys.exit(1)

    with open(output_file, 'wb') as f:
        f.write(combined_data)

    total_size = len(combined_data)
    print(f"\n 生成成功！最终镜像: {output_file}")
    print(f"总大小: {total_size:,} 字节 ({(total_size / (1024**2)):.2f} MB)")

    target_output_dir = os.path.join(root_path, 'output', '3322')
    os.makedirs(target_output_dir, exist_ok=True)

    final_file_bin = os.path.join(target_output_dir, "file.bin")
    try:
        shutil.copy2(output_file, final_file_bin)
        print(f"已将最终镜像 file.bin 复制到目标目录：{final_file_bin}")
    except Exception as e:
        print(f"复制 file.bin 到 {target_output_dir} 失败：{e}")
        sys.exit(1)

    return True

def main():
    # 在生成任何镜像前，先复制文件
    copy_dsp_and_bt_files()
    copy_res_files()

    for src_path, part_size_kb, output_filename in partition_list:
        if not os.path.exists(src_path):
            os.makedirs(src_path, exist_ok=True)
        if len(os.listdir(src_path)) == 0:
            placeholder_file = os.path.join(src_path, "empty.txt")
            with open(placeholder_file, 'w', encoding='utf-8') as f:
                f.write(f"# This is a placeholder file for empty partition: {output_filename}\n")
                f.write(f"# Generated by mkfatimgtool.py on {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
            print(f"目录 {src_path} 为空，已创建占位文件：empty.txt")

    if sys.platform == 'win32':
        if not os.path.exists(mkfatimgtool):
            print(f"错误：未找到 mkfatimg.exe 工具！路径：{mkfatimgtool}")
            sys.exit(1)

        for i, partition in enumerate(partition_list):
            part_path, part_size_kb, output_filename = partition
            image_file = os.path.join(out_path, output_filename)

            sector_size = " 4096"

            if not os.path.exists(part_path):
                print(f"错误：分区 {i+1} 的源路径不存在！{part_path}")
                continue

            if os.path.exists(image_file):
                os.remove(image_file)

            command = f"{mkfatimgtool} \"{part_path}\" \"{image_file}\" {part_size_kb} {sector_size}"
            ret = os.system(command)

            if ret == 0:
                print(f"分区 {i+1} 生成成功：{output_filename}")
                if ENABLE_TRIMMING:
                    trimmed_file = os.path.join(out_path, f"{os.path.splitext(output_filename)[0]}_trimmed.bin")
                    trim_image(image_file, trimmed_file)

                    os.replace(trimmed_file, image_file)
                    print(f"已将裁剪后镜像覆盖为：{image_file}")
            else:
                print(f"分区 {i+1} 生成失败：{output_filename}")
        
        if ENABLE_BUILD_EMMC_IMAGE:
            print("\n正在拼接多个分区镜像为 file.bin...")
            generate_combined_file()
            print("\n所有镜像生成完成！") 

    elif sys.platform.startswith('linux'):
        ret = os.system("which mkfs.vfat > /dev/null 2>&1")
        if ret != 0:
            print("错误：未安装 dosfstools，请运行：")
            print("    sudo apt-get update && sudo apt-get install dosfstools")
            sys.exit(1)

        if os.path.exists("fs_temp_mount"):
            print("清理上一次残留的临时挂载点...")
            for root, dirs, files in os.walk("fs_temp_mount"):
                for d in dirs:
                    full_path = os.path.join(root, d)
                    if os.path.ismount(full_path):
                        print(f"强制卸载残留挂载点：{full_path}")
                        os.system(f"sudo umount -f {full_path}")
            shutil.rmtree("fs_temp_mount", ignore_errors=True)

        for i, partition in enumerate(partition_list):
            part_path, part_size_kb, output_filename = partition
            image_file = os.path.join(out_path, output_filename)
            total_size_kb = int(part_size_kb)

            if not os.path.exists(part_path):
                print(f"错误：分区 {i+1} 的源路径不存在！{part_path}")
                continue

            if os.path.exists(image_file):
                os.remove(image_file)

            count = int(total_size_kb * 2)

            command = f"dd if=/dev/zero of={image_file} bs=512 count={count}"
            ret = os.system(command)
            if ret != 0:
                print(f"dd 命令失败，无法创建镜像文件：{image_file}")
                continue

            command = f"mkfs.vfat -F 32 -S 512 -s 8 {image_file}"
            ret = os.system(command)
            if ret != 0:
                print(f"mkfs.vfat 格式化失败！")
                continue

            mount_point = os.path.join("fs_temp_mount", f"part_{i+1}")
            os.makedirs(mount_point, exist_ok=True)

            command = f"sudo mount -o loop,uid=$(id -u),gid=$(id -g) {image_file} {mount_point}"
            ret = os.system(command)
            if ret != 0:
                print(f"挂载失败！请检查权限。")
                continue

            command = f"sudo cp -r {part_path}/. {mount_point}/"
            ret = os.system(command)
            if ret != 0:
                print(f"拷贝文件失败！")
                continue

            command = f"sudo umount {mount_point}"
            ret = os.system(command)
            if ret != 0:
                print(f"首次卸载失败，尝试强制卸载：sudo umount -f {mount_point}")
                command = f"sudo umount -f {mount_point}"
                ret = os.system(command)
                if ret != 0:
                    print(f"强制卸载失败！请手动执行：")
                    print(f"    sudo umount -f {mount_point}")

            try:
                if os.path.ismount(mount_point):
                    print(f"警告：{mount_point} 仍被挂载，尝试强制卸载...")
                    os.system(f"sudo umount -f {mount_point}")

                if os.path.ismount(mount_point):
                    print(f"警告：{mount_point} 仍然被挂载，无法删除！")
                else:
                    shutil.rmtree(mount_point)
            except Exception as e:
                print(f"删除临时目录失败：{e}")

            if ret == 0:
                print(f"分区 {i+1} 生成成功：{output_filename}")

                if ENABLE_TRIMMING:
                    trimmed_file = os.path.join(out_path, f"{os.path.splitext(output_filename)[0]}_trimmed.bin")
                    trim_image(image_file, trimmed_file)
                    os.replace(trimmed_file, image_file)   

            else:
                print(f"分区 {i+1} 生成失败：{output_filename}")

        if ENABLE_BUILD_EMMC_IMAGE:
            print("\n正在拼接多个分区镜像为 file.bin...")
            generate_combined_file()
            print("\n所有镜像生成完成！") 

        if os.path.exists("fs_temp_mount"):
            try:
                for root, dirs, files in os.walk("fs_temp_mount"):
                    for d in dirs:
                        full_path = os.path.join(root, d)
                        if os.path.ismount(full_path):
                            print(f"清理残留挂载点：{full_path}")
                            os.system(f"sudo umount -f {full_path}")
                shutil.rmtree("fs_temp_mount")
                print("fs_temp_mount 目录已成功清理。")
            except Exception as e:
                print(f"清理 fs_temp_mount 失败：{e}")

    else:
        print(f"不支持当前平台：{sys.platform}")
        print("仅支持 Windows 和 Linux 系统！")
        sys.exit(1)

if __name__ == "__main__":
    main()
