/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved. \n
 * Description: user header file for audio decoder \n
 * Author: @CompanyNameTag \n
 */

#ifndef __SOC_UAPI_ADEC_H__
#define __SOC_UAPI_ADEC_H__

#include "soc_uapi_acodec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @defgroup soc_uapi_adec soc_uapi_adec
 * @ingroup  audio
 * @{
 */

/**
 * @if Eng
 * @brief  ADEC event type.
 * @else
 * @brief  音频解码事件类型
 * @endif
 */
typedef enum {
    UAPI_ADEC_EVENT_ERROR_FRAME,
    UAPI_ADEC_EVENT_UNSUPPORT_STREAM,
    UAPI_ADEC_EVENT_TYPE_MAX,
} uapi_adec_event_type;

/**
 * @if Eng
 * @brief  ADEC param type.
 * @else
 * @brief  音频解码参数类型
 * @endif
 */
typedef enum {
    UAPI_ADEC_PARAM_PROFILE_CONSTRAINT_FLAG, /*!< @if Eng Profile Constraint Flag. \n
                                              * FALSE: support AAC MPEG2 decoding and AAC MPEG4 decoding
                                              * TRUE: only support AAC MERG2 decodeding (DEFAULT)
                                                  @else   约束标志. \n
                                              * FALSE: 支持AAC MPEG2解码和AAC MPEG4解码
                                              * TRUE: 仅支持AAC MPEG2解码 (默认) @endif */
} uapi_adec_param_type;

/**
 * @if Eng
 * @brief  Defines the attributes of the audio decoder.
 * @else
 * @brief  音频解码的属性
 * @endif
 */
typedef struct {
    uapi_acodec_id codec_id;     /*!< @if Eng Audio Codec id.
                                      @else   Audio Codec 序号 @endif */
    uint8_t frame_mode_input;    /*!< @if Eng Frame mode input. \n
                                  * Closed by default, decoder need input data by frame when opened.
                                      @else   帧输入模式.\n
                                  * 默认关闭，开启时解码器需要按帧输入数据. @endif */
    uint32_t buf_dur_ms;         /*!< @if Eng Buffer duration, unit: ms.
                                      @else   缓存Buffer的长度，以毫秒为单位 @endif */
    uapi_acodec_dec_param param; /*!< @if Eng Audio decoder parameter.
                                      @else   音频解码配置参数 @endif */
    uint32_t max_mute_dur_s;           /*!< @if Eng maximum mute frame count, unit: s.
                                      @else   最大静音帧播放时间，以秒为单位 @endif */
} uapi_adec_attr;

/**
 * @if Eng
 * @brief  ADEC event callback function.
 * @else
 * @brief  音频解码的事件回调函数
 * @endif
 */
typedef int32_t (*uapi_adec_event_proc)(uint32_t adec, uapi_adec_event_type event, void *param,
    void *context);

/**
 * @if Eng
 * @brief  Initialize Audio Decoder module.
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  ADEC初始化
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_adec_init(void);

/**
 * @if Eng
 * @brief  Deinitialize Audio Decoder module.
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  ADEC去初始化
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_adec_deinit(void);

/**
 * @if Eng
 * @brief  Create an Audio Decoder instance.
 * @param  [out] adec ADEC handle pointer
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  创建Audio Decoder实例
 * @param  [out] adec ADEC句柄指针
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_adec_create(uint32_t *adec);

/**
 * @if Eng
 * @brief  Destroy an Audio Decoder instance.
 * @param  [in]  adec ADEC handle
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  销毁Audio Decoder实例
 * @param  [in]  adec ADEC句柄
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_adec_destroy(uint32_t adec);

/**
 * @if Eng
 * @brief  Register callback to process ADEC events.
 * @param  [in]  adec ADEC handle
 * @param  [in]  event_proc User callback to process ADEC events
 * @param  [in]  context Data passthrough to callback
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  注册ADEC事件处理回调函数
 * @param  [in]  adec ADEC句柄
 * @param  [in]  event_proc 用户的ADEC事件处理回调函数
 * @param  [in]  context 透传给回调函数的数据
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_adec_register_event_proc(uint32_t adec, uapi_adec_event_proc event_proc, void *context);

/**
 * @if Eng
 * @brief  Start an ADEC instance.
 * @param  [in]  adec ADEC handle
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  启动ADEC实例开始解码
 * @param  [in]  adec ADEC句柄
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_adec_start(uint32_t adec);

/**
 * @if Eng
 * @brief  Stop an ADEC instance.
 * @param  [in]  adec ADEC handle
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  停止ADEC实例
 * @param  [in]  adec ADEC句柄
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_adec_stop(uint32_t adec);

/**
 * @if Eng
 * @brief  Get the attribute of ADEC instance.
 * @param  [in]  adec ADEC handle
 * @param  [out] attr ADEC instance attribute
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  获取ADEC实例属性
 * @param  [in]  adec ADEC句柄
 * @param  [out] attr ADEC实例属性
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_adec_get_attr(uint32_t adec, uapi_adec_attr *attr);

/**
 * @if Eng
 * @brief  Set the attribute of ADEC instance.
 * @param  [in]  adec ADEC handle
 * @param  [in]  attr ADEC instance attribute
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  设置ADEC实例属性
 * @param  [in]  adec ADEC句柄
 * @param  [in]  attr ADEC实例属性
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_adec_set_attr(uint32_t adec, const uapi_adec_attr *attr);

/**
 * @if Eng
 * @brief  Attach a output instance to ADEC instance.
 * @param  [in]  adec ADEC handle
 * @param  [in]  output Output instance handle
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  绑定输出实例到ADEC
 * @param  [in]  adec ADEC句柄
 * @param  [in]  output 输出实例句柄
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_adec_attach_output(uint32_t adec, uint32_t output);

/**
 * @if Eng
 * @brief  Detach a output instance from ADEC instance.
 * @param  [in]  adec ADEC handle
 * @param  [in]  output Output instance handle
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  解绑输出实例
 * @param  [in]  adec ADEC句柄
 * @param  [in]  output 输出实例句柄
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_adec_detach_output(uint32_t adec, uint32_t output);

/**
 * @if Eng
 * @brief  Get the play stream size of ADEC instance.
 * @param  [in]  adec ADEC handle
 * @param  [out] pss ADEC instance play stream size
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  获取ADEC解码播放的es流数据大小
 * @param  [in]  adec ADEC句柄
 * @param  [out] pss ADEC解码播放的es流数据大小
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_adec_get_pss(uint32_t adec, uint32_t *pss);

/**
 * @if Eng
 * @brief  Set the play stream offset of ADEC instance before start.
 * @param  [in]  adec ADEC handle
 * @param  [in]  pss ADEC instance play stream offset before start
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  设置ADEC解码开始播放的es流数据偏移
 * @param  [in]  adec ADEC句柄
 * @param  [in]  pss ADEC解码启动播放的es流数据偏移
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_adec_set_pss(uint32_t adec, uint32_t pss);

/**
 * @if Eng
 * @brief  Set ADEC parameters.
 * @param  [in]  adec       ADEC handle
 * @param  [in]  type       ADEC parameter type
 * @param  [in]  param      ADEC parameter
 * @param  [in]  param_size parameter size
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  设置ADEC参数
 * @param  [in]  adec       ADEC句柄
 * @param  [in]  type       ADEC参数类型
 * @param  [in]  param      ADEC参数
 * @param  [in]  param_size 参数大小
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_adec_set_param(uint32_t adec, uapi_adec_param_type type, const void *param, uint32_t param_size);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SOC_UAPI_ADEC_H__ */
