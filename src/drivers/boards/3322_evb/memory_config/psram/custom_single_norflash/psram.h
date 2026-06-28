/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description:  psram memory configurations
 * Author: @CompanyNameTag
 * Create:  2022-03-09
 */

#ifndef PSRAM_H
#define PSRAM_H

/**
 *  index       region name        start addr      length      size    memory cache attr
 *  0           dsp code           0x22000000      0xA0000     640K    cacheable
 *  1           synload code       0x220A0000      0x10000     64K     cacheable
 *  2           ace data           0x220B0000      0x30000     192K    cacheable
 *  3           app heap(bss&data) 0x220E0000      0x720000    7296K   cacheable
 *  4           npu heap           0x22800000      0x200000    2M      non-cacheable
 *  5           xip code           0x22A00000      0x600000    6M      writeback cacheable
 *  6           gui pool           0x23000000      0x000000    0M      non-cacheable
 *  7           vau cmd list       0x23000000      0x000000    0M      non-cacheable
 */

#define PSRAM_BASE_ADDR                     0x22000000
#define PSRAM_LENTH                         0x1000000

#define PSRAM_DSP_CODE_ADDR                 (PSRAM_BASE_ADDR)
#define PSRAM_DSP_CODE_LENGTH               0xA0000     // 640k

#define PSRAM_APP_SYN_LOAD_ADDR             (PSRAM_DSP_CODE_ADDR + PSRAM_DSP_CODE_LENGTH)
#define PSRAM_APP_SYN_LOAD_LENGTH           0x10000     // 64k

/* Address used by the ACE of the application subsystem */
#define PSRAM_HM_APP_SYS_ADDR               (PSRAM_APP_SYN_LOAD_ADDR + PSRAM_APP_SYN_LOAD_LENGTH)
#define PSRAM_HM_APP_SYS_LENGTH             0x30000     // 192k

/* FS start address */
#define PSRAM_FS_ADDR                       (PSRAM_HM_APP_SYS_ADDR + PSRAM_HM_APP_SYS_LENGTH)
#ifdef CONFIG_SUPPORT_FS_INDEPENDENT_MEMORY_POOL
#define PSRAM_FS_LENGTH                     0x13000    // 78k
#else
#define PSRAM_FS_LENGTH                     0x0
#endif

/* Heap start address */
#define PSRAM_HEAP_ADDR                     (PSRAM_FS_ADDR + PSRAM_FS_LENGTH)
#define PSRAM_HEAP_LENGTH                   (0x720000 -  PSRAM_FS_LENGTH)   // 7296K - PSRAM_FS_LENGTH

/* npu heap address */
#define PSRAM_NPU_APP_HEAP_START            (PSRAM_HEAP_ADDR + PSRAM_HEAP_LENGTH)
#define PSRAM_NPU_APP_HEAP_LENGTH           0x200000    // 2M

#define PSRAM_XIP_CODE_ADDR                 (PSRAM_NPU_APP_HEAP_START + PSRAM_NPU_APP_HEAP_LENGTH)
#define PSRAM_XIP_CODE_SIZE                 0x600000    // 6MB
#define REMOTE_FUNC_ADDR                    PSRAM_XIP_CODE_ADDR

/* GUI memory pool address NON Cache */
#define PSRAM_GUI_MEM_POOL_ADDR             (PSRAM_XIP_CODE_ADDR + PSRAM_XIP_CODE_SIZE)
#define PSRAM_GUI_MEM_POOL_SIZE             0x0    // 0MB

/* VAU Linked List Address NON Cache */
#define PSRAM_DISP_VAU_NODE_ADDR            (PSRAM_GUI_MEM_POOL_ADDR + PSRAM_GUI_MEM_POOL_SIZE)
#define PSRAM_DISP_VAU_NODE_LENGTH          0x0    // 0MB

/* reserve */
#define PSRAM_RESERVE_ADDR                  (PSRAM_DISP_VAU_NODE_ADDR + PSRAM_DISP_VAU_NODE_LENGTH)
#define PSRAM_RESERVE_ADDR_LENGTH           0

#endif // PSRAM_H
