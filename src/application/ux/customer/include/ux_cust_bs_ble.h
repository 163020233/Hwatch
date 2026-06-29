/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux bs customer api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-22， Create file. \n
 */

#ifndef UX_CUST_BS_BLE_H
#define UX_CUST_BS_BLE_H

#include "stdint.h"
#include "stdbool.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    uint8_t uuid_len;
    const uint8_t* uuid;
    bool enable_uuid;
} ux_cust_bs_ble_uuid_t;

/**
* @brief 获取BLE广播字段中的UUID配置
* @param[in, out] uuid_cfg: BLE广播字段中的UUID配置
* @retval ERRCODE_SUCC 成功
* @retval Other 失败，参考 @ref errcode_t
*/
errcode_t ux_cust_bs_ble_get_uuid_cfg(ux_cust_bs_ble_uuid_t *uuid_cfg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif