/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: display gfx internal
 * Author: Hisi Graphic Team
 * Created: 2025-10
 */

#ifndef DISPLAY_GFX_INTERNAL_H
#define DISPLAY_GFX_INTERNAL_H
#include "display_common.h"
#include "display_type.h"
#include "soc_gfx_type.h"

#define TIME_OUT 1000

struct ColorFmtConvert {
    PixelFormat pixFmt;
    ext_gfx_fmt vauFmt;
};

#endif /* DISPLAY_GFX_INTERNAL_H */
