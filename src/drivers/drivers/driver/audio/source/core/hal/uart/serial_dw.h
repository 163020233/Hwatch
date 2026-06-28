/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: driver for PL010/PL011 UARTs
 * Author: audio
 */

#ifndef __SERIAL_DW_H__
#define __SERIAL_DW_H__

#include "audio_type.h"

td_s32 serial_init(td_void);
td_void serial_puts(const td_char *s);
td_void serial_putc(const td_char ch);
td_s32 serial_getc(td_void);

#endif /* __SERIAL_DW_H__ */
