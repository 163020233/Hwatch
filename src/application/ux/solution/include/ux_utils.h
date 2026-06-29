/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: hearing media api
 * Author: AuthorNameMagicTag
 * Create: 2023-02-28
 */

#ifndef UX_UTILS_H
#define UX_UTILS_H

#include "stdio.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int32_t ux_utils_hex_char_to_dec(char c);

uint32_t ux_utils_uint8_to_uint32(uint8_t *p, uint32_t size);

void ux_utils_uint32_to_uint8(uint8_t *p, uint32_t size, uint32_t v);

uint16_t ux_utils_uint8_to_uint16(uint8_t *p, uint32_t size);

void ux_utils_uint16_to_uint8(uint8_t *p, uint32_t size, uint16_t v);

uint64_t ux_utils_uint8_to_uint64(uint8_t *p, uint32_t size);
 
void ux_utils_uint64_to_uint8(uint8_t *p, uint32_t size, uint64_t v);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
