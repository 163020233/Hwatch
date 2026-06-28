#===============================================================================
# @brief   Shell script
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================

source_dir="$WORKSPACE/output/3322"
target_dir="$WORKSPACE/output/release_pkg/3322_cmc"

# 检查源目录是否存在
if [ ! -d "$source_dir" ]; then
    echo "error: source_dir $source_dir not exist"
    exit 1
fi

# 创建目标目录
mkdir -p "$target_dir"

# 函数：拷贝指定目录（/acore 和 /bt_core）中的部分文件（保留子目录结构）
copy_filtered_dir() {
    local dir_name="$1"
    rsync -avm \
          --include="*/" \
          --include="application.bin" --include="application.elf" --include="application.lst" --include="application.map" --include="application_signed.bin" \
          --include="bt.bin" --include="bt_signed.bin" --include="bt.elf" --include="bt.lst" \
          --include="loaderboot.bin" --include="loaderboot.elf" --include="loaderboot.lst" --include="loaderboot.map" --include="loaderboot_signed.bin" \
		  --include="recovery.bin" --include="recovery.elf" --include="recovery.lst" --include="recovery.map" --include="recovery_signed.bin" \
		  --include="seliteos.bin" --include="seliteos.elf" --include="seliteos.lst" --include="seliteos.map" --include="seliteos_signed.bin" \
		  --include="ssb.bin" --include="ssb.elf" --include="ssb.lst" --include="ssb.map" --include="ssb_signed.bin" \
		  --include="ssb_backup_signed.bin"\
          --exclude="*"\
          "$source_dir/$dir_name"/ "$target_dir/$dir_name"
}

# 拷贝 a 和 b 目录
copy_filtered_dir "acore"
copy_filtered_dir "bt_core"

# 拷贝 s 下的其他所有内容（排除 acore 和 bt_core 目录）
echo "copying $source_dir/ 下的其他所有内容..."
rsync -av \
      --exclude="/acore/" \
      --exclude="/bt_core/" \
      "$source_dir"/ "$target_dir"

echo "success, target_dir: $target_dir"

source_sdk_dir="$WORKSPACE/output/sdk"
target_sdk_dir="$WORKSPACE/output/release_pkg/sdk"

echo "copying $source_sdk_dir/ 下的所有内容..."
rsync -av \
      "$source_sdk_dir"/ "$target_sdk_dir"

echo "success, target_dir: $target_sdk_dir"

# 拷贝资源文件
source_res_dir="$WORKSPACE/output/release_pkg/sdk/application/wearable/res"
target_res_dir="$WORKSPACE/output/res"

mkdir -p "$target_res_dir"

echo "copying $source_res_dir/ 下的所有内容..."
rsync -av \
      "$source_res_dir"/ "$target_res_dir"

echo "success, target_dir: $target_res_dir"


# 产测版本
INNER_VERSION='HIDITINGV100-22-CG-C'
PRODUCTION_VERSION="${1}"
FT_VERSION="${INNER_VERSION}${PRODUCTION_VERSION}"

mkdir -p "$WORKSPACE/output/${FT_VERSION}"
mkdir -p "$WORKSPACE/output/FT_RELEASE"

cp "$WORKSPACE/output/3322/fwpkg/3322-diting-ft.fwpkg" "$WORKSPACE/output/${FT_VERSION}/"
cp "$WORKSPACE/output/3322/acore/3322-loaderboot/loaderboot_signed.bin" "$WORKSPACE/output/${FT_VERSION}/"
cp "$WORKSPACE/interim_binary/3322/volte/firmwares/Hi2131EV100.fwpkg" "$WORKSPACE/output/${FT_VERSION}/"

tar -zcvf "$WORKSPACE/output/${FT_VERSION}.tar.gz" "$WORKSPACE/output/${FT_VERSION}/"

cp "$WORKSPACE/output/${FT_VERSION}.tar.gz" "$WORKSPACE/output/FT_RELEASE"
