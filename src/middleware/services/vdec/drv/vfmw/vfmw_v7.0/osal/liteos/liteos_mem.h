/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2024. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */
#ifndef __FREERTOS_MEM__
#define __FREERTOS_MEM__

#include "td_base.h"
#include "vfmw.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/
td_s32 liteos_mem_init(td_void);
td_void liteos_mem_deinit(td_void);
td_s32 liteos_mem_proc(td_void *buffer);
td_s32 liteos_mem_alloc(const td_char *name, td_u32 size, td_u32 align, mem_record *mem_rec);
td_void liteos_mem_free(mem_record *mem);
#ifdef VFMW_CHECK_MEM_SUPPORT
td_u32 liteos_mem_check(td_void);
td_void liteos_mem_record(td_void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
