# !/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.

workdir=$(cd $(dirname $0); pwd)
artget pull -d ${workdir}/depend_binary.xml -ap ${workdir}
tar -xzvf ${workdir}/binary/vsix.tar.gz -C ${workdir}
rm -rf ${workdir}/hal/vau/lib
cp -r ${workdir}/vsix/dll ${workdir}/hal/vau/lib
rm -rf ${workdir}/vsix
rm -rf ${workdir}/binary