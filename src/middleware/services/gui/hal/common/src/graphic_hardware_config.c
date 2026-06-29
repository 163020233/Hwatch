/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: graphic_hardware_config
 * Create: 2026-04-29
 */

#include "graphic_hardware_config.h"

bool IsFBCompress(void)
{
#if defined(ENABLE_FB_COMPRESS) && (ENABLE_FB_COMPRESS == 1)
    return true;
#else
    return false;
#endif
}

