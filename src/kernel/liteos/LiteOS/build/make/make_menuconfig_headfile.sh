#!/bin/bash
# ===============================================================================
# @shell make_menuconfig_headfile.sh
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
# @brief make LiteOS head file(menuconfig.h) depend on .config
# @version 1.0
# ===============================================================================

set -e
config_file="$1"
menuconfig_headfile_file="$2"

sed -e '/^#/d' -e '/^[  ]*$/d' -e 's/=y/ 1/' -e 's/=/ /' -e 's/^/#define &/g' $config_file > $menuconfig_headfile_file
