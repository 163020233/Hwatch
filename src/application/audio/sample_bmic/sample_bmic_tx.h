/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: bmic tx sample
 * Author: audio
 */

#ifndef __SAMPLE_BMIC_TX_H__
#define __SAMPLE_BMIC_TX_H__

#include "thread_os.h"
#include "soc_uapi_ai.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"

typedef struct sample_bmic_tx sample_bmic_tx_inst;
typedef struct sample_bmic_rec sample_bmic_rec_inst;

struct sample_bmic_tx {
    td_bool is_running;
    td_bool sea_enable;

    td_handle h_ai;
    td_handle h_sea;
    td_handle h_aenc;
    td_handle h_adp;

    uapi_ai_port ai_port;
    uapi_ai_gain volume;

    uapi_acodec_id acodec_id;
    uapi_audio_pcm_format pcm_format;

    uapi_audio_scene audio_scene;
    sample_bmic_rec_inst *rec_inst;

    td_bool is_listening;
    td_handle h_track;
    td_handle h_snd;
    uapi_snd snd_id;
    uapi_snd_attr snd_attr;
};

struct sample_bmic_rec {
    td_bool is_running;
    td_bool sea_enable;

    td_bool task_active;
    athread_handle task;

    td_handle h_ai;
    td_handle h_sea;
    td_handle h_aenc;
    td_handle h_adp;

    uapi_ai_port ai_port;
    uapi_ai_gain volume;

    uapi_acodec_id acodec_id;
    uapi_audio_pcm_format pcm_format;
    uapi_audio_pcm_format pcm_out_format;

    uapi_audio_scene audio_scene;
    sample_bmic_tx_inst *tx_inst;

    FILE *h_file;
    td_char save_path[FILE_PATH_LEN];
    td_u64 save_cur_size;
    td_u32 write_count;
    td_u32 data_source;

    td_bool attach_flag;
    td_bool dump_sea_inout;
};

sample_bmic_tx_inst *sample_bmic_tx_get_inst(td_void);

td_s32 sample_bmic_tx_run(sample_bmic_tx_inst *inst, td_bool sea_enable, td_u32 frame_len_ms);

td_s32 sample_bmic_tx_quit(sample_bmic_tx_inst *inst);

sample_bmic_rec_inst *sample_bmic_rec_get_inst(td_void);

td_void sample_bmic_rec_inst_init(sample_bmic_rec_inst *inst);

td_s32 sample_bmic_rec_run(sample_bmic_rec_inst *inst, td_bool sea_enable, td_u32 frame_len_ms);

td_s32 sample_bmic_rec_attach(sample_bmic_rec_inst *inst, td_handle h_mod, td_bool sea_enable);

td_s32 sample_bmic_rec_sea_inout(sample_bmic_rec_inst *inst);

td_s32 sample_bmic_tx_start_listening(sample_bmic_tx_inst *inst);

td_s32 sample_bmic_rec_quit(sample_bmic_rec_inst *inst);

#endif /* __SAMPLE_BMIC_TX_H__ */
