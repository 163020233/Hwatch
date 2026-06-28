/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux utils \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-19， Create file. \n
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ux_common_define.h"
#include "ux_utils.h"

uint64_t ux_utils_uint8_to_uint64(uint8_t *p, uint32_t size)
{
    if (p == NULL || size != UX_UINT64_BYTE_NUM) {
        return 0;
    }
    return (uint64_t)(((uint64_t)(*(p + UX_BYTE_INDEX_0)) << (UX_BIT_INDEX_56)) |
                      ((uint64_t)(*(p + UX_BYTE_INDEX_1)) << (UX_BIT_INDEX_48)) |
                      ((uint64_t)(*(p + UX_BYTE_INDEX_2)) << (UX_BIT_INDEX_40)) |
                      ((uint64_t)(*(p + UX_BYTE_INDEX_3)) << (UX_BIT_INDEX_32)) |
                      ((uint64_t)(*(p + UX_BYTE_INDEX_4)) << (UX_BIT_INDEX_24)) |
                      ((uint64_t)(*(p + UX_BYTE_INDEX_5)) << (UX_BIT_INDEX_16)) |
                      ((uint64_t)(*(p + UX_BYTE_INDEX_6)) << (UX_BYTE_INDEX_8))  |
                      (*(p + UX_BYTE_INDEX_7)));
}
 
void ux_utils_uint64_to_uint8(uint8_t *p, uint32_t size, uint64_t v)
{
    if (p == NULL || size != UX_UINT64_BYTE_NUM) {
        return;
    }
    *(p + UX_BYTE_INDEX_0) = (uint8_t)((v) >> UX_BIT_INDEX_56);
    *(p + UX_BYTE_INDEX_1) = (uint8_t)((v) >> UX_BIT_INDEX_48);
    *(p + UX_BYTE_INDEX_2) = (uint8_t)((v) >> UX_BIT_INDEX_40);
    *(p + UX_BYTE_INDEX_3) = (uint8_t)((v) >> UX_BIT_INDEX_32);
    *(p + UX_BYTE_INDEX_4) = (uint8_t)((v) >> UX_BIT_INDEX_24);
    *(p + UX_BYTE_INDEX_5) = (uint8_t)((v) >> UX_BIT_INDEX_16);
    *(p + UX_BYTE_INDEX_6) = (uint8_t)((v) >> UX_BIT_INDEX_8);
    *(p + UX_BYTE_INDEX_7) = (uint8_t)(v);
}

uint32_t ux_utils_uint8_to_uint32(uint8_t *p, uint32_t size)
{
    if (p == NULL || size != UX_UINT32_BYTE_NUM) {
        return 0;
    }
    return (uint32_t)(((uint32_t)(*(p + UX_BYTE_INDEX_0)) << (UX_BIT_INDEX_24)) |
                      ((uint32_t)(*(p + UX_BYTE_INDEX_1)) << (UX_BIT_INDEX_16)) |
                      ((uint32_t)(*(p + UX_BYTE_INDEX_2)) << (UX_BIT_INDEX_8)) | (*(p + UX_BYTE_INDEX_3)));
}

void ux_utils_uint32_to_uint8(uint8_t *p, uint32_t size, uint32_t v)
{
    if (p == NULL || size != UX_UINT32_BYTE_NUM) {
        return;
    }
    *(p + UX_BYTE_INDEX_0) = (uint8_t)((v) >> UX_BIT_INDEX_24);
    *(p + UX_BYTE_INDEX_1) = (uint8_t)((v) >> UX_BIT_INDEX_16);
    *(p + UX_BYTE_INDEX_2) = (uint8_t)((v) >> UX_BIT_INDEX_8);
    *(p + UX_BYTE_INDEX_3) = (uint8_t)(v);
}

uint16_t ux_utils_uint8_to_uint16(uint8_t *p, uint32_t size)
{
    if (p == NULL || size != UX_UINT16_BYTE_NUM) {
        return 0;
    }
    return (uint16_t)(((uint16_t)(*(p + UX_BYTE_INDEX_0)) << (UX_BIT_INDEX_8)) | (*(p + UX_BYTE_INDEX_1)));
}

void ux_utils_uint16_to_uint8(uint8_t *p, uint32_t size, uint16_t v)
{
    if (p == NULL || size != UX_UINT16_BYTE_NUM) {
        return;
    }
    *(p + UX_BYTE_INDEX_0) = (uint8_t)((v >> UX_BIT_INDEX_8) & UX_BYTE_MAX_VALUE);
    *(p + UX_BYTE_INDEX_1) = (uint8_t)(v & UX_BYTE_MAX_VALUE);
}

int32_t ux_utils_hex_char_to_dec(char c)
{
    if (c >= '0' && c <= '9') {
        return (c - '0');
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a' + 10);  // 10:tenth
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A' + 10);  // 10:tenth
    } else {
        return -1;
    }
}
