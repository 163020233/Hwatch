/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BR media service，stream module
 */

#ifndef BS_BR_MEDIA_STREAM_H
#define BS_BR_MEDIA_STREAM_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of br media avrcp service role.
 * @else
 * @brief 蓝牙媒体a2dp服务角色
 * @endif
 */
typedef enum {
    BR_MEDIA_A2DP_SNK = 0x01,
    BR_MEDIA_A2DP_SRC = 0x02,
} bs_br_media_a2dp_role_t;

/**
 * @if Eng
 * @brief Enum of br media stream operate type.
 * @else
 * @brief 蓝牙媒体流操作类型
 * @endif
 */
typedef enum {
    BR_MEDIA_STREAM_OPT_START = 0x01,
} bs_br_media_stream_opt_type_t;

/**
 * @if Eng
 * @brief Enum of a2dp stream state.
 * @else
 * @brief A2dp流状态。
 * @endif
 */
typedef enum {
    BR_MEDIA_STREAM_STARTING = 1,               /*!< @if Eng when start request is reported,stream is starting state.
                                                 @else   启播请求上报时，流状态是启播请求状态。 @endif */
    BR_MEDIA_STREAM_STREAMING,              /*!< @if Eng when start request is accepted,stream is streaming state.
                                                 @else   启播请求接受后，流状态是播放状态。 @endif */
    BR_MEDIA_STREAM_STOPED,                 /*!< @if Eng when suspend request is accepted,stream is stoped state.
                                                 @else   停止请求接受后，流状态是停止状态。 @endif */
    BR_MEDIA_STREAM_CLOSED,                 /*!< @if Eng when service is disconnected,stream is closed state.
                                                 @else   服务断连时，流状态是关闭状态。 @endif */
} bs_br_media_stream_state_t;

/**
 * @if Eng
 * @brief Enum of SBC Sampling Frequency.
 * @else
 * @brief SBC采样率类型。
 * @endif
 */
typedef enum {
    BR_MEDIA_SBC_SF_16000 = 0x80,        /*!< @if Eng 16KHz Sampling Frequency.
                                            @else   16KHz采样率类型。 @endif */
    BR_MEDIA_SBC_SF_32000 = 0x40,        /*!< @if Eng 32KHz Sampling Frequency.
                                            @else   32KHz采样率类型。 @endif */
    BR_MEDIA_SBC_SF_44100 = 0x20,        /*!< @if Eng 44.1KHz Sampling Frequency.
                                            @else   44.1KHz采样率类型。 @endif */
    BR_MEDIA_SBC_SF_48000 = 0x10,        /*!< @if Eng 48KHz Sampling Frequency.
                                            @else   48KHz采样率类型。 @endif */
    BR_MEDIA_SBC_SF_ALL   = 0xF0,        /*!< @if Eng All of the above Sampling Frequency.
                                            @else   以上全部采样率类型。 @endif */
} bs_br_media_sbc_sf_t;

/**
 * @if Eng
 * @brief Enum of SBC Channel Mode.
 * @else
 * @brief SBC通道模式类型。
 * @endif
 */
typedef enum {
    BR_MEDIA_SBC_CHMODE_MONO         = 0x08,     /*!< @if Eng MONO channel mode.
                                                    @else   单声道。 @endif */
    BR_MEDIA_SBC_CHMODE_DUAL_CHANNEL = 0x04,     /*!< @if Eng dual channel channel mode.
                                                    @else   多通道。 @endif */
    BR_MEDIA_SBC_CHMODE_STEREO       = 0x02,     /*!< @if Eng stereo channel mode.
                                                    @else   立体声。 @endif */
    BR_MEDIA_SBC_CHMODE_JOINT_STEREO = 0x01,     /*!< @if Eng joint stereo channel mode.
                                                    @else   联合立体声。 @endif */
    BR_MEDIA_SBC_CHMODE_ALL          = 0x0F,     /*!< @if Eng All of the above channel mode.
                                                    @else   以上全部通道模式。 @endif */
} bs_br_media_sbc_chmode_t;

/**
 * @if Eng
 * @brief Enum of SBC Block Length.
 * @else
 * @brief SBC块长度类型。
 * @endif
 */
typedef enum {
    BR_MEDIA_SBC_BLOCK_4   = 0x80,       /*!< @if Eng The block length is 4.
                                            @else   块长度为4。 @endif */
    BR_MEDIA_SBC_BLOCK_8   = 0x40,       /*!< @if Eng The block length is 8.
                                            @else   块长度为8。 @endif */
    BR_MEDIA_SBC_BLOCK_12  = 0x20,       /*!< @if Eng The block length is 12.
                                            @else   块长度为12。 @endif */
    BR_MEDIA_SBC_BLOCK_16  = 0x10,       /*!< @if Eng The block length is 16.
                                            @else   块长度为16。 @endif */
    BR_MEDIA_SBC_BLOCK_ALL = 0xF0,       /*!< @if Eng All of the above block length.
                                            @else   以上全部块长度类型。 @endif */
} bs_br_media_sbc_block_t;

/**
 * @if Eng
 * @brief Enum of SBC Subbands.
 * @else
 * @brief SBC子带个数类型。
 * @endif
 */
typedef enum {
    BR_MEDIA_SBC_SUBBAND_4   = 0x08,     /*!< @if Eng The number of subbands is 4.
                                            @else   子带个数为4。 @endif */
    BR_MEDIA_SBC_SUBBAND_8   = 0x04,     /*!< @if Eng The number of subbands is 8.
                                            @else   子带个数为8。 @endif */
    BR_MEDIA_SBC_SUBBAND_ALL = 0x0C,     /*!< @if Eng All of the above subands number.
                                            @else   以上全部子带个数类型。 @endif */
} bs_br_media_sbc_subband_t;

/**
 * @if Eng
 * @brief Enum of SBC Allocation Method.
 * @else
 * @brief SBC分配方式类型。
 * @endif
 */
typedef enum {
    BR_MEDIA_SBC_SNR       = 0x02,   /*!< @if Eng SNR allocation method.
                                        @else   信噪比优化。 @endif */
    BR_MEDIA_SBC_LOUDNESS  = 0x01,   /*!< @if Eng Loundness allocation method.
                                        @else   响度优化。 @endif */
    BR_MEDIA_SBC_ALLOC_ALL = 0x03,   /*!< @if Eng All of the above allocation method.
                                        @else   以上全部分配方式。 @endif */
} bs_br_media_sbc_allocation_t;

/**
 * @if Eng
 * @brief Enum of MPEG-2, 4 AAC Object Type.
 * @else
 * @brief MPEG-2, 4 AAC对象类型。
 * @endif
 */
typedef enum {
    BR_MEDIA_AAC_OBJECT_TYPE_2_LC       = 0x80,  /*!< @if Eng MPEG-2 AAC LC object type.
                                                    @else   MPEG-2 AAC低复杂度。 @endif */
    BR_MEDIA_AAC_OBJECT_TYPE_4_LC       = 0x40,  /*!< @if Eng MPEG-4 AAC LC object type.
                                                    @else   MPEG-4 AAC低复杂度。 @endif */
    BR_MEDIA_AAC_OBJECT_TYPE_4_LTP      = 0x20,  /*!< @if Eng MPEG-4 AAC LTP object type.
                                                    @else   MPEG-4 AAC LTP。 @endif */
    BR_MEDIA_AAC_OBJECT_TYPE_4_SCALABLE = 0x10,  /*!< @if Eng MPEG-4 AAC Scalable object type.
                                                    @else   MPEG-4 AAC Scalable。 @endif */
    BR_MEDIA_AAC_OBJECT_TYPE_ALL        = 0xF0,  /*!< @if Eng All of the above object type.
                                                    @else   以上全部对象类型。 @endif */
} bs_br_media_aac_object_type_t;

/**
 * @if Eng
 * @brief Enum of MPEG-2, 4 AAC Sampling Frequency.
 * @else
 * @brief MPEG-2, 4 AAC采样率类型。
 * @endif
 */
typedef enum {
    BR_MEDIA_AAC_SF_8000  = 0x0800,      /*!< @if Eng 8KHz Sampling Frequency.
                                            @else 8KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_11025 = 0x0400,      /*!< @if Eng 11.025KHz Sampling Frequency.
                                            @else 11.025KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_12000 = 0x0200,      /*!< @if Eng 12KHz Sampling Frequency.
                                            @else 12KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_16000 = 0x0100,      /*!< @if Eng 16KHz Sampling Frequency.
                                            @else 16KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_22050 = 0x0080,      /*!< @if Eng 22.05KHz Sampling Frequency.
                                            @else 22.05KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_24000 = 0x0040,      /*!< @if Eng 24KHz Sampling Frequency.
                                            @else 24KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_32000 = 0x0020,      /*!< @if Eng 32KHz Sampling Frequency.
                                            @else 32KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_44100 = 0x0010,      /*!< @if Eng 44.1KHz Sampling Frequency.
                                            @else 44.1KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_48000 = 0x0008,      /*!< @if Eng 48KHz Sampling Frequency.
                                            @else 48KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_64000 = 0x0004,      /*!< @if Eng 64KHz Sampling Frequency.
                                            @else 64KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_88200 = 0x0002,      /*!< @if Eng 88.2KHz Sampling Frequency.
                                            @else 88.2KHz采样率类型。 @endif */
    BR_MEDIA_AAC_SF_96000 = 0x0001,      /*!< @if Eng 96KHz Sampling Frequency.
                                            @else 96KHz采样率类型。 @endif */
} bs_br_media_aac_sf_t;

/**
 * @if Eng
 * @brief Enum of AAC Channels.
 * @else
 * @brief MPEG-2, 4 AAC通道类型。
 * @endif
 */
typedef enum {
    BR_MEDIA_AAC_CHANNEL_1   = 0x08,     /*!< @if Eng 1 channel.
                                            @else  1通道。 @endif */
    BR_MEDIA_AAC_CHANNEL_2   = 0x04,     /*!< @if Eng 2 channel.
                                            @else  2通道。 @endif */
    BR_MEDIA_AAC_CHANNEL_ALL = 0x0C,     /*!< @if Eng All of the above channels.
                                            @else   以上全部通道类型。 @endif */
} bs_br_media_aac_channel_t;

/**
 * @if Eng
 * @brief Enum of AAC Variable Bit Rate.
 * @else
 * @brief MPEG-2, 4 AAC可变比特率类型。
 * @endif
 */
typedef enum {
    BR_MEDIA_AAC_VBR_SUPPORTED = 0x80,    /*!< @if Eng Variable Bit Rate supported.
                                            @else  支持可变比特率类型。 @endif */
} bs_br_media_aac_vbr_t;

/**
 * @if Eng
 * @brief MPEG4 AAC ISO 14496-3 codec info.
 * @else
 * @brief MPEG4 AAC ISO 14496-3编码格式信息。
 * @endif
 */
typedef struct {
    uint16_t sample_rate;  /*!< @if Eng Sample frequency, @See bs_br_media_aac_sf_t.
                                    @else 采样频率，@参考 bs_br_media_aac_sf_t @endif */
    uint32_t bit_rate;          /*!< @if Eng bit rate.
                                    @else 比特率。 @endif */
    uint8_t channel;           /*!< @if Eng channels, @See bs_br_media_aac_channel_t.
                                    @else 通道，@参考 bs_br_media_aac_channel_t @endif */
    uint8_t object_type;        /*!< @if Eng object type, @See bs_br_media_aac_object_type_t.
                                    @else 对象类型，@参考 bs_br_media_aac_object_type_t @endif */
    uint8_t rfa;                /*!< @if Eng Reserved for future use.
                                    @else 预留 @endif */
    uint8_t vbr;                /*!< @if Eng Variable Bit Rate supported, @See bs_br_media_aac_vbr_t.
                                    @else 支持动态比特率, @参考 bs_br_media_aac_vbr_t @endif */
} bs_br_media_aac_codec_t;

/**
 * @if Eng
 * @brief SBC codec info.
 * @else
 * @brief SBC编码格式信息。
 * @endif
 */
typedef struct {
    uint8_t sample_rate;    /*!< @if Eng Sample frequency, @See bs_br_media_sbc_sf_t.
                                     @else   采样频率，@参考 bs_br_media_sbc_sf_t @endif */
    uint8_t channel;            /*!< @if Eng Channel mode, @See bs_br_media_sbc_chmode_t.
                                     @else 通道模式  @参考 bs_br_media_sbc_chmode_t @endif */
    uint8_t block_length;         /*!< @if Eng Block length, @See bs_br_media_sbc_block_t.
                                     @else   块长度, @参考 bs_br_media_sbc_block_t @endif */
    uint8_t subband;              /*!< @if Eng Num of subband, @See bs_br_media_sbc_subband_t.
                                     @else   子带个数, @参考 bs_br_media_sbc_subband_t @endif */
    uint8_t alloc_method;         /*!< @if Eng Allocation Method,@See bs_br_media_sbc_allocation_t.
                                     @else Allocation Method, @参考 bs_br_media_sbc_allocation_t @endif */
    uint8_t min_bitpool;          /*!< @if Eng Min bitpool value 2~250.
                                     @else   最位池，取值 2~250。 @endif */
    uint8_t max_bitpool;          /*!< @if Eng Max bitpool value 2~250.
                                     @else   最位池，值 2~250。 @endif */
} bs_br_media_sbc_codec_t;

/**
 * @if Eng
 * @brief codec type.
 * @else
 * @brief 编码格式类型。
 * @endif
 */
typedef enum {
    BR_MEDIA_STREAM_CODEC_SBC = 0x00,          /*!< @if Eng SBC codec type. @else SBC编码格式类型。 @endif */
    BR_MEDIA_STREAM_CODEC_AAC = 0x01,       /*!< @if Eng MPEG24/AAC codec type.
                                                    @else MPEG24/AAC编码格式类型。 @endif */
} bs_br_media_codec_type_t;

/**
 * @if Eng
 * @brief Define of A2DP codec parameters.
 * @else
 * @brief A2DP 编码格式参数定义。
 * @endif
 */
typedef struct {
    uint8_t codec_type;             /*!< @if Eng The stream codec type, @See bs_br_media_codec_type_t.
                                         @else 音频流编解码类型, @参考 bs_br_media_codec_type_t @endif */
    union {
        bs_br_media_sbc_codec_t sbc;   /*!< @if Eng SBC codec info.
                                        @else SBC编解码信息。 @endif */
        bs_br_media_aac_codec_t aac;   /*!< @if Eng AAC codec info.
                                        @else AAC编解码信息。 @endif */
    } info;
} bs_br_media_codec_info_t;

/**
 * @if Eng
 * @brief Use this funtion to register a2dp service.
 * @par Description: Use this funtion to register a2dp service.
 * @param  [in]  a2dp_role local a2dp profile role。See @ref bs_br_media_a2dp_role_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 注册蓝牙a2dp服务。
 * @par 说明: 注册蓝牙a2dp服务。
 * @param  [in]  a2dp_role 本地服务角色。参考 @ref bs_br_media_a2dp_role_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_media_a2dp_register(uint8_t a2dp_role);

/**
 * @if Eng
 * @brief Use this funtion to unregister a2dp service.
 * @par Description: Use this funtion to unregister a2dp service.
 * @param  [in]  a2dp_role local a2dp profile role。See @ref bs_br_media_a2dp_role_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 去注册蓝牙a2dp服务。
 * @par 说明: 去注册蓝牙a2dp服务。
 * @param  [in]  a2dp_role 本地服务角色。参考 @ref bs_br_media_a2dp_role_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_media_a2dp_unregister(uint8_t a2dp_role);

/**
 * @if Eng
 * @brief Use this funtion to create a2dp connection.
 * @par Description: Use this funtion to create a2dp connection.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  a2dp_role local avrcp profile role。See @ref bs_br_media_a2dp_role_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 建立蓝牙媒体a2dp连接。
 * @par 说明: 建立蓝牙媒体a2dp连接。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  a2dp_role 本端服务角色。参考 @ref bs_br_media_a2dp_role_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_media_a2dp_connect(uint16_t dev_id, uint8_t a2dp_role);

/**
 * @if Eng
 * @brief Use this funtion to release a2dp connection.
 * @par Description: Use this funtion to release a2dp connection.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  a2dp_role local a2dp profile role。See @ref bs_br_media_a2dp_role_t
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 断开蓝牙媒体a2dp连接。
 * @par 说明: 断开蓝牙媒体a2dp连接。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  a2dp_role 本端服务角色。参考 @ref bs_br_media_a2dp_role_t
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_media_a2dp_disconnect(uint16_t dev_id, uint8_t a2dp_role);

/**
 * @if Eng
 * @brief Use this funtion to start media stream.
 * @par Description: Use this funtion to start media stream.
 * @param  [in]  dev_id The id of one connected device.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 蓝牙媒体音频流启播请求。
 * @par 说明: 蓝牙媒体音频流启播请求。
 * @param  [in]  dev_id 对端设备编号。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_start_media_stream(uint16_t dev_id);

/**
 * @if Eng
 * @brief Use this funtion to stop media stream.
 * @par Description: Use this funtion to stop media stream.
 * @param  [in]  dev_id The id of one connected device.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 蓝牙媒体音频流停止请求。
 * @par 说明: 蓝牙媒体音频流停止请求。
 * @param  [in]  dev_id 对端设备编号。
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_stop_media_stream(uint16_t dev_id);

/**
 * @if Eng
 * @brief Use this funtion to reply media stream operate.
 * @par Description: Use this funtion to reply media stream operate.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  opt_type operate type. See @ref bs_br_media_stream_opt_type_t.
 * @param  [in]  is_accept true is accept, false is reject.
 * @retval #ERRCODE_BT_SUCCESS   Excute successfully.
 * @retval #Other               Error code. See @ref errcode_bt_t
 * @par Dependency:
 * @else
 * @brief 蓝牙媒体音频流操作响应。
 * @par 说明: 蓝牙媒体音频流操作响应。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  opt_type 操作类型. See @ref bs_br_media_stream_opt_type_t.
 * @param  [in]  is_accept true 接受, false 拒绝.
 * @retval #ERRCODE_BT_SUCCESS   执行成功。
 * @retval #Other               错误码。参考 @ref errcode_bt_t
 * @par 依赖:
 * @endif
 */
errcode_t bs_br_media_stream_operate_result_reply(uint16_t dev_id, uint8_t opt_type, bool is_accept);

/**
 * @if Eng
 * @brief Callback invoked when a2dp connection status changed.
 * @par Description: Callback invoked when a2dp connection status changed.
 * @param  [in]  dev_id  The id of one connected device.
 * @param  [in]  a2dp_role local a2dp profile role。See @ref bs_br_media_a2dp_role_t
 * @param  [in]  conn_state hfp connection status。See @ref bs_br_profile_connect_state_t
 * @retval #void  No return value.
 * @par Dependency:
 * @else
 * @brief 服务连接状态上报。
 * @par 说明: 服务连接状态上报。
 * @param  [in]  dev_id  对端设备编号
 * @param  [in]  a2dp_role 本端服务角色。参考 @ref bs_br_media_a2dp_role_t
 * @param  [in]  conn_state 服务连接状态。See @ref bs_br_profile_connect_state_t
 * @retval #void  无返回值。
 * @par 依赖:
 * @endif
 */
typedef void (*bs_br_media_a2dp_conn_state_cbk)(uint16_t dev_id, uint8_t a2dp_role, uint8_t conn_state);

/**
 * @if Eng
 * @brief  Callback invoked when media stream state changed.
 * @par Description:
 *           Callback invoked when media stream state changed.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  a2dp_role local a2dp profile role。See @ref bs_br_media_a2dp_role_t
 * @param  [in]  stream_state the status of stream. See @ref bs_br_media_stream_state_t.
 * @retval #void  no return value.
 * @par Dependency:
 * @li
 * @see  bs_br_media_stream_callbacks_t
 * @else
 * @brief  媒体流状态上报。
 * @par 说明:媒体流状态上报。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  a2dp_role 本端服务角色。参考 @ref bs_br_media_a2dp_role_t
 * @param  [in]  stream_state the status of stream. See @ref bs_br_media_stream_state_t.
 * @retval #void  无返回值。
 * @par 依赖:
 * @li
 * @see  bs_br_media_stream_callbacks_t
 * @endif
 */
typedef void (*bs_br_media_stream_state_changed_cbk)(uint16_t dev_id, uint8_t a2dp_role, uint8_t stream_state);

/**
 * @if Eng
 * @brief  Callback invoked when receive media stream codec.
 * @par Description:
 *           Callback invoked when receive media stream codec.
 * @param  [in]  dev_id The id of one connected device.
 * @param  [in]  a2dp_role local a2dp profile role。See @ref bs_br_media_a2dp_role_t
 * @param  [in]  codec the pointer of stream codec. See @ref bs_br_media_codec_info_t.
 * @retval #void  no return value.
 * @par Dependency:
 * @li
 * @see  bs_br_media_stream_callbacks_t
 * @else
 * @brief  媒体流编解码上报。
 * @par 说明:媒体流编解码上报。
 * @param  [in]  dev_id 对端设备编号。
 * @param  [in]  a2dp_role 本端服务角色。参考 @ref bs_br_media_a2dp_role_t
 * @param  [in]  codec 流编解码参数指针. See @ref bs_br_media_codec_info_t.
 * @retval #void  无返回值。
 * @par 依赖:
 * @li
 * @see  bs_br_media_stream_callbacks_t
 * @endif
 */
typedef void (*bs_br_media_stream_codec_report_cbk)(uint16_t dev_id, uint8_t a2dp_role,
    bs_br_media_codec_info_t *codec);

/**
 * @if Eng
 * @brief  Struct of br media stream callback function.
 * @else
 * @brief  BR media 流回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_br_media_a2dp_conn_state_cbk connect_state_cbk;
    bs_br_media_stream_state_changed_cbk stream_state_cbk;
    bs_br_media_stream_codec_report_cbk stream_codec_cbk;
} bs_br_media_stream_callbacks_t;

/**
 * @if Eng
 * @brief  Use this funtion to register callback function of framework.
 * @par Description:
 *           Use this funtion to register callback function of framework.
 * @param  [in]  app_id application id , default is 0.
 * @param  [in]  func   A poniter of the callback function. See @ref bs_br_media_stream_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  注册上层应用的回调。
 * @par 说明:
 *           注册上层应用的回调。
 * @param  [in]  app_id 应用id 默认为0。
 * @param  [in]  func   回调函数指针。参考 @ref bs_br_media_stream_callbacks_t
 * @retval #ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 * @li
 */
errcode_t bs_br_media_stream_register_callbacks(uint8_t app_id, bs_br_media_stream_callbacks_t *func);

/**
 * @if Eng
 * @brief  Use this funtion to deregister callback function of framework.
 * @par Description:
 *           Use this funtion to deregister callback function of framework.
 * @param  [in]  app_id application id , default is 0.
 * @retval #ERRCODE_BT_SUCCESS   Success.
 * @retval Other                Failure. For details, see @ref errcode_bt_t
 * @else
 * @brief  去注册上层应用的回调。
 * @par 说明:
 *           去注册上层应用的回调。
 * @param  [in]  app_id 应用id 默认为0。
 * @retval #ERRCODE_BT_SUCCESS   成功。
 * @retval Other                失败，参考 @ref errcode_bt_t
 * @endif
 * @par Dependency:
 * @li bts_a2dp_snk.h
 */
errcode_t bs_br_media_stream_unregister_callbacks(uint8_t app_id);

#ifdef __cplusplus
}
#endif
#endif