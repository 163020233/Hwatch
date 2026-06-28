/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: user api for active noise canceling module
 * Author: @CompanyNameTag
 */

#ifndef __SOC_UAPI_ANC_H__
#define __SOC_UAPI_ANC_H__

#include <stdint.h>
#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @defgroup soc_uapi_anc soc_uapi_anc
 * @ingroup  audio
 * @{
 */

/**
 * @if Eng
 * @brief  make anc command id.
 * @else
 * @brief  anc接口命令id生成方法
 * @endif
 */
#define uapi_anc_cmd(cmd_id) (uint32_t)(0x0a0c0000 | (cmd_id))

/**
 * @if Eng
 * @brief  AHA mode.
 * @else
 * @brief  AHA模式
 * @endif
 */
typedef enum {
    UAPI_AHA_MODE_ANC_OFF = 0,
    UAPI_AHA_MODE_ANC_ON,
    UAPI_AHA_MODE_AH,
    UAPI_AHA_MODE_HT,

    UAPI_AHA_MODE_MAX,
} uapi_aha_mode;

/**
 * @if Eng
 * @brief  ANC channel define.
 * @else
 * @brief  ANC通路类型
 * @endif
 */
typedef enum {
    UAPI_ANC_CHANNEL_FF0 = 0, /*!< @if Eng ff0 channel in ANC
                                   @else   ANC模块的ff0通路 @endif */

    UAPI_ANC_CHANNEL_FF1 = 1, /*!< @if Eng ff1 channel in ANC
                                   @else   ANC模块的ff1通路 @endif */

    UAPI_ANC_CHANNEL_FB = 2,  /*!< @if Eng fb channel in ANC
                                   @else   ANC模块的fb通路 @endif */

    UAPI_ANC_CHANNEL_SPE = 3,  /*!< @if Eng spe channel in ANC
                                    @else   ANC模块的spe通路 @endif */

    UAPI_ANC_CHANNEL_POST_PROC0 = 4, /*!< @if Eng post-proc0 channel in ANC
                                          @else   ANC模块的post-proc0通路 @endif */

    UAPI_ANC_CHANNEL_POST_PROC1 = 5, /*!< @if Eng post-proc1 channel in ANC
                                          @else   ANC模块的post-proc1通路 @endif */

    UAPI_ANC_CHANNEL_WFIR = 6, /*!< @if Eng wfir channel in ANC
                                    @else   ANC模块的wfir通路 @endif */

    UAPI_ANC_CHANNEL_TYPE_MAX,
} uapi_anc_channel_type;

/**
 * @if Eng
 * @brief  ANC module type.
 * @else
 * @brief  ANC通路中的模块类型
 * @endif
 */
typedef enum {
    UAPI_ANC_MODULE_BQ,        /*!< @if Eng BQ in ANC
                                    @else   ANC通路中的BQ模块 @endif */

    UAPI_ANC_MODULE_PGA,       /*!< @if Eng PGA in ANC
                                    @else   ANC通路中的PGA模块 @endif */

    UAPI_ANC_MODULE_LIMITER,   /*!< @if Eng limiter in ANC
                                    @else   ANC通路中的limiter模块 @endif */

    UAPI_ANC_MODULE_MAX,
} uapi_anc_module_type;

/**
 * @if Eng
 * @brief  ANC mixer id.
 * @else
 * @brief  ANC通路中mixer的编号
 * @endif
 */
typedef enum {
    UAPI_ANC_MIXER_SPE, /*!< @if Eng Mixer in ANC SPE channel
                             @else   ANC SPE通路中的Mixer @endif */

    UAPI_ANC_MIXER_FB, /*!< @if Eng Mixer in ANC FB channel
                            @else   ANC FB通路中的Mixer @endif */

    UAPI_ANC_MIXER_POST_FF0, /*!< @if Eng Mixer in ANC POST_FF0 channel
                                  @else   ANC POST_FF0通路中的Mixer @endif */

    UAPI_ANC_MIXER_POST_FF1, /*!< @if Eng Mixer in ANC POST_FF1 channel
                                  @else   ANC POST_FF1通路中的Mixer @endif */

    UAPI_ANC_MIXER_PATH_MIX0, /*!< @if Eng Mixer in ANC PATH_MIX0 channel
                                   @else   ANC PATH_MIX0通路中的Mixer @endif */

    UAPI_ANC_MIXER_PATH_MIX1, /*!< @if Eng Mixer in ANC PATH_MIX0 channel
                                   @else   ANC PATH_MIX0通路中的Mixer @endif */

    UAPI_ANC_MIXER_ID_MAX,
} uapi_anc_mixer_id;

/**
 * @if Eng
 * @brief  ANC mixer coefficient.
 * @else
 * @brief  ANC mixer系数
 * @endif
 */
typedef struct {
    uint32_t mute;
} uapi_anc_mixer_coef;

/**
 * @if Eng
 * @brief  ANC BQ coefficient.
 * @else
 * @brief  ANC BQ系数
 * @endif
 */
typedef struct {
    uint32_t a1;
    uint32_t a2;
    uint32_t b0;
    uint32_t b1;
    uint32_t b2;
} uapi_anc_bq_coef;

/**
 * @if Eng
 * @brief  Max number of BQ filters in ANC channel
 * @else
 * @brief  ANC通路中级联的BQ滤波器数量
 * @endif
 */
#define UAPI_ANC_BQ_NUM_MAX 20

/**
 * @if Eng
 * @brief  The coefficients of all BQ filters in ANC channel
 * @else
 * @brief  ANC模块中所以的BQ滤波器的系数
 * @endif
 */
typedef struct {
    uint32_t bypass;
    uint32_t gain;
    uapi_anc_bq_coef coef[UAPI_ANC_BQ_NUM_MAX];
} uapi_anc_ch_bq_coef;

/**
 * @if Eng
 * @brief  The coefficients of the PGA module in the ANC
 * @else
 * @brief  ANC中PGA模块的系数
 * @endif
 */
typedef struct {
    uint32_t sft;            /* PGA step sft default 0 [0 15] 5bit  */
    uint32_t gain;           /* PGA step gain default 1 [0 2^15] 16bit  */
    uint32_t pga_fade_in;    /* PGA step  time=2^N/fs [0 15], 5bit */
    uint32_t pga_fade_out;   /* PGA step  time=2^N/fs [0 15], 5bit */
    int32_t pga_gain;       /* PGA step  S4.20 [-8388608, 8388607] */
    uint32_t pga_auto_en;    /* PGA step  [0 1] 1bit */
    uint32_t pga_zero_en;    /* PGA step  [0 1] 1bit */
    uint32_t gain_byp;       /* pga nonstep 1 bypass 0 nonbypass [0 1] 1bit */
    uint32_t pga_byp;        /* pga step 1 bypass 0 nonbypass [0 1] 1bit */
    uint32_t pga_auto_thred; /* PGA step  [0 96000] 24bit */
} uapi_anc_pga_coef;

/**
 * @if Eng
 * @brief  The coefficients of the limiter module in the ANC
 * @else
 * @brief  ANC中limiter模块的系数
 * @endif
 */
typedef struct {
    uint32_t bypass;
    uint32_t mode;
    uint32_t thresh;
    uint32_t comp_type; /* 0-peak, 1-rms */
    uint32_t attack_step;
    uint32_t release_step;
    uint32_t alpha_rms;
    uint32_t zc_on; /* apply gain change on zero-crossing only */
} uapi_anc_limiter_coef;

/**
 * @if Eng
 * @brief  Defines the anc microphone.
 * @else
 * @brief  anc麦克风
 * @endif
 */
typedef enum {
    UAPI_ANC_AMIC0 = 0,
    UAPI_ANC_AMIC1,
    UAPI_ANC_AMIC2,
    UAPI_ANC_AMIC3,

    UAPI_ANC_DMIC0 = 0x10,
    UAPI_ANC_DMIC1,
    UAPI_ANC_DMIC2,
    UAPI_ANC_DMIC3,

    UAPI_ANC_MIC_MAX,
} uapi_anc_mic_id;

/**
 * @if Eng
 * @brief  Microphone selection for the anc channel
 * @else
 * @brief  ANC中一个通路的麦克风选择
 * @endif
 */
typedef struct {
    uint32_t channel;
    uapi_anc_mic_id mic;
} uapi_anc_channel_mic_sel;

/**
 * @if Eng
 * @brief  maximum microphone num used by anc.
 * @else
 * @brief  ANC使用的麦克风数量最大值
 * @endif
 */
#define UAPI_ANC_MIC_NUM_MAX 3

/**
 * @if Eng
 * @brief  Microphone selection for the anc
 * @else
 * @brief  ANC的麦克风选择
 * @endif
 */
typedef struct {
    uapi_anc_channel_mic_sel ch_mic_sel[UAPI_ANC_MIC_NUM_MAX];
} uapi_anc_mic_sel;

/**
 * @if Eng
 * @brief  maximum output used by anc.
 * @else
 * @brief  ANC输出使用的端口数量最大值
 * @endif
 */
#define UAPI_ANC_OUT_NUM_MAX 2

/**
 * @if Eng
 * @brief  Defines the anc output selection.
 * @else
 * @brief  ANC输出端口选择
 * @endif
 */
typedef enum {
    UAPI_ANC_OUT_HS0 = 0, /*!< @if Eng anc output to head speaker 0.
                               @else   anc输出到端口HS0 @endif */
    UAPI_ANC_OUT_HS1,     /*!< @if Eng anc output to head speaker 1.
                               @else   anc输出到端口HS1 @endif */

    UAPI_ANC_OUT_ID_MAX,
} uapi_anc_out_id;

/**
 * @if Eng
 * @brief  ANC data out select.
 * @else
 * @brief  ANC输出端口选择
 * @endif
 */
typedef struct {
    uapi_anc_out_id out_id[UAPI_ANC_OUT_NUM_MAX];
} uapi_anc_out_sel;

/**
 * @if Eng
 * @brief  ANC attribute.
 * @else
 * @brief  ANC模块属性
 * @endif
 */
typedef struct {
    uint8_t ch_en[UAPI_ANC_CHANNEL_TYPE_MAX]; /*!< @if Eng anc channel enable switch.
                                                   @else   anc通道使能开关 @endif */

    uapi_anc_mic_sel mic_sel; /*!< @if Eng anc mic select config.
                                   @else   anc麦克选择 @endif */

    uapi_anc_out_sel out_sel; /*!< @if Eng anc output select config.
                                   @else   anc输出口选择 @endif */
} uapi_anc_attr;

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
    UAPI_ANC_RW_ALL_COEF_CMD = uapi_anc_cmd(1),

    /*!< @if Eng Read and Write one BQ coef command.
         @else   获取和设置一个BQ滤波器系数命令 @endif */
    UAPI_ANC_RW_BQ_COEF_CMD = uapi_anc_cmd(2),

    /*!< @if Eng Read and Write all BQ coef of an anc channel command.
         @else   获取和设置ANC一个通道中所有BQ滤波器系数命令 @endif */
    UAPI_ANC_RW_CH_BQ_COEF_CMD = uapi_anc_cmd(3),

    /*!< @if Eng Read and Write one PGA coef command.
         @else   获取和设置一个PGA的系数命令 @endif */
    UAPI_ANC_RW_PGA_COEF_CMD = uapi_anc_cmd(4),

    /*!< @if Eng Read and Write one LIMITER coef command.
         @else   获取和设置一个LIMITER的系数命令 @endif */
    UAPI_ANC_RW_LIMITER_COEF_CMD = uapi_anc_cmd(5),

    /*!< @if Eng Read and Write one MIXER coef command.
         @else   获取和设置一个MIXER的系数命令 @endif */
    UAPI_ANC_RW_MIXER_COEF_CMD = uapi_anc_cmd(6),

    /*!< @if Eng Read and Write wfir channel coef command.
         @else   获取和设置wfir通道的系数命令 @endif */
    UAPI_ANC_RW_WFIR_COEF_CMD = uapi_anc_cmd(7),

    /*!< @if Eng Read and Write ANC ramp coef command.
         @else   获取和设置ANC ramp模块的系数命令 @endif */
    UAPI_ANC_RW_RAMP_COEF_CMD = uapi_anc_cmd(8),

    /*!< @if Eng Read and Write ANC attribute command.
         @else   获取和设置ANC模块的属性命令 @endif */
    UAPI_ANC_RW_ATTR_CMD = uapi_anc_cmd(9),

    /*!< @if Eng Read and Write all MIXER coef command.
         @else   获取和设置所有MIXER的系数命令 @endif */
    UAPI_ANC_RW_ALL_MIXER_COEF_CMD = uapi_anc_cmd(10),

    /*!< @if Eng Read and Write BQ global gain of an anc channel command.
         @else   获取和设置ANC一个通道中所有BQ滤波器全局增益命令 @endif */
    UAPI_ANC_RW_CH_BQ_GLOBAL_GAIN_CMD = uapi_anc_cmd(11),
} uapi_anc_cmd_id;

/**
 * @if Eng
 * @brief  ANC one BQ coefficient read-write param, used by #UAPI_ANC_RW_BQ_COEF_CMD.
 * @else
 * @brief  ANC一个BQ的系数读写参数，对应命令#UAPI_ANC_RW_BQ_COEF_CMD
 * @endif
 */
typedef struct {
    uint32_t channel; /* [in] see #uapi_anc_channel_type */
    uint32_t id; /* [in] */
    uapi_anc_bq_coef bq_coef; /* [in/out] */
} uapi_anc_bq_coef_rw_param;

/**
 * @if Eng
 * @brief  ANC channel BQ coefficient read-write param, used by #UAPI_ANC_RW_CH_BQ_COEF_CMD.
 * @else
 * @brief  ANC一个通道所有BQ的系数读写参数，对应命令#UAPI_ANC_RW_CH_BQ_COEF_CMD
 * @endif
 */
typedef struct {
    uint32_t channel; /* [in] see #uapi_anc_channel_type */
    uapi_anc_ch_bq_coef bq_coef; /* [in/out] */
} uapi_anc_ch_bq_coef_rw_param;

/**
 * @if Eng
 * @brief  ANC channel BQ gain read-write param, used by #UAPI_ANC_RW_CH_BQ_GLOBAL_GAIN_CMD.
 * @else
 * @brief  ANC一个通道BQ的全局增益读写参数，对应命令#UAPI_ANC_RW_CH_BQ_GLOBAL_GAIN_CMD
 * @endif
 */
typedef struct {
    uint32_t channel; /* [in] see #uapi_anc_channel_type */
    uint32_t gain; /* [in/out] */
} uapi_anc_ch_bq_gain_rw_param;

/**
 * @if Eng
 * @brief  ANC one PGA coefficient read-write param, used by #UAPI_ANC_RW_PGA_COEF_CMD.
 * @else
 * @brief  ANC一个PGA的系数读写参数，对应命令#UAPI_ANC_RW_PGA_COEF_CMD
 * @endif
 */
typedef struct {
    uint32_t channel; /* [in] see #uapi_anc_channel_type */
    uint32_t id; /* [in] */
    uapi_anc_pga_coef pga_coef; /* [in/out] */
} uapi_anc_pga_coef_rw_param;

/**
 * @if Eng
 * @brief  ANC one specific limiter's coefficient read-write param, used by #UAPI_ANC_RW_LIMITER_COEF_CMD.
 * @else
 * @brief  ANC一个limiter的系数读写参数，对应命令#UAPI_ANC_RW_LIMITER_COEF_CMD
 * @endif
 */
typedef struct {
    uint32_t channel; /* [in] see #uapi_anc_channel_type */
    uint32_t id; /* [in] */
    uapi_anc_limiter_coef limiter_coef; /* [in/out] */
} uapi_anc_limiter_coef_rw_param;

/**
 * @if Eng
 * @brief  ANC one specific mixer's coefficient read-write param, used by #UAPI_ANC_RW_MIXER_COEF_CMD.
 * @else
 * @brief  ANC一个mixer的系数读写参数，对应命令#UAPI_ANC_RW_MIXER_COEF_CMD
 * @endif
 */
typedef struct {
    uint32_t mixer_id; /* [in] see #uapi_anc_mixer_id */
    uapi_anc_mixer_coef mixer_coef; /* [in/out] */
} uapi_anc_mixer_coef_rw_param;

/**
 * @if Eng
 * @brief  ANC all mixer's coefficient read-write param, used by #UAPI_ANC_RW_ALL_MIXER_COEF_CMD.
 * @else
 * @brief  ANC所有mixer的系数读写参数，对应命令#UAPI_ANC_RW_ALL_MIXER_COEF_CMD
 * @endif
 */
typedef struct {
    uapi_anc_mixer_coef mixer_coef[UAPI_ANC_MIXER_ID_MAX]; /* [in/out] */
} uapi_anc_all_mixer_coef_rw_param;

/**
 * @if Eng
 * @brief  maximum coefficient of wfir
 * @else
 * @brief  wfir系数最大组数
 * @endif
 */
#define UAPI_ANC_WFIR_COEFB_LENGTH 128

/**
 * @if Eng
 * @brief  Defines the anc wfir connect type.
 * @else
 * @brief  wfir模块连接方式
 * @endif
 */
typedef enum {
    UAPI_ANC_WFIR_SERIAL_CONN_BQ = 0, /*!< @if Eng wfir serial connect with BQ.
                                           @else   wfir串联BQ，wfir在前 @endif */

    UAPI_ANC_BQ_SERIAL_CONN_WFIR,     /*!< @if Eng BQ serial connect with wfir.
                                           @else   BQ串联wfir，wfir在后 @endif */

    UAPI_ANC_WFIR_PARALLEL_CONN_BQ, /*!< @if Eng wfir parallel connect with BQ.
                                           @else   wfir并联BQ @endif */

    UAPI_ANC_WFIR_CONN_TYPE_MAX,
} uapi_anc_wfir_conn_type;

/**
 * @if Eng
 * @brief  ANC wfir coefficient, used by #UAPI_ANC_RW_WFIR_COEF_CMD.
 * @else
 * @brief  ANC wfir通道系数，对应命令#UAPI_ANC_RW_WFIR_COEF_CMD
 * @endif
 */
typedef struct {
    uint32_t filter_order; /* anc_wfir_cas_sel [0~127] */
    uint32_t conn_type; /* anc_ch_filters_cas_sel #uapi_anc_wfir_conn_type */
    uint32_t work_channel; /* anc_wfir_ch_sel UAPI_ANC_CHANNEL_FF0/FF1/FB/SPE */

    uint32_t lamda; /* anc_wfir_coefa S2.14 */
    uint32_t b[UAPI_ANC_WFIR_COEFB_LENGTH]; /* anc_wfir_coefb S8.16 */
} uapi_anc_wfir_coef;

/**
 * @if Eng
 * @brief  ANC ramp coefficient, used by #UAPI_ANC_RW_RAMP_COEF_CMD.
 * @else
 * @brief  ANC ramp模块系数，对应命令#UAPI_ANC_RW_RAMP_COEF_CMD
 * @endif
 */
typedef struct {
    uint32_t zero_time;
    uint32_t ramp_time;
    int32_t ramp_lamda;
    uint32_t zero_sel;
    uint32_t zero_mode;
    uint32_t ramp_mode;
} uapi_anc_ramp_coef;

/**
 * @if Eng
 * @brief  ANC core entire coefficient read-write param, used by #UAPI_ANC_RW_ALL_COEF_CMD.
 * @else
 * @brief  ANC core全量系数读写参数，对应命令#UAPI_ANC_RW_ALL_COEF_CMD
 * @endif
 */
typedef struct {
    /* anc ff0 channel */
    uapi_anc_pga_coef pga_ff0;
    uapi_anc_limiter_coef limiter_ff0;
    uapi_anc_ch_bq_coef bq_ff0;

    /* anc ff1 channel */
    uapi_anc_pga_coef pga_ff1;
    uapi_anc_limiter_coef limiter_ff1;
    uapi_anc_ch_bq_coef bq_ff1;

    /* anc fb channel */
    uapi_anc_pga_coef pga_fb;
    uapi_anc_limiter_coef limiter_fb;
    uapi_anc_ch_bq_coef bq_fb;

    /* anc spe channel */
    uapi_anc_pga_coef pga_spe;
    uapi_anc_ch_bq_coef bq_spe;

    /* anc post_proc0/anc0 channel */
    uapi_anc_pga_coef pga_anc0;
    uapi_anc_limiter_coef limiter_anc0;

    /* anc post_proc1/anc1 channel */
    uapi_anc_pga_coef pga_anc1;
    uapi_anc_limiter_coef limiter_anc1;

    /* anc mixer */
    uapi_anc_mixer_coef mixer[UAPI_ANC_MIXER_ID_MAX];
} uapi_anc_core_coef_rw_param;

/**
 * @if Eng
 * @brief  ANC module init.
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  ANC模块初始化
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_anc_init(void);

/**
 * @if Eng
 * @brief  ANC module deinit.
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  ANC模块去初始化
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_anc_deinit(void);

/**
 * @if Eng
 * @brief  Get the default attributes of ANC.
 * @param  [out]  attr ANC attributes
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  获取ANC通路默认配置
 * @param  [out]  attr ANC通路属性
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_anc_get_default_attr(uapi_anc_attr *attr);

/**
 * @if Eng
 * @brief  Open a instance for ANC.
 * @param  [in]  attr  ANC instance attribute
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  创建ANC实例
 * @param  [in]  attr  ANC实例属性
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_anc_open(const uapi_anc_attr *attr);

/**
 * @if Eng
 * @brief  Close ANC instance.
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  销毁ANC实例
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_anc_close(td_void);

/**
 * @if Eng
 * @brief  Get anc module‘s parameters.
 * @param  [in]  cmd Instruction see #uapi_anc_cmd_id
 * @param  [in/out]  param Parameter information
 * @param  [in]  param_size parameter size
 * @param  [in-out] param Parameter information
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  获取ANC中的参数
 * @param  [in]  cmd 指令，请参考#uapi_anc_cmd_id
 * @param  [in/out]  param 参数信息
 * @param  [in]  param_size 参数大小
 * @param  [in-out] param 参数信息
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_anc_get_param(uint32_t cmd, uint8_t *param, uint32_t param_size);

/**
 * @if Eng
 * @brief  Set anc module‘s parameters.
 * @param  [in] cmd Instruction see #uapi_anc_cmd_id
 * @param  [in] param Parameter information
 * @param  [in] param_size parameter size
 * @retval SUCCESS Success.
 * @retval FAILURE Failure.
 * @else
 * @brief  设置ANC中的参数
 * @param  [in]  cmd 指令，请参考#uapi_anc_cmd_id
 * @param  [in]  param 参数信息
 * @param  [in]  param_size 参数大小
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_anc_set_param(uint32_t cmd, const uint8_t *param, uint32_t param_size);

/**
 * @if Eng
 * @brief  Set ANC work mode.
 * @param  [in]  mode ANC work mode, see #uapi_aha_mode
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  设置ANC的工作模式(see #uapi_aha_mode)
 * @param  [in]  mode ANC工作模式
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_anc_set_mode(uint8_t mode);

/**
 * @if Eng
 * @brief  Get ANC work mode.
 * @param  [out] mode ANC work mode, see #uapi_aha_mode
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  获取ANC的工作模式(see #uapi_aha_mode)
 * @param  [out] mode ANC工作模式
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_anc_get_mode(uint8_t *mode);

/**
 * @if Eng
 * @brief  Set attributes of ANC.
 * @param  [in]  attr ANC attribute
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  设置ANC属性
 * @param  [in]  attr ANC属性
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_anc_set_attr(const uapi_anc_attr *attr);

/**
 * @if Eng
 * @brief  Get attributes of ANC.
 * @param  [out] attr ANC attribute
 * @retval SUCCESS   Success
 * @retval FAILURE   Failure
 * @else
 * @brief  获取AENC通道属性
 * @param  [out] attr ANC属性
 * @retval SUCCESS   成功
 * @retval FAILURE   失败
 * @endif
 */
int32_t uapi_anc_get_attr(uapi_anc_attr *attr);

/**
 * @if Eng
 * @brief  Set ANC Microphone selection.
 * @param  [in]  mic_sel Microphone selection
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  设置ANC的麦克风选择
 * @param  [in]  mic_sel 麦克风选择配置
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_anc_set_mic_sel(const uapi_anc_mic_sel *mic_sel);

/**
 * @if Eng
 * @brief  Get ANC Microphone selection.
 * @param  [out]  mic_sel Microphone selection
 * @retval SUCCESS Success
 * @retval FAILURE Failed
 * @else
 * @brief  获取ANC中一个mixer的系数
 * @param  [out] mic_sel 麦克风选择配置
 * @retval SUCCESS 成功
 * @retval FAILURE 失败
 * @endif
 */
int32_t uapi_anc_get_mic_sel(uapi_anc_mic_sel *mic_sel);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SOC_UAPI_ANC_H__ */
