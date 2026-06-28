/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GRAPHIC_HARDWARE_CONFIG_H
#define GRAPHIC_HARDWARE_CONFIG_H

#include <stdbool.h>
#ifndef _WIN32
#include "chip_definitions.h"
#endif

#if defined(TARGET_CHIP_SOCMN2) && (TARGET_CHIP_SOCMN2 == 1)
#include "graphic_hardware_config_socmn2.h"
#elif defined(_WIN32)
#include "graphic_hardware_config_simulator.h"
#else
#include "graphic_hardware_config_wearable.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get state of fb compress.
 *
 */
bool IsFBCompress(void);

#ifdef __cplusplus
}
#endif
#endif // GRAPHIC_HARDWARE_CONFIG_H
