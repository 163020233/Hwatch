# !/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.

workdir=$(cd $(dirname $0); pwd)
bash ${workdir}/replace_simulator_vau_dll.sh
cp -r ${workdir}/../../wearable/res ${workdir}/
rm -rf ${workdir}/res/js/certcfg
cp -r ${workdir}/../../tools/graphic_tools/map_convert_tool ${workdir}/
cd ${workdir}/../../sdl
rm -rf .git
CFLAGS="-fstack-protector-strong" LDFLAGS="-lssp" ./configure --host=x86_64-w64-mingw32 --prefix=${workdir}/../../sdl/out
make -j16
make install
cp -r ${workdir}/../../sdl/include ${workdir}/third_party/SDL2/
rm -rf ${workdir}/third_party/SDL2/lib/x64
mkdir ${workdir}/third_party/SDL2/lib/x64
cp ${workdir}/../../sdl/out/lib/libSDL2.a ${workdir}/third_party/SDL2/lib/x64
cp ${workdir}/../../sdl/out/lib/libSDL2.dll.a ${workdir}/third_party/SDL2/lib/x64
cp ${workdir}/../../sdl/out/lib/libSDL2main.a ${workdir}/third_party/SDL2/lib/x64
cp ${workdir}/../../sdl/out/bin/SDL2.dll ${workdir}/third_party/SDL2/lib/x64
mkdir ${workdir}/../../out
cd ${workdir}/../simulator
zip -q -r ${workdir}/../../out/simulator.zip *
