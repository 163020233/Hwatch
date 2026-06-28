/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: mutex
 * Author: FBB OS Team
 * Create: 2021-12-16
 */

#include <los_mux.h>
#include <los_mux_pri.h>
#include "soc_osal.h"
#include "osal_errno.h"
#include "osal_inner.h"

#ifdef LOSCFG_BOX
#define MUTEX_MAGIC_WORD 0x80000000
#else
#define MUTEX_MAGIC_WORD 0x800
#endif
// 非法 mux id, 赋一个异常的ID给上层，防止把0号锁误释放
#define LOS_MUTEX_MAX_INDEX           (KERNEL_MUX_LIMIT + 1)

int osal_mutex_init(osal_mutex *mutex)
{
#ifdef LOSCFG_MUTEX_DEBUG
    osal_log("mutex_init:0x%x\r\n", (unsigned int)__builtin_return_address(0));
#endif

    if (mutex == NULL) {
        osal_log("param invalid!\n");
        return OSAL_FAILURE;
    }
    if (((unsigned int)(UINTPTR)mutex->mutex & MUTEX_MAGIC_WORD) == MUTEX_MAGIC_WORD) {
        osal_log("mutex inited. id:%d.\n",
            ((unsigned int)(UINTPTR)mutex->mutex ^ MUTEX_MAGIC_WORD));
        return OSAL_FAILURE;
    }
    if (GET_MUX_INDEX((unsigned int)(UINTPTR)mutex->mutex) >= (unsigned int)KERNEL_MUX_LIMIT) {
        osal_log("mutex id invalid!\n");
        return OSAL_FAILURE;
    }
    unsigned int ret = LOS_MuxCreate((unsigned int *)&(mutex->mutex));
    if (ret == LOS_OK) {
        mutex->mutex = (void *)(UINTPTR)((unsigned int)(UINTPTR)mutex->mutex | MUTEX_MAGIC_WORD);
    } else {
        mutex->mutex = (void *)(UINTPTR)((unsigned int)(UINTPTR)LOS_MUTEX_MAX_INDEX | MUTEX_MAGIC_WORD);
        osal_log("MuxCreate fail! ret=%#x.\n", ret);
    }
    return (int)ret;
}

void osal_mutex_destroy(osal_mutex *mutex)
{
#ifdef LOSCFG_MUTEX_DEBUG
    osal_log("mutex_destroy:0x%x\r\n", (unsigned int)__builtin_return_address(0));
#endif
    if (mutex == NULL) {
        osal_log("param invalid!\n");
        return;
    }
    if (mutex->mutex == NULL) {
        osal_log("mutex destroyed or not inited!\n");
        return;
    }
    unsigned int mutex_tmp = (unsigned int)(UINTPTR)mutex->mutex ^ MUTEX_MAGIC_WORD;
    unsigned int ret = LOS_MuxDelete(mutex_tmp);
    if (ret != LOS_OK) {
        osal_log("MuxDelete fail! ret=%#x.\n", ret);
        return;
    }
    mutex->mutex = NULL;
}

int osal_mutex_lock(osal_mutex *mutex)
{
    if (mutex == NULL) {
        osal_log("param invalid!\n");
        return OSAL_FAILURE;
    }
#ifdef OSAL_MUTEX_DEBUG_LOG
    unsigned int *caller = __builtin_return_address(0);
#endif
    unsigned int mutex_tmp = (unsigned int)(UINTPTR)mutex->mutex ^ MUTEX_MAGIC_WORD;
    unsigned int ret = LOS_MuxPend(mutex_tmp, LOS_WAIT_FOREVER);
    if (ret != LOS_OK) {
        osal_log("MuxPend fail! ret=%#x.\n", ret);
#ifdef OSAL_MUTEX_DEBUG_LOG
        osal_log("MuxPend lock: %p\n", caller);
#endif
    }
    return (int)ret;
}

int osal_mutex_lock_timeout(osal_mutex *mutex, unsigned int timeout)
{
    if (mutex == NULL) {
        osal_log("param invalid!\n");
        return OSAL_FAILURE;
    }
    unsigned int mutex_tmp = (unsigned int)(UINTPTR)mutex->mutex ^ MUTEX_MAGIC_WORD;
    unsigned int tick =
        (timeout == (unsigned int)OSAL_MUTEX_WAIT_FOREVER) ? LOS_WAIT_FOREVER : osal_msecs_to_jiffies(timeout);

    unsigned int ret = LOS_MuxPend(mutex_tmp, tick);
    if (ret != LOS_OK) {
        osal_log("MuxPend fail! ret=%#x.\n", ret);
    }
    return (int)ret;
}

int osal_mutex_lock_interruptible(osal_mutex *mutex)
{
    if (mutex == NULL) {
        osal_log("param invalid!\n");
        return OSAL_FAILURE;
    }
    unsigned int mutex_tmp = (unsigned int)(UINTPTR)mutex->mutex ^ MUTEX_MAGIC_WORD;
    unsigned int ret = LOS_MuxPend(mutex_tmp, LOS_WAIT_FOREVER);
    if (ret == LOS_ERRNO_MUX_PEND_INTERR) {
        osal_log("mutex locked in ISR!\n");
    } else if (ret != LOS_OK) {
        osal_log("MuxPend fail! ret=%#x.\n", ret);
    }

    return (int)ret;
}

int osal_mutex_trylock(osal_mutex *mutex)
{
    if (mutex == NULL) {
        osal_log("param invalid!\n");
        return FALSE;
    }
    unsigned int mutex_tmp = (unsigned int)(UINTPTR)mutex->mutex ^ MUTEX_MAGIC_WORD;
    return (int)LOS_MuxPend(mutex_tmp, LOS_NO_WAIT);
}

void osal_mutex_unlock(osal_mutex *mutex)
{
    if (mutex == NULL) {
        osal_log("param invalid!\n");
        return;
    }
#ifdef OSAL_MUTEX_DEBUG_LOG
    unsigned int *caller = __builtin_return_address(0);
#endif
    unsigned int mutex_tmp = (unsigned int)(UINTPTR)mutex->mutex ^ MUTEX_MAGIC_WORD;
    unsigned int ret = LOS_MuxPost(mutex_tmp);
    if (ret != LOS_OK) {
        osal_log("MuxPost fail! ret=%#x.\n", ret);
#ifdef OSAL_MUTEX_DEBUG_LOG
        osal_log("MuxPost unlock: %p\n", caller);
#endif
    }
}

int osal_mutex_is_locked(osal_mutex *mutex)
{
    osal_unused(mutex);
    return -1;
}
