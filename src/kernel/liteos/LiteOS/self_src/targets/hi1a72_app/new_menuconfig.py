# encoding=utf-8
# ============================================================================
# @brief    Accept configure from app
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
# ============================================================================
import os, sys
from functools import reduce

def handle_demo_none(root_dir, conf_base):
    # use base configure, do nothing
    pass

def build_conf_line(contexts):
    lines = []
    for context in contexts:
        lines.append("%s\n"%context)
    return lines

def dy_data_switch(page, demo_conf_data):
    flag = page[0].find('=X')
    value = ""
    for demo_conf_line in demo_conf_data:
        if demo_conf_line.startswith(page[0][:flag]):
            value = demo_conf_line[flag:]
            break;
    new_page = []
    for item in page[1]:
        pos = item.find('=X')
        if pos > 0:
            src = item[:pos]
            if value == "":
                new_page = None
            else:
                new_page.append("%s%s"%(src, value))

        else:
            new_page.append(item)
    return new_page

def build_os_conf_data(demo_conf_data, base_conf_data):
    book = [
        # Format: [app config, [os config]]
        ["CONFIG_SUPPORT_FILESYSTEM=y",     ["# FileSystem", "LOSCFG_FS_COMPAT_NUTTX=y", "LOSCFG_FS_VFS=y", "LOSCFG_FS_FAT=y",
                                             "LOSCFG_FS_FAT_VOLUMES=16", "LOSCFG_FS_FAT_STACK_LFN=y",
                                             "LOSCFG_FS_PROC=y", "LOSCFG_DRIVER_DISK=y"]],
        ["CONFIG_SUPPORT_FS_CACHE=y",       ["LOSCFG_FS_FAT_CACHE=y", "LOSCFG_FS_FAT_BLOCK_NUMS=8", "LOSCFG_FS_FAT_CACHE_INDEPENDENT_MEMORY_POOL=y"]],
        ["CONFIG_FS_BASE_EMMC=y",           ["LOSCFG_DRIVERS_MMC=y", "LOSCFG_DRIVERS_MMC_HISDEMMC=y", "LOSCFG_DRIVERS_EMMC=y",
                                             "LOSCFG_DRIVERS_MMC_TASK_PRI=2", "LOSCFG_DRIVERS_MMC_TASK_STACK_SIZE=0x2000",
                                             "LOSCFG_DRIVERS_EMMC_HS200=y", "LOSCFG_DRIVERS_MMC_DMA_CACHE_DISABLE=y",
                                             "LOSCFG_MMCSD_TASK_STACK_SIZE=0xC00", "LOSCFG_DRIVERS_MMC_EXT_CLK_SWITCH=y",
                                             "LOSCFG_FS_FAT_TRIM=y", "LOSCFG_FS_FAT_DIRECT_TRIM=y", "#end FileSystem",]],
    ]
    os_conf_data = base_conf_data

    for page in book:
        if page[0].find('=X') > 0:
            new_page = dy_data_switch(page, demo_conf_data)
            if new_page != None:
                os_conf_data.extend(build_conf_line(new_page))
            continue
        for demo_conf_line in demo_conf_data:
            if demo_conf_line.startswith(page[0]):
                os_conf_data.extend(build_conf_line(page[1]))
    return os_conf_data

def handle_melody_demo(root_dir, conf_base, demo_target, demo_config):
    if demo_config:
        demo_conf = os.path.join(root_dir, "application", "melody", demo_config, "config", "%s.config"%demo_config)
    else:
        demo_conf = os.path.join(root_dir, "application", "melody", demo_target, "config", "%s.config"%demo_target)

    out_dir = os.path.join(root_dir, "output", "1a72", "acore", demo_target)

    conf_name = "1a72_app.config"
    conf_file = os.path.join(out_dir, conf_name)

    if os.path.isfile(conf_base) is False:
        raise Exception("%s NOT found!"%conf_base)
        exit(1)

    if os.path.isfile(demo_conf) is False:
        raise Exception("%s NOT found!"%demo_conf)
        exit(1)

    # read app configure
    demo_conf_data = None
    with open(demo_conf, "r") as f_src:
        demo_conf_data = f_src.readlines()

    # read template configure
    base_conf_data = None
    with open(conf_base, "r") as f_base:
        base_conf_data = f_base.readlines()

    # make configure context
    conf_data = build_os_conf_data(demo_conf_data, base_conf_data)

    # write to configure
    with open(conf_file, "w") as f_conf:
        f_conf.writelines(conf_data)

def add_xts_conf_data(config_file, xts_conf):
    xts_conf = xts_conf.split("=", 1)[1].split(";")
    xts_conf = list(filter(lambda x: x.startswith("LOSCFG_"), xts_conf)) + \
        build_os_conf_data(xts_conf, [])
    xts_conf_dict = {}
    for x in xts_conf:
        xts_conf_dict[x.split("=", 1)[0]] = x + "\n"

    conf_data = []
    with open(conf_file, "r") as f_src:
        conf_data = f_src.readlines()

    for i in range(len(conf_data)):
       name = conf_data[i].split("=", 1)[0]
       if name in xts_conf_dict:
           conf_data[i] = xts_conf_dict[name]
           del xts_conf_dict[name]
    conf_data += xts_conf_dict.values()

    with open(conf_file, "w") as f_conf:
        f_conf.writelines(conf_data)

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Parameters invalid!")
        exit(1)

    root_dir = sys.argv[1]
    conf_base = sys.argv[2]
    print("Parameters:", sys.argv)
    demo_target = None
    demo_config = None
    for idx in range(3, len(sys.argv)):
        target_def = sys.argv[idx]
        if target_def == "MELODY_DEMO_BMIC_RX":
            if 'CONFIG_SUPPORT_DISPLAY' in sys.argv:
                demo_target = "demo_bmic_rx_with_gui"
                demo_config = "demo_bmic_rx"
            elif 'XTS_SUPPORT' in sys.argv:
                demo_target = "demo_bmic_rx_with_xts"
                demo_config = "demo_bmic_rx"
            else:
                demo_target = "demo_bmic_rx"
            break
        elif target_def == "MELODY_DEMO_BMIC_TX":
            if 'XTS_SUPPORT' in sys.argv:
                demo_target = "demo_bmic_tx_with_xts"
                demo_config = "demo_bmic_tx"
            else:
                demo_target = "demo_bmic_tx"
            break
        elif target_def == "MELODY_DEMO_SOUND_BOX":
            demo_target = "demo_sound_box"
            break
        elif target_def == "MELODY_DEMO_DONGLE":
            demo_target = "demo_dongle"
            break
        elif target_def == "MELODY_DEMO_SOUNDBAR":
            demo_target = "demo_soundbar"
            break
        elif target_def == "MELODY_DEMO_HA":
            demo_target = "demo_hearing_aid"
            break
        elif target_def == "MELODY_DEMO_EARPHONE":
            if 'XTS_SUPPORT' in sys.argv:
                demo_target = "demo_earphone_with_xts"
                demo_config = "demo_earphone"
            else:
                demo_target = "demo_earphone"
            break
        else:
            continue

    if demo_target is None:
        handle_demo_none(root_dir, conf_base)
    else:
        handle_melody_demo(root_dir, conf_base, demo_target, demo_config)

    if "XTS_SUPPORT" in sys.argv:
        out_dir = os.path.join(root_dir, "output", "1a72", "acore", demo_target)
        conf_name = "1a72_app.config"
        conf_file = os.path.join(out_dir, conf_name)
        xts_conf = list(filter(lambda x: x.startswith("LITEOS_XTS_CONFIG="), sys.argv))[0]
        add_xts_conf_data(conf_file, xts_conf)

    exit(0)
