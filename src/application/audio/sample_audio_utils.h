/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: sample audio utils
 * Author: audio
 * Create: 2021-07-27
 */

#ifndef __SAMPLE_AUDIO_UTILS_H__
#define __SAMPLE_AUDIO_UTILS_H__

#include <stdio.h>
#include "td_type.h"
#include "soc_uapi_ai.h"
#include "soc_uapi_audio.h"
#include "soc_uapi_acodec.h"
#include "soc_uapi_sound.h"
#include "sample_audio_fops.h"

typedef struct {
    const char *optarg;   /* pointer to option argument, if any */
    int optind;           /* next index in argv to process */
    int optopt;           /* option value resulting in error, if any */
    const char *nextchar; /* next character to process */
} parg_state;

td_void parg_init(parg_state *ps);

td_s32 parg_getopt(parg_state *ps, int argc, td_char *const argv[], const td_char *optstring);

td_void parse_audio_params_from_file(const td_char *file, uapi_audio_pcm_format *pcm_format);

FILE *sample_audio_open_input_stream(const td_char *stream);
FILE *sample_audio_open_output_stream(const td_char *stream);

td_u32 calc_pcm_frame_size(const uapi_audio_pcm_format *pcm_fmt);

td_u32 calc_audio_frame_size(const uapi_audio_frame *frame);

uapi_ai_port sample_audio_get_ai_port(const td_char *port_name);
uapi_snd_out_port sample_audio_get_ao_port(const td_char *port_name);

td_s32 sample_audio_get_acodec_id(const td_char *name, uapi_acodec_id *acodec_id);
td_s32 get_pcm_out_bitdepth(const td_char *opt, td_u32 *out_bit_depth);

td_void get_i2s_attr(uapi_audio_i2s_attr *i2s_attr, const uapi_audio_pcm_format *pcm_fmt, td_bool local_out_port);

uapi_snd_aef_profile audio_scene_to_aef_profile(uapi_audio_scene audio_scene);

td_void flac_demux_header(FILE *in, td_u8 *buf, td_u32 *buf_left_size);

#endif
