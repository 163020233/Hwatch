#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    Build Update Pkg File
# Copyright HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
# ============================================================================

import os
import sys
import shutil
import filecmp
import argparse
import configparser
from typing import Dict, Set, List

# 使用此脚本生成升级包，请注意修改以下几点：
#   1. 3322-native-js.fwpkg分别存放在upgrade/new_version和upgrade/old_version
#      资源文件分别存放在upgrade/new_version/res和upgrade/old_version/res
#       upgrade目录层次如下
#       ├── new_version
#       │   ├── res
#       │   │   └── user
#       │   └── 3322-native-js.fwpkg
#       └── old_version
#           ├── res
#           │   └── user
#           └── 3322-native-js.fwpkg
#   2. 在get_new_image() 中对想要升级的镜像进行增减，仅打开想要升级的镜像即可
#   3. 脚本执行时，通过type参数改变升级方式
#                      --type=default  原始镜像不处理, 生成update_default.fwpkg
#                      --type=compress 镜像压缩升级, 生成update_compress.fwpkg
#                      --type=diff     镜像差分升级，生成update_diff.fwpkg
#      上板验证时，需要重命名成update.fwpkg
#   4. 升级的校验方式分为安全校验和非安全校验，详细修改方式请详见开发文档
#   5. 修改完成后，运行此脚本即可在默认路径(output/3322/upgrade)下生成升级包update_{type}.fwpkg

class CasePreservingConfigParser(configparser.ConfigParser):
    def optionxform(self, optionstr):
        return optionstr

def modify_config_value(config_file_path, section, option, new_value):
    config = CasePreservingConfigParser()
    config.read(config_file_path)
    if (section in config) and (option in config[section]):
        config[section][option] = str(new_value)
    with open(config_file_path, 'w') as configfile:
        config.write(configfile)

def get_project_root():
    file_dir = os.path.dirname(os.path.realpath(__file__))
    return os.path.abspath(os.path.join(file_dir, "..", "..", "..", ".."))

class upg_base_info:
    def __init__(self, root, upgrade_type='default'):
        self.root_path = root
        self.upgrade_type = upgrade_type
        self.base_config_path = os.path.join(self.root_path, "build", "config", "target_config", "3322")
        self.upgrade_dir = os.path.join(self.base_config_path, "upgrade")
        self.output_path = os.path.join(self.root_path, "output", "3322")
        self._init_paths()
        self._select_custom_fota_cfg()
        self.removed_dirs = []
        self.deleted_files = []

    def _init_paths(self):
        # 升级包结构配置文件
        self.fota_format_path = os.path.join(self.base_config_path, "fota")
        # 产品升级配置文件
        self.fota_cfg = os.path.join(self.fota_format_path, "fota.cfg")
        # 产品镜像输出路径
        self.output = self.output_path
        # 产品升级镜像包输出路径
        self.upg_output = os.path.join(self.output, "upgrade")
        # 产品升级制作临时文件输出路径
        self.temp_dir = os.path.join(self.upg_output, "temp_dir")
        # pkg、res存储目录
        self.new_version = os.path.join(self.upgrade_dir, "new_version")
        self.old_version = os.path.join(self.upgrade_dir, "old_version")
        self.res_dir = os.path.join(self.upgrade_dir, "res_file")
        self.res_index = os.path.join(self.upgrade_dir, "res_index.bin")
        self.res_data = os.path.join(self.upgrade_dir, "res_data.bin")

    def _select_custom_fota_cfg(self):
        CONFIG_MAP = {
            'compress': "fota_compress.cfg",
            'diff': "fota_diff.cfg",
            'default': "fota_default.cfg"
        }

        DECOMPRESS_FLAGS = {
            'default':  ('0',          ['ssb', 'recovery', 'application', 'bt', 'dsp_main', 'dsp_overlay']),
            'compress': ('0x3C7896E1', ['ssb', 'recovery', 'application', 'bt', 'dsp_main', 'dsp_overlay']),
            'diff':     ('0x44494646', ['application', 'bt', 'dsp_main', 'dsp_overlay'])
        }

        # 获取配置文件名，如果升级类型不在映射中，默认使用default
        config_filename = CONFIG_MAP.get(self.upgrade_type, CONFIG_MAP['default'])
        # 设置自定义配置文件路径
        self.custom_fota_cfg = os.path.join(self.upgrade_dir, config_filename)

        os.makedirs(os.path.dirname(self.custom_fota_cfg), exist_ok=True)
        shutil.copy2(self.fota_cfg, self.custom_fota_cfg)

        # 根据升级类型获取对应的解压标志和节区列表
        decompress_flag, sections = DECOMPRESS_FLAGS.get(self.upgrade_type, DECOMPRESS_FLAGS['default'])

        # 修改配置文件中的解压标志
        for section in sections:
            modify_config_value(self.custom_fota_cfg, section, 'DecompressFlag', decompress_flag)

    def _compare_files_from_offset(self, file1: str, file2: str, offset: int, chunk_size: int = 8192) -> bool:
        """
        分两段对比两个文件：
        - 偏移前部分（[0, offset)）：不一致时仅打印警告，继续
        - 偏移后部分（[offset, end)）：不一致时立即返回 False 并报错

        参数:
            file1 (str): 第一个文件路径
            file2 (str): 第二个文件路径
            offset (int): 开始对比的字节偏移(从 0 开始)
            chunk_size (int): 每次读取的字节数(默认 8KB)

        返回:
            bool:
                - 若偏移前不一致 → 返回 True(警告已打印)
                - 若偏移后不一致 → 返回 False(已打印错误)
        """
        if chunk_size <= 0:
            print(f"[ERROR] chunk_size 必须大于 0")
            return False

        try:
            with open(file1, 'rb') as f1, open(file2, 'rb') as f2:
                # 获取文件大小
                size1 = f1.seek(0, 2)
                size2 = f2.seek(0, 2)

                # 检查偏移是否超出范围
                if offset > size1 or offset > size2:
                    print(f"[WARNING] 偏移 {offset} 超出文件范围 "
                        f"({file1}: {size1}, {file2}: {size2})")
                    return False

                # -------------------------------
                # 段1：偏移前部分 [0, offset)
                # -------------------------------
                if offset > 0:
                    f1.seek(0)
                    f2.seek(0)

                    pos = 0
                    while pos < offset:
                        # 计算本次读取的最大长度
                        read_len = min(chunk_size, offset - pos)
                        chunk1 = f1.read(read_len)
                        chunk2 = f2.read(read_len)

                        # 如果一个文件提前结束 → 长度不一致
                        if not chunk1 and not chunk2:
                            break
                        if not chunk1 or not chunk2:
                            print(f"[WARNING] 文件长度在偏移前不一致 "
                                f"({file1}: {pos}, {file2}: {pos})")
                            break

                        # 内容不一致
                        if chunk1 != chunk2:
                            break

                        pos += read_len

                # ——————————————————
                # 段2：偏移后部分 [offset, end)
                # ——————————————————
                f1.seek(offset)
                f2.seek(offset)

                while True:
                    chunk1 = f1.read(chunk_size)
                    chunk2 = f2.read(chunk_size)

                    # 两个都读完 → 一致
                    if not chunk1 and not chunk2:
                        return True

                    # 一个读完，另一个没读完 → 长度不一致
                    if not chunk1 or not chunk2:
                        print(f"[ERROR] 文件长度在偏移后部分不一致 "
                            f"({file1}: {f1.tell()}, {file2}: {f2.tell()})")
                        return False

                    # 内容不一致
                    if chunk1 != chunk2:
                        return False

        except FileNotFoundError as e:
            print(f"[ERROR] 文件未找到: {e}")
            return False
        except PermissionError as e:
            print(f"[ERROR] 权限不足，无法读取文件: {e}")
            return False
        except OSError as e:
            print(f"[ERROR] 系统错误读取文件: {e}")
            return False
        except Exception as e:
            print(f"[ERROR] 未知异常导致文件对比失败: {e}")
            return False

    def _check_partition_table(self) -> bool:
        """
        检查新旧版本的partition_table.bin是否一致
        如果不一致返回False,一致返回True
        """
        old_pt = os.path.join(self.old_version, "pkg", "rootkey_partition_cfg.bin")
        new_pt = os.path.join(self.new_version, "pkg", "rootkey_partition_cfg.bin")

        # 检查文件是否存在
        if not os.path.exists(old_pt):
            print(f"[ERROR] 分区表校验失败: 旧版本分区表文件不存在")
            return False
        if not os.path.exists(new_pt):
            print(f"[ERROR] 分区表校验失败: 新版本分区表文件不存在")
            return False

        # 执行两段式比对文件内容
        if self._compare_files_from_offset(old_pt, new_pt, offset = 0x380):
            print("[INFO] 分区表校验通过: 新旧版本分区表一致")
            return True
        else:
            print(f"[ERROR] 分区表校验失败内容不一致")
            return False

    def _clean_directories(self):
        try:
            #清理pkg目录中的bin文件
            for version_dir in [self.new_version, self.old_version]:
                pkg_dir = os.path.join(version_dir, "pkg")
                if os.path.exists(pkg_dir):
                    shutil.rmtree(pkg_dir)
                    print(f"Cleaned up directory: {pkg_dir}")
            # 清理 res_file 目录
            if os.path.exists(self.res_dir):
                shutil.rmtree(self.res_dir)
                print(f"Cleaned up directory: {self.res_dir}")
            # 清理 res_index.bin 和 res_data.bin 文件
            for file_path in [self.res_index, self.res_data, self.custom_fota_cfg]:
                if os.path.exists(file_path):
                    os.remove(file_path)
                    print(f"Cleaned up file: {file_path}")
        except Exception as e:
            print(f"[WARNING] Failed to clean up directories: {e}")

    def _process_image_files(self):
        # 当前版本的镜像信息
        image_files = {
            'ssb': 'ssb_signed.bin',
            'recovery': 'recovery_signed.bin',
            'application': 'application_signed.bin',
            'bt': 'bt_signed.bin',
            'dsp_main': 'dsp_main.bin',
            'dsp_overlay': 'dsp_overlay.bin',
            'nv': '3322_all_nv.bin'
        }
        for name, bin_file in image_files.items():
            # 设置二进制文件路径
            new_bin_path = os.path.join(os.path.join(self.new_version, "pkg"), bin_file)
            old_bin_path = os.path.join(os.path.join(self.old_version, "pkg"), bin_file)
            if os.path.exists(new_bin_path) and  os.path.exists(old_bin_path):
                if not self._compare_files_from_offset(old_bin_path, new_bin_path, offset = 0x300):
                    setattr(self, f'{name}_bin', new_bin_path)

        # 差分升级时，上一个版本的镜像信息
        old_image_files = {
            'recovery': 'recovery_signed.bin',
            'application': 'application_signed.bin',
            'bt': 'bt_signed.bin',
            'dsp_main': 'dsp_main.bin',
            'dsp_overlay': 'dsp_overlay.bin'
        }

        for name, bin_file in old_image_files.items():
            # 设置二进制文件路径
            old_bin_path = os.path.join(os.path.join(self.old_version, "pkg"), bin_file)
            new_bin_path = os.path.join(os.path.join(self.new_version, "pkg"), bin_file)
            if os.path.exists(old_bin_path) and os.path.exists(new_bin_path):
                if not self._compare_files_from_offset(old_bin_path, new_bin_path, offset = 0x300):
                    setattr(self, f'{name}_old_bin', old_bin_path)

    def _split_pkg(self):
        try:
            for pkg_dir in [self.new_version, self.old_version]:
                pkg_path = os.path.join(pkg_dir, "pkg")
                if os.path.exists(pkg_path):
                    shutil.rmtree(pkg_path)
                os.makedirs(pkg_path)
                
                # 处理上传文件
                upload_path = os.path.join(pkg_dir, "3322-native-js.fwpkg")
                if os.path.exists(upload_path):
                    split_bin(upload_path, pkg_path)
        except Exception as e:
            print(f"[ERROR] Failed to prepare directories: {e}")
            sys.exit(1)

    def _compare_res_directories(self, old_res: str, new_res: str) -> Dict[str, List[str]]:
        """
        比较两个目录，返回差异文件和孤立文件
        
        返回结构:
        {
            'left_only': [文件路径列表],  # 只在dir1存在的文件
            'right_only': [文件路径列表], # 只在dir2存在的文件
            'diff_files': [文件路径列表],  # 内容不同的文件
        }
        """
        dcmp = filecmp.dircmp(old_res, new_res)
        result = {
            'left_only': [],
            'right_only': [],
            'diff_files': [],
        }

        # 处理当前目录的比较结果
        for name in dcmp.left_only:
            result['left_only'].append(os.path.join(old_res, name))
        for name in dcmp.right_only:
            result['right_only'].append(os.path.join(new_res, name))
        for name in dcmp.diff_files:
            result['diff_files'].append(os.path.join(new_res, name))

        # 递归处理子目录
        for sub_dcmp in dcmp.subdirs.values():
            sub_result = self._compare_res_directories(sub_dcmp.left, sub_dcmp.right)
            for key in result:
                result[key].extend(sub_result[key])

        return result

    def _copy_files_to_res_info_with_structure(self, results: Dict[str, List[str]], res_info_dir: str) -> None:
        def _copy_item(src: str, dst_dir: str) -> None:
            """递归拷贝文件或目录"""
            if os.path.isfile(src):
                # 如果是文件，直接拷贝
                file_name = os.path.basename(src)
                dst_path = os.path.join(dst_dir, file_name)
                shutil.copy2(src, dst_path)
                print(f"Copied: {src} -> {dst_path}")
            elif os.path.isdir(src):
                # 如果是目录，递归处理
                for item in os.listdir(src):
                    item_src = os.path.join(src, item)
                    item_dst = os.path.join(dst_dir, os.path.basename(src))
                    os.makedirs(item_dst, exist_ok=True)
                    _copy_item(item_src, item_dst)
            else:
                print(f"Warning: {src} 不是文件或目录，跳过")

        try:
            # 确保目标目录存在
            os.makedirs(res_info_dir, exist_ok=True)

            # 处理 right_only 文件/目录
            for item_path in results.get('right_only', []):
                try:
                    if not os.path.exists(item_path):
                        print(f"Warning: 源路径 {item_path} 不存在，跳过")
                        continue

                    # 获取相对于 new_version/res 的相对路径
                    rel_path = os.path.relpath(item_path, start=os.path.join(self.new_version, "res"))
                    # 构建目标路径
                    dest_dir = os.path.join(res_info_dir, os.path.dirname(rel_path))

                    # 确保目标目录存在
                    os.makedirs(dest_dir, exist_ok=True)

                    # 拷贝文件/目录
                    _copy_item(item_path, dest_dir)
                except Exception as e:
                    print(f"Error copying {item_path}: {str(e)}")

            # 处理 diff_files 文件/目录
            for item_path in results.get('diff_files', []):
                try:
                    if not os.path.exists(item_path):
                        print(f"Warning: 源路径 {item_path} 不存在，跳过")
                        continue

                    rel_path = os.path.relpath(item_path, start=os.path.join(self.new_version, "res"))
                    dest_dir = os.path.join(res_info_dir, os.path.dirname(rel_path))

                    os.makedirs(dest_dir, exist_ok=True)
                    _copy_item(item_path, dest_dir)
                except Exception as e:
                    print(f"Error copying {item_path}: {str(e)}")

        except Exception as e:
            print(f"Error in _copy_files_to_res_info_with_structure: {str(e)}")

    def _categorize_removed_items(self, results: Dict[str, List[str]]) -> None:
        """
        遍历 result['left_only']，分类到 removed_dirs 和 deleted_files

        Args:
            results: 包含比较结果的字典
        """
        self.removed_dirs.clear()  # 清空现有列表
        self.deleted_files.clear()

        for item_path in results.get('left_only', []):
            try:
                if not os.path.exists(item_path):
                    print(f"Warning: {item_path} 不存在，跳过")
                    continue

                if os.path.isdir(item_path):
                    rel_path = os.path.relpath(item_path, start=os.path.join(self.old_version, "res"))
                    self.removed_dirs.append(rel_path)
                    print(f"标记为删除目录: {rel_path}")
                elif os.path.isfile(item_path):
                    rel_path = os.path.relpath(item_path, start=os.path.join(self.old_version, "res"))
                    self.deleted_files.append(rel_path)
                    print(f"标记为删除文件: {rel_path}")
                else:
                    print(f"Warning: {item_path} 不是文件或目录，跳过")

            except Exception as e:
                print(f"Error processing {item_path}: {str(e)}")

    def _process_res_directories(self):
        """
        处理资源目录比较和文件管理
        """
        new_res_dir = os.path.join(self.new_version, "res")
        old_res_dir = os.path.join(self.old_version, "res")
        results = self._compare_res_directories(old_res_dir, new_res_dir)
        self._copy_files_to_res_info_with_structure(results, self.res_dir)
        self._categorize_removed_items(results)


def get_new_image(info):
    # 当前版本的镜像信息
    # 定义镜像和签名的名称列表
    image_names = ['ssb', 'recovery', 'application', 'bt', 'dsp_main', 'dsp_overlay', 'nv']
    image_list = []
    # 镜像
    for name in image_names:
        if hasattr(info, f'{name}_bin'):
            image_list.append(f"{getattr(info, f'{name}_bin')}={name}")

    # 资源文件
    if info.res_dir and os.path.exists(info.res_dir) and os.listdir(info.res_dir):
        result = res_pack(info.res_dir, info.res_index, info.res_data, info.deleted_files, info.removed_dirs)
        if not result:
            sys.exit("[ERROR] resource file pack failed!!!")
        image_list.append("=".join([info.res_index,        "res_index"]))
        image_list.append("=".join([info.res_data,         "res_data"]))

    if not image_list:
        return None

    new_image = "|".join(image_list)
    return new_image


def get_old_image(info):
    # 要进行差分升级的image的旧版本镜像列表
    image_names = ['recovery', 'application', 'bt', 'dsp_main', 'dsp_overlay']
    image_list = []
    # 镜像
    for name in image_names:
        if hasattr(info, f'{name}_old_bin'):
            image_list.append(f"{getattr(info, f'{name}_old_bin')}={name}")

    # 镜像签名
    for name in image_names:
        if hasattr(info, f'{name}_old_sign'):
            image_list.append(f"{getattr(info, f'{name}_old_sign')}={name}")

    old_image = "|".join(image_list)
    return old_image

if __name__ == '__main__':
    # 添加命令行参数解析
    parser = argparse.ArgumentParser(description='升级包制作工具')
    parser.add_argument('--type',
                    type=str,
                    choices=['default', 'compress', 'diff'],
                    default='default',
                    help='选择压缩类型: default(默认), compress(压缩), diff(差分)')
    args = parser.parse_args()
    for i, arg in enumerate(sys.argv[1:]):
        if arg.startswith('--type='):
            sys.argv.remove(arg)
            break

    root = get_project_root()
    sys.path.append(os.path.join(root, 'build', 'script'))
    sys.path.append(os.path.join(root, 'tools', 'pkg'))

    from build_upg_pkg import get_parameters, begin
    from make_upg_res_pack import res_pack
    from packet_create import split_bin

    info = upg_base_info(root, args.type)

    # 解析pkg包
    info._split_pkg()
    # 新增分区表校验
    if not info._check_partition_table():
        print("\n[流程终止] 由于分区表校验失败，升级包制作流程已终止")
        print("[提示] 请根据上述错误信息检查分区表配置")
        print("[提示] 确认后重新运行脚本")
        info._clean_directories()
        sys.exit(1)
    # 处理资源
    info._process_res_directories()
    info._process_image_files()

    conf = get_parameters()
    # 根据args.type设置app_name
    if args.type == 'default':
        conf.app_name = "update_default"
    elif args.type == 'compress':
        conf.app_name = "update_compress"
    elif args.type == 'diff':
        conf.app_name = "update_diff"
    conf.upg_format_path = info.fota_format_path
    conf.base            = info.custom_fota_cfg
    conf.temp_dir        = info.temp_dir
    conf.new_images      = get_new_image(info)
    if conf.new_images is None:
        print("\n[流程终止] 没有需要更新的镜像，升级包制作流程已终止")
        print("[提示] 请检查镜像文件是否存在或配置是否正确")
        info._clean_directories()
        sys.exit(1)

    conf.old_images      = get_old_image(info)
    conf.output_dir      = info.upg_output
    conf.type            = 0
    begin(conf)
    info._clean_directories()
