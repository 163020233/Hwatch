/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: source file for module operation
 */


#ifndef __AW88262_HW_ADAPT_H__
#define __AW88262_HW_ADAPT_H__

#include "platform_core.h"

#if defined(BRANDY_PRODUCT_EVB4) /* brandy EVB4 */
#define PA_PWR_ON_GPIO  S_AGPIO_L25 /* 实际对应 S_AGPIO61, 详见 product_pin.h */
#define PA_I2C_BUS_ID   0x3
#define PA_I2C_ADDR     0x30

#else /* brandy EVB2 */
#define PA_PWR_ON_GPIO  S_MGPIO4
#define PA_I2C_BUS_ID   0x0
#define PA_I2C_ADDR     0x30

#endif /* SAP_FPGA */

#define PA_I2SCFG2_REG_VAL 0x210C /* PA IV data format :16bit*2ch in low 16bit of 32bit*2ch stereo channel */
#endif /* __AW88262_HW_ADAPT_H__ */
