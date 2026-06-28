/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: header file for active noise cancellation
 * Author: audio
 */

#ifndef __ANC_STRUCT_H__
#define __ANC_STRUCT_H__

#include "td_type.h"
#include "anc_coef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief  make anc command id.
 * @else
 * @brief  anc接口命令id生成方法
 * @endif
 */
#define anc_cmd(cmd_id) (td_u32)(0x0a0c0000 | (cmd_id))

#define ANC_REG_SIZE 1024

/* AHA mode */
typedef enum {
    AHA_MODE_ANC_OFF = 0,
    AHA_MODE_ANC_ON,
    AHA_MODE_AH,
    AHA_MODE_HT,

    AHA_MODE_MAX,
} aha_mode;

typedef struct {
    td_u32 offset;
    td_u32 value;
} anc_reg;

typedef struct {
    aha_mode mode;
    anc_reg anc_reg[ANC_REG_SIZE];
} anc_config;

typedef struct {
    aha_mode mode;
    td_s8 *config;
    td_u32 config_len;
} anc_cfg;

/**
 * @if Eng
 * @brief  Defines the anc microphone.
 * @else
 * @brief  anc麦克风
 * @endif
 */
typedef enum {
    ANC_AMIC0 = 0,
    ANC_AMIC1,
    ANC_AMIC2,
    ANC_AMIC3,

    ANC_DMIC0 = 0x10,
    ANC_DMIC1,
    ANC_DMIC2,
    ANC_DMIC3,

    ANC_MIC_MAX,
} anc_mic_id;

/**
 * @if Eng
 * @brief  Microphone selection for the anc channel
 * @else
 * @brief  ANC中一个通路的麦克风选择
 * @endif
 */
typedef struct {
    td_u32 channel;
    anc_mic_id mic;
} anc_channel_mic_sel;

/**
 * @if Eng
 * @brief  maximum microphone num used by anc.
 * @else
 * @brief  ANC使用的麦克风数量最大值
 * @endif
 */
#define ANC_MIC_NUM_MAX 3

/**
 * @if Eng
 * @brief  Microphone selection for the anc
 * @else
 * @brief  ANC的麦克风选择
 * @endif
 */
typedef struct {
    anc_channel_mic_sel ch_mic_sel[ANC_MIC_NUM_MAX];
} anc_mic_sel;

/**
 * @if Eng
 * @brief  maximum output used by anc.
 * @else
 * @brief  ANC输出使用的端口数量最大值
 * @endif
 */
#define ANC_OUT_NUM_MAX 2

/**
 * @if Eng
 * @brief  Defines the anc output selection.
 * @else
 * @brief  ANC输出端口选择
 * @endif
 */
typedef enum {
    ANC_OUT_HS0 = 0, /*!< @if Eng anc output to head speaker 0.
                          @else   anc输出到端口HS0 @endif */
    ANC_OUT_HS1,     /*!< @if Eng anc output to head speaker 1.
                          @else   anc输出到端口HS1 @endif */

    ANC_OUT_ID_MAX,
} anc_out_id;

/**
 * @if Eng
 * @brief  ANC data out select.
 * @else
 * @brief  ANC输出端口选择
 * @endif
 */
typedef struct {
    anc_out_id out_id[ANC_OUT_NUM_MAX];
} anc_out_sel;

/**
 * @if Eng
 * @brief  ANC attribute.
 * @else
 * @brief  ANC模块属性
 * @endif
 */
typedef struct {
    td_u8 ch_en[ANC_CH_NUM]; /*!< @if Eng anc channel enable switch.
                                    @else   anc通道使能开关 @endif */

    anc_mic_sel mic_sel; /*!< @if Eng anc mic select config.
                              @else   anc麦克选择 @endif */

    anc_out_sel out_sel; /*!< @if Eng anc output select config.
                              @else   anc输出口选择 @endif */
} anc_attr;

/**
 * @if Eng
 * @brief  Audio ANC command.
 * @else
 * @brief  音频ANC接口命令
 * @endif
 */
typedef enum {
    /*!< @if Eng Read and Write all coef command.
         @else   获取和设置ANC模块所有系数命令 @endif */
    ANC_RW_ALL_COEF_CMD = anc_cmd(1),

    /*!< @if Eng Read and Write one BQ coef command.
         @else   获取和设置一个BQ滤波器系数命令 @endif */
    ANC_RW_BQ_COEF_CMD = anc_cmd(2),

    /*!< @if Eng Read and Write all BQ coef of an anc channel command.
         @else   获取和设置ANC一个通道中所有BQ滤波器系数命令 @endif */
    ANC_RW_CH_BQ_COEF_CMD = anc_cmd(3),

    /*!< @if Eng Read and Write one PGA coef command.
         @else   获取和设置一个PGA的系数命令 @endif */
    ANC_RW_PGA_COEF_CMD = anc_cmd(4),

    /*!< @if Eng Read and Write one LIMITER coef command.
         @else   获取和设置一个LIMITER的系数命令 @endif */
    ANC_RW_LIMITER_COEF_CMD = anc_cmd(5),

    /*!< @if Eng Read and Write one MIXER coef command.
         @else   获取和设置一个MIXER的系数命令 @endif */
    ANC_RW_MIXER_COEF_CMD = anc_cmd(6),

    /*!< @if Eng Read and Write wfir channel coef command.
         @else   获取和设置wfir通道的系数命令 @endif */
    ANC_RW_WFIR_COEF_CMD = anc_cmd(7),

    /*!< @if Eng Read and Write ANC ramp coef command.
         @else   获取和设置ANC ramp模块的系数命令 @endif */
    ANC_RW_RAMP_COEF_CMD = anc_cmd(8),

    /*!< @if Eng Read and Write ANC attribute command.
         @else   获取和设置ANC模块的属性命令 @endif */
    ANC_RW_ATTR_CMD = anc_cmd(9),

    /*!< @if Eng Read and Write all MIXER coef command.
         @else   获取和设置所有MIXER的系数命令 @endif */
    ANC_RW_ALL_MIXER_COEF_CMD = anc_cmd(10),

    /*!< @if Eng Read and Write BQ global gain of an anc channel command.
         @else   获取和设置ANC一个通道中所有BQ滤波器全局增益命令 @endif */
    ANC_RW_CH_BQ_GLOBAL_GAIN_CMD = anc_cmd(11),
} anc_cmd_id;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* __ANC_STRUCT_H__ */
