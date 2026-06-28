/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BR call service sco
 */
#ifndef BS_BR_CALL_SCO_H
#define BS_BR_CALL_SCO_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "bs_br_call.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief sco status
 * @else
 * @brief sco状态
 * @endif
 */
typedef enum {
    BR_SCO_STATE_CONNECTING = 0x01,    /*!< @if Eng status of sco is connecting. @else sco正在连接。 @endif */
    BR_SCO_STATE_CONNECTED = 0x02,     /*!< @if Eng status of sco is connected. @else sco已连接。 @endif */
    BR_SCO_STATE_DISCONNECTED = 0x03,   /*!< @if Eng status of sco is disconnected. @else sco已断连。 @endif */
} bs_br_sco_state_t;

/**
 * @if Eng
 * @brief sco codec
 * @else
 * @brief sco编解码类型
 * @endif
 */
typedef enum {
    BR_SCO_CODEC_ID_CVSD = 0x01, /*!< @if Eng codec type of CVSD @else 编码格式为CVSD @endif */
    BR_SCO_CODEC_ID_MSBC,        /*!< @if Eng codec type of MSBC @else 编码格式为MSBC @endif */
} bs_br_sco_codec_id_t;

/**
 * @if Eng
 * @brief Use this function to create sco connection.
 * @par Description: Use this function to create sco connection.
 * @param  [in]  dev_id The id of one connected device.
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 建立sco音频流连接。
 * @par 说明: 建立sco音频流连接。
 * @param  [in]  dev_id 对端设备编号。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_create_sco_connection(uint16_t dev_id);

/**
 * @if Eng
 * @brief Use this function to release sco connection.
 * @par Description: Use this function to release sco connection.
 * @param  [in]  dev_id The id of one connected device.
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 断开sco音频流连接。
 * @par 说明: 断开sco音频流连接。
 * @param  [in]  dev_id 对端设备编号。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_release_sco_connection(uint16_t dev_id);

/**
 * @if Eng
 * @brief Use this function to reply sco connection result.
 * @par Description: Use this function to reply sco connection result.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  is_accept true:accept false:reject.
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 接受或拒绝sco连接。
 * @par 说明: 接受或拒绝sco连接。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  is_accept true:接受 false:拒绝.
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_create_sco_result_reply(uint16_t dev_id, bool is_accept);

/**
 * @if Eng
 * @brief Callback invoked when sco state changed.
 * @par Description: Callback invoked when sco state changed.
 * @param  [in]  dev_id  The id of one connected device.
 * @param  [in]  hfp_role local hfp profile role。See @ref bs_br_call_hfp_role_t
 * @param  [in]  sco_state  The status of sco oeration result, @ref {bs_br_sco_state_t}.
 * @retval #void  No return value.
 * @par Dependency:
 * @else
 * @brief sco状态上报。
 * @par 说明: sco状态上报。
 * @param  [in]  dev_id  对端设备编号
 * @param  [in]  hfp_role 本端服务角色。参考 @ref bs_br_call_hfp_role_t
 * @param  [in]  sco_state  sco状态, 参考 {bs_br_sco_state_t}.
 * @retval #void  无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_br_sco_state_changed_cbk)(uint16_t dev_id, uint8_t hfp_role, uint8_t sco_state);

/**
 * @if Eng
 * @brief Callback invoked when receive the result of sco oeration.
 * @par Description: Callback invoked when receive sco create request.
 * @param  [in]  dev_id  The id of one connected device.
 * @param  [in]  hfp_role local hfp profile role。See @ref bs_br_call_hfp_role_t
 * @param  [in]  codec_id  The codec id of sco, @ref {bs_br_sco_codec_id_t}.
 * @retval #void  No return value.
 * @par Dependency:
 * @else
 * @brief sco编解码上报。
 * @par 说明: sco编解码上报。
 * @param  [in]  dev_id  对端设备编号
 * @param  [in]  hfp_role 本端服务角色。参考 @ref bs_br_call_hfp_role_t
 * @param  [in]  codec_id  编解码,  参考 {bs_br_sco_codec_id_t}.
 * @retval #void  无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_br_sco_codec_report_cbk)(uint16_t dev_id, uint8_t hfp_role, uint8_t codec_id);

/**
 * @if Eng
 * @brief sco callbacks
 * @else
 * @brief sco回调函数
 * @endif
 */
typedef struct {
    bs_br_sco_state_changed_cbk sco_state_cbk;
    bs_br_sco_codec_report_cbk sco_codec_cbk;
} bs_br_sco_callbacks_t;

/**
 * @if Eng
 * @brief Use this function to register sco callbacks.
 * @par Description: Use this function to register sco callbacks.
 * @param  [in]  app_id application id , default is 0.
 * @param  [in]  func the pointer of callbacks struct.
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 注册蓝牙sco回调函数。
 * @par 说明: 注册蓝牙sco回调函数。
 * @param  [in]  app_id 应用id 默认为0。
 * @param  [in]  func 回调函数。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_sco_register_callbacks(uint8_t app_id, bs_br_sco_callbacks_t *func);

/**
 * @if Eng
 * @brief Use this function to unregister sco callbacks.
 * @par Description: Use this function to unregister sco callbacks.
 * @param  [in]  app_id application id , default is 0.
 * @retval #ERRCODE_BT_SUCCESS   Execute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 去注册蓝牙sco回调函数。
 * @par 说明: 去注册蓝牙sco回调函数。
 * @param  [in]  app_id 应用id 默认为0。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_sco_unregister_callbacks(uint8_t app_id);

#ifdef __cplusplus
}
#endif
#endif