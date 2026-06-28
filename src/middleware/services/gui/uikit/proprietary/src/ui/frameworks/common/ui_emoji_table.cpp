/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: UIEmojiTable
 * Author: Hisi Graphic Team
 * Created: 2026-03
 */

#include "common/ui_emoji_table.h"

namespace OHOS {
namespace {
#if defined (CAPABILITY_HW_SUPPORT_EMOJI) && (CAPABILITY_HW_SUPPORT_EMOJI != 0)
const char *EMOJI_PATH = RES_PATH"emoji.bin";
const EmojiTable EMOJI_TABLE[] = {
    {0x1f600, 0x100001},
    {0x1f601, 0x100002},
    {0x1f602, 0x100003},
    {0x1f603, 0x100004},
    {0x1f604, 0x100005},
    {0x1f605, 0x100006},
    {0x1f606, 0x100007},
    {0x1f607, 0x100008},
    {0x1f608, 0x100009},
    {0x1f609, 0x10000a},
    {0x1f60a, 0x10000b},
    {0x1f60b, 0x10000c},
    {0x1f60c, 0x10000d},
    {0x1f60d, 0x10000e},
    {0x1f60e, 0x10000f},
    {0x1f60f, 0x100010},
    {0x1f610, 0x100011},
    {0x1f611, 0x100012},
    {0x1f612, 0x100013},
    {0x1f613, 0x100014},
    {0x1f614, 0x100015},
    {0x1f615, 0x100016},
    {0x1f616, 0x100017},
    {0x1f617, 0x100018},
    {0x1f618, 0x100019},
    {0x1f619, 0x10001a},
    {0x1f61a, 0x10001b},
    {0x1f61b, 0x10001c},
    {0x1f61c, 0x10001d},
    {0x1f61d, 0x10001e},
    {0x1f61e, 0x10001f},
    {0x1f61f, 0x100020},
    {0x1f620, 0x100021},
    {0x1f621, 0x100022},
    {0x1f622, 0x100023},
    {0x1f623, 0x100024},
    {0x1f624, 0x100025},
    {0x1f625, 0x100026},
    {0x1f626, 0x100027},
    {0x1f627, 0x100028},
    {0x1f628, 0x100029},
    {0x1f629, 0x10002a},
};
#else
const char *EMOJI_PATH = nullptr;
const EmojiTable *EMOJI_TABLE = nullptr;
#endif
}

int GetEmojiTableLen()
{
#if defined (CAPABILITY_HW_SUPPORT_EMOJI) && (CAPABILITY_HW_SUPPORT_EMOJI != 0)
    return sizeof(EMOJI_TABLE) / sizeof(EMOJI_TABLE[0]);
#else
    return 0;
#endif
}

bool IsEmojiSupport()
{
#if defined (CAPABILITY_HW_SUPPORT_EMOJI) && (CAPABILITY_HW_SUPPORT_EMOJI != 0)
    return true;
#else
    return false;
#endif
}

const EmojiTable *GetEmojiTable()
{
#if defined (CAPABILITY_HW_SUPPORT_EMOJI) && (CAPABILITY_HW_SUPPORT_EMOJI != 0)
    return EMOJI_TABLE;
#else
    return nullptr;
#endif
}

const char *GetEmojiPath()
{
#if defined (CAPABILITY_HW_SUPPORT_EMOJI) && (CAPABILITY_HW_SUPPORT_EMOJI != 0)
    return EMOJI_PATH;
#else
    return nullptr;
#endif
}
} // namespace OHOS
