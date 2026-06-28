/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: product graphic config
 * Author: Hisi Graphic Team
 * Created: 2025-07
 */
#ifndef PRODUCT_GRAPHIC_LITE_CONFIG_H
#define PRODUCT_GRAPHIC_LITE_CONFIG_H
#ifndef _WIN32
#include "chip_definitions.h"
#endif

#if defined(TARGET_CHIP_SOCMN2) && (TARGET_CHIP_SOCMN2 == 1)
#include "../../config/socmn2_graphic_lite_config.h"
#elif defined(_WIN32)
#include "graphic_lite_config_simulator.h"
#else
#include "../../config/3322_graphic_lite_config.h"
#endif
#endif