/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: header file for audio sync
 * Author: audio
 */

#ifndef __SYNC_STRUCT_H__
#define __SYNC_STRUCT_H__

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_u32 total_tog_bias;
    td_u32 cur_tog_bias;
    td_u32 max_tog_bias;
    td_u32 min_tog_bias;
    td_u32 avg_tog_bias;
} tog_bias_block;

typedef struct {
    /* sync_status */
    td_u32 sync_state;
    td_bool slave_start_sync;
    td_bool auto_speed_sync;
    td_u8 audio_scene;
    td_u32 tws_scene;

    /* sync paket status */
    td_u32 pkt_cnt;
    td_u32 pcm_in_samples;
    td_u32 pcm_out_samples;
    
    /* sync sink status */
    td_u32 dma_count;
    td_u32 dma_ts;
    td_u32 expected_ts;

    /* sync delay */
    td_u32 max_pcm_delay;
    td_u32 min_pcm_delay;
    td_u32 max_packet_delay;
    td_u32 min_packet_delay;
    td_u32 max_delay;
    td_u32 min_delay;

    /* tog_bias_status */
    td_u32 irq_count;
    tog_bias_block tog_bias;
} sync_proc_item;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* __SYNC_STRUCT_H__ */
