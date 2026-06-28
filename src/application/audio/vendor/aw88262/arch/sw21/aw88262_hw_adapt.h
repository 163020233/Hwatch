/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: source file for module operation
 */


#ifndef __AW88262_HW_ADAPT_H__
#define __AW88262_HW_ADAPT_H__

#include "product_pin.h"

#if defined(SAP_FPGA) /* sw21 fpga */
#define PA_PWR_ON_GPIO  S_AGPIO61 /* 实际对应 S_AGPIO61, 详见 product_pin.h */
#define PA_I2C_BUS_ID   0x2 /* I2C2 */
#define PA_I2C_ADDR     0x31

#else /* sw21 EVB2 */
#define PA_PWR_ON_GPIO  S_AGPIO49 /* 实际对应 S_AGPIO49, 详见 product_pin.h */
#define PA_I2C_BUS_ID   0x4 /* I2C4 */
#define PA_I2C_ADDR     0x30

#endif /* SAP_FPGA */

#define PA_I2SCFG2_REG_VAL 0x2108 /* PA IV data format :16bit*2ch in left channel */
#endif /* __AW88262_HW_ADAPT_H__ */
