/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provides ux bs customer api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-22， Create file. \n
 */

#ifndef UX_BS_CUSTOMER_H
#define UX_BS_CUSTOMER_H

#include "stdint.h"
#include "ux_cust_bs_ble.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    UX_ANNOUNCE_FILTER_RESULT_TRUE,
    UX_ANNOUNCE_FILTER_RESULT_FALSE,

    UX_ANNOUNCE_FILTER_RESULT_MAX
} ux_announce_filter_result_t;

typedef struct {
    /**
    * @brief  自定义广播参数过滤条件(只作用于空中配对方式，地址白名单配对方式不可用)
    * @param[in] announce_data: 广播数据
    * @param[in] announce_data_len: 广播数据长度
    * @param[in, out] result: 广播过滤结果
    * @retval ERRCODE_SUCC 成功
    * @retval Other 失败，参考 @ref errcode_t
    */
    errcode_t (*bs_announce_filter)(const uint8_t *announce_data, uint8_t announce_data_len,
                                    ux_announce_filter_result_t *result);

    /**
    * @brief 获取BLE广播字段中的UUID配置
    * @param[in, out] uuid_cfg: BLE广播字段中的UUID配置
    * @retval ERRCODE_SUCC 成功
    * @retval Other 失败，参考 @ref errcode_t
    */
    errcode_t (*bs_get_adv_uuid_cfg)(ux_cust_bs_ble_uuid_t *uuid_cfg);
} ux_bs_customer_funcs;

/**
* @brief  获取bs_customer模块实例对象
* @return ux_bs_customer_funcs* bs_customer模块函数指针结构体，@ref ux_bs_customer.h
*/
ux_bs_customer_funcs *ux_bs_customer_get_instance(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
