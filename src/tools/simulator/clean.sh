# !/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.

workdir=$(cd $(dirname $0); pwd)
cd ${workdir}/../..
# 获取第一个参数
PARAM=$1
# 根据参数执行不同操作
case $PARAM in
    "uikit")
        rm -rf simulator_build
        rm -rf ${workdir}/../../interim_binary/3322/libs/uikit/
        ;;
    "sdl")
        rm -rf ${workdir}/../../interim_binary/3322/libs/SDL2/
        ;;
    *)
        echo "无效的参数，请使用 'uikit'"
        exit 1
        ;;
esac