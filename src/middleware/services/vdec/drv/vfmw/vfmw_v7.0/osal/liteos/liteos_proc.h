/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __LINUX_PROC__
#define __LINUX_PROC__
#include "soc_osal.h"
#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define NAME_SIZE 16

#define OS_VA_LIST  va_list
#define OS_VA_START va_start
#define OS_VA_END   va_end

#define OS_PROC_FILE_S    td_void
#define OS_PROC_PRINT     osal_seq_printf

#define PROC_FILE  OS_PROC_FILE_S
#define PROC_READ_BUF_SIZE (100 * 1024)

typedef td_s32 (*proc_read_fn)(struct osal_proc_dir_entry *entry);

#define OS_PROC_CMD osal_proc_cmd

td_s32 liteos_proc_create(td_char *proc_name, td_void *read, td_void *cmd_list, td_u32 cmd_cnt);
td_void liteos_proc_destroy(td_char *proc_name);
td_s32 liteos_proc_init(td_void);
td_void liteos_proc_exit(td_void);
td_void liteos_proc_dump(td_void *page, td_s32 page_count, td_s32 *used_bytes, td_s8 from_shr,
    const td_char *format, ...);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
