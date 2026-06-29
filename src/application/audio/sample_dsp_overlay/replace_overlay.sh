#===============================================================================
# @brief   Shell script
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
###########################################################################
 # @File Name            :   aef_replace_overlay.sh
 # @Author               :   audio
 # @Date                 :   2023-08-16 22:00:00
 # @Last Modified time   :   2023-08-26 16:49:16
 # @Description          :   aef_replace_overlay
 ##########################################################################
#!/bin/bash

script_path=`dirname "$0"`
chip_type=$1
dsp_version_name=$2
lib_src_name=$3
lib_dest_name=$4

case ${chip_type} in
    "brandy")
        overlay_bin_path=../../../../tools/pkg/bin/brandy/dsp/${dsp_version_name}
        dsp_component=dsp_overlay.bin
        ;;
    "sw21")
        overlay_bin_path=../../../../interim_binary/sw21/bin/audio/sw21_${dsp_version_name}
        dsp_component=dsp_overlay.bin
        ;;
    "socmn2")
        overlay_bin_path=../../../../interim_binary/socmn2/audio/socmn2_${dsp_version_name}
        dsp_component=dsp.bin
        ;;
    *)
        echo "invalid chip type: ${chip_type}, please use the following chip type:"
        echo "brandy"
        echo "sw21"
        echo "socmn2"
        echo "sample: ${script_path}/replace_overlay.sh sw21 ak_user_evb aef_lib0 aef"
        exit 0
        ;;
esac

case ${lib_dest_name} in
    "aef")
        lib_dest_path="aef"
        ;;
    "see_1mic")
        lib_dest_path="sea"
        ;;
    "phs_gru")
        lib_dest_path="aai"
        ;;
    *)
        echo "invalid lib dest name: ${lib_dest_name}, please use the following lib dest name:"
        echo "aef"
        echo "see_1mic"
        echo "phs_gru"
        echo ""
        echo "sample: ${script_path}/replace_overlay.sh ${chip_type} ak_user_evb aef_lib0 aef"
        exit 0
        ;;
esac

project_path=${script_path}/${lib_dest_path}

pushd ${project_path}

#1. copy dsp_overlay.bin from overlay_bin_path
mkdir out/overlay_bin -p
cp ${overlay_bin_path}/${dsp_component} out/overlay_bin/${dsp_component}

#2. unpacket dsp_overlay.bin
python3 ./build/xtensa/tool/unpacket.py out/overlay_bin/${dsp_component}

#3. make
make clean
make lib
make all

#4. replace
rm -rf out/overlay_bin/${lib_dest_name}.bin out/overlay_bin/${dsp_component}
cp out/bin/${lib_src_name}.bin out/overlay_bin/${lib_src_name}.bin
mv out/overlay_bin/${lib_src_name}.bin out/overlay_bin/${lib_dest_name}.bin

#5. re packet dsp_overlay.bin
python3 ./build/xtensa/tool/packet.py out/overlay_bin/${dsp_component} out/overlay_bin/*.bin

#6. copy dsp_overlay.bin to overlay_bin_path
cp out/overlay_bin/${dsp_component} ${overlay_bin_path}/${dsp_component}
rm -rf out/overlay_bin

popd
