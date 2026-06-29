/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Provides burn uart port template \n
 * History: \n
 * 2022-06-06， Create file. \n
 */
#include "ymodem.h"

u16 ymodem_crc16(const u8 *buf, u32 len)
{
    u16 cksum = 0;
    u32 stat = len;
    const u8 *in_ptr = buf;
    while (stat > 0) {
        cksum = (u16)(cksum ^ ((u16)(*in_ptr++) << 0x8));
        for (int i = 0; i < 8; i++) { // len 8
            if ((cksum & 0x8000) != 0) {
                cksum = (u16)((cksum << 1) ^ 0x1021);
            } else {
                cksum = (u16)(cksum << 1);
            }
        }
        stat--;
    }
    return cksum;
}
