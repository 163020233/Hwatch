#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    build system entry, receive param & start to build
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
# ============================================================================
import os
import sys
import shutil
import json  # Add json import for mapping functions
import re  # Add for replace_in_file
import subprocess



# 设置 FBB_BUILD_ROOT_PATH 环境变量，必须在导入 build_utils 之前
def setup_environment():
    """设置必要的环境变量和路径"""
    # 计算基础路径
    script_dir = os.path.dirname(os.path.abspath(__file__))
    script_parent_dir = os.path.dirname(script_dir)  # script_dir的父目录
    current_dir = os.getcwd().replace("\\", "/")

    # 计算项目根目录：三级父目录
    sdk_root_dir = os.path.dirname(os.path.dirname(os.path.dirname(script_dir))).replace("\\", "/")
   

    # 设置环境变量
    if 'FBB_BUILD_ROOT_PATH' not in os.environ:
        # SDK根目录应该总是固定的，不依赖于当前工作目录
        os.environ['FBB_BUILD_ROOT_PATH'] = current_dir
        os.environ['FBB_SDK_ROOT_DIR'] = sdk_root_dir
        os.environ['FBB_SDK_DIR'] = sdk_root_dir
        print(f"Set FBB_BUILD_ROOT_PATH to: {current_dir}, FBB_SDK_ROOT_DIR to: {sdk_root_dir}, script_dir: {script_dir}, execute_path: {current_dir}")
    else:
        print(f"FBB_BUILD_ROOT_PATH already set to: {os.environ['FBB_BUILD_ROOT_PATH']}")    
    
    return script_dir, sdk_root_dir

# 调用环境设置函数
script_dir, sdk_root_dir = setup_environment()

sys.dont_write_bytecode = True

sys.path.append(os.path.join(script_dir, '../', '../', '../', 'build', 'config'))
sys.path.append(os.path.join(script_dir, '../', '../', '../', 'build', 'script'))


from cmake_builder import CMakeBuilder
from enviroment import TargetEnvironment, all_target, all_group, chip_group_target
from utils.build_utils import output_root

# 保存目标名的文件路径
TARGET_FILE = os.path.join(os.path.expanduser("~"), ".fbb_target")

def target_exists(target_name):
    """检查目标是否存在于系统中"""
    try:
        return target_name in all_target or target_name in all_group
    except:
        return False
def _is_group_target(target_name):
    """判断目标是否为 group（不可直接用于 clean/menuconfig/save-defconfig）"""
    try:
        return target_name in all_group
    except:
        return False
def _group_individual_targets(group_name):
    """返回 group 中的所有单独 target（过滤掉 copy/fwpkg 等非构建条目）"""
    try:
        members = all_group.get(group_name, [])
        return [t for t in members if t in all_target]
    except:
        return []
def _print_group_error(cmd, target):
    """打印 group target 不支持该命令的提示，并列出可用的单独 target"""
    print(f"Error: '{cmd}' does not support group target '{target}'.")
    members = _group_individual_targets(target)
    if members:
        print(f"Please use one of the individual targets from this group:")
        for t in members:
            print(f"  {t}")
    else:
        print("Use 'fbb list-targets' to see available individual targets.")

def find_available_target(target_name, command):
    """查找可用的目标"""
    # 搜索策略：首先搜索包含目标名和命令关键词的目标
    search_patterns = [
        # 完整匹配模式
        lambda name: target_name in name and command in name,
        # 目标名匹配
        lambda name: target_name in name,
        # 命令关键词匹配
        lambda name: command in name,
        # 任意目标
        lambda name: True
    ]
    
    # 在所有目标中搜索
    all_possible_targets = list(all_target.keys()) + list(all_group.keys())
    
    for pattern in search_patterns:
        matches = [target for target in all_possible_targets if pattern(target)]
        if matches:
            # 返回第一个匹配的目标
            return matches[0]
    
    return None

def save_target(target_name):
    """保存目标名到文件"""
    # 验证目标是否存在
    if not target_exists(target_name):
        print("Error: Target '%s' does not exist." % target_name)
        print("Use 'fbb list-targets' to see available targets.")
        sys.exit(1)
    
    try:
        with open(TARGET_FILE, 'w') as f:
            f.write(target_name)
        # 验证写入是否成功
        with open(TARGET_FILE, 'r') as f:
            saved_target = f.read().strip()
        if saved_target != target_name:
            raise IOError("Target verification failed: expected '%s', got '%s'" % (target_name, saved_target))
        print("Target saved: %s" % target_name)
    except (IOError, OSError) as e:
        print("Error saving target: %s" % e)
        sys.exit(1)

def load_target():
    """从文件加载目标名，如果文件不存在则报错"""
    if not os.path.exists(TARGET_FILE):
        print("Error: No target set. Use 'fbb set-target <target>' first.")
        print("Use 'fbb list-targets' to see available targets.")
        sys.exit(1)
    with open(TARGET_FILE, 'r') as f:
        target_name = f.read().strip()
    if not target_name:
        print("Error: Saved target is empty. Use 'fbb set-target <target>' to set a valid target.")
        sys.exit(1)
    return target_name
def _clean_output_root():
    """清理整个output目录"""
    if os.path.exists(output_root):
        shutil.rmtree(output_root)
        print("Cleaned output directory: %s" % output_root)
    else:
        print("Nothing to clean: %s does not exist." % output_root)

def fullclean_target(target, command="build"):
    """清理指定芯片和命令对应的目标的构建输出"""
    # group target: 逐个清理子 target
    if target in all_group:
        for sub_target in all_group[target]:
            if sub_target in all_target:
                fullclean_target(sub_target, command)
        return
    try:
        env = TargetEnvironment(target, [])
        chip_output_path = env.get_output_path()
        if os.path.exists(chip_output_path):
            shutil.rmtree(chip_output_path)
            print("Cleaned chip-specific output: %s" % chip_output_path)
    except Exception as e:
        print("Error during clean: %s" % e)
        sys.exit(1)


def fix_toolchain_symlinks():
    """
    修复 Windows 下 BiSheng 工具链里的假 exe 文件。
    Git 在 Windows 上把 Linux 符号链接存成纯文本文件（内容为链接目标名），
    这些文件被当作 exe 调用时会报 "This version of %1 is not compatible..."。
    本函数扫描所有小于 200 字节的 .exe，读取其文本内容（即链接目标名），
    再把目标真实二进制 copy 过来覆盖。
    Linux / WSL 下直接跳过。
    """
    if os.name != 'nt':
        print("[fix-toolchain] Non-Windows platform detected — nothing to do.")
        return

    import glob
    compiler_base = os.path.join(sdk_root_dir, "tools", "bin", "compiler", "bisheng_26_03_1")
    # 动态查找 *-win-* 目录（兼容版本号变化）
    win_dirs = glob.glob(os.path.join(compiler_base, "*-win-*", "bin"))
    if not win_dirs:
        print(f"[fix-toolchain] No Windows toolchain found under {compiler_base}")
        return

    FAKE_THRESHOLD = 200  # bytes；真实 exe 至少数 MB
    total_fixed = 0
    total_skipped = 0

    for bin_dir in win_dirs:
        print(f"[fix-toolchain] Scanning: {bin_dir}")
        try:
            entries = [e for e in os.listdir(bin_dir)
                       if e.lower().endswith('.exe')]
        except OSError as exc:
            print(f"[fix-toolchain] Cannot list {bin_dir}: {exc}")
            continue

        fixed = skipped = 0
        for fname in entries:
            fpath = os.path.join(bin_dir, fname)
            try:
                size = os.path.getsize(fpath)
            except OSError:
                continue
            if size >= FAKE_THRESHOLD:
                continue  # 真实二进制，跳过

            # 读取符号链接目标名
            try:
                with open(fpath, 'r', errors='ignore') as f:
                    target_name = f.read().strip()
            except OSError:
                continue

            if not target_name:
                print(f"  SKIP  {fname}  (empty content)")
                skipped += 1
                continue

            # 补 .exe 后缀（Linux 符号链接目标通常没有后缀）
            if not target_name.lower().endswith('.exe'):
                target_name += '.exe'

            target_path = os.path.join(bin_dir, target_name)
            if not os.path.exists(target_path):
                print(f"  SKIP  {fname} -> {target_name}  (target not found)")
                skipped += 1
                continue

            target_size = os.path.getsize(target_path)
            if target_size < FAKE_THRESHOLD:
                print(f"  SKIP  {fname} -> {target_name}  (target also fake, {target_size}B)")
                skipped += 1
                continue

            try:
                shutil.copy2(target_path, fpath)
                print(f"  FIXED {fname} <- {target_name}  ({target_size // (1024*1024)} MB)")
                fixed += 1
            except OSError as exc:
                print(f"  ERROR {fname}: {exc}")
                skipped += 1

        print(f"  Done. Fixed: {fixed}  Skipped: {skipped}")
        total_fixed += fixed
        total_skipped += skipped

    print(f"\n[fix-toolchain] Total fixed: {total_fixed}  skipped: {total_skipped}")
    if total_skipped > 0:
        print("[fix-toolchain] Skipped files need manual resolution (target missing or also fake).")


def python_clean():
    """删除 Python cache 字节码，避免 Python 版本切换导致问题"""
    project_root = os.environ.get('FBB_BUILD_ROOT_PATH', os.getcwd())
    deleted_count = 0
    
    for root, dirs, files in os.walk(project_root):
        # 删除 __pycache__ 目录
        if '__pycache__' in dirs:
            pycache_path = os.path.join(root, '__pycache__')
            try:
                shutil.rmtree(pycache_path)
                print("Deleted: %s" % pycache_path)
                deleted_count += 1
                dirs.remove('__pycache__')  # 避免继续遍历已删除的目录
            except Exception as e:
                print("Error deleting %s: %s" % (pycache_path, e))
        
        # 删除 .pyc 和 .pyo 文件
        for file in files:
            if file.endswith('.pyc') or file.endswith('.pyo'):
                file_path = os.path.join(root, file)
                try:
                    os.remove(file_path)
                    print("Deleted: %s" % file_path)
                    deleted_count += 1
                except Exception as e:
                    print("Error deleting %s: %s" % (file_path, e))
    
    print("Python clean completed. Deleted %d cache files/directories." % deleted_count)

def reconfigure_target(target_name, command="build"):
    """强制重新配置，解决新增文件重新配置的问题"""
    if target_name in all_group:
        for sub_target in all_group[target_name]:
            if sub_target in all_target:
                reconfigure_target(sub_target, command)
        return
    try:
        # 直接使用传入的目标名称
        env = TargetEnvironment(target_name, [])
        output_path = env.get_output_path()
        
        # 删除 CMakeCache.txt 以强制重新配置
        cache_file = os.path.join(output_path, "CMakeCache.txt")
        if os.path.exists(cache_file):
            os.remove(cache_file)
            print("Deleted CMakeCache.txt for reconfigure: %s" % cache_file)
        else:
            print("No CMakeCache.txt found at %s" % cache_file)
        
        # 删除 CMakeFiles 目录以确保完全重新配置
        cmake_files_dir = os.path.join(output_path, "CMakeFiles")
        if os.path.exists(cmake_files_dir):
            shutil.rmtree(cmake_files_dir)
            print("Deleted CMakeFiles directory for reconfigure: %s" % cmake_files_dir)
        
        print("Reconfigure setup completed for target: %s" % target_name)
        print("Next build will perform full reconfigure.")
    except Exception as e:
        print("Error during reconfigure: %s" % e)
        sys.exit(1)

# ============================================================
# flash / monitor — ported from hs-fbb-cli
# ============================================================

_WIN = os.name == "nt"
_INSTALL_DIR   = os.path.join(os.path.expanduser("~"), ".hs-fbb")
_TOOLCHAIN_DIR = os.path.join(_INSTALL_DIR, "toolchain")

# BurnTool exit codes: (error_code, recoverable)
_BURNTOOL_EXIT = {
    0:  (None, False),
    1:  ("DEVICE_NOT_RESPONDING", True),
    4:  ("DEVICE_NOT_RESPONDING", True),
    5:  ("DEVICE_NOT_RESPONDING", True),
    7:  ("DEVICE_NOT_RESPONDING", True),
    8:  ("DEVICE_NOT_RESPONDING", True),
    15: ("FLASH_PROTOCOL_ERROR", False),
    16: ("FWPKG_OPEN_FAILED", False),
    17: ("PORT_BUSY", False),
}
_BURNTOOL_TIMEOUT_S = 180


def _load_chip_json(chip):
    """读取 build/config/target_config/<chip>/<chip>.json"""
    p = os.path.join(sdk_root_dir, "build", "config", "target_config", chip, f"{chip}.json")
    if not os.path.exists(p):
        return {}
    try:
        with open(p, encoding="utf-8") as f:
            return json.load(f)
    except Exception:
        return {}


def _resolve_chip_from_target(target_name):
    """从 target 名推断 chip（扫描 target_config/ 目录）"""
    tc_dir = os.path.join(sdk_root_dir, "build", "config", "target_config")
    chips = []
    if os.path.isdir(tc_dir):
        for d in os.listdir(tc_dir):
            if os.path.isfile(os.path.join(tc_dir, d, f"{d}.json")):
                chips.append(d)
    tl = target_name.lower()
    for c in chips:
        if tl.startswith(c.lower()) or c.lower() in tl:
            return c
    return chips[0] if len(chips) == 1 else None


def _find_fwpkg(chip, target, load_only=False):
    """查找固件包文件，支持多种路径格式"""
    base = os.path.join(sdk_root_dir, "output", chip, "fwpkg")
    base_cur = os.path.join(os.getcwd(), "output", chip, "fwpkg")
    suffix = "_load_only" if load_only else "_all"
    candidates = [
        os.path.join(base, f"{target}{suffix}.fwpkg"),   # 带后缀
        os.path.join(base, f"{target}.fwpkg"),            # 无后缀（本 SDK 实际格式）
        os.path.join(base, target, f"{target}{suffix}.fwpkg"),  # 子目录+后缀
        os.path.join(base, target, f"{target}.fwpkg"),           # 子目录无后缀
		os.path.join(base, "diting-community.fwpkg"),
		os.path.join(base_cur, "diting-community.fwpkg"),
    ]
    for p in candidates:
        if os.path.exists(p):
            return p
    return None


def _find_burntool(burntool_path=None):
    """在工具链目录中搜索 BurnTool(.exe)"""
    if burntool_path and os.path.exists(burntool_path):
        return burntool_path
    name = "BurnTool.exe" if _WIN else "BurnTool"
    if os.path.isdir(_TOOLCHAIN_DIR):
        for root, dirs, files in os.walk(_TOOLCHAIN_DIR):
            for f in files:
                if f.lower() == name.lower() and "burntoolcmd" not in f.lower():
                    return os.path.join(root, f)
    return None


def _autodetect_port_win():
    """Windows 注册表枚举串口"""
    if not _WIN:
        return []
    try:
        import winreg
        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r"HARDWARE\DEVICEMAP\SERIALCOMM")
        ports, i = [], 0
        while True:
            try:
                _, val, _ = winreg.EnumValue(key, i); ports.append(str(val)); i += 1
            except OSError:
                break
        winreg.CloseKey(key)
        return sorted(ports)
    except Exception:
        return []


def _port_number(port):
    """COM4 -> 4"""
    return re.sub(r"\D", "", port) or port


def _parse_optlog(log_path):
    """解析 BurnTool optLog 获取 die_id 和烧写分区数"""
    die_id, sections = None, 0
    for enc in ("gbk", "utf-8"):
        try:
            with open(log_path, encoding=enc) as f:
                text = f.read()
            m = re.search(r"dieId\D*?(0x[0-9A-Fa-f]+)", text)
            if m:
                die_id = m.group(1)
            sections = len(re.findall(r"^\s*\d+\.\S+\.bin\s*$", text, re.MULTILINE))
            break
        except Exception:
            continue
    return die_id, sections


def flash_command(argv):
    """fbb.py flash <target> [--port COMx] [--baud N] [--erase-mode 0|1|2]
                             [--manual-reset] [--load-only] [--chip NAME]
                             [--burntool-path PATH]"""
    import argparse
    p = argparse.ArgumentParser(prog="fbb.py flash",
                                description="烧写固件到开发板（通过 BurnTool）")
    p.add_argument("target", help="编译目标，如 diting-community")
    p.add_argument("--port", default=None, help="串口（如 COM4）；自动检测时可省略")
    p.add_argument("--baud", type=int, default=None, help="烧写波特率（默认读 chip.json）")
    p.add_argument("--erase-mode", type=int, choices=[0,1,2], default=0,
                   help="0=正常 1=全片 2=跳过（默认0）")
    p.add_argument("--manual-reset", action="store_true",
                   help="不发送自动复位，手动按 RESET 键（首烧/崩溃板）")
    p.add_argument("--load-only", action="store_true",
                   help="烧 *_load_only.fwpkg")
    p.add_argument("--chip", default=None, help="芯片名（目标名无前缀时指定）")
    p.add_argument("--burntool-path", default=None, help="BurnTool 路径（默认自动查找）")
    args = p.parse_args(argv)

    target = args.target

    # 1. 解析 chip
    chip = args.chip or _resolve_chip_from_target(target)
    if not chip:
        print(f"Error: 无法从目标 '{target}' 推断芯片，请用 --chip 指定")
        sys.exit(3)

    # 2. 读 chip JSON
    chip_json = _load_chip_json(chip)
    flash_cfg = chip_json.get("flash", {})
    chiptype = flash_cfg.get("chiptype") or chip
    signalbaud = args.baud or flash_cfg.get("signalbaud") or flash_cfg.get("baud") or 921600

    # 3. 查找 fwpkg
    fwpkg = _find_fwpkg(chip, target, args.load_only)
    if not fwpkg:
        print(f"Error: 未找到固件包，请先运行 fbb.py build {target}")
        print(f"  搜索路径: {os.path.join(sdk_root_dir, 'output', chip, 'fwpkg')}")
        sys.exit(3)
    if " " in fwpkg:
        print(f"Error: fwpkg 路径含空格，BurnTool 不支持: {fwpkg}")
        sys.exit(4)

    # 4. 查找 BurnTool
    burntool = _find_burntool(args.burntool_path)
    if not burntool:
        print(f"Error: 未找到 BurnTool，请运行 fbb setup 或用 --burntool-path 指定")
        sys.exit(3)

    # 5. 解析串口
    port = args.port
    if not port:
        cands = _autodetect_port_win()
        if len(cands) == 1:
            port = cands[0]; print(f"自动检测串口: {port}")
        else:
            print(f"Error: 请用 --port 指定串口（检测到: {cands or '无'}）")
            sys.exit(3)

    # 6. 杀掉残留 BurnTool（Windows）
    if _WIN:
        try:
            subprocess.run(["taskkill", "/im", "BurnTool.exe", "/f"],
                           capture_output=True, timeout=10)
        except Exception:
            pass

    # 7. 组装命令并运行
    before_reset = not args.manual_reset
    cmd = [
        burntool,
        f"-com:{_port_number(port)}",
        f"-bin:{fwpkg}",
        f"-chiptype:{chiptype}",
        f"-signalbaud:{signalbaud}",
        f"-erasemode:{args.erase_mode}",
        "-switchafterloader", "-reset", "-console",
    ]
    if before_reset:
        cmd.append("-beforereset")

    print(f"[fbb] flash: {os.path.basename(fwpkg)} -> {port} @ {signalbaud}"
          + ("  (自动复位)" if before_reset else "  (手动复位)"))
    if args.manual_reset:
        print("[fbb] 手动复位模式：请在烧写开始后30秒内按下开发板 RESET 键")

    try:
        result = subprocess.run(cmd, capture_output=True, text=True,
                                errors="replace", timeout=_BURNTOOL_TIMEOUT_S)
        rc = result.returncode
        if result.stdout:
            print(result.stdout, end="")
        if result.stderr:
            print(result.stderr, end="", file=sys.stderr)
    except subprocess.TimeoutExpired:
        print("Error: BurnTool 超时")
        sys.exit(1)

    # 8. 解析 optLog
    optlog_dir = os.path.join(os.path.dirname(burntool), "optLog")
    die_id, sections = None, 0
    if os.path.isdir(optlog_dir):
        logs = sorted(
            [os.path.join(optlog_dir, f) for f in os.listdir(optlog_dir)
             if f.startswith("optLog_") and f.endswith(".txt")],
            key=os.path.getmtime, reverse=True
        )
        if logs:
            die_id, sections = _parse_optlog(logs[0])

    # 9. 结果
    err_code, _ = _BURNTOOL_EXIT.get(rc, ("FLASH_FAILED", False))
    if rc == 0:
        print(f"[fbb] 烧写成功: {sections} 分区" + (f", dieId {die_id}" if die_id else ""))
        return
    msgs = {
        "DEVICE_NOT_RESPONDING": "设备未进入下载模式，请按 RESET 键并加 --manual-reset 重试",
        "PORT_BUSY": f"{port} 被占用，请关闭占用程序后重试",
        "FWPKG_OPEN_FAILED": "BurnTool 无法打开固件包文件",
        "FLASH_PROTOCOL_ERROR": "烧写协议错误",
    }
    print(f"Error: 烧写失败 [{err_code}]: {msgs.get(err_code or '', f'BurnTool 退出码 {rc}')}")
    sys.exit(1)


def monitor_command(argv):
    """fbb.py monitor [--port COMx] [--baud N] [--chip NAME]
                      [--until REGEX] [--timeout N] [--reset] [--log FILE]"""
    import argparse
    p = argparse.ArgumentParser(prog="fbb.py monitor",
                                description="串口监视器（支持 --until / --timeout）")
    p.add_argument("--port", default=None, help="串口（如 COM4）；自动检测时可省略")
    p.add_argument("--baud", type=int, default=None, help="波特率（默认读 chip.json）")
    p.add_argument("--chip", default=None, help="芯片名（单芯片 SDK 自动选择）")
    p.add_argument("--until", default=None, metavar="REGEX",
                   help="匹配到正则时 exit 0；超时无匹配 exit 4")
    p.add_argument("--timeout", type=float, default=None, metavar="SECONDS",
                   help="超时秒数（HIL 使用时与 --until 配合）")
    p.add_argument("--reset", action="store_true",
                   help="打开串口后先发送 chip.json 中的 reset 命令")
    p.add_argument("--log", default=None, metavar="FILE",
                   help="同时将输出写入文件")
    args = p.parse_args(argv)

    # 1. pyserial
    try:
        import serial
    except ImportError:
        print("Error: pyserial 未安装，请运行 fbb setup")
        sys.exit(3)

    # 2. 编译 --until 正则
    until_re = None
    if args.until:
        try:
            until_re = re.compile(args.until)
        except re.error as e:
            print(f"Error: --until 正则无效: {e}")
            sys.exit(2)

    # 3. 读 chip JSON
    chip = args.chip or _resolve_chip_from_target("")
    monitor_meta = {"default_baud": 115200, "reset_method": "none", "reset_command": ""}
    if chip:
        chip_json = _load_chip_json(chip)
        mon = chip_json.get("monitor", {})
        monitor_meta = {
            "default_baud":   int(mon.get("default_baud", 115200)),
            "reset_method":   mon.get("reset_method", "none"),
            "reset_command":  mon.get("reset_command", ""),
        }

    # 4. 串口
    port = args.port
    if not port:
        if _WIN:
            cands = _autodetect_port_win()
        else:
            try:
                import serial.tools.list_ports
                cands = [p.device for p in serial.tools.list_ports.comports()]
            except Exception:
                cands = []
        if len(cands) == 1:
            port = cands[0]; print(f"自动检测串口: {port}")
        else:
            print(f"Error: 请用 --port 指定串口（检测到: {cands or '无'}）")
            sys.exit(3)

    baud = args.baud or monitor_meta["default_baud"]

    # 5. 打开日志
    log_fp = None
    if args.log:
        try:
            log_fp = open(args.log, "ab")
        except OSError as e:
            print(f"Error: 无法打开日志文件 {args.log}: {e}")
            sys.exit(1)

    # 6. 打开串口
    extras = ""
    if until_re:
        extras += f"  until /{args.until}/"
    if args.timeout:
        extras += f"  timeout {args.timeout}s"
    else:
        extras += "  (Ctrl+C 退出)"
    print(f"[fbb] monitor: {port} @ {baud}{extras}")

    try:
        ser = serial.Serial(port, baud, timeout=0.2)
    except serial.SerialException as e:
        print(f"Error: 无法打开串口 {port}: {e}")
        sys.exit(3)

    # 7. 发送 reset
    if args.reset and monitor_meta["reset_method"] == "at_rst" and monitor_meta["reset_command"]:
        try:
            ser.write(monitor_meta["reset_command"].encode("utf-8"))
            ser.flush()
            print(f"[fbb] 发送 reset: {monitor_meta['reset_command']!r}")
            import time; time.sleep(0.3)
        except Exception as e:
            print(f"[fbb] reset 失败: {e}", file=sys.stderr)

    # 8. 读取循环
    import time
    buf = bytearray()
    bytes_read = 0
    deadline = time.monotonic() + args.timeout if args.timeout else None

    try:
        while True:
            if deadline and time.monotonic() >= deadline:
                if until_re:
                    print("\n[fbb] 超时，未匹配到目标字符串", file=sys.stderr)
                    sys.exit(4)
                break
            chunk = ser.read(4096)
            if chunk:
                sys.stdout.buffer.write(chunk)
                sys.stdout.buffer.flush()
                if log_fp:
                    log_fp.write(chunk); log_fp.flush()
                bytes_read += len(chunk)
                if until_re:
                    buf.extend(chunk)
                    if len(buf) > 65536:
                        buf = buf[-65536:]
                    m = until_re.search(buf.decode("utf-8", errors="replace"))
                    if m:
                        print(f"\n[fbb] 匹配成功: {m.group()!r}", file=sys.stderr)
                        break
    except KeyboardInterrupt:
        print("\n[fbb] monitor 退出 (Ctrl+C)", file=sys.stderr)
    finally:
        ser.close()
        if log_fp:
            log_fp.close()


def _is_out_of_tree():
    """判断当前是否为树外构建：cwd 不在 SDK 目录内且存在 CMakeLists.txt"""
    cwd = os.getcwd().replace("\\", "/")
    sdk = sdk_root_dir.replace("\\", "/")
    # if cwd == sdk or cwd.startswith(sdk + "/"):
    if cwd == sdk:
        return False
    return os.path.isfile(os.path.join(cwd, "CMakeLists.txt"))
def replace_in_file(filename, pattern, replacement):
    """在文件中替换文本"""
    with open(filename, 'r', encoding='utf-8') as f:
        content = f.read()
    content = re.sub(pattern, replacement, content, flags=re.M)
    with open(filename, 'w', encoding='utf-8') as f:
        f.write(content)
def to_pascal_case(name):
    """my_app → MyApp，myapp → Myapp"""
    return ''.join(word.capitalize() for word in re.split(r'[_\-\s]+', name))

def is_empty_and_create(path, action_type):
    """检查路径是否为空并创建目录"""
    abspath = os.path.abspath(path)
    if not os.path.exists(abspath):
        os.makedirs(abspath)
    elif not os.path.isdir(abspath):
        print('Error: Your target path is not a directory. Please remove the', abspath, 'or use different target path.')
        sys.exit(4)
    elif len(os.listdir(path)) > 0:
        print('Error: The directory', abspath, 'is not empty. To create a', action_type, 'you must empty the directory or choose a different path.')
        sys.exit(3)

def create_project(project_name, path=None):
    """创建新项目"""
    if path is None:
        path = os.path.join(os.getcwd(), project_name)
    else:
        path = os.path.join(path, project_name)
    is_empty_and_create(path, "project")
    
    # 项目模板路径
    project_template_dir = os.path.join(script_dir, "templates", "sample_project")
    if not os.path.exists(project_template_dir):
        print("Error: Project template not found at:", project_template_dir)
        sys.exit(1)
    
    # 复制模板文件
    for item in os.listdir(project_template_dir):
        src = os.path.join(project_template_dir, item)
        dst = os.path.join(path, item)
        if os.path.isfile(src):
            shutil.copy2(src, dst)
        else:
            shutil.copytree(src, dst)
    
    # 替换项目名称（如果需要）
    cmake_file = os.path.join(path, "CMakeLists.txt")
    if os.path.exists(cmake_file):
        replace_in_file(cmake_file, "sample_project", project_name)
    
    pascal_name = to_pascal_case(project_name)
    lower_name = project_name.lower()
    upper_name = project_name.upper()

    for root, dirs, files in os.walk(path):
        for fname in files:
            if fname.endswith(('.h', '.cpp')):
                fpath = os.path.join(root, fname)
                replace_in_file(fpath, 'Sample', pascal_name)
                replace_in_file(fpath, 'SAMPLE', upper_name)
                replace_in_file(fpath, 'samplepage', lower_name)
            elif fname == 'CMakeLists.txt':
                fpath = os.path.join(root, fname)
                replace_in_file(fpath, 'Sample', pascal_name)
                replace_in_file(fpath, 'samplepage', lower_name)

    # 重命名文件和目录（topdown=False 确保先处理叶子节点）
    for root, dirs, files in os.walk(path, topdown=False):
        for fname in files:
            if 'Sample' in fname:
                new_fname = fname.replace('Sample', pascal_name)
                os.rename(os.path.join(root, fname), os.path.join(root, new_fname))
        for dname in dirs:
            if dname == 'samplepage':
                os.rename(os.path.join(root, dname), os.path.join(root, lower_name))
    print("Project", project_name, "created successfully at", os.path.abspath(path))

def create_component(component_name, path=None):
    """创建新组件"""
    if path is None:
        path = os.path.join(os.getcwd(), "components", component_name)
    else:
        path = os.path.join(path, "components", component_name)
    is_empty_and_create(path, "component")
    
    
    # 组件模板路径
    component_template_dir = os.path.join(script_dir, "templates", "sample_component")
    if not os.path.exists(component_template_dir):
        print("Error: Component template not found at:", component_template_dir)
        sys.exit(1)
    
    # 复制模板文件
    for item in os.listdir(component_template_dir):
        src = os.path.join(component_template_dir, item)
        dst = os.path.join(path, item)
        if os.path.isfile(src):
            shutil.copy2(src, dst)
        else:
            shutil.copytree(src, dst)
    
    # 替换组件名称
    cmake_file = os.path.join(path, "CMakeLists.txt")
    if os.path.exists(cmake_file):
        replace_in_file(cmake_file, "sample_component", component_name)

    print("Component", component_name, "created at", os.path.abspath(path))
    print("It will be auto-discovered by the build system (scans components/ in the project root).")

def create_project_from_example(example_name, project_path=None):
    """基于示例创建新项目"""
    if project_path is None:
        project_path = os.path.join(os.getcwd(), example_name)
    
    # 构建示例路径
    examples_dir = os.path.join(sdk_root_dir, "application", "wearable", "nativeapp", "nativeui")
    example_path = os.path.join(examples_dir, example_name)
    
    # 检查示例是否存在
    if not os.path.exists(example_path):
        print(f"Error: Example '{example_name}' not found in {examples_dir}")
        print("Available examples:")
        try:
            examples = os.listdir(examples_dir)
            for ex in examples:
                if os.path.isdir(os.path.join(examples_dir, ex)):
                    print(f"  - {ex}")
        except Exception as e:
            print(f"Error listing examples: {e}")
        sys.exit(1)

    # 检查目标路径是否为空
    is_empty_and_create(project_path, "project")

    # 复制示例文件
    for item in os.listdir(example_path):
        src = os.path.join(example_path, item)
        dst = os.path.join(project_path, item)

        # 跳过隐藏文件和特殊文件
        if item.startswith('.'):
            continue

        if os.path.isfile(src):
            shutil.copy2(src, dst)
        elif os.path.isdir(src):
            shutil.copytree(src, dst)

    print(f"Project created successfully from example '{example_name}' at {os.path.abspath(project_path)}")

def list_templates(json_output=False):
    """列出可用的模板（示例项目）"""
    # 构建示例路径 - 与 create_project_from_example 使用相同的目录
    examples_dir = os.path.join(sdk_root_dir, "application", "wearable", "nativeapp", "nativeui")

    # 检查示例目录是否存在
    if not os.path.exists(examples_dir):
        print("Error: Examples directory not found:", examples_dir)
        sys.exit(1)

    try:
        # 获取所有示例目录
        examples = []
        for item in os.listdir(examples_dir):
            item_path = os.path.join(examples_dir, item)
            if os.path.isdir(item_path) and not item.startswith('.'):
                examples.append(item)

        examples.sort()

        if json_output:
            # JSON 格式输出
            import json
            print(json.dumps({"templates": examples}, indent=2))
        else:
            # 普通文本输出
            print("Available templates:")
            print("=" * 50)
            for example in examples:
                print(f"  {example}")
            print(f"\nTotal: {len(examples)} templates")
    except Exception as e:
        print("Error listing templates:", e)
        sys.exit(1)

def list_targets_simple():
    """简单列出所有可用的目标"""
    print("Available targets:")
    print("=" * 50)

    # 列出所有目标
    if all_target:
        print("Individual targets:")
        for target in sorted(all_target.keys()):
            print(f"  {target}")
        print()

    if all_group:
        print("Target groups:")
        for group in sorted(all_group.keys()):
            print(f"  {group}")
        print()

    print(f"Total: {len(all_target) if all_target else 0} targets, {len(all_group) if all_group else 0} groups")

def main():
    # 检查是否为调试模式
    debug_mode = "--debug" in sys.argv

    # 检查是否为帮助模式（--help 或 -h 只在第一个参数位置触发顶层 help）
    first_arg = sys.argv[1] if len(sys.argv) >= 2 else ""
    if first_arg in ('--help', '-h') or len(sys.argv) < 2:
        print("=" * 50)
        print("Usage: fbb.py <command> [options]")
        print()
        print("基本命令:")
        print("  set-target <target>              设置目标平台")
        print("  build [<target>] [args...]       编译镜像（可指定目标）")
        print("  rebuild <target>                 重新编译（先清理再构建）")
        print("  clean [<target>]                 删除output目录下构建配置")
        print("  python-clean                     删除Python缓存字节码")
        print("  fix-toolchain                    修复Windows工具链假符号链接exe（Linux跳过）")
        print("  menuconfig [<target>]            打开图形配置界面")
        print("  reconfigure [<target>]           强制 CMake 重新配置")
        print("  list-targets                     列出所有编译目标")
        print()
        print("烧写/监视命令:")
        print("  flash <target> [--port COMx] [--baud N] [--manual-reset]")
        print("                [--erase-mode 0|1|2] [--load-only] [--chip NAME]")
        print("                [--burntool-path PATH]    烧写固件到开发板")
        print("  monitor [--port COMx] [--baud N] [--chip NAME]")
        print("          [--until REGEX] [--timeout N] [--reset] [--log FILE]")
        print("                                          串口监视器")
        print()
        print("项目管理命令:")
        print("  create-project <name> [--template TPL] [--path DIR] 创建新项目")
        print("  create-component <name> 创建新组件")
        print("  create-project-from-example <example> [-p <path>] 基于示例创建项目")
        print("  list-templates             列出可用模板")
        print()
        print("选项:")
        print("  --help, -h                       显示此帮助信息")
        print("  --debug                          启用调试模式")
        print()
        print("示例:")
        print("  fbb.py set-target diting-community")
        print("  fbb.py build                      编译默认目标")
        print("  fbb.py flash diting-community --port COM3")
        print("  fbb.py flash diting-community --port COM3 --manual-reset")
        print("  fbb.py monitor --port COM3                   (交互式，Ctrl+C 退出)")
        print("  fbb.py create-project myapp")
        print("  fbb.py --help")
        sys.exit(0)

    # 其余代码保持不变...
    # ... existing code ...

    command = sys.argv[1]

    if command == "set-target":
        if len(sys.argv) < 3:
            print("Usage: fbb.py set-target <target>")
            print("Use 'fbb list-targets' to see available targets.")
            sys.exit(1)
        target_name = sys.argv[2]
        save_target(target_name)
        return  # 退出 after saving target

    elif command == "flash":
        flash_command(sys.argv[2:])

    elif command == "monitor":
        monitor_command(sys.argv[2:])

    elif command == "build":
        # 检查是否有可选的 target 参数（第二个参数且不是以 '-' 开头的选项）
        target = None
        remaining_args = []
        i = 2

        while i < len(sys.argv):
            if not sys.argv[i].startswith('-') and target is None:
                # 如果不是选项且还没有设置 target，则将其作为 target
                target = sys.argv[i]
                i += 1
            else:
                # 其他参数保留
                remaining_args.append(sys.argv[i])
                i += 1

        # 如果没有提供 target 参数，则从文件加载保存的目标
        if target is None:
            target = load_target()

        if 'FBB_PROJECT_DIR' not in os.environ and _is_out_of_tree():
            os.environ['FBB_PROJECT_DIR'] = os.getcwd()
            os.environ['FBB_PROJECT_TARGET'] = "diting-community"
            print(f"[fbb] out-of-tree build detected: project_dir={os.getcwd()}, target={target}")
        # 验证目标是否存在
        if not target_exists(target):
            print(f"Error: Target '{target}' does not exist.")

            list_targets_simple()
            sys.exit(1)

        # 构建新参数列表: 脚本名 + 剩余参数 + 目标
        new_argv = [sys.argv[0]] + remaining_args + [target]
        builder = CMakeBuilder(new_argv)
        builder.build()


    elif command == "rebuild":
        # 检查是否有可选的 target 参数（第二个参数且不是以 '-' 开头的选项）
        target = None
        remaining_args = []
        i = 2

        while i < len(sys.argv):
            if not sys.argv[i].startswith('-') and target is None:
                # 如果不是选项且还没有设置 target，则将其作为 target
                target = sys.argv[i]
                i += 1
            else:
                # 其他参数保留
                remaining_args.append(sys.argv[i])
                i += 1

        # 如果没有提供 target 参数，则从文件加载保存的目标
        if target is None:
            target = load_target()
        if 'FBB_PROJECT_DIR' not in os.environ and _is_out_of_tree():
            os.environ['FBB_PROJECT_DIR'] = os.getcwd()
            os.environ['FBB_PROJECT_TARGET'] = "diting-community"
            print(f"[fbb] out-of-tree build detected: project_dir={os.getcwd()}, target={target}")

        # 验证目标是否存在
        if not target_exists(target):
            print(f"Error: Target '{target}' does not exist.")
            list_targets_simple()
            sys.exit(1)

        # 先执行 fullclean
        print("Cleaning build output...")
        fullclean_target(target)

        # 然后执行 build
        print("Rebuilding...")
        new_argv = [sys.argv[0]] + remaining_args + [target]
        builder = CMakeBuilder(new_argv)
        builder.build()

    elif command == "clean":
        # 检查是否有可选的 target 参数（第二个参数且不是以 '-' 开头的选项）
        target = None
        remaining_args = []
        i = 2

        while i < len(sys.argv):
            if not sys.argv[i].startswith('-') and target is None:
                # 如果不是选项且还没有设置 target，则将其作为 target
                target = sys.argv[i]
                i += 1
            else:
                # 其他参数保留
                remaining_args.append(sys.argv[i])
                i += 1

        # 如果没有提供 target 参数，则从文件加载保存的目标
        if target is None:
            if not os.path.exists(TARGET_FILE):
                _clean_output_root()
                sys.exit(0)
            with open(TARGET_FILE, 'r') as f:
                target = f.read().strip()
            if not target:
                _clean_output_root()
                sys.exit(0)

        if not target_exists(target):
            print(f"Warning: Target '{target}' not found. Cleaning entire output directory.")
            _clean_output_root()
            sys.exit(0)
        if _is_group_target(target):
            print(f"Warning: Target '{target}' not found. Cleaning entire output directory.")
            _clean_output_root()
            sys.exit(1)

        # 只清理特定目标的构建输出
        print("Cleaning build output...")

        fullclean_target(target)

    elif command == "python-clean":
        python_clean()
    elif command == "fix-toolchain":
        fix_toolchain_symlinks()
    elif command == "reconfigure":
        # 允许显式指定目标，否则使用保存的目标
        target_command = None
        if len(sys.argv) > 2 and not sys.argv[2].startswith('-'):
            target_command = sys.argv[2]
        if target_command is None:
            target_command = load_target()
        reconfigure_target(target_command)

    elif command == "menuconfig":
        # 检查是否有可选的 target 参数
        target = None
        remaining_args = []
        i = 2

        while i < len(sys.argv):
            if not sys.argv[i].startswith('-') and target is None:
                # 如果不是选项且还没有设置 target，则将其作为目标
                target = sys.argv[i]
                i += 1
            else:
                # 其他参数保留
                remaining_args.append(sys.argv[i])
                i += 1

        # 如果没有提供 target 参数，则从文件加载保存的目标
        if target is None:
            target = load_target()

        if 'FBB_PROJECT_DIR' not in os.environ and _is_out_of_tree():
            os.environ['FBB_PROJECT_DIR'] = os.getcwd()
            os.environ['FBB_PROJECT_TARGET'] = "diting-community"
        # 验证目标是否存在
        if not target_exists(target):
            print(f"Error: menuconfig cannot be performed - target '{target}' not found.")
            print("menuconfig requires a valid build target. Use 'fbb list-targets' to see available targets.")
            sys.exit(1)
        if _is_group_target(target):
            _print_group_error("menuconfig", target)
            sys.exit(1)

        # 构建参数并调用 CMakeBuilder
        new_argv = [sys.argv[0]] + remaining_args + [target, "menuconfig"]
        print(f"Running menuconfig for target: {target}")
        builder = CMakeBuilder(new_argv)
        builder.build()

    # 移除 app 和 bootloader 命令

    elif command == "create-project":
        if len(sys.argv) < 3:
            print("Usage: fbb.py create-project <project_name> [--template TPL] [--path DIR]")
            print("  project_name: 项目名称")
            print("  --template: 可选，使用的模板名称（示例项目名）")
            print("  --path: 可选，项目创建的路径，默认为当前目录下的 project_name 文件夹")
            print("\n使用 'fbb.py list-templates' 查看可用模板")
            sys.exit(1)
        project_name = sys.argv[2]
        template = None
        path = None

        # 解析参数，支持 --template 和 --path
        i = 3
        while i < len(sys.argv):
            if sys.argv[i] == "--template" and i + 1 < len(sys.argv):
                template = sys.argv[i + 1]
                i += 2
            elif sys.argv[i] in ["--path", "-p"] and i + 1 < len(sys.argv):
                path = sys.argv[i + 1]
                i += 2
            else:
                print(f"Error: Unknown option or missing value: {sys.argv[i]}")
                sys.exit(1)

        if template:
            # 使用指定模板创建项目
            if path is None:
                path = os.path.join(os.getcwd(), project_name)
            # -p 是父目录，追加 project_name 得到完整路径
            example_path = os.path.join(path, project_name) if path else None
            create_project_from_example(template, example_path)
        else:
            # 使用默认模板创建项目
            create_project(project_name, path)

    elif command == "create-component":
        if len(sys.argv) < 3:
            print("Usage: fbb.py create-component <component_name>")
            print("  component_name: 组件名称")
            print("  -p/--path: 可选，组件创建的路径，默认为当前目录下的 component_name 文件夹")
            sys.exit(1)
        component_name = sys.argv[2]
        path = None

        # 解析路径参数，支持 -p 和 --path
        i = 3
        while i < len(sys.argv):
            if sys.argv[i] in ["-p", "--path"] and i + 1 < len(sys.argv):
                path = sys.argv[i + 1]
                i += 2
            else:
                i += 1

        create_component(component_name, path)

    elif command == "list-templates":
        json_output = False

        # 检查是否有 --json 选项
        if "--json" in sys.argv:
            json_output = True

        list_templates(json_output)

    elif command == "create-project-from-example":
        if len(sys.argv) < 3:
            print("Usage: fbb.py create-project-from-example <example_name> [-p <project_path>]")
            print("  example_name: 示例名称，如 'weather', 'music' 等")
            print("  -p/--path: 可选，项目创建的路径，默认为当前目录下的 example_name 文件夹")
            sys.exit(1)

        example_name = sys.argv[2]
        project_path = None

        # 解析路径参数（-p 是父目录，追加 example_name 得到完整路径）
        i = 3
        while i < len(sys.argv):
            if sys.argv[i] in ["-p", "--path"] and i + 1 < len(sys.argv):
                project_path = os.path.join(sys.argv[i + 1], example_name)
                i += 2
            else:
                i += 1

        create_project_from_example(example_name, project_path)

    elif command == "list-targets":
        # 简单列出所有目标
        list_targets_simple()

    elif command == "save-defconfig":
        target = load_target()

        # 首先验证目标是否存在
        if not target_exists(target):
            print("Error: Target '%s' does not exist." % target)
            print("Available targets: %s" % list(all_target.keys()))
            sys.exit(1)

        if _is_group_target(target):
            _print_group_error("save-defconfig", target)
            sys.exit(1)
        env = TargetEnvironment(target, [])
        core = env.get('core')
        chip = env.get('chip')

        try:
            from usr_config import mconfig
        except ImportError:
            print("Error: Failed to import mconfig from usr_config")
            sys.exit(1)

        output_path = os.getcwd()
        
        # 获取项目根目录
        project_root = sdk_root_dir  # 使用setup_environment中计算的项目根目录

        temp_target = target.replace("-", "_")

        # 备份原来的配置文件
        target_conf_path = os.path.join(project_root, "build", "config", "target_config", chip, "menuconfig", core, "%s.config" % temp_target)
        backup_path = target_conf_path + ".backup"
        if os.path.exists(target_conf_path):
            shutil.copy2(target_conf_path, backup_path)
        else:
            # 如果配置文件不存在，创建一个空的备份文件用于恢复
            open(backup_path, 'w').close()

        try:
            # 执行defconfig - 添加root参数
            mconfig('defconfig', chip, core, target, output_path, root=project_root)
            
            # 检查defconfig是否成功生成配置文件
            if not os.path.exists(target_conf_path):
                print("Error: Defconfig failed to generate configuration file at: %s" % target_conf_path)
                sys.exit(1)
                
            # 复制到sdk_defconfig
            sdk_defconfig_path = os.path.join(output_path, "sdk_defconfig")
            shutil.copy2(target_conf_path, sdk_defconfig_path)
            print("Defconfig saved to: %s" % sdk_defconfig_path)
            
        except Exception as e:
            print("Error during defconfig generation: %s" % e)
            sys.exit(1)
        finally:
            # 恢复原来的配置文件
            if os.path.exists(backup_path) and os.path.getsize(backup_path) > 0:
                shutil.move(backup_path, target_conf_path)
            elif os.path.exists(backup_path):
                # 如果备份文件是空的（原来的配置文件不存在），则删除生成的文件
                if os.path.exists(target_conf_path):
                    os.remove(target_conf_path)
                os.remove(backup_path)

        # 删除mconfig.h - 确保路径正确
        mconfig_h_path = os.path.join(output_path, "mconfig.h")
        if os.path.exists(mconfig_h_path):
            os.remove(mconfig_h_path)
            print("Cleaned up mconfig.h")
    else:
        # 旧模式兼容：直接调用 CMakeBuilder
        builder = CMakeBuilder(sys.argv)
        builder.build()

if __name__ == "__main__":
    main()
