/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2024-2024. All rights reserved.
 * Description: OS Abstract Layer.
 */

#ifndef __SOC_OSAL_H__
#define __SOC_OSAL_H__

#include "osal_atomic.h"
#include "osal_timer.h"

#define OSAL_SUCCESS 0
#define OSAL_FAILURE (-1)

struct osal_list_head {
    struct osal_list_head *next, *prev;
};
#endif /* __SOC_OSAL_H__ */
