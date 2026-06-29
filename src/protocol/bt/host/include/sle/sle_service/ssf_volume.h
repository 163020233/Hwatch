
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Service Framework (SSF) Functions about Volume API File.
 */

#ifndef SSF_VOLUME_H
#define SSF_VOLUME_H
#include <stdint.h>
#include "errcode.h"
#include "ssf_device.h"
#include "bs_sdm_common_stru.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief  Callback function for receiving call volume adjustment requests.
 * @par    After registering this callback function, the server (headset) will report the callback to the upper
 *         layer upon receiving a call volume adjustment request from the client.
 * @param [in] param              Volume synchronization information.
 * @else
 * @brief  收到通话音量调整请求的回调函数。
 * @par    注册该回调函数之后，服务端（耳机）收到客户端通话音量调整请求后该回调函数会上报上层。
 * @param [in] param              音量同步信息。
 * @endif
 */
typedef void (*bs_sle_set_call_volume_request_cbk)(bs_sle_volume_update_event_param_t *param);

/**
 * @if Eng
 * @brief  Callback function for receiving media volume adjustment requests.
 * @par    After registering this callback function, the server (headset) will report the callback to the upper
 *         layer upon receiving a media volume adjustment request from the client.
 * @param [in] param              Volume synchronization information.
 * @else
 * @brief  收到媒体音量调整请求的回调函数。
 * @par    注册该回调函数之后，服务端（耳机）收到客户端媒体音量调整请求后该回调函数会上报上层。
 * @param [in] param              音量同步信息。
 * @endif
 */
typedef void (*bs_sle_set_media_volume_request_cbk)(bs_sle_volume_update_event_param_t *param);

/**
 * @if Eng
 * @brief  Callback function for receiving call volume adjustment requests.
 * @par    After registering this callback function, the client will report the callback to the upper
 *         layer upon receiving a call volume adjustment request from the server (headset).
 * @param [in] obj              device object.
 * @param [in] volume           volume value.
 * @else
 * @brief  收到通话音量调整请求的回调函数。
 * @par    注册该回调函数之后，客户端收到服务端（耳机）通话音量调整请求后该回调函数会上报上层。
 * @param [in] obj              设备对象。
 * @param [in] volume           音量操作值。
 * @endif
 */
typedef void (*bs_sle_set_call_volume_notify_cbk)(sle_obj_t obj, uint8_t volume);

/**
 * @if Eng
 * @brief  Callback function for receiving media volume adjustment requests.
 * @par    After registering this callback function, the client will report the callback to the upper
 *         layer upon receiving a media volume adjustment request from the server (headset).
 * @param [in] obj              device object.
 * @param [in] volume           volume value.
 * @else
 * @brief  收到媒体音量调整请求的回调函数。
 * @par    注册该回调函数之后，客户端收到服务端（耳机）媒体音量调整请求后该回调函数会上报上层。
 * @param [in] obj              设备对象。
 * @param [in] volume           音量操作值。
 * @endif
 */
typedef void (*bs_sle_set_media_volume_notify_cbk)(sle_obj_t obj, uint8_t volume);

/**
 * @if Eng
 * @brief Struct of volume manage callback function.
 * @else
 * @brief 音量控制回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_sle_set_call_volume_request_cbk      call_volume_request_cbk;
    bs_sle_set_media_volume_request_cbk     media_volume_request_cbk;
    bs_sle_set_call_volume_notify_cbk       call_volume_notify_cbk;
    bs_sle_set_media_volume_notify_cbk      media_volume_notify_cbk;
} bs_sle_volume_callbacks_t;

/**
 * @if Eng
 * @brief register the callback function for volume control.
 * @par Description: register the callback function for volume control.
 * @param [in] cb               callback function.
 * @retval errcode { @ref errcode_t }.
 * @par Depends:
 * @li
 * @else
 * @brief  注册音量控制回调函数。
 * @par Description: 注册音量控制回调函数。
 * @param [in] cb               回调函数。
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li
 * @endif
 */
errcode_t bs_sle_volume_register_callbacks(uint32_t app_id, bs_sle_volume_callbacks_t *cb);

/**
 * @if Eng
 * @brief unregister the callback function for volume control.
 * @par Description: unregister the callback function for volume control.
 * @param [in] None
 * @retval errcode { @ref errcode_t }.
 * @par Depends:
 * @li
 * @else
 * @brief  注销音量控制回调函数。
 * @par Description: 注销音量控制回调函数。
 * @param [in] 无
 * @retval 执行结果错误码。
 * @par 依赖：
 * @li
 * @endif
 */
errcode_t bs_sle_volume_unregister_callbacks(uint32_t app_id);

/**
 * @if Eng
 * @brief  Set call volume.
 * @par Description: Client sets server (headset) call volume.
 * @param [in] obj              device object.
 * @param [in] volume           Volume value.
 * @retval error code.
 * @else
 * @brief  设置通话音量。
 * @par Description: 客户端设置服务端（耳机）通话音量。
 * @param [in] obj              设备对象.
 * @param [in] volume           音量值
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t bs_sle_set_call_volume_request(sle_obj_t obj, uint8_t volume);

/**
 * @if Eng
 * @brief  Set media volume.
 * @par Description: Client sets server (headset) media volume.
 * @param [in] obj              device object.
 * @param [in] volume           Volume value.
 * @retval error code.
 * @else
 * @brief  设置媒体音量。
 * @par Description: 客户端设置服务端（耳机）媒体音量。
 * @param [in] obj              设备对象.
 * @param [in] volume           音量值
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t bs_sle_set_media_volume_request(sle_obj_t obj, uint8_t volume);

/**
 * @if Eng
 * @brief  Set media volume by channel.
 * @par Description: Client sets server (Home theater sound box) media volume.
 * @param [in] obj              device object.
 * @param [in] volume           Volume value.
 * @param [in] channel          Channel enumeration value.
 * @retval error code.
 * @else
 * @brief  根据声道设置媒体音量。
 * @par Description: 客户端设置服务端（家庭影院音箱）媒体音量。
 * @param [in] obj              设备对象.
 * @param [in] volume           音量值
 * @param [in] channel          声道枚举值
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t bs_sle_set_media_volume_request_by_channel(sle_obj_t obj, uint8_t volume, uint8_t channel);

/**
 * @if Eng
 * @brief  Send volume setting request.
 * @par Description: The server (headset) sends a volume setting request to the client.
 * @param [in] obj              device object.
 * @param [in] volume           Volume value.
 * @retval error code.
 * @else
 * @brief  发送音量设置请求。
 * @par Description: 服务端（耳机）向客户端发送音量设置请求
 * @param [in] obj              设备对象.
 * @param [in] volume           音量值
 * @retval 执行结果错误码。
 * @endif
 */
errcode_t bs_sle_set_main_volume_notify(sle_obj_t obj, uint8_t volume);

#ifdef __cplusplus
}
#endif
#endif /* SSF_VOLUME_H */