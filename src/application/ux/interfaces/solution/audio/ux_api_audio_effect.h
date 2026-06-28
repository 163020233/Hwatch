/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux audio effect uapi \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-01， Create file. \n
 */

#ifndef UX_API_AUDIO_EFFECT_H
#define UX_API_AUDIO_EFFECT_H

#include "stdint.h"
#include "stdbool.h"
#include "errcode.h"
#include "ux_api_audio_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PGA_GAIN_MIN     (-8388608)           /**< PGA最小增益值 */
#define PGA_GAIN_MAX      8388607             /**< PGA最大增益值 */
#define PGA_GAIN_DEFAULT  1048576             /**< PGA默认增益值 */
#define UX_API_AUDIO_DEVICE_NUM_MAX 8         /**< 最大音频设备数量 */
#define UX_API_AUDIO_CHANNEL_8 8              /**< 8通道 */

/**
 * @brief 音频混音矩阵联合体
 */
typedef union {
    int32_t mix_matrix[UX_API_AUDIO_DEVICE_NUM_MAX][UX_API_AUDIO_CHANNEL_8]; /**< 8×8 全量矩阵 */
    struct {
        int32_t device0[UX_API_AUDIO_CHANNEL_8]; /**< 发射器0的音量矩阵 */
        int32_t device1[UX_API_AUDIO_CHANNEL_8]; /**< 发射器1的音量矩阵 */
        int32_t device2[UX_API_AUDIO_CHANNEL_8]; /**< 发射器2的音量矩阵 */
        int32_t device3[UX_API_AUDIO_CHANNEL_8]; /**< 发射器3的音量矩阵 */
        int32_t device4[UX_API_AUDIO_CHANNEL_8]; /**< 发射器4的音量矩阵 */
        int32_t device5[UX_API_AUDIO_CHANNEL_8]; /**< 发射器5的音量矩阵 */
        int32_t device6[UX_API_AUDIO_CHANNEL_8]; /**< 发射器6的音量矩阵 */
        int32_t device7[UX_API_AUDIO_CHANNEL_8]; /**< 发射器7的音量矩阵 */
    } device_volume;
} ux_api_audio_mix_matrix_t;

/**
 * @brief ANC自定义模式枚举
 */
typedef enum {
    UX_API_AUDIO_ANC_CUST_MODE_0   = 0,
    UX_API_AUDIO_ANC_CUST_MODE_1   = 1,
    UX_API_AUDIO_ANC_CUST_MODE_2   = 2,
    UX_API_AUDIO_ANC_CUST_MODE_3   = 3,
    UX_API_AUDIO_ANC_CUST_MODE_4   = 4,
    UX_API_AUDIO_ANC_CUST_MODE_5   = 5,
    UX_API_AUDIO_ANC_CUST_MODE_6   = 6,
    UX_API_AUDIO_ANC_CUST_MODE_7   = 7,
    UX_API_AUDIO_ANC_CUST_MODE_8   = 8,
    UX_API_AUDIO_ANC_CUST_MODE_9   = 9,
    UX_API_AUDIO_ANC_CUST_MODE_10  = 10,
    UX_API_AUDIO_ANC_CUST_MODE_11  = 11,
    UX_API_AUDIO_ANC_CUST_MODE_12  = 12,
    UX_API_AUDIO_ANC_CUST_MODE_13  = 13,
    UX_API_AUDIO_ANC_CUST_MODE_14  = 14,
    UX_API_AUDIO_ANC_CUST_MODE_15  = 15,
    UX_API_AUDIO_ANC_CUST_MODE_16  = 16,
    UX_API_AUDIO_ANC_CUST_MODE_17  = 17,
    UX_API_AUDIO_ANC_CUST_MODE_18  = 18,
    UX_API_AUDIO_ANC_CUST_MODE_19  = 19,
    UX_API_AUDIO_ANC_CUST_MODE_20  = 20,
    UX_API_AUDIO_ANC_MODE_MAX             /**< 无效ANC模式 */
} ux_api_audio_anc_mode_t;

/**
 * @brief 降噪模式枚举
 */
typedef enum {
    UX_API_AUDIO_NR_OFF_MODE      = 0, /**< 无降噪模式 */
    UX_API_AUDIO_NR_NORMAL_MODE   = 1, /**< 普通降噪模式 */
    UX_API_AUDIO_NR_ENHANCED_MODE = 2, /**< 增强降噪模式 */
    UX_API_AUDIO_NR_MODE_MAX            /**< 无效降噪模式 */
} ux_api_audio_nr_mode_t;

/**
 * @brief EQ模式枚举
 */
typedef enum {
    UX_API_AUDIO_EQ_OFF_MODE,            /**< 无EQ模式 */
    UX_API_AUDIO_EQ_HIFI_MODE,           /**< 高保真模式 */
    UX_API_AUDIO_EQ_VOICE_ENHANCED_MODE, /**< 人声增强模式 */
    UX_API_AUDIO_EQ_MODE_MAX,            /**< 无效EQ模式 */
} ux_api_audio_eq_mode_t;

/**
 * @brief 混音模式枚举
 */
typedef enum {
    UX_API_AUDIO_MIX_MODE_MONO,         /**< 麦克风混音模式单声道 */
    UX_API_AUDIO_MIX_MODE_STEREO,       /**< 麦克风混音模式立体声 */
    UX_API_AUDIO_MIX_MODE_SAFETY_TRACK, /**< 麦克风混音模式安全音轨 */
    UX_API_AUDIO_MIX_MODE_MAX,          /**< 最大值 */
} ux_api_audio_mix_mode_t;

/**
 * @brief ANC信息结构体
 */
typedef struct {
    ux_api_audio_anc_mode_t anc_mode;   /**< 需要设置的anc档位 */
    uint8_t anc_tone_id;                /**< 设anc时需要播放的提示音id(若无需播放则传入静音帧id) */
} ux_api_audio_anc_info_t;

/**
 * @brief 音频输出端口枚举
 */
typedef enum {
    UX_API_AUDIO_OUTPUT_PORT_DAC, /**< DAC输出端口 */
    UX_API_AUDIO_OUTPUT_PORT_UAC, /**< UAC输出端口 */
    UX_API_AUDIO_PORT_MAX,        /**< 最大值 */
} ux_api_audio_output_port_t;

/**
 * @brief DRC参数结构体
 */
typedef struct {
    int16_t target_gain;                /**< DRC目标增益 范围[-40, 80] */
    int16_t max_level;                   /**< 最大输出能量 范围[-80, 0] */
    int16_t silence_threshold;          /**< DRC底噪压制阈值 范围[-360, -120] */
    int16_t transit_band_width;         /**< DRC过度带宽度 范围[0, 160] */
    int16_t silence_gain;               /**< DRC底噪压制力度 范围[-120, 0] */
} ux_api_audio_drc_param_t;

/**
 * @brief 音频降噪控制结构体
 */
typedef struct {
    ux_api_audio_stream_type_t    stream; /**< 音频类型 */
    ux_api_audio_nr_mode_t mode;          /**< 降噪模式 */
} ux_api_audio_nr_info_t;

/**
 * @defgroup 支持产品 [livemic_tx]
 * @brief 设置本地音频降噪参数
 * @param [in] info 音频降噪控制结构体，包含音频类型和降噪模式
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_nr(const ux_api_audio_nr_info_t* info);

/**
 * @defgroup 支持产品 [livemic_tx]
 * @brief 获取本地音频降噪参数
 * @param [in] stream 音频流类型
 * @return ux_api_audio_nr_mode_t 当前设置的降噪模式
 */
ux_api_audio_nr_mode_t ux_api_audio_get_nr(ux_api_audio_stream_type_t stream);

/**
 * @defgroup 支持产品 [livemic_rx]
 * @brief 设置远程音频降噪参数
 * @param [in] device_id 远程设备ID
 * @param [in] info 音频降噪控制结构体，包含音频类型和降噪模式
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_remote_nr(uint16_t device_id, ux_api_audio_nr_info_t* info);

/**
 * @defgroup 支持产品 [livemic_rx]
 * @brief 获取远程音频降噪参数
 * @param [in] device_id 远程设备ID
 * @param [in] stream 音频流类型
 * @return ux_api_audio_nr_mode_t 当前设置的降噪模式
 */
ux_api_audio_nr_mode_t ux_api_audio_get_remote_nr(uint16_t device_id, ux_api_audio_stream_type_t stream);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 设置降噪模式。
 * @param [in] device_id 设备ID
 * @param [in] mode 要设置的降噪模式，参考 @ref ux_api_audio_nr_mode_t
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_nr_mode(uint16_t device_id, ux_api_audio_nr_mode_t mode);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 获取当前的降噪模式。
 * @param [in] device_id 设备ID
 * @param [out] mode 指向用于存储当前降噪模式的变量的指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_get_nr_mode(uint16_t device_id, ux_api_audio_nr_mode_t *mode);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 启用或禁用低频切除功能
 * @param [in] device_id 设备ID
 * @param [in] enable 布尔值，'true' 表示启用低频切除，'false' 表示禁用低频切除
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_low_cut(uint16_t device_id, bool enable);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 检查低频切除功能是否启用
 * @param [in] device_id 设备ID
 * @param [out] enable 布尔值，'true' 表示启用低频切除，'false' 表示禁用低频切除
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_get_low_cut(uint16_t device_id, bool *enable);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 设置EQ模式
 * @param [in] device_id 设备ID
 * @param [in] mode 要设置的eq模式，参考 @ref ux_api_audio_eq_mode_t
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_eq_mode(uint16_t device_id, ux_api_audio_eq_mode_t mode);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 获取当前的EQ模式
 * @param [in] device_id 设备ID
 * @param [out] mode 指向用于存储当前eq模式的变量的指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_get_eq_mode(uint16_t device_id, ux_api_audio_eq_mode_t *mode);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 设置第几套EQ参数
 * @param [in] param_index NV中已调好的EQ参数中的某一组序号
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_eq_param(uint8_t param_index);

/**
 * @defgroup 支持产品 [livemic_rx]
 * @brief 设置混音模式
 * @param [in] mix_mode 设置的混音模式，参考 @ref ux_api_audio_mix_mode_t
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_mix_mode(ux_api_audio_mix_mode_t mix_mode);

/**
 * @defgroup 支持产品 [livemic_rx]
 * @brief 获取混音模式
 * @return ux_api_audio_mix_mode_t 设置的混音模式，参考 @ref ux_api_audio_mix_mode_t
 */
ux_api_audio_mix_mode_t ux_api_audio_get_mix_mode(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 禁用ANC
 * @param [in] anc_tone_id 关闭ANC时需要播放的提示音id(若无需播放则传入静音帧id)
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_anc_disable(uint8_t anc_tone_id);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 设置ANC全量系数
 * @param [in] anc_info 设置的anc档位, 以及设置时播报的提示音
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_anc_info(ux_api_audio_anc_info_t anc_info);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 设置PGA增益值
 * @param [in] pga_gain 设置的pga增益值, 范围是[-8388608,8388607]
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_pga_gain(int32_t pga_gain);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 获取ANC开关状态
 * @return true 开启；false 关闭
 */
bool ux_api_audio_get_anc_state(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 查询ANC档位
 * @return ux_api_audio_anc_mode_t 获取到的ANC档位，参考 @ref ux_api_audio_anc_mode_t
 */
ux_api_audio_anc_mode_t ux_api_audio_get_anc_mode(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 查询PGA增益值
 * @return int32_t 获取到的PGA增益值
 */
int32_t ux_api_audio_get_pga_gain(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 获取EQ使能状态
 * @return true：使能；false：去使能
 */
bool ux_api_audio_get_eq_state(void);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 设置EQ使能状态
 * @param [in] state 使能状态，true：使能；false：去使能
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_eq_state(bool state);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 获取指定音频流EQ参数索引
 * @param [in] audio_stream_type 获取EQ参数索引的音频流，参考 @ref ux_api_audio_stream_type_t
 *        必须是以下值：AUDIO_STREAM_TYPE_MUSIC_A2DP_SINK、AUDIO_STREAM_TYPE_CALL_HFP_SCO
 * @param [out] param_index NV中已调好的EQ参数中的某一组序号
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_get_stream_eq_param_index(ux_api_audio_stream_type_t audio_stream_type, uint8_t *param_index);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 设置指定音频流EQ参数索引
 * @param [in] audio_stream_type 设置EQ参数的音频流，参考 @ref ux_api_audio_stream_type_t
 *        必须是以下值：AUDIO_STREAM_TYPE_MUSIC_A2DP_SINK、AUDIO_STREAM_TYPE_CALL_HFP_SCO
 * @param [in] param_index NV中已调好的EQ参数中的某一组序号
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_stream_eq_param_index(ux_api_audio_stream_type_t audio_stream_type, uint8_t param_index);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 使能DRC，必须在音频流创建后调用
 * @param [in] audio_port 端口类型，参考 @ref ux_api_audio_output_port_t
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_drc_enable(ux_api_audio_output_port_t audio_port);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 设置DRC参数，必须在音频流创建后调用
 * @param [in] audio_port 端口类型，参考 @ref ux_api_audio_output_port_t
 * @param [in] param DRC参数，参考 @ref ux_api_audio_drc_param_t
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_drc_set_param(ux_api_audio_output_port_t audio_port, ux_api_audio_drc_param_t *param);

/**
 * @defgroup 支持产品 [earphone]
 * @brief 去使能DRC，必须在音频流创建后调用
 * @param [in] audio_port 端口类型，参考 @ref ux_api_audio_output_port_t
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_drc_disable(ux_api_audio_output_port_t audio_port);

/**
 * @defgroup 支持产品 [livemic_rx]
 * @brief 设置混音矩阵
 * @param [in] port 音频输出端口
 * @param [in] matrix 混音矩阵指针，包含8个发射器的音量矩阵
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_mix_matrix(ux_api_audio_output_port_t port, ux_api_audio_mix_matrix_t* matrix);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
