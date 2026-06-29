/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 3322 memory map header for SELiteOS.
 */

#ifndef MEMMAP_CONFIG_H
#define MEMMAP_CONFIG_H

#include "seliteos_memory_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern char __kernel_data_start;
extern char __kernel_data_load_start;
extern char __kernel_data_load_size;
extern char __kernel_bss_start;
extern char __kernel_bss_size;
extern char __kernel_heap_start;
extern char __kernel_heap_size;

extern char __boxA_text_rodata_start;
extern char __boxA_text_rodata_size;
extern char __boxA_data_start;
extern char __boxA_data_load_start;
extern char __boxA_data_load_size;
extern char __boxA_bss_start;
extern char __boxA_bss_size;
extern char __boxA_heap_start;
extern char __boxA_heap_size;
extern char __boxA_stack_start;
extern char __boxA_stack_size;

extern char __boxB_text_rodata_start;
extern char __boxB_text_rodata_size;
extern char __boxB_data_start;
extern char __boxB_data_load_start;
extern char __boxB_data_load_size;
extern char __boxB_bss_start;
extern char __boxB_bss_size;
extern char __boxB_heap_start;
extern char __boxB_heap_size;
extern char __boxB_stack_start;
extern char __boxB_stack_size;

extern char __drvbox_heap_start;
extern char __drvbox_heap_size;
extern char __drvbox_data_start;
extern char __drvbox_data_load_start;
extern char __drvbox_data_load_size;
extern char __drvbox_bss_start;
extern char __drvbox_bss_size;

extern char __usr_data_start;
extern char __usr_data_load_start;
extern char __usr_data_load_size;
extern char __usr_bss_start;
extern char __usr_bss_size;

extern char __usr_readonly_data_start;
extern char __usr_readonly_data_load_start;
extern char __usr_readonly_data_load_size;

extern char __shared_data_tee_alloc_start;
extern char __shared_data_tee_alloc_size;

#define TEE_ITCM_START_ADDR       TEE_ITCM_ORIGIN
#define TEE_ITCM_END_ADDR         (TEE_ITCM_ORIGIN + TEE_ITCM_LENGTH - 1)
#define TEE_DTCM_START_ADDR       TEE_DTCM_ORIGIN
#define TEE_DTCM_END_ADDR         (TEE_DTCM_ORIGIN + TEE_DTCM_LENGTH - 1)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MEMMAP_CONFIG_H */

