/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: user api for hearing aid
 * Author: @CompanyNameTag
 */

#ifndef __SOC_UAPI_HAID_H__
#define __SOC_UAPI_HAID_H__

#include <stdint.h>
#include "td_type.h"
#include "soc_uapi_ai.h"
#include "soc_uapi_sound.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @defgroup soc_uapi_haid soc_uapi_haid
 * @ingroup  audio
 * @{
 */

/**
 * @if Eng
 * @brief  HAID event type.
 * @else
 * @brief  HAID事件类型
 * @endif
 */
typedef enum {
    UAPI_HAID_EVENT_SCENE_CHANGE,
    UAPI_HAID_EVENT_TYPE_MAX,
} uapi_haid_event_type;

/**
 * @if Eng
 * @brief  HAID event callback function.
 * @param  [in]  haid handle
 * @param  [in]  haid event
 * @param  [in]  param
 * @param  [in]  context
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @param  [in]  haid 句柄
 * @param  [in]  haid 事件
 * @param  [in]  参数
 * @param  [in]  上下文
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @brief  HAID事件回调函数
 * @endif
 */
typedef int32_t (*uapi_haid_event_proc)(uint32_t haid, uapi_haid_event_type event, void *param, void *context);

/**
 * @if Eng
 * @brief  HAID scene.
 * @else
 * @brief  HAID场景
 * @endif
 */
typedef enum {
    UAPI_HAID_SCENE_AUTO = 0,           /*!< @if Eng Auto detect scene
                                             @else   自动识别场景 @endif */
    UAPI_HAID_SCENE_TRANSPORTATION,     /*!< @if Eng Transportation
                                             @else   交通 @endif */
    UAPI_HAID_SCENE_SOCIALIZING,        /*!< @if Eng Socializing
                                             @else   社交 @endif */
    UAPI_HAID_SCENE_PARTY,              /*!< @if Eng Party
                                             @else   聚会 @endif */
    UAPI_HAID_SCENE_QUIET,              /*!< @if Eng Quiet
                                             @else   安静 @endif */
    UAPI_HAID_SCENE_EXT0,               /*!< @if Eng EXT0
                                             @else   扩展0 @endif */
    UAPI_HAID_SCENE_EXT1,               /*!< @if Eng EXT1
                                             @else   扩展1 @endif */
    UAPI_HAID_SCENE_TEST,               /*!< @if Eng Test
                                             @else   测试 @endif */

    UAPI_HAID_SCENE_MAX,
} uapi_haid_scene;

/**
 * @if Eng
 * @brief  codec voltage parameter.
 * @else
 * @brief  codec电压参数
 * @endif
 */
typedef enum {
    UAPI_HAID_VOLTAGE_1P2 = 0x2,  /*!< @if Eng voltage level 2, correspond to voltage 1.2V
                                       @else   电压档位2, 对应电压值1.2V @endif */
    UAPI_HAID_VOLTAGE_1P3 = 0x4,  /*!< @if Eng voltage level 4, correspond to voltage 1.3V
                                       @else   电压档位4, 对应电压值1.3V @endif */
    UAPI_HAID_VOLTAGE_1P4 = 0x6,  /*!< @if Eng voltage level 6, correspond to voltage 1.4V
                                       @else   电压档位6, 对应电压值1.4V @endif */
    UAPI_HAID_VOLTAGE_1P5 = 0x8,  /*!< @if Eng voltage level 8, correspond to voltage 1.5V
                                       @else   电压档位8, 对应电压值1.5V @endif */
    UAPI_HAID_VOLTAGE_1P6 = 0xa,  /*!< @if Eng voltage level 10, correspond to voltage 1.6V
                                       @else   电压档位10, 对应电压值1.6V @endif */
    UAPI_HAID_VOLTAGE_MAX = 0x10,  /*!< @if Eng maximum voltage level
                                        @else   最大电压档位 @endif */
} uapi_haid_voltage;

/**
 * @if Eng
 * @brief  Defines the haid fifo selection.
 * @else
 * @brief  助听fifo选择
 * @endif
 */
typedef enum {
    UAPI_HAID_UP_FIFO_0 = 0,
    UAPI_HAID_UP_FIFO_1,
    UAPI_HAID_DN_FIFO,

    UAPI_HAID_FIFO_SEL_MAX,
} uapi_haid_fifo_sel;

/**
 * @if Eng
 * @brief  Struct of HAID attributes.
 * @else
 * @brief  助听通路属性结构体
 * @endif
 */
typedef struct {
    uapi_ai_port ai_port; /*!< @if Eng Audio Input Port
                               @else   音频输入硬件端口 @endif */
    uapi_snd_out_port ao_port;      /*!< @if Eng Audio Output Port
                                         @else   音频输出硬件端口 @endif */
    uapi_audio_pcm_format in_pcm;   /*!< @if Eng Input PCM format
                                         @else   输入PCM格式 @endif */
    uapi_audio_pcm_format out_pcm;  /*!< @if Eng Output PCM format
                                         @else   输出PCM格式 @endif */
    uapi_haid_voltage haid_voltage;  /*!< @if Eng codec voltage setting value
                                          @else   codec电压设定值 @endif */
    uapi_ai_amic_sel mic_sel[UAPI_AI_MIC_NUM_MAX]; /*!< @if Eng microphone used by hearing aid.
                                                        @else   助听使用的麦克风 @endif */
} uapi_haid_attr;

/**
 * @if Eng
 * @brief  haid volume type
 * @else
 * @brief  助听音量类型
 * @endif
 */
typedef enum {
    UAPI_HAID_POST_GAIN,  /*!< @if Eng Algorithm post Volume.
                               @else   算法后置音量       @endif */

    UAPI_HAID_PRE_GAIN,   /*!< @if Eng Algorithm pre volume.
                               @else   算法前置音量       @endif */

    UAPI_HAID_WDRC_GAIN,  /*!< @if Eng WDRC algorithm volume.
                               @else   WDRC算法音量     @endif */

    UAPI_HAID_GAIN_TYPE_MAX,
} uapi_haid_volume_type;

/**
 * @if Eng
 * @brief  High Precision Gain
 * @else
 * @brief  音频高精度增益
 * @endif
 */
typedef struct {
    int32_t integer; /*!< @if Eng Interger part of high preicision gain.
                         @else   高精度增益的整数部分 @endif */

    int32_t decimal; /*!< @if Eng Decimal part of high preicision gain, if -0.125dB, value is -125.
                         @else   高精度增益的小数部分，如果是-0.125dB，值为-125 @endif */

    uapi_haid_volume_type volume_type; /*!< @if Eng Haid volume type
                                            @else   助听音量类型 @endif */
} uapi_haid_volume;

/**
 * @if Eng
 * @brief  hearing aid microphone calibration volume
 * @else
 * @brief  助听麦克风的校准音量
 * @endif
 */
typedef struct {
    uint32_t mic_num;  /*!< @if Eng microphone num used by hearing aid.
                          @else   助听使用的麦克风数量 @endif */
    uapi_haid_volume volume[UAPI_AI_MIC_NUM_MAX]; /*!< @if Eng microphone calibration volume
                                                       @else   每个麦克风的音量校准值 @endif */
} uapi_haid_cali_volume;

/**
 * @if Eng
 * @brief  hearing aid ioctl
 * @else
 * @brief  助听输入输出
 * @endif
 */
typedef struct {
    int8_t *payload; /*!< @if Eng ioctl payload
                         @else   输入输出的载体 @endif */
    uint32_t length;  /*!< @if Eng Audio Input Port
                         @else   输入输出的长度 @endif */
} uapi_haid_ioctl_param;

/**
 * @if Eng
 * @brief  HAID module init.
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  HAID模块初始化
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_init(void);

/**
 * @if Eng
 * @brief  HAID module deinit.
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  HAID模块去初始化
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_deinit(void);

/**
 * @if Eng
 * @brief  Get the default attributes of HAID.
 * @param  [out] haid_attr HAID instance attribute
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  获取助听通路默认配置
 * @param  [out] haid_attr HAID实例属性
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_get_default_attr(uapi_haid_attr *attr);

/**
 * @if Eng
 * @brief  Create a instance for HAID.
 * @param  [in]  haid handle
 * @param  [in]  haid HAID instance attribute
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  创建助听实例
 * @param  [in]  haid 句柄
 * @param  [in]  haid HAID实例属性
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_create(uint32_t *haid, const uapi_haid_attr *attr);

/**
 * @if Eng
 * @brief  Destroy HAID instance.
 * @param  [in]  haid HAID handle
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  销毁助听实例
 * @param  [in]  haid HAID句柄
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_destroy(uint32_t haid);

/**
 * @if Eng
 * @brief  Set enable of HAID.
 * @param  [in]  HAID handle
 * @param  [in]  HAID enable
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  配置HAID使能状态
 * @param  [in]  HAID句柄
 * @param  [in]  HAID使能
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_set_enable(uint32_t haid, uint8_t enable);

/**
 * @if Eng
 * @brief  Get enable of HAID.
 * @param  [in]  HAID handle
 * @param  [out] HAID enable
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  获取HAID使能状态
 * @param  [in]  HAID句柄
 * @param  [out] HAID使能
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_get_enable(uint32_t haid, uint8_t *enable);

/**
 * @if Eng
 * @brief  Register callback to process HAID events.
 * @param  [in]  haid    HAID handle
 * @param  [in]  proc    User callback to process HAID events
 * @param  [in]  context Data passthrough to callback
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  注册处理HAID事件的回调函数
 * @param  [in]  haid    HAID句柄
 * @param  [in]  proc    用户的HAID事件处理回调函数
 * @param  [in]  context 透传给回调函数的数据
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_register_event_proc(uint32_t haid, uapi_haid_event_proc proc, void *context);

/**
 * @if Eng
 * @brief  Set volume value of HAID.
 * @param  [in]  haid handle
 * @param  [in]  HAID volume value
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  配置HAID音量
 * @param  [in]  HAID句柄
 * @param  [in]  HAID音量
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_set_volume(uint32_t haid, const uapi_haid_volume *volume);

/**
 * @if Eng
 * @brief  Get volume value of HAID.
 * @param  [in]  haid handle
 * @param  [out] HAID volume value
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  获取HAID音量
 * @param  [in]  HAID句柄
 * @param  [out] HAID音量
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_get_volume(uint32_t haid, uapi_haid_volume *volume);

/**
 * @if Eng
 * @brief  Set the config of HAID.
 * @param  [in]  HAID handle
 * @param  [in]  HAID config
 * @param  [in]  HAID config length
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  设置HAID实例配置
 * @param  [in]  HAID句柄
 * @param  [in]  HAID配置
 * @param  [in]  HAID配置长度
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_set_config(uint32_t haid, const int8_t *config, uint32_t config_len);

/**
 * @if Eng
 * @brief  Get the config of HAID.
 * @param  [in]  HAID handle
 * @param  [out] HAID config
 * @param  [out] HAID config length
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  获取HAID实例配置
 * @param  [in]  HAID句柄
 * @param  [out] HAID配置
 * @param  [out] HAID配置长度
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_get_config(uint32_t haid, int8_t *config, uint32_t *config_len);

/**
 * @if Eng
 * @brief  Set the hardware_config of HAID.
 * @param  [in]  HAID handle
 * @param  [in]  HAID hardware_config
 * @param  [in]  HAID hardware_config length
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  设置HAID硬件配置
 * @param  [in]  HAID句柄
 * @param  [in]  HAID硬件配置
 * @param  [in]  HAID硬件配置长度
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_set_hardware_config(uint32_t haid, const int8_t *config, uint32_t config_len);

/**
 * @if Eng
 * @brief  Get the hardware config of HAID.
 * @param  [in]  HAID handle
 * @param  [out] HAID hardware config
 * @param  [out] HAID hardware config length
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  获取HAID硬件配置
 * @param  [in]  HAID句柄
 * @param  [out] HAID硬件配置
 * @param  [out] HAID硬件配置长度
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_get_hardware_config(uint32_t haid, int8_t *config, uint32_t *config_len);

/**
 * @if Eng
 * @brief  Set the test_config of HAID and set scene 'TEST'.
 * @param  [in]  HAID handle
 * @param  [in]  HAID test_config
 * @param  [in]  HAID test_config length
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  设置HAID测试场景参数配置并使能测试场景的参数
 * @param  [in]  HAID句柄
 * @param  [in]  HAID测试场景的配置
 * @param  [in]  HAID测试场景的配置的长度
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_set_test_config(uint32_t haid, const int8_t *config, uint32_t config_len);

/**
 * @if Eng
 * @brief  Save all configs of HAID.
 * @param  [in]  HAID handle
 * @param  [in]  HAID all configs
 * @param  [in]  HAID all configs length
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  保存HAID实例配置
 * @param  [in]  HAID句柄
 * @param  [in]  HAID全套配置
 * @param  [in]  HAID全套配置的长度
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_set_all_config(uint32_t haid, const int8_t *config, uint32_t config_len);

/**
 * @if Eng
 * @brief  Get all the configs of HAID.
 * @param  [in]  HAID handle
 * @param  [out] HAID all configs
 * @param  [out] HAID all configs length
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  获取HAID实例全套配置
 * @param  [in]  HAID句柄
 * @param  [out] HAID全套配置
 * @param  [out] HAID全套配置的长度
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_get_all_config(uint32_t haid, int8_t *config, uint32_t *config_len);

/**
 * @if Eng
 * @brief  Start playing sinus tone audio.
 * @param  [in]  HAID handle
 * @param  [in]  HAID frequence
 * @param  [in]  HAID volume
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  开始播放单频音
 * @param  [in]  HAID句柄
 * @param  [in]  HAID单频音频率
 * @param  [in]  HAID音量
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_sinus_tone_start(uint32_t haid, uint32_t frequence, const uapi_haid_volume *volume);

/**
 * @if Eng
 * @brief  Stop playing sinus tone audio.
 * @param  [in]  HAID handle
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  停止播放单频音
 * @param  [in]  HAID句柄
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_sinus_tone_stop(uint32_t haid);

/**
 * @if Eng
 * @brief  Start calibrating the AFC coefficient.
 * @param  [in]  HAID handle
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  开始校准AFC系数
 * @param  [in]  HAID句柄
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_calibration_start(uint32_t haid);

/**
 * @if Eng
 * @brief  Stop calibrating the afc coefficient.
 * @param  [in]  HAID handle
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  停止校准AFC系数
 * @param  [in]  HAID句柄
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_calibration_stop(uint32_t haid);

/**
 * @if Eng
 * @brief  Set scene of HAID.
 * @param  [in]  haid handle
 * @param  [in]  HAID scene
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  配置HAID场景
 * @param  [in]  HAID句柄
 * @param  [in]  HAID场景
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_set_scene(uint32_t haid, uapi_haid_scene scene);

/**
 * @if Eng
 * @brief  Get scene of HAID.
 * @param  [in]  haid handle
 * @param  [out] HAID scene
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  获取HAID场景
 * @param  [in]  HAID句柄
 * @param  [out] HAID场景
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_get_scene(uint32_t haid, uapi_haid_scene *scene);

/**
 * @if Eng
 * @brief  Set speaker calibration biquad filters.
 * @param  [in]  Haid handle
 * @param  [in]  Calib filters parameters, e.g. freqs, Qs, gains, filters are 8 bands
 * @param  [in]  Calib filters parameters' length in byte，its value is 48
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  设置喇叭输出校准滤波器参数
 * @param  [in]  HAID句柄
 * @param  [in]  校准滤波器参数, 包括中心频率、品质因数、增益等，滤波器共8段
 * @param  [in]  滤波器参数长度字节数，值为48
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_set_spk_cali_coef(uint32_t haid, const uint8_t *filter_coefs, uint32_t coef_len);

/**
 * @if Eng
 * @brief  Set haid hearing microphone calibration volume.
 * @param  [in]  haid hearing aid handle
 * @param  [in]  cali_volume hearing aid microphone calibration volume
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  设置麦克风校准音量
 * @param  [in]  haid 助听句柄
 * @param  [in]  cali_volume 助听麦克风的校准音量
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_set_mic_cali_volume(uint32_t haid, const uapi_haid_cali_volume *cali_volume);

/**
 * @if Eng
 * @brief  Get haid environment noise.
 * @param  [in]  haid hearing aid handle
 * @param  [out] env noise
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  获取助听环境噪声
 * @param  [in]  HAID句柄
 * @param  [out] 环境噪声
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_get_env_noise(uint32_t haid, int32_t *noise);

/**
 * @if Eng
 * @brief  Start get haid environment noise.
 * @param  [in]  haid hearing aid handle
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  开始获取助听环境噪声
 * @param  [in]  HAID句柄
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_get_env_noise_start(uint32_t haid);

/**
 * @if Eng
 * @brief  Stop get haid environment noise.
 * @param  [in]  haid hearing aid handle
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  停止获取助听环境噪声
 * @param  [in]  HAID句柄
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_get_env_noise_stop(uint32_t haid);

/**
 * @if Eng
 * @brief  Start haid fifo.
 * @param  [in]  haid hearing aid handle
 * @param  [in]  haid fifo selection
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  开启助听fifo
 * @param  [in]  HAID句柄
 * @param  [in]  HAID fifo选择
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_fifo_start(uint32_t haid, uapi_haid_fifo_sel fifo_sel);

/**
 * @if Eng
 * @brief  Stop haid fifo.
 * @param  [in]  haid hearing aid handle
 * @param  [in]  haid fifo selection
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  停止助听fifo
 * @param  [in]  HAID句柄
 * @param  [in]  HAID fifo选择
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_fifo_stop(uint32_t haid, uapi_haid_fifo_sel fifo_sel);

/**
 * @if Eng
 * @brief  Hearing aid ioctl.
 * @param  [in]  Haid handle
 * @param  [out] Haid ioctl param
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  助听输入输出
 * @param  [in]  haid 助听句柄
 * @param  [out] haid 助听输入输出的参数
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_haid_ioctl(uint32_t haid, uapi_haid_ioctl_param *ioctl_param);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SOC_UAPI_HAID_H__ */
