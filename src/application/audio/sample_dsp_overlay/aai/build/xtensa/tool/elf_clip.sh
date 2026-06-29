#===============================================================================
# @brief   Shell script
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
#!/bin/sh

if [ $# != 2 ]; then
    exit -1
fi

DIR=`dirname $0`
python3 $DIR/clip.py $*
