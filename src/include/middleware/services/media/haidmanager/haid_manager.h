/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Haid manager interface
 * Author: Media Software Group
 * Create: 2023-02-026
 */

#ifndef HAID_MANAGER_H
#define HAID_MANAGER_H
#include <stdint.h>
#include <stdbool.h>
#include "audio_base_type.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define ANC_REG_SIZE 1024
#define MIC_NUM_MAX 2

/**
 * @if Eng
 * @brief  HAID event type.
 * @else
 * @brief  HAID事件类型
 * @endif
 */
typedef enum {
    HAID_EVENT_SCENE_CHANGE,
    HAID_EVENT_TYPE_MAX = 0xFFFFFFFF,
} HaidEventType;

typedef enum {
    HAID_ANC_CHANNEL_FF0 = 0, /*!< @if Eng ff0 channel in ANC
                                   @else   ANC模块的ff0通路 @endif */

    HAID_ANC_CHANNEL_FF1 = 1, /*!< @if Eng ff1 channel in ANC
                                   @else   ANC模块的ff1通路 @endif */

    HAID_ANC_CHANNEL_FB = 2,  /*!< @if Eng fb channel in ANC
                                   @else   ANC模块的fb通路 @endif */

    HAID_ANC_CHANNEL_SPE = 3,  /*!< @if Eng spe channel in ANC
                                    @else   ANC模块的spe通路 @endif */

    HAID_ANC_CHANNEL_POST_PROC0 = 4, /*!< @if Eng post-proc0 channel in ANC
                                          @else   ANC模块的post-proc0通路 @endif */

    HAID_ANC_CHANNEL_POST_PROC1 = 5, /*!< @if Eng post-proc1 channel in ANC
                                          @else   ANC模块的post-proc1通路 @endif */

    HAID_ANC_CHANNEL_TYPE_MAX,
    HAID_ANC_CHANNEL_TYPE_BUTT = 0x7FFFFFFFu,
} HaidAncChannelType;


/**
 * @if Eng
 * @brief  HAID event callback function.
 * @param  [in]  haid event
 * @param  [in]  param
 * @param  [in]  context
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  HAID事件回调函数
 * @param  [in]  haid 事件
 * @param  [in]  param 参数
 * @param  [in]  context 上下文
 * @return 返回0成功，其他值失败
 * @endif
 */
typedef int32_t (*HaidEventCallback)(HaidEventType event, void *param, void *context);

/**
 * @if Eng
 * @brief  HAID scene.
 * @else
 * @brief  HAID场景
 * @endif
 */
typedef enum {
    HAID_SCENE_AUTO = 0,           /*!< @if Eng Auto detect scene
                                             @else   自动识别场景 @endif */
    HAID_SCENE_TRANSPORTATION,     /*!< @if Eng Transportation
                                             @else   交通 @endif */
    HAID_SCENE_SOCIALIZING,        /*!< @if Eng Socializing
                                             @else   社交 @endif */
    HAID_SCENE_PARTY,              /*!< @if Eng Party
                                             @else   聚会 @endif */
    HAID_SCENE_QUIET,              /*!< @if Eng Quiet
                                             @else   安静 @endif */

    HAID_SCENE_MAX = 0xFFFFFFFF,
} HaidScene;

/**
 * @if Eng
 * @brief  haid volume type
 * @else
 * @brief  助听音量类型
 * @endif
 */
typedef enum {
    HAID_POST_GAIN,  /*!< @if Eng Algorithm post Volume.
                               @else   算法后置音量       @endif */
 
    HAID_PRE_GAIN,   /*!< @if Eng Algorithm pre volume.
                               @else   算法前置音量       @endif */
 
    HAID_WDRC_GAIN,  /*!< @if Eng WDRC algorithm volume.
                               @else   WDRC算法音量     @endif */
 
    HAID_GAIN_TYPE_MAX = 0xFFFFFFFF,
} HaidVolumeType;

/**
 * @if Eng
 * @brief  High Precision Gain
 * @else
 * @brief  音频输出增益
 * @endif
 */
typedef struct {
    int32_t integer; /*!< @if Eng Interger part of high preicision gain.
                         @else   高精度增益的整数部分 @endif */

    int32_t decimal; /*!< @if Eng Decimal part of high preicision gain, if -0.125dB, value is -125.
                         @else   高精度增益的小数部分，如果是-0.125dB，值为-125 @endif */
    HaidVolumeType volumeType;
} HaidVolume;

typedef struct {
    uint32_t usedLength;  /*!< @if Eng microphone num used by audio input. */
    HaidVolume micVolume[MIC_NUM_MAX];         /*!< @if Eng microphone calibration volume */
} HaidMicCaliVolume;

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
} HaidIoctlParam;

typedef enum {
    HAID_VOLTAGE_1P2 = 0x2,  /*!< @if Eng voltage level 2, correspond to voltage 1.2V
                                       @else   电压档位2, 对应电压值1.2V @endif */
    HAID_VOLTAGE_1P3 = 0x4,  /*!< @if Eng voltage level 4, correspond to voltage 1.3V
                                       @else   电压档位4, 对应电压值1.3V @endif */
    HAID_VOLTAGE_1P4 = 0x6,  /*!< @if Eng voltage level 6, correspond to voltage 1.4V
                                       @else   电压档位6, 对应电压值1.4V @endif */
    HAID_VOLTAGE_1P5 = 0x8,  /*!< @if Eng voltage level 8, correspond to voltage 1.5V
                                       @else   电压档位8, 对应电压值1.5V @endif */
    HAID_VOLTAGE_1P6 = 0xa,  /*!< @if Eng voltage level 10, correspond to voltage 1.6V
                                       @else   电压档位10, 对应电压值1.6V @endif */
    HAID_VOLTAGE_MAX = 0x10,  /*!< @if Eng maximum voltage level
                                        @else   最大电压档位 @endif */
    HAID_VOLTAGE_INVALID = 0xFFFFFFFF,
} HaidVoltage;

typedef struct {
    uint16_t usedLength;
    CapturerMicType micType[MIC_NUM_MAX];
    HaidVoltage voltage;
} HaidAttr;

/**
 * @if Eng
 * @brief  Create channel for HAID.
 * @param  [in]  attr HAID instance attribute
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  创建助听通路
 * @param  [in]  attr HAID实例属性
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidCreate(const HaidAttr *attr);
/**
 * @if Eng
 * @brief  Destroy HAID channel.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  销毁助听通路
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidDestroy(void);

/**
 * @if Eng
 * @brief  Set enable of HAID.
 * @param  [in]  enable HAID enable
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  配置HAID使能状态
 * @param  [in]  enable HAID使能
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidSetEnable(bool enable);

/**
 * @if Eng
 * @brief  Get enable of HAID.
 * @param  [out]  enable HAID enable
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  获取HAID使能状态
 * @param  [out]  enable HAID使能
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidGetEnable(bool *enable);

/**
 * @if Eng
 * @brief  Register callback to process HAID events.
 * @param  [in]  cb User callback to process HAID events
 * @param  [in]  context Data passthrough to callback
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  注册处理HAID事件的回调函数
 * @param  [in]  cb 用户的HAID事件处理回调函数
 * @param  [in]  context 透传给回调函数的数据
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidSetEventCallback(HaidEventCallback cb, void *context);

/**
 * @if Eng
 * @brief  Set volume value of HAID.
 * @param  [in]  volume HAID volume value
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  配置HAID音量，验配或助听过程可用
 * @param  [in]  volume HAID音量
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidSetVolume(HaidVolume volume);

/**
 * @if Eng
 * @brief  Get volume value of HAID.
 * @param  [out]  volume HAID volume value
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  获取HAID音量，验配或助听过程可用
 * @param  [out]  volume HAID音量
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidGetVolume(HaidVolume *volume);

/**
 * @if Eng
 * @brief  Set the hardware_config of HAID.
 * @param  [in]  config HAID hardware config
 * @param  [in]  configLen HAID hardware config length
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  设置HAID硬件配置
 * @param  [in]  config HAID硬件配置
 * @param  [in]  configLen HAID硬件配置长度
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidSetHardwareConfig(const int8_t *config, uint32_t configLen);

/**
 * @if Eng
 * @brief  Get the hardware config of HAID.
 * @param  [out]  config HAID hardware config
 * @param  [out]  configLen HAID hardware config length
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  获取HAID硬件配置
 * @param  [out]  config HAID硬件配置
 * @param  [out]  configLen HAID硬件配置长度
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidGetHardwareConfig(int8_t *config, uint32_t *configLen);

/**
 * @if Eng
 * @brief  Start calibrating the AFC coefficient.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  开始校准AFC系数
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidCalibrationStart(void);

/**
 * @if Eng
 * @brief  Stop calibrating the afc coefficient.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  停止校准AFC系数
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidCalibrationStop(void);

/**
 * @if Eng
 * @brief  Set the test_config of HAID and set scene 'TEST'.
 * @param  [in]  config HAID test_config
 * @param  [in]  configLen HAID test_config length
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  设置HAID测试场景参数配置并使能测试场景的参数
 * @param  [in]  config HAID测试场景的配置
 * @param  [in]  configLen HAID测试场景的配置的长度
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidSetTestConfig(const int8_t *config, uint32_t configLen);

/**
 * @if Eng
 * @brief  Hearing aid ioctl.
 * @param  [out]  ioctlParam ioctl param
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  助听输入输出
 * @param  [out]  ioctlParam 助听输入输出的参数
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidIoctl(HaidIoctlParam *ioctlParam);

/**
 * @if Eng
 * @brief  Set the config of HAID.
 * @param  [in]  config HAID config
 * @param  [in]  configLen HAID config length
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  设置HAID实例配置
 * @param  [in]  config HAID配置
 * @param  [in]  configLen HAID配置长度
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidSetConfig(const int8_t *config, uint32_t configLen);

/**
 * @if Eng
 * @brief  Get the config of HAID.
 * @param  [out]  config HAID config
 * @param  [out]  configLen HAID config length
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  获取HAID实例配置
 * @param  [out]  config HAID配置
 * @param  [out]  configLen HAID配置长度
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidGetConfig(int8_t *config, uint32_t *configLen);

/**
 * @if Eng
 * @brief  Save all configs of HAID.
 * @param  [in]  config HAID all configs
 * @param  [in]  configLen HAID all configs length
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  保存HAID实例配置
 * @param  [in]  config HAID全套配置
 * @param  [in]  configLen HAID全套配置的长度
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidSetAllConfig(const int8_t *config, uint32_t configLen);

/**
 * @if Eng
 * @brief  Get all the configs of HAID.
 * @param  [out]  config HAID all configs
 * @param  [out]  configLen HAID all configs length
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  获取HAID实例全套配置
 * @param  [out]  config HAID全套配置
 * @param  [out]  configLen HAID全套配置的长度
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidGetAllConfig(int8_t *config, uint32_t *configLen);

/**
 * @if Eng
 * @brief  Start playing sinus tone audio.
 * @param  [in]  frequence HAID frequence,[20, 20k]
 * @param  [in]  volume HAID volume
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  开始播放单频音
 * @param  [in]  frequence HAID单频音频率，[20, 20k]
 * @param  [in]  volume HAID音量
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidTonePlayForExamine(uint32_t frequence, const HaidVolume volume);

/**
 * @if Eng
 * @brief  Stop playing sinus tone audio.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  停止播放单频音
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidToneStopForExamine(void);

/**
 * @if Eng
 * @brief  Set scene of HAID.
 * @param  [in]  scene HAID scene
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  配置HAID场景
 * @param  [in]  scene HAID场景
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidSetScene(HaidScene scene);

/**
 * @if Eng
 * @brief  Get scene of HAID.
 * @param  [out]  scene HAID scene
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  获取HAID场景
 * @param  [out]  scene HAID场景
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidGetScene(HaidScene *scene);

/**
 * @if Eng
 * @brief  Set speaker calibration biquad filters.
 * @param  [in]  filtercoefs filters parameters, e.g. freqs, Qs, gains, filters are 8 bands
 * @param  [in]  coeflen filters parameters' length in byte，its value is 48
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  设置喇叭输出校准滤波器参数
 * @param  [in]  filtercoefs 校准滤波器参数, 包括中心频率、品质因数、增益等，滤波器共8段
 * @param  [in]  coeflen 滤波器参数长度字节数，值为48
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidSetSpkCaliCoef(const uint8_t *filtercoefs, uint32_t coeflen);

/**
 * @if Eng
 * @brief  Set haid hearing microphone calibration volume.
 * @param  [in]  calivolume hearing aid microphone calibration volume
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  设置麦克风校准音量
 * @param  [in]  calivolume 助听麦克风的校准音量
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidSetMicCaliVolume(const HaidMicCaliVolume *caliolume);

/**
 * @if Eng
 * @brief  Get haid environment noise.
 * @param  [out]  envNoiseDb HAID environment noise
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  获取助听环境噪声
 * @param  [out]  envNoiseDb HAID环境噪声
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t HaidGetEnvNoise(int32_t *envNoiseDb);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

