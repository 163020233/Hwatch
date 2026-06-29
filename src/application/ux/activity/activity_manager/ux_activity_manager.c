/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: ux task init src code.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-03-15, Create file. \n
 */

#include "errcode.h"
#include "ux_module_init.h"

errcode_t ux_activity_init(void)
{
    /* 各activity初始化添加在此处 */
    UX_MODULE_INIT();
    return ERRCODE_SUCC;
}
