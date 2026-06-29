/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Service Framework (SSF) server audio sream management APIs
 */
#ifndef SSF_AS_H
#define SSF_AS_H

#include <stdbool.h>
#include "sle_common.h"
#include "errcode.h"
#include "ssf_device.h"
#include "ssf_autorate.h"
#include "ssf_as_stru.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 上报对端音频能力的回调函数
 * @param [in] sle_obj UX操作对象句柄
 * @param [in] device_id 设备id
 * @param [in] cap 对端设备音频能力
 */
typedef void (*bs_sle_report_peer_audio_ability_cbk)(
    sle_obj_t sle_obj, uint32_t device_id, bs_sle_audio_stream_cap_t cap);

/**
 * @brief 上报编码器参数回调函数
 * @param [in] sle_obj UX操作对象句柄。
 * @param [in] stream_num 音频流数目。
 * @param [in] param 音频流编码器信息。
 */
typedef void (*bs_sle_audio_stream_codec_report_cbk)(
    sle_obj_t sle_obj, uint8_t stream_num, bs_sle_stream_codec_param_t *param);

/**
 * @brief 音频流状态改变回调函数
 * @param [in] sle_obj UX操作对象句柄。
 * @param [in] stream_num 音频流数目。
 * @param [in] stream_stru 音频流信息。
 * @param [in] state 音频流信息。
 * @param [in] result 错误码。
 */
typedef void (*bs_sle_audio_stream_state_changed_cbk)(
    sle_obj_t sle_obj, uint8_t stream_num, bs_sle_stream_stru_t *stream_stru,
    bs_sle_audio_stream_state_t state, errcode_t result);

/**
 * @brief TWS上报声道绑定同步链路结果回调
 * @param [in] earphone_num 耳机数量。
 * @param [in] param 绑定信息。
 */
typedef void (*bs_sle_audio_stream_iso_bond_report_cbk)(uint8_t num, iso_bond_param_t* param);

/**
 * @brief 上报码率修改变化值
 * @param [in] rate 码率值。
 * @param [in] param 提供给DSP发送到BGC的phy参数。
 */
typedef void (*bs_sle_autorate_rate_change_report_cbk)(uint32_t rate, sle_iog_phy_param_t *param);

/**
 * @brief 上报autorate档位切换后的值
 * @param [in] level 档位值。
 * @param [in] rate 当前的码率值。
 */
typedef void (*bs_sle_autorate_level_change_report_cbk)(uint8_t level, uint32_t rate);

typedef struct {
    bs_sle_report_peer_audio_ability_cbk report_peer_ability_cb;             // 回调函数，报告对端音频能力

    bs_sle_audio_stream_codec_report_cbk report_as_codec_cb;                 // 回调函数，报告编码器参数

    bs_sle_audio_stream_state_changed_cbk report_as_state_changed_cb;        // 回调函数，报告创建音频流状态

    bs_sle_audio_stream_iso_bond_report_cbk report_as_bond_cb;               // 回调函数，报告同步链路

    bs_sle_autorate_rate_change_report_cbk report_rate_change_cb;            // 回调函数，报告码率修改

    bs_sle_autorate_level_change_report_cbk report_autorate_level_cb;        // 回调函数，报告自动码率档位改变
} bs_sle_audio_stream_callbacks_t;

// open to UX
/**
 * @brief 注册音频流客户端回调函数
 * @param [in] app_id 上报的应用id。
 * @param [in] cbks 注册的回调函数。
 */
errcode_t bs_sle_audio_stream_register_callbacks_client(uint32_t app_id, bs_sle_audio_stream_callbacks_t *cbks);

/**
 * @brief 注册音频流服务端回调函数
 * @param [in] app_id 上报的应用id。
 * @param [in] cbks 注册的回调函数。
 */
errcode_t bs_sle_audio_stream_register_callbacks_server(uint32_t app_id, bs_sle_audio_stream_callbacks_t *cbks);

/**
 * @brief 去注册音频流客户端回调函数
 * @param [in] app_id 上报的应用id。
 */
errcode_t bs_sle_unreg_audio_stream_cbk_client(uint32_t app_id);

/**
 * @brief 去注册音频流服务端回调函数
 * @param [in] app_id 上报的应用id。
 */
errcode_t bs_sle_unreg_audio_stream_cbk_server(uint32_t app_id);

/**
 * @brief 创建音频流函数
 * @param [in] sle_obj UX操作对象句柄。
 * @param [in] type 音频流类型。
 */
errcode_t bs_sle_create_audio_stream(sle_obj_t sle_obj, bs_sle_audio_stream_type_t *stream_type);

/**
 * @brief 创建音频流函数
 * @param [in] sle_obj UX操作对象句柄。
 */
errcode_t bs_sle_start_audio_stream(sle_obj_t sle_obj,  bs_sle_audio_stream_type_t *stream_type);

/**
 * @brief 创建音频流函数
 * @param [in] sle_obj UX操作对象句柄。
 */
errcode_t bs_sle_stop_audio_stream(sle_obj_t sle_obj, bs_sle_audio_stream_type_t *stream_type);

/**
 * @brief 创建音频流函数
 * @param [in] sle_obj UX操作对象句柄。
 */
errcode_t bs_sle_release_audio_stream(sle_obj_t sle_obj, bs_sle_audio_stream_type_t *stream_type);

/**
 * @brief 回复对端音频流操作请求
 * @param [in] stream_num 音频流个数。
 * @param [in] stream_res 音频流操作结果。
 * @param [in] opt_type 音频流操作类型。
 */
errcode_t bs_sle_audio_stream_operate_result_reply(uint8_t stream_num, bs_sle_stream_res_stru_t *stream_res);

/**
 * @brief 仅在服务端用，查询对应obj设备当前音频业务码率
 * @param [in] sle_obj UX操作对象句柄。
 * @param [out] bitrate 当前音频流的实时码率。
 */
errcode_t bs_sle_audio_stream_server_get_current_bitrate(sle_obj_t sle_obj, uint32_t *bitrate);

#ifdef __cplusplus
}
#endif
#endif