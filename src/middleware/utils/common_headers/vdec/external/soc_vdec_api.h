/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2024-10-11
 */

#ifndef SOC_VDEC_API_H
#define SOC_VDEC_API_H

#include "soc_vdec_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup vdec_api vdec_api
 * @ingroup  vdec_api
 * @{
 */

/*-------------------------------- func declares ------------------------------------------*/

/**
 * @if Eng
 * @brief  This interface is used to get vdec capability.
 * @param  capability   [in && out] capability.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取当前解码能力集
 * @param  capability   [in && out] 能力集
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_get_capability(ext_vdec_capability *capability);

/**
 * @if Eng
 * @brief  This interface is used to judge whether the standard is supported.
 * @param  standard     [in]  standard.
 * @param  is_support   [out] support.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  判断当前协议是否支持
 * @param  standard     [in]  协议
 * @param  is_support   [out] 是否支持
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_is_standard_support(ext_vdec_standard standard, td_bool *is_support);

/**
 * @if Eng
 * @brief  This interface is used to create a decoder.
 * @param  handle       [out] the handle of decoder. The application must provide memory space.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  创建解码器，该接口返回handle用于其它接口调用
 * @param  handle [out] 解码器句柄，用户需要为其提供内存空间
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_create(td_handle *handle);

/**
 * @if Eng
 * @brief  This interface is used to destroy a decoder.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  销毁解码器
 * @param  handle       [in]  解码句柄
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_destroy(td_handle handle);

/**
 * @if Eng
 * @brief  This interface is used to start a decoder.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  启动解码器
 * @param  handle       [in]  解码句柄
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_start(td_handle handle);

/**
 * @if Eng
 * @brief  This interface is used to stop a decoder.
 * @param  handle [in]  the handle of decoder, created by uapi_vdec_create.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  暂停解码器
 * @param  handle       [in]  解码句柄
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_stop(td_handle handle);

/**
 * @if Eng
 * @brief  This interface is used to reset a decoder.
 * @param  handle       [in]   the handle of decoder, created by uapi_vdec_create.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  复位解码器
 * @param  handle       [in]  解码句柄
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_reset(td_handle handle);

/**
 * @if Eng
 * @brief  This interface is used to flush a decoder.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @param  option       [in]  interface control option.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  刷新解码器
 * @param  handle       [in]  解码句柄
 * @param  option       [in]  控制选项
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_flush(td_handle handle, const ext_vdec_opt_flush *option);

/**
 * @if Eng
 * @brief  This interface is used to decode one frame, supports blocking or non-blocking calls..
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @param  stream       [in]  stream data to be decoded. The application must read/copy stream to the memory.
 * @param  frame        [out] frame struct to be filled.
 * @param  option       [in]  interface control option.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  单帧解码，支持阻塞或者非阻塞式调用
 * @param  handle       [in]  解码句柄
 * @param  stream       [in]  需要解码的原始数据。用户需要将码流读至或者拷贝至一块用户内存中
 * @param  frame        [out] 帧输出结构体
 * @param  option       [in]  控制选项
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_decode(td_handle handle, const ext_vdec_stream *stream, ext_vdec_frame *frame,
    const ext_vdec_opt_decode *option);

/**
 * @if Eng
 * @brief  This interface is used to get channel attribute.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @param  attr         [out] channel attribute.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取通道属性
 * @param  handle       [in]  解码句柄
 * @param  attr         [out] 通道属性
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_get_attr(td_handle handle, ext_vdec_attr *attr);

/**
 * @if Eng
 * @brief  This interface is used to set channel attribute.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @param  attr         [in]  channel attribute.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  设置通道属性
 * @param  handle       [in]  解码句柄
 * @param  attr         [in]  通道属性
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_set_attr(td_handle handle, const ext_vdec_attr *attr);

/**
 * @if Eng
 * @brief  This interface is used to get channel paramter.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @param  param        [out] paramter.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取通道参数
 * @param  handle       [in]  解码句柄
 * @param  param        [out] 参数内容
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_get_param(td_handle handle, ext_vdec_param *param);

/**
 * @if Eng
 * @brief  This interface is used to set channel paramter.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @param  param        [in]  paramter.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  设置通道参数
 * @param  handle       [in]  解码句柄
 * @param  param        [in]  参数内容
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_set_param(td_handle handle, const ext_vdec_param *param);

/**
 * @if Eng
 * @brief  This interface is used to get channel feature.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @param  feature      [out] vdec features, such as width and height, frame rate, sampling type, and color space.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取通道特性
 * @param  handle       [in]  解码句柄
 * @param  feature      [out] 解码特性，例如宽高，帧率，采样类型，颜色空间等
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_get_feature(td_handle handle, ext_vdec_feature *feature);

/**
 * @if Eng
 * @brief  This interface is used to get channel status.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @param  status       [out] vdec status, stream status and frame status.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取通道状态
 * @param  handle       [in]  解码句柄
 * @param  status       [out] 解码状态，码流状态与帧存状态
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_get_status(td_handle handle, ext_vdec_status *status);

/**
 * @if Eng
 * @brief  This interface is used to get free stream buffer.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @param  stream       [out] stream buffer, application must to use uapi_vdec_put_stream to return buffer.
 * @param  option       [in]  interface control option.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取空闲的码流缓冲
 * @param  handle       [in]  解码句柄
 * @param  stream       [out] 码流缓冲，用户使用uapi_vdec_put_stream接口归还码流缓冲
 * @param  option       [in]  控制选项
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_get_stream(td_handle handle, ext_vdec_stream *stream, const ext_vdec_opt_get_stream *option);

/**
 * @if Eng
 * @brief  This interface is used to put stream buffer.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @param  stream       [in]  stream to be decoded, get stream memory from uapi_vdec_get_stream.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  归还码流缓冲
 * @param  handle       [in]  解码句柄
 * @param  stream       [in]  码流缓冲，其码流内存来自于uapi_vdec_get_stream
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_put_stream(td_handle handle, const ext_vdec_stream *stream);

/**
 * @if Eng
 * @brief  This interface is used to acquire frame,supports blocking or non-blocking calls.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @param  frame        [out] frame buffer
 * @param  option       [in]  interface control option.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取解码帧存，支持阻塞或者非阻塞式调用
 * @param  handle       [in]  解码句柄
 * @param  frame        [out] 帧存缓冲
 * @param  option       [in]  控制选项
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_acquire_frame(td_handle handle, ext_vdec_frame *frame, const ext_vdec_opt_acq_frame *option);

/**
 * @if Eng
 * @brief  This interface is used to release frame.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @param  frame        [in]  frame buffer.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  释放解码帧存
 * @param  handle       [in]  解码句柄
 * @param  frame        [in]  帧存缓冲
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_release_frame(td_handle handle, const ext_vdec_frame *frame);

/**
 * @if Eng
 * @brief  This interface is used to register event.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @param  event_map    [in]  event set.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  注册事件
 * @param  handle       [in]  解码句柄
 * @param  event_map    [in]  事件集合
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_register_event(td_handle handle, td_u64 event_map);

/**
 * @if Eng
 * @brief  This interface is used to listen event, supports blocking or non-blocking calls.
 * @param  handle       [in]        the handle of decoder, created by uapi_vdec_create.
 * @param  event        [in && out] event which is triggered.
 * @param  option       [in]        interface control option.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  监测解码事件，支持阻塞或者非阻塞式调用
 * @param  handle       [in]        解码句柄
 * @param  event        [in && out] 触发事件
 * @param  option       [in]        控制选项
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_listen_event(td_handle handle, ext_vdec_event *event, const ext_vdec_opt_listen *option);

/**
 * @if Eng
 * @brief  This interface is used to send end of stream flag.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  发送码流结束标志
 * @param  handle       [in]  解码句柄
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_send_eos(td_handle handle);

/**
 * @if Eng
 * @brief  This interface is used to bind frame buffer.
 * @param  handle       [in]  the handle of decoder, created by uapi_vdec_create.
 * @param  buffer       [in]  frame buffer.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  绑定帧存缓冲
 * @param  handle       [in]  解码句柄
 * @param  buffer       [in]  帧存缓冲
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_vdec_bind_frame_buffer(td_handle handle, const ext_vdec_frame_buf *buffer);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SOC_VDEC_API_H */
