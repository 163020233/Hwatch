/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: kws api
 * Author: audio
 */

#include <stdlib.h>
#include <string.h>
#include "securec.h"
#include "plugin_errno.h"
#include "audio_sea.h"
#include "audio_type.h"
#include "aai_alg0_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define AAI_ALG_FRAME_LEN  20
#define AAI_ALG_SCR_BUF_SIZE (0x8000)
#define AAI_ALG_INST_SIZE 0x100
#define AAI_ALG_DATA_SIZE ((16000 << 1) / (1000 / AAI_ALG_FRAME_LEN)) /* 16KHz/16bit/1ch/20ms */
#define AAI_ALG_MAX_SCORE  100
#define AAI_ALG_MAX_INDEX  200

#define aai_alg_check_null_return(ptr) \
    do { \
        if ((td_void *)(ptr) == TD_NULL) { \
            return PLUGIN_FAILURE; \
        } \
    } while (0)

typedef struct {
    td_void *handle;
    sea_pcm_format pcm_attr;
    td_u8 *inst_base;
    td_u8 *scr_buf;
    td_u32 scr_size;
    td_u8 *pcm_buf;
    td_u32 pcm_size;
} aai_alg_inst;

static td_s32 aai_alg_api_init(td_void *handle)
{
    aai_alg_inst *alg_inst = (aai_alg_inst *)handle;
    aai_alg_check_null_return(handle);

    alg_inst->handle = TD_NULL;
    alg_inst->inst_base = (td_u8 *)(handle + sizeof(aai_alg_inst));
    alg_inst->scr_buf = TD_NULL;
    alg_inst->scr_size = AAI_ALG_SCR_BUF_SIZE;
    alg_inst->pcm_size = AAI_ALG_DATA_SIZE;
    alg_inst->pcm_buf = (td_u8 *)(handle + sizeof(aai_alg_inst) + AAI_ALG_INST_SIZE);
    return PLUGIN_SUCCESS;
}

static td_s32 aai_alg_set_scr_buf(td_void *handle, td_u8 *scr_buf)
{
    aai_alg_inst *alg_inst = (aai_alg_inst *)handle;
    aai_alg_check_null_return(handle);
    aai_alg_check_null_return(scr_buf);

    alg_inst->scr_buf = scr_buf;
    return PLUGIN_SUCCESS;
}

static td_s32 aai_alg_set_param(td_void *handle, const td_void *param)
{
    aai_alg_inst *alg_inst = (aai_alg_inst *)handle;
    aai_alg_check_null_return(handle);
    aai_alg_check_null_return(param);

    audio_unused(alg_inst);
    return PLUGIN_SUCCESS;
}

static td_s32 aai_alg_set_config(td_void *handle, td_u32 cmd, const td_void *cfg)
{
    switch (cmd) {
        case ALG_COMMON_CMD_API_INIT:
            return aai_alg_api_init(handle);

        case AAI_ALG_CMD_SET_SCR_BUF:
            return aai_alg_set_scr_buf(handle, (td_u8 *)cfg);

        case AAI_ALG_CMD_SET_PARAM:
            return aai_alg_set_param(handle, cfg);

        default:
            break;
    }

    return PLUGIN_SUCCESS;
}

static td_s32 aai_alg_get_api_size(td_u32 *size)
{
    aai_alg_check_null_return(size);
    *size = sizeof(aai_alg_inst) + AAI_ALG_INST_SIZE + AAI_ALG_DATA_SIZE;
    return PLUGIN_SUCCESS;
}

static td_s32 aai_alg_get_scr_buf_size(td_u32 *size)
{
    aai_alg_check_null_return(size);
    *size = AAI_ALG_SCR_BUF_SIZE;
    return PLUGIN_SUCCESS;
}

static td_s32 aai_alg_get_config(td_void *handle, td_u32 cmd, td_void *cfg)
{
    switch (cmd) {
        case ALG_COMMON_CMD_GET_API_SIZE:
            return aai_alg_get_api_size((td_u32 *)cfg);

        case AAI_ALG_CMD_GET_SCR_BUF_SIZE:
            return aai_alg_get_scr_buf_size((td_u32 *)cfg);

        case AAI_ALG_CMD_GET_VERSION:
            return PLUGIN_SUCCESS;

        default:
            break;
    }

    audio_unused(handle);
    return PLUGIN_SUCCESS;
}

static td_s32 aai_alg_destroy(td_void *handle)
{
    aai_alg_inst *alg_inst = (aai_alg_inst *)handle;
    aai_alg_check_null_return(handle);

    audio_unused(alg_inst);
    return PLUGIN_SUCCESS;
}

static td_s32 aai_alg_create(td_void *handle, const td_void *attr)
{
    aai_alg_inst *alg_inst = (aai_alg_inst *)handle;
    aai_alg_check_null_return(handle);

    if ((td_void *)alg_inst->inst_base != (handle + sizeof(aai_alg_inst))) {
        return PLG_ERR_NOT_INIT;
    }

    alg_inst->handle = (td_void *)alg_inst->inst_base;
    audio_unused(attr);
    return PLUGIN_SUCCESS;
}

static td_s32 aai_alg_process(td_void *handle, td_void *input, td_void *output)
{
    static td_u32 index = 0;
    audio_kws_out *output_frame = (audio_kws_out *)output;

    aai_alg_check_null_return(handle);
    aai_alg_check_null_return(input);
    aai_alg_check_null_return(output);

    /* process input frame here */

    /* set member 'iswakeup' to 1, report index and score when input frame matchs keyword */
    output_frame->iswakeup = 1;
    output_frame->index = index;
    output_frame->score[output_frame->index] = AAI_ALG_MAX_SCORE;

    /* index increases from 0 to 199 in this sample */
    index++;
    if (index >= AAI_ALG_MAX_INDEX) {
        index = 0;
    }

    audio_unused(handle);
    audio_unused(input);
    return PLUGIN_SUCCESS;
}

static td_s32 aai_alg_reset(td_void *handle)
{
    aai_alg_inst *alg_inst = (aai_alg_inst *)handle;
    aai_alg_check_null_return(handle);
    aai_alg_check_null_return(alg_inst->handle);

    return PLUGIN_SUCCESS;
}

static audio_alg_component g_aai_alg0_comp = {
    .name = "sea alg0",
    .version = 0,

    .create = aai_alg_create,
    .destroy = aai_alg_destroy,

    .set_config = aai_alg_set_config,
    .get_config = aai_alg_get_config,

    .reset = aai_alg_reset,

    .process = aai_alg_process,
};

audio_alg_component *aai_alg0_report_entry(td_void)
{
    return &g_aai_alg0_comp;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
