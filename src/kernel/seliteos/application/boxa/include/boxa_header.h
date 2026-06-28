/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * filename:boxa_header.h
 * Author: @CompanyNameTag
 * Create:2024-04-01
 * Description: boxa_header
 *
 */

#ifndef _BOXA_HEADER_H_
#define _BOXA_HEADER_H_

#define TA_UUID                                            \
    {                                                      \
        0x426e8f6f, 0xec74, 0x427b,                        \
        {                                                  \
            0x94, 0x24, 0x0d, 0xd8, 0xf3, 0xf3, 0x37, 0x06 \
        }                                                  \
    }

enum ta_command {
    TA_HELLO_TEST = 0,
};

#endif /* _BOXA_HEADER_H_ */
