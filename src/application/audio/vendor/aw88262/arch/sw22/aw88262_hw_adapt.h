/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: source file for module operation
 */


#ifndef __AW88262_HW_ADAPT_H__
#define __AW88262_HW_ADAPT_H__

#include "product_pin.h"

#if defined(SAP_FPGA) /* fpga */
#define PA_PWR_ON_GPIO  PIN_NONE /* ke-todo 实际对应 xx, 详见 product_pin.h */
#define PA_I2C_BUS_ID   0x1 /* chip-I2C2 soft-BUS1 */
#define PA_I2C_ADDR     0x31

#else /* EVB2 */
#define PA_PWR_ON_GPIO  GPIO_056 /* 详见 product_pin.h */
#define PA_I2C_BUS_ID   0x3 /* I2C_BUS_3 对应原理图上 I2C5-SMPA-NFC  */
#define PA_I2C_ADDR     0x30

#endif /* SAP_FPGA */

#define PA_I2SCFG2_REG_VAL 0x2108 /* PA IV data format :16bit*2ch in left channel */
#endif /* __AW88262_HW_ADAPT_H__ */
