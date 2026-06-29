/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Service Framework (SSF) about Device API File.
 */
#ifndef SSF_DEVICE_H
#define SSF_DEVICE_H
#include <stdint.h>
#include "errcode.h"
#include "sle_common.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Define of invalid sle_obj id.
 * @else
 * @brief 星闪非法sle_obj对象id。
 * @endif
 */
#define SLE_INVALID_OBJ_ID 0xFFFFFFFF
#define SLE_INVALID_DEVICE_ID 0xFFFF

/**
 * @if Eng
 * @brief Definition of sle_obj_t.
 * @else
 * @brief 星闪sle_obj_t的定义。
 * @endif
 */
typedef uint32_t sle_obj_t;

/**
 * @if Eng
 * @brief SLE Object Allocation Report.
 * @par Description: SLE Object Allocation Report.
 * @param  [in]  obj  sle obj id
 * @param  [in]  device_num number of remote device
 * @param  [in]  device_id_list list of remote device id
 * @retval #void  No return value.
 * @par Dependency:
 * @else
 * @brief 星闪对象分配上报。
 * @par 说明: 星闪对象分配上报。
 * @param  [in]  obj  星闪对象编号
 * @param  [in]  device_num  对端设备数量
 * @param  [in]  device_id_list  对端设备编号列表
 * @retval #void  无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_obj_alloc_cbk)(sle_obj_t obj, uint32_t device_num, uint16_t *device_id_list);

/**
 * @if Eng
 * @brief SLE Object Recovery Report.
 * @par Description: SLE Object Recovery Report.
 * @param  [in]  obj  sle obj id
 * @param  [in]  device_num number of remote device
 * @param  [in]  device_id_list list of remote device id
 * @param  [in]  addr_list list of remote device addr
 * @retval #void  No return value.
 * @par Dependency:
 * @else
 * @brief 星闪对象恢复上报。
 * @par 说明: 星闪对象恢复上报。
 * @param  [in]  obj  星闪对象编号
 * @param  [in]  device_num  对端设备数量
 * @param  [in]  device_id_list  对端设备编号列表
 * @param  [in]  addr_list  对端设备地址列表
 * @retval #void  无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_sle_obj_recover_cbk)(
    sle_obj_t obj, uint32_t device_num, uint16_t *device_id_list, sle_addr_t *addr_list);

/**
 * @if Eng
 * @brief  Struct of br sle device callback function.
 * @else
 * @brief  SLE 设备回调函数结构体。
 * @endif
 */
typedef struct {
    bs_sle_obj_alloc_cbk obj_alloc_cbk;
    bs_sle_obj_recover_cbk obj_recover_cbk;
} bs_sle_device_callbacks_t;

/**
 * @if Eng
 * @brief SLE Device Register Callback Function.
 * @par Description: SLE Device Register Callback Function.
 * @param  [in]  app_id  app id
 * @retval #ERRCODE_SLE_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_sle_t
 * @par Dependency:
 * @else
 * @brief 星闪对注册设备回调函数。
 * @par 说明: 星闪对注册设备回调函数。
 * @param  [in]  app_id  应用id
 * @retval #ERRCODE_SLE_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_sle_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_sle_register_device_callbacks(uint32_t app_id, bs_sle_device_callbacks_t *cbks);

/**
 * @if Eng
 * @brief SLE Device Unregister Callback Function.
 * @par Description: SLE Device Unregister Callback Function.
 * @param  [in]  app_id  app id
 * @retval #ERRCODE_SLE_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_sle_t
 * @par Dependency:
 * @else
 * @brief 星闪对取消注册设备回调函数。
 * @par 说明: 星闪对取消注册设备回调函数。
 * @param  [in]  app_id  应用id
 * @retval #ERRCODE_SLE_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_sle_t
 * @par 依赖:
 * @endif
 */
void bs_sle_unregister_device_callbacks(uint32_t app_id);

/**
 * @if Eng
 * @brief SLE Removes a Specified Device.
 * @par Description: SLE Removes a Specified Device.
 * @param  [in]  addr  device addr
 * @retval #ERRCODE_SLE_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_sle_t
 * @par Dependency:
 * @else
 * @brief 星闪移除指定设备addr。
 * @par 说明: 星闪移除指定设备addr。
 * @param  [in]  addr  设备addr
 * @retval #ERRCODE_SLE_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_sle_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_sle_remove_addr(uint8_t *addr);
#ifdef __cplusplus
}
#endif
#endif /* SSF_DEVICE_H */