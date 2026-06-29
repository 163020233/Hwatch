/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: header file for module operation
 */


#ifndef __AW88262_HW_ADAPT_H__
#define __AW88262_HW_ADAPT_H__

#include "product_pin.h"

#if defined(SAP_FPGA) /* 3322 fpga */
#define PA_PWR_ON_GPIO  S_AGPIO34
#define PA_I2C_BUS_ID   0x3 /* I2C3 */
#define PA_I2C_ADDR     0x32

#else /* 3322 EVB2 */
#define PA_PWR_ON_GPIO  S_AGPIO2
#define PA_I2C_BUS_ID   0x2 /* I2C2 */
#define PA_I2C_ADDR     0x30

#endif /* SAP_FPGA */

#define PA_I2SCFG2_REG_VAL 0x210C /* PA IV data format :16bit*2ch in low 16bit of 32bit*2ch stereo channel */
#endif /* __AW88262_HW_ADAPT_H__ */
