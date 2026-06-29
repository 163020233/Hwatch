/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux charger solution \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-11-11， Create file. \n
 */
#ifndef UX_CHARGER_H
#define UX_CHARGER_H

#include "errcode.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_charger_init(void);
errcode_t ux_charger_mock_battery_soc(uint8_t val);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UX_CHARGER_H */
