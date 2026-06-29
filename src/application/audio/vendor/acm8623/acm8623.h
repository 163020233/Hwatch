/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: header file for ACM8623 driver
 */

#ifndef ACM8623_H
#define ACM8623_H

#include "soc_uapi_vendor.h"

/* registers list */
#define ACM8623_AMP_CTRL1_REG (0x01)
#define ACM8623_AMP_CTRL2_REG (0x02)
#define ACM8623_AMP_CTRL3_REG (0x03)
#define ACM8623_STATE_CTRL_REG (0x04)
#define ACM8623_I2S_FORMATE1_REG (0x07)
#define ACM8623_I2S_FORMATE2_REG (0x08)
#define ACM8623_I2S_FORMATE3_REG (0x09)
#define ACM8623_GPIO1_CTRL_REG (0x0A)
#define ACM8623_GPIO2_CTRL_REG (0x0B)
#define ACM8623_GPIO2_FAULT_SEL_REG (0x0C)
#define ACM8623_GPIO1_FAULT_SEL_REG (0x0D)
#define ACM8623_SS_CTRL_REG (0x0E)
#define ACM8623_VOL_CTRL_L_REG (0x0F)
#define ACM8623_VOL_CTRL_R_REG (0x10)
#define ACM8623_MSIC_CTRL_REG (0x11)
#define ACM8623_I2S_CLK_RPT1_REG (0x12)
#define ACM8623_I2S_CLK_RPT2_REG (0x13)
#define ACM8623_STATE_RPT_REG (0x16)
#define ACM8623_FAULT_RPT1_REG (0x17)
#define ACM8623_FAULT_RPT2_REG (0x18)
#define ACM8623_FAULT_RPT3_REG (0x19)

#define ACM8623_MAX_VOL (16)
#define ACM8623_VOL_BYTES (4)
#define ACM8623_VOL_TAB_COL_0 (0)
#define ACM8623_VOL_TAB_COL_1 (1)
#define ACM8623_VOL_TAB_COL_2 (2)
#define ACM8623_VOL_TAB_COL_3 (3)

td_s32 audio_acm8623_init(const uapi_vendor_codec_attr *attr);
td_s32 audio_acm8623_deinit(td_void);

#endif
