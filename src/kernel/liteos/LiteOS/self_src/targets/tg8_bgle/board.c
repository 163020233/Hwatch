/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: os platform config files
 */

#include "los_task_pri.h"
#include "los_init_pri.h"
#include "los_init.h"
#include "los_tick_pri.h"

VOID BoardConfig(VOID)
{
    OsSetMainTask();
    OsCurrTaskSet(OsGetMainTask());
}
