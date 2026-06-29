#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
# Description: build bash
set -e

product=''
BEPCHECK=false
output="output"
bepversion="secBepkit-2.2.0"

if [ -z "$1" ]; then
    echo "The first parameter is missing. The first parameter is a product domain, for example, digitalpower."
    exit 1
else
    product=$1
    BEPCHECK=true
fi

digitalpower=("hi1230" "sd5091mxs" "sd5091linx" "sd5091" "sd5012" "gd32f527")

BOARD_TYPES=()

if [ "${product}" = "digitalpower" ]; then
    BOARD_TYPES=("${digitalpower[@]}")
else
    echo "Invalid parameter. Options: digitalpower."
    exit 1
fi

RTOS_LITE_HOME=$(pwd | sed 's/\/build.*//g')

pushd $RTOS_LITE_HOME
if [ "${BEPCHECK}" == "true" ]; then
    source /opt/buildtools/${bepversion}/bep_env.sh -s build/bep_env.conf
fi

for BOARD_TYPE in "${BOARD_TYPES[@]}"; do
    source ${RTOS_LITE_HOME}/build/libck_cmake/common/common_function_prepare_env.sh
    prepare_env ${RTOS_LITE_HOME} ${BOARD_TYPE}
    export PATH=${HCC_PATH}:${HCC_PATH}/riscv32:$PATH

	cp build/libck_image/${BOARD_TYPE}_app.config .config
    if ! grep -q '^CONFIG_OS_HARDWARE_PLATFORM=' .config; then
        echo "CONFIG_OS_HARDWARE_PLATFORM not found."
    exit 1
    fi

    CONFIG_VALUE=$(grep '^CONFIG_OS_HARDWARE_PLATFORM' .config | cut -d= -f2)

    # 去掉双引号
    CONFIG=$(echo "${CONFIG_VALUE}" | tr -d '"')

    # 去掉开头的OS_并转为小写
    CPU_TYPE=$(echo "$CONFIG" | sed 's/^OS_//' | tr '[:upper:]' '[:lower:]')

    ./build/build_cmake.sh clean
	./build/build_cmake.sh uni_install ${BOARD_TYPE} ${CPU_TYPE}

done

packtime=$(grep 'pack_time' build/libck_cmake/common/pack_conf.json | cut -d '"' -f 4)
version=$(grep 'version' build/libck_cmake/common/pack_conf.json | cut -d '"' -f 4)
zip_name="rtos_lite-${1#*=}-${version}-${packtime}"
popd

outputdir="${RTOS_LITE_HOME}/$output"
pushd $outputdir
zip -r ${zip_name}.zip ./*
for file in ${outputdir}/*; do
    if [[ "$file" != *.zip ]]; then
        rm -r "$file"
    fi
done

popd