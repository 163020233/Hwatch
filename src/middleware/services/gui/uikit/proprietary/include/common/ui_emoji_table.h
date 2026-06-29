/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: UIEmojiTable
 * Author: Hisi Graphic Team
 * Created: 2024-05
 */

#ifndef UI_EMOJI_TABLE_H
#define UI_EMOJI_TABLE_H
#include "graphic_config.h"

namespace OHOS {
struct EmojiTable {
    uint32_t unicode;
    uint32_t resId;
};

int GetEmojiTableLen();
bool IsEmojiSupport();
const EmojiTable *GetEmojiTable();
const char *GetEmojiPath();
} // OHOS

#endif // UI_EMOJI_TABLE_H