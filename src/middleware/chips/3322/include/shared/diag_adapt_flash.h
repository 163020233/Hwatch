/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: diag adapt flash
 * This file should be changed only infrequently and with great care.
 */

#ifndef DIAG_ADAPT_FLASH_H
#define DIAG_ADAPT_FLASH_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

uint32_t diag_adapt_flash_get_region_size(uint8_t opt_type);
int32_t diag_adapt_flash_read(uint8_t opt_type, uint32_t offset, uint8_t *buf, uint32_t size);
int32_t diag_adapt_flash_write(uint8_t opt_type, uint32_t offset, const uint8_t *buf, uint32_t size,
    bool do_erase);
errcode_t diag_adapt_flash_erase(uint8_t opt_type, const uint32_t offset, const uint32_t size);

#endif /* DIAG_ADAPT_FLASH_H */