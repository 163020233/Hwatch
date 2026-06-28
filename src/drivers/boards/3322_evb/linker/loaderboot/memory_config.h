/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  Default memory configurations
 * Author: @CompanyNameTag
 * Create:  2020-01
 */
#ifndef MEMORY_CONFIG_H
#define MEMORY_CONFIG_H

#include "memory_config_common.h"

#define LOADERBOOT_PROGRAM_ORIGIN       (APP_ITCM_ORIGIN + FLASH_SIGN_HEADER + ROOT_PUBLIC_KEY_LEN)
#define LOADERBOOT_PROGRAM_LENGTH       (APP_ITCM_LENGTH - FLASH_SIGN_HEADER - ROOT_PUBLIC_KEY_LEN)

#define LOADERBOOT_DATA_ORIGIN          APP_DTCM_ORIGIN
#define LOADERBOOT_DATA_LENGTH          APP_DTCM_LENGTH

// provision use start
/* romboot stack data bss user 64KB */
#define ROM_USE_DTCM_ORIGIN             APP_DTCM_ORIGIN
#define ROM_USE_DTCM_LENGTH             0x10000

/* romboot heap data bss user 128KB */
#define ROM_HEAP_USE_DTCM_ORIGIN        (APP_DTCM_ORIGIN + ROM_USE_DTCM_LENGTH)
#define ROM_HEAP_USE_DTCM_LENGTH        0x40000

#define IRMALLOC_HEAP_USE_DTCM_ORIGIN   (ROM_HEAP_USE_DTCM_ORIGIN + ROM_HEAP_USE_DTCM_LENGTH)
#define IRMALLOC_HEAP_USE_DTCM_LENGTH   0x40000
// provision end

/* stack for normal 5k */
#define LOADER_USER_STACK_LEN           0x1400
/* stack for irq 4k */
#define LOADER_IRQ_STACK_LEN            0x1000
/* stack for exception 1k */
#define LOADER_EXCP_STACK_LEN           0x400
/* stack for nmi  1k */
#define LOADER_NMI_STACK_LEN            0x400
#endif
