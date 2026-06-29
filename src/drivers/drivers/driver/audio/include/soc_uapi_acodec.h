/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved. \n
 * Description: user header file for audio decoders and encoders \n
 * Author: @CompanyNameTag \n
 */

#ifndef __SOC_UAPI_ACODEC_H__
#define __SOC_UAPI_ACODEC_H__

#include <stdint.h>
#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup soc_uapi_acodec soc_uapi_acodec
 * @ingroup  audio
 * @{
 */

/**
 * @if Eng
 * @brief  Audio codec ID definition.
 * @else
 * @brief  音频编解码器ID定义
 * @endif
 */
typedef enum {
    UAPI_ACODEC_ID_PCM,
    UAPI_ACODEC_ID_MP2,
    UAPI_ACODEC_ID_MP3,
    UAPI_ACODEC_ID_SBC,
    UAPI_ACODEC_ID_AAC,
    UAPI_ACODEC_ID_BLYRAYLPCM,
    UAPI_ACODEC_ID_COOK,
    UAPI_ACODEC_ID_DRA,
    UAPI_ACODEC_ID_WMA9STD,
    UAPI_ACODEC_ID_AMRNB,
    UAPI_ACODEC_ID_AMRWB,
    UAPI_ACODEC_ID_G711,
    UAPI_ACODEC_ID_G722,
    UAPI_ACODEC_ID_G726,
    UAPI_ACODEC_ID_ADPCM,
    UAPI_ACODEC_ID_VOICE,
    UAPI_ACODEC_ID_TRUEHD,
    UAPI_ACODEC_ID_AC3PASSTHROUGH,
    UAPI_ACODEC_ID_DTSPASSTHROUGH,
    UAPI_ACODEC_ID_OPUS,
    UAPI_ACODEC_ID_VORBIS,
    UAPI_ACODEC_ID_MSBC,
    UAPI_ACODEC_ID_FLAC,
    UAPI_ACODEC_ID_L2HC,
    UAPI_ACODEC_ID_LC3,
    UAPI_ACODEC_ID_SILK,
    UAPI_ACODEC_ID_APE,
    UAPI_ACODEC_ID_ALAC,
    UAPI_ACODEC_ID_LDAC,
    UAPI_ACODEC_ID_LHDC,
    UAPI_ACODEC_ID_L2HC_VOICE,
    UAPI_ACODEC_ID_MAX,
} uapi_acodec_id;

/**
 * @if Eng
 * @brief  Definition of the distribution mode of the channels of a codec.
 * @else
 * @brief  定义codec声道分布模式
 * @endif
 */
typedef enum {
    UAPI_ACODEC_CHANNEL_NONE = 0x0, /*!< @if Eng Unused or empty
                                         @else   未使用或为空 @endif */
    UAPI_ACODEC_CHANNEL_LF = 0x1,   /*!< @if Eng Left front
                                         @else   左前 @endif */
    UAPI_ACODEC_CHANNEL_RF = 0x2,   /*!< @if Eng Right front
                                         @else   右前 @endif */
    UAPI_ACODEC_CHANNEL_CF = 0x3,   /*!< @if Eng Center front
                                         @else   中置 @endif */
    UAPI_ACODEC_CHANNEL_LS = 0x4,   /*!< @if Eng Left surround
                                         @else   左环绕 @endif */
    UAPI_ACODEC_CHANNEL_RS = 0x5,   /*!< @if Eng Right surround
                                         @else   右环绕 @endif */
    UAPI_ACODEC_CHANNEL_LFE = 0x6,  /*!< @if Eng Low frequency effects
                                         @else   低音 @endif */
    UAPI_ACODEC_CHANNEL_CS = 0x7,   /*!< @if Eng Back surround
                                         @else   后环绕 @endif */
    UAPI_ACODEC_CHANNEL_LR = 0x8,   /*!< @if Eng Left rear
                                         @else   左后 @endif */
    UAPI_ACODEC_CHANNEL_RR = 0x9,   /*!< @if Eng Right rear
                                         @else   右后 @endif */

    UAPI_ACODEC_CHANNEL_MAX,
} uapi_acodec_channel;

/**
 * @if Eng
 * @brief  Definition of the frame length of the opus decoder
 * @else
 * @brief  定义opus解码器帧长
 * @endif
 */
typedef enum {
    UAPI_ACODEC_OPUS_FRAME_LENGTH_2P5_MS = 25,
    UAPI_ACODEC_OPUS_FRAME_LENGTH_5_MS = 50,
    UAPI_ACODEC_OPUS_FRAME_LENGTH_10_MS = 100,
    UAPI_ACODEC_OPUS_FRAME_LENGTH_20_MS = 200,
    UAPI_ACODEC_OPUS_FRAME_LENGTH_40_MS = 400,
    UAPI_ACODEC_OPUS_FRAME_LENGTH_60_MS = 600,
    UAPI_ACODEC_OPUS_FRAME_LENGTH_80_MS = 800,
    UAPI_ACODEC_OPUS_FRAME_LENGTH_100_MS = 1000,
    UAPI_ACODEC_OPUS_FRAME_LENGTH_120_MS = 1200,
} uapi_acodec_opus_frame_length;

/**
 * @if Eng
 * @brief  Configuration parameters required by an codec for creating devices.
 * @else
 * @brief  解码器创建设备的配置参数
 * @endif
 */
typedef struct {
    uint32_t channels;          /*!< @if Eng Number of channels
                                   @else   声道数 @endif */
    uint32_t bit_depth;         /*!< @if Eng (PCM) Data depth, and format of storing the output data. \n
                                           If the data depth is 16 bits, 16-bit word memory is used. \n
                                           If the data depth is greater than 16 bits, 32-bit word memory is used, \n
                                           and data is stored as left-aligned data. (the valid data is at upper bits)
                                   @else   (PCM) 数据位宽设置. 输出存放格式 \n
                                           等于16bit: 占用16bit word内存 \n
                                           大于16bit: 占用32bit word内存, \n
                                           数据左对齐方式存放(有效数据在高位) @endif */
    uint32_t sample_rate;       /*!< @if Eng Sampling rate
                                   @else   采样率 @endif */
    void *private_data;    /*!< @if Eng Input, pointer to private data. \n
                                           If the decoder does not contain private data, this parameter is set to NULL.
                                   @else   IN 指向私有配置结构体的指针 \n
                                           如果解码器没有私有配置，设置为NULL @endif */
    uint32_t private_data_size; /*!< @if Eng Input, size of the private data. \n
                                           If the decoder does not contain private data, this parameter is set to 0.
                                   @else   IN 私有结构体大小 \n
                                           如果解码器没有私有配置，设置为0 @endif */
} uapi_acodec_dec_param;

/**
 * @if Eng
 * @brief  Parameters for creating an encoder
 * @else
 * @brief  编码器创建配置参数
 * @endif
 */
typedef struct {
    uint8_t interleaved;      /*!< @if Eng IN Interleave or not
                                   @else   交织模式 @endif */
    uint32_t channels;          /*!< @if Eng Number of channels
                                   @else   声道数 @endif */
    uint32_t bit_depth;         /*!< @if Eng (PCM) Data depth, and format of storing the output data. \n
                                           If the data depth is 16 bits, 16-bit word memory is used. \n
                                           If the data depth is greater than 16 bits, 32-bit word memory is used, \n
                                           and data is stored as left-aligned data. (the valid data is at upper bits)
                                   @else   (PCM) 数据位宽设置. 输出存放格式 \n
                                           等于16bit: 占用16bit word内存 \n
                                           大于16bit: 占用32bit word内存, \n
                                           数据左对齐方式存放(有效数据在高位) @endif */
    uint32_t sample_rate;       /*!< @if Eng Sampling rate
                                   @else   采样率 @endif */
    uint32_t samples_per_frame; /*!< @if Eng IN Pcm samples per frame for encoder
                                   @else   每帧采样点 @endif */
    void *private_data;    /*!< @if Eng Pointer of decoder private open parameters \n
                                           note: set NULL if there is no private parameters
                                   @else   解码器私有参数 \n
                                           注意: 若没有私有参数，设置为NULL @endif */
    uint32_t private_data_size; /*!< @if Eng Size of decoder private open parameters \n
                                           note: set to 0 if there is no private parameters
                                   @else   解码器私有参数大小 \n
                                           注意: 若没有私有参数，设置为0 @endif */
} uapi_acodec_enc_param;

/**
 * @if Eng
 * @brief  Private data struct for Vorbis decoder
 * @else
 * @brief  Vorbis解码器私有数据结构体
 * @endif
 */
typedef struct {
    uint8_t raw_file_flag;      /*!< @if Eng Decode in raw vorbis file mode
                                     @else   裸流解码模式 @endif */
} uapi_acodec_vorbis_dec_priv_cfg;

/**
 * @if Eng
 * @brief  Private data struct for opus decoder
 * @else
 * @brief  opus解码器私有数据结构体
 * @endif
 */
typedef struct {
    td_u32 frame_length; /*!< @if Eng frame length: #uapi_acodec_opus_frame_length
                              @else   帧长: #uapi_acodec_opus_frame_length @endif */
} uapi_acodec_opus_dec_priv_cfg;

#define UAPI_CODEC_LHDC_BT_NAME_MAX_SIZE 64
#define UAPI_CODEC_LHDC_UUID_MAX_SIZE    20
#define UAPI_CODEC_LHDC_LICENSE_MAX_SIZE 256

/**
 * @if Eng
 * @brief  Private data struct for LHDC decoder
 * @else
 * @brief  LHDC解码器私有数据结构体
 * @endif
 */
typedef struct {
    uint8_t bt_name[UAPI_CODEC_LHDC_BT_NAME_MAX_SIZE];
    uint32_t bt_name_bytes;
    uint8_t uuid[UAPI_CODEC_LHDC_UUID_MAX_SIZE];
    uint32_t uuid_bytes;
    uint8_t lhdc_license[UAPI_CODEC_LHDC_LICENSE_MAX_SIZE];
    uint32_t lhdc_license_bytes;
} uapi_acodec_lhdc_dec_priv_cfg;

typedef enum {
    UAPI_CODEC_L2HC_FRAME_LEN_5MS = 0x4,
    UAPI_CODEC_L2HC_FRAME_LEN_10MS = 0x8,
    UAPI_CODEC_L2HC_FRAME_LEN_MAX,
} uapi_acodec_l2hc_frame_len;

typedef struct {
    uint8_t protocol;
    uint8_t vendor_id;
    uint8_t version_id;
} uapi_acodec_l2hc_version;

/**
 * @if Eng
 * @brief  Private data struct for L2HC codec
 * @else
 * @brief  L2HC编解码器私有数据结构体
 * @endif
 */
typedef struct {
    uapi_acodec_l2hc_version version;     /*!< @if Eng L2HC decoder version
                                               @else   L2HC解码器版本 @endif */
    uint8_t aggregate_enable;             /*!< @if Eng Aggregate frame or not
                                               @else   聚合帧开关 @endif */
    uapi_acodec_l2hc_frame_len frame_len; /*!< @if Eng Time duration per frame
                                               @else   每帧时长 @endif */
    uint8_t max_plc_count;                /*!< @if Eng Max supported continuous PLC frame count
                                               @else   最大可支持的连续丢包补偿帧数 @endif */
    uint8_t max_plc_search_count;         /*!< @if Eng supported PLC search frame count
                                               @else   配置当前的搜索帧数 @endif */
} uapi_acodec_l2hc_priv_cfg;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SOC_UAPI_ACODEC_H__ */
