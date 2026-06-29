/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TA memory config for SELiteOS.
 */
#ifndef TA_MEMORY_CONFIG_H
#define TA_MEMORY_CONFIG_H

#define BOXA_TEXT_RODATA_START      (FLASH_COMMON_RODATA_START + FLASH_COMMON_RODATA_SIZE)
#define BOXA_TEXT_RODATA_SIZE       0x2000 // 8KB

#define USRBOX_DATA_START           TEE_DTCM_ORIGIN // 0x10000000
#define BOXA_DATA_START             USRBOX_DATA_START
#define BOXA_HEAP_START             BOXA_DATA_START
#define BOXA_HEAP_SIZE              0x1000 // 4KB
#define BOXA_STACK_START            (BOXA_HEAP_START + BOXA_HEAP_SIZE)
#define BOXA_STACK_SIZE             0x1000 // 4KB
#define BOXA_DATA_SIZE              0x4000 // 16KB

#ifdef SELITEOS_KERNEL_TEST
#define BOXB_TEXT_RODATA_START      (BOXA_TEXT_RODATA_START + BOXA_TEXT_RODATA_SIZE)
#define BOXB_TEXT_RODATA_SIZE       0x2000 // 8KB

#define BOXB_DATA_START             (BOXA_DATA_START + BOXA_DATA_SIZE)
#define BOXB_HEAP_START             BOXB_DATA_START
#define BOXB_HEAP_SIZE              0x1000 // 4KB
#define BOXB_STACK_START            (BOXB_HEAP_START + BOXB_HEAP_SIZE)
#define BOXB_STACK_SIZE             0x1000 // 4KB
#define BOXB_DATA_SIZE              0x4000 // 16KB
#define USRBOX_DATA_SIZE            (BOXA_DATA_SIZE + BOXB_DATA_SIZE)
#else
#define BOXB_TEXT_RODATA_START      (BOXA_TEXT_RODATA_START + BOXA_TEXT_RODATA_SIZE)
#define BOXB_TEXT_RODATA_SIZE       0x0000

#define BOXB_DATA_START             (BOXA_DATA_START + BOXA_DATA_SIZE)
#define BOXB_HEAP_START             BOXB_DATA_START
#define BOXB_HEAP_SIZE              0x0000
#define BOXB_STACK_START            (BOXB_HEAP_START + BOXB_HEAP_SIZE)
#define BOXB_STACK_SIZE             0x0000
#define BOXB_DATA_SIZE              0x0000
#define USRBOX_DATA_SIZE            (BOXA_DATA_SIZE + BOXB_DATA_SIZE)
#endif

#endif /* TA_MEMORY_CONFIG_H */

