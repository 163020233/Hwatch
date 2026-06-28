/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#ifndef __DRV_VDEC_OSAL_H__
#define __DRV_VDEC_OSAL_H__

#include "td_type.h"
#include "td_base.h"
#include "soc_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define uint_2_ptr_(x) (td_void*)((uintptr_t)(x))
#define UINT_2_PTR uint_2_ptr_
#define vdec_unused(x) ((x) = (x))

#define proc_print_(fmt...) osal_seq_printf(fmt)
#define PROC_PRINT proc_print_
#define vdec_copy_from_user(to, from, len) osal_copy_from_user(to, from, len)
#define vdec_check_assess_ok(type, addr, size) osal_access_ok(type, addr, size)

typedef struct {
    osal_wait wait;
    td_s32 flag;
} kern_event;

typedef struct seq_file proc_file;
typedef kern_event vdec_osal_event;

typedef osal_semaphore vdec_osal_sema;
typedef osal_atomic vdec_osal_atomic;

td_s32 vdec_osal_atomic_init(vdec_osal_atomic *var);
td_void vdec_osal_atomic_exit(vdec_osal_atomic *var);
td_s32 vdec_osal_atomic_inc_return(vdec_osal_atomic *var);
td_s32 vdec_osal_atomic_dec_return(vdec_osal_atomic *var);
td_void vdec_osal_atomic_inc(vdec_osal_atomic *var);
td_void vdec_osal_atomic_dec(vdec_osal_atomic *var);
td_s32 vdec_osal_atomic_read(vdec_osal_atomic *var);
td_void vdec_osal_atomic_set(vdec_osal_atomic *var, td_s32 value);

td_void vdec_osal_sema_init(vdec_osal_sema *s);
td_void vdec_osal_sema_down(vdec_osal_sema *s);
td_void vdec_osal_sema_up(vdec_osal_sema *s);
td_void vdec_osal_sema_exit(vdec_osal_sema *s);


td_s32 vdec_osal_event_init(vdec_osal_event *s);
td_s32 vdec_osal_event_wait(vdec_osal_event *s, td_u32 ms_wait_time);
td_s32 vdec_osal_event_wait_interruptible(vdec_osal_event *s, td_u32 ms_wait_time);
td_s32 vdec_osal_event_init_cond(vdec_osal_event *s);

td_void vdec_osal_event_give(vdec_osal_event *s);
td_void vdec_osal_event_exit(vdec_osal_event *s);

td_handle vdec_osal_get_mem_fd(td_void *buf);
td_u32 vdec_osal_get_time(td_void);
td_void vdec_osal_msleep(td_u32 time);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
