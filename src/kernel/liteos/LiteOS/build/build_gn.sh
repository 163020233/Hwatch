#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
# Description: build bash
set -e

OUT_BIN_DIR=./out/obj/build/gn/bin
CONFIG_FILE="./build/gn/config.gni"
DYNLOAD_GSYMBOL_FILE="./self_src/dynload_dir/dynload_sym/los_dynload_gsymbol.c"
# Check if the toolchain path is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <toolchain_path>"
  exit 1
fi

# Assign the provided path to a variable
TOOLCHAIN_PATH=""
SAFETY_KERNEL=false
VERBOSE=false
INSTALL=false
INSTALLPATH="./out"
COMMERCIAL=false
BEPCHECK=false
PRODUCT_BUILD=false

while [ $# -gt 0 ]; do
  case "$1" in
    -c=*|--compiler=*)
      TOOLCHAIN_PATH="${1#*=}"
      ;;
    -s|--safety)
      SAFETY_KERNEL=true
      ;;
    -v|--verbose)
      VERBOSE=true
      ;;
    -i=*|--install=*)
      packtime=$(grep 'pack_time' build/libck_cmake/common/pack_conf.json | cut -d '"' -f 4)
      version=$(grep 'version' build/libck_cmake/common/pack_conf.json | cut -d '"' -f 4)
      INSTALLPATH="rtos_lite-${1#*=}-${version}-${packtime}"
      INSTALL=true
      ;;
    --commercial)
      COMMERCIAL=true
      ;;
    --bep)
      BEPCHECK=true
      ;;
    -p|--product)
      PRODUCT_BUILD=true
      ;;
    esac
    shift
done

if [ -z "$TOOLCHAIN_PATH" ]; then
  echo "Error: Compiler needs to be specified"
  exit 1
fi

# Export the PATH environment variable
export PATH=$TOOLCHAIN_PATH:$PATH

# Copy the .gn file
cp build/gn/.gn .gn

# Remove the out directory
rm -rf out

# Eliminate BEP differences
if [ "${BEPCHECK}" == "true" ]; then
  source /opt/buildtools/secBepkit-2.2.0/bep_env.sh -s build/bep_env.conf
fi

if [[ -f "build/../self_src/dynload_dir/dynload_sym/los_dynload_gsymbol.c" ]]; then
  has_dynload_gsymbol="true"
else
  has_dynload_gsymbol="false"
fi

grep -v "has_dynload_gsymbol" "$CONFIG_FILE" > tmp && mv tmp "$CONFIG_FILE"
echo "has_dynload_gsymbol = $has_dynload_gsymbol" >> "$CONFIG_FILE"

# Generate build files
gn gen out -v --args="board_toolchain_path=\"$TOOLCHAIN_PATH\" safety_kernel=$SAFETY_KERNEL IS_INSTALL=$INSTALL \
                      IS_COMMERCIAL=$COMMERCIAL INSTALL_PATH=\"$INSTALLPATH\" PRODUCT_BUILD=$PRODUCT_BUILD"

# Build using ninja
if [ "$VERBOSE" == "true" ]; then
  ninja -C ./out/ -d explain -v
else
  ninja -C ./out/ -d explain
fi

if [ "$COMMERCIAL" == "false" ]; then
  # Generate the binary file
  /$TOOLCHAIN_PATH/riscv32/riscv32-linux-musl-objcopy -O binary $OUT_BIN_DIR/vs_server $OUT_BIN_DIR/vs_server.bin
  /$TOOLCHAIN_PATH/riscv32/riscv32-linux-musl-objdump -d -h $OUT_BIN_DIR/vs_server > $OUT_BIN_DIR/vs_server.asm
  /$TOOLCHAIN_PATH/riscv32/riscv32-linux-musl-readelf -a $OUT_BIN_DIR/vs_server > $OUT_BIN_DIR/vs_server.readelf
  /$TOOLCHAIN_PATH/riscv32/riscv32-linux-musl-objdump -t $OUT_BIN_DIR/vs_server |sort > $OUT_BIN_DIR/vs_server.sym.sorted
fi

if [ "$INSTALL" == "true" ]; then
  mkdir out/$INSTALLPATH/libs
  cp -f out/libs/libbase.a  out/$INSTALLPATH/libs/ || true
  cp -f out/libs/libkernel.a  out/$INSTALLPATH/libs/ || true
  cp -f out/libs/libriscv.a  out/$INSTALLPATH/libs/ || true
  cp -f out/libs/libsyscalls.a  out/$INSTALLPATH/libs/ || true
  cp -f out/libs/libusr.a  out/$INSTALLPATH/libs/ || true
  cp -f out/libs/libcppsupport.a  out/$INSTALLPATH/libs/ || true
  cd out
  zip -r ${INSTALLPATH}.zip ${INSTALLPATH}/
fi
