/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2025. All rights reserved. \n
 * Description: ux power customer header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-04， Create file. \n
 */

#ifndef UX_POWER_CUSTOMER_H
#define UX_POWER_CUSTOMER_H

#include "stdint.h"
#include "ux_service_define.h"
#include "ux_status_define.h"
#include "ux_api_fwk.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SERVICE_CNT_PER_PRIORITY_MAX 4
typedef struct {
    ux_api_prio_t power_on_prio;
    ux_api_prio_t power_off_prio;
} ux_power_service_prio_cfg_t;

typedef struct {
    /**
    * @brief  获取关机电量阈值
    * @return 电量阈值
    */
    uint8_t (*get_power_off_soc)(void);

    /**
    * @brief  获取告警电量阈值
    * @return 电量阈值
    */
    uint8_t (*get_power_warning_soc)(void);

    /**
    * @brief  获取电量监控心跳间隔
    * @return 心跳间隔
    */
    uint32_t (*get_power_monitor_time)(void);

    /**
    * @brief  获取延迟关机时间
    * @return 关机时间
    */
    uint32_t (*get_power_off_delay_time)(void);

    /**
    * @brief  获取与电源相关的服务
    * @param [out] services 用于存储服务的数组指针
    * @param [in] arr_len 服务数组长度
    * @param [out] service_num 实际获取的服务数量
    * @param [in] prio 服务优先级
    * @param [in] state 服务内容
    */
    errcode_t (*get_power_services)(ux_service_t *services, uint8_t arr_len,
        uint8_t* service_num, ux_api_prio_t prio, working_state_t state);
} ux_power_customer_funcs;

ux_power_customer_funcs *ux_power_get_instance(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif