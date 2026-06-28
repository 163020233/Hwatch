/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __FREERTOS_OSAL__
#define __FREERTOS_OSAL__

#include "vfmw.h"
#include "vfmw_osal.h"
#include "soc_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    osal_wait wait;
    td_s32 flag;
} kern_event;

typedef struct {
    osal_spinlock irq_lock;
    int is_init;
} kern_irq_lock;

typedef  kern_event osal_event_vfmw;
typedef  kern_irq_lock  osal_irq_spin_lock;
typedef  osal_semaphore  osal_sema;

td_void osal_intf_init(vfmw_osal_ops *ops);
td_void osal_intf_exit(vfmw_osal_ops *ops);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
