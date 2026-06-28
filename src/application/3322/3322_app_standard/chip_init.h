/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  Application Core OS Initialize Interface Header for Standard
 */

#ifndef CHIP_INIT_H
#define CHIP_INIT_H

/**
 * @defgroup MAIN_ENTRY CHIP Main Entry
 * @{
 */

extern void *g_intheap_begin;
extern void *g_intheap_size;
extern void *g_extend_heap_begin;
extern void *g_extend_heap_size;

#define OS_SYS_MEM_ADDR    ((void *)&g_intheap_begin)
#define OS_SYS_MEM_SIZE    ((unsigned)&g_intheap_size)
#define OS_EXTEND_MEM_ADDR    ((void *)&g_extend_heap_begin)
#define OS_EXTEND_MEM_SIZE    ((unsigned)&g_extend_heap_size)

/**
 * @brief  Chip init config.
 */
void chip_init(void);

/**
 * @}
 */

#endif
