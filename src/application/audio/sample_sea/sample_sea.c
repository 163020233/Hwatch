/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: sea sample
 * Author: audio
 * Create: 2019-09-17
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "thread_os.h"
#include "chip_type.h"
#include "securec.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_ai.h"
#include "soc_uapi_sea.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"
#include "sample_audio_interface.h"
#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
#include "sample_asr.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SEA_PHRASE_COUNT 20
#define ARGV_FILE_SIZE_INDEX 1
#define ARGV_MIC_TYPE_INDEX 4
#define ARGV_STORAGE_MODE_INDEX 5
#define ARGV_ASR_ENABLE_INDEX 6
#define ARGV_ASR_DFX_ENABLE_INDEX 7
#define SAMPLERATE_TRANS 1000
#define SEA_AI_REF_CH_NUM UAPI_AUDIO_CHANNEL_1

#define AI_FRAME_DURATION_MS 10 /* 10ms */
#define SEA_ADP_OUT_FRAME_CNT 3 /* 3 frames: 60ms */
#define SEA_DFX_DATA_CACHING_FRAME_CNT 100 /* 100 frames: 2000ms=2s */

#define SEA_PHRASE_SAVE_ONLY_WAKEUP /* Only save wake up phase when wake up event happened */

typedef struct {
    td_char *file_size;
    td_char *storage_type;
    td_char *lib_type;
    td_char *ai_port;
} sample_sea_params;

typedef enum {
    DATA_STORE_NONE = 0,
    DATA_STORE_FILE = 1,
    DATA_STORE_ADDR = 2,
} sample_sea_data_storage_type;

typedef enum {
    DATA_STORE_MODE_ALL = 0,
    DATA_STORE_MODE_PHASE = 1,
} sample_sea_data_storage_mode;

typedef struct {
    td_handle h_ai;
    uapi_ai_port ai_port;
    uapi_audio_pcm_format pcm_attr;
} sample_sea_ai_inst;

typedef struct {
    td_u32 phs_count;
    td_u32 phs_match_cnt[SEA_PHRASE_COUNT];
    uapi_sea_item phrase[SEA_PHRASE_COUNT];
} sample_sea_phs_sets;

typedef struct {
    td_handle h_sea;
    uapi_sea_item_sets item_sets;
    sample_sea_phs_sets phs_sets;
    td_u8 lib_id;
} sample_sea_inst;

typedef struct {
    sample_sea_data_storage_type data_storage_flag;
    sample_sea_data_storage_mode data_storage_mode;
    td_char *name;
    FILE *h_file;
    td_u32 file_size;
    td_u32 file_limit_size;
    td_char file_path[FILE_PATH_LEN];

    td_handle h_adp;
    td_bool task_enable;
    athread_handle task;
    td_char *task_name;
    td_bool task_save_data_enable;
    athread_handle task_save_data;
    td_char *task_save_data_name;

    data_info data_caching;
    td_u32 data_caching_size;
    td_u8 *frame_buf;
    uapi_audio_pcm_format data_pcm_attr;
    td_bool wake_up_flag;
    td_bool asr_enable;
    td_bool asr_dfx;
    td_void *asr_st;
    td_char *scratch_buffer;
} sample_sea_task_inst;

typedef struct {
    sample_sea_ai_inst ai_inst;
    sample_sea_inst sea_inst;
    sample_sea_task_inst ai_storage;
    sample_sea_task_inst sea_storage;
    sample_sea_task_inst asr_inst;
    td_bool running;
} sample_sea_context;

static sample_sea_context g_sample_sea_ctx;

typedef struct {
    td_u8 *data_addr;
    td_u32 data_size;
    td_u32 write_pos;
} sample_sea_write_info;

#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
static td_s16 __attribute((aligned(8))) g_pps_data_sea[CHAN_NUM_MAX][FUZZ_FRAME_LEN] = { 0 }; /* 8 bytes aligned */
#endif
static sample_sea_write_info g_sample_sea_write_info;

static td_s32 sample_sea_event_proc(td_handle sea, uapi_sea_event_type event, td_void *param, td_void *context)
{
    td_u8 i;
    sample_sea_context *ctx = (sample_sea_context *)context;
    sample_sea_inst *inst = (sample_sea_inst *)&ctx->sea_inst;
    uapi_sea_kws_event_param *event_param = (uapi_sea_kws_event_param *)param;
    sample_sea_phs_sets *phs_sets = &inst->phs_sets;

    if (event == UAPI_SEA_EVENT_KWS_MATCH) {
        if (event_param->phrase_id == 0) {
            return EXT_SUCCESS;
        }
        phs_sets->phs_match_cnt[event_param->phrase_id]++;

        ctx->ai_storage.wake_up_flag = TD_TRUE;
        ctx->sea_storage.wake_up_flag = TD_TRUE;
        ctx->asr_inst.wake_up_flag = TD_TRUE;

        sap_printf("\n--------------------------------------------------");
        sap_printf("cmdid %u\n", event_param->phrase_id);
        for (i = 0; i < phs_sets->phs_count; i++) {
            if (event_param->phrase_id == phs_sets->phrase[i].id) {
                sap_printf("cmd: <%s>, cnt: %u\n", phs_sets->phrase[i].name, phs_sets->phs_match_cnt[i]);
            } else {
                sap_printf("cmd: [%s], cnt: %u\n", phs_sets->phrase[i].name, phs_sets->phs_match_cnt[i]);
            }
        }
        sap_printf("\n--------------------------------------------------");
    }

    sap_unused(sea);
    return EXT_SUCCESS;
}

static td_u32 sample_sea_write_buffer(sample_sea_write_info *write_info, td_u8 *src, td_u32 size)
{
    td_u32 real_write;
    td_s32 ret;

    if (size > write_info->data_size) {
        sap_printf("data not enought\n");
        return EXT_FAILURE;
    }

    if (write_info->write_pos >= write_info->data_size) {
        write_info->write_pos = 0;
        real_write = size;
    } else if ((write_info->write_pos + size) > write_info->data_size) {
        real_write = write_info->data_size - write_info->write_pos;
    } else {
        real_write = size;
    }

    ret = memcpy_s(write_info->data_addr + write_info->write_pos, size, src, real_write);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    write_info->write_pos += real_write;

    return real_write;
}

static td_s32 sample_sea_sys_init(td_void)
{
    td_s32 ret;

    ret = uapi_adp_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_init, ret);
        return ret;
    }

    ret = uapi_ai_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_init, ret);
        goto out0;
    }

    ret = uapi_sea_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_init, ret);
        goto out1;
    }

    return EXT_SUCCESS;

out1:
    (td_void)uapi_ai_deinit();

out0:
    (td_void)uapi_adp_deinit();

    return ret;
}

static td_void sample_sea_sys_deinit(td_void)
{
    td_s32 ret;

    ret = uapi_sea_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_deinit, ret);
    }

    ret = uapi_ai_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_deinit, ret);
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_deinit, ret);
    }
}

static td_s32 sample_sea_parse_storage_size(td_char *param, sample_sea_context *ctx)
{
    td_u32 storage_size = (td_u32)strtol(param, TD_NULL, 0);
    if (storage_size == 0 || storage_size > 500) { /* 500 max storage file size */
        sap_err_log_ret(storage_size);
        return EXT_FAILURE;
    }

    ctx->ai_storage.file_limit_size = storage_size * 1024 * 1024;  /* 1K = 1024 Bytes */
    ctx->sea_storage.file_limit_size = storage_size * 1024 * 1024; /* 1K = 1024 Bytes */
    return EXT_SUCCESS;
}

static td_s32 sample_sea_parse_storage_type(td_char *param, sample_sea_context *ctx)
{
    td_s32 ret;

    if (strcasecmp(param, "null") == 0) {
        ctx->ai_storage.data_storage_flag = DATA_STORE_NONE;
        ctx->sea_storage.data_storage_flag = DATA_STORE_NONE;
        sap_alert_log_info("Data save type:No saving data");
        return EXT_SUCCESS;
    } else if (strcasecmp(param, "addr") == 0) {
        ctx->ai_storage.data_storage_flag = DATA_STORE_ADDR;
        ctx->sea_storage.data_storage_flag = DATA_STORE_ADDR;
        sap_alert_log_info("Data save type:Saving data to address");
        return EXT_SUCCESS;
    } else {
        ctx->ai_storage.data_storage_flag = DATA_STORE_FILE;
        ctx->sea_storage.data_storage_flag = DATA_STORE_FILE;
        sap_alert_log_info("Data save type:Saving data to file");
    }

    ret = strncpy_s(ctx->ai_storage.file_path, sizeof(ctx->ai_storage.file_path) - 1, param, strlen(param));
    if (ret != EOK) {
        sap_err_log_fun(strncpy_s, ret);
        return ret;
    }

    ret = strncpy_s(ctx->sea_storage.file_path, sizeof(ctx->sea_storage.file_path) - 1, param, strlen(param));
    if (ret != EOK) {
        sap_err_log_fun(strncpy_s, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_sea_parse_storage_attr(sample_sea_context *ctx)
{
    td_s32 ret;

    ret = memcpy_s(&ctx->ai_storage.data_pcm_attr, sizeof(ctx->ai_storage.data_pcm_attr),
                   &ctx->ai_inst.pcm_attr, sizeof(ctx->ai_inst.pcm_attr));
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
    }

    /* mic(1ch) + ref(1ch) */
    ctx->ai_storage.data_pcm_attr.channels = ctx->ai_inst.pcm_attr.channels + SEA_AI_REF_CH_NUM;

    ctx->ai_storage.data_caching_size = calc_pcm_frame_size(&ctx->ai_storage.data_pcm_attr);
    ctx->ai_storage.data_caching_size *= SEA_DFX_DATA_CACHING_FRAME_CNT;

#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
    ret = memcpy_s(&ctx->asr_inst.data_pcm_attr, sizeof(ctx->asr_inst.data_pcm_attr),
                   &ctx->ai_inst.pcm_attr, sizeof(ctx->ai_inst.pcm_attr));
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
    }

    /* mic(1ch) + ref(1ch) */
    ctx->asr_inst.data_pcm_attr.channels = ctx->ai_inst.pcm_attr.channels + SEA_AI_REF_CH_NUM;

    ctx->asr_inst.data_caching_size = calc_pcm_frame_size(&ctx->asr_inst.data_pcm_attr);
    ctx->asr_inst.data_caching_size *= SEA_DFX_DATA_CACHING_FRAME_CNT;
#endif

    ret = memcpy_s(&ctx->sea_storage.data_pcm_attr, sizeof(ctx->sea_storage.data_pcm_attr),
                   &ctx->ai_inst.pcm_attr, sizeof(ctx->ai_inst.pcm_attr));
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
    }

    ctx->sea_storage.data_caching_size = calc_pcm_frame_size(&ctx->sea_storage.data_pcm_attr);
    ctx->sea_storage.data_caching_size *= SEA_DFX_DATA_CACHING_FRAME_CNT;

    return EXT_SUCCESS;
}

static td_s32 sample_sea_parse_lib_type(td_char *param, sample_sea_context *ctx)
{
    if (strcmp(param, "kws") == 0) {
        ctx->sea_inst.lib_id = UAPI_SEA_LIB_KWS;
        return EXT_SUCCESS;
    }

    return EXT_FAILURE;
}

static td_s32 sample_sea_parse_ai_port(td_char *param, sample_sea_context *ctx)
{
    if (strcmp(param, "adc0") == 0) {
        ctx->ai_inst.ai_port = UAPI_AI_PORT_ADC0;
        return EXT_SUCCESS;
    } else if (strcmp(param, "lpadc0") == 0) {
        ctx->ai_inst.ai_port = UAPI_AI_PORT_LPADC0;
        return EXT_SUCCESS;
    } else if (strcmp(param, "pdm0") == 0) {
        ctx->ai_inst.ai_port = UAPI_AI_PORT_PDM0;
        return EXT_SUCCESS;
    } else if (strcmp(param, "pdm1") == 0) {
        ctx->ai_inst.ai_port = UAPI_AI_PORT_PDM1;
        return EXT_SUCCESS;
    }

    return EXT_FAILURE;
}

static td_void sample_sea_parse_storage_mode(td_char *param, sample_sea_context *ctx)
{
    if (param != TD_NULL && strcmp(param, "phase") == 0) {
        ctx->ai_storage.data_storage_mode = DATA_STORE_MODE_PHASE;
        ctx->sea_storage.data_storage_mode = DATA_STORE_MODE_PHASE;
        sap_alert_log_info("Data save mode:Only save matching phase");
    } else {
        ctx->ai_storage.data_storage_mode = DATA_STORE_MODE_ALL;
        ctx->sea_storage.data_storage_mode = DATA_STORE_MODE_ALL;
        sap_alert_log_info("Data save mode:Save all");
    }
}

static td_void sample_sea_parse_asr(sample_sea_context *ctx, td_s32 argc, td_char *argv[])
{
    if (argc > ARGV_ASR_ENABLE_INDEX) {
        ctx->asr_inst.asr_enable = *(td_char *)argv[ARGV_ASR_ENABLE_INDEX] - '0';
    } else {
        ctx->asr_inst.asr_enable = TD_FALSE;
    }
    ctx->ai_storage.asr_enable = TD_FALSE;
    ctx->sea_storage.asr_enable = TD_FALSE;
    if (argc > ARGV_ASR_DFX_ENABLE_INDEX) {
        ctx->asr_inst.asr_dfx = *(td_char *)argv[ARGV_ASR_DFX_ENABLE_INDEX] - '0';
    } else {
        ctx->asr_inst.asr_dfx = TD_FALSE;
    }
    ctx->ai_storage.asr_dfx = TD_FALSE;
    ctx->sea_storage.asr_dfx = TD_FALSE;
}

static td_s32 sample_sea_parse_param(td_s32 argc, td_char *argv[], sample_sea_context *ctx)
{
    td_s32 ret;
    td_char *storage_mode = TD_NULL;
    sample_sea_params *params;

    if (argc < ARGV_MIC_TYPE_INDEX) {
        return EXT_FAILURE;
    } else if (argc >= ARGV_STORAGE_MODE_INDEX) {
        storage_mode = (td_char *)argv[ARGV_STORAGE_MODE_INDEX];
    }

    params = (sample_sea_params *)(argv + ARGV_FILE_SIZE_INDEX);

    ret = sample_sea_parse_storage_size(params->file_size, ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_parse_storage_size, ret);
        return ret;
    }

    ret = sample_sea_parse_storage_type(params->storage_type, ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_parse_storage_type, ret);
        return ret;
    }

    ret = sample_sea_parse_storage_attr(ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_parse_storage_attr, ret);
        return ret;
    }

    sample_sea_parse_storage_mode(storage_mode, ctx);

    ret = sample_sea_parse_lib_type(params->lib_type, ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_parse_lib_type, ret);
        return ret;
    }

    ret = sample_sea_parse_ai_port(params->ai_port, ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_parse_ai_port, ret);
        return ret;
    }
    sample_sea_parse_asr(ctx, argc, argv);

    return EXT_SUCCESS;
}

static td_s32 sample_sea_open_storage_file(sample_sea_task_inst *storage)
{
    td_s32 ret;
    size_t file_tail_size;
    td_char *file_prefix_end;
    const uapi_audio_pcm_format *pcm_attr = &storage->data_pcm_attr;

    if (storage->data_storage_flag != DATA_STORE_FILE) {
        return EXT_SUCCESS;
    }

    file_prefix_end = storage->file_path + strlen(storage->file_path);
    file_tail_size = sizeof(storage->file_path) - strlen(storage->file_path);

    ret = sprintf_s(file_prefix_end, file_tail_size, "_%s_%uk_%ubit_%uch.pcm", storage->name,
        pcm_attr->sample_rate / SAMPLERATE_TRANS, pcm_attr->bit_depth, pcm_attr->channels);
    if (ret <= 0) {
        sap_err_log_fun(strncpy_s, ret);
        return ret;
    }

    storage->h_file = sap_fopen(storage->file_path, "wb");
    if (storage->h_file == TD_NULL) {
        sap_err_log_fun(sap_fopen, TD_NULL);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_sea_close_storage_file(sample_sea_task_inst *storage)
{
    td_s32 ret;

    if (storage->data_storage_flag != DATA_STORE_FILE) {
        return EXT_SUCCESS;
    }

    if (storage->h_file == TD_NULL) {
        return EXT_FAILURE;
    }

    ret = sap_fclose(storage->h_file);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sap_fclose, ret);
        return ret;
    }
    storage->h_file = TD_NULL;

    return EXT_SUCCESS;
}

static td_s32 sample_sea_create_adp_by_storage(sample_sea_task_inst *storage)
{
    td_s32 ret;
    uapi_adp_attr adp_attr;

    if (storage->data_storage_flag == DATA_STORE_NONE && storage->asr_enable == TD_FALSE) {
        return EXT_SUCCESS;
    }

    ret = uapi_adp_get_def_attr(&adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_get_def_attr, ret);
        return ret;
    }

    adp_attr.buf_size = calc_pcm_frame_size(&storage->data_pcm_attr) * SEA_ADP_OUT_FRAME_CNT;
    sap_trace_log_u32(adp_attr.buf_size);

    ret = uapi_adp_create(&storage->h_adp, &adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_create, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_sea_destroy_adp_by_storage(sample_sea_task_inst *storage)
{
    td_s32 ret;

    if (storage->data_storage_flag == DATA_STORE_NONE && storage->asr_enable == TD_FALSE) {
        return EXT_SUCCESS;
    }

    ret = uapi_adp_destroy(storage->h_adp);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_destroy, ret);
        return ret;
    }

    storage->h_adp = 0;
    return EXT_SUCCESS;
}

static td_s32 sample_sea_create_storage_adp(sample_sea_context *ctx)
{
    td_s32 ret;

    ret = sample_sea_create_adp_by_storage(&ctx->ai_storage);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_create_adp_by_storage, ret);
        return ret;
    }

    ret = sample_sea_create_adp_by_storage(&ctx->sea_storage);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_create_adp_by_storage, ret);
        sample_sea_destroy_adp_by_storage(&ctx->ai_storage);
        return ret;
    }

#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
    ret = sample_sea_create_adp_by_storage(&ctx->asr_inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_create_adp_by_storage, ret);
        sample_sea_destroy_adp_by_storage(&ctx->ai_storage);
        sample_sea_destroy_adp_by_storage(&ctx->sea_storage);
        return ret;
    }
#endif

    if (ctx->sea_storage.data_storage_flag == DATA_STORE_ADDR) {
        g_sample_sea_write_info.data_addr = (td_u8 *)FPGA_SAVE_DATA_ADDR;
        g_sample_sea_write_info.data_size = FPGA_SAVE_DATA_SIZE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_sea_destroy_storage_adp(sample_sea_context *ctx)
{
    td_s32 ret;

    ret = sample_sea_destroy_adp_by_storage(&ctx->ai_storage);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_destroy_adp_by_storage, ret);
        return ret;
    }

    ret = sample_sea_destroy_adp_by_storage(&ctx->sea_storage);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_destroy_adp_by_storage, ret);
        return ret;
    }

#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
    ret = sample_sea_destroy_adp_by_storage(&ctx->asr_inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_destroy_adp_by_storage, ret);
        return ret;
    }
#endif

    return EXT_SUCCESS;
}

static td_s32 sample_sea_create_storage(sample_sea_context *ctx)
{
    td_s32 ret;

    ret = sample_sea_open_storage_file(&ctx->ai_storage);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_open_storage_file, ret);
        return ret;
    }

    ret = sample_sea_open_storage_file(&ctx->sea_storage);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_open_storage_file, ret);
        goto out0;
    }

    ret = sample_sea_create_storage_adp(ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_create_storage_adp, ret);
        goto out1;
    }

    return EXT_SUCCESS;

out1:
    (td_void)sample_sea_close_storage_file(&ctx->sea_storage);

out0:
    (td_void)sample_sea_close_storage_file(&ctx->ai_storage);

    return ret;
}

static td_void sample_sea_destroy_storage(sample_sea_context *ctx)
{
    td_s32 ret;

    ret = sample_sea_destroy_storage_adp(ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_destroy_storage_adp, ret);
    }

    ret = sample_sea_close_storage_file(&ctx->sea_storage);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_close_storage_file, ret);
    }

    ret = sample_sea_close_storage_file(&ctx->ai_storage);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_close_storage_file, ret);
    }
}

static td_s32 sample_sea_get_ai_attr(sample_sea_context *ctx, uapi_ai_attr *ai_attr)
{
    td_s32 ret;
    sample_sea_ai_inst *ai_inst = &ctx->ai_inst;

    ret = uapi_ai_get_default_attr(ctx->ai_inst.ai_port, ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_get_default_attr, ret);
        return ret;
    }

    switch (ai_inst->ai_port) {
        case UAPI_AI_PORT_PDM0:
        case UAPI_AI_PORT_PDM1:
            ai_attr->port_attr.pdm.rx_type = UAPI_AI_RX_DEFAULT;
            ai_attr->port_attr.pdm.i2s_attr.channels = ai_inst->pcm_attr.channels;
            ai_attr->port_attr.pdm.i2s_attr.bit_depth = ai_inst->pcm_attr.bit_depth;
            break;

        default:
            break;
    }

    ai_attr->ref_attr.enable = TD_TRUE;
    ai_attr->ref_attr.port = SND_OUT_PORT_DEFAULT;

    return EXT_SUCCESS;
}

static td_s32 sample_sea_open_ai(sample_sea_context *ctx)
{
    td_s32 ret;
    uapi_ai_attr ai_attr;
    sample_sea_ai_inst *ai_inst = &ctx->ai_inst;

    /* ai_get_defualt here */
    ret = sample_sea_get_ai_attr(ctx, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_get_ai_attr, ret);
        return ret;
    }

    ai_attr.pcm_attr = ai_inst->pcm_attr;

    ret = uapi_ai_open(&ai_inst->h_ai, ai_inst->ai_port, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_open, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_sea_close_ai(sample_sea_context *ctx)
{
    td_s32 ret;
    ret = uapi_ai_close(ctx->ai_inst.h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_close, ret);
    }
}

static td_void init_sea_eng(uapi_sea_eng_sel *sea_eng, const sample_sea_inst *sea_inst)
{
    (td_void)memset_s(sea_eng, sizeof(*sea_eng), 0, sizeof(*sea_eng));
    sea_eng->afe.type = UAPI_SEA_AFE_SEE;
    sea_eng->afe.lib_id = UAPI_SEA_LIB_SEE;
    sea_eng->aai[0].type = UAPI_SEA_AAI_KWS;
    sea_eng->aai[0].lib_id = sea_inst->lib_id;
}

static td_s32 sample_sea_open_sea(sample_sea_context *ctx)
{
    td_s32 ret;
    uapi_sea_eng_sel sea_eng;
    uapi_sea_attr sea_attr;
    sample_sea_inst *sea_inst = &ctx->sea_inst;

    init_sea_eng(&sea_eng, sea_inst);

    ret = uapi_sea_load_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_load_engine, ret);
    }

    ret = uapi_sea_load_engine(sea_inst->lib_id, "imedia_keyword");
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_load_engine, ret);
    }

    ret = uapi_sea_get_default_attr(&sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_get_default_attr, ret);
        goto out0;
    }

    sea_attr.ref_pcm.channels = SEA_AI_REF_CH_NUM;

    ret = uapi_sea_create(&sea_inst->h_sea, &sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_create, ret);
        goto out0;
    }

    /* Register SEA event process callback */
    ret = uapi_sea_register_event_proc(sea_inst->h_sea, sample_sea_event_proc, ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_register_event_proc, ret);
        goto out1;
    }

    (td_void)memset_s(&sea_inst->phs_sets, sizeof(sea_inst->phs_sets), 0, sizeof(sea_inst->phs_sets));
    sea_inst->item_sets.size = SEA_PHRASE_COUNT;
    sea_inst->item_sets.array = sea_inst->phs_sets.phrase;
    ret = uapi_sea_get_phrase_sets(sea_inst->h_sea, &sea_inst->item_sets);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_get_phrase_sets, ret);
        goto out1;
    }
    sea_inst->phs_sets.phs_count = sea_inst->item_sets.count;

    return EXT_SUCCESS;

out1:
    (td_void)uapi_sea_destroy(sea_inst->h_sea);

out0:
    (td_void)uapi_sea_unload_engine(sea_inst->lib_id, "imedia_keyword");
    (td_void)uapi_sea_unload_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");

    return ret;
}

static td_void sample_sea_close_sea(sample_sea_context *ctx)
{
    td_s32 ret;

    ret = uapi_sea_destroy(ctx->sea_inst.h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_destroy, ret);
    }

    ret = uapi_sea_unload_engine(ctx->sea_inst.lib_id, "imedia_keyword");
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_unload_engine, ret);
    }

    ret = uapi_sea_unload_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_unload_engine, ret);
    }
}

static td_void sample_sea_ctx_init(sample_sea_context *ctx)
{
    ctx->ai_inst.pcm_attr.channels = UAPI_AUDIO_CHANNEL_1;
    ctx->ai_inst.pcm_attr.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    ctx->ai_inst.pcm_attr.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    ctx->ai_inst.pcm_attr.sample_per_frame = ctx->ai_inst.pcm_attr.sample_rate / SAMPLERATE_TRANS; /* 1ms : samples */
    ctx->ai_inst.pcm_attr.sample_per_frame *= AI_FRAME_DURATION_MS; /* 10ms : samples */

    ctx->sea_storage.task_name = "sample_sea_save_sea";
    ctx->sea_storage.name = "sea";
    ctx->ai_storage.task_name = "sample_sea_save_ai";
    ctx->ai_storage.name = "ai";
    ctx->asr_inst.task_name = "sample_sea_asr";
    ctx->asr_inst.name = "asr";
    ctx->asr_inst.task_save_data_name = "sample_sea_asr_storage";
}

static td_void sample_sea_write_frame(sample_sea_task_inst *storage)
{
    if (storage->frame_buf == TD_NULL) {
        sap_err_log_info("frame_buf is NULL!");
        return;
    }

    circ_buf *cb = &storage->data_caching.cb;
    td_u32 data_size = circ_buf_query_busy(cb);
    if (circ_buf_read(cb, storage->frame_buf, data_size) != data_size) {
        sap_err_log_info("Fatal Error: circ_buf_read error");
        return;
    }

    storage->file_size += data_size;
    if (storage->file_size > storage->file_limit_size) {
        return;
    }

    if (storage->data_storage_flag == DATA_STORE_FILE && storage->h_file != TD_NULL) {
        sap_fwrite((td_void *)storage->frame_buf, 1, data_size, storage->h_file);
    } else if (storage->data_storage_flag == DATA_STORE_ADDR) {
        sample_sea_write_buffer(&g_sample_sea_write_info, (td_u8 *)storage->frame_buf, data_size);
    }
}

static td_void sample_sea_storage_data_caching_init(sample_sea_task_inst *storage)
{
    td_void *caching_buf = malloc(storage->data_caching_size);
    if (caching_buf == TD_NULL) {
        sap_err_log_info("caching_buf malloc failed!");
        return;
    }
    data_info_init(&storage->data_caching, (td_u32)(td_uintptr_t)caching_buf, storage->data_caching_size);

    storage->frame_buf = malloc(storage->data_caching_size);
    if (storage->frame_buf == TD_NULL) {
        sap_err_log_info("frame_buf malloc failed!");
        free(caching_buf);
        return;
    }
    sap_alert_log_h32(storage->data_caching_size);
}

static td_void sample_sea_storage_data_caching_proc(data_info *data_info, uapi_audio_frame *frame)
{
    circ_buf *cb = &data_info->cb;
    td_u32 free_size = circ_buf_query_free(cb);
    if (frame->bits_bytes >= free_size) { /* when caching buf full, move rptr automatically */
        /* cicle buf must have 4 Bytes left */
        circ_buf_update_read_pos(cb, frame->bits_bytes - free_size + sizeof(td_u32));
    }

    (td_void)circ_buf_write(cb, (const td_u8 *)frame->bits_buffer, frame->bits_bytes);
}

static td_void sample_sea_storage_data_caching_deinit(sample_sea_task_inst *storage)
{
    if (storage->data_caching.buf != TD_NULL) {
        free(storage->data_caching.buf);
    }

    if (storage->frame_buf != TD_NULL) {
        free(storage->frame_buf);
        storage->frame_buf = TD_NULL;
    }

    (td_void)memset_s(&storage->data_caching, sizeof(storage->data_caching), 0, sizeof(data_info));
}

static td_void sample_sea_task(td_void *inst)
{
    td_s32 ret;
    uapi_audio_frame frame;
    sample_sea_task_inst *storage = (sample_sea_task_inst *)inst;

    sample_sea_storage_data_caching_init(storage);

    while (storage->task_enable) {
        ret = uapi_adp_acquire_frame(storage->h_adp, &frame);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_5MS);
            continue;
        }

        sample_sea_storage_data_caching_proc(&storage->data_caching, &frame);

        if (storage->data_storage_mode == DATA_STORE_MODE_PHASE) {
            /* Only save wakeup phase clip */
            if (storage->wake_up_flag == TD_TRUE) {
                sample_sea_write_frame(storage);
                storage->wake_up_flag = TD_FALSE;
            }
        } else {
            /* Save all */
            sample_sea_write_frame(storage);
        }

        ret = uapi_adp_release_frame(storage->h_adp, &frame);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_release_frame, ret);
        }
    }

    sample_sea_storage_data_caching_deinit(storage);

    athread_set_exit(storage->task, TD_TRUE);
}

#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
static td_void sample_sea_asr_storage_task(td_void *inst)
{
    td_s32 ret;
    uapi_audio_frame frame;
    sample_sea_task_inst *asr_inst = (sample_sea_task_inst *)inst;

    sample_sea_storage_data_caching_init(asr_inst);

    while (asr_inst->task_save_data_enable) {
        ret = uapi_adp_acquire_frame(asr_inst->h_adp, &frame);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }

        if (asr_inst->wake_up_flag == TD_TRUE) {
            sample_sea_storage_data_caching_proc(&asr_inst->data_caching, &frame);
        }

        ret = uapi_adp_release_frame(asr_inst->h_adp, &frame);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_release_frame, ret);
        }
    }

    athread_set_exit(asr_inst->task_save_data, TD_TRUE);
}

static td_void sample_sea_asr_task(td_void *inst)
{
    td_s32 ret;
    sample_sea_task_inst *asr_inst = (sample_sea_task_inst *)inst;

    sample_asr_set_pps_data(asr_inst->asr_st, g_pps_data_sea);

    while (asr_inst->task_enable) {
        if (asr_inst->wake_up_flag == TD_TRUE) {
            circ_buf *cb = &asr_inst->data_caching.cb;
            td_u32 data_size = FUZZ_FRAME_LEN * sizeof(td_s16);
            if (asr_inst->frame_buf == TD_NULL) {
                sap_err_log_info("frame_buf is NULL!");
                sap_msleep(THREAD_SLEEP_5MS);
                continue;
            }

            if (circ_buf_query_busy(cb) < data_size) {
                sap_msleep(THREAD_SLEEP_10MS);
                continue;
            }
            if (circ_buf_read(cb, asr_inst->frame_buf, data_size) != data_size) {
                sap_err_log_info("Error: circ_buf_read error");
                sap_msleep(THREAD_SLEEP_5MS);
                continue;
            }

            ret = asr_fuzz_proc(asr_inst->asr_st, (td_s16 *)asr_inst->frame_buf, FUZZ_FRAME_LEN);
            if (ret != FUZZ_SUCCESS && ret != FUZZ_WORD_DONE) {
                sap_err_log_fun(asr_fuzz_proc, ret);
            }
            if (ret == FUZZ_WORD_DONE) {
                asr_inst->wake_up_flag = TD_FALSE;
                circ_buf_flush(cb);
            }
        }
        sap_msleep(THREAD_SLEEP_5MS);
    }

    athread_set_exit(asr_inst->task, TD_TRUE);
}

static td_s32 sample_sea_create_asr_task(sample_sea_task_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    if (inst->asr_enable == TD_FALSE) {
        return EXT_SUCCESS;
    }

    inst->task_save_data_enable = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = inst->task_save_data_name;
    ret = athread_create(&inst->task_save_data, sample_sea_asr_storage_task, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        return ret;
    }

    inst->task_enable = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = inst->task_name;
    ret = athread_create(&inst->task, sample_sea_asr_task, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        return ret;
    }

    return EXT_SUCCESS;
}
#endif

static td_s32 sample_sea_create_storage_task(sample_sea_task_inst *storage)
{
    td_s32 ret;
    athread_attr attr;

    if (storage->data_storage_flag != DATA_STORE_FILE) {
        return EXT_SUCCESS;
    }

    storage->task_enable = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = storage->task_name;
    ret = athread_create(&storage->task, sample_sea_task, (td_void *)storage, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_sea_destroy_storage_task(sample_sea_task_inst *storage)
{
    storage->task_enable = TD_FALSE;
    if (storage->task != TD_NULL) {
        athread_exit(storage->task);
        storage->task = TD_NULL;
    }
    storage->task_save_data_enable = TD_FALSE;
    if (storage->task_save_data != TD_NULL) {
        athread_exit(storage->task_save_data);
        storage->task_save_data = TD_NULL;
    }
}

static td_s32 sample_sea_create_task(sample_sea_context *ctx)
{
    td_s32 ret;

    ret = sample_sea_create_storage_task(&ctx->ai_storage);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_create_storage_task, ret);
        return ret;
    }

    ret = sample_sea_create_storage_task(&ctx->sea_storage);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_create_storage_task, ret);
        goto out0;
    }

#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
    ret = sample_sea_create_asr_task(&ctx->asr_inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_create_asr_task, ret);
        goto out1;
    }
#endif

    return EXT_SUCCESS;

#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
out1:
    sample_sea_destroy_storage_task(&ctx->sea_storage);
#endif
out0:
    sample_sea_destroy_storage_task(&ctx->ai_storage);

    return ret;
}

static td_void sample_sea_destroy_task(sample_sea_context *ctx)
{
    sample_sea_destroy_storage_task(&ctx->ai_storage);
    sample_sea_destroy_storage_task(&ctx->sea_storage);
#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
    sample_sea_destroy_storage_task(&ctx->asr_inst);
#endif
}

static td_s32 sample_sea_start_inst(sample_sea_context *ctx)
{
    td_s32 ret;

    /* Start AI instance to send data to SEA instance */
    ret = uapi_ai_start(ctx->ai_inst.h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_start, ret);
        return ret;
    }

    /* Start SEA instance to process and send data into ADP instance */
    ret = uapi_sea_start(ctx->sea_inst.h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_start, ret);
        uapi_ai_stop(ctx->ai_inst.h_ai);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_sea_stop_inst(sample_sea_context *ctx)
{
    td_s32 ret;

    ret = uapi_sea_stop(ctx->sea_inst.h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_stop, ret);
    }

    ret = uapi_ai_stop(ctx->ai_inst.h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_stop, ret);
    }
}

static td_s32 sample_sea_open_ctx(sample_sea_context *ctx)
{
    td_s32 ret;

    ret = sample_sea_create_storage(ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_create_storage, ret);
        return ret;
    }

    ret = sample_sea_open_ai(ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_open_ai, ret);
        goto destroy_storage;
    }

    ret = sample_sea_open_sea(ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_open_sea, ret);
        goto ai_inst_close;
    }

    return EXT_SUCCESS;

ai_inst_close:
    sample_sea_close_ai(ctx);

destroy_storage:
    sample_sea_destroy_storage(ctx);
    return ret;
}

static td_void sample_sea_close_ctx(sample_sea_context *ctx)
{
    sample_sea_close_sea(ctx);
    sample_sea_close_ai(ctx);
    sample_sea_destroy_storage(ctx);
}

static td_void sample_sea_stop_ctx(sample_sea_context *ctx)
{
    sample_sea_destroy_task(ctx);
    sample_sea_stop_inst(ctx);
}

static td_s32 sample_sea_start_ctx(sample_sea_context *ctx)
{
    td_s32 ret;

    ret = sample_sea_start_inst(ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_start_inst, ret);
        return ret;
    }

    ret = sample_sea_create_task(ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_create_task, ret);
        goto out0;
    }

    return EXT_SUCCESS;

out0:
    sample_sea_stop_inst(ctx);
    return ret;
}

static td_s32 sample_sea_attach_ctx(sample_sea_context *ctx)
{
    td_s32 ret;

    /* Attach SEA instance to AI instance */
    ret = uapi_ai_attach_output(ctx->ai_inst.h_ai, ctx->sea_inst.h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_attach_output, ret);
        return ret;
    }

    /* Attach ADP instance to AI instance */
    if (ctx->ai_storage.data_storage_flag != DATA_STORE_NONE) {
        ret = uapi_ai_attach_output(ctx->ai_inst.h_ai, ctx->ai_storage.h_adp);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_ai_attach_output, ret);
            goto out0;
        }
    }

    /* Attach ADP instance to SEA instance */
    if (ctx->sea_storage.data_storage_flag != DATA_STORE_NONE) {
        ret = uapi_sea_attach_output(ctx->sea_inst.h_sea, UAPI_SEA_OUTPUT_ASR_SRC, ctx->sea_storage.h_adp);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_attach_output, ret);
            goto out1;
        }
    }

#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
    /* Attach ADP instance of asr to SEA instance */
    if (ctx->asr_inst.asr_enable == TD_TRUE) {
        ret = uapi_sea_attach_output(ctx->sea_inst.h_sea, UAPI_SEA_OUTPUT_ASR_SRC, ctx->asr_inst.h_adp);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_attach_output, ret);
            goto out2;
        }
    }
#endif

    return EXT_SUCCESS;

#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
out2:
    if (ctx->asr_inst.asr_enable == TD_TRUE) {
        (td_void)uapi_sea_detach_output(ctx->sea_inst.h_sea, UAPI_SEA_OUTPUT_ASR_SRC, ctx->sea_storage.h_adp);
    }
#endif
out1:
    if (ctx->ai_storage.data_storage_flag != DATA_STORE_NONE) {
        (td_void)uapi_ai_detach_output(ctx->ai_inst.h_ai, ctx->ai_storage.h_adp);
    }

out0:
    (td_void)uapi_ai_detach_output(ctx->ai_inst.h_ai, ctx->sea_inst.h_sea);

    return ret;
}

static td_void sample_sea_detach_ctx(sample_sea_context *ctx)
{
    td_s32 ret;

    /* Detach SEA instance from AI instance */
    ret = uapi_ai_detach_output(ctx->ai_inst.h_ai, ctx->sea_inst.h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_detach_output, ret);
    }

    /* Detach ADP instance from AI instance */
    if (ctx->ai_storage.data_storage_flag != DATA_STORE_NONE) {
        ret = uapi_ai_detach_output(ctx->ai_inst.h_ai, ctx->ai_storage.h_adp);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_ai_detach_output, ret);
        }
    }

    /* Detach ADP instance from SEA instance */
    if (ctx->sea_storage.data_storage_flag != DATA_STORE_NONE) {
        ret = uapi_sea_detach_output(ctx->sea_inst.h_sea, UAPI_SEA_OUTPUT_ASR_SRC, ctx->sea_storage.h_adp);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_detach_output, ret);
        }
    }

#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
    if (ctx->asr_inst.asr_enable == TD_TRUE) {
        ret = uapi_sea_detach_output(ctx->sea_inst.h_sea, UAPI_SEA_OUTPUT_ASR_SRC, ctx->asr_inst.h_adp);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_detach_output, ret);
        }
    }
#endif
}

#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
static td_void sample_sea_inst_init(sample_sea_context *ctx)
{
    td_s32 ret;
    td_u32 asr_size = sample_asr_get_inst_size();
    td_u32 asr_scratch_size = sample_asr_get_scratch_size();

    if (ctx->asr_inst.asr_enable == TD_FALSE) {
        return;
    }

    ctx->asr_inst.asr_st = malloc(asr_size);
    if (ctx->asr_inst.asr_st == TD_NULL) {
        sap_err_log_info("asr inst malloc failed!");
        return;
    }
    ctx->asr_inst.scratch_buffer = (td_char *)malloc(asr_scratch_size);
    if (ctx->asr_inst.scratch_buffer == TD_NULL) {
        sap_err_log_info("scratch buffer malloc failed!");
        goto out0;
    }
    sample_asr_set_scratch(ctx->asr_inst.asr_st, ctx->asr_inst.scratch_buffer);
    ret = asr_fuzz_init(ctx->asr_inst.asr_st, ctx->asr_inst.asr_dfx);
    if (ret != FUZZ_SUCCESS) {
        sap_err_log_fun(asr_fuzz_init, ret);
        goto out1;
    }
    return;

out1:
    ret = asr_fuzz_deinit(ctx->asr_inst.asr_st);
    if (ret != FUZZ_SUCCESS) {
        sap_err_log_fun(asr_fuzz_deinit, ret);
    }
    free(ctx->asr_inst.scratch_buffer);
out0:
    free(ctx->asr_inst.asr_st);
}

static td_void sample_sea_inst_deinit(sample_sea_context *ctx)
{
    td_s32 ret;
    if (ctx->asr_inst.asr_enable == TD_FALSE) {
        return;
    }

    ret = asr_fuzz_deinit(ctx->asr_inst.asr_st);
    if (ret != FUZZ_SUCCESS) {
        sap_err_log_fun(asr_fuzz_deinit, ret);
    }
    if (ctx->asr_inst.scratch_buffer != TD_NULL) {
        free(ctx->asr_inst.scratch_buffer);
        ctx->asr_inst.scratch_buffer = TD_NULL;
    }
    if (ctx->asr_inst.asr_st != TD_NULL) {
        free(ctx->asr_inst.asr_st);
        ctx->asr_inst.asr_st = TD_NULL;
    }
}
#endif

static td_s32 sample_sea_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_sea_context *ctx = &g_sample_sea_ctx;

    if (ctx->running == TD_TRUE) {
        sap_printf("sample sea is running\n");
        return EXT_FAILURE;
    }

    (td_void)memset_s(ctx, sizeof(*ctx), 0, sizeof(*ctx));
    sample_sea_ctx_init(ctx);

    ret = sample_sea_parse_param(argc, argv, ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_parse_param, ret);
        return ret;
    }

    ret = sample_sea_sys_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_sys_init, ret);
        return ret;
    }
#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
    sample_sea_inst_init(ctx);
#endif

    ret = sample_sea_open_ctx(ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_open_ctx, ret);
        goto sample_deinit;
    }

    ret = sample_sea_attach_ctx(ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_attach_ctx, ret);
        goto sample_close;
    }

    ret = sample_sea_start_ctx(ctx);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_sea_start_ctx, ret);
        goto sample_detach;
    }

    ctx->running = TD_TRUE;
    return EXT_SUCCESS;

sample_detach:
    sample_sea_detach_ctx(ctx);
sample_close:
    sample_sea_close_ctx(ctx);
sample_deinit:
    sample_sea_sys_deinit();
    return ret;
}

static td_s32 sample_sea_exit(td_void)
{
    sample_sea_context *ctx = &g_sample_sea_ctx;
    if (!ctx->running) {
        sap_printf("sample sea is not running\n");
        return EXT_FAILURE;
    }

    sample_sea_stop_ctx(ctx);
    sample_sea_detach_ctx(ctx);
    sample_sea_close_ctx(ctx);
#if defined(CONFIG_NPU_LIB) && defined(SAP_ASR_SUPPORT)
    sample_sea_inst_deinit(ctx);
#endif
    sample_sea_sys_deinit();

    ctx->running = TD_FALSE;
    return EXT_SUCCESS;
}

static td_void sample_sea_usage(const td_char *name)
{
    sap_printf("usage:  %s #size #path #aai #port [#mode] [#asr_enable] [#asr_dfx]\n", name);
    sap_printf("           #size: ai && sea data save size(MB)\n");
    sap_printf("           #path: real path: Saving data to file system\n");
    sap_printf("                  addr: Saving data to DFX address space\n");
    sap_printf("                  null: No data saving\n");
    sap_printf("           #aai: Switching different engine, such as: kws (Only support kws temporarily)\n");
    sap_printf("           #port: lpadc0/adc0/pdm0, recommend to use lpadc0\n");
    sap_printf("           #mode: Data saving mode, optional param\n");
    sap_printf("                  phase:Saving phase only when matching\n");
    sap_printf("                  default:Saving all\n");
    sap_printf("           #asr_enable: asr enable, optional param\n");
    sap_printf("                  0 or 1, default: 0, disabled\n");
    sap_printf("           #asr_dfx: asr dfx enable, optional param\n");
    sap_printf("                  0 or 1, default: 0, disabled\n");
    sap_printf("examples:\n");
    sap_printf("  %s 10 null kws lpadc0\n", name);
    sap_printf("  %s 10 /user/kb2_111_lpadc_sea_0626_1430 kws adc0\n", name);
    sap_printf("  %s 10 /user/kb2_111_lpadc_sea_0626_1430 kws pdm0 phase\n", name);
    sap_printf("  %s 10 null kws adc0 1 1\n", name);
}

td_s32 sample_sea(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_sea_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_sea_exit();
    } else {
        return sample_sea_entry(argc, argv);
    }

    return EXT_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
