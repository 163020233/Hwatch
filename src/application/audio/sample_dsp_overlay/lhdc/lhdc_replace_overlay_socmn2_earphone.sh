#===============================================================================
# @brief   Shell script
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
#!/bin/bash

script_path=`dirname "$0"`
chip_type=socmn2
dsp_version_name=earphone
lib_src_name=lhdc_lib0
lib_dest_name=lhdc_dec

overlay_bin_path=`realpath ${script_path}/../../../../interim_binary/socmn2/audio/socmn2_${dsp_version_name}`
dsp_component=dsp_lib.bin

export XTENSA_LSP_FILE=lsp_lhdc
export CFG_SAP_PRODUCT_TYPE=${dsp_version_name}

rm -rf out

#1. copy dsp_component from sdk interim_binary path
mkdir -p out/overlay_bin
cp ${overlay_bin_path}/${dsp_component} out/overlay_bin/${dsp_component}

#2. unpacket dsp_component
python3 ./build/xtensa/tool/unpacket.py out/overlay_bin/${dsp_component}

#3. make
make clean
make -j

#4. replace
rm -rf out/overlay_bin/${lib_dest_name}.bin out/overlay_bin/${dsp_component}
cp out/bin/${lib_src_name}.bin out/overlay_bin/${lib_src_name}.bin
mv out/overlay_bin/${lib_src_name}.bin out/overlay_bin/${lib_dest_name}.bin

#5. packet dsp_component
python3 ./build/xtensa/tool/packet.py out/overlay_bin/${dsp_component} out/overlay_bin/*.bin

#6. copy dsp_component to sdk interim_binary path
cp out/overlay_bin/${dsp_component} ${overlay_bin_path}/${dsp_component}
