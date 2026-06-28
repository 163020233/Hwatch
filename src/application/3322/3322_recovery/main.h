/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:   Application Core Main function for Standard
 * Author:
 * Create: 2025-05-13
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
typedef struct {
    uint32_t s_regs[12];    // store s0~s11
    uint32_t t_regs[7];     // store t0~t6
} ree_nmi_stack_t;

typedef enum {
    BOOTROM_IDX       = 0,
    L2RAM_DSP_IDX     = 1,
    L2RAM_GPU_IDX     = 2,
    PSRAM_IDX         = 3,
    NOR_FLASH_IDX     = 4,

    MAX_REGION_NUM
} region_index_t;

#define L2MEM_DSP_ADDR_START 0x60100000
#define L2MEM_DSP_ADDR_LEN 0x100000

#define L2MEM_GPU_ADDR_START 0x60200000
#define L2MEM_GPU_ADDR_LEN 0x200000

#define PSRAM_ADDR_START 0x6c000000
#define PSRAM_ADDR_LEN 0x2000000

#define NOR_FLASH_ADDR_START 0x70000000
#define NOR_FLASH_ADDR_LEN 0x800000

#endif