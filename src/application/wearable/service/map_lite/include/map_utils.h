/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-11
 */

#ifndef UI_MAP_UTILS_H
#define UI_MAP_UTILS_H
#include <stdint.h>
#include "mal_common_defines.h"
#include "mal_map_defines.h"
#include "gfx_utils/graphic_types.h"

namespace OHOS {

mal_pixel_mode_t getMapColorMode();

OHOS::ColorMode ConvertPixelModeToColorMode(mal_pixel_mode_t mode);

mal_pixel_mode_t ConventColorModeToPixelMode(OHOS::ColorMode mode);

uint8_t GetBytesPerPixelByMalMode(mal_pixel_mode_t mode);

bool HasAlpha(mal_pixel_mode_t mode);

/**
 * 由调用方外部释放mal_map_texture_data_t数据， 导入图片纹理数据
 */
bool LoadTexture(const char *fileName, mal_map_texture_data_t *texture);
}  // namespace OHOS

#endif