/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: ta_exit source file.
 */

#include "los_task_pri.h"
#include "ta_exit.h"

void ta_exit(unsigned long a0, unsigned long a1)
{
    TaskContext context = {0};
    context.a0 = a0;
    context.a1 = a1;
    LOS_TaskUsrExit((void *)&context);
}
