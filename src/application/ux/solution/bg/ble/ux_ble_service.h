/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_ble_service.h
 * Description:  provide ux ble service header file.
 */
#ifndef UX_BLE_SERVICE_H
#define UX_BLE_SERVICE_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "bts_def.h"
#include "bs_ble_gatt_server.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
enum {
    MOBILE_CHARA,
    MOBILE_DESC,
    MOBILE_MAX,
};
enum {
    BLE_MOBILE_RW_CHARACTER_IDX,
    BLE_MOBILE_RW_DESCRIPTOR_IDX,
    BLE_MOBILE_CHARACTER_COUNT,
};
enum {
    BS_GATTS_CHARAC_TYPE,
    BS_GATTS_DESC_TYPE,
};
typedef struct {
    bt_uuid_t uuid;                                             /*!< @if Eng uuid of this service.
                                                                     @else   该服务uuid。 @endif */
    uint8_t is_primary;                                         /*!< @if Eng is primary service tag
                                                                     @else   是否是主服务标记 */
    uint8_t attribute_count;                                    /*!< @if Eng size of attribute_array.
                                                                     @else   attribute_array 中存储的属性个数 */
    bs_gatt_service_callbacks_t cbks;                           /*!< @if Eng server callback function.
                                                                     @else   服务端回调函数 @endif */
    bs_gatt_service_extend_param_t extend_param;                /*!< @if Eng server structure extend param.
                                                                     @else   服务端扩展参数 @endif */
    bs_gatts_attribute_info attribute_array[MOBILE_MAX];         /*!< @if Eng point array of attribute config.
                                                                     @else   特征/描述符配置数组。 @endif */
} ux_bs_gatt_service_t;
uint8_t ux_bs_get_ble_service_id(void);
errcode_t ux_bs_ble_service_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif