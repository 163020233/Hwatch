/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: source file for module operation
 */

#ifndef _SAMPLE_AUDIO_API_H_
#define _SAMPLE_AUDIO_API_H_

#include "soc_uapi_adec.h"
#include "soc_uapi_aenc.h"
#include "soc_uapi_sound.h"
#include "soc_uapi_sea.h"
#include "circ_buf.h"
#include "cmsis_os.h"
#include "audio_debug.h"
#include "tcxo.h"
#include "systick.h"

#ifdef BRANDY_PROT
#define sap_printf
#else
#include "sample_audio_interface.h"
#endif /* SAP_CHIP_TYPE */

#define EXT_SUCCESS 0
#define EXT_FAILURE (-1)

#define sap_msleep(ms) osDelay(ms) /* osDelay(1) : 1ms */
#define sap_udelay uapi_tcxo_delay_us
#define sap_mdelay uapi_tcxo_delay_ms
#ifndef SINGLE_CORE_DEPLOY
#define sap_get_ms uapi_systick_get_ms
#else
#define sap_get_ms uapi_tcxo_get_ms
#endif
#define THREAD_SLEEP_5MS 5
#define THREAD_SLEEP_10MS 10
#define THREAD_HEART_BEAT_DUR 100
#define FILE_READ_LEN 0x400 /* 1K */
#define FILE_READ_LEN_MIN 0x80 /* 128B */

#define FILE_PATH_LEN 256
#define FILE_STREAM_PATH "/user/stream"

#define BITS_NUM_PER_BYTE 8
#define HZ_NUM_PER_KHZ 1000
#define MS_NUM_PER_S   1000

#define AI_VOLUME_DEFAULT 15
#define BT_A2DP_MTU_SIZE 1005
#define BT_SCO_FRAME_SIZE 60

/* I2S for CAT1 default i2s attr */
#define SIO_CAT1_I2S_SAMPLE_RATE_DEF UAPI_AUDIO_SAMPLE_RATE_16K
#define SIO_CAT1_I2S_CHANNEL_DEF     UAPI_AUDIO_CHANNEL_2
#define SIO_CAT1_I2S_BIT_DEPTH_DEF   UAPI_AUDIO_BIT_DEPTH_16
#define SIO_CAT1_I2S_PCM_DELAY_DEF   UAPI_AUDIO_I2S_PCM_0_DELAY
#define SIO_CAT1_I2S_MST_DEF         TD_TRUE /* TD_TRUE:master;TD_FALSE:slave */
#define SIO_CAT1_I2S_DATA_EDGE_DEF   TD_TRUE /* TD_TRUE:Rising;TD_FALSE:Falling */
#define SIO_CAT1_FRAME_MODE_WORD_LEN UAPI_AUDIO_BIT_DEPTH_16 /* 16:for pcm16/i2s32, 32:for pcm32/i2s64 */

#define sap_min(a, b) ((a) < (b) ? (a) : (b))
#define sap_max(a, b) ((a) > (b) ? (a) : (b))

#define sap_unused(var) \
    do { \
        (void)(var); \
    } while (0)

#define clear_obj(obj) ((*(obj)) = (__typeof__(*(obj))) { 0 })

typedef enum {
    GUI_AEF_AO,
    GUI_AEF_SEA,
    GUI_AEF_HAID,
    GUI_AEF_MAX,
} gui_aef_type;

typedef struct {
    gui_aef_type type;
    td_handle handle;
} gui_aef_str;

typedef struct {
    td_u8 *buf;
    td_u32 size;
    td_u32 read;
    td_u32 write;
    circ_buf cb;
} data_info;

static inline td_void data_info_init(data_info *d, td_u32 buf, td_u32 size)
{
    d->buf = (td_u8 *)(td_uintptr_t)buf;
    d->size = size;
    d->read = 0;
    d->write = 0;
    circ_buf_init(&d->cb, &d->write, &d->read, d->buf, d->size);
}

typedef union {
    uapi_acodec_vorbis_dec_priv_cfg vorbis_priv_cfg;
    uapi_acodec_opus_dec_priv_cfg opus_priv_cfg;
    uapi_acodec_lhdc_dec_priv_cfg lhdc_priv_cfg;
} sample_acodec_dec_priv_cfg;

/* SBC codec info */
typedef union {
    struct {
        unsigned int chnl_mode : 4;        /* [3..0] */
        unsigned int sample_frequency : 4; /* [7..4] */
        unsigned int alloc_method : 2;     /* [9..8] */
        unsigned int subband : 2;          /* [11..10] */
        unsigned int block_length : 4;     /* [15..12] */
        unsigned int min_bitpool : 8;      /* [23..16] */
        unsigned int max_bitpool : 8;      /* [31..24] */
    } bits;

    unsigned int u32;
} a2dp_sbc_codec_caps;

/* Channel modes */
typedef enum {
    A2DP_SBC_CHANNELS_JOINT_STEREO = 1,
    A2DP_SBC_CHANNELS_STEREO = 2,
    A2DP_SBC_CHANNELS_DUAL = 4,
    A2DP_SBC_CHANNELS_MONO = 8,
} a2dp_sbc_channels;

/* Sampling Frequency */
typedef enum {
    A2DP_SBC_SAMPLE_FREQ_48K = 1,
    A2DP_SBC_SAMPLE_FREQ_44K = 2,
    A2DP_SBC_SAMPLE_FREQ_32K = 4,
    A2DP_SBC_SAMPLE_FREQ_16K = 8,
} a2dp_sbc_sample_freq;

/* Bit allocation methods */
typedef enum {
    A2DP_SBC_ALLOC_METHOD_LOUDNESS = 1,
    A2DP_SBC_ALLOC_METHOD_SNR = 2,
} a2dp_sbc_alloc_method;

/* Sub_bands */
typedef enum {
    A2DP_SBC_SUB_BANDS_8 = 1,
    A2DP_SBC_SUB_BANDS_4 = 2,
} a2dp_sbc_sub_bands;

/* Block lengths */
typedef enum {
    A2DP_SBC_BLOCKS_16 = 1,
    A2DP_SBC_BLOCKS_12 = 2,
    A2DP_SBC_BLOCKS_08 = 4,
    A2DP_SBC_BLOCKS_04 = 8,
} a2dp_sbc_blocks;

typedef struct {
    const td_char *file;
    uapi_acodec_id acodec_id;
    uapi_audio_pcm_format pcm_format;
    td_bool dnlink_sea_en;
    sample_acodec_dec_priv_cfg dec_priv_cfg;
} sample_acodec_arg;

typedef struct {
    td_handle *player;               /* cast_player input adp */
    td_handle *track;               /* cast_player input track */
    const sample_acodec_arg *in_arg; /* cast player input es attr */
    td_handle *snd;                  /* cast_player output track */
    const uapi_snd_attr *snd_attr; /* cast player output pcm attr */
    uapi_audio_scene audio_scene;
    FILE *in_file;
    td_u32 in_data_addr;
    td_u32 in_data_size;
    uapi_snd_track_mode track_mode;
    td_bool run_dsp_loopback;
} sample_escast_arg;

td_s32 sample_ao(td_s32 argc, td_char *argv[]);
td_s32 sample_aef(td_s32 argc, td_char *argv[]);
td_s32 sample_effect(td_s32 argc, td_char *argv[]);
td_s32 sample_ai(td_s32 argc, td_char *argv[]);
td_s32 sample_asr(td_s32 argc, td_char *argv[]);
td_s32 sample_vad_feed(td_s32 argc, td_char *argv[]);
td_s32 sample_encode(td_s32 argc, td_char *argv[]);
td_s32 sample_adec(td_s32 argc, td_char *argv[]);
td_s32 sample_decode(td_s32 argc, td_char *argv[]);

td_s32 sample_esplay_open(const sample_acodec_arg *acodec_arg, const td_handle *player,
                          uapi_audio_scene audio_scene);
td_void sample_esplay_close(td_void);

td_s32 sample_escast_open(sample_escast_arg *escast_arg);
td_void sample_escast_close(td_void);

td_s32 sample_ao_player_open(sample_escast_arg *escast_arg, uapi_snd snd_id, td_bool codec_test);
td_void sample_ao_player_close(td_void);

td_s32 sample_cast_aenc_play(td_s32 argc, td_char *argv[]);
td_s32 sample_encode_play(td_s32 argc, td_char *argv[]);
td_s32 sample_ai_aenc(td_s32 argc, td_char *argv[]);
td_s32 sample_ai_sea_aenc(td_s32 argc, td_char *argv[]);
td_s32 sample_ai_ao(td_s32 argc, td_char *argv[]);
td_s32 sample_ai_sea_ao(td_s32 argc, td_char *argv[]);
td_s32 sample_sea(td_s32 argc, td_char *argv[]);
td_s32 sample_sea_only(td_s32 argc, td_char *argv[]);
td_s32 sample_gui_aef(td_s32 argc, td_char *argv[]);

td_s32 sample_haid(td_s32 argc, td_char *argv[]);
td_void sample_gui_haid_register_param(td_void *params);

td_s32 sample_bmic(td_s32 argc, td_char *argv[]);

td_s32 sample_dpm(td_s32 argc, td_char *argv[]);

td_s32 sample_test_int(td_s32 argc, td_char *argv[]);

td_s32 sample_lpwk(td_s32 argc, td_char *argv[]);
td_s32 sample_phone_application(td_s32 argc, td_char *argv[]);

td_s32 sample_dump(td_s32 argc, td_char *argv[]);

td_s32 sample_rms(td_s32 argc, td_char *argv[]);

td_s32 sample_proc(td_s32 argc, td_char *argv[]);

td_s32 sample_aha(td_s32 argc, td_char *argv[]);

td_s32 sample_sle_music(td_s32 argc, td_char *argv[]);

td_s32 sample_tts(td_s32 argc, td_char *argv[]);

td_u32 sample_get_vector_size(td_void);

td_void sample_gui_aef_add_handle(gui_aef_type type, td_handle handle);
td_void sample_gui_aef_delete_handle(td_handle handle);
td_handle sample_gui_aef_get_handle(gui_aef_type type);

const td_u16 *sample_get_vector_16k_1ch_16bit(td_void);

td_void sample_audio_set_hifi_mode(uapi_audio_scene audio_scene);

td_s32 sample_audio_w4(td_s32 argc, td_char *argv[]);

td_s32 sample_audio_mem32(td_s32 argc, td_char *argv[]);

#endif
