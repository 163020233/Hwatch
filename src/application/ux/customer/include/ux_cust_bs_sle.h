/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux bs customer api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-22， Create file. \n
 */

#ifndef UX_CUST_BS_SLE_H
#define UX_CUST_BS_SLE_H

#include "stdint.h"
#include "errcode.h"
#include "ux_bs_customer.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
* @brief  自定义空中配对参数过滤条件
* @param[in] announce_data: 空中配对广播数据
* @param[in] announce_data_len: 空中配对广播数据长度
* @param[in, out] result: 广播过滤结果
* @retval ERRCODE_SUCC 成功
* @retval Other 失败，参考 @ref errcode_t
*/
errcode_t ux_cust_bs_sle_air_pairing_filter(const uint8_t *announce_data, uint8_t announce_data_len,
                                            ux_announce_filter_result_t *result);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
