/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decode sample
 * Author: audio
 * Create: 2019-09-17
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "thread_os.h"
#include "osal_list.h"
#include "securec.h"
#include "chip_type.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_adec.h"
#include "soc_uapi_sea.h"
#include "soc_uapi_sound.h"
#include "soc_uapi_audio_sys.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"
#include "sample_audio_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SEND_DATA_MODE 1 /* 0: Use get/put buffer interface; 1: Use send_stream interface; */
#define MAX_ADEC_NUM 2
#define ESPLAY_SND_IDX UAPI_SND_0
#define ESCAST_SND_IDX UAPI_SND_1
#define HEX_FORMATE_STEP 16
#define PCM_SIZE_128MS 24576 /* 24576: pcm data size of 48k 16bit 2ch */
#define SLE_MUSIC_SRC_L2HC_BIT_RATE 64
#define SLE_MUSIC_AENC_NUM 2
#define SLE_MUSIC_ADP_NUM 2
#define TONE_STREAM_MAX_SIZE 0x1400 /* 5K */ /* Follow GT Watch for tone play to match terminal data rhythm */
#define MUSIC_STREAM_MAX_SIZE 0x5000 /* 20K */ /* Follow GT Watch for local play to match terminal data rhythm */

typedef struct {
    td_char *sample;
    td_char *track_speed_str;
    td_char *track_handle;
    td_char *speed;
} sample_decode_track_speed_arg;

typedef struct {
    td_bool task_active;
    athread_handle task;
    td_u8 frame_buf[DECODE_FRAME_BUFFER_SIZE];
    const td_char *in_stream;
    const td_char *out_stream;
    FILE *in_file;
    FILE *out_file;
    td_slong in_file_len;

    sample_acodec_arg acodec_arg;

    td_u32 stream_addr;
    td_u32 stream_size;
    td_handle h_adp_in;
    td_handle h_adec;
    td_handle h_sea; /* audio dnlink sea */
    td_handle h_track;
    data_info reader;
    uapi_audio_scene_config asc;
    td_bool play_once;
    osSemaphoreId_t semaphore_adp_empty;
    sample_acodec_dec_priv_cfg priv_cfg;
    td_bool pcm_bit_depth_changed;
    uapi_audio_pcm_format pcm_out_format; /* pcm output format */
    td_bool profile_constraint_flag;
} sample_decode_inst;

typedef struct {
    td_bool task_active;
    athread_handle task;

    td_handle h_adp_out;
    td_handle h_aenc;
    uapi_audio_scene_config asc;
} sample_decode_bmic_uac_inst;

typedef struct {
    td_handle h_aenc[SLE_MUSIC_AENC_NUM];
    td_handle h_adp_out[SLE_MUSIC_AENC_NUM];
} sample_decode_sle_music_inst;

typedef struct {
    td_bool play_once;
    uapi_audio_scene audio_scene;
    td_handle h_snd;
    uapi_snd snd_id;
    td_bool thread_enable;
    uapi_snd_attr snd_attr;
    uapi_snd_gain gain;
    uapi_snd_aef_profile aef_profile;
    td_u32 decode_num;
    sample_decode_inst decode_inst[MAX_ADEC_NUM];
    struct osal_list_head node;
} sample_decode_ao_inst;

static OSAL_LIST_HEAD(g_sample_decode_list);

static td_void sample_decode_show_inst(td_void)
{
    td_u32 cnt = 0;
    sample_decode_ao_inst *inst = TD_NULL;

    if (osal_list_empty(&g_sample_decode_list) != TD_FALSE) {
        sap_alert_log_info("sample_decode all exit!");
        return;
    }

    osal_list_for_each_entry(inst, &g_sample_decode_list, node) {
        cnt++;
        sap_alert_log_u32(cnt);
        sap_alert_log_u32(inst->snd_id);
        sap_alert_log_u32(inst->decode_num);
    }
}

static sample_decode_ao_inst *sample_decode_alloc_inst(td_void)
{
    sample_decode_ao_inst *inst = (sample_decode_ao_inst *)malloc(sizeof(sample_decode_ao_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    memset_s(inst, sizeof(*inst), 0, sizeof(sample_decode_ao_inst));

    osal_list_add_tail(&inst->node, &g_sample_decode_list);
    return inst;
}

static td_void sample_decode_free_inst(sample_decode_ao_inst *inst)
{
    if (inst == TD_NULL) {
        return;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static sample_decode_ao_inst *sample_decode_get_inst(uapi_snd snd_id)
{
    sample_decode_ao_inst *inst = TD_NULL;

    osal_list_for_each_entry(inst, &g_sample_decode_list, node) {
        if (inst->snd_id == snd_id) {
            return inst;
        }
    }

    return TD_NULL;
}

static td_void sample_decode_init_progress_info(sample_decode_inst *inst)
{
#ifdef SAP_ADEC_PSS_SUPPORT
    td_s32 ret;

    ret = uapi_adec_set_pss(inst->h_adec, 0x0); /* 0x0: start from file head */
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_set_pss, ret);
    }
    sap_alert_log_info("Progress(percentage of file size ***%%)");
    sap_alert_log_info("Cnts(numbers of file size counted by 48k16bit2ch 128ms pcm size)");
    sap_alert_log_info("Pts(pcm time outputed by dsp)");
    sap_alert_log_info("PlayTime: lasting time after sample started");
    sap_alert_log_info("Diff: (PlayTime-Pts)difference between PlayTime and Pts");
#else
    audio_unused(inst);
#endif /* SAP_ADEC_PSS_SUPPORT */
}

static td_void sample_decode_show_progress_info(sample_decode_inst *inst, td_u32 play_start_ms)
{
#ifdef SAP_ADEC_PSS_SUPPORT
    td_s32 ret;
    td_s64 pts;
    td_u32 stream_cnt;
    td_u32 lasting_ms;
    td_u32 cur_ms, cur_pss;
    static td_u32 last_ms = 0;

    cur_ms = sap_get_ms();
    if (((cur_ms - last_ms) / MS_NUM_PER_S) >= 2) { /* Print play process bar info at least every 2s */
        ret = uapi_adec_get_pss(inst->h_adec, &cur_pss);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adec_get_pss, ret);
            return;
        }

        ret = uapi_snd_get_track_play_pts(inst->h_track, &pts); /* us */
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_snd_get_track_play_pts, ret);
            return;
        }

        last_ms = cur_ms;
        lasting_ms = cur_ms - play_start_ms;
        pts /= MS_NUM_PER_S; /* dsp pts : Convert from us to ms */
        stream_cnt = cur_pss / PCM_SIZE_128MS; /* dsp pss : num of 128ms for 48k 16bit 2ch pcm */
        cur_pss = cur_pss * 100 / inst->in_file_len; /* dsp pss : Show as percentage of file size； 100 : % */

        audio_logx(AUDIO_LOG_LEVEL_ALERT,
                   "Progress:%3d;Cnts:%4u;Pts:%8lldms;PlayTime:%8ums;Diff:%4dms",
                   cur_pss, stream_cnt, pts, lasting_ms, lasting_ms - (td_u32)pts);
    }
#else
    audio_unused(inst);
    audio_unused(play_start_ms);
    return;
#endif /* SAP_ADEC_PSS_SUPPORT */
}

static td_s32 sample_decode_parse_addr(td_s32 opt, const td_char *opt_arg, sample_decode_inst *inst)
{
    if (strncasecmp(opt_arg, "0x", 2) != 0) { /* 2 is the number of comparison */
        sap_printf("invalid address prefix, should be '0x' or '0X'\n");
        return EXT_FAILURE;
    }

    inst->stream_addr = (td_u32)strtoul(opt_arg, NULL, HEX_FORMATE_STEP); /* 16 represents Hexadecimal */
    sap_printf("get input addr 0x%x\n", inst->stream_addr);

    sap_unused(opt);
    return EXT_SUCCESS;
}

static td_void sample_decode_reader_init(sample_decode_inst *inst)
{
    data_info_init(&inst->reader, inst->stream_addr, inst->stream_size);
    circ_buf_update_write_pos(&inst->reader.cb, inst->reader.cb.size - sizeof(td_s16));
}

static td_void sample_decode_close_file(sample_decode_inst *inst)
{
    if (inst->out_file != TD_NULL) {
        sap_fclose(inst->out_file);
        inst->out_file = TD_NULL;
    }

    if (inst->in_file != TD_NULL) {
        sap_fclose(inst->in_file);
        inst->in_file = TD_NULL;
    }
}

static td_s32 sample_decode_open_file(sample_decode_inst *inst)
{
    sample_decode_reader_init(inst);

    /* open input file */
    inst->in_file = TD_NULL;

    if (inst->in_stream != TD_NULL && strcmp(inst->in_stream, "null") != 0) {
        inst->in_file = sample_audio_open_input_stream(inst->in_stream);
        if (inst->in_file == TD_NULL) {
            sap_printf("open file %s error!\n", inst->in_stream);
            return EXT_FAILURE;
        }
        sap_fseek(inst->in_file, 0, SEEK_END);
        inst->in_file_len = sap_ftell(inst->in_file);
        sap_rewind(inst->in_file);
    } else {
        /* check read flash addr and len set */
        if (inst->stream_addr == 0 || inst->stream_size == 0) {
            sap_printf("need to set flash addr and len to read first!\n");
            goto out;
        }
    }

    /* open output file */
    inst->out_file = TD_NULL;

    if (inst->out_stream != TD_NULL && strcmp(inst->out_stream, "null") != 0) {
        inst->out_file = sap_fopen(inst->out_stream, "wb");
        if (inst->out_file == TD_NULL) {
            sap_printf("open file %s error!\n", inst->out_stream);
            goto out;
        }
    }

    return 0;
out:
    if (inst->in_file != TD_NULL) {
        sap_fclose(inst->in_file);
        inst->in_file = TD_NULL;
    }

    return EXT_FAILURE;
}

static td_void sample_decode_ao_close_file(sample_decode_ao_inst *inst)
{
    td_u32 i;

    for (i = 0; i < inst->decode_num; i++) {
        sample_decode_close_file(&inst->decode_inst[i]);
    }
}

static td_s32 sample_decode_ao_open_file(sample_decode_ao_inst *inst)
{
    td_u32 i;
    td_s32 ret;

    for (i = 0; i < inst->decode_num; i++) {
        ret = sample_decode_open_file(&inst->decode_inst[i]);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_decode_open_file, ret);
            sample_decode_ao_close_file(inst);
            return ret;
        }
    }

    return EXT_SUCCESS;
}

static td_u32 read_es_from_flash(sample_decode_inst *inst, td_u8 *to, td_u32 len)
{
    circ_buf *cb = &inst->reader.cb;

    if (cb == TD_NULL) {
        sap_err_log_info("Fatal Error: circ_buf pointer is null");
        return 0;
    }

    if (len > cb->size) {
        sap_err_log_info("Fatal Error: data not enough");
        return 0;
    }

    if (circ_buf_query_busy(cb) < len) {
        sap_err_log_info("reset read");
        circ_buf_flush(cb);
        circ_buf_update_write_pos(cb, cb->size - sizeof(td_s16));
    }

    if (circ_buf_read(cb, to, len) != len) {
        sap_err_log_info("Fatal Error: circ_buf_read error");
        return 0;
    }

    circ_buf_update_write_pos(cb, len);
    return len;
}

static td_void sample_decode_rewind_file(sample_decode_inst *inst)
{
    td_u32 left_size = sizeof(inst->frame_buf);
    sap_rewind(inst->in_file);

    sample_decode_init_progress_info(inst); /* set pss to 0x0 when file rewind */

    if (inst->acodec_arg.acodec_id == UAPI_ACODEC_ID_FLAC) {
        flac_demux_header(inst->in_file, inst->frame_buf, &left_size);
    }
}

static td_u32 sample_decode_read_file(sample_decode_inst *inst, uapi_stream_buf *stream)
{
    td_u32 read_len;

    read_len = sap_fread(stream->data, 1, stream->size, inst->in_file);
    if (read_len == 0) {
        if (inst->play_once != TD_TRUE) {
            sample_decode_rewind_file(inst);
            sap_printf("sap_rewind file %s\n", inst->in_stream);
        }
        return 0;
    } else {
        stream->size = read_len;
        stream->pts = 0LL;
        stream->eos = ((inst->in_file_len == sap_ftell(inst->in_file)) && (inst->play_once == TD_TRUE));
        return stream->size;
    }
}

static td_u32 sample_decode_read_es(sample_decode_inst *inst, uapi_stream_buf *stream)
{
    if (inst->in_file != TD_NULL) {
        return sample_decode_read_file(inst, stream);
    } else {
        return read_es_from_flash(inst, stream->data, stream->size);
    }
}

static td_void sample_decode_destroy_semaphore(sample_decode_inst *inst)
{
#ifdef PLAY_ENABLE_ADP_EVENT_CTRL
    td_s32 ret;
    if (inst->semaphore_adp_empty != TD_NULL) {
        ret = osSemaphoreDelete(inst->semaphore_adp_empty);
        if (ret != osOK) {
            sap_err_log_fun(osSemaphoreDelete, ret);
        }
    }
#endif
    audio_unused(inst);
}

static td_s32 sample_decode_create_semaphore(sample_decode_inst *inst)
{
#ifdef PLAY_ENABLE_ADP_EVENT_CTRL
    sap_alert_log_info("Send data to dsp by adp empty event");
    inst->semaphore_adp_empty = osSemaphoreNew(1, 0, TD_NULL);
    if (inst->semaphore_adp_empty == TD_NULL) {
        sap_err_log_fun(osSemaphoreNew, AUDIO_FAILURE);
        return EXT_FAILURE;
    }
#endif
    audio_unused(inst);
    return EXT_SUCCESS;
}

static td_s32 sample_decode_acquire_semaphore(sample_decode_inst *inst)
{
    if (inst->task == TD_NULL) {
        return EXT_SUCCESS;
    }

#ifdef PLAY_ENABLE_ADP_EVENT_CTRL /* Wait for semaphore from adp empty event */
        td_s32 ret;
        ret = osSemaphoreAcquire(inst->semaphore_adp_empty, osWaitForever);
        if (ret != osOK) {
            sap_err_log_fun(osSemaphoreAcquire, ret);
            return ret;
        }
#endif
    audio_unused(inst);
    return EXT_SUCCESS;
}

static td_s32 sample_decode_release_semaphore(sample_decode_inst *inst)
{
    if (inst->task == TD_NULL) {
        return EXT_SUCCESS;
    }

#ifdef PLAY_ENABLE_ADP_EVENT_CTRL
    td_s32 ret;
    ret = osSemaphoreRelease(inst->semaphore_adp_empty);
    if (ret != osOK) {
        sap_err_log_fun(osSemaphoreRelease, ret);
        return ret;
    }
#endif
    audio_unused(inst);
    return EXT_SUCCESS;
}

static td_s32 sample_decode_adp_event_empty_proc(td_handle adp, sample_decode_inst *inst)
{
    td_s32 ret;

    ret = sample_decode_release_semaphore(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_release_semaphore, ret);
    }

    audio_unused(adp);
    return ret;
}

static td_s32 sample_decode_adp_event_proc(td_handle adp, uapi_adp_event_type event,
    const td_void *param, const td_void *context)
{
    td_s32 ret;
    sample_decode_inst *inst = (sample_decode_inst *)context;

    switch (event) {
        case UAPI_ADP_EVENT_EMPTY:
            ret = sample_decode_adp_event_empty_proc(adp, inst);
            if (ret != EXT_SUCCESS) {
                sap_err_log_fun(sample_decode_adp_event_empty_proc, ret);
                return ret;
            }
            break;
        default:
            sap_err_log_info("Invalid ADP evevt!");
            sap_err_log_h32(event);
            break;
    }
    sap_unused(param);
    sap_unused(context);
    return EXT_SUCCESS;
}

static td_u32 sample_decode_play_task_prepare(sample_decode_inst *inst)
{
    td_u32 pre_ms;

    sap_alert_log_info("thread enter.");
    sap_alert_log_h32(inst->h_adp_in);

    sample_decode_init_progress_info(inst);

    pre_ms = sap_get_ms();
    sap_alert_log_u32(pre_ms);
    return pre_ms;
}

#if SEND_DATA_MODE
static td_u32 sample_decode_buf_size_init(uapi_acodec_id acodec_id, td_u32 frame_buf_size)
{
    if (acodec_id == UAPI_ACODEC_ID_SBC) {
        return circ_buf_min(frame_buf_size, TONE_STREAM_MAX_SIZE); /* tone */
    } else {
        return circ_buf_min(frame_buf_size, MUSIC_STREAM_MAX_SIZE); /* music */
    }
}

static td_void sample_decode_play_task(td_void *arg)
{
    td_s32 ret;
    sample_decode_inst *inst = (sample_decode_inst *)arg;
    uapi_stream_buf stream = {
        .data = inst->frame_buf,
        .pts = 0LL,
        .eos = TD_FALSE,
        .pkg_loss = TD_FALSE,
    };
    td_u32 read_len = (td_u32)sizeof(inst->frame_buf);
    td_u32 buf_size = sample_decode_buf_size_init(inst->acodec_arg.acodec_id, read_len);
    td_u32 pre_ms = sample_decode_play_task_prepare(inst);

    while (inst->task_active) {
        ret = sample_decode_acquire_semaphore(inst);
        if (ret != EXT_SUCCESS) {
            break;
        }

        uapi_adp_query_free(inst->h_adp_in, &read_len);
        if (read_len < FILE_READ_LEN_MIN) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }

        stream.size = circ_buf_min(buf_size, read_len);
        stream.size = sample_decode_read_es(inst, &stream);
        if (stream.size == 0) {
            if (inst->play_once == TD_TRUE) {
                sap_printf("end of file %s", inst->in_stream);
                break;
            } else {
                sap_msleep(THREAD_SLEEP_10MS);
                sample_decode_adp_event_empty_proc(inst->h_adp_in, inst); /* Send one packet data after rewind */
                continue;
            }
        }

        while (inst->task_active) {
            ret = uapi_adp_send_stream(inst->h_adp_in, &stream);
            if (ret != EXT_SUCCESS) {
                sap_msleep(THREAD_SLEEP_10MS);
                continue;
            }
            break;
        }
        sample_decode_show_progress_info(inst, pre_ms); /* Show play progress(file size && pts) */
    }

    sap_alert_log_info("thread exit.");
    athread_set_exit(inst->task, TD_TRUE);
}

#else
static td_void sample_decode_play_task(td_void *arg)
{
    td_s32 ret;
    td_u32 read_len = 0;
    uapi_stream_buf stream = {
        .pts = 0LL,
        .eos = TD_FALSE,
        .pkg_loss = TD_FALSE,
    };
    sample_decode_inst *inst = (sample_decode_inst *)arg;
    td_u32 pre_ms = sample_decode_play_task_prepare(inst);

    while (inst->task_active) {
        /* Use get/put buffer to send data */
        ret = sample_decode_acquire_semaphore(inst);
        if (ret != EXT_SUCCESS) {
            break;
        }

        uapi_adp_query_free(inst->h_adp_in, &read_len);
        if (read_len < FILE_READ_LEN_MIN) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }

        ret = uapi_adp_get_buffer(inst->h_adp_in, &stream);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }

        read_len = sample_decode_read_es(inst, &stream);
        if (read_len == 0) {
            if (inst->play_once == TD_TRUE) {
                sap_printf("end of file %s", inst->in_stream);
                break;
            } else {
                sap_msleep(THREAD_SLEEP_10MS);
                sample_decode_adp_event_empty_proc(inst->h_adp_in, inst); /* Send one packet data after rewind */
                continue;
            }
        }

        ret = uapi_adp_put_buffer(inst->h_adp_in, &stream);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_put_buffer, ret);
        }

        sample_decode_show_progress_info(inst, pre_ms); /* Show play progress(file size && pts) */
    }

    sap_alert_log_info("thread exit.");
    athread_set_exit(inst->task, TD_TRUE);
}
#endif

static td_s32 sample_decode_sys_init(td_void)
{
    td_s32 ret;

    ret = uapi_adp_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_init, ret);
        return ret;
    }

    ret = uapi_snd_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_init, ret);
        goto out0;
    }

    ret = uapi_adec_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_init, ret);
        goto out1;
    }

    ret = uapi_aenc_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_init, ret);
        goto out1;
    }

    return EXT_SUCCESS;
out1:
    (td_void)uapi_snd_deinit();
out0:
    (td_void)uapi_adp_deinit();
    return ret;
}

static td_void sample_decode_sys_deinit(td_void)
{
    td_s32 ret;

    ret = uapi_aenc_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_deinit, ret);
    }

    ret = uapi_adec_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_deinit, ret);
    }

    ret = uapi_snd_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_deinit, ret);
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_deinit, ret);
    }
}

static td_void sample_decode_close_adp(sample_decode_inst *inst)
{
    td_s32 ret;

    if (inst->h_adp_in == 0) {
        return;
    }

    ret = uapi_adp_detach_output(inst->h_adp_in, inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_detach_output, ret);
    }

    ret = uapi_adp_destroy(inst->h_adp_in);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_destroy, ret);
    }

    inst->h_adp_in = 0;
}

static td_s32 sample_decode_open_adp(sample_decode_inst *inst)
{
    td_s32 ret;
    uapi_adp_attr adp_attr;

    ret = uapi_adp_get_def_attr(&adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_get_def_attr, ret);
        return ret;
    }

    inst->h_adp_in = 0;
    ret = uapi_adp_create(&inst->h_adp_in, &adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_create, ret);
        return ret;
    }

    ret = uapi_adp_register_event_proc(inst->h_adp_in, (uapi_adp_event_proc)sample_decode_adp_event_proc, inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_register_event_proc, ret);
        goto out;
    }

    ret = uapi_adp_attach_output(inst->h_adp_in, inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_adp_destroy(inst->h_adp_in);
    return ret;
}

static td_void sample_decode_close_snd(sample_decode_ao_inst *inst)
{
    td_s32 ret;

    if (inst->h_snd == 0) {
        return;
    }

    ret = uapi_snd_close(inst->h_snd);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_close, ret);
    }

    inst->h_snd = 0;
}

static td_void sample_decode_get_def_attr_snd(sample_decode_ao_inst *inst)
{
    inst->snd_attr.port_num = 1;
    inst->snd_attr.port_attr[0].out_port = SND_OUT_PORT_DEFAULT;
    inst->snd_attr.channels = SND_OUT_CHANNEL_DEF;
    inst->snd_attr.bit_depth = SND_OUT_BIT_DEPTH_DEF;
    inst->snd_attr.sample_rate = SND_OUT_SAMPLE_RATE_DEF;
}

static td_void sample_decode_set_i2s_attr(uapi_snd_attr *snd_attr)
{
    td_bool local_out_port;
    uapi_audio_pcm_format pcm_fmt = {
        .sample_rate = SND_OUT_I2S_SAMPLE_RATE_DEF,
        .channels = SND_OUT_I2S_CHANNEL_DEF,
        .bit_depth = SND_OUT_I2S_BIT_DEPTH_DEF,
    };
    uapi_audio_i2s_attr *i2s_attr = TD_NULL;

    for (td_u32 i = 0; i < snd_attr->port_num; ++i) {
        if (snd_attr->port_attr[i].out_port < UAPI_SND_OUT_PORT_I2S0 ||
            snd_attr->port_attr[i].out_port > UAPI_SND_OUT_PORT_I2S4) {
            continue; /* Set i2s attr when port is i2s */
        }

        i2s_attr = &snd_attr->port_attr[i].u.i2s_attr.attr;
        local_out_port = (snd_attr->port_attr[i].out_port != CAT1_OUT_PORT_DEFAULT) ? TD_TRUE : TD_FALSE;

        if (local_out_port != TD_TRUE) { /* CAT1 adapt */
            pcm_fmt.sample_rate = SIO_CAT1_I2S_SAMPLE_RATE_DEF;
            pcm_fmt.channels = SIO_CAT1_I2S_CHANNEL_DEF;
            pcm_fmt.bit_depth = SIO_CAT1_I2S_BIT_DEPTH_DEF;
        }

        get_i2s_attr(i2s_attr, &pcm_fmt, local_out_port);
    }
}

static td_s32 sample_decode_open_snd(sample_decode_ao_inst *inst)
{
    td_s32 ret;
    uapi_snd_attr snd_attr;

    inst->h_snd = 0;
    ret = uapi_snd_get_default_attr(inst->snd_id, &snd_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_default_attr, ret);
        return ret;
    }

    snd_attr.port_num = inst->snd_attr.port_num;
    snd_attr.port_attr[0].out_port = inst->snd_attr.port_attr[0].out_port;
    snd_attr.channels = inst->snd_attr.channels;
    snd_attr.bit_depth = inst->snd_attr.bit_depth;
    snd_attr.sample_rate = inst->snd_attr.sample_rate;

    sample_decode_set_i2s_attr(&snd_attr);

    ret = uapi_snd_open(&inst->h_snd, inst->snd_id, &snd_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_open, ret);
        return ret;
    }

    ret = uapi_snd_set_volume(inst->h_snd, snd_attr.port_attr[0].out_port, &inst->gain, TD_NULL);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_volume, ret);
        goto out;
    }

    ret = uapi_snd_set_aef_profile(inst->h_snd, inst->aef_profile);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_profile, ret);
        goto out;
    }

    ret = uapi_snd_set_aef_bypass(inst->h_snd, UAPI_SND_OUT_PORT_ALL, AEF_BYPASS_DEFAULT);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_bypass, ret);
        goto out;
    }

    ret = uapi_snd_set_track_mode(inst->h_snd, snd_attr.port_attr[0].out_port, TRACK_MODE_DEFAULT);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_track_mode, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_snd_close(inst->h_snd);
    inst->h_snd = 0;
    return ret;
}

static td_void sample_decode_close_track(sample_decode_inst *inst)
{
    td_s32 ret;

    if (inst->h_track == 0) {
        return;
    }

    ret = uapi_snd_destroy_track(inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_destroy_track, ret);
    }

    inst->h_track = 0;
}

static td_s32 sample_decode_open_track(td_handle h_snd, sample_decode_inst *inst)
{
    td_s32 ret;
    uapi_snd_track_attr track_attr;

    inst->h_track = 0;
    ret = uapi_snd_get_track_default_attr(&track_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_track_default_attr, ret);
        return ret;
    }

    ret = uapi_snd_create_track(&inst->h_track, h_snd, &track_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_create_track, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_decode_close_sea(sample_decode_inst *inst)
{
    td_s32 ret;

    if (inst->h_sea == 0) {
        return;
    }

    ret = uapi_sea_detach_output(inst->h_sea, UAPI_SEA_OUTPUT_VC_SRC, inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_detach_output, ret);
    }

    ret = uapi_sea_destroy(inst->h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_destroy, ret);
    }

    inst->h_sea = 0;
}

static td_s32 sample_decode_open_sea(sample_decode_inst *inst)
{
    td_s32 ret;
    uapi_sea_attr sea_attr;
    uapi_sea_eng_sel sea_eng;
    uapi_sea_afe_attr afe_attr = {
        .type = UAPI_SEA_AFE_VQE,
        .mode = UAPI_SEA_NORM_MODE,
        .u.see.options = UAPI_SEA_AFE_OPT_AGC,
    };

    inst->h_sea = 0;
    if (inst->acodec_arg.dnlink_sea_en == TD_FALSE) {
        return EXT_SUCCESS;
    }

    /* Create SEA instance */
    (td_void)memset_s(&sea_eng, sizeof(sea_eng), 0, sizeof(sea_eng));
    sea_eng.afe.type = UAPI_SEA_AFE_VQE;
    sea_eng.afe.lib_id = UAPI_SEA_LIB_SEE;
    ret = uapi_sea_get_default_attr(&sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_get_default_attr, ret);
        return ret;
    }

    // 下行sea不支持ref信号
    sea_attr.ref_pcm.channels = 0;
    ret = uapi_sea_create(&inst->h_sea, &sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_create, ret);
        return ret;
    }

    ret = uapi_sea_set_afe_attr(inst->h_sea, UAPI_SEA_AFE_VQE, &afe_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_set_afe_attr, ret);
        goto out;
    }

    ret = uapi_sea_attach_output(inst->h_sea, UAPI_SEA_OUTPUT_VC_SRC, inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_sea_destroy(inst->h_sea);
    return ret;
}

static td_void sample_decode_close_adec(sample_decode_inst *inst)
{
    td_s32 ret;
    td_handle adec_output;

    if (inst->h_adec == 0) {
        return;
    }

    adec_output = ((inst->acodec_arg.dnlink_sea_en == TD_TRUE) ? inst->h_sea : inst->h_track);
    ret = uapi_adec_detach_output(inst->h_adec, adec_output);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_detach_output, ret);
    }

    ret = uapi_adec_destroy(inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_destroy, ret);
    }

    inst->h_adec = 0;
}

static td_void sample_decode_adec_attr_cfg(sample_decode_inst *inst, uapi_adec_attr *adec_attr)
{
    td_u32 tmp_size;

    adec_attr->codec_id = inst->acodec_arg.acodec_id;
    adec_attr->frame_mode_input = TD_FALSE;
    adec_attr->param.sample_rate = inst->acodec_arg.pcm_format.sample_rate;
    adec_attr->param.channels = inst->acodec_arg.pcm_format.channels;
    adec_attr->param.bit_depth = inst->acodec_arg.pcm_format.bit_depth;

    sap_alert_log_u32(inst->acodec_arg.pcm_format.sample_rate);
    sap_alert_log_u32(inst->acodec_arg.pcm_format.channels);
    sap_alert_log_u32(inst->acodec_arg.pcm_format.bit_depth);

    if (inst->acodec_arg.acodec_id == UAPI_ACODEC_ID_FLAC) {
        tmp_size = sizeof(inst->frame_buf);
        flac_demux_header(inst->in_file, inst->frame_buf, &tmp_size);
        adec_attr->param.private_data = inst->frame_buf;
        adec_attr->param.private_data_size = sizeof(inst->frame_buf) - tmp_size;
    } else if (inst->acodec_arg.acodec_id == UAPI_ACODEC_ID_VORBIS) {
        inst->play_once = TD_TRUE; /* sample support play once only */
        /* For vorbis music: use priv_data to set decode mode: ogg file or raw file */
        adec_attr->param.private_data = &(inst->priv_cfg.vorbis_priv_cfg);
        adec_attr->param.private_data_size = sizeof(inst->priv_cfg.vorbis_priv_cfg);
    } else if (inst->acodec_arg.acodec_id == UAPI_ACODEC_ID_OPUS) {
        if (inst->priv_cfg.opus_priv_cfg.frame_length != 0) {
            adec_attr->param.private_data = &(inst->priv_cfg.opus_priv_cfg);
            adec_attr->param.private_data_size = sizeof(inst->priv_cfg.opus_priv_cfg);
        }
    } else if (inst->acodec_arg.acodec_id == UAPI_ACODEC_ID_PCM && inst->pcm_bit_depth_changed == TD_TRUE) {
        inst->pcm_out_format.sample_rate = inst->acodec_arg.pcm_format.sample_rate;
        inst->pcm_out_format.channels = inst->acodec_arg.pcm_format.channels;
        inst->pcm_out_format.sample_per_frame = inst->pcm_out_format.sample_rate / 100; /* 100 --> 10ms */
        adec_attr->param.private_data = &inst->pcm_out_format;
        adec_attr->param.private_data_size = sizeof(inst->pcm_out_format);
    } else {
        adec_attr->param.private_data = TD_NULL;
        adec_attr->param.private_data_size = 0;
    }
}

static td_s32 sample_decode_set_aac_param(sample_decode_inst *inst)
{
    td_s32 ret;

    ret = uapi_adec_set_param(inst->h_adec, UAPI_ADEC_PARAM_PROFILE_CONSTRAINT_FLAG, &inst->profile_constraint_flag,
        sizeof(inst->profile_constraint_flag));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_set_param, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_decode_set_param(sample_decode_inst *inst)
{
    if (inst->acodec_arg.acodec_id == UAPI_ACODEC_ID_AAC) {
        return sample_decode_set_aac_param(inst);
    }

    return EXT_SUCCESS;
}

static td_s32 sample_decode_open_adec(sample_decode_inst *inst)
{
    td_s32 ret;
    uapi_adec_attr adec_attr;
    td_handle adec_output;

    inst->h_adec = 0;
    ret = uapi_adec_create(&inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_create, ret);
        return ret;
    }

    ret = uapi_adec_get_attr(inst->h_adec, &adec_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_get_attr, ret);
        goto out;
    }

    sample_decode_adec_attr_cfg(inst, &adec_attr);

    ret = uapi_adec_set_attr(inst->h_adec, &adec_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_set_attr, ret);
        goto out;
    }

    ret = sample_decode_set_param(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_set_param, ret);
        goto out;
    }

    /* attach_output时会根据场景决定输入buffer大小，需要在此之前配置adec的场景 */
    ret = uapi_audio_set_audio_scene(inst->h_adec, &inst->asc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_audio_set_audio_scene, ret);
    }

    adec_output = ((inst->acodec_arg.dnlink_sea_en == TD_TRUE) ? inst->h_sea : inst->h_track);
    ret = uapi_adec_attach_output(inst->h_adec, adec_output);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_adec_destroy(inst->h_adec);
    return ret;
}

static td_void sample_decode_close_inst(sample_decode_inst *inst)
{
    sample_decode_close_adp(inst);
    sample_decode_close_adec(inst);
    sample_decode_close_sea(inst);
    sample_decode_close_track(inst);
}

static td_s32 sample_decode_open_inst(td_handle h_snd, sample_decode_inst *inst)
{
    td_s32 ret;

    /*
     * 数据流向 adp --> adec --> track(snd)
     * 按照数据流向反方向打开实例
     * 按照数据方向attach output
     */
    ret = sample_decode_open_track(h_snd, inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_open_track, ret);
        return ret;
    }

    ret = sample_decode_open_sea(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_open_sea, ret);
        goto out0;
    }

    ret = sample_decode_open_adec(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_open_adec, ret);
        goto out1;
    }

    ret = sample_decode_open_adp(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_open_adp, ret);
        goto out2;
    }

    return EXT_SUCCESS;
out2:
    sample_decode_close_adec(inst);
out1:
    sample_decode_close_sea(inst);
out0:
    sample_decode_close_track(inst);
    return ret;
}

static td_void sample_decode_ao_close_inst(sample_decode_ao_inst *inst)
{
    td_u32 i;

    for (i = 0; i < inst->decode_num; i++) {
        sample_decode_close_inst(&inst->decode_inst[i]);
    }

    sample_gui_aef_delete_handle(inst->h_snd);

    sample_decode_close_snd(inst);
    sample_decode_sys_deinit();
}

static td_s32 sample_decode_ao_open_inst(sample_decode_ao_inst *inst)
{
    td_u32 i;
    td_s32 ret;

    ret = sample_decode_sys_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_sys_init, ret);
        return ret;
    }

    ret = sample_decode_open_snd(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_open_snd, ret);
        goto out0;
    }

    sample_gui_aef_add_handle(GUI_AEF_AO, inst->h_snd);

    for (i = 0; i < inst->decode_num; i++) {
        inst->decode_inst[i].play_once = inst->play_once;
        inst->decode_inst[i].asc.scene_id = i;
        if (inst->decode_inst[i].acodec_arg.acodec_id == UAPI_ACODEC_ID_SBC
            && inst->audio_scene == UAPI_AUDIO_SCENE_LOCAL_MUSIC) {
            inst->decode_inst[i].asc.scene_type = UAPI_AUDIO_SCENE_LOCAL_TONE;
        } else {
            inst->decode_inst[i].asc.scene_type = inst->audio_scene;
        }

        ret = sample_decode_open_inst(inst->h_snd, &inst->decode_inst[i]);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_decode_open_inst, ret);
            goto out0;
        }
    }

    return EXT_SUCCESS;
out0:
    sample_decode_ao_close_inst(inst);
    return ret;
}

static td_s32 sample_decode_start_sea(sample_decode_inst *inst)
{
    td_s32 ret;

    if (inst->acodec_arg.dnlink_sea_en == TD_FALSE) {
        return EXT_SUCCESS;
    }

    ret = uapi_sea_start(inst->h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_start, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_decode_stop_sea(sample_decode_inst *inst)
{
    td_s32 ret;

    if (inst->acodec_arg.dnlink_sea_en == TD_FALSE) {
        return;
    }

    ret = uapi_sea_stop(inst->h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_stop, ret);
    }
}

static td_void sample_decode_stop_task(sample_decode_inst *inst)
{
    if (inst->task == TD_NULL) {
        return;
    }

    if (inst->task_active) {
        inst->task_active = TD_FALSE;
        sample_decode_adp_event_empty_proc(inst->h_adp_in, inst); /* Wake semaphore to exit task */
        athread_exit(inst->task);
    }

    inst->task = TD_NULL;

    sample_decode_destroy_semaphore(inst);
}

static td_s32 sample_decode_start_task(sample_decode_inst *inst, td_bool thread_enable)
{
    td_s32 ret;
    athread_attr attr;
    athread_fun thread_func;

    if (thread_enable == TD_FALSE) {
        sap_alert_log_info("Thread not enable");
        return EXT_SUCCESS;
    }

    ret = sample_decode_create_semaphore(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_create_semaphore, ret);
        return ret;
    }

    thread_func = sample_decode_play_task;

    inst->task_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_ABOVENORMAL3;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_dec_play";
    ret = athread_create(&inst->task, thread_func, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        return ret;
    }

    sample_decode_adp_event_empty_proc(inst->h_adp_in, inst); /* 同步终端的代码逻辑，start后先送一包数据 */

    return EXT_SUCCESS;
}

static td_void sample_decode_stop_link(sample_decode_inst *inst)
{
    td_s32 ret;

    ret = uapi_adec_stop(inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_stop, ret);
    }

    sample_decode_stop_sea(inst);

    ret = uapi_snd_track_stop(inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_track_stop, ret);
    }
}

static td_s32 sample_decode_start_link(sample_decode_inst *inst)
{
    td_s32 ret;

    ret = uapi_snd_track_start(inst->h_track, TD_NULL);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_track_start, ret);
        return ret;
    }

    ret = sample_decode_start_sea(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_start_sea, ret);
        goto out0;
    }

    ret = uapi_adec_start(inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_start, ret);
        goto out1;
    }

    return EXT_SUCCESS;
out1:
    sample_decode_stop_sea(inst);
out0:
    (td_void)uapi_snd_track_stop(inst->h_track);
    return ret;
}

static td_void sample_decode_stop_inst(sample_decode_inst *inst)
{
    td_s32 ret;
    td_s64 delay_us = 0;

    sample_decode_stop_task(inst);

    sample_decode_stop_link(inst);

    ret = uapi_snd_get_track_delay(inst->h_track, &delay_us);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_track_delay, ret);
        return;
    }

    sap_alert_log_s64(delay_us);
    return;
}

static td_s32 sample_decode_start_inst(sample_decode_inst *inst, td_bool thread_enable)
{
    td_s32 ret;
#ifndef MONO_PROFILE_CUT
    if (inst->asc.scene_type == UAPI_AUDIO_SCENE_SLE_MUSIC_SINK) {
        ret = uapi_audio_set_tws_mode(UAPI_AUDIO_TWS_MODE_DUAL_TRANSMIT);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_audio_set_tws_mode, ret);
            return ret;
        }
    }
#endif
    ret = uapi_audio_set_audio_scene(inst->h_adp_in, &inst->asc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_audio_set_audio_scene, ret);
    }

    sample_audio_set_hifi_mode(inst->asc.scene_type);

    ret = sample_decode_start_task(inst, thread_enable);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_start_task, ret);
        return ret;
    }

    ret = sample_decode_start_link(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_start_link, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_decode_ao_stop_inst(sample_decode_ao_inst *inst)
{
    td_u32 i;
    td_s32 ret;

    ret = uapi_snd_set_enable(inst->h_snd, TD_FALSE);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_enable, ret);
    }

    for (i = 0; i < inst->decode_num; i++) {
        sample_decode_stop_inst(&inst->decode_inst[i]);
    }
}

static td_s32 sample_decode_ao_start_inst(sample_decode_ao_inst *inst)
{
    td_u32 i;
    td_s32 ret;

    for (i = 0; i < inst->decode_num; i++) {
        ret = sample_decode_start_inst(&inst->decode_inst[i], inst->thread_enable);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_decode_start_inst, ret);
            goto out;
        }
    }

    ret = uapi_snd_set_enable(inst->h_snd, TD_TRUE);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_enable, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    sample_decode_ao_stop_inst(inst);
    return ret;
}

static td_void sample_decode_usage(td_void)
{
    sap_printf("\n");
    sap_printf("If you want to decode a file\n");
    sap_printf("    usage:  sample_decode -i [InputFileName] [OPTIONS]\n");
    sap_printf("\n");
    sap_printf("        -h     show this usage message and abort\n");
    sap_printf("        -i     input file name[read flash when no file specified]\n");
    sap_printf("        -a     specify addr to read from flash\n");
    sap_printf("        -f     opus dec frame length(25/50/100/200/400/600/800/1000/1200)\n");
    sap_printf("\n");
    sap_printf("        -t     the type of play audio(aac/mp3/opus/lc3/l2hc/sbc/msbc/flac)\n");
    sap_printf("        -c     channels of the input stream(default: 2)\n");
    sap_printf("        -b     bit_depth of the input stream(default: 16)\n");
    sap_printf("        -s     samplerate of the input stream(default: 48000)\n");
    sap_printf("        -o     the output file name to save\n");
    sap_printf("        -v     set play volume(default: 0dB)\n");
    sap_printf("\n");
    sap_printf("        -l                         specify one long argument\n");
    sap_printf("        -l once                    play input stream once and deal eos event\n");
    sap_printf("        -l mpeg4_support           support AAC MPEG2 decoding and AAC MPEG4 decoding\n");
    sap_printf("        -l 0x123                   specify stream size to read(byte)\n");
    sap_printf("        -l pcm_out[bit_depth]      specify pcm output bit depth\n");
    sap_printf("\n");
    sap_printf("for example: sample_decode -i /mnt/dada.sbc -t sbc -c 2 -o /mnt/dada.pcm\n");
    sap_printf("             sample_decode -i /mnt/dada.pcm -t pcm -c 2 -l pcm_out16\n");
    sap_printf("             sample_decode -i /mnt/dada.aac -t aac\n");
    sap_printf("             sample_decode -a 0x105D8000 -l 0x123 -t mp3 -b 32 (read mp3 data from flash)\n");
    sap_printf("\n");
    sap_printf("\n");
    sap_printf("If you want to set player speed\n");
    sap_printf("    usage:  sample_decode track_speed [track_handle] [speed]\n");
    sap_printf("\n");
    sap_printf("for example:\n");
    sap_printf("    set 2x speed:\n");
    sap_printf("        sample_decode track_speed 0x6d000008 2\n");
    sap_printf("\n");
}

td_void sample_decode_get_priv_cfg(const td_char *name, sample_decode_inst *inst)
{
    if (strcasecmp(name, "ogg") == 0) { /* For Vorbis music, -t ogg indicates decode in ogg file mode */
        inst->priv_cfg.vorbis_priv_cfg.raw_file_flag = TD_FALSE;
    } else if (strcasecmp(name, "vorbis") == 0) { /* For Vorbis music, -t vorbis indicates decode in raw file mode */
        inst->priv_cfg.vorbis_priv_cfg.raw_file_flag = TD_TRUE;
    }
}

td_void sample_decode_get_frame_length(const td_char *opt_arg, sample_decode_inst *inst)
{
    /* opus dec frame length */
    inst->priv_cfg.opus_priv_cfg.frame_length = (td_u32)strtoul(opt_arg, TD_NULL, 0);
    sap_printf("opus dec frame length %u\n", inst->priv_cfg.opus_priv_cfg.frame_length);
}

static td_s32 sample_decode_parse_opt(td_char opt, const td_char *opt_arg, sample_decode_inst *inst)
{
    if (opt == 'h') {
        sample_decode_usage();
        return EXT_FAILURE;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return EXT_FAILURE;
    }

    switch (opt) {
        case 'i':
            inst->in_stream = opt_arg;
            parse_audio_params_from_file(inst->in_stream, &inst->acodec_arg.pcm_format);
            break;
        case 'o':
            inst->out_stream = opt_arg;
            break;
        case 's':
            inst->acodec_arg.pcm_format.sample_rate = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'c':
            inst->acodec_arg.pcm_format.channels = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'b':
            inst->acodec_arg.pcm_format.bit_depth = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 't': /* direct return */
            sample_decode_get_priv_cfg(opt_arg, inst);
            return sample_audio_get_acodec_id(opt_arg, &inst->acodec_arg.acodec_id);
        case 'a':
            return sample_decode_parse_addr(opt, opt_arg, inst);
        case 'f':
            sample_decode_get_frame_length(opt_arg, inst);
            break;
        case 'v':
        case 'l':
            break;
        default:
            sap_printf("invalid command line\n");
            sample_decode_usage();
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_void sample_decode_parse_volume(td_char opt, const td_char *opt_arg, sample_decode_ao_inst *inst)
{
    td_double volume;
    td_s32 volume_integer;
    td_s32 volume_decimal;

    if (opt != 'v') {
        return;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return;
    }

    volume = strtod(opt_arg, TD_NULL);

    volume_integer = (td_s32)volume;

    /* Multiply by 1000.0f, decimal part of high preicision gain */
    volume = (volume - (td_double)volume_integer) * 1000.0f;
    volume_decimal = (td_s32)volume;

    inst->gain.integer = volume_integer;
    inst->gain.decimal = volume_decimal;
}

static td_void sample_decode_pcm_out_cfg(const td_char *opt_arg, sample_decode_inst *inst)
{
    td_s32 ret;

    inst->pcm_bit_depth_changed = TD_FALSE;
    ret = get_pcm_out_bitdepth(opt_arg, &inst->pcm_out_format.bit_depth);
    if (ret != EXT_SUCCESS) {
        return;
    }
    inst->pcm_bit_depth_changed = TD_TRUE;
}

static td_void sample_decode_parse_long_opt(td_char opt, const td_char *opt_arg, sample_decode_ao_inst *inst)
{
    if (opt != 'l') {
        return;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return;
    }

    if (strcmp(opt_arg, "once") == 0) {
        inst->play_once = TD_TRUE;
    } else if (strcmp(opt_arg, "tws_music") == 0) {
        inst->audio_scene = UAPI_AUDIO_SCENE_BT_MUSIC_SNK;
        inst->thread_enable = TD_FALSE;
    } else if (strcmp(opt_arg, "sle_music_sink") == 0) {
        inst->audio_scene = UAPI_AUDIO_SCENE_SLE_MUSIC_SINK;
        inst->thread_enable = TD_FALSE;
    } else if (strcmp(opt_arg, "mpeg4_support") == 0) {
        inst->decode_inst[0].profile_constraint_flag = TD_FALSE;
    } else if (strncasecmp(opt_arg, "0x", 2) == 0) { /* compare first 2 characters */
        /* 16 represents Hexadecimal */
        inst->decode_inst[0].stream_size = (td_u32)strtoul(opt_arg, TD_NULL, 16);
    } else if (strncasecmp(opt_arg, "pcm_out", strlen("pcm_out")) == 0) { /* output bit depth */
        sample_decode_pcm_out_cfg(opt_arg, &inst->decode_inst[0]);
    }
}

/* sample_decode_parse_cmd_line: parses the command-line input */
static td_s32 sample_decode_parse_cmd_line(td_s32 argc, td_char *argv[], sample_decode_ao_inst *inst)
{
    parg_state arg_parse;
    td_s32 ret;
    td_u32 i;
    td_u32 decode_num = 0;
    td_u32 step = 0;
    td_char opt;

    /* check input arguments */
    if ((argv == TD_NULL) || (argc < 0x2)) {
        goto out;
    }

    parg_init(&arg_parse);
    while (1) {
        ret = parg_getopt(&arg_parse, argc, argv, "i:o:s:c:b:t:v:l:a:f:h");
        if (ret == -1) {
            break;
        }

        opt = (td_char)ret;
        sample_decode_parse_volume(opt, arg_parse.optarg, inst);
        sample_decode_parse_long_opt(opt, arg_parse.optarg, inst);

        if (opt == 'i') {
            decode_num += step;
            step = 1;
        }

        if (sample_decode_parse_opt(opt, arg_parse.optarg, &inst->decode_inst[decode_num]) != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
    }

    inst->decode_num = decode_num + 1;
    for (i = 0; i < inst->decode_num; i++) {
        inst->decode_inst[i].asc.scene_type = inst->audio_scene;
        inst->decode_inst[i].asc.scene_id = i;
        if (inst->decode_inst[i].acodec_arg.acodec_id == UAPI_ACODEC_ID_MAX) {
            goto out;
        }
    }

    return EXT_SUCCESS;
out:
    sample_decode_usage();
    return EXT_FAILURE;
}

static td_void sample_decode_codec_arg_init(sample_acodec_arg *acodec_arg)
{
    acodec_arg->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    acodec_arg->pcm_format.channels = UAPI_AUDIO_CHANNEL_2;
    acodec_arg->pcm_format.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    acodec_arg->acodec_id = UAPI_ACODEC_ID_MAX;
}

static td_void sample_decode_priv_cfg_init(sample_acodec_dec_priv_cfg *priv_cfg)
{
    (td_void)memset_s(priv_cfg, sizeof(*priv_cfg), 0, sizeof(*priv_cfg));
}

static td_void sample_decode_pcm_out_arg_init(uapi_audio_pcm_format *pcm_format)
{
    pcm_format->sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    pcm_format->channels = UAPI_AUDIO_CHANNEL_2;
    pcm_format->bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
}

static td_void sample_decode_inst_init(sample_decode_ao_inst *inst)
{
    td_u32 i;

    inst->snd_id = ESPLAY_SND_IDX;
    inst->thread_enable = TD_TRUE;
    inst->decode_num = 0;
    inst->gain.integer = 0;
    inst->gain.decimal = 0;
    inst->audio_scene = UAPI_AUDIO_SCENE_LOCAL_MUSIC;
    inst->aef_profile = UAPI_SND_AEF_PROFILE_MUSIC;

    for (i = 0; i < MAX_ADEC_NUM; i++) {
        sample_decode_codec_arg_init(&inst->decode_inst[i].acodec_arg);
        sample_decode_priv_cfg_init(&inst->decode_inst[i].priv_cfg);
        sample_decode_pcm_out_arg_init(&inst->decode_inst[i].pcm_out_format);
        inst->decode_inst[i].stream_addr = 0x0;
        inst->decode_inst[i].stream_size = 0x0;
        inst->decode_inst[i].pcm_bit_depth_changed = TD_FALSE;
        inst->decode_inst[i].profile_constraint_flag = TD_TRUE;
    }
}

static td_s32 sample_decode_track_speed_uint_to_enum(td_u32 speed_uint, uapi_snd_track_speed *speed_enum)
{
    td_u32 i;
    const struct {
        td_u32 speed_uint;
        uapi_snd_track_speed speed_enum;
    } speed_change[] = {
        {1, UAPI_SND_TRACK_1X_SPEED},
        {2, UAPI_SND_TRACK_2X_SPEED},
        {4, UAPI_SND_TRACK_4X_SPEED},
        {8, UAPI_SND_TRACK_8X_SPEED}
    };

    for (i = 0; i < sizeof(speed_change) / sizeof(speed_change[0]); i++) {
        if (speed_uint == speed_change[i].speed_uint) {
            *speed_enum = speed_change[i].speed_enum;
            return EXT_SUCCESS;
        }
    }

    return EXT_FAILURE;
}

static td_s32 sample_decode_track_speed_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    td_handle track_handle;
    td_u32 speed_uint;
    uapi_snd_track_speed speed_enum;
    sample_decode_track_speed_arg *arg = (sample_decode_track_speed_arg *)argv;

    if (argc != (sizeof(sample_decode_track_speed_arg) / sizeof(td_char *))) {
        sample_decode_usage();
        return EXT_FAILURE;
    }

    track_handle = (td_u32)strtol(arg->track_handle, TD_NULL, 16); /* 16: 16进制 */
    speed_uint = (td_u32)strtol(arg->speed, TD_NULL, 0);
    ret = sample_decode_track_speed_uint_to_enum(speed_uint, &speed_enum);
    if (ret != EXT_SUCCESS) {
        sap_err_log_u32(speed_uint);
        sap_err_log_fun(sample_decode_track_speed_uint_to_enum, ret);
        return ret;
    }

    ret = uapi_snd_set_track_speed(track_handle, speed_enum);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_track_speed, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_decode_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_decode_ao_inst *inst = sample_decode_alloc_inst();
    if (inst == TD_NULL) {
        sap_printf("sample_decode_alloc_inst failed\n");
        return EXT_FAILURE;
    }

    sample_decode_inst_init(inst);

    ret = sample_decode_parse_cmd_line(argc, argv, inst);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_decode_ao_open_file(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_ao_open_file, ret);
        goto out0;
    }

    sample_decode_get_def_attr_snd(inst);

    ret = sample_decode_ao_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_ao_open_inst, ret);
        goto out1;
    }

    ret = sample_decode_ao_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_ao_start_inst, ret);
        goto out2;
    }

    sample_decode_show_inst();
    return EXT_SUCCESS;
out2:
    sample_decode_ao_close_inst(inst);
out1:
    sample_decode_ao_close_file(inst);
out0:
    sample_decode_free_inst(inst);
    return ret;
}

static td_s32 sample_decode_exit(td_void)
{
    sample_decode_ao_inst *inst = sample_decode_get_inst(ESPLAY_SND_IDX);
    if (inst != TD_NULL) {
        sample_decode_ao_stop_inst(inst);
        sample_decode_ao_close_inst(inst);
        sample_decode_ao_close_file(inst);
        sample_decode_free_inst(inst);
        sample_decode_show_inst();
    }

    return EXT_SUCCESS;
}

td_s32 sample_decode(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_decode_usage();
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_decode_exit();
    } else if (strcmp(argv[1], "track_speed") == 0) {
        return sample_decode_track_speed_entry(argc, argv);
    } else {
        return sample_decode_entry(argc, argv);
    }
}

static td_void sample_esplay_parse_input_type(sample_escast_arg *escast_arg, sample_decode_ao_inst *ao_inst)
{
    sample_decode_inst *inst = &ao_inst->decode_inst[0];

    ao_inst->thread_enable = TD_TRUE; /* enable thread in default */
    if (escast_arg->in_file != TD_NULL) {
        inst->in_file = escast_arg->in_file;
        sap_fseek(inst->in_file, 0, SEEK_END);
        inst->in_file_len = sap_ftell(inst->in_file);
        sap_rewind(inst->in_file);
    } else if (escast_arg->in_data_addr != 0 && escast_arg->in_data_size != 0) {
        inst->stream_addr = escast_arg->in_data_addr;
        inst->stream_size = escast_arg->in_data_size;
        sample_decode_reader_init(inst);
    } else {
        ao_inst->thread_enable = TD_FALSE; /* Don't need thread for no input data */
    }
}

static td_s32 sample_esplay_init(sample_escast_arg *escast_arg, uapi_snd snd_id)
{
    td_s32 ret;
    sample_decode_inst *inst = TD_NULL;
    sample_decode_ao_inst *ao_inst = sample_decode_alloc_inst();
    if (ao_inst == TD_NULL) {
        sap_printf("sample_decode_alloc_inst failed\n");
        return EXT_FAILURE;
    }

    inst = &ao_inst->decode_inst[0];
    sample_decode_inst_init(ao_inst);
    ao_inst->snd_id = snd_id;
    ao_inst->decode_num = 1;
    ao_inst->thread_enable = TD_FALSE;

    ao_inst->audio_scene = escast_arg->audio_scene;

    /* 指定 adec 输入码流属性 */
    (td_void)memcpy_s(&inst->acodec_arg, sizeof(inst->acodec_arg), escast_arg->in_arg, sizeof(*escast_arg->in_arg));

    /* 指定 track 输出码流属性 */
    (td_void)memcpy_s(&ao_inst->snd_attr, sizeof(ao_inst->snd_attr),
        escast_arg->snd_attr, sizeof(*escast_arg->snd_attr));

    /* Set aef profile type */
    ao_inst->aef_profile = audio_scene_to_aef_profile(escast_arg->audio_scene);

    sample_esplay_parse_input_type(escast_arg, ao_inst);

    inst->priv_cfg = escast_arg->in_arg->dec_priv_cfg;

    ret = sample_decode_ao_open_inst(ao_inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_ao_open_inst, ret);
        goto out1;
    }

    ret = sample_decode_ao_start_inst(ao_inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_ao_start_inst, ret);
        goto out2;
    }

    *escast_arg->player = inst->h_adp_in;
    *escast_arg->snd = ao_inst->h_snd;

    sample_decode_show_inst();
    return EXT_SUCCESS;
out2:
    sample_decode_ao_close_inst(ao_inst);
out1:
    sample_decode_free_inst(ao_inst);
    return ret;
}

static td_void sample_esplay_deinit(uapi_snd snd_id)
{
    sample_decode_ao_inst *ao_inst = sample_decode_get_inst(snd_id);
    if (ao_inst == TD_NULL) {
        return;
    }

    sample_decode_ao_stop_inst(ao_inst);
    sample_decode_ao_close_inst(ao_inst);
    sample_decode_ao_close_file(ao_inst);
    sample_decode_free_inst(ao_inst);
    sample_decode_show_inst();
}

td_s32 sample_escast_open(sample_escast_arg *escast_arg)
{
    return sample_esplay_init(escast_arg, ESCAST_SND_IDX);
}

td_void sample_escast_close(td_void)
{
    sample_esplay_deinit(ESCAST_SND_IDX);
}

td_s32 sample_esplay_open(const sample_acodec_arg *acodec_arg, const td_handle *player,
                          uapi_audio_scene audio_scene)
{
    td_handle h_snd;
    const uapi_snd_attr snd_attr = {
        .port_num = 1,
        .port_attr[0].out_port = SND_OUT_PORT_DEFAULT,
        .channels = SND_OUT_CHANNEL_DEF,
        .bit_depth = SND_OUT_BIT_DEPTH_DEF,
        .sample_rate = SND_OUT_SAMPLE_RATE_DEF,
    };

    sample_escast_arg escast_arg = {
        .player = (td_handle *)player,
        .in_arg = acodec_arg,
        .snd = &h_snd,
        .snd_attr = &snd_attr,
        .audio_scene = audio_scene,
        .in_file = TD_NULL,
        .in_data_addr = 0,
        .in_data_size = 0,
    };

    return sample_esplay_init(&escast_arg, ESPLAY_SND_IDX);
}

td_void sample_esplay_close(td_void)
{
    sample_esplay_deinit(ESPLAY_SND_IDX);
}

td_s32 sample_sle_music_src_open_player(sample_escast_arg *escast_arg)
{
    return sample_esplay_init(escast_arg, UAPI_SND_0);
}

td_void sample_sle_music_src_close_player(td_void)
{
    sample_esplay_deinit(UAPI_SND_0);
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
