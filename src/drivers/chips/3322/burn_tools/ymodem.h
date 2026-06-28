/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Provides burn uart port template \n
 * History: \n
 * 2022-06-06， Create file. \n
 */
#ifndef YMODEM_H
#define YMODEM_H

#include "burn_types.h"

#ifdef __cplusplus
extern "C" {
#endif

u16 ymodem_crc16(const u8 *buf, u32 len);

#ifdef __cplusplus
}
#endif

#endif /* YMODEM_H */
