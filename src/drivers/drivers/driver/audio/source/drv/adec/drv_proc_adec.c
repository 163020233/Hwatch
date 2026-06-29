/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: source file for drv adec proc
 * Author: audio
 * Create: 2021-06-18
 */

#include "drv_proc_adec.h"
#include "audio_struct.h"
#include "audio_osal.h"
#include "audio_debug.h"
#include "drv_audio_proc.h"
#include "drv_audio_osal.h"
#include "core_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef SAP_PROC_SUPPORT
static td_void drv_adec_proc_show_attr(td_void *file, const adec_proc_item *proc)
{
    drv_audio_proc_print_title(file, "attr");

    /* line 1 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10s |" "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "\n",
        "acodec name", drv_audio_proc_get_acodec_name(proc->codec_id),
        "in_buf_dur_ms", proc->ibuf_dur_ms,
        "out_buf_dur_ms", proc->obuf_dur_ms,
        "plc_off_thresh", proc->plc_off_threshold);
}

static td_void drv_adec_proc_show_out_pcm_format(td_void *file, const adec_proc_item *proc)
{
    drv_audio_proc_print_title(file, "out pcm format");

    drv_audio_osal_proc_print(file,
        "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "\n",
        "channels", proc->pcm_attr.channels,
        "bit_depth", proc->pcm_attr.bit_depth,
        "sample_rate", proc->pcm_attr.sample_rate,
        "pcm_samples", proc->pcm_attr.samples_per_frame);
}

static td_void drv_adec_proc_show_basic_status(td_void *file, td_handle adec, const adec_proc_item *proc)
{
    drv_audio_proc_print_title(file, "basic status");

    /* line 1 */
    drv_audio_osal_proc_print(file,
        "%-16s: 0x%08x |" "%-16s: %-10s |" "%-16s: %-10s |" "%-16s: %-10u |" "\n",
        "handle", adec,
        "state", drv_audio_proc_get_state((audio_state)(proc->state)),
        "scene_type", drv_audio_proc_get_scene_name(proc->asc.scene_type),
        "scene_id", proc->asc.scene_id);

    /* line 2 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10s |" "%-16s: %-10s |" "%-16s: %-10d |" "%-16s: %-10d |" "\n",
        "bypass", drv_audio_proc_get_bool(proc->bypass),
        "eos", drv_audio_proc_get_bool(proc->eos),
        "frame_pms", proc->frame_pms,
        "frame_size", proc->frame_size);

    /* line 3 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "\n",
        "max_pcm_out_size", proc->max_pcm_out_size,
        "frame_ms", calc_pcm_frame_ms(&proc->pcm_attr, proc->frame_size),
        "soft_gain_int", proc->vol_cfg.integer,
        "soft_gain_dec", proc->vol_cfg.decimal);
}

static td_void drv_adec_proc_show_run_status(td_void *file, const adec_proc_item *proc)
{
    drv_audio_proc_print_title(file, "run status");

    /* line 1 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10u |" "%-16s: %-10u |" "%-16s: %-10u |" "%-16s: %-10u |" "\n",
        "frame_cnt", proc->frame_cnt,
        "proc_data_size", proc->proc_data_size,
        "underrun_cnt", proc->not_enough_data_cnt,
        "pkg_loss_cnt", proc->pkg_loss_cnt);

    /* line 2 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10u |" "%-16s: %-10u |" "%-16s: %-10u |" "%-16s: %-10u |" "\n",
        "underflow_cnt", proc->underflow_cnt,
        "acquire_fail_cnt", proc->acquire_fail_cnt,
        "dec_fail_cnt", proc->dec_fail_cnt,
        "dec_success_cnt", proc->dec_success_cnt);

    /* line 3 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10u |" "%-16s: %-10u |" "%-16s: %-10u |" "%-16s: %-10u |" "\n",
        "release_fail_cnt", proc->release_fail_cnt,
        "output_fail_cnt", proc->output_fail_cnt,
        "info_change_cnt", proc->info_change_cnt,
        "change_fail_cnt", proc->change_fail_cnt);

    /* line 4 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10u |" "%-16s: %-10u |" "%-16s: %-10s |" "%-16s: %-10u |" "\n",
        "eos_fail_cnt", proc->eos_fail_cnt,
        "eos_success_cnt", proc->eos_success_cnt,
        "packet_decode", drv_audio_proc_get_bool(proc->packet_decode),
        "bit_rate", proc->bit_rate);
}

static td_void drv_adec_proc_show_spend_status(td_void *file, const adec_proc_item *proc)
{
    drv_audio_proc_print_title(file, "spend status");

    /* line 1 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "\n",
        "cur_dec_interval", proc->cur_dec_interval,
        "max_dec_interval", proc->max_dec_interval,
        "min_dec_interval", proc->min_dec_interval,
        "avg_dec_interval", proc->avg_dec_interval);

    /* line 2 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "\n",
        "cur_dec_consume", proc->cur_dec_consume,
        "max_dec_consume", proc->max_dec_consume,
        "min_dec_consume", proc->min_dec_consume,
        "avg_dec_consume", proc->avg_dec_consume);
}

static td_void drv_adec_proc_show_in_buf(td_void *file, const audio_ring_buf *rbuf_in)
{
    td_u32 data_size;
    td_u32 data_ratio;

    data_size = drv_audio_proc_query_rbuf_busy(rbuf_in->size, rbuf_in->read_pos, rbuf_in->write_pos);
    data_ratio = data_size * 100 / rbuf_in->size; /* 100: percent(%) */

    drv_audio_proc_print_title(file, "in buf");

    /* line 1 */
    drv_audio_osal_proc_print(file,
        "%-16s: 0x%08x |" "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "\n",
        "buf_addr", rbuf_in->buf_addr,
        "buf_size", rbuf_in->size,
        "read_pos", rbuf_in->read_pos,
        "write_pos", rbuf_in->write_pos);

    /* line 2 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10s |" "\n",
        "tail_size", rbuf_in->tail_size,
        "data_size", data_size,
        "data_ratio", data_ratio,
        "", "");

    /* line 3 */
    drv_audio_osal_proc_print(file,
        "%-16s: 0x%08x |" "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10s |" "\n",
        "info_addr", rbuf_in->info_addr,
        "info_num", rbuf_in->info_num,
        "info_used", rbuf_in->info_used,
        "", "");
}

static td_void drv_adec_proc_show_out_buf(td_void *file, td_u8 buf_index, const audio_ring_buf *rbuf_out,
    td_u32 frame_pms)
{
    td_u32 data_size;
    td_u32 data_ratio, data_ms;

    data_size = drv_audio_proc_query_rbuf_busy(rbuf_out->size, rbuf_out->read_pos, rbuf_out->write_pos);
    data_ratio = data_size * 100 / rbuf_out->size; /* 100: percent(%) */
    data_ms = drv_audio_proc_get_data_ms(data_size, frame_pms);

    drv_audio_proc_print_title(file, "out buf[%d]", buf_index);

    /* line 1 */
    drv_audio_osal_proc_print(file,
        "%-16s: 0x%08x |" "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "\n",
        "buf_addr", rbuf_out->buf_addr,
        "buf_size", rbuf_out->size,
        "read_pos", rbuf_out->read_pos,
        "write_pos", rbuf_out->write_pos);

    /* line 2 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "%-16s: %-10d |" "\n",
        "tail_size", rbuf_out->tail_size,
        "data_size", data_size,
        "data_ratio", data_ratio,
        "data_ms", data_ms);

    /* line 3 */
    drv_audio_osal_proc_print(file,
        "%-16s: %-10u |" "%-16s: 0x%08x |" "%-16s: %-10d |" "%-16s: %-10d |" "\n",
        "overrun", rbuf_out->overrun_cnt,
        "info_addr", rbuf_out->info_addr,
        "info_num", rbuf_out->info_num,
        "info_used", rbuf_out->info_used);
}

td_s32 drv_proc_adec_show(td_void *file, td_handle adec, const adec_proc_item *proc)
{
    td_u8 i;
    drv_audio_proc_print_head(file, "adec%d", drv_audio_proc_get_mod_index(adec));

    drv_adec_proc_show_attr(file, proc);
    drv_adec_proc_show_basic_status(file, adec, proc);
    drv_adec_proc_show_run_status(file, proc);
    drv_adec_proc_show_spend_status(file, proc);

    /* input */
    if (proc->rbuf_in.handle != 0) {
        drv_adec_proc_show_in_buf(file, &(proc->rbuf_in));
    }

    /* output */
    drv_adec_proc_show_out_pcm_format(file, proc);
    for (i = 0; i < ADEC_RBUF_MAX_NUM; i++) {
        if (proc->rbuf_out[i].handle != 0) {
            drv_adec_proc_show_out_buf(file, i, &(proc->rbuf_out[i]), proc->frame_pms);
        }
    }

    drv_audio_proc_print_tail(file);

    return AUDIO_SUCCESS;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
