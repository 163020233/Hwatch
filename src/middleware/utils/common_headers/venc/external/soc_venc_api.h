/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: venc interface
 * Author: Hisilicon multimedia software group
 * Create: 2025/03/15
 * History:
 */
#ifndef SOC_VENC_API_H
#define SOC_VENC_API_H

#include "soc_venc_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup venc_api venc_api
 * @ingroup  video_codec_api
 * @{
 */

/*-------------------------------- func declares ------------------------------------------*/

/**
 * @if Eng
 * @brief  This interface is used to judge whether a standard is supported.
 * @param  type         [in]  standard
 * @param  is_support   [out] support
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  判断当前协议是否支持
 * @param  type         [in]  协议
 * @param  is_support   [out] 是否支持
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_is_standard_support(ext_venc_standard type, td_bool *is_support);

/**
 * @if Eng
 * @brief  This interface is used to get default attribute.
 * @param  info         [in]  scene info
 * @param  attr         [out] attribute
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取默认属性
 * @param  info         [in]  场景信息
 * @param  attr         [out] 默认属性
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_get_default_attr(const ext_venc_scene_info *info, ext_venc_attr *attr);

/**
 * @if Eng
 * @brief  This interface is used to create an encoder.
 * @param  handle       [out] the handle of encoder
 * @param  option       [in]  create option
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  创建编码器实例，返回的句柄（handle）用于后续调用
 * @param  handle       [out] 编码器句柄
 * @param  option       [in]  控制选项
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_create(td_handle *handle, const ext_venc_opt_create *option);

/**
 * @if Eng
 * @brief  This interface is used to destroy an encoder.
 * @param  handle       [in]  the handle of encoder
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  销毁编码器
 * @param  handle       [in]  编码句柄
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_destroy(td_handle handle);

/**
 * @if Eng
 * @brief  This interface is used to start an encoder.
 * @param  handle       [in]  the handle of encoder
 * @param  option       [in]  start option.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  启动编码器
 * @param  handle       [in]  编码句柄
 * @param  option       [in]  控制选项
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_start(td_handle handle, const ext_venc_opt_start *option);

/**
 * @if Eng
 * @brief  This interface is used to stop an encoder.
 * @param  handle       [in]  the handle of encoder
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  暂停编码器
 * @param  handle       [in]  编码句柄
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_stop(td_handle handle);

/**
 * @if Eng
 * @brief  This interface is used to reset an encoder.
 * @param  handle       [in]   the handle of encoder
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  复位编码器
 * @param  handle       [in]  编码句柄
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_reset(td_handle handle);

/**
 * @if Eng
 * @brief  This interface is used to flush a encoder.
 * @param  handle       [in]  the handle of encoder
 * @param  option       [in]  interface control option.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  刷新编码器
 * @param  handle       [in]  编码句柄
 * @param  option       [in]  控制选项
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_flush(td_handle handle);

/**
 * @if Eng
 * @brief  This interface is used to get channel status.
 * @param  handle       [in]  the handle of encoder
 * @param  status       [out] venc status.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取通道状态
 * @param  handle       [in]  编码句柄
 * @param  status       [out] 编码状态
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_get_status(td_handle handle, ext_venc_status *status);

/**
 * @if Eng
 * @brief  This interface is used to send frame, supports blocking or non-blocking calls.
 * @param  handle       [in]  the handle of encoder
 * @param  frame        [in]  frame info
 * @param  option       [in]  interface control option
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  发送编码图像，支持阻塞或者非阻塞调用
 * @param  handle       [in]  编码句柄
 * @param  frame        [in]  图像信息
 * @param  option       [in]  控制选项
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_send_frame(td_handle handle, const ext_venc_frame *frame, const ext_venc_opt_send_frame *option);

/**
 * @if Eng
 * @brief  This interface is used to send user data.
 * @param  handle       [in]  the handle of encoder
 * @param  user_data    [in]  user data.
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  发送用户数据
 * @param  handle       [in]  编码句柄
 * @param  user_data    [in]  用户数据
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_send_userdata(td_handle handle, const ext_venc_userdata *user_data);

/**
 * @if Eng
 * @brief  This interface is used to get stream,supports blocking or non-blocking calls.
 * @param  handle       [in]  the handle of encoder
 * @param  stream       [out] stream info
 * @param  option       [in]  interface control option
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取编码码流，支持阻塞或者非阻塞调用
 * @param  handle       [in]  编码句柄
 * @param  stream       [out] 码流信息
 * @param  option       [in]  控制选项
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_get_stream(td_handle handle, ext_venc_stream *stream, const ext_venc_opt_get_stream *option);

/**
 * @if Eng
 * @brief  This interface is used to put stream.
 * @param  handle       [in]  the handle of encoder
 * @param  stream       [in]  stream info
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  释放编码码流
 * @param  handle       [in]  编码句柄
 * @param  stream       [in]  码流信息
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_put_stream(td_handle handle, const ext_venc_stream *stream);

/**
 * @if Eng
 * @brief  This interface is used to get channel attribute.
 * @param  handle       [in]  the handle of encoder
 * @param  attr         [out] venc attribute
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取编码属性
 * @param  handle       [in]  编码句柄
 * @param  attr         [out] 编码属性
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_get_attr(td_handle handle, ext_venc_attr *attr);

/**
 * @if Eng
 * @brief  This interface is used to set channel attribute.
 * @param  handle       [in]  the handle of encoder
 * @param  attr         [in]  channel attribute
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  设置编码属性
 * @param  handle       [in]  编码句柄
 * @param  attr         [in]  编码属性
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_set_attr(td_handle handle, const ext_venc_attr *attr);

/**
 * @if Eng
 * @brief  This interface is used to get rc paramter.
 * @param  handle       [in]  the handle of encoder
 * @param  param        [out] paramter
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取码控参数
 * @param  handle       [in]  编码句柄
 * @param  param        [out] 码控参数
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_get_rc_param(td_handle handle, ext_venc_rc_param *param);

/**
 * @if Eng
 * @brief  This interface is used to set rc paramter.
 * @param  handle       [in]  the handle of encoder
 * @param  param        [in]  paramter
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  设置码控参数
 * @param  handle       [in]  编码句柄
 * @param  param        [in]  码控参数
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_set_rc_param(td_handle handle, const ext_venc_rc_param *param);

/**
 * @if Eng
 * @brief  This interface is used to get bit rate overshoot strategy.
 * @param  handle       [in]  the handle of encoder
 * @param  strategy     [out] strategy
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取比特率过冲策略
 * @param  handle       [in]  编码句柄
 * @param  strategy     [out] 策略
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_get_bit_rate_overshoot(td_handle handle, ext_venc_bit_rate_overshoot *strategy);

/**
 * @if Eng
 * @brief  This interface is used to set bit rate overshoot strategy.
 * @param  handle       [in]  the handle of encoder
 * @param  strategy     [in]  strategy
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  设置比特率过冲策略
 * @param  handle       [in]  编码句柄
 * @param  strategy     [in]  策略
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_set_bit_rate_overshoot(td_handle handle, const ext_venc_bit_rate_overshoot *strategy);

/**
 * @if Eng
 * @brief  This interface is used to get frame bits overshoot strategy.
 * @param  handle       [in]  the handle of encoder
 * @param  strategy     [out] strategy
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取帧存比特过冲策略
 * @param  handle       [in]  编码句柄
 * @param  strategy     [out] 策略
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_get_frame_bits_overshoot(td_handle handle, ext_venc_frame_bits_overshoot *strategy);

/**
 * @if Eng
 * @brief  This interface is used to set frame bits overshoot strategy.
 * @param  handle       [in]  the handle of encoder
 * @param  strategy     [in]  strategy
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  设置帧存比特过冲策略
 * @param  handle       [in]  编码句柄
 * @param  strategy     [in]  策略
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_set_frame_bits_overshoot(td_handle handle, const ext_venc_frame_bits_overshoot *strategy);

/**
 * @if Eng
 * @brief  This interface is used to get jpeg ecs.
 * @param  handle       [in]  the handle of encoder
 * @param  ecs          [out] ecs info
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取jpeg ecs信息
 * @param  handle       [in]  编码句柄
 * @param  ecs          [out] ecs信息
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_get_jpeg_ecs(td_handle handle, ext_venc_jpeg_ecs *ecs);

/**
 * @if Eng
 * @brief  This interface is used to set jpeg ecs.
 * @param  handle       [in]  the handle of encoder
 * @param  ecs          [in]  ecs info
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  设置jpeg ecs信息
 * @param  handle       [in]  编码句柄
 * @param  ecs          [in]  ecs信息
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_set_jpeg_ecs(td_handle handle, const ext_venc_jpeg_ecs *ecs);

/**
 * @if Eng
 * @brief  This interface is used to get jpeg qt.
 * @param  handle       [in]  the handle of encoder
 * @param  qt           [out] quantization table
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取jpeg量化表
 * @param  handle       [in]  编码句柄
 * @param  qt           [out] 量化表
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_get_jpeg_qt(td_handle handle, ext_venc_jpeg_qt *qt);

/**
 * @if Eng
 * @brief  This interface is used to set jpeg qt.
 * @param  handle       [in]  the handle of encoder
 * @param  qt           [in]  quantization table
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  设置jpeg量化表
 * @param  handle       [in]  编码句柄
 * @param  qt           [in]  量化表
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_set_jpeg_qt(td_handle handle, const ext_venc_jpeg_qt *qt);

/**
 * @if Eng
 * @brief  This interface is used to get slice.
 * @param  handle       [in]  the handle of encoder
 * @param  slice        [out] slice info
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取slice信息
 * @param  handle       [in]  编码句柄
 * @param  slice        [out] slice信息
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_get_slice(td_handle handle, ext_venc_slice *slice);

/**
 * @if Eng
 * @brief  This interface is used to set slice.
 * @param  handle       [in]  the handle of encoder
 * @param  slice        [in]  slice info
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  设置slice信息
 * @param  handle       [in]  编码句柄
 * @param  slice        [in]  slice信息
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_set_slice(td_handle handle, const ext_venc_slice *slice);

/**
 * @if Eng
 * @brief  This interface is used to get roi parameter.
 * @param  handle       [in]  the handle of encoder
 * @param  param        [out] region of interest
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取感兴趣区域参数
 * @param  handle       [in]  编码句柄
 * @param  param        [out] 感兴趣区域参数
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_get_roi_param(td_handle handle, ext_venc_roi_param *param);

/**
 * @if Eng
 * @brief  This interface is used to get roi parameter.
 * @param  handle       [in]  the handle of encoder
 * @param  param        [in]  region of interest
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  设置感兴趣区域参数
 * @param  handle       [in]  编码句柄
 * @param  param        [in]  感兴趣区域参数
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_set_roi_param(td_handle handle, const ext_venc_roi_param *param);

/**
 * @if Eng
 * @brief  This interface is used to get roi rect.
 * @param  handle       [in]  the handle of encoder
 * @param  rect         [out] region of interest rectangle
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取感兴趣区域矩形
 * @param  handle       [in]  编码句柄
 * @param  rect         [out] 感兴趣区域矩形
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_get_roi_rect(td_handle handle, ext_venc_roi_rect *rect);

/**
 * @if Eng
 * @brief  This interface is used to set roi rect.
 * @param  handle       [in]  the handle of encoder
 * @param  rect         [in]  region of interest rectangle
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  设置感兴趣区域矩形
 * @param  handle       [in]  编码句柄
 * @param  rect         [in]  感兴趣区域矩形
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_set_roi_rect(td_handle handle, const ext_venc_roi_rect *rect);

/**
 * @if Eng
 * @brief  This interface is used to get skip ref.
 * @param  handle       [in]  the handle of encoder
 * @param  skip_ref     [out] skip ref
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取跳帧参考
 * @param  handle       [in]  编码句柄
 * @param  skip_ref     [out] 跳帧参考
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_get_skip_ref(td_handle handle, ext_venc_skip_ref *skip_ref);

/**
 * @if Eng
 * @brief  This interface is used to set skip ref.
 * @param  handle       [in]  the handle of encoder
 * @param  skip_ref     [in]  skip ref
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  设置跳帧参考
 * @param  handle       [in]  编码句柄
 * @param  skip_ref     [in]  跳帧参考
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_set_skip_ref(td_handle handle, const ext_venc_skip_ref *skip_ref);

/**
 * @if Eng
 * @brief  This interface is used to get intra refresh.
 * @param  handle         [in]  the handle of encoder
 * @param  intra_refresh  [out] intra refresh
 * @retval EXT_SUCCESS    Success.
 * @retval Other          Failure.
 * @else
 * @brief  获取帧内刷新
 * @param  handle         [in]  编码句柄
 * @param  intra_refresh  [out] 帧内刷新
 * @retval EXT_SUCCESS    成功
 * @retval Other          失败
 * @endif
 */
td_s32 uapi_venc_get_intra_refresh(td_handle handle, ext_venc_intra_refresh *intra_refresh);

/**
 * @if Eng
 * @brief  This interface is used to set intra refresh.
 * @param  handle         [in]  the handle of encoder
 * @param  intra_refresh  [in]  intra refresh
 * @retval EXT_SUCCESS    Success.
 * @retval Other          Failure.
 * @else
 * @brief  设置帧内刷新
 * @param  handle         [in]  编码句柄
 * @param  intra_refresh  [in]  帧内刷新
 * @retval EXT_SUCCESS    成功
 * @retval Other          失败
 * @endif
 */
td_s32 uapi_venc_set_intra_refresh(td_handle handle, const ext_venc_intra_refresh *intra_refresh);

/**
 * @if Eng
 * @brief  This interface is used to request idr.
 * @param  handle         [in]  the handle of encoder
 * @param  instant        [in]  instant
 * @retval EXT_SUCCESS    Success.
 * @retval Other          Failure.
 * @else
 * @brief  请求IDR帧
 * @param  handle         [in]  编码句柄
 * @param  instant        [in]  立即生效标志
 * @retval EXT_SUCCESS    成功
 * @retval Other          失败
 * @endif
 */
td_s32 uapi_venc_request_idr(td_handle handle, td_bool instant);

/**
 * @if Eng
 * @brief  This interface is used to request vi.
 * @param  handle         [in]  the handle of encoder
 * @retval EXT_SUCCESS    Success.
 * @retval Other          Failure.
 * @else
 * @brief  请求VI帧
 * @param  handle         [in]  编码句柄
 * @retval EXT_SUCCESS    成功
 * @retval Other          失败
 * @endif
 */
td_s32 uapi_venc_request_vi(td_handle handle);

/**
 * @if Eng
 * @brief  This interface is used to attach osd.
 * @param  handle       [in]  the handle of encoder
 * @param  osd_id       [in]  osd id
 * @param  osd          [in]  on-screen display
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  绑定osd
 * @param  handle       [in]  编码句柄
 * @param  osd_id       [in]  OSD标识
 * @param  osd          [in]  OSD
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_attach_osd(td_handle handle, td_s32 osd_id, const ext_venc_osd *osd);

/**
 * @if Eng
 * @brief  This interface is used to detach osd.
 * @param  handle       [in]  the handle of encoder
 * @param  osd_id       [in]  osd id
 * @param  option       [in]  interface control option
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  解绑osd，支持阻塞或者非阻塞调用
 * @param  handle       [in]  编码句柄
 * @param  osd_id       [in]  OSD标识
 * @param  option       [in]  控制选项
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_detach_osd(td_handle handle, td_s32 osd_id, const ext_venc_opt_detach_osd *option);

/**
 * @if Eng
 * @brief  This interface is used to refresh osd.
 * @param  handle       [in]  the handle of encoder
 * @param  osd_id       [in]  osd id
 * @param  osd          [in]  on-screen display
 * @param  option       [in]  interface control option
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  刷新osd
 * @param  handle       [in]  编码句柄
 * @param  osd_id       [in]  OSD标识
 * @param  osd          [in]  OSD
 * @param  option       [in]  控制选项
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_refresh_osd(td_handle handle, td_s32 osd_id, const ext_venc_osd *osd, const ext_venc_opt_refresh_osd *option);

/**
 * @if Eng
 * @brief  This interface is used to get osd attribute.
 * @param  handle       [in]  the handle of encoder
 * @param  osd_id       [in]  osd id
 * @param  attr         [out] osd attribute
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  获取编码OSD属性
 * @param  handle       [in]  编码句柄
 * @param  osd_id       [in]  OSD标识
 * @param  attr         [out] OSD属性
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_get_osd_attr(td_handle handle, td_s32 osd_id, ext_venc_osd_attr *attr);

/**
 * @if Eng
 * @brief  This interface is used to set osd attribute.
 * @param  handle       [in]  the handle of encoder
 * @param  osd_id       [in]  osd id
 * @param  attr         [in]  osd attribute
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  设置编码OSD属性
 * @param  handle       [in]  编码句柄
 * @param  osd_id       [in]  OSD标识
 * @param  attr         [in]  OSD属性
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_set_osd_attr(td_handle handle, td_s32 osd_id, const ext_venc_osd_attr *attr);

/**
 * @if Eng
 * @brief  This interface is used to enable osd.
 * @param  handle       [in]  the handle of encoder
 * @param  osd_id       [in]  osd id
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  使能OSD
 * @param  handle       [in]  编码句柄
 * @param  osd_id       [in]  OSD标识
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_enable_osd(td_handle handle, td_s32 osd_id);

/**
 * @if Eng
 * @brief  This interface is used to disable osd.
 * @param  handle       [in]  the handle of encoder
 * @param  osd_id       [in]  osd id
 * @retval EXT_SUCCESS  Success.
 * @retval Other        Failure.
 * @else
 * @brief  去使能OSD
 * @param  handle       [in]  编码句柄
 * @param  osd_id       [in]  OSD标识
 * @retval EXT_SUCCESS  成功
 * @retval Other        失败
 * @endif
 */
td_s32 uapi_venc_disable_osd(td_handle handle, td_s32 osd_id);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

