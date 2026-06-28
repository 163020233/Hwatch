/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: haid_tone.c header file
 * Author : AuthorNameMagicTag
 * Create : 2023-07-03
 */

#ifndef HAID_TONE_H
#define HAID_TONE_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void haid_notify_change_mode_tone(uint8_t mode, uint8_t tws_flag);
void haid_notify_vol_up_tone(void);
void haid_notify_vol_down_tone(void);

#ifdef __cplusplus
}
#endif

#endif