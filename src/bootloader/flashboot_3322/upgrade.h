/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Upgrade relative component.
 * Author: HiSilicon
 * Create: 2025-05-26
 */

#ifndef UPGREADE_H
#define UPGREADE_H
#include "upg_common_porting.h"

core_images_e ssb_get_jump_image(void);
errcode_t boot_upg_start(void);
errcode_t boot_alloc_and_get_upgrade_flag(fota_store_flag_area_t **upg_flag);
void boot_upg_init(void);
void boot_upg_handle(void);

#endif