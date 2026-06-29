/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
 * Description: simulator mouse driver definition
 */
#include <stddef.h>
#include "osal_task.h"

osal_task *osal_kthread_create(osal_kthread_handler handler, void *data, const char *name, unsigned int stack_size)
{
    return NULL;
}

int osal_kthread_set_priority(osal_task *task, unsigned int priority)
{
    return 0;
}