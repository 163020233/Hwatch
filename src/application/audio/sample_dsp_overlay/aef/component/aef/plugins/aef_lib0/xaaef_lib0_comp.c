/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: a dummy audio effect adpter layer
 * Author: audio
 */

#include "audio_type.h"
#include "component.h"
#include "log_overlay.h"

#include "audio_aef_customer.h"

typedef struct {
    td_u32 value;
} customer_aef_parameter1;

typedef struct {
    customer_aef_parameter1 parameter1;
    td_bool bypass;
} customer_aef_inst;

static customer_aef_inst g_customer_aef_inst = {
    .parameter1 = {
        .value = 0x5a, /* 假定一个u32值作为算法参数 */
    },

    .bypass = TD_TRUE, /* 算法默认bypass，设置为0使能 */
};

static td_s32 customer_aef_create(const td_void *aef_attr, td_void **aef, const aef_cfg *cfg)
{
    audio_unused(aef_attr);
    audio_unused(cfg);

    aef_check_null_ptr_return(aef);
    *aef = &g_customer_aef_inst;

    audio_log0(LOG_LEVEL_WARNING, "[customer_aef_create]");
    return AUDIO_SUCCESS;
}

static td_s32 customer_aef_destroy(td_void *aef)
{
    customer_aef_inst *inst = (customer_aef_inst *)aef;
    aef_check_null_ptr_return(aef);

    inst->parameter1.value = 0x5a;
    inst->bypass = TD_TRUE;
    audio_log0(LOG_LEVEL_WARNING, "[customer_aef_destroy]");
    return AUDIO_SUCCESS;
}

static td_s32 customer_aef_get_bypass(customer_aef_inst *inst, td_bool *bypass, td_u32 param_size)
{
    aef_check_null_ptr_return(bypass);

    if (param_size != sizeof(td_bool)) {
        return AUDIO_FAILURE;
    }

    *bypass = inst->bypass;
    return AUDIO_SUCCESS;
}

static td_s32 customer_aef_set_bypass(customer_aef_inst *inst, td_bool *bypass, td_u32 param_size)
{
    aef_check_null_ptr_return(bypass);

    if (param_size != sizeof(td_bool)) {
        return AUDIO_FAILURE;
    }

    inst->bypass = *bypass;
    return AUDIO_SUCCESS;
}

static td_s32 customer_aef_get_parameter1(customer_aef_inst *inst, customer_aef_parameter1 *p, td_u32 param_size)
{
    aef_check_null_ptr_return(p);

    if (param_size != sizeof(customer_aef_parameter1)) {
        return AUDIO_FAILURE;
    }

    p->value = inst->parameter1.value;
    return AUDIO_SUCCESS;
}

static td_s32 customer_aef_set_parameter1(customer_aef_inst *inst, const customer_aef_parameter1 *p, td_u32 param_size)
{
    aef_check_null_ptr_return(p);

    if (param_size != sizeof(customer_aef_parameter1)) {
        return AUDIO_FAILURE;
    }

    inst->parameter1.value = p->value;
    return AUDIO_SUCCESS;
}

static td_s32 customer_aef_set_parameter(td_void *aef, td_u32 cmd, const td_void *parameter, td_u32 param_size)
{
    customer_aef_inst *inst = (customer_aef_inst *)aef;
    aef_check_null_ptr_return(aef);

    audio_log2(LOG_LEVEL_WARNING, "[customer_aef_set_parameter] cmd: 0x%x |param_size: %u", cmd, param_size);

    switch (cmd) {
        case AEF_CUSTOMER_RW_PARAM_CMD:
            return customer_aef_set_parameter1(inst, (const customer_aef_parameter1 *)parameter, param_size);

        case AEF_CUSTOMER_SET_BYPASS_CMD:
            return customer_aef_set_bypass(inst, (td_bool *)parameter, param_size);

        default:
            return AUDIO_FAILURE;
    }

    return AUDIO_SUCCESS;
}

static td_s32 customer_aef_get_parameter(td_void *aef, td_u32 cmd, td_void *parameter, td_u32 param_size)
{
    customer_aef_inst *inst = (customer_aef_inst *)aef;
    aef_check_null_ptr_return(aef);

    audio_log2(LOG_LEVEL_WARNING, "[customer_aef_get_parameter] cmd: 0x%x |param_size: %u", cmd, param_size);

    switch (cmd) {
        case AEF_CUSTOMER_RW_PARAM_CMD:
            return customer_aef_get_parameter1(inst, (customer_aef_parameter1 *)parameter, param_size);

        case AEF_CUSTOMER_GET_BYPASS_CMD:
            return customer_aef_get_bypass(inst, (td_bool *)parameter, param_size);

        default:
            return AUDIO_FAILURE;
    }

    return AUDIO_SUCCESS;
}

static td_s32 customer_aef_proc_frame(td_void *aef, audio_frame *in_frame, audio_frame *out_frame)
{
    aef_check_null_ptr_return(aef);
    aef_check_null_ptr_return(in_frame);
    aef_check_null_ptr_return(out_frame);

    *out_frame = *in_frame;
    return AUDIO_SUCCESS;
}

static aef_component g_ha_audio_customer_aef_entry = {
    .name = (const td_char *)"customer_aef",
    .type = AEF_TYPE_CUSTOMER,
    .version = 0x00800001,
    .create = customer_aef_create,
    .destroy = customer_aef_destroy,
    .set_config = TD_NULL,
    .get_config = TD_NULL,
    .set_parameter = customer_aef_set_parameter,
    .get_parameter = customer_aef_get_parameter,
    .set_enable = TD_NULL,
    .get_enable = TD_NULL,
    .get_max_pcm_in_size = TD_NULL,
    .get_max_pcm_out_size = TD_NULL,
    .get_input_pcm_attr = TD_NULL,
    .proc_frame = customer_aef_proc_frame,
};

define_component(AEF, g_ha_audio_customer_aef_entry);
