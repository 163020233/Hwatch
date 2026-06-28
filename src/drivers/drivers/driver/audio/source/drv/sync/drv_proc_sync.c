/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: source file for drv sync proc
 * Author: audio
 */

#include "drv_proc_sync.h"
#include "audio_debug.h"
#include "audio_osal.h"
#include "drv_audio_proc.h"
#include "drv_audio_osal.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SAP_PROC_SUPPORT
static td_void drv_sync_proc_show_sync_status(td_void *file, const sync_proc_item *proc)
{
    drv_audio_proc_print_title(file, "sync_status");

    /* line 1 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10u |" "%-16s: %-10s |" "%-16s: %-10s |" "%-16s: %-10u |" "\n",
        "sync_state", proc->sync_state,
        "slave_start_sync", drv_audio_proc_get_bool(proc->slave_start_sync),
        "auto_speed_sync", drv_audio_proc_get_bool(proc->auto_speed_sync),
        "audio_scene", proc->audio_scene);

    /* line 2 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10u |" "%-16s: %-10s |" "%-16s: %-10s |" "%-16s: %-10s |" "\n",
        "tws_scene", proc->tws_scene,
        "", "",
        "", "",
        "", "");
}

static td_void drv_sync_proc_show_sync_packet_status(td_void *file, const sync_proc_item *proc)
{
    drv_audio_proc_print_title(file, "sync packet status");

    /* line 1 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10u |" "%-16s: %-10u |" "%-16s: %-10u |" "%-16s: %-10s |" "\n",
        "pkt_cnt", proc->pkt_cnt,
        "pcm_in_samples", proc->pcm_in_samples,
        "pcm_out_samples", proc->pcm_out_samples,
        "", "");
}

static td_void drv_sync_proc_show_sync_sink_status(td_void *file, const sync_proc_item *proc)
{
    drv_audio_proc_print_title(file, "sync sink status");

    /* line 1 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10u |" "%-16s: %-10u |" "%-16s: %-10u |" "%-16s: %-10s |" "\n",
        "dma_count", proc->dma_count,
        "dma_ts", proc->dma_ts,
        "expected_ts", proc->expected_ts,
        "", "");
}

static td_void drv_sync_proc_show_sync_delay(td_void *file, const sync_proc_item *proc)
{
    drv_audio_proc_print_title(file, "sync delay");

    /* line 1 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10u |" "%-16s: %-10u |" "%-16s: %-10u |" "%-16s: %-10u |" "\n",
        "max_pcm_delay", proc->max_pcm_delay,
        "min_pcm_delay", proc->min_pcm_delay,
        "max_delay", proc->max_delay,
        "min_delay", proc->min_delay);

    /* line 2 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10u |" "%-16s: %-10u |" "%-16s: %-10s |" "%-16s: %-10s |" "\n",
        "max_packet_delay", proc->max_packet_delay,
        "min_packet_delay", proc->min_packet_delay,
        "", "",
        "", "");
}

static td_void drv_sync_proc_show_tog_bias_status(td_void *file, const sync_proc_item *proc)
{
    drv_audio_proc_print_title(file, "tog_bias_status");

    /* line 1 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10d |" "%-16s: %-10s |" "%-16s: %-10s |" "%-16s: %-10s |" "\n",
        "irq_count", proc->irq_count,
        "", "",
        "", "",
        "", "");

    /* line 2 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "\n",
        "cur_tog_bias", proc->tog_bias.cur_tog_bias,
        "max_tog_bias", proc->tog_bias.max_tog_bias,
        "min_tog_bias", proc->tog_bias.min_tog_bias,
        "avg_tog_bias", proc->tog_bias.avg_tog_bias);
}

td_s32 drv_proc_sync_show(td_void *file, const sync_proc_item *proc)
{
    drv_audio_proc_print_head(file, "sync");

    /* sync_status */
    drv_sync_proc_show_sync_status(file, proc);

    /* sync_packet_status */
    drv_sync_proc_show_sync_packet_status(file, proc);

    /* sync_sink_status */
    drv_sync_proc_show_sync_sink_status(file, proc);

    /* sync_delay */
    drv_sync_proc_show_sync_delay(file, proc);

    /* tog_bias_status */
    drv_sync_proc_show_tog_bias_status(file, proc);

    return AUDIO_SUCCESS;
}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
