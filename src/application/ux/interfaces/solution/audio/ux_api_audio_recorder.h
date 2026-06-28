/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux audio recorder uapi \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-01， Create file. \n
 */

#ifndef UX_API_AUDIO_RECORDER_H
#define UX_API_AUDIO_RECORDER_H

#include "stdint.h"
#include "stdbool.h"
#include "errcode.h"
#include "ux_api_audio_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 录音位宽枚举
 */
typedef enum {
    UX_API_AUDIO_RECORDER_BIT_WIDTH_24  = 24,         /**< 24位宽 */
    UX_API_AUDIO_RECORDER_BIT_WIDTH_32_FLOAT = 33,    /**< 32位宽浮点 */

    UX_API_AUDIO_RECORDER_BIT_WIDTH_MAX = 0xff        /**< 无效枚举值 */
} ux_api_audio_recorder_bit_width_t;

/**
 * @brief 录音采样率枚举
 */
typedef enum {
    UX_API_AUDIO_RECORDER_SAMPLE_RATE_48000  = 48000,  /**< 48000采样率 */

    UX_API_AUDIO_RECORDER_SAMPLE_RATE_MAX = 0xffffff   /**< 无效枚举值 */
} ux_api_audio_recorder_sample_rate_t;

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 启动设备录音
 * @param [in] device_id 设备ID
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_start(uint16_t device_id);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 停止设备录音
 * @param [in] device_id 设备ID
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_stop(uint16_t device_id);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 获取设备录音状态
 * @param [in] device_id 设备ID
 * @param [out] status 指向用于存储录音状态的变量的指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_get_status(uint16_t device_id, bool *status);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 设置录音的位宽
 * @param [in] device_id 设备ID
 * @param [in] bit_width 录音的位宽，单位为比特（bit）
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_set_bit_width(uint16_t device_id, ux_api_audio_recorder_bit_width_t bit_width);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 获取当前录音的位宽
 * @param [in] device_id 设备ID
 * @param [out] bit_width 指向用于存储当前位宽的变量的指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_get_bit_width(uint16_t device_id, ux_api_audio_recorder_bit_width_t *bit_width);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 设置录音的采样率
 * @param [in] device_id 设备ID
 * @param [in] sample_rate 录音的采样率，单位为赫兹（Hz）
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_set_sample_rate(uint16_t device_id, ux_api_audio_recorder_sample_rate_t sample_rate);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 获取当前录音的采样率
 * @param [in] device_id 设备ID
 * @param [out] sample_rate 指向用于存储当前采样率的变量的指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_get_sample_rate(uint16_t device_id, ux_api_audio_recorder_sample_rate_t *sample_rate);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 设置录音文件的分段时长
 * @param [in] device_id 设备ID
 * @param [in] duration 录音文件的分段时长，单位为秒（s）
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_set_split_by_duration(uint16_t device_id, const uint32_t duration);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 获取当前录音文件的分段时长
 * @param [in] device_id 设备ID
 * @param [out] duration 指向用于存储当前分段时长的变量的指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_get_split_by_duration(uint16_t device_id, uint32_t *duration);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 设置录音文件的分段大小
 * @param [in] device_id 设备ID
 * @param [in] size 录音文件的分段大小，单位为字节（bytes）
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_set_split_by_size(uint16_t device_id, uint32_t size);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 获取当前录音文件的分段大小
 * @param [in] device_id 设备ID
 * @param [out] size 指向用于存储当前分段大小的变量的指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_get_split_by_size(uint16_t device_id, uint32_t *size);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 设置录音文件的输出目录
 * @param [in] device_id 设备ID
 * @param [in] output_dir 录音文件的输出目录路径
 * @param [in] output_dir_len 录音文件输出路径的长度
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_set_output_dir(uint16_t device_id, char *output_dir, uint16_t output_dir_len);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 获取当前录音文件的输出目录
 * @param [in] device_id 设备ID
 * @param [out] output_dir 指向用于存储当前输出目录路径的缓冲区
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_get_output_dir(uint16_t device_id, char *output_dir, uint16_t output_dir_len);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 获取当前录音的剩余时间
 * @param [in] device_id 设备ID
 * @param [out] remain_time 指向用于存储剩余时间的变量的指针，单位为毫秒（ms）
 * @param [in] data_len 数据长度
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_get_remain_time(uint16_t device_id, uint64_t *remain_time, uint16_t data_len);

/**
 * @defgroup 支持产品 [livemic_tx]
 * @brief 设置录音打标
 * @param [in] buffer 打标数据缓冲区
 * @param [in] buf_len 打标数据长度
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_write_marks(const uint8_t *buffer, const int buf_len);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 设置录音算法旁路模式
 * @param [in] device_id 通过device_id可以设置本地以及对端录音配置
 * @param [in] bypass 是否旁路音频算法处理（关闭音频算法处理）
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_set_alg_bypass(uint16_t device_id, bool bypass);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 获取录音算法旁路模式状态
 * @param [in] device_id 通过device_id可以获取本地以及对端录音配置
 * @param [out] bypass 是否旁路音频算法处理（关闭音频算法处理）
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_recorder_get_alg_bypass(uint16_t device_id, bool* bypass);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
