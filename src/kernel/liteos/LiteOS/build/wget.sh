#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
# Description: rt-thread download bash

REPO_URL="https://github.com/RT-Thread/rt-thread/archive/refs/tags/v5.2.1.zip"
TARGET_DIR="../open_source"
ZIP_FILE="rt-thread.zip"
EXTRACTED_DIR="rt-thread-5.2.1"
FINAL_DIR="rt-thread"

if [ -d "$TARGET_DIR/$FINAL_DIR" ]; then
    echo "RT-Thread v5.2.1 dir already exists: $TARGET_DIR/$FINAL_DIR"
    exit 0
fi

cd "$TARGET_DIR" || { echo "Failed to enter directory $TARGET_DIR"; exit 1; }

wget --no-check-certificate "$REPO_URL" -O rt-thread.zip || { echo "Failed to download $REPO_URL"; exit 1; }

unzip "$ZIP_FILE" || { echo "Failed to unzip $ZIP_FILE"; exit 1; }

mv "$EXTRACTED_DIR" "$FINAL_DIR" || { echo "Failed to rename $EXTRACTED_DIR to $FINAL_DIR"; exit 1; }

rm "$ZIP_FILE"

echo "RT-Thread v5.2.1 dir :$TARGET_DIR/$FINAL_DIR"
