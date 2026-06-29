#===============================================================================
# @brief   Shell script
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
#!/bin/sh

if [ $# -lt 5 ]; then
    exit -1
fi

DIR=`dirname $0`

SIGN_TOOL=${1}
SIGN_TYPE=${2}
IMAGE_ID=${3}
BIN_FILE=${4}
SIGNED_FILE=${5}

if [ ${SIGN_TYPE} == "cbg" ]; then
python3 ${SIGN_TOOL} -t ${IMAGE_ID} -i ${BIN_FILE} -o ${SIGNED_FILE} -a
fi
