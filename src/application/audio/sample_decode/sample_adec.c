/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: decode sample
 * Author: audio
 * Create: 2024-10-17
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "thread_os.h"
#include "osal_list.h"
#include "securec.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_adec.h"
#include "soc_uapi_audio.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"
#include "sample_audio_interface.h"
#include "soc_uapi_audio_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ESPLAY_SND_IDX UAPI_SND_0
#define HEX_FORMATE_STEP 16 /* 16 represents Hexadecimal */
#define SIGN_FOR_ADDR "0x"
#define SIGN_FOR_PLAY "play"
#define BITS_TO_BYTES   8
#define SECOND_TO_2P5MS 400
#define PLAY_SEND_LEN_MIN 128

typedef enum {
    SAMPLE_DATA_IO_IN = 0,
    SAMPLE_DATA_IO_OUT,
    SAMPLE_DATA_IO_DIR_MAX,
} sample_data_io_dir;

typedef enum {
    SAMPLE_PROC_SIZE_ONCE = 0,
    SAMPLE_PROC_SIZE_LOOP = 0xFFFFFFFF,
} sample_proc_size;

typedef enum {
    SAMPLE_DATA_FILE,
    SAMPLE_DATA_ADDR,
    SAMPLE_DATA_ADP,
} sample_data_mode;

typedef union { /* Union for data io handle for different data io type */
    FILE *data_hdl_file; /* Read/Write from/to file */
    data_info data_hdl_addr; /* Read/Write from/to addr(cicle buf) */
    td_handle data_hdl_adp; /* Read/Write from/to adp */
} sample_data_hdl;

typedef struct sample_data_task_t {
    td_bool task_active;
    athread_handle task;
    struct sample_data_io_t {
        const td_char *data_str;
        td_u32 data_size;
        sample_data_mode data_mode;
        sample_data_hdl data_hdl;
        union { /* Union for data process function in thread for input/output scene */
            td_s32 (*stream_in_proc)(struct sample_data_task_t *data_task, uapi_stream_buf *stream);
            td_s32 (*frame_out_proc)(struct sample_data_task_t *data_task, uapi_audio_frame *frame);
        } data_proc; /* Get/Put data from/to data io */
    } data_io; /* file/addr/adp */
} sample_data_task;

typedef struct sample_data_io_t sample_data_io;

typedef struct {
    /* handles of the main scene */
    td_handle h_adec;
    td_handle h_adp_in;
    td_handle h_adp_out;
    /* codec attr */
    sample_acodec_arg acodec_arg;
    /* data process */
    td_u32 exp_proc_size;
    td_u32 act_proc_size;
    td_u32 out_proc_size;
    /* data input/output */
    sample_data_task data_in;
    sample_data_task data_out;
    /* inst list node */
    struct osal_list_head node;
    /* pcm output config */
    td_bool pcm_bit_depth_changed;
    uapi_audio_pcm_format pcm_out_format;
} sample_adec_inst;

static OSAL_LIST_HEAD(g_sample_adec_list);

static td_void sample_adec_show_inst(td_void)
{
    td_u32 cnt = 0;
    sample_adec_inst *inst = TD_NULL;

    if (osal_list_empty(&g_sample_adec_list) != TD_FALSE) {
        sap_alert_log_info("sample_adec all exit!");
        return;
    }

    osal_list_for_each_entry(inst, &g_sample_adec_list, node) {
        cnt++;
        sap_alert_log_u32(cnt);
        sap_alert_log_u32(inst->h_adec);
    }
}

static sample_adec_inst *sample_adec_alloc_inst(td_void)
{
    sample_adec_inst *inst = (sample_adec_inst *)malloc(sizeof(sample_adec_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    (td_void)memset_s(inst, sizeof(*inst), 0, sizeof(sample_adec_inst));

    osal_list_add_tail(&inst->node, &g_sample_adec_list);
    return inst;
}

static td_void sample_adec_free_inst(sample_adec_inst *inst)
{
    if (inst == TD_NULL) {
        return;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static sample_adec_inst *sample_adec_get_inst(td_void)
{
    if (osal_list_empty(&g_sample_adec_list) != TD_FALSE) {
        return TD_NULL;
    }

    return osal_list_first_entry(&g_sample_adec_list, sample_adec_inst, node);
}

static td_s32 sample_adec_player_open(sample_adec_inst *inst)
{
    td_handle h_snd;
    td_handle track;
    const uapi_snd_attr snd_attr = {
        .port_num = 1,
        .port_attr[0].out_port = SND_OUT_PORT_DEFAULT,
        .channels = SND_OUT_CHANNEL_DEF,
        .bit_depth = SND_OUT_BIT_DEPTH_DEF,
        .sample_rate = SND_OUT_SAMPLE_RATE_DEF,
    };

    sample_escast_arg escast_arg = {
        .player = &inst->data_out.data_io.data_hdl.data_hdl_adp,
        .track = &track,
        .in_arg = &inst->acodec_arg,
        .snd = &h_snd,
        .snd_attr = &snd_attr,
        .audio_scene = UAPI_AUDIO_SCENE_UNKNOWN,
        .in_file = TD_NULL,
        .in_data_addr = 0,
        .in_data_size = 0,
        .track_mode = TRACK_MODE_DEFAULT,
    };

    return sample_ao_player_open(&escast_arg, ESPLAY_SND_IDX, TD_FALSE);
}

static td_void sample_adec_data_read_from_addr_reset(circ_buf *cb)
{
    circ_buf_flush(cb);
    circ_buf_update_write_pos(cb, cb->size - sizeof(td_s16));
}

static td_s32 sample_adec_data_read_from_addr(sample_data_task *data_task, uapi_stream_buf *stream)
{
    td_u32 read_len;
    td_u32 read_len_total = 0;
    td_u8 *exp_addr = stream->data;
    const td_u32 exp_len = stream->size;
    circ_buf *cb = &data_task->data_io.data_hdl.data_hdl_addr.cb;

    while (read_len_total < exp_len && (data_task->task_active == TD_TRUE)) {
        read_len = circ_buf_read(cb, exp_addr, (exp_len - read_len_total));
        if (read_len == 0) {
            sap_printf("reset read\n");
            sample_adec_data_read_from_addr_reset(cb);
            continue;
        }
        read_len_total += read_len;
        exp_addr += read_len;
    }

    return read_len_total;
}

static td_s32 sample_adec_data_write_to_addr(sample_data_task *data_task, uapi_audio_frame *frame)
{
    td_u32 buf_size;
    data_info *info = &data_task->data_io.data_hdl.data_hdl_addr;
    circ_buf *cb = &info->cb;

    buf_size = circ_buf_query_free(cb);
    if (buf_size <= 0) {
        sap_printf("Warning: addr to store data is full, size: 0x%x.", info->size);
        return 0;
    }

    buf_size = (buf_size > frame->bits_bytes) ? frame->bits_bytes : buf_size;

    return circ_buf_write(cb, (const td_u8 *)frame->bits_buffer, buf_size);
}

static td_s32 sample_adec_data_read_from_file(sample_data_task *data_task, uapi_stream_buf *stream)
{
    td_u32 read_len;
    td_u32 read_len_total = 0;
    td_u8 *exp_addr = stream->data;
    const td_u32 exp_len = stream->size;
    FILE *file_hdl = data_task->data_io.data_hdl.data_hdl_file;

    while (read_len_total < exp_len && data_task->task_active == TD_TRUE) {
        read_len = sap_fread(exp_addr, 1, (exp_len - read_len_total), file_hdl);
        if (read_len == 0) {
            sap_printf("reset read\n");
            sap_rewind(file_hdl);
            continue;
        }
        read_len_total += read_len;
        exp_addr += read_len;
    }

    return read_len_total;
}

static td_s32 sample_adec_data_write_to_file(sample_data_task *data_task, uapi_audio_frame *frame)
{
    FILE *file_hdl = data_task->data_io.data_hdl.data_hdl_file;

    return sap_fwrite((const td_u8 *)frame->bits_buffer, 1, frame->bits_bytes, file_hdl);
}

static td_s32 sample_adec_data_write_to_adp(sample_data_task *data_task, uapi_audio_frame *frame)
{
    td_s32 ret;
    td_u32 buf_size, len, target_size, frame_size;
    td_handle h_ao_player_adp = data_task->data_io.data_hdl.data_hdl_adp;
    td_s32 *frame_bits_buffer = frame->bits_buffer;

    target_size = frame->bits_bytes;
    frame_size = frame->bits_bytes;

    while (data_task->task_active == TD_TRUE && target_size != 0) {
        uapi_adp_query_free(h_ao_player_adp, &buf_size);
        if (buf_size < PLAY_SEND_LEN_MIN) {
            sap_msleep(THREAD_SLEEP_5MS);
            continue;
        }
        len = circ_buf_min(buf_size, target_size);
        frame->bits_bytes = len;

        ret = uapi_adp_send_frame(h_ao_player_adp, frame);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }
        target_size -= len;
        frame->bits_buffer = (td_s32 *)((td_u8 *)frame->bits_buffer + len);
    }

    frame->bits_bytes = frame_size;
    frame->bits_buffer = frame_bits_buffer;
    return frame_size;
}

static td_s32 sample_adec_data_mode_addr_init(sample_data_io_dir data_io_dir, sample_data_io *data_io)
{
    td_u32 buf = (td_u32)strtoul(data_io->data_str, NULL, HEX_FORMATE_STEP);
    td_u32 size = data_io->data_size;
    circ_buf *cb = &data_io->data_hdl.data_hdl_addr.cb;

    sap_printf("data mode:addr\n");
    data_io->data_mode = SAMPLE_DATA_ADDR;

    data_info_init(&data_io->data_hdl.data_hdl_addr, buf, size);

    if (data_io_dir == SAMPLE_DATA_IO_IN) { /* Set input buffer to full state */
        data_io->data_proc.stream_in_proc = sample_adec_data_read_from_addr;
        sample_adec_data_read_from_addr_reset(cb);
    } else {
        data_io->data_proc.frame_out_proc = sample_adec_data_write_to_addr;
    }

    return EXT_SUCCESS;
}

static td_void sample_adec_data_mode_file_deinit(sample_data_io *data_io)
{
    if (data_io->data_hdl.data_hdl_file != TD_NULL) {
        sap_fclose(data_io->data_hdl.data_hdl_file);
        data_io->data_hdl.data_hdl_file = TD_NULL;
    }
}

static td_s32 sample_adec_data_mode_file_init(sample_data_io_dir data_io_dir, sample_data_io *data_io)
{
    sap_printf("data mode:file\n");
    data_io->data_mode = SAMPLE_DATA_FILE;

    if (data_io_dir == SAMPLE_DATA_IO_IN) { /* open input file */
        data_io->data_hdl.data_hdl_file = sample_audio_open_input_stream(data_io->data_str);
        if (data_io->data_hdl.data_hdl_file == TD_NULL) {
            sap_printf("open file %s error!\n", data_io->data_str);
            return EXT_FAILURE;
        }
        sap_fseek(data_io->data_hdl.data_hdl_file, 0, SEEK_END);
        data_io->data_size = (td_u32)sap_ftell(data_io->data_hdl.data_hdl_file);
        sap_rewind(data_io->data_hdl.data_hdl_file);
        data_io->data_proc.stream_in_proc = sample_adec_data_read_from_file;
    } else if (data_io_dir == SAMPLE_DATA_IO_OUT) { /* open output file */
        data_io->data_hdl.data_hdl_file = sample_audio_open_output_stream(data_io->data_str);
        if (data_io->data_hdl.data_hdl_file == TD_NULL) {
            sap_printf("open file %s error!\n", data_io->data_str);
            return EXT_FAILURE;
        }
        data_io->data_proc.frame_out_proc = sample_adec_data_write_to_file;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_adec_data_mode_adp_init(sample_data_io_dir data_io_dir, sample_data_io *data_io)
{
    sap_printf("data mode:play\n");
    data_io->data_mode = SAMPLE_DATA_ADP;

    if (data_io_dir == SAMPLE_DATA_IO_OUT) {
        data_io->data_proc.frame_out_proc = sample_adec_data_write_to_adp;
    }

    return EXT_SUCCESS;
}

static td_void sample_adec_data_io_deinit(sample_data_io *data_io)
{
    if (data_io->data_mode == SAMPLE_DATA_FILE) {
        sample_adec_data_mode_file_deinit(data_io);
    }
}

static td_s32 sample_adec_data_io_init(sample_data_io_dir data_io_dir, sample_data_io *data_io)
{
    if (data_io->data_str == TD_NULL) {
        return EXT_FAILURE;
    }

    if (strncasecmp(data_io->data_str, SIGN_FOR_ADDR, sizeof(SIGN_FOR_ADDR)) == 0) { /* addr */
        return sample_adec_data_mode_addr_init(data_io_dir, data_io);
    } else if (strncasecmp(data_io->data_str, SIGN_FOR_PLAY, sizeof(SIGN_FOR_PLAY)) == 0) { /* adp/play(AO) */
        return sample_adec_data_mode_adp_init(data_io_dir, data_io);
    } else { /* file */
        return sample_adec_data_mode_file_init(data_io_dir, data_io);
    }
}

static td_void sample_adec_close_data_io(sample_adec_inst *inst)
{
    sample_data_io *data_io_in = &inst->data_in.data_io;
    sample_data_io *data_io_out = &inst->data_out.data_io;

    sample_adec_data_io_deinit(data_io_in);
    sample_adec_data_io_deinit(data_io_out);

    if (data_io_out->data_mode == SAMPLE_DATA_ADP) { /* Send output data to ao_player scene */
        /* ao player exit */
        sample_ao_player_close();
    }
}

static td_s32 sample_adec_open_data_io(sample_adec_inst *inst)
{
    td_s32 ret;
    td_char file_path[FILE_PATH_LEN];
    td_u32 file_path_len = FILE_PATH_LEN - 1;
    sample_data_io *data_io_in = &inst->data_in.data_io;
    sample_data_io *data_io_out = &inst->data_out.data_io;

    /* input data io init */
    ret = sample_adec_data_io_init(SAMPLE_DATA_IO_IN, data_io_in); /* Input data init */
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_adec_data_io_init, ret);
        goto out;
    }

    /* Set output file name same as input file when params lack of '-o' */
    if (data_io_out->data_str == TD_NULL) {
        if (data_io_in->data_str != TD_NULL && data_io_in->data_mode == SAMPLE_DATA_FILE) {
            ret = snprintf_s(file_path, file_path_len, file_path_len, "%s.pcm", data_io_in->data_str);
            if (ret < 0) {
                sap_err_log_fun(snprintf_s, ret);
                sap_printf("Trying to get output file name using %s&%s error!\n", data_io_in->data_str, file_path);
                goto out;
            }
            data_io_out->data_str = file_path; /* this file name will only be used inside this function */
        }
    }

    ret = sample_adec_data_io_init(SAMPLE_DATA_IO_OUT, data_io_out); /* Output data init */
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_adec_data_io_init, ret);
        goto out;
    }

    if (data_io_out->data_str == file_path) { /* Output file name was generate from input file name in default */
        data_io_out->data_str = TD_NULL; /* Deinit this variable after using the temp var before exit this func */
    }

    if (data_io_in->data_mode == SAMPLE_DATA_ADP) {
        sap_err_log_info("Getting input data from adp isn't supported");
        goto out;
    }

    if (data_io_out->data_mode == SAMPLE_DATA_ADP) { /* Send output data to ao_player's input ADP */
        /* Create ao player && get the input adp handle for output data process task */
        ret = sample_adec_player_open(inst);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_adec_player_open, ret);
            goto out;
        }
    }

    return ret;
out:
    sample_adec_close_data_io(inst);
    return ret;
}

static td_void sample_adec_data_in_thread(td_void *args)
{
    td_s32 ret;
    uapi_stream_buf stream;
    td_u32 proc_len;
    sample_adec_inst *inst = (sample_adec_inst *)args;
    sample_data_task *data_task = &inst->data_in;
    sample_data_io *data_io = &data_task->data_io;

    stream.pts = 0LL;
    stream.eos = TD_FALSE;
    stream.pkg_loss = TD_FALSE;

    sap_err_log_info("in thread enter.");
    while (data_task->task_active) {
        ret = uapi_adp_get_buffer(inst->h_adp_in, &stream);
        if (ret != EXT_SUCCESS || stream.size < (FILE_READ_LEN >> 1)) { /* buffer should larger than 512B */
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }

        if (data_io->data_proc.stream_in_proc != NULL) {
            proc_len = (td_u32)data_io->data_proc.stream_in_proc(data_task, &stream);
            if (proc_len != stream.size || stream.size == 0) {
                sap_msleep(THREAD_SLEEP_10MS);
                sap_err_log_info("read es stream failed.");
                continue;
            }
            inst->act_proc_size += proc_len;
            if (inst->exp_proc_size != SAMPLE_PROC_SIZE_ONCE && inst->exp_proc_size != SAMPLE_PROC_SIZE_LOOP &&
                inst->act_proc_size > inst->exp_proc_size) {
                sap_printf("play data size exceed max proc size: 0x%x", inst->act_proc_size);
                data_task->task_active = TD_FALSE;
            }
        }

        ret = uapi_adp_put_buffer(inst->h_adp_in, &stream);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_put_buffer, ret);
        }
    }

    sap_err_log_info("thread exit.");
    athread_set_exit(data_task->task, TD_TRUE);
}

static void sample_adec_process_l2hc_first_frame(sample_adec_inst *inst, uapi_audio_frame *frame)
{
    td_u32 proc_len;
    td_u32 sample_rate;
    td_u32 byte_per_sample;
    td_u32 mute_samples;
    td_u32 mute_data_size;
    uapi_audio_frame tmp_frame;
    sample_data_task *data_task = &inst->data_out;
    sample_data_io *data_io = &data_task->data_io;

    /* L2HC采样率为44k/88k时，帧长需要用48k/96k计算 */
    if (inst->acodec_arg.pcm_format.sample_rate == UAPI_AUDIO_SAMPLE_RATE_44K) {
        sample_rate = UAPI_AUDIO_SAMPLE_RATE_48K;
    } else if (inst->acodec_arg.pcm_format.sample_rate == UAPI_AUDIO_SAMPLE_RATE_88K) {
        sample_rate = UAPI_AUDIO_SAMPLE_RATE_96K;
    } else {
        sample_rate = inst->acodec_arg.pcm_format.sample_rate;
    }

    /* L2HC保存文件时需要去掉第一帧开头2.5ms静音数据 */
    mute_samples = sample_rate / SECOND_TO_2P5MS;
    byte_per_sample = inst->acodec_arg.pcm_format.bit_depth / BITS_TO_BYTES;
    mute_data_size = mute_samples * byte_per_sample * inst->acodec_arg.pcm_format.channels;

    tmp_frame.bits_buffer = frame->bits_buffer + mute_data_size * sizeof(td_s8) / sizeof(td_s32);
    tmp_frame.bits_bytes = frame->bits_bytes - mute_data_size;

    proc_len = (td_u32)data_io->data_proc.frame_out_proc(data_task, &tmp_frame);
    if (proc_len != tmp_frame.bits_bytes) {
        sap_err_log_fun(frame_out_proc, proc_len);
    }
    inst->out_proc_size += proc_len;
}

static td_void process_output_frame(sample_adec_inst *inst, uapi_audio_frame *frame)
{
    td_u32 proc_len;
    sample_data_task *data_task = &inst->data_out;
    sample_data_io *data_io = &data_task->data_io;

    if (inst->out_proc_size == 0 && inst->acodec_arg.acodec_id == UAPI_ACODEC_ID_L2HC) {
        sample_adec_process_l2hc_first_frame(inst, frame);
    } else {
        proc_len = (td_u32)data_io->data_proc.frame_out_proc(data_task, frame);
        if (proc_len != frame->bits_bytes) {
            sap_err_log_fun(frame_out_proc, proc_len);
        }
        inst->out_proc_size += proc_len;
    }
}

static td_void sample_adec_data_out_thread(td_void *args)
{
    td_s32 ret;
    uapi_audio_frame frame;
    sample_adec_inst *inst = (sample_adec_inst *)args;
    sample_data_task *data_task = &inst->data_out;
    sample_data_io *data_io = &data_task->data_io;

    sap_err_log_info("out thread enter.");
    while (data_task->task_active) {
        ret = uapi_adp_acquire_frame(inst->h_adp_out, &frame);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_5MS);
            continue;
        }

        if (data_io->data_proc.frame_out_proc != NULL) {
            process_output_frame(inst, &frame);
        }

        ret = uapi_adp_release_frame(inst->h_adp_out, &frame);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_release_frame, ret);
        }
    }

    athread_set_exit(data_task->task, TD_TRUE);
}

static td_s32 sample_adec_sys_init(td_void)
{
    td_s32 ret;

    ret = uapi_adp_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_init, ret);
        return ret;
    }

    ret = uapi_adec_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_init, ret);
        (td_void)uapi_adp_deinit();
    }

    return ret;
}

static td_void sample_adec_sys_deinit(td_void)
{
    td_s32 ret;

    ret = uapi_adec_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_deinit, ret);
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_deinit, ret);
    }
}

static td_void sample_adec_close_adp_in(sample_adec_inst *inst)
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

static td_void sample_adec_close_adp_out(sample_adec_inst *inst)
{
    td_s32 ret;

    if (inst->h_adp_out == 0) {
        return;
    }

    ret = uapi_adp_destroy(inst->h_adp_out);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_destroy, ret);
    }

    inst->h_adp_out = 0;
}

static td_s32 sample_adec_adp_event_proc(td_handle adp, uapi_adp_event_type event,
    const td_void *param, const td_void *context)
{
    sap_trace_log_h32(adp);
    sap_trace_log_u32(event);

    sap_unused(adp);
    sap_unused(event);
    sap_unused(param);
    sap_unused(context);
    return EXT_SUCCESS;
}

static td_s32 sample_adec_open_adp_in(sample_adec_inst *inst)
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

    ret = uapi_adp_register_event_proc(inst->h_adp_in, (uapi_adp_event_proc)sample_adec_adp_event_proc, TD_NULL);
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

static td_s32 sample_adec_open_adp_out(sample_adec_inst *inst)
{
    td_s32 ret;
    uapi_adp_attr adp_attr;

    ret = uapi_adp_get_def_attr(&adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_get_def_attr, ret);
        return ret;
    }

    inst->h_adp_out = 0;
    ret = uapi_adp_create(&inst->h_adp_out, &adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_create, ret);
        return ret;
    }

    ret = uapi_adp_register_event_proc(inst->h_adp_out, (uapi_adp_event_proc)sample_adec_adp_event_proc, TD_NULL);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_register_event_proc, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_adp_destroy(inst->h_adp_out);
    return ret;
}

static td_void sample_adec_close_adec(sample_adec_inst *inst)
{
    td_s32 ret;

    if (inst->h_adec == 0) {
        return;
    }

    ret = uapi_adec_detach_output(inst->h_adec, inst->h_adp_out);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_detach_output, ret);
    }

    ret = uapi_adec_destroy(inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_destroy, ret);
    }

    inst->h_adec = 0;
}

static td_void sample_adec_attr_cfg(sample_adec_inst *inst, uapi_adec_attr *adec_attr)
{
    adec_attr->codec_id = inst->acodec_arg.acodec_id;
    adec_attr->frame_mode_input = TD_FALSE;
    adec_attr->param.sample_rate = inst->acodec_arg.pcm_format.sample_rate;
    adec_attr->param.channels = inst->acodec_arg.pcm_format.channels;
    adec_attr->param.bit_depth = inst->acodec_arg.pcm_format.bit_depth;

    if (adec_attr->codec_id == UAPI_ACODEC_ID_PCM && inst->pcm_bit_depth_changed == TD_TRUE) {
        inst->pcm_out_format.sample_rate = inst->acodec_arg.pcm_format.sample_rate;
        inst->pcm_out_format.channels = inst->acodec_arg.pcm_format.channels;
        inst->pcm_out_format.sample_per_frame = inst->pcm_out_format.sample_rate / 100; /* 100 --> 10ms */
        adec_attr->param.private_data = &inst->pcm_out_format;
        adec_attr->param.private_data_size = sizeof(inst->pcm_out_format);
    }
}

static td_s32 sample_adec_open_adec(sample_adec_inst *inst)
{
    td_s32 ret;
    uapi_adec_attr adec_attr;

    uapi_audio_scene_config asc = {
        .scene_type = UAPI_AUDIO_SCENE_LOCAL_MUSIC,
        .scene_id = 0,
    };

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

    sample_adec_attr_cfg(inst, &adec_attr);

    ret = uapi_adec_set_attr(inst->h_adec, &adec_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_set_attr, ret);
        goto out;
    }

    ret = uapi_audio_set_audio_scene(inst->h_adec, &asc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_audio_set_audio_scene, ret);
    }

    sample_audio_set_hifi_mode(asc.scene_type);

    ret = uapi_adec_attach_output(inst->h_adec, inst->h_adp_out);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_adec_destroy(inst->h_adec);
    return ret;
}

static td_void sample_adec_close_inst(sample_adec_inst *inst)
{
    sample_adec_close_adp_in(inst);
    sample_adec_close_adec(inst);
    sample_adec_close_adp_out(inst);
    sample_adec_sys_deinit();
}

static td_s32 sample_adec_open_inst(sample_adec_inst *inst)
{
    td_s32 ret;

    ret = sample_adec_sys_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_adec_sys_init, ret);
        return ret;
    }

    /*
     * 数据流向 adp_in --> adec --> adp_out
     * 按照数据流向反方向打开实例
     * 按照数据方向attach output
     */
    ret = sample_adec_open_adp_out(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_adec_open_adp_out, ret);
        return ret;
    }

    ret = sample_adec_open_adec(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_adec_open_adec, ret);
        goto out0;
    }

    ret = sample_adec_open_adp_in(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_adec_open_adp_in, ret);
        goto out1;
    }

    return EXT_SUCCESS;
out1:
    sample_adec_close_adec(inst);
out0:
    sample_adec_close_adp_out(inst);
    return ret;
}

static td_void sample_adec_stop_inst_data_out_athread_exit(sample_adec_inst *inst)
{
    /* Destory && stop data out thread */
    if (inst->data_out.task != TD_NULL) {
        if (inst->data_out.task_active) {
            inst->data_out.task_active = TD_FALSE;
            athread_exit(inst->data_out.task);
        }

        inst->data_out.task = TD_NULL;
    }
}

static td_void sample_adec_stop_inst(sample_adec_inst *inst)
{
    td_s32 ret;

    /* Destory && stop data in thread */
    if (inst->data_in.task != TD_NULL) {
        if (inst->data_in.task_active) {
            inst->data_in.task_active = TD_FALSE;
            athread_exit(inst->data_in.task);
        }

        inst->data_in.task = TD_NULL;
    }

    ret = uapi_adec_stop(inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_stop, ret);
    }

    /* Destory && stop data out thread */
    sample_adec_stop_inst_data_out_athread_exit(inst);
}

static td_s32 sample_adec_start_inst(sample_adec_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    /* Create && start data out thread */
    inst->data_out.task_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_adec_data_out";
    ret = athread_create(&inst->data_out.task, sample_adec_data_out_thread, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        return ret;
    }

    ret = uapi_adec_start(inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_start, ret);
        goto out0;
    }

    /* Create && start data in thread */
    inst->data_in.task_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_adec_data_in";
    ret = athread_create(&inst->data_in.task, sample_adec_data_in_thread, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        goto out1;
    }

    return EXT_SUCCESS;
out1:
    uapi_adec_stop(inst->h_adec);
out0:
    /* Destory && stop data out thread */
    sample_adec_stop_inst_data_out_athread_exit(inst);
    return ret;
}

static td_void sample_adec_usage(td_void)
{
    sap_printf("\n"
           "usage:  sample_adec -i [InputFileName] [OPTIONS]\n");
    sap_printf("\n"
           "        -h        show this usage message and abort\n"
           "        -i        input[file_name/mem_addr]\n");
    sap_printf("\n"
           "        -t        the type of play audio(aac/mp3/opus/lc3/l2hc/sbc/msbc/flac)\n"
           "        -o        output[file_name/mem_addr/play(adp_ao)]\n");
    sap_printf("\n"
           "        -s        samplerate of the input stream(default: 48000)\n"
           "        -c        channels of the input stream(default: 2)\n"
           "        -b        bit_depth of the input stream(default: 16)\n");
    sap_printf("\n"
           "        -l        specify input/output data size(Specify it just after -i/-o options)\n");
    sap_printf("\n"
           "        -lpcm_out[bit_depth]  specify output pcm bit depth(default: input bit depth)\n");
    sap_printf("\n"
           "        -m        specify sample proc mode:once/loop/certain_size\n"
           "        -m once   play input stream once and deal eos event\n"
           "        -m 0x1000 specify stream size to read(byte)\n");
    sap_printf("\n"
           "for example: ./sample_adec -i /mnt/dada.sbc -t sbc -o /mnt/dada.pcm\n"
           "             ./sample_adec -i 0x105D8000 -l 0x10 -o 0x58200000 -l 0x10 -t mp3 -b 32\n");
}

static td_void sample_adec_get_pcm_out_cfg(const td_char *opt_arg, sample_adec_inst *inst)
{
    td_s32 ret;

    inst->pcm_bit_depth_changed = TD_FALSE;
    ret = get_pcm_out_bitdepth(opt_arg, &inst->pcm_out_format.bit_depth);
    if (ret != EXT_SUCCESS) {
        return;
    }
    inst->pcm_bit_depth_changed = TD_TRUE;
}

static td_void sample_adec_parse_long_opt(const td_char *opt_arg, sample_adec_inst *inst, td_u32 *data_size)
{
    if (strncasecmp(opt_arg, "pcm_out", strlen("pcm_out")) == 0) { /* output bit depth */
        sample_adec_get_pcm_out_cfg(opt_arg, inst);
    } else if (data_size != TD_NULL) {
        *data_size = (td_u32)strtoul(opt_arg, TD_NULL, HEX_FORMATE_STEP);
    }
}

static td_s32 sample_adec_parse_opt(td_char opt, const td_char *opt_arg, sample_adec_inst *inst, td_u32 *data_size)
{
    if (opt == 'h') {
        sample_adec_usage();
        return EXT_FAILURE;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return EXT_FAILURE;
    }

    switch (opt) {
        case 'i':
            inst->data_in.data_io.data_str = opt_arg;
            parse_audio_params_from_file(inst->data_in.data_io.data_str, &inst->acodec_arg.pcm_format);
            break;
        case 'o':
            inst->data_out.data_io.data_str = opt_arg;
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
            return sample_audio_get_acodec_id(opt_arg, &inst->acodec_arg.acodec_id);
        case 'l':
            sample_adec_parse_long_opt(opt_arg, inst, data_size);
            break;
        case 'm':
            if (strcmp(opt_arg, "once") == 0) {
                inst->exp_proc_size = SAMPLE_PROC_SIZE_ONCE;
            } else if (strncasecmp(opt_arg, "0x", 2) == 0) { /* compare first 2 characters */
                inst->exp_proc_size = (td_u32)strtoul(opt_arg, TD_NULL, HEX_FORMATE_STEP);
            } else {
                inst->exp_proc_size = SAMPLE_PROC_SIZE_LOOP;
            }
            break;
        default:
            sap_printf("invalid command line\n");
            sample_adec_usage();
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

/* sample_adec_parse_cmd_line: parses the command-line input */
static td_s32 sample_adec_parse_cmd_line(td_s32 argc, td_char *argv[], sample_adec_inst *inst)
{
    td_s32 ret;
    td_char opt;
    td_u32 *data_size = TD_NULL;
    parg_state arg_parse;

    /* check input arguments */
    if ((argv == TD_NULL) || (argc < 0x2)) {
        goto out;
    }

    parg_init(&arg_parse);
    while (1) {
        ret = parg_getopt(&arg_parse, argc, argv, "i:o:s:c:b:t:l:m:h");
        if (ret == -1) {
            break;
        }

        opt = (td_char)ret;

        if (opt == 'i') {
            data_size = &inst->data_in.data_io.data_size; /* -l after -i is for input data size */
        }

        if (opt == 'o') {
            data_size = &inst->data_out.data_io.data_size; /* -l after -o is for output data size */
        }

        if (sample_adec_parse_opt(opt, arg_parse.optarg, inst, data_size) != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
    }

    if (inst->acodec_arg.acodec_id == UAPI_ACODEC_ID_MAX) {
        goto out;
    }

    return EXT_SUCCESS;
out:
    sample_adec_usage();
    return EXT_FAILURE;
}

static td_void sample_adec_codec_arg_init(sample_acodec_arg *acodec_arg)
{
    acodec_arg->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    acodec_arg->pcm_format.channels = UAPI_AUDIO_CHANNEL_2;
    acodec_arg->pcm_format.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    acodec_arg->acodec_id = UAPI_ACODEC_ID_MAX;
}

static td_void sample_adec_pcm_out_arg_init(uapi_audio_pcm_format *pcm_format)
{
    pcm_format->sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    pcm_format->channels = UAPI_AUDIO_CHANNEL_2;
    pcm_format->bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
}

static td_void sample_adec_inst_init(sample_adec_inst *inst)
{
    inst->exp_proc_size = SAMPLE_PROC_SIZE_LOOP; /* default in loop mode */
    sample_adec_pcm_out_arg_init(&inst->pcm_out_format);
    sample_adec_codec_arg_init(&inst->acodec_arg);
}

static td_s32 sample_adec_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_adec_inst *inst = sample_adec_alloc_inst();
    sap_trace_log_h32(inst);

    if (inst == TD_NULL) {
        sap_printf("sample_adec_alloc_inst failed\n");
        return EXT_FAILURE;
    }

    sample_adec_inst_init(inst);

    ret = sample_adec_parse_cmd_line(argc, argv, inst);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_adec_open_data_io(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_adec_open_data_io, ret);
        goto out0;
    }

    ret = sample_adec_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_adec_open_inst, ret);
        goto out1;
    }

    ret = sample_adec_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_adec_start_inst, ret);
        sample_adec_stop_inst(inst);
        goto out2;
    }

    sample_adec_show_inst();
    return EXT_SUCCESS;
out2:
    sample_adec_close_inst(inst);
out1:
    sample_adec_close_data_io(inst);
out0:
    sample_adec_free_inst(inst);
    return ret;
}

static td_s32 sample_adec_exit(td_void)
{
    sample_adec_inst *inst = sample_adec_get_inst();
    sap_trace_log_h32(inst);
    if (inst != TD_NULL) {
        sample_adec_stop_inst(inst);
        sample_adec_close_inst(inst);
        sample_adec_close_data_io(inst);
        sample_adec_free_inst(inst);
        sample_adec_show_inst();
    }

    return EXT_SUCCESS;
}

td_s32 sample_adec(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_adec_usage();
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_adec_exit();
    } else {
        return sample_adec_entry(argc, argv);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
