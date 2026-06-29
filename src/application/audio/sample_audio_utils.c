/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: sample audio utils
 * Author: audio
 * Create: 2021-07-27
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "securec.h"
#include "sample_audio_interface.h"
#ifdef DSP_FREQ_CTRL_SUPPORT
#include "pm_clocks.h"
#endif
#include "sample_audio_api.h"
#include "sample_audio_fops.h"
#include "sample_audio_utils.h"

td_char *strcasestr(const td_char *, const td_char *);

static const td_char *g_samplerate_signs[] = {
    "hz",
    "khz",
    "k_",
    "k.",
};

static const td_char *g_channel_signs[] = {
    "channel",
    "ch_",
    "ch.",
};

static const td_char *g_bit_signs[] = {
    "bit",
};

static const td_char *find_sign_pos_in_line(const td_char *line, const td_char *sign)
{
    td_char *sign_pos = strcasestr(line, sign);
    td_char *num_pos = TD_NULL;
    if (sign_pos == TD_NULL || sign_pos == line) {
        return TD_NULL;
    }

    num_pos = sign_pos - 1;
    while (num_pos >= line && (isdigit((td_s32)(*num_pos)) != 0 || *num_pos == '.')) {
        num_pos--;
    }
    num_pos++;

    return num_pos;
}

static double parse_sign_float_value(const td_char *line, const td_char *sign)
{
    const td_char *num_pos = find_sign_pos_in_line(line, sign);
    if (num_pos == TD_NULL) {
        return 0.0;
    }
    return strtod(num_pos, TD_NULL);
}

static td_s32 parse_sign_int_value(const td_char *line, const td_char *sign)
{
    const td_char *num_pos = find_sign_pos_in_line(line, sign);
    if (num_pos == TD_NULL) {
        return -1;
    }
    return (td_s32)strtol(num_pos, TD_NULL, 0);
}

static td_s32 parse_samplerate(const td_char *line)
{
    const size_t sign_num = sizeof(g_samplerate_signs) / sizeof(g_samplerate_signs[0]);
    td_u32 i;
    td_s32 samplerate;
    double vf;

    for (i = 0; i < sign_num; i++) {
        vf = parse_sign_float_value(line, g_samplerate_signs[i]);
        if (strcasestr(g_samplerate_signs[i], "k") != TD_NULL) {
            vf = vf * 1000; /* 1000 kHz to Hz */
        }
        samplerate = (td_s32)vf;
        if (samplerate > 0) {
            return samplerate;
        }
    }

    return -1;
}

static td_s32 parse_sign(const td_char *line, const td_char **signs, td_u32 sign_num)
{
    td_u32 i;
    td_s32 v;

    for (i = 0; i < sign_num; i++) {
        v = parse_sign_int_value(line, signs[i]);
        if (v > 0) {
            return v;
        }
    }

    return -1;
}

td_void parse_audio_params_from_file(const td_char *file, uapi_audio_pcm_format *pcm_format)
{
    td_s32 channels;
    td_s32 samplerate;
    td_s32 bit_depth;

    if (file == TD_NULL || pcm_format == TD_NULL) {
        return;
    }

    channels = parse_sign(file, g_channel_signs, sizeof(g_channel_signs) / sizeof(g_channel_signs[0]));
    samplerate = parse_samplerate(file);
    bit_depth = parse_sign(file, g_bit_signs, sizeof(g_bit_signs) / sizeof(g_bit_signs[0]));

    sap_printf("parsed from file = '%s': channels = %d, samplerate = %d, bit_depth = %d\n", file, channels, samplerate,
        bit_depth);

    if (channels <= 0 || samplerate <= 0 || bit_depth <= 0) {
        return;
    }

    pcm_format->sample_rate = (td_u32)samplerate;
    pcm_format->bit_depth = (td_u32)bit_depth;
    pcm_format->channels = (td_u32)channels;
}

#define MAX_PATH_LEN 128

static FILE *sample_audio_open_stream(const td_char *stream, const td_char *mode)
{
    td_u32 i;
    td_s32 ret;
    FILE *file = TD_NULL;
    const td_char *path_prefix[] = {
        "/user/",
        "/user/test/",
        "/user/stream/",
        "/music/",
        "/music/test/",
        "/music/stream/",
    };

    td_char path[MAX_PATH_LEN];

    file = sap_fopen(stream, mode);
    if (file != TD_NULL) {
        return file;
    }

    for (i = 0; i < sizeof(path_prefix) / sizeof(path_prefix[0]); i++) {
        ret = snprintf_s(path, sizeof(path) - 1, sizeof(path) - 1,
            "%s%s", path_prefix[i], stream);
        if (ret < 0) {
            sap_err_log_fun(snprintf_s, ret);
            return TD_NULL;
        }

        file = sap_fopen(path, mode);
        if (file != TD_NULL) {
            return file;
        }
    }

    return TD_NULL;
}

FILE *sample_audio_open_input_stream(const td_char *stream)
{
    return sample_audio_open_stream(stream, "rb");
}

FILE *sample_audio_open_output_stream(const td_char *stream)
{
    return sample_audio_open_stream(stream, "wb");
}

td_u32 calc_pcm_frame_size(const uapi_audio_pcm_format *pcm_fmt)
{
    size_t frame_size;

    if (pcm_fmt == TD_NULL) {
        return 0;
    }

    if (pcm_fmt->bit_depth == UAPI_AUDIO_BIT_DEPTH_16) {
        frame_size = sizeof(td_s16);
    } else {
        frame_size = sizeof(td_s32);
    }

    return frame_size * pcm_fmt->channels * pcm_fmt->sample_per_frame;
}

static td_u32 pcm_sample_size(td_u32 channels, td_u32 bit_depth)
{
    size_t depth_size;

    switch (bit_depth) {
        case UAPI_AUDIO_BIT_DEPTH_8:
            depth_size = sizeof(td_s8);
            break;

        case UAPI_AUDIO_BIT_DEPTH_16:
            depth_size = sizeof(td_s16);
            break;

        case UAPI_AUDIO_BIT_DEPTH_24:
        case UAPI_AUDIO_BIT_DEPTH_32:
            depth_size = sizeof(td_s32);
            break;

        default:
            return 0;
    }

    return depth_size * channels;
}

/* how many bytes per frame */
td_u32 calc_audio_frame_size(const uapi_audio_frame *frame)
{
    if (frame == TD_NULL) {
        return 0;
    }

    if (frame->channels < UAPI_AUDIO_CHANNEL_1 || frame->channels > UAPI_AUDIO_CHANNEL_16) {
        return 0;
    }

    return pcm_sample_size(frame->channels, frame->bit_depth) * frame->pcm_samples;
}

uapi_ai_port sample_audio_get_ai_port(const td_char *port_name)
{
    td_u32 i;
    const struct {
        const td_char *name;
        uapi_ai_port ai_port;
    } ai_port_name[] = {
        {"mad0", UAPI_AI_PORT_PDM0},
        {"pdm0", UAPI_AI_PORT_PDM0},
        {"pdm1", UAPI_AI_PORT_PDM1},
        {"i2s0", UAPI_AI_PORT_I2S0},
        {"i2s1", UAPI_AI_PORT_I2S1},
        {"i2s2", UAPI_AI_PORT_I2S2},
        {"i2s3", UAPI_AI_PORT_I2S3},
        {"adc0", UAPI_AI_PORT_ADC0},
        {"adc1", UAPI_AI_PORT_ADC1},
        {"lpadc0", UAPI_AI_PORT_LPADC0},
        {"lpadc1", UAPI_AI_PORT_LPADC1},
        {"anc0", UAPI_AI_PORT_ANC0},
    };

    for (i = 0; i < sizeof(ai_port_name) / sizeof(ai_port_name[0]); i++) {
        if (strcmp(ai_port_name[i].name, port_name) == 0) {
            return ai_port_name[i].ai_port;
        }
    }

    return AI_PORT_DEFAULT;
}

uapi_snd_out_port sample_audio_get_ao_port(const td_char *port_name)
{
    td_u32 i;
    const struct {
        const td_char *name;
        uapi_snd_out_port ao_port;
    } ao_port_name[] = {
        {"dac0", UAPI_SND_OUT_PORT_DAC0},
        {"dac1", UAPI_SND_OUT_PORT_DAC1},
        {"i2s0", UAPI_SND_OUT_PORT_I2S0},
        {"i2s1", UAPI_SND_OUT_PORT_I2S1},
        {"i2s2", UAPI_SND_OUT_PORT_I2S2},
        {"cast0", UAPI_SND_OUT_PORT_CAST0},
        {"all", UAPI_SND_OUT_PORT_ALL},
    };

    for (i = 0; i < sizeof(ao_port_name) / sizeof(ao_port_name[0]); i++) {
        if (strcmp(ao_port_name[i].name, port_name) == 0) {
            return ao_port_name[i].ao_port;
        }
    }

    return SND_OUT_PORT_DEFAULT;
}

typedef struct {
    const td_char *name;
    uapi_acodec_id acodec_id;
} acodec_type_map;

static acodec_type_map g_acodec_type_map[] = {
    {"pcm", UAPI_ACODEC_ID_PCM},
    {"mp3", UAPI_ACODEC_ID_MP3},
    {"aac", UAPI_ACODEC_ID_AAC},
    {"sbc", UAPI_ACODEC_ID_SBC},
    {"msbc", UAPI_ACODEC_ID_MSBC},
    {"opus", UAPI_ACODEC_ID_OPUS},
    {"flac", UAPI_ACODEC_ID_FLAC},
    {"l2hc", UAPI_ACODEC_ID_L2HC},
    {"l2hc_voice", UAPI_ACODEC_ID_L2HC_VOICE},
    {"lc3", UAPI_ACODEC_ID_LC3},
    {"silk", UAPI_ACODEC_ID_SILK},
    {"vorbis", UAPI_ACODEC_ID_VORBIS},
    {"alac", UAPI_ACODEC_ID_ALAC},
    {"amrwb", UAPI_ACODEC_ID_AMRWB},
    {"amrnb", UAPI_ACODEC_ID_AMRNB},
    {"ogg", UAPI_ACODEC_ID_VORBIS},
    {"lhdc", UAPI_ACODEC_ID_LHDC},
    {"ldac", UAPI_ACODEC_ID_LDAC},
};

td_s32 sample_audio_get_acodec_id(const td_char *name, uapi_acodec_id *acodec_id)
{
    td_u32 i;
    const size_t num = sizeof(g_acodec_type_map) / sizeof(acodec_type_map);

    for (i = 0; i < num; i++) {
        if (strcasecmp(name, g_acodec_type_map[i].name) == 0) {
            *acodec_id = g_acodec_type_map[i].acodec_id;
            return EXT_SUCCESS;
        }
    }

    *acodec_id = UAPI_ACODEC_ID_MAX;
    return EXT_FAILURE;
}

typedef struct {
    td_char *arg;
    uapi_audio_bit_depth bit_depth;
} pcm_bitdepth_map;

static const pcm_bitdepth_map g_pcm_bitdepth_map[] = {
    {"pcm_out16", UAPI_AUDIO_BIT_DEPTH_16},
    {"pcm_out24", UAPI_AUDIO_BIT_DEPTH_24},
    {"pcm_out32", UAPI_AUDIO_BIT_DEPTH_32},
    {"pcm_out32f", UAPI_AUDIO_BIT_DEPTH_32F},
};

td_s32 get_pcm_out_bitdepth(const td_char *opt, td_u32 *out_bit_depth)
{
    td_u32 i;

    if (opt == TD_NULL || out_bit_depth == TD_NULL) {
        sap_err_log_info("opt or out_bit_depth = NULL");
        return EXT_FAILURE;
    }

    for (i = 0; i < sizeof(g_pcm_bitdepth_map) / sizeof(pcm_bitdepth_map); ++i) {
        if (strcmp(opt, g_pcm_bitdepth_map[i].arg) == 0) {
            *out_bit_depth = (td_u32)g_pcm_bitdepth_map[i].bit_depth;
            return EXT_SUCCESS;
        }
    }
    return EXT_FAILURE;
}

#define I2S_MCLK_DEF 24576000 /* i2s mclk default 24.576M */
static uapi_audio_i2s_mclk_sel get_i2s_mclk_div(uapi_audio_sample_rate sample_rate)
{
    const td_u32 clk_div = I2S_MCLK_DEF / (td_u32)sample_rate;
    const struct {
        uapi_audio_i2s_mclk_sel mclk_div_e;
        td_u32 mclk_div;
    } div_tab[] = {
        {UAPI_AUDIO_I2S_MCLK_128_FS, 128}, /* 128 = mclk / fs */
        {UAPI_AUDIO_I2S_MCLK_256_FS, 256}, /* 256 = mclk / fs */
        {UAPI_AUDIO_I2S_MCLK_384_FS, 384}, /* 384 = mclk / fs */
        {UAPI_AUDIO_I2S_MCLK_512_FS, 512}, /* 512 = mclk / fs */
        {UAPI_AUDIO_I2S_MCLK_768_FS, 768}, /* 768 = mclk / fs */
        {UAPI_AUDIO_I2S_MCLK_1024_FS, 1024}, /* 1024 = mclk / fs */
        {UAPI_AUDIO_I2S_MCLK_1536_FS, 1536}, /* 1536 = mclk / fs */
    };

    sap_trace_log_u32(sample_rate);
    sap_trace_log_u32(clk_div);

    for (td_u32 i = 0; i < sizeof(div_tab) / sizeof(div_tab[0]); i++) {
        if (div_tab[i].mclk_div == clk_div) {
            return div_tab[i].mclk_div_e;
        }
    }

    return UAPI_AUDIO_I2S_MCLK_MAX;
}

static uapi_audio_i2s_bclk_sel get_i2s_bclk_div(uapi_audio_channel channels, uapi_audio_bit_depth bit_depth,
                                                uapi_audio_sample_rate sample_rate)
{
    td_u32 bclk_hz = (td_u32)sample_rate * (td_u32)bit_depth * (td_u32)channels;

    sap_trace_log_u32(bclk_hz);

    return (uapi_audio_i2s_bclk_sel)(I2S_MCLK_DEF / bclk_hz);
}

static uapi_audio_i2s_mode get_i2s_mode(uapi_audio_channel channels)
{
    uapi_audio_i2s_mode i2s_mode;

    /* Set I2S mode according to input channels */
    if (channels == UAPI_AUDIO_CHANNEL_1) {
        sap_alert_log_info("i2s_pcm mode");
        i2s_mode = UAPI_AUDIO_I2S_PCM_MODE;
    } else if (channels == UAPI_AUDIO_CHANNEL_2) {
        sap_alert_log_info("i2s_std mode");
        i2s_mode = UAPI_AUDIO_I2S_STD_MODE;
    } else {
        sap_alert_log_info("i2s_tdm mode");
        i2s_mode = UAPI_AUDIO_I2S_TDM_MODE;
    }

    sap_trace_log_u32(i2s_mode);

    return i2s_mode;
}

td_void get_i2s_attr(uapi_audio_i2s_attr *i2s_attr, const uapi_audio_pcm_format *pcm_fmt, td_bool local_out_port)
{
    i2s_attr->bit_depth = pcm_fmt->bit_depth;
    i2s_attr->channels = pcm_fmt->channels;
    /* Set I2S mode according to output channels */
    i2s_attr->i2s_mode = get_i2s_mode(pcm_fmt->channels);
    i2s_attr->mclk = get_i2s_mclk_div(pcm_fmt->sample_rate); /* The fs division from mclk */

    if (local_out_port == TD_TRUE) { /* Local output port : SmartPA Speaker */
        i2s_attr->master = SND_OUT_I2S_MST_DEF;
        i2s_attr->pcm_sample_rise_edge = SND_OUT_I2S_DATA_EDGE_DEF;
        i2s_attr->pcm_delay_cycle = UAPI_AUDIO_I2S_PCM_1_DELAY;
        i2s_attr->bclk = get_i2s_bclk_div(pcm_fmt->channels, SND_OUT_FRAME_MODE_WORD_LEN, pcm_fmt->sample_rate);
    } else { /* Cat1 port */
        i2s_attr->master = SIO_CAT1_I2S_MST_DEF;
        i2s_attr->pcm_sample_rise_edge = SIO_CAT1_I2S_DATA_EDGE_DEF;
        i2s_attr->pcm_delay_cycle = SIO_CAT1_I2S_PCM_DELAY_DEF;
        i2s_attr->bclk = get_i2s_bclk_div(pcm_fmt->channels, SIO_CAT1_FRAME_MODE_WORD_LEN, pcm_fmt->sample_rate);
    }

    sap_trace_log_u32(i2s_attr->mclk);
    sap_trace_log_u32(i2s_attr->bclk);
}

uapi_snd_aef_profile audio_scene_to_aef_profile(uapi_audio_scene audio_scene)
{
    switch (audio_scene) {
        case UAPI_AUDIO_SCENE_BT_MUSIC_SNK:
        case UAPI_AUDIO_SCENE_LOCAL_MUSIC:
            return UAPI_SND_AEF_PROFILE_MUSIC;

        case UAPI_AUDIO_SCENE_BT_VOIP:
            return UAPI_SND_AEF_PROFILE_VOIP;

        case UAPI_AUDIO_SCENE_LOCAL_TONE:
        case UAPI_AUDIO_SCENE_SYNC_TONE:
            return UAPI_SND_AEF_PROFILE_RINGTONE;

        default:
            return UAPI_SND_AEF_PROFILE_NONE;
    }
}

td_void sample_audio_set_hifi_mode(uapi_audio_scene audio_scene)
{
#ifdef DSP_FREQ_CTRL_SUPPORT
    switch (audio_scene) {
        case UAPI_AUDIO_SCENE_BT_MUSIC_SNK:
        case UAPI_AUDIO_SCENE_LOCAL_MUSIC:
        case UAPI_AUDIO_SCENE_BT_VOIP:
            uapi_set_hifi_mode(HIFI_LPM_CLK_LEVEL1); /* dsp:133Mhz, codec:24.576Mhz */
            return;

        case UAPI_AUDIO_SCENE_BT_MUSIC_SRC:
            uapi_set_hifi_mode(HIFI_LPM_CLK_LOW_LEVEL); /* dsp:64MHz(asic)/32Mhz(fpga), codec:32Mhz */
            return;

        default:
            uapi_set_hifi_mode(HIFI_LPM_CLK_HIGH_LEVEL); /* dsp:266Mhz, codec:24.576Mhz */
            return;
    }
#else
    audio_unused(audio_scene);
#endif
}

#define FLAC_HEADER_SIZE 4
#define FLAC_HEADER_BLOCK_TYPE_STREAMINFO 0
#define ID3V2_HEADER_SIZE 10
/* Default magic bytes for ID3v2 header: "ID3"  */
#define ID3V2_DEFAULT_MAGIC "ID3"

#define av_rb32(x)                                \
    (((uint32_t)((const uint8_t*)(x))[0] << 24) | \
               (((const uint8_t*)(x))[1] << 16) | \
               (((const uint8_t*)(x))[2] <<  8) | \
                ((const uint8_t*)(x))[3])

#define flac_tag(a, b, c, d) ((d) | ((c) << 8) | ((b) << 16) | ((unsigned)(a) << 24))

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int block_size : 24; /* 24 bits is metadata size */
        unsigned int block_type : 7;  /* 7 bits is metadata type */
        unsigned int last : 1;        /* 1 bits represent last metadata block */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
    unsigned char u8[4];
} flac_metadata_header;

static td_bool ff_id3v2_match(const uint8_t *buf, const char *magic)
{
    return buf[0] == magic[0] && /* 0 magic num */
        buf[1] == magic[1] &&    /* 1 magic num */
        buf[2] == magic[2] &&    /* 2 magic num */
        buf[3] != 0xff &&        /* 3 magic num */
        buf[4] != 0xff &&        /* 4 magic num */
        (buf[6] & 0x80) == 0 &&  /* 6 magic num */
        (buf[7] & 0x80) == 0 &&  /* 7 magic num */
        (buf[8] & 0x80) == 0 &&  /* 8 magic num */
        (buf[9] & 0x80) == 0;    /* 9 magic num */
}

static td_u32 ff_id3v2_tag_len(const uint8_t *buf, td_u32 size)
{
    td_u32 len;

    len = ((buf[6] & 0x7f) << 21) + /* 6 represent sixth byte, 21 is shift num */
        ((buf[7] & 0x7f) << 14) +   /* 7 represent seventh byte, 14 is shift num */
        ((buf[8] & 0x7f) << 7) +    /* 8 represent eighth byte, 7 is shift num */
        (buf[9] & 0x7f) +           /* 9 represent ninth byte */
        ID3V2_HEADER_SIZE;
    if ((buf[5] & 0x10) != 0x0) { /* 5 represent fifth byte */
        len += ID3V2_HEADER_SIZE;
    }
    sap_unused(size);
    return len;
}

static td_void skip_id3v2_tag(FILE *in)
{
    td_u32 tag_len;
    td_u32 read_len;
    td_u32 ret;
    uint8_t header[ID3V2_HEADER_SIZE];

    read_len = sap_fread(header, 1, sizeof(header), in);
    if (read_len != sizeof(header)) {
        sap_printf("read failed!");
        return;
    }

    if (!(ff_id3v2_match(header, ID3V2_DEFAULT_MAGIC))) {
        sap_rewind(in);
        return;
    }

    tag_len = ff_id3v2_tag_len(header, sizeof(header));
    tag_len -= ID3V2_HEADER_SIZE;
    ret = (td_u32)sap_fseek(in, (td_s32)tag_len, SEEK_CUR);
    if (ret != 0) {
        sap_printf("offset failed!");
    }
}

static td_void get_header(FILE *in, td_u8 *buf, td_u32 *buf_left_size)
{
    td_u32 tmp_size;
    td_u8 *header = buf;

    if (*buf_left_size < FLAC_HEADER_SIZE) {
        sap_err_log_u32(*buf_left_size);
        return;
    }

    tmp_size = sap_fread(header, 1, FLAC_HEADER_SIZE, in);
    if (tmp_size != FLAC_HEADER_SIZE) {
        sap_printf("read failed!");
        return;
    }

    if (av_rb32(header) != flac_tag('f', 'L', 'a', 'C')) {
        sap_printf("check fLaC header error");
        return;
    }
    *buf_left_size -= tmp_size;
}

static td_void get_streaminfo_from_metadata(FILE *in, td_u8 *buf, td_u32 *buf_left_size, flac_metadata_header *metadata)
{
    td_u32 ret;
    td_u32 tmp_size;
    td_u8 *tmp_buf = buf;
    flac_metadata_header tmp_metadata;

    if (*buf_left_size < FLAC_HEADER_SIZE) {
        sap_err_log_u32(*buf_left_size);
        return;
    }

    /* Get metadata header */
    tmp_size = sap_fread(tmp_buf, 1, FLAC_HEADER_SIZE, in);
    if (tmp_size != FLAC_HEADER_SIZE) {
        sap_printf("read failed!");
        return;
    }
    metadata->u32 = av_rb32(tmp_buf);
    /* check block type */
    if (metadata->bits.block_type != FLAC_HEADER_BLOCK_TYPE_STREAMINFO) {
        ret = (td_u32)sap_fseek(in, metadata->bits.block_size, SEEK_CUR); /* Skip this block */
        if (ret != 0) {
            sap_printf("offset failed!");
        }
        return;
    }
    /* Save metadata header of stream info block to buf */
    tmp_metadata.u32 = metadata->u32; /* use tmp var to edit last bit */
    tmp_metadata.bits.last = 0x1;
    tmp_buf[0] = tmp_metadata.u8[3]; /* 0 : index; 3 : index */
    tmp_buf[1] = tmp_metadata.u8[2]; /* 1 : index; 2 : index */
    tmp_buf[2] = tmp_metadata.u8[1]; /* 2 : index; 1 : index */
    tmp_buf[3] = tmp_metadata.u8[0]; /* 3 : index; 0 : index */
    *buf_left_size -= tmp_size;
    tmp_buf += tmp_size;

    if (*buf_left_size < tmp_metadata.bits.block_size) {
        sap_err_log_u32(*buf_left_size);
        sap_err_log_u32(tmp_metadata.bits.block_size);
        return;
    }

    /* Save stream info block to buf */
    tmp_size = sap_fread(tmp_buf, 1, tmp_metadata.bits.block_size, in);
    if (tmp_size != tmp_metadata.bits.block_size) {
        sap_printf("read failed!");
        return;
    }
    *buf_left_size -= tmp_size;
    tmp_buf += tmp_size;
}

static td_void demux_metadata(FILE *in, td_u8 *buf, td_u32 *buf_left_size)
{
    td_u32 tmp_size;
    td_u8 *tmp_buf = buf;
    flac_metadata_header metadata = { 0 };

    /* Save fLaC header to buf */
    tmp_size = *buf_left_size;
    get_header(in, tmp_buf, buf_left_size);
    tmp_size -= *buf_left_size;
    tmp_buf += tmp_size;

    /* Get stream info block to buf && Skip other block */
    do {
        tmp_size = *buf_left_size;
        get_streaminfo_from_metadata(in, tmp_buf, buf_left_size, &metadata);
        tmp_size -= *buf_left_size;
        tmp_buf += tmp_size;
    } while (metadata.bits.last == 0x0); /* Check last block from metadata header */
}

td_void flac_demux_header(FILE *in, td_u8 *buf, td_u32 *buf_left_size)
{
    skip_id3v2_tag(in);
    demux_metadata(in, buf, buf_left_size);
}

#define MIN_ARG_NUM 2
#define INDEX_1 1
#define INDEX_2 2

static td_bool is_argv_end(const parg_state *ps, int argc, char *const argv[])
{
    return (ps->optind >= argc || argv[ps->optind] == NULL) ? TD_TRUE : TD_FALSE;
}

static td_s32 match_short(parg_state *ps, int argc, char *const argv[], const char *optstring)
{
    const char *p = strchr(optstring, *ps->nextchar);

    if (p == NULL) {
        ps->optopt = *ps->nextchar++;
        return '?';
    }

    /* if no option argument, return option */
    if (p[INDEX_1] != ':') {
        return *ps->nextchar++;
    }

    /* if more characters, return as option argument */
    if (ps->nextchar[INDEX_1] != '\0') {
        ps->optarg = &ps->nextchar[1];
        ps->nextchar = NULL;
        return *p;
    }

    /* if option argument is optional, return option */
    if (p[INDEX_2] == ':') {
        return *ps->nextchar++;
    }

    /* option argument required, so return next argv element */
    if (is_argv_end(ps, argc, argv)) {
        ps->optopt = *ps->nextchar++;
        return (optstring[0] == ':') ? ':' : '?';
    }

    ps->optarg = argv[ps->optind++];
    ps->nextchar = NULL;
    return *p;
}

td_void parg_init(parg_state *ps)
{
    ps->optarg = NULL;
    ps->optind = 1;
    ps->optopt = '?';
    ps->nextchar = NULL;
}

td_s32 parg_getopt(parg_state *ps, int argc, td_char *const argv[], const td_char *optstring)
{
    if (ps == NULL || argv == NULL || optstring == NULL) {
        return -1;
    }

    if (argc < MIN_ARG_NUM) {
        return -1;
    }
    ps->optarg = NULL;

    /* advance to next element if needed */
    if (ps->nextchar == NULL || *ps->nextchar == '\0') {
        if (is_argv_end(ps, argc, argv)) {
            return -1;
        }

        ps->nextchar = argv[ps->optind++];

        /* check for nonoption element (including '-') */
        if (ps->nextchar[0] != '-' || ps->nextchar[1] == '\0') {
            ps->optarg = ps->nextchar;
            ps->nextchar = NULL;
            return 1;
        }

        /* check for '--' */
        if (ps->nextchar[INDEX_1] == '-') {
            if (ps->nextchar[INDEX_2] == '\0') {
                ps->nextchar = NULL;
                return -1;
            }
        }
        ps->nextchar++;
    }

    /* match nextchar */
    return match_short(ps, argc, argv, optstring);
}
