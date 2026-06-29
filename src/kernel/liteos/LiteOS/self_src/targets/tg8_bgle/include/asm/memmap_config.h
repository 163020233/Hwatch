/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: os platform config files
 */

#ifndef _MEMMAP_CONFIG_H
#define _MEMMAP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void *g_intheap_begin;
extern void *g_intheap_size;

#define OS_SYS_MEM_ADDR         (&g_intheap_begin)
#define OS_SYS_MEM_SIZE         (unsigned int)(&g_intheap_size)

#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
#define IRQ_STACK_TOP           ((UINTPTR)&__irq_stack_top)
#define IRQ_STACK_SIZE          0xC00
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MEMMAP_CONFIG_H */
