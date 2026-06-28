/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: sea sample
 * Author: audio
 * Create: 2026-05-13
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "thread_os.h"
#include "osal_list.h"
#include "chip_type.h"
#include "securec.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_sea.h"
#include "soc_uapi_audio_sys.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"
#include "sample_audio_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define FRAME_BUFFER_SIZE 0x1000
#define DEFAULT_FRAME_SIZE 640

typedef struct sample_sea_only_inst {
    td_bool task_active;
    td_bool task_out_active;
    athread_handle task_in;
    athread_handle task_out;

    const td_char *in_stream;
    const td_char *out_stream;
    FILE *h_file_in;
    FILE *h_file_out;
    td_u8 frame_buf[FRAME_BUFFER_SIZE];
    td_u32 frame_size;
    td_u32 (*read_frame)(struct sample_sea_only_inst *inst, uapi_audio_frame *frame);
    td_u32 (*write_frame)(struct sample_sea_only_inst *inst, const td_u8 *from, td_u32 len);

    /* handles of the main scene */
    td_handle h_sea;
    td_handle h_adp_in;
    td_handle h_adp_out;

    td_u32 data_send_interval;

    struct osal_list_head node;
} sample_sea_only_inst;

static OSAL_LIST_HEAD(g_sample_sea_only_list);

static sample_sea_only_inst *sample_sea_only_alloc_inst(td_void)
{
    sample_sea_only_inst *inst = (sample_sea_only_inst *)malloc(sizeof(sample_sea_only_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    (td_void)memset_s(inst, sizeof(*inst), 0, sizeof(sample_sea_only_inst));

    osal_list_add_tail(&inst->node, &g_sample_sea_only_list);
    return inst;
}

static td_void sample_sea_only_free_inst(sample_sea_only_inst *inst)
{
    if (inst == TD_NULL) {
        return;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static sample_sea_only_inst *sample_sea_only_get_inst(td_void)
{
    if (osal_list_empty(&g_sample_sea_only_list) != TD_FALSE) {
        return TD_NULL;
    }

    return osal_list_first_entry(&g_sample_sea_only_list, sample_sea_only_inst, node);
}

static td_void sample_sea_only_usage(td_void)
{
    sap_printf("    usage: sample_sea_only -i [InputFileName] -o [OutputFileName] -d [DataSendingInterval]\n");
    sap_printf("\n");
    sap_printf("        -i     input file name\n");
    sap_printf("        -o     output file name\n");
    sap_printf("        -d     data sending interval(unit: ms)\n");
    sap_printf("\n");
    sap_printf("examples:\n");
    sap_printf("  sample_sea_only -i /user/nc_test.pcm -o /user/nc_test_ainr.pcm -d 100\n");
}

static td_s32 sample_sea_only_sys_init(td_void)
{
    td_s32 ret;

    ret = uapi_adp_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_init, ret);
        return ret;
    }

    ret = uapi_sea_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_init, ret);
        (td_void)uapi_adp_deinit();
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_sea_only_sys_deinit(td_void)
{
    td_s32 ret;

    ret = uapi_sea_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_deinit, ret);
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_deinit, ret);
    }
}

static td_void init_sea_eng(uapi_sea_eng_sel *sea_eng)
{
    (td_void)memset_s(sea_eng, sizeof(*sea_eng), 0, sizeof(*sea_eng));
    sea_eng->afe.type = UAPI_SEA_AFE_SEE;
    sea_eng->afe.lib_id = UAPI_SEA_LIB_SEE;
    sea_eng->aai[0].type = UAPI_SEA_AAI_KWS;
    sea_eng->aai[0].lib_id = UAPI_SEA_LIB_KWS;
}

static td_void sample_sea_only_close_adp_input(sample_sea_only_inst *inst)
{
    td_s32 ret;

    if (inst->h_adp_in == 0) {
        return;
    }

    ret = uapi_adp_detach_output(inst->h_adp_in, inst->h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_detach_output, ret);
    }

    ret = uapi_adp_destroy(inst->h_adp_in);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_destroy, ret);
    }

    inst->h_adp_in = 0;
}

static td_s32 sample_sea_only_open_adp_input(sample_sea_only_inst *inst)
{
    td_s32 ret;
    td_s32 err;
    uapi_adp_attr adp_attr;

    inst->h_adp_in = 0;
    ret = uapi_adp_get_def_attr(&adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_get_def_attr, ret);
        return ret;
    }

    ret = uapi_adp_create(&inst->h_adp_in, &adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_create, ret);
        return ret;
    }

    ret = uapi_adp_attach_output(inst->h_adp_in, inst->h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    err = uapi_adp_destroy(inst->h_adp_in);
    if (err != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_destroy, err);
    }

    inst->h_adp_in = 0;
    return ret;
}

static td_void sample_sea_only_close_adp_output(sample_sea_only_inst *inst)
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

static td_s32 sample_sea_only_open_adp_output(sample_sea_only_inst *inst)
{
    td_s32 ret;
    uapi_adp_attr adp_attr;

    inst->h_adp_out = 0;
    ret = uapi_adp_get_def_attr(&adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_get_def_attr, ret);
        return ret;
    }

    ret = uapi_adp_create(&inst->h_adp_out, &adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_create, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_sea_only_open_sea(sample_sea_only_inst *inst)
{
    td_s32 ret;
    uapi_sea_eng_sel sea_eng;
    uapi_sea_attr sea_attr;

    init_sea_eng(&sea_eng);

    ret = uapi_sea_load_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_load_engine, ret);
        return ret;
    }

    ret = uapi_sea_get_default_attr(&sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_get_default_attr, ret);
        goto out0;
    }

    sea_attr.ref_pcm.channels = 1;

    ret = uapi_sea_create(&inst->h_sea, &sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_create, ret);
        goto out0;
    }

    ret = uapi_sea_attach_output(inst->h_sea, UAPI_SEA_OUTPUT_ASR_SRC, inst->h_adp_out);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_attach_output, ret);
        goto out1;
    }

    return EXT_SUCCESS;

out1:
    (td_void)uapi_sea_destroy(inst->h_sea);
out0:
    (td_void)uapi_sea_unload_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");

    return ret;
}

static td_void sample_sea_only_close_sea(sample_sea_only_inst *inst)
{
    td_s32 ret;

    ret = uapi_sea_detach_output(inst->h_sea, UAPI_SEA_OUTPUT_ASR_SRC, inst->h_adp_out);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_detach_output, ret);
    }

    ret = uapi_sea_destroy(inst->h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_destroy, ret);
    }

    ret = uapi_sea_unload_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_unload_engine, ret);
    }
}

static td_s32 sample_sea_only_open_inst(sample_sea_only_inst *inst)
{
    td_s32 ret;

    ret = sample_sea_only_open_adp_output(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_only_open_adp_output, ret);
        return ret;
    }

    ret = sample_sea_only_open_sea(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_only_open_sea, ret);
        goto out0;
    }

    ret = sample_sea_only_open_adp_input(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_only_open_adp_input, ret);
        goto out1;
    }

    return EXT_SUCCESS;

out1:
    (td_void)sample_sea_only_close_sea(inst);
out0:
    (td_void)sample_sea_only_close_adp_output(inst);

    return ret;
}

static td_void sample_sea_only_close_inst(sample_sea_only_inst *inst)
{
    (td_void)sample_sea_only_close_adp_input(inst);
    (td_void)sample_sea_only_close_sea(inst);
    (td_void)sample_sea_only_close_adp_output(inst);
}

static td_void sample_sea_only_build_frame(const sample_sea_only_inst *inst, uapi_audio_frame *frame)
{
    clear_obj(frame);
    frame->interleaved = TD_TRUE;
    frame->channels = UAPI_AUDIO_CHANNEL_2;
    frame->bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    frame->sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    frame->bits_buffer = (td_s32 *)inst->frame_buf;
    frame->bits_bytes = DEFAULT_FRAME_SIZE;
}

static td_void sample_sea_only_adp_data_send_proc(td_void *args)
{
    td_s32 ret;
    td_u32 len;
    td_u32 read_len;
    uapi_audio_frame input_frame;
    sample_sea_only_inst *inst = (sample_sea_only_inst *)args;

    sample_sea_only_build_frame(inst, &input_frame);

    sap_err_log_info("thread enter.");
    while (inst->task_active) {
        uapi_adp_query_free(inst->h_adp_in, &read_len);
        if (read_len < FILE_READ_LEN_MIN) {
            sap_msleep(THREAD_SLEEP_5MS);
            continue;
        }

        read_len = circ_buf_min(read_len, inst->frame_size);
        input_frame.bits_bytes = read_len;

        len = inst->read_frame(inst, &input_frame);
        if (len == 0) {
            continue;
        }
        input_frame.bits_bytes = len;

        while (inst->task_active) {
            ret = uapi_adp_send_frame(inst->h_adp_in, &input_frame);
            if (ret != EXT_SUCCESS) {
                sap_msleep(THREAD_SLEEP_10MS);
                continue;
            }

            break;
        }

        if (inst->data_send_interval != 0) {
            /* FPGA环境，由于性能限制，需要控制数据的发送速度 */
            sap_msleep(inst->data_send_interval);
        }
    }

    sap_err_log_info("thread exit.");
    athread_set_exit(inst->task_in, TD_TRUE);
}

static td_void sample_sea_only_adp_data_recv_proc(td_void *args)
{
    td_s32 ret;
    td_u32 len;
    uapi_stream_buf stream;
    sample_sea_only_inst *inst = (sample_sea_only_inst *)args;

    sap_err_log_info("thread enter.");
    while (inst->task_out_active) {
        ret = uapi_adp_acquire_stream(inst->h_adp_out, &stream);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }

        len = inst->write_frame(inst, (const td_u8 *)stream.data, stream.size);
        if (len == 0) {
            inst->task_out_active = TD_FALSE;
            break;
        }

        ret = uapi_adp_release_stream(inst->h_adp_out, &stream);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_release_stream, ret);
        }

        sap_msleep(THREAD_SLEEP_10MS);
    }

    sap_err_log_info("thread exit.");
    athread_set_exit(inst->task_out, TD_TRUE);
}

static td_void sample_sea_only_stop_inst(sample_sea_only_inst *inst)
{
    td_s32 ret;

    inst->task_active = TD_FALSE;
    if (inst->task_in != TD_NULL) {
        athread_exit(inst->task_in);
        inst->task_in = TD_NULL;
    }

    inst->task_out_active = TD_FALSE;
    if (inst->task_out != TD_NULL) {
        athread_exit(inst->task_out);
        inst->task_out = TD_NULL;
    }

    ret = uapi_sea_stop(inst->h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_stop, ret);
    }

    sample_gui_aef_delete_handle(inst->h_sea);
}

static td_s32 sample_sea_only_start_inst(sample_sea_only_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    ret = uapi_sea_start(inst->h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_start, ret);
        return ret;
    }

    sample_gui_aef_add_handle(GUI_AEF_SEA, inst->h_sea);

    /* Create Task for ADP instance to send audio data */
    inst->task_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_sea_only_send";
    ret = athread_create(&inst->task_in, sample_sea_only_adp_data_send_proc, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        goto out0;
    }

    inst->task_out_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_sea_only_recv";
    ret = athread_create(&inst->task_out, sample_sea_only_adp_data_recv_proc, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        goto out1;
    }

    return EXT_SUCCESS;
out1:
    inst->task_active = TD_FALSE;
    if (inst->task_in != TD_NULL) {
        athread_exit(inst->task_in);
        inst->task_in = TD_NULL;
    }
out0:
    sample_gui_aef_delete_handle(inst->h_sea);
    (td_void)uapi_sea_stop(inst->h_sea);
    return ret;
}

static td_u32 sample_sea_only_write_file(sample_sea_only_inst *inst, const td_u8 *from, td_u32 len)
{
    td_u32 write;

    write = sap_fwrite(from, 1, len, inst->h_file_out);
    return write;
}

static td_u32 sample_sea_only_read_file(sample_sea_only_inst *inst, uapi_audio_frame *frame)
{
    td_u32 len;

    len = sap_fread(frame->bits_buffer, 1, frame->bits_bytes, inst->h_file_in);
    if (len == 0) {
        sap_printf("reset read\n");
        sap_rewind(inst->h_file_in);
        sap_msleep(THREAD_SLEEP_5MS);
        return 0;
    }

    return len;
}

static td_void sample_sea_only_close_file(sample_sea_only_inst *inst)
{
    if (inst->h_file_out != TD_NULL) {
        sap_fclose(inst->h_file_out);
        inst->h_file_out = TD_NULL;
    }

    if (inst->h_file_in != TD_NULL) {
        sap_fclose(inst->h_file_in);
        inst->h_file_in = TD_NULL;
    }
}

static td_s32 sample_sea_only_open_file(sample_sea_only_inst *inst)
{
    /* open input file */
    if (inst->in_stream != TD_NULL) {
        inst->h_file_in = sample_audio_open_input_stream(inst->in_stream);
        if (inst->h_file_in == TD_NULL) {
            sap_printf("open file %s error!\n", inst->in_stream);
            return EXT_FAILURE;
        }
        sap_alert_log_str(inst->in_stream);
    }

    /* open output file */
    if (inst->out_stream != TD_NULL) {
        inst->h_file_out = sample_audio_open_output_stream(inst->out_stream);
        if (inst->h_file_out == TD_NULL) {
            sap_printf("open file %s error!\n", inst->out_stream);
            if (inst->h_file_in != TD_NULL) {
                sap_fclose(inst->h_file_in);
                inst->h_file_in = TD_NULL;
            }
            return EXT_FAILURE;
        }
        sap_alert_log_str(inst->out_stream);
    }

    inst->read_frame = sample_sea_only_read_file;
    inst->write_frame = sample_sea_only_write_file;
    sap_err_log_info("sample_sea_only read data from file.");

    return EXT_SUCCESS;
}

static td_s32 sample_sea_only_parse_opt(td_char opt, const td_char *opt_arg, sample_sea_only_inst *inst)
{
    if (opt == 'h') {
        sample_sea_only_usage();
        return EXT_FAILURE;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return EXT_FAILURE;
    }

    switch (opt) {
        case 'i':
            inst->in_stream = opt_arg;
            break;
        case 'o':
            inst->out_stream = opt_arg;
            break;
        case 'd':
            inst->data_send_interval = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        default:
            sap_printf("invalid command line\n");
            sample_sea_only_usage();
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_sea_only_parse_cmd_line(td_s32 argc, td_char *argv[], sample_sea_only_inst *inst)
{
    td_s32 ret;
    td_char opt;
    parg_state arg_parse;

    /* check input arguments */
    if ((argv == TD_NULL) || (argc < 0x2)) {
        goto out;
    }

    parg_init(&arg_parse);
    while (1) {
        ret = parg_getopt(&arg_parse, argc, argv, "i:o:d:h");
        if (ret == -1) {
            break;
        }

        opt = (td_char)ret;
        if (sample_sea_only_parse_opt(opt, arg_parse.optarg, inst) != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
out:
    sample_sea_only_usage();
    return EXT_FAILURE;
}

static td_void sample_sea_only_inst_init(sample_sea_only_inst *inst)
{
    inst->frame_size = DEFAULT_FRAME_SIZE;
}

static td_s32 sample_sea_only_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_sea_only_inst *inst = sample_sea_only_alloc_inst();

    if (inst == TD_NULL) {
        sap_printf("sample_sea_only_alloc_inst failed\n");
        return EXT_FAILURE;
    }

    sample_sea_only_inst_init(inst);

    ret = sample_sea_only_parse_cmd_line(argc, argv, inst);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_sea_only_open_file(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_only_open_file, ret);
        goto out0;
    }

    ret = sample_sea_only_sys_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_only_sys_init, ret);
        goto out1;
    }

    ret = sample_sea_only_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_only_open_inst, ret);
        goto out2;
    }

    ret = sample_sea_only_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_only_start_inst, ret);
        goto out3;
    }

    return EXT_SUCCESS;
out3:
    sample_sea_only_close_inst(inst);
out2:
    sample_sea_only_sys_deinit();
out1:
    sample_sea_only_close_file(inst);
out0:
    sample_sea_only_free_inst(inst);
    return ret;
}

static td_s32 sample_sea_only_exit(td_void)
{
    sample_sea_only_inst *inst = sample_sea_only_get_inst();
    if (inst != TD_NULL) {
        sample_sea_only_stop_inst(inst);
        sample_sea_only_close_inst(inst);
        sample_sea_only_sys_deinit();
        sample_sea_only_close_file(inst);
        sample_sea_only_free_inst(inst);
    }

    return EXT_SUCCESS;
}

td_s32 sample_sea_only(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_sea_only_usage();
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_sea_only_exit();
    } else {
        return sample_sea_only_entry(argc, argv);
    }

    return EXT_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
