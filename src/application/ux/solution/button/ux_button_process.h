/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux button process \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef UX_BUTTON_PROCESS_H
#define UX_BUTTON_PROCESS_H

#include "errcode.h"
#include "ux_api_button.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_BUTTON_PRESS_BEHAVIOR "press"
#define UX_BUTTON_LONG_PRESS_BEHAVIOR "long_press"

errcode_t ux_button_init(void);

errcode_t ux_button_deinit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
