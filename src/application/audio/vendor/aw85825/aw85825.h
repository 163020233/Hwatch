/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: header file for AW85825 driver
 */

#ifndef AW85825_H
#define AW85825_H

#include "soc_uapi_vendor.h"

/* registers list */
#define AW85825_ID_REG (0x00)
#define AW85825_SYSST_REG (0x01)
#define AW85825_SYSINT_REG (0x02)
#define AW85825_SYSINTM_REG (0x03)
#define AW85825_SYSCTRL_REG (0x04)
#define AW85825_SYSCTRL2_REG (0x05)
#define AW85825_I2SCTRL1_REG (0x06)
#define AW85825_I2SCTRL2_REG (0x07)
#define AW85825_I2SCTRL3_REG (0x08)
#define AW85825_DACCFG1_REG (0x09)
#define AW85825_DACCFG2_REG (0x0A)
#define AW85825_DACCFG3_REG (0x0B)
#define AW85825_DACCFG4_REG (0x0C)
#define AW85825_GPIOCTRL_REG (0x1F)
#define AW85825_TEMP_REG (0x21)
#define AW85825_PVDD_REG (0x22)
#define AW85825_PTC_REG (0x3B)
#define AW85825_CDACTRL1_REG (0x53)
#define AW85825_CDACTRL2_REG (0x54)
#define AW85825_RAMPGEN1_REG (0x56)
#define AW85825_RAMPGEN3_REG (0x58)
#define AW85825_BIASVDD3_REG (0x5F)
#define AW85825_DC_DRIVER_REG (0x63)
#define AW85825_PROTECT_LOG1_REG (0x67)

#define AW85825_CHIP_ID (0x21D9)
#define AW85825_DEV1_SLOT (0x3010)
#define AW85825_DEV2_SLOT (0x3032)
#define I2C_SEND_DATA_SHIFT_COUNT (8)
#define READ_DATA_SIZE (2)
#define REC_I2C_DATA_LEN (2)
#define REG_VAL_BYTE_SIZE (2)
#define REG_UNITE_SHIFT_COUNT (8)
#define AW85825_MIN_VOL (0)
#define AW85825_MAX_VOL (1023)
#define AW85825_BIT_DEPTH_REG_BITS (2)
#define AW85825_BIT_DEPTH_16_VAL (0)
#define AW85825_BIT_DEPTH_24_VAL (2)
#define AW85825_BIT_DEPTH_32_VAL (3)
#define AW_MAX_RAM_WRITE_BYTE_SIZE        (128)

td_s32 audio_aw85825_init(const uapi_vendor_codec_attr *attr);
td_s32 audio_aw85825_deinit(td_void);

#endif
