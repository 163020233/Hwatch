/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Anc manager interface
 * Author: Media Software Group
 * Create: 2025-11-05
 */

#ifndef ANC_MANAGER_H
#define ANC_MANAGER_H
#include <stdint.h>
#include <stdbool.h>
#include "audio_base_type.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
    AHA_MODE_ANC_OFF = 0,
    AHA_MODE_ANC_ON,
    AHA_MODE_AH,
    AHA_MODE_HT,
    AHA_MODE_MAX,
    AHA_MODE_BUTT = 0xFFFFFFFF,
} AhaMode;

#define AUDIO_ANC_CMD(num) (uint32_t)(0x0a0c0000 | (num))

typedef enum {
    /*!< @if Eng Read and Write all coef command.
         @else   获取和设置ANC模块所有系数命令 @endif */
    AUDIO_ANC_RW_ALL_COEF_CMD = AUDIO_ANC_CMD(1),

    /*!< @if Eng Read and Write one BQ coef command.
         @else   获取和设置一个BQ滤波器系数命令 @endif */
    AUDIO_ANC_RW_BQ_COEF_CMD = AUDIO_ANC_CMD(2),

    /*!< @if Eng Read and Write all BQ coef of an anc channel command.
         @else   获取和设置ANC一个通道中所有BQ滤波器系数命令 @endif */
    AUDIO_ANC_RW_CH_BQ_COEF_CMD = AUDIO_ANC_CMD(3),

    /*!< @if Eng Read and Write one PGA coef command.
         @else   获取和设置一个PGA的系数命令 @endif */
    AUDIO_ANC_RW_PGA_COEF_CMD = AUDIO_ANC_CMD(4),

    /*!< @if Eng Read and Write one LIMITER coef command.
         @else   获取和设置一个LIMITER的系数命令 @endif */
    AUDIO_ANC_RW_LIMITER_COEF_CMD = AUDIO_ANC_CMD(5),

    /*!< @if Eng Read and Write one MIXER coef command.
         @else   获取和设置一个MIXER的系数命令 @endif */
    AUDIO_ANC_RW_MIXER_COEF_CMD = AUDIO_ANC_CMD(6),

    /*!< @if Eng Read and Write wfir channel coef command.
         @else   获取和设置wfir通道的系数命令 @endif */
    AUDIO_ANC_RW_WFIR_COEF_CMD = AUDIO_ANC_CMD(7),

    /*!< @if Eng Read and Write ANC ramp coef command.
         @else   获取和设置ANC ramp模块的系数命令 @endif */
    AUDIO_ANC_RW_RAMP_COEF_CMD = AUDIO_ANC_CMD(8),

    /*!< @if Eng Read and Write ANC attribute command.
         @else   获取和设置ANC模块的属性命令 @endif */
    AUDIO_ANC_RW_ATTR_CMD = AUDIO_ANC_CMD(9),

    /*!< @if Eng Read and Write all MIXER coef command.
         @else   获取和设置所有MIXER的系数命令 @endif */
    AUDIO_ANC_RW_ALL_MIXER_COEF_CMD = AUDIO_ANC_CMD(10),

    /*!< @if Eng Read and Write BQ global gain of an anc channel command.
         @else   获取和设置ANC一个通道中所有BQ滤波器全局增益命令 @endif */
    AUDIO_ANC_RW_CH_BQ_GLOBAL_GAIN_CMD = AUDIO_ANC_CMD(11),
    AUDIO_ANC_RW_CMD_MAX,
    AUDIO_ANC_RW_CMD_BUTT = 0x7FFFFFFFu,
} AudioAncCmd;

/**
 * @if Eng
 * @brief  ANC module init.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  ANC模块初始化
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t AncInit(void);

/**
 * @if Eng
 * @brief  ANC module deinit.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  ANC模块去初始化
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t AncDeinit(void);

/**
 * @if Eng
 * @brief  ANC open.
 * @param  [in]  mode AHA mode {@link AhaMode}.
 * @param  [in]  attr ANC config
 * @param  [in]  attrLength Length of ANS configuration.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  ANC打开
 * @param  [in]  mode AHA的模式 {@link AhaMode}。
 * @param  [in]  attr ANC的配置
 * @param  [in]  attrLength ANC配置的长度。
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t AncOpen(AhaMode mode, const char *attr, uint32_t attrLength);

/**
 * @if Eng
 * @brief  ANC close.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  ANC关闭
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t AncClose(void);

/**
 * @if Eng
 * @brief  ANC get default parameters.
 * @param  [in]  attr ANC configuration.
 * @param  [in]  attrLength Length of ANS configuration.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  获取ANC参数
 * @param  [in]  attr ANC的配置。
 * @param  [in]  attrLength ANC配置的长度。
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t AncGetAttr(char *attr, uint32_t attrLength);

/**
 * @if Eng
 * @brief  Setting ANC cmd Parameters.
 * @param  [in]  cmd cmd {@link AudioAncCmd}.
 * @param  [in]  param Structure corresponding to ANC in cmd.
 * @param  [in]  length Length of the structure corresponding to ANC in cmd.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  设置ANC cmd的参数
 * @param  [in]  cmd cmd {@link AudioAncCmd}.
 * @param  [in]  param ANC对应cmd的结构体。
 * @param  [in]  length ANC对应cmd的结构体的长度。
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t AncSetParam(AudioAncCmd cmd, const char *param, uint32_t length);

/**
 * @if Eng
 * @brief  Getting ANC cmd Parameters.
 * @param  [in]  cmd cmd {@link AudioAncCmd}.
 * @param  [in]  param Structure corresponding to ANC in cmd.
 * @param  [in]  length Length of the structure corresponding to ANC in cmd.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief  获取ANC cmd的参数
 * @param  [in]  cmd cmd {@link AudioAncCmd}.
 * @param  [in]  param ANC对应cmd的结构体。
 * @param  [in]  length ANC对应cmd的结构体的长度。
 * @return 返回0成功，其他值失败
 * @endif
 */
errcode_t AncGetParam(AudioAncCmd cmd, char *param, uint32_t length);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

