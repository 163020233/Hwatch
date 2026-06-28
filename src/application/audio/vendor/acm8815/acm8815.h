/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: header file for ACM8815 driver
 */

#ifndef ACM8815_H
#define ACM8815_H

#include "soc_uapi_vendor.h"

/* registers list */
#define ACM8815_AMP_CTRL1_REG (0x01)
#define ACM8815_AMP_CTRL2_REG (0x02)
#define ACM8815_AMP_CTRL3_REG (0x03)
#define ACM8815_STATE_CTRL_REG (0x04)
#define ACM8815_I2S_FORMATE1_REG (0x07)
#define ACM8815_I2S_FORMATE2_REG (0x08)
#define ACM8815_GPIO0_CTRL_REG (0x0A)
#define ACM8815_GPIO1_CTRL_REG (0x0B)
#define ACM8815_GPIO0_FAULT_SEL_REG (0x0C)
#define ACM8815_GPIO1_FAULT_SEL_REG (0x0D)
#define ACM8815_SS_CTRL_REG (0x0E)
#define ACM8815_VOL_CTRL_REG (0x0F)
#define ACM8815_MSIC_CTRL_REG (0x11)
#define ACM8815_I2S_CLK_RPT1_REG (0x12)
#define ACM8815_I2S_CLK_RPT2_REG (0x13)
#define ACM8815_STATE_RPT_REG (0x16)
#define ACM8815_FAULT_RPT1_REG (0x17)
#define ACM8815_FAULT_RPT2_REG (0x18)

#define ACM8815_MAX_VOL (16)
#define ACM8815_VOL_BYTES (4)
#define ACM8815_VOL_TAB_COL_0 (0)
#define ACM8815_VOL_TAB_COL_1 (1)
#define ACM8815_VOL_TAB_COL_2 (2)
#define ACM8815_VOL_TAB_COL_3 (3)

td_s32 audio_acm8815_init(const uapi_vendor_codec_attr *attr);
td_s32 audio_acm8815_deinit(td_void);

#endif
