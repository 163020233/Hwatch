/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: ux task init header code.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-03-15, Create file. \n
 */
#ifndef UX_ACTIVITY_MANAGER_H
#define UX_ACTIVITY_MANAGER_H

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @note Activity初始化接口，包含所有activity初始化，需在开机流程中调用
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败，参考 @ref errcode_t
 */
errcode_t ux_activity_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
