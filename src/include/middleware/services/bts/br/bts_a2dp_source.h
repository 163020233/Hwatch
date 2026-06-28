/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: BT A2DP-SRC module.
 */

/**
 * @addtogroup bluetooth_bts_a2dp_source A2DP SRC API
 * @ingroup    bluetooth
 * @{
 */

#ifndef BTS_A2DP_SOURCE_H
#define BTS_A2DP_SOURCE_H

#include "errcode.h"
#include "bts_def.h"
#include "bts_a2dp_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Setting of A2dp codec priority.
 * @else
 * @brief A2dp 编码格式优先级设置。
 * @endif
 */
#define A2DP_CODEC_PRIORITY_DISABLED (-1)
#define A2DP_CODEC_PRIORITY_DEFAULT 0
#define A2DP_CODEC_PRIORITY_HIGHEST (1000 * 1000)

/**
 * @if Eng
 * @brief Sample rate of a2dp.
 * @else
 * @brief A2dp 采样率。
 * @endif
 */
#define A2DP_CODEC_SAMPLE_RATE_NONE 0x00
#define A2DP_CODEC_SAMPLE_RATE_44100 (0x01 << 0)
#define A2DP_CODEC_SAMPLE_RATE_48000 (0x01 << 1)
#define A2DP_CODEC_SAMPLE_RATE_88200 (0x01 << 2)
#define A2DP_CODEC_SAMPLE_RATE_96000 (0x01 << 3)
#define A2DP_CODEC_SAMPLE_RATE_176400 (0x01 << 4)
#define A2DP_CODEC_SAMPLE_RATE_192000 (0x01 << 5)

/**
 * @if Eng
 * @brief Sample bit of a2dp sample rate.
 * @else
 * @brief A2dp 的采样比特位。
 * @endif
 */
#define A2DP_CODEC_BITS_PER_SAMPLE_NONE 0x00
#define A2DP_CODEC_BITS_PER_SAMPLE_16 (0x01 << 0)
#define A2DP_CODEC_BITS_PER_SAMPLE_24 (0x01 << 1)
#define A2DP_CODEC_BITS_PER_SAMPLE_32 (0x01 << 2)

/**
 * @if Eng
 * @brief Channel mode of a2dp NONE.
 * @else
 * @brief A2dp声道模式NONE。
 * @endif
 */
#define A2DP_CODEC_CHANNEL_MODE_NONE 0x00

/**
 * @if Eng
 * @brief Channel mode of a2dp MONO.
 * @else
 * @brief A2dp声道模式MONO。
 * @endif
 */
#define A2DP_CODEC_CHANNEL_MODE_MONO (0x01 << 0)
/**
 * @if Eng
 * @brief Channel mode of a2dp STEREO.
 * @else
 * @brief A2dp声道模式STEREO。
 * @endif
 */
#define A2DP_CODEC_CHANNEL_MODE_STEREO (0x01 << 1)

/**
 * @if Eng
 * @brief  Callback invoked in response to a2dp src connection state changed.
 * @par Description:
 *           When registered, the bts uses this callback to deliver a2dp connect stat info updated
 *           after connection state updated.
 * @attention  1. This function is called in bts context, should not be blocked or do long time waiting.
 * @attention  2. The memories of devices are requested and freed by the bts automatically.
 * @param  [in]  bd_addr        A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @param  [in]  conn_state     A2dp src connection state. See @ref profile_connect_state_t
 * @retval No return value. See  @ref void
 * @else
 * @brief  a2dp src连接状态改变回调函数。
 * @par 说明:
 *           注册该回调函数之后，BTS每次完成a2dp状态更新后调用该接口反馈结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. devices由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bd_addr        对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @param  [in]  conn_state     A2dp src的连接状态。参考 @ref profile_connect_state_t
 * @retval 无返回值。 参考 @ref void
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 * @see  a2dp_src_callbacks_t
 */
typedef void (*a2dp_src_connect_state_changed_callback)(const bd_addr_t *bd_addr, uint8_t conn_state);

/**
 * @if Eng
 * @brief  Callback invoked in response to a2dp stream state changed.
 * @par Description:
 *           When registered, the bts uses this callback to deliver a2dp stream state
 *           after stream state changed.
 * @attention  1. This function is called in bts context, should not be blocked or do long time waiting.
 * @attention  2. The memories of devices are requested and freed by the bts automatically.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @param  [in]  stream_handle The stream handle.
 * @param  [in]  state       The stream state. See @ref a2dp_stream_state_t
 * @retval No return value. See @ref void
 * @else
 * @brief  A2dp流状态改变回调函数。
 * @par 说明:
 *           注册该回调函数之后，BTS每次完成a2dp流状态更新后调用该接口反馈结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. devices由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @param  [in]  stream_handle 流句柄.
 * @param  [in]  state       流状态. See @ref a2dp_stream_state_t
 * @retval 无返回值。参考 @ref void
 * @endif
 * @par Dependency:
 * @see  a2dp_snk_callbacks_t
 */
typedef void (*a2dp_src_stream_state_changed_callback)(const bd_addr_t *bd_addr, uint32_t stream_handle, uint8_t state);

/**
 * @if Eng
 * @brief  Callback invoked in response to a2dp codec information changed.
 * @par Description:
 *           When registered, the bts uses this callback to deliver a2dp codec information
 *           after codec information changed.
 * @attention  1. This function is called in bts context, should not be blocked or do long time waiting.
 * @attention  2. The memories of devices are requested and freed by the bts automatically.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @param  [in]  info       A poniter of device's codec information. See @ref a2dp_stream_codec_t
 * @retval No return value. See @ref void
 * @else
 * @brief  A2dp 编码格式改变回调函数。
 * @par 说明:
 *           注册该回调函数之后，BTS每次完成a2dp状态更新后调用该接口反馈结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. devices由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @param  [in]  info       A2dp 的编码格式信息。参考 @ref a2dp_stream_codec_t
 * @retval 无返回值。参考 @ref void
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 * @see  a2dp_src_callbacks_t
 */
typedef void (*a2dp_src_configuration_changed_callback)(const bd_addr_t *bd_addr, const a2dp_stream_codec_t *info);

/**
 * @if Eng
 * @brief  Callback invoked in response to A2DP request to start an audio stream.
 * @par Description:
 *           When registered, the bts uses this callback to deliver A2DP start stream request
 *           when received a request to start an audio stream.
 * @attention  1. This function is called in bts context, should not be blocked or do long time waiting.
 * @attention  2. The memories of devices are requested and freed by the bts automatically.
 * @param  [in]  bd_addr        A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval No return value. See  @ref void
 * @else
 * @brief  A2DP请求流开始回调函数。
 * @par 说明:
 *           注册该回调函数之后，BTS每次收到A2DP请求流开始时调用该接口反馈结果给上层。
 * @attention  1. 该回调函数运行于bts线程，不能阻塞或长时间等待。
 * @attention  2. devices由bts申请内存，也由bts释放，回调中不应释放。
 * @param  [in]  bd_addr        对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval 无返回值。 参考 @ref void
 * @endif
 * @par Dependency:
 * @li bts_a2dp_snk.h
 * @see  a2dp_snk_callbacks_t
 */
typedef void (*a2dp_src_start_stream_request_callback)(bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief Struct of a2dp src callback function.
 * @else
 * @brief A2dp src回调接口定义。
 * @endif
 */
typedef struct {
    a2dp_src_connect_state_changed_callback connectstate_changed_cb;
    a2dp_src_configuration_changed_callback configuration_changed_cb;
    a2dp_src_stream_state_changed_callback  stream_state_changed_cb;
    a2dp_src_start_stream_request_callback start_stream_request_cb;
} a2dp_src_callbacks_t;

/**
 * @if Eng
 * @brief  Use this funtion to connect to the peer bluetooth device by address.
 * @par Description:
 *           Use this funtion to connect to the peer bluetooth device by address.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  与对端设备建立a2dp连接。
 * @par 说明:
 *           与对端设备建立a2dp连接。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
errcode_t a2dp_src_connect(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to disconnect to the peer bluetooth device by address.
 * @par Description:
 *           Use this funtion to disconnect to the peer bluetooth device by address.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  与对端设备断开a2dp连接。
 * @par 说明:
 *           与对端设备断开a2dp连接。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
errcode_t a2dp_src_disconnect(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to set the status of Audio start streaming.
 * @par Description:
 *           Use this funtion to set the status of Audio start streaming.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  设置蓝牙音乐的播放为start状态。
 * @par 说明:
 *           设置蓝牙音乐的播放为start状态。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
errcode_t a2dp_src_start_playing(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to set the status of Audio suspend streaming.
 * @par Description:
 *           Use this funtion to set the status of Audio suspend streaming.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  设置蓝牙音乐的播放为suspend状态。
 * @par 说明:
 *           设置蓝牙音乐的播放为suspend状态。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
errcode_t a2dp_src_suspend_playing(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to set the status of Audio stop streaming.
 * @par Description:
 *           Use this funtion to set the status of Audio stop streaming.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  设置蓝牙音乐的播放为stop状态。
 * @par 说明:
 *           设置蓝牙音乐的播放为stop状态。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
errcode_t a2dp_src_stop_playing(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this function to accept a request to start an audio stream.
 * @par Description:
 *           Use this function to accept a request to start an audio stream.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  使用此函数接受启动音频流的请求。
 * @par 说明:
 *           使用此函数接受启动音频流的请求。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
errcode_t a2dp_src_start_accept(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this function to reject a request to start an audio stream.
 * @par Description:
 *           Use this function to reject a request to start an audio stream.
 * @param  [in]  bd_addr    A poniter of the peer bluetooth device addresss. See @ref bd_addr_t
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  使用此函数拒绝启动音频流的请求。
 * @par 说明:
 *           使用此函数拒绝启动音频流的请求。
 * @param  [in]  bd_addr    对端蓝牙设备的地址信息。参考 @ref bd_addr_t
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
errcode_t a2dp_src_start_reject(const bd_addr_t *bd_addr);

/**
 * @if Eng
 * @brief  Use this funtion to register callback function of framework.
 * @par Description:
 *           Use this funtion to register callback function of framework.
 * @param  [in]  func   A poniter of the callback function. See @ref a2dp_src_callbacks_t
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  注册上层应用的回调。
 * @par 说明:
 *           注册上层应用的回调。
 * @param  [in]  func   回调函数指针。参考 @ref a2dp_src_callbacks_t
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
errcode_t a2dp_src_register_callbacks(a2dp_src_callbacks_t *func);

/**
 * @if Eng
 * @brief  Use this funtion to deregister callback function of framework.
 * @par Description:
 *           Use this funtion to deregister callback function of framework.
 * @param  [in]  func   A poniter of the callback function. See @ref a2dp_src_callbacks_t
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  去注册上层应用的回调。
 * @par 说明:
 *           去注册上层应用的回调。
 * @param  [in]  func   回调函数指针。参考 @ref a2dp_src_callbacks_t
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
errcode_t a2dp_src_deregister_callbacks(a2dp_src_callbacks_t *func);

/**
 * @if Eng
 * @brief  Use this funtion to register A2DP SRC service.
 * @par Description:
 *           Use this funtion to register A2DP SRC service.
 * @param  NULL
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  注册A2DP SRC服务。
 * @par 说明:
 *           注册A2DP SRC服务。
 * @param   NULL
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
errcode_t a2dp_src_service_register(void);

/**
 * @if Eng
 * @brief  Use this funtion to unregister A2DP SRC service.
 * @par Description:
 *           Use this funtion to unregister A2DP SRC service.
 * @param  NULL
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  去注册A2DP SRC服务。
 * @par 说明:
 *           去注册A2DP SRC服务。
 * @param   NULL
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
errcode_t a2dp_src_service_unregister(void);

/**
 * @if Eng
 * @brief  Use this funtion to register codec before a2dp src connect.
 * @par Description:
 *           Use this funtion to register codec before a2dp src connect.
 * @param  [in] codec_caps the point of codec info, @ref a2dp_stream_codec_param_t.
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  配置编解码（发起a2dp src连接之前）。
 * @par 说明:
 *           配置编解码（发起a2dp src连接之前）。
 * @param  [in] codec_caps 编解码信息指针, @ref a2dp_stream_codec_param_t.
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
errcode_t a2dp_src_codec_config(const a2dp_stream_codec_param_t *codec_caps);

/**
 * @if Eng
 * @brief  Use this funtion to reconfig codec when a2dp src stream state is suspend or open.
 * @par Description:
 *           Use this funtion to reconfig codec when a2dp src stream state is suspend or open.
 * @param  [in] bd_addr the point of bluetooth address, @ref bd_addr_t.
 * @param  [in] codec_caps the point of codec info, @ref a2dp_stream_codec_param_t.
 * @retval ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  重新协商编解码（在流状态为暂停或打开状态时）。
 * @par 说明:
 *           重新协商编解码（在流状态为暂停或打开状态时）。
 * @param  [in] bd_addr 蓝牙地址指针, @ref bd_addr_t.
 * @param  [in] codec_caps 编解码信息指针, @ref a2dp_stream_codec_param_t.
 * @retval ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 *            @li bts_a2dp_source.h
 */
errcode_t a2dp_src_codec_reconfig(const bd_addr_t *bd_addr, const a2dp_stream_codec_param_t *codec_caps);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* BTS_A2DP_SOURCE_H */
