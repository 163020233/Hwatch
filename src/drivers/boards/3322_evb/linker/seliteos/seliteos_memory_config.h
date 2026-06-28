/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: SeliteOS memory config for SELiteOS.
 */
#ifndef SELITEOS_MEMORY_CONFIG_H
#define SELITEOS_MEMORY_CONFIG_H

#include "memory_config_common.h"
#include "memory_config_flash.h"
#include "ta_memory_config.h"

#ifdef SELITEOS_KERNEL_TEST
/* flash_ext tee layout */
#define FLASH_COMMON_TEXT_START            SELITEOS_PROGRAM_EXT
#define FLASH_COMMON_TEXT_SIZE             0x8000 // 32KB

#define FLASH_COMMON_RODATA_START          (FLASH_COMMON_TEXT_START + FLASH_COMMON_TEXT_SIZE)
#define FLASH_COMMON_RODATA_SIZE           0x8000 // 32KB
#else
#define FLASH_COMMON_TEXT_START            SELITEOS_PROGRAM_EXT
#define FLASH_COMMON_TEXT_SIZE             0x0000

#define FLASH_COMMON_RODATA_START          (FLASH_COMMON_TEXT_START + FLASH_COMMON_TEXT_SIZE)
#define FLASH_COMMON_RODATA_SIZE           0x0000
#endif

#define FLASH_EXT_START                    (BOXB_TEXT_RODATA_START + BOXB_TEXT_RODATA_SIZE)
#define FLASH_EXT_SIZE                     (SELITEOS_PROGRAM_EXT + SELITEOS_PROGRAM_EXT_LENGTH - FLASH_EXT_START)

/* ITCM memory layout */
#define KERNEL_TEXT_RODATA_START           TEE_ITCM_ORIGIN
#define KERNEL_TEXT_RODATA_SIZE            0x8000 // 32KB

#define DRVBOX_TEXT_START                  (KERNEL_TEXT_RODATA_START + KERNEL_TEXT_RODATA_SIZE)
#define DRVBOX_TEXT_SIZE                   0x8000 // 32KB
#define DRVBOX_TEXT_ALIGNED_SIZE           0x8000 // 32KB

#define USR_TEXT_START                     (DRVBOX_TEXT_START + DRVBOX_TEXT_SIZE)
#define USR_TEXT_SIZE                      0x4000 // 16KB

#define DRVBOX_RODATA_START                (USR_TEXT_START + USR_TEXT_SIZE)
#define DRVBOX_RODATA_SIZE                 0x2000 // 8KB
#define DRVBOX_RODATA_ALIGNED_SIZE         0x2000 // 8KB

#define USR_RODATA_START                   (DRVBOX_RODATA_START + DRVBOX_RODATA_SIZE)
#define USR_RODATA_SIZE                    0x2000 // 8KB

/* DTCM memory layout */
#define KERNEL_DATA_START                  (USRBOX_DATA_START + USRBOX_DATA_SIZE)
#define KERNEL_HEAP_SIZE                   0x1000 // 4KB
#define KERNEL_STACK_SIZE                  0x1000 // 4KB
#define KERNEL_DATA_SIZE                   0x3800 // 14KB
#define KERNEL_DATA_ALIGNED_SIZE           0x4000 // 16KB

#define USR_DATA_START                     (KERNEL_DATA_START + KERNEL_DATA_SIZE)
#define USR_DATA_SIZE                      0x7C0 // 2KB - 64B
#define USR_DATA_ALIGNED_SIZE              0x800 // 2KB

#define USR_RO_DATA_START                  (USR_DATA_START + USR_DATA_SIZE)
#define USR_RO_DATA_SIZE                   0x40 // 64B

#define DRVBOX_DATA_START                  (USR_RO_DATA_START + USR_RO_DATA_SIZE)
#define DRVBOX_HEAP_SIZE                   0x1000 // 4KB
#define DRVBOX_DATA_SIZE                   0x2000 // 8KB

#define SHARED_DATA_START                  (DRVBOX_DATA_START + DRVBOX_DATA_SIZE)
#define SHARED_DATA_SIZE                   0x2000 // 8KB
#define SHARED_DATA_REE_ALLOC_SIZE         0x1000 // 4KB
#define SHARED_DATA_TEE_ALLOC_SIZE         0x1000 // 4KB
#define SHARED_DATA_REE_START              SHARED_DATA_START
#define SHARED_DATA_TEE_START              (SHARED_DATA_START + SHARED_DATA_REE_ALLOC_SIZE)
#endif

