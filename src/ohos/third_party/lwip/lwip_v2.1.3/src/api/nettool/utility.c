/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: Character and Data Conversion API implementation
 * Create:2026
 */
 
#include "lwip/nettool/utility.h"

s32_t lwip_isdigitstr(const s8_t *str)
{
    if (str == NULL || *str == '\0') {
        return -1;
    }
    
    while (*str) {
        if ((*str >= '0') && (*str <= '9')) { /* 0 , 9: Arabic numerals */
            ++str;
            continue;
        }

        return -1;
    }

    return 0;
}
