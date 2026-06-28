/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  Application Core OS Initialize Interface Header for Standard
 */

#ifndef KERNEL_INIT_H
#define KERNEL_INIT_H

extern void *g_intheap_begin;
extern void *g_intheap_size;
extern void *g_extend_heap_begin;
extern void *g_extend_heap_size;

#ifdef CONFIG_PSRAM_SUPPORT
extern void *__psram_data_start;
extern void *__psram_data_end;
extern void *__psram_data_load;
extern void *__psram_bss_start;
extern void *__psram_bss_end;
#endif

typedef void (*FSMEMPOOLHOOK)(unsigned int *pool, unsigned int *size);
extern void osal_fs_mem_pool_hook_reg(FSMEMPOOLHOOK hook);

typedef enum os_class {
    OS_CLASS_LITEOS,
    OS_CLASS_FREERTOS,
    OS_CLASS_UNDEFINED,
} os_class;

extern os_class g_os_init_select;

#ifndef __FREERTOS__
void OsCacheInit(void);
#endif

/**
 * @defgroup MAIN_ENTRY CHIP Main Entry
 * @{
 */
/**
 * @brief  Service init.
 */
void kernel_init(void);

/**
 * @}
 */

#endif
