/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: bmic rx sample
 * Author: audio
 */

#ifndef __SAMPLE_BMIC_RX_H__
#define __SAMPLE_BMIC_RX_H__

#include "thread_os.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"

#define MAX_PIPELINE_NUM 4

typedef struct sample_bmic_uac_pipeline sample_bmic_rx_uac_pipeline;
typedef struct sample_bmic_pipeline sample_bmic_rx_pipeline;
typedef struct sample_bmic_rx sample_bmic_rx_inst;

struct sample_bmic_uac_pipeline {
    td_bool task_active;
    athread_handle task;

    td_handle h_aenc;
    td_handle h_adp;

    sample_acodec_arg acodec_arg;
};

struct sample_bmic_pipeline {
    td_bool task_active;
    athread_handle task;

    td_u32 stream_addr;
    td_u32 stream_size;

    td_handle h_adp;
    td_handle h_adec;
    td_handle h_track;

    sample_acodec_arg acodec_arg;

    uapi_audio_scene_config asc;
};

struct sample_bmic_rx {
    td_bool is_running;
    td_bool uac_enable;

    td_handle h_snd;
    uapi_snd snd_id;
    uapi_snd_attr snd_attr;

    sample_bmic_rx_pipeline pipelines[MAX_PIPELINE_NUM];
    sample_bmic_rx_uac_pipeline uac_pipeline;

    td_handle h_linein_ai;
    td_handle h_linein_adp;
};

sample_bmic_rx_inst *sample_bmic_rx_get_inst(td_void);

td_s32 sample_bmic_rx_run(sample_bmic_rx_inst *inst, td_bool uac_enable);

td_s32 sample_bmic_rx_show_pipelines(sample_bmic_rx_inst *inst);

td_s32 sample_bmic_rx_add(sample_bmic_rx_inst *inst);

td_s32 sample_bmic_rx_del(sample_bmic_rx_inst *inst, td_u32 pipeline_no);

td_s32 sample_bmic_rx_quit(sample_bmic_rx_inst *inst);

td_s32 sample_bmic_rx_drc_enable(uapi_snd_out_port port, td_bool enable);

td_s32 sample_bmic_rx_linein(sample_bmic_rx_inst *inst, td_bool enable);

td_s32 sample_bmic_rx_mute(uapi_snd_out_port port, td_bool mute);

#endif /* __SAMPLE_BMIC_RX_H__ */
