# !/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.

workdir=$(cd $(dirname $0); pwd)
cd ${workdir}/../..
if [ -e "tools/simulator/CMakeLists.txt.simulator" ]; then
    mv CMakeLists.txt CMakeLists.txt.sdk
    mv tools/simulator/CMakeLists.txt.simulator CMakeLists.txt
fi
pwd
if [ $# -eq 0 ]; then
    mkdir -p simulator_build
    cmake . -B simulator_build -DBUILD_SIMULATOR=ON -DUSE_BACKGROUNDIMAGE=ON -DCMAKE_BUILD_TYPE=Debug -DRELEASE_VERSION=ON -DBUILD_IN_LINUX=ON
    cd simulator_build
    make -j32
    cd ..
else
    # 获取第一个参数
    PARAM=$1
    # 根据参数执行不同操作
    case $PARAM in
        "uikit")
            mkdir -p simulator_build
            cmake . -B simulator_build -DBUILD_UIKIT=ON -DUSE_BACKGROUNDIMAGE=ON -DCMAKE_BUILD_TYPE=Debug -DRELEASE_VERSION=ON -DBUILD_IN_LINUX=ON
            cd simulator_build
            make -j32
            cd ..
            mkdir -p tools/bin/ohos_tools/simulator
            cp -r tools/simulator/uikit ${workdir}/../../interim_binary/3322/libs/uikit
            ;;
        "sdl")
            cd open_source/sdl
            CFLAGS="-fstack-protector-strong" LDFLAGS="-lssp" ./configure --host=x86_64-w64-mingw32 --prefix=${workdir}/../../open_source/sdl/out
            make -j16
            make install
            mkdir -p ${workdir}/../../interim_binary/3322/libs/SDL2/lib/x64
            cp ${workdir}/../../open_source/sdl/out/lib/libSDL2.a ${workdir}/../../interim_binary/3322/libs/SDL2/lib/x64
            cp ${workdir}/../../open_source/sdl/out/lib/libSDL2.dll.a ${workdir}/../../interim_binary/3322/libs/SDL2/lib/x64
            cp ${workdir}/../../open_source/sdl/out/lib/libSDL2main.a ${workdir}/../../interim_binary/3322/libs/SDL2/lib/x64
            cp ${workdir}/../../open_source/sdl/out/bin/SDL2.dll ${workdir}/../../interim_binary/3322/libs/SDL2/lib/x64
            ;;
        *)
            echo "无效的参数，请使用 'uikit'"
            exit 1
            ;;
    esac
fi
if [ -e "CMakeLists.txt.sdk" ]; then
    mv CMakeLists.txt tools/simulator/CMakeLists.txt.simulator
    mv CMakeLists.txt.sdk CMakeLists.txt
fi
