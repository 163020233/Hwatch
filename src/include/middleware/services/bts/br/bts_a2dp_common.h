/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: BT A2DP-SNK module.
 */

/**
 * @addtogroup bluetooth_bts_a2dp_common A2DP API
 * @ingroup    bluetooth
 * @{
 */

#ifndef BTS_A2DP_COMMON_H
#define BTS_A2DP_COMMON_H

#include "bts_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of a2dp stream state.
 * @else
 * @brief A2dp流状态。
 * @endif
 */
typedef enum {
    A2DP_STREAM_UNKNOWN = 0,            /*!< @if Eng unknown invalid state.
                                            @else   未知无效状态。 @endif */
    A2DP_STREAM_CREATE = 1,             /*!< @if Eng when service is connecting,stream is create state.
                                            @else   服务连接中时，流状态是创建状态。 @endif */
    A2DP_STREAM_OPENED,                 /*!< @if Eng when service is connected,stream is open state.
                                            @else   服务已连接时，流状态是打开状态。 @endif */
    A2DP_STREAM_START,                  /*!< @if Eng when start request is accepted,stream is start state.
                                            @else   启播请求接受后，流状态是开始状态。 @endif */
    A2DP_STREAM_SUSPENDED,              /*!< @if Eng when suspend request is accepted,stream is suspended state.
                                            @else   暂停请求接受后，流状态是暂停状态。 @endif */
    A2DP_STREAM_CLOSED,                 /*!< @if Eng when service is disconnected,stream is closed state.
                                            @else   服务断连时，流状态是关闭状态。 @endif */
    A2DP_STREAM_START_REQ,               /*!< @if Eng when start request is reported,stream is start_req state.
                                            @else   启播请求上报时，流状态是启播请求状态。 @endif */
} a2dp_stream_state_t;

/**
 * @if Eng
 * @brief Enum of A2DP codec type.
 * @else
 * @brief A2DP 编码格式类型。
 * @endif
 */
typedef enum {
    A2DP_CODEC_TYPE_SBC,                /*!< @if Eng SBC codec type.
                                            @else   编码格式类型为SBC。 @endif */
    A2DP_CODEC_TYPE_MPEG1_2_AUDIO,      /*!< @if Eng MPEG1_2_AUDIO codec type.
                                            @else   编码格式类型为MPEG1_2_AUDIO。 @endif */
    A2DP_CODEC_TYPE_AAC,                /*!< @if Eng AAC codec type.
                                            @else   编码格式类型为AAC。 @endif */
    A2DP_CODEC_TYPE_MPEGD_USAC,                /*!< @if Eng MPEG-D USAC codec type.
                                            @else   编码格式类型为MPEG-D USAC。 @endif */
    A2DP_CODEC_TYPE_ATRAC,              /*!< @if Eng ATRAC codec type.
                                            @else   编码格式类型为ATRAC。 @endif */
    A2DP_CODEC_TYPE_NON_A2DP = 0xFF     /*!< @if Eng Invalid codec type.
                                            @else   无效编码格式类型。 @endif */
} a2dp_codec_type_t;

/* AVDTP codec type ID */
typedef enum {
    SERVICE_AVDTP_CODEC_TYPE_SBC,
    SERVICE_AVDTP_CODEC_TYPE_MPEG1_2_AUDIO,
    SERVICE_AVDTP_CODEC_TYPE_MPEG2_4_AAC,
    SERVICE_AVDTP_CODEC_TYPE_ATRAC,
    SERVICE_AVDTP_CODEC_TYPE_OPUS,
    SERVICE_AVDTP_CODEC_TYPE_H263,
    SERVICE_AVDTP_CODEC_TYPE_MPEG4_VSP,
    SERVICE_AVDTP_CODEC_TYPE_H263_PROF3,
    SERVICE_AVDTP_CODEC_TYPE_H263_PROF8,
    SERVICE_AVDTP_CODEC_TYPE_LHDC,
    SERVICE_AVDTP_CODEC_TYPE_LC3,
    SERVICE_AVDTP_CODEC_TYPE_L2HC,
    SERVICE_AVDTP_CODEC_TYPE_L2HC_LC3,
    SERVICE_AVDTP_CODEC_TYPE_LDAC,
    SERVICE_AVDTP_CODEC_TYPE_NON_A2DP
} service_avdtp_codec_type;

/**
 * @if Eng
 * @brief Enum of SBC Sampling Frequency.
 * @else
 * @brief SBC采样率类型。
 * @endif
 */
typedef enum {
    BT_A2DP_SBC_SF_16000 = 0x80,        /*!< @if Eng 16KHz Sampling Frequency.
                                            @else   16KHz采样率类型。 @endif */
    BT_A2DP_SBC_SF_32000 = 0x40,        /*!< @if Eng 32KHz Sampling Frequency.
                                            @else   32KHz采样率类型。 @endif */
    BT_A2DP_SBC_SF_44100 = 0x20,        /*!< @if Eng 44.1KHz Sampling Frequency.
                                            @else   44.1KHz采样率类型。 @endif */
    BT_A2DP_SBC_SF_48000 = 0x10,        /*!< @if Eng 48KHz Sampling Frequency.
                                            @else   48KHz采样率类型。 @endif */
    BT_A2DP_SBC_SF_ALL   = 0xF0,        /*!< @if Eng All of the above Sampling Frequency.
                                            @else   以上全部采样率类型。 @endif */
} a2dp_sbc_sf_field_t;

/**
 * @if Eng
 * @brief Enum of SBC Channel Mode.
 * @else
 * @brief SBC通道模式类型。
 * @endif
 */
typedef enum {
    BT_A2DP_SBC_CHMODE_MONO         = 0x08,     /*!< @if Eng MONO channel mode.
                                                    @else   单声道。 @endif */
    BT_A2DP_SBC_CHMODE_DUAL_CHANNEL = 0x04,     /*!< @if Eng dual channel channel mode.
                                                    @else   多通道。 @endif */
    BT_A2DP_SBC_CHMODE_STEREO       = 0x02,     /*!< @if Eng stereo channel mode.
                                                    @else   立体声。 @endif */
    BT_A2DP_SBC_CHMODE_JOINT_STEREO = 0x01,     /*!< @if Eng joint stereo channel mode.
                                                    @else   联合立体声。 @endif */
    BT_A2DP_SBC_CHMODE_ALL          = 0x0F,     /*!< @if Eng All of the above channel mode.
                                                    @else   以上全部通道模式。 @endif */
} a2dp_sbc_chmode_field_t;

/**
 * @if Eng
 * @brief Enum of SBC Block Length.
 * @else
 * @brief SBC块长度类型。
 * @endif
 */
typedef enum {
    BT_A2DP_SBC_BLOCK_4   = 0x80,       /*!< @if Eng The block length is 4.
                                            @else   块长度为4。 @endif */
    BT_A2DP_SBC_BLOCK_8   = 0x40,       /*!< @if Eng The block length is 8.
                                            @else   块长度为8。 @endif */
    BT_A2DP_SBC_BLOCK_12  = 0x20,       /*!< @if Eng The block length is 12.
                                            @else   块长度为12。 @endif */
    BT_A2DP_SBC_BLOCK_16  = 0x10,       /*!< @if Eng The block length is 16.
                                            @else   块长度为16。 @endif */
    BT_A2DP_SBC_BLOCK_ALL = 0xF0,       /*!< @if Eng All of the above block length.
                                            @else   以上全部块长度类型。 @endif */
} a2dp_sbc_block_field_t;

/**
 * @if Eng
 * @brief Enum of SBC Subbands.
 * @else
 * @brief SBC子带个数类型。
 * @endif
 */
typedef enum {
    BT_A2DP_SBC_SUBBAND_4   = 0x08,     /*!< @if Eng The number of subbands is 4.
                                            @else   子带个数为4。 @endif */
    BT_A2DP_SBC_SUBBAND_8   = 0x04,     /*!< @if Eng The number of subbands is 8.
                                            @else   子带个数为8。 @endif */
    BT_A2DP_SBC_SUBBAND_ALL = 0x0C,     /*!< @if Eng All of the above subands number.
                                            @else   以上全部子带个数类型。 @endif */
} a2dp_sbc_subband_field_t;

/**
 * @if Eng
 * @brief Enum of SBC Allocation Method.
 * @else
 * @brief SBC分配方式类型。
 * @endif
 */
typedef enum {
    BT_A2DP_SBC_SNR       = 0x02,   /*!< @if Eng SNR allocation method.
                                        @else   信噪比优化。 @endif */
    BT_A2DP_SBC_LOUDNESS  = 0x01,   /*!< @if Eng Loundness allocation method.
                                        @else   响度优化。 @endif */
    BT_A2DP_SBC_ALLOC_ALL = 0x03,   /*!< @if Eng All of the above allocation method.
                                        @else   以上全部分配方式。 @endif */
} a2dp_sbc_allocation_field_t;

/**
 * @if Eng
 * @brief Enum of MPEG-2, 4 AAC Object Type.
 * @else
 * @brief MPEG-2, 4 AAC对象类型。
 * @endif
 */
typedef enum {
    BT_A2DP_AAC_OBJECT_TYPE_2_LC       = 0x80,  /*!< @if Eng MPEG-2 AAC LC object type.
                                                    @else   MPEG-2 AAC低复杂度。 @endif */
    BT_A2DP_AAC_OBJECT_TYPE_4_LC       = 0x40,  /*!< @if Eng MPEG-4 AAC LC object type.
                                                    @else   MPEG-4 AAC低复杂度。 @endif */
    BT_A2DP_AAC_OBJECT_TYPE_4_LTP      = 0x20,  /*!< @if Eng MPEG-4 AAC LTP object type.
                                                    @else   MPEG-4 AAC LTP。 @endif */
    BT_A2DP_AAC_OBJECT_TYPE_4_SCALABLE = 0x10,  /*!< @if Eng MPEG-4 AAC Scalable object type.
                                                    @else   MPEG-4 AAC Scalable。 @endif */
    BT_A2DP_AAC_OBJECT_TYPE_ALL        = 0xF0,  /*!< @if Eng All of the above object type.
                                                    @else   以上全部对象类型。 @endif */
} a2dp_aac_object_type_field_t;

/**
 * @if Eng
 * @brief Enum of MPEG-2, 4 AAC Sampling Frequency.
 * @else
 * @brief MPEG-2, 4 AAC采样率类型。
 * @endif
 */
typedef enum {
    BT_A2DP_AAC_SF_8000  = 0x0800,      /*!< @if Eng 8KHz Sampling Frequency.
                                            @else 8KHz采样率类型。 @endif */
    BT_A2DP_AAC_SF_11025 = 0x0400,      /*!< @if Eng 11.025KHz Sampling Frequency.
                                            @else 11.025KHz采样率类型。 @endif */
    BT_A2DP_AAC_SF_12000 = 0x0200,      /*!< @if Eng 12KHz Sampling Frequency.
                                            @else 12KHz采样率类型。 @endif */
    BT_A2DP_AAC_SF_16000 = 0x0100,      /*!< @if Eng 16KHz Sampling Frequency.
                                            @else 16KHz采样率类型。 @endif */
    BT_A2DP_AAC_SF_22050 = 0x0080,      /*!< @if Eng 22.05KHz Sampling Frequency.
                                            @else 22.05KHz采样率类型。 @endif */
    BT_A2DP_AAC_SF_24000 = 0x0040,      /*!< @if Eng 24KHz Sampling Frequency.
                                            @else 24KHz采样率类型。 @endif */
    BT_A2DP_AAC_SF_32000 = 0x0020,      /*!< @if Eng 32KHz Sampling Frequency.
                                            @else 32KHz采样率类型。 @endif */
    BT_A2DP_AAC_SF_44100 = 0x0010,      /*!< @if Eng 44.1KHz Sampling Frequency.
                                            @else 44.1KHz采样率类型。 @endif */
    BT_A2DP_AAC_SF_48000 = 0x0008,      /*!< @if Eng 48KHz Sampling Frequency.
                                            @else 48KHz采样率类型。 @endif */
    BT_A2DP_AAC_SF_64000 = 0x0004,      /*!< @if Eng 64KHz Sampling Frequency.
                                            @else 64KHz采样率类型。 @endif */
    BT_A2DP_AAC_SF_88200 = 0x0002,      /*!< @if Eng 88.2KHz Sampling Frequency.
                                            @else 88.2KHz采样率类型。 @endif */
    BT_A2DP_AAC_SF_96000 = 0x0001,      /*!< @if Eng 96KHz Sampling Frequency.
                                            @else 96KHz采样率类型。 @endif */
} a2dp_aac_sf_field_t;

/**
 * @if Eng
 * @brief Enum of AAC Channels.
 * @else
 * @brief MPEG-2, 4 AAC通道类型。
 * @endif
 */
typedef enum {
    BT_A2DP_AAC_CHANNEL_1   = 0x08,     /*!< @if Eng 1 channel.
                                            @else  1通道。 @endif */
    BT_A2DP_AAC_CHANNEL_2   = 0x04,     /*!< @if Eng 2 channel.
                                            @else  2通道。 @endif */
    BT_A2DP_AAC_CHANNEL_ALL = 0x0C,     /*!< @if Eng All of the above channels.
                                            @else   以上全部通道类型。 @endif */
} a2dp_aac_channel_field_t;

/**
 * @if Eng
 * @brief Enum of AAC Variable Bit Rate.
 * @else
 * @brief MPEG-2, 4 AAC可变比特率类型。
 * @endif
 */
typedef enum {
    BT_A2DP_AAC_VBR_SUPPORTED = 0x80,    /*!< @if Eng Variable Bit Rate supported.
                                            @else  支持可变比特率类型。 @endif */
} a2dp_aac_vbr_field_t;

/**
 * @if Eng
 * @brief Struct of SBC codec info.
 * @else
 * @brief SBC编码格式信息。
 * @endif
 */
typedef struct {
    uint8_t sample_frequency;    /*!< @if Eng Sample frequency, @See a2dp_sbc_sf_field_t.
                                     @else   采样频率，@参考 a2dp_sbc_sf_field_t。 @endif */
    uint8_t chnl_mode;            /*!< @if Eng Channel mode, @See a2dp_sbc_chmode_field_t.
                                     @else 通道模式  @参考 a2dp_sbc_chmode_field_t。 @endif */
    uint8_t block_length;         /*!< @if Eng Block length, @See a2dp_sbc_block_field_t.
                                     @else   块长度, @参考 a2dp_sbc_block_field_t。 @endif */
    uint8_t subband;              /*!< @if Eng Num of subband, @See a2dp_sbc_subband_field_t.
                                     @else   子带个数, @参考 a2dp_sbc_subband_field_t。 @endif */
    uint8_t alloc_method;         /*!< @if Eng Allocation Method,@See a2dp_sbc_allocation_field_t.
                                     @else Allocation Method, @参考 a2dp_sbc_allocation_field_t @endif */
    uint8_t min_bitpool;          /*!< @if Eng Min bitpool value 2~250.
                                     @else   最位池，取值 2~250。 @endif */
    uint8_t max_bitpool;          /*!< @if Eng Max bitpool value 2~250.
                                     @else   最位池，值 2~250。 @endif */
} a2dp_sbc_codec_t;

/**
 * @if Eng
 * @brief Struct of MPEG-2, 4 AAC codec info.
 * @else
 * @brief MPEG-2, 4 AAC编码格式信息。
 * @endif
 */
typedef struct {
    uint16_t sample_frequency;  /*!< @if Eng Sample frequency, @See a2dp_aac_sf_field_t.
                                    @else 采样频率，@参考 a2dp_aac_sf_field_t。 @endif */
    uint32_t bit_rate;          /*!< @if Eng bit rate.
                                    @else 比特率。 @endif */
    uint8_t channels;           /*!< @if Eng channels, @See a2dp_aac_channel_field_t.
                                    @else 通道，@参考 a2dp_aac_channel_field_t。 @endif */
    uint8_t object_type;        /*!< @if Eng object type, @See a2dp_aac_object_type_field_t.
                                    @else 对象类型，@参考 a2dp_aac_object_type_field_t @endif */
    uint8_t rfa;                /*!< @if Eng Reserved for future use.
                                    @else 预留 @endif */
    uint8_t vbr;                /*!< @if Eng Variable Bit Rate supported, @See a2dp_aac_vbr_field_t.
                                    @else 支持动态比特率, @参考 a2dp_aac_vbr_field_t @endif */
} a2dp_aac_codec_t;

/**
 * @if Eng
 * @brief Struct of codec info.
 * @else
 * @brief 编解码配置信息。
 * @endif
 */
typedef struct {
    uint8_t codec_type;             /*!< @if Eng The stream codec type, @See a2dp_codec_type_t.
                                        @else 音频流编解码类型, @参考 a2dp_codec_type_t。 @endif */
    union {
        a2dp_sbc_codec_t sbc;   /*!< @if Eng SBC codec info.
                                        @else SBC编解码信息。 @endif */
        a2dp_aac_codec_t aac;   /*!< @if Eng AAC codec info.
                                        @else AAC编解码信息。 @endif */
    } info;
} a2dp_stream_codec_param_t;

/**
 * @if Eng
 * @brief Struct of stream config info.
 * @else
 * @brief 流配置信息。
 * @endif
 */
typedef struct {
    uint32_t stream_handle;         /*!< @if Eng Stream handle.
                                        @else 流句柄。 @endif */
    uint16_t payload_size;          /*!< @if Eng The size of media stream playload.
                                        @else 音频流负载大小。 @endif */
    a2dp_stream_codec_param_t stream_codec; /*!< @if Eng The info of stream codec.
                                                @else 音频流编解码信息。 @endif */
} a2dp_stream_codec_t;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* BTS_A2DP_SINK_H */
