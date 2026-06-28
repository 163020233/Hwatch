#===============================================================================
# @brief   Shell script
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
#!/bin/bash

# 定义源目录和目标目录
SOURCE_APP_DIR="$WORKSPACE/output/3322/acore"
TARGET_APP_DIR="$WORKSPACE/output/3322_CI/acore"

# 创建目标目录（如果不存在）
mkdir -p "$TARGET_APP_DIR"

# 函数：拷贝指定目录中的部分文件（保留子目录结构）
copy_filtered_dir() {
    local dir_name="$1"
    rsync -avm \
          --include="*/" \
          --include="application.elf" --include="application.lst" --include="application.map"\
          --include="bt.elf" --include="bt.lst" --include="bt.map"\
          --include="loaderboot.elf" --include="loaderboot.lst" --include="loaderboot.map"\
		  --include="recovery.elf" --include="recovery.lst" --include="recovery.map"\
		  --include="seliteos.elf" --include="seliteos.lst" --include="seliteos.map"\
		  --include="ssb.elf" --include="ssb.lst" --include="ssb.map"\
          --exclude="*"\
          "$SOURCE_APP_DIR/$dir_name"/ "$TARGET_APP_DIR/$dir_name"
}

copy_filtered_dir "3322-wstp-app/"
copy_filtered_dir "3322-native-js/"


# 定义源目录和目标目录
SOURCE_DIR="$WORKSPACE/output/3322"
TARGET_DIR="$WORKSPACE/output/3322_CI"
# 创建target/fw目录（如果不存在）
mkdir -p "$TARGET_DIR/fwpkg"
mkdir -p "$TARGET_DIR/dsp"
mkdir -p "$TARGET_DIR/bt"

if [ -f "$SOURCE_DIR/fwpkg/3322-wstp-app.fwpkg" ]; then
    cp "$SOURCE_DIR/fwpkg/3322-wstp-app.fwpkg" "$TARGET_DIR/fwpkg/"
    echo "已拷贝3322-wstp-app.fwpkg文件到target/fwpkg"
else
    echo "警告: $SOURCE_DIR/fwpkg/3322-wstp-app.fwpkg 文件不存在"
fi

if [ -f "$SOURCE_DIR/fwpkg/3322-native-js.fwpkg" ]; then
    cp "$SOURCE_DIR/fwpkg/3322-native-js.fwpkg" "$TARGET_DIR/fwpkg/"
    echo "已拷贝3322-native-js.fwpkg文件到target/fwpkg"
else
    echo "警告: $SOURCE_DIR/fwpkg/3322-native-js.fwpkg 文件不存在"
fi

if [ -f "$WORKSPACE/interim_binary/3322/audio/3322_evb/dsp_main.bin" ]; then
    cp "$WORKSPACE/interim_binary/3322/audio/3322_evb/dsp_main.bin" "$TARGET_DIR/dsp/"
    echo "已拷贝dsp_main.bin文件到$TARGET_DIR/dsp/"
else
    echo "警告: $WORKSPACE/interim_binary/3322/audio/3322_evb/dsp_main.bin 文件不存在"
fi

if [ -f "$WORKSPACE/interim_binary/3322/audio/3322_evb/dsp_overlay.bin" ]; then
    cp "$WORKSPACE/interim_binary/3322/audio/3322_evb/dsp_overlay.bin" "$TARGET_DIR/dsp/"
    echo "dsp_overlay.bin文件到$TARGET_DIR/dsp/"
else
    echo "警告: $WORKSPACE/interim_binary/3322/audio/3322_evb/dsp_overlay.bin 文件不存在"
fi

if [ -f "$SOURCE_DIR/bt_core/standard-3322-bt/bt_signed.bin" ]; then
    cp "$SOURCE_DIR/bt_core/standard-3322-bt/bt_signed.bin" "$TARGET_DIR/bt/"
    echo "已拷贝bt_signed.bin文件到$TARGET_DIR/bt/"
else
    echo "警告: $SOURCE_DIR/bt_core/standard-3322-bt/bt_signed.bin 文件不存在"
fi
echo "操作完成！"