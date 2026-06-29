/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2022. All rights reserved.
* Description:osal_mutex
* Author: AuthorNameMagicTag
* Create: 2021-08-10
*/
#include <los_base.h>
#include "soc_osal.h"
#include "osal_errno.h"

int osal_mutex_init(osal_mutex *mutex)
{
    return OSAL_SUCCESS;
}
void osal_mutex_destroy(osal_mutex *mutex)
{
    (void)mutex;
}
int osal_mutex_lock(osal_mutex *mutex)
{
    return OSAL_SUCCESS;
}
int osal_mutex_lock_timeout(osal_mutex *mutex, unsigned int timeout)
{
    return OSAL_SUCCESS;
}
int osal_mutex_lock_interruptible(osal_mutex *mutex)
{
    return 0;
}
int osal_mutex_trylock(osal_mutex *mutex)
{
    return 0;
}
void osal_mutex_unlock(osal_mutex *mutex)
{
    (void)mutex;
}
int osal_mutex_is_locked(osal_mutex *mutex)
{
    return 0;
}
