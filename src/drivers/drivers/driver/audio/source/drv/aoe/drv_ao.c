/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: source file for drv audio output
 * Author: audio
 */

#include "securec.h"
#include "audio_struct.h"
#include "audio_debug.h"
#include "audio_osal.h"
#include "core_module.h"
#include "drv_sap_ioctl.h"
#include "drv_msg_ao.h"
#include "drv_proc_ao.h"
#include "drv_overlay_load.h"
#include "drv_audio_vendor.h"
#include "audio_aef.h"
#include "drv_ao.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_handle ao_handle;
    td_u64 context;
    td_u64 event_proc;
    td_u32 open_cnt;
    aef_type type;
} drv_ao_inst;

static drv_ao_inst g_drv_ao_insts[MOD_AO_MAX_NUM];

static td_s32 drv_ao_find_inst(drv_ao_inst **ao_inst, td_handle ao)
{
    td_u8 ao_ind;
    sap_mod_id mod_id;
    drv_ao_inst *ao_inst_p = TD_NULL;

    mod_id.u32 = (td_u32)ao;
    ao_ind = mod_id.bits.index;

    if (ao_ind >= MOD_AO_MAX_NUM) {
        return ERR_AO_INVALID_HANDLE;
    }
    ao_inst_p = &g_drv_ao_insts[ao_ind];

    if (ao_inst_p->ao_handle != ao) {
        return ERR_AO_INVALID_HANDLE;
    }

    *ao_inst = ao_inst_p;
    return AUDIO_SUCCESS;
}

static td_s32 drv_ao_alloc_inst(drv_ao_inst **ao_inst, td_handle ao)
{
    td_u8 ao_ind;
    drv_ao_inst *ao_inst_p = TD_NULL;
    sap_mod_id mod_id;

    mod_id.u32 = (td_u32)ao;
    ao_ind = mod_id.bits.index;

    if (ao_ind >= MOD_AO_MAX_NUM) {
        return ERR_AO_INVALID_HANDLE;
    }
    ao_inst_p = &g_drv_ao_insts[ao_ind];

    ao_inst_p->ao_handle = ao;
    *ao_inst = ao_inst_p;
    ao_inst_p->open_cnt++;
    return AUDIO_SUCCESS;
}

static td_s32 drv_ao_free_inst(td_handle ao)
{
    td_u8 ao_ind;
    sap_mod_id mod_id;
    drv_ao_inst *ao_inst_t = TD_NULL;

    mod_id.u32 = (td_u32)ao;
    ao_ind = mod_id.bits.index;

    if (ao_ind >= MOD_AO_MAX_NUM) {
        return ERR_AO_INVALID_HANDLE;
    }
    ao_inst_t = &g_drv_ao_insts[ao_ind];

    if (ao_inst_t->ao_handle != ao) {
        return ERR_AO_INVALID_HANDLE;
    }

    if (ao_inst_t->open_cnt > 1) {
        ao_inst_t->open_cnt--;
        return AUDIO_SUCCESS;
    }

    (td_void)memset_s(ao_inst_t, sizeof(*ao_inst_t), 0, sizeof(drv_ao_inst));

    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_init(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;

    ret = drv_msg_ao_init();
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_init, ret);
        return ret;
    }

    (td_void)memset_s(&g_drv_ao_insts, sizeof(g_drv_ao_insts), 0, sizeof(g_drv_ao_insts));

    audio_unused(cmd);
    audio_unused(arg);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_deinit(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;

    ret = drv_msg_ao_deinit();
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_deinit, ret);
        return ret;
    }

    (td_void)memset_s(&g_drv_ao_insts, sizeof(g_drv_ao_insts), 0, sizeof(g_drv_ao_insts));

    audio_unused(cmd);
    audio_unused(arg);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_get_def_attr(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    sap_ao_open *ao_open;

    ao_open = (sap_ao_open *)arg;
    ret = drv_msg_ao_get_def_attr(ao_open->snd_id, &ao_open->attr);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_get_def_attr, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

static td_s32 drv_aef_get_lib_type(const aef_type type_id, audio_lib_id *lib_id, audio_lib_type *lib_type)
{
    td_u32 i;
    const struct {
        aef_type type_id;
        audio_lib_id lib_id;
        audio_lib_type lib_type;
    } aef_overlay_map[] = {
        {AEF_TYPE_SMARTPA, AUDIO_LIB_AEF_SMARTPA, AUDIO_LIB_TYPE_OVERLAY},
        {AEF_TYPE_SWS, AUDIO_LIB_AEF_SWS, AUDIO_LIB_TYPE_OVERLAY},
        {AEF_TYPE_HVS, AUDIO_LIB_AEF_HVS, AUDIO_LIB_TYPE_OVERLAY},
        {AEF_TYPE_WDRC, AUDIO_LIB_AEF_WDRC, AUDIO_LIB_TYPE_OVERLAY},
        {AEF_TYPE_AEF_PRE, AUDIO_LIB_AEF_PRE, AUDIO_LIB_TYPE_OVERLAY},
        {AEF_TYPE_PEQ, AUDIO_LIB_AEF_PEQ, AUDIO_LIB_TYPE_BUILT_IN},
        {AEF_TYPE_DRC, AUDIO_LIB_AEF_DRC, AUDIO_LIB_TYPE_BUILT_IN},
#if defined(SAP_EFFECT_CUSTOMER_SUPPORT)
        {AEF_TYPE_CUSTOMER, AUDIO_LIB_AEF_CUSTOMER, AUDIO_LIB_TYPE_OVERLAY},
#endif
    };

    if (drv_msg_ao_get_lib_type(type_id, lib_type) != AUDIO_SUCCESS) {
        sap_err_log_info("drv ao get lib type failed");
    }

    for (i = 0; i < sizeof(aef_overlay_map) / sizeof(aef_overlay_map[0]); i++) {
        if (aef_overlay_map[i].type_id == type_id) {
            *lib_id = aef_overlay_map[i].lib_id;

            return AUDIO_SUCCESS;
        }
    }

    return ERR_AEF_NOT_SUPPORTED;
}

static td_s32 drv_aef_load_lib(td_handle ao, aef_type type)
{
    td_s32 ret;
    audio_lib_id lib_id;
    audio_lib_type lib_type;

    ret = drv_aef_get_lib_type(type, &lib_id, &lib_type);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_aef_get_lib_type, ret);
        return ERR_AEF_INVALID_PARAM;
    }

    if (lib_type == AUDIO_LIB_TYPE_BUILT_IN) {
        return AUDIO_SUCCESS;
    }

    ret = drv_overlay_request_firmware(ao, OVERLAY_AEF);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_overlay_request_firmware, ret);
        return ret;
    }

    ret = drv_overlay_load_lib_program(AUDIO_CORE_DSP0, lib_id, OVERLAY_AEF);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_overlay_load_lib_program, ret);
        (td_void)drv_overlay_release_firmware(ao, OVERLAY_AEF);
        return ERR_AEF_INVALID_HANDLE;
    }

    return AUDIO_SUCCESS;
}

static td_s32 drv_aef_unload_lib(td_handle ao, aef_type type)
{
    td_s32 ret;
    audio_lib_id lib_id;
    audio_lib_type lib_type;

    ret = drv_aef_get_lib_type(type, &lib_id, &lib_type);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_aef_get_lib_type, ret);
        return ERR_AEF_INVALID_PARAM;
    }

    if (lib_type == AUDIO_LIB_TYPE_BUILT_IN) {
        return AUDIO_SUCCESS;
    }

    (td_void)drv_overlay_release_firmware(ao, OVERLAY_AEF);
    (td_void)drv_overlay_unload_lib_program(OVERLAY_AEF);

    return AUDIO_SUCCESS;
}

static td_s32 drv_ao_load_aef_lib(aef_type type)
{
    sap_mod_id mod_id;

    mod_id.u32 = 0;
    mod_id.bits.type = SAP_MOD_AO;

    if (type == AEF_TYPE_NULL) {
        return AUDIO_SUCCESS;
    }

    return drv_aef_load_lib(mod_id.u32, type);
}

static td_s32 drv_ao_unload_aef_lib(aef_type type)
{
    sap_mod_id mod_id;

    mod_id.u32 = 0;
    mod_id.bits.type = SAP_MOD_AO;

    if (type == AEF_TYPE_NULL) {
        return AUDIO_SUCCESS;
    }

    return drv_aef_unload_lib(mod_id.u32, type);
}

static td_s32 ao_load_aef(drv_ao_inst *ao_inst, ao_aef_profile aef_profile)
{
    td_s32 ret;
    aef_type type = AEF_TYPE_NULL;

    if (aef_profile <= AO_AEF_PROFILE_NONE ||
        aef_profile >= AO_AEF_PROFILE_MAX ||
        ao_inst->type != AEF_TYPE_NULL) {
        return AUDIO_SUCCESS;
    }

#if defined(SAP_EFFECT_SMARTPA_SUPPORT) || defined(SAP_EFFECT_SMARTPA_V1_SUPPORT)
    type = AEF_TYPE_SMARTPA;
#endif
#ifdef SAP_EFFECT_SWS_SUPPORT
    type = AEF_TYPE_SWS;
#endif
#ifdef SAP_EFFECT_WDRC_SUPPORT
    type = AEF_TYPE_WDRC;
#endif
#ifdef SAP_EFFECT_HVS_SUPPORT
    type = AEF_TYPE_HVS;
#endif
#ifdef SAP_EFFECT_CUSTOMER_SUPPORT
    /* customer effect */
    type = AEF_TYPE_CUSTOMER;
#endif

    ret = drv_ao_load_aef_lib(type);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_ao_load_aef_lib, ret);
        return ret;
    }

    ao_inst->type = type;
    return AUDIO_SUCCESS;
}

static td_s32 ao_unload_aef(drv_ao_inst *ao_inst)
{
    td_s32 ret;

    if (ao_inst->open_cnt != 1) {
        return AUDIO_SUCCESS;
    }

    if (ao_inst->type == AEF_TYPE_NULL) {
        return AUDIO_SUCCESS;
    }

    ret = drv_ao_unload_aef_lib(ao_inst->type);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_ao_unload_aef_lib, ret);
        return ret;
    }

    ao_inst->type = AEF_TYPE_NULL;
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_open(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    ao_attr *attr = TD_NULL;
    td_handle ao_handle;
    sap_ao_open *param = TD_NULL;
    drv_ao_inst *ao_inst = TD_NULL;

    param = (sap_ao_open *)arg;
    attr = &param->attr;

    ret = drv_msg_ao_open(&ao_handle, param->snd_id, attr);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_open, ret);
        return ret;
    }

    ret = drv_ao_alloc_inst(&ao_inst, ao_handle);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_ao_alloc_inst, ret);
        drv_msg_ao_close(ao_handle);
        return ret;
    }

    ao_inst->ao_handle = ao_handle;
    param->h_mod = ao_handle;

#ifdef SAP_VENDOR_DRIVER_SUPPORT
    for (td_u32 i = 0; i < attr->port_num; ++i) {
        ao_out_port_attr *port = &(attr->port_attr[i]);
        audio_i2s_attr *i2s_attr = &(port->u.i2s_attr.attr);
        ret = drv_audio_vendor_init(port->out_port, attr->sample_rate, i2s_attr->bit_depth, i2s_attr->channels);
        if (ret != AUDIO_SUCCESS) {
            sap_err_log_h32(port->out_port);
            sap_err_log_fun(drv_audio_vendor_init, ret);
        }
    }
#endif

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_close(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_drv_handle *ao_handle = TD_NULL;
    drv_ao_inst *ao_inst = TD_NULL;

    ao_handle = (sap_drv_handle *)arg;
    drv_ao = ao_handle->h_mod;

#ifdef SAP_VENDOR_DRIVER_SUPPORT
    ao_attr attr = {0};

    ret = drv_msg_ao_get_attr(drv_ao, &attr);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_get_attr, ret);
        return ret;
    }

    for (td_u32 i = 0; i < attr.port_num; ++i) {
        ret = drv_audio_vendor_deinit(attr.port_attr[i].out_port);
        if (ret != AUDIO_SUCCESS) {
            sap_err_log_h32(attr.port_attr[i].out_port);
            sap_err_log_fun(drv_audio_vendor_deinit, ret);
        }
    }
#endif

    ret = drv_msg_ao_close(drv_ao);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_close, ret);
        return ret;
    }

    ret = drv_ao_find_inst(&ao_inst, drv_ao);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_h32(drv_ao);
        return ret;
    }

    (td_void)ao_unload_aef(ao_inst);

    ret = drv_ao_free_inst(drv_ao);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_ao_free_inst, ret);
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_get_enable(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    sap_drv_enable *en = (sap_drv_enable *)arg;

    ret = drv_msg_ao_get_enable(en->h_mod, &en->enable);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_get_enable, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_set_enable(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    sap_drv_enable *en = (sap_drv_enable *)arg;

    ret = drv_msg_ao_set_enable(en->h_mod, en->enable);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_set_enable, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_register_event_proc(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_drv_event *ao_event = TD_NULL;
    drv_ao_inst *ao_inst = TD_NULL;

    ao_event = (sap_drv_event *)arg;
    drv_ao = ao_event->h_mod;
    ret = drv_ao_find_inst(&ao_inst, drv_ao);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_h32(drv_ao);
        sap_err_log_fun(drv_ao_find_inst, ret);
        return ret;
    }

    ao_inst->event_proc = ao_event->event_proc;
    ao_inst->context = ao_event->context;

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_get_port_enable(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_ao_port_enable *ao_enable = TD_NULL;

    ao_enable = (sap_ao_port_enable *)arg;
    drv_ao = ao_enable->h_mod;

    ret = drv_msg_ao_get_port_enable(drv_ao, ao_enable->port, &ao_enable->enable);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_get_port_enable, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_set_port_enable(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_ao_port_enable *ao_enable = TD_NULL;

    ao_enable = (sap_ao_port_enable *)arg;
    drv_ao = ao_enable->h_mod;

    ret = drv_msg_ao_set_port_enable(drv_ao, ao_enable->port, ao_enable->enable);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_set_port_enable, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_get_mute(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_ao_port_enable *ao_enable = TD_NULL;

    ao_enable = (sap_ao_port_enable *)arg;
    drv_ao = ao_enable->h_mod;

    ret = drv_msg_ao_get_mute(drv_ao, ao_enable->port, &ao_enable->enable);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_get_mute, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_set_mute(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_ao_port_enable *ao_enable = TD_NULL;

    ao_enable = (sap_ao_port_enable *)arg;
    drv_ao = ao_enable->h_mod;

    ret = drv_msg_ao_set_mute(drv_ao, ao_enable->port, ao_enable->enable);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_set_mute, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_get_volume(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_ao_port_gain *param = TD_NULL;

    param = (sap_ao_port_gain *)arg;
    drv_ao = param->h_mod;

    ret = drv_msg_ao_get_volume(drv_ao, param->port, &param->gain);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_get_volume, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_set_volume(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_ao_port_gain *param = TD_NULL;

    param = (sap_ao_port_gain *)arg;
    drv_ao = param->h_mod;

    ret = drv_msg_ao_set_volume(drv_ao, param->port, &param->gain, &param->toggle_config);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_set_volume, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_get_track_mode(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_ao_port_mode *ao_mode = TD_NULL;

    ao_mode = (sap_ao_port_mode *)arg;
    drv_ao = ao_mode->h_mod;

    ret = drv_msg_ao_get_track_mode(drv_ao, ao_mode->port, &ao_mode->mode);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_get_track_mode, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_set_track_mode(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_ao_port_mode *ao_mode = TD_NULL;

    ao_mode = (sap_ao_port_mode *)arg;
    drv_ao = ao_mode->h_mod;

    ret = drv_msg_ao_set_track_mode(drv_ao, ao_mode->port, ao_mode->mode);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_set_track_mode, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_get_all_track_mute(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_drv_enable *ao_enable;

    ao_enable = (sap_drv_enable *)arg;
    drv_ao = ao_enable->h_mod;

    ret = drv_msg_ao_get_all_track_mute(drv_ao, &ao_enable->enable);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_get_all_track_mute, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_set_all_track_mute(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_drv_enable *ao_enable;

    ao_enable = (sap_drv_enable *)arg;
    drv_ao = ao_enable->h_mod;

    ret = drv_msg_ao_set_all_track_mute(drv_ao, ao_enable->enable);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_set_all_track_mute, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_set_bypass(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_ao_port_enable *ao_enable = TD_NULL;

    ao_enable = (sap_ao_port_enable *)arg;
    drv_ao = ao_enable->h_mod;

    ret = drv_msg_ao_set_bypass(drv_ao, ao_enable->port, ao_enable->enable);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_set_bypass, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_attach_output(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_ao_port_output *ao_output = TD_NULL;

    ao_output = (sap_ao_port_output *)arg;
    drv_ao = ao_output->h_mod;

    ret = drv_msg_ao_attach_output(drv_ao, ao_output->port, ao_output->h_output);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_attach_output, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_detach_output(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_ao_port_output *ao_output = TD_NULL;

    ao_output = (sap_ao_port_output *)arg;
    drv_ao = ao_output->h_mod;

    ret = drv_msg_ao_detach_output(drv_ao, ao_output->port, ao_output->h_output);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_detach_output, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_get_aef_param(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_ao_port_aef_param *attr = TD_NULL;

    attr = (sap_ao_port_aef_param *)arg;
    drv_ao = attr->h_mod;

    ret = drv_msg_ao_get_aef_param(drv_ao, attr->port, attr->cmd, attr->param, attr->param_size);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_get_aef_param, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_set_aef_param(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    sap_ao_port_aef_param *attr = TD_NULL;

    attr = (sap_ao_port_aef_param *)arg;
    drv_ao = attr->h_mod;

    ret = drv_msg_ao_set_aef_param(drv_ao, attr->port, attr->cmd, attr->param, attr->param_size);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_set_aef_param, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_get_aef_bypass(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    sap_ao_port_aef_bypass *attr = TD_NULL;

    attr = (sap_ao_port_aef_bypass *)arg;

    ret = drv_msg_ao_get_aef_bypass(attr->h_mod, attr->port, &attr->bypass);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_get_aef_bypass, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_set_aef_bypass(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    sap_ao_port_aef_bypass *attr = TD_NULL;

    attr = (sap_ao_port_aef_bypass *)arg;

    ret = drv_msg_ao_set_aef_bypass(attr->h_mod, attr->port, attr->bypass);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_set_aef_bypass, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_get_port_aef_enable(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    sap_ao_port_aef_enable *param = (sap_ao_port_aef_enable *)arg;

    ret = drv_msg_ao_get_port_aef_enable(param->h_mod, param->port, param->effect_type, &param->enable);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_get_port_aef_enable, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_set_port_aef_enable(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    sap_ao_port_aef_enable *param = (sap_ao_port_aef_enable *)arg;

    if (param->enable == TD_TRUE) {
        drv_ao_load_aef_lib(param->effect_type);
    }

    ret = drv_msg_ao_set_port_aef_enable(param->h_mod, param->port, param->effect_type, param->enable);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_set_port_aef_enable, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

#ifdef SAP_EFFECT_SMARTPA_SUPPORT
static td_s32 ao_get_aef_param_bin_by_profile(ao_aef_profile aef_profile, td_u8 *lib_buf, td_u32 *lib_buf_size)
{
    td_s32 ret;
    const struct {
        ao_aef_profile aef_profile;
        const td_char *bin_name;
    } aef_profile_bin_name[] = {
        {AO_AEF_PROFILE_MUSIC, "aef_music.bin"},
        {AO_AEF_PROFILE_VOIP, "aef_voip.bin"},
        {AO_AEF_PROFILE_RINGTONE, "aef_ringtone.bin"},
    };
    const td_u32 item_cnt = sizeof(aef_profile_bin_name) / sizeof(aef_profile_bin_name[0]);

    for (td_u32 i = 0; i < item_cnt; ++i) {
        if (aef_profile_bin_name[i].aef_profile == aef_profile) {
            sap_alert_log_str(aef_profile_bin_name[i].bin_name);
            ret = drv_overlay_get_lib(aef_profile_bin_name[i].bin_name, lib_buf, lib_buf_size);
            if (ret != AUDIO_SUCCESS) {
                sap_err_log_h32(aef_profile);
                sap_err_log_fun(drv_overlay_get_lib, ret);
                ret = ERR_AO_NOT_SUPPORTED;
            }
            return ret;
        }
    }

    return ERR_AO_NOT_SUPPORTED;
}

static td_s32 ao_set_aef_param_by_profile(td_handle drv_ao, const ao_attr *attr, ao_aef_profile aef_profile)
{
    td_s32 ret;
    td_u32 lib_buf_size = AO_AEF_PARAM_SIZE_MMZ_MAX;
    td_u8 *lib_buf = (td_u8 *)audio_osal_malloc(lib_buf_size);
    if (lib_buf == TD_NULL) {
        return ERR_AO_NULL_PTR;
    }

    ret = ao_get_aef_param_bin_by_profile(aef_profile, lib_buf, &lib_buf_size);
    if (ret == (td_s32)ERR_AO_NOT_SUPPORTED) {
        sap_warn_log_info("No need to load aef alp param bin.");
        goto out;
    }
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(ao_get_aef_param_bin_by_profile, ret);
        goto out;
    }

    for (td_u32 i = 0; i < attr->port_num; ++i) {
        ret = drv_msg_ao_set_aef_param(drv_ao, attr->port_attr[i].out_port, AEF_CMD_ALG_PARAM,
                                       (td_void *)lib_buf, lib_buf_size);
        if (ret != AUDIO_SUCCESS) {
            sap_err_log_u32(aef_profile);
            sap_err_log_h32(attr->port_attr[i].out_port);
            sap_err_log_fun(drv_msg_ao_set_aef_param, ret);
            goto out;
        }
    }

out:
    audio_osal_free(lib_buf);
    return ret;
}
#endif

#ifdef SAP_VENDOR_DRIVER_SUPPORT
static td_void sap_vendor_driver_set_aef_profile(const ao_attr *attr, ao_aef_profile aef_profile)
{
    for (td_u32 i = 0; i < attr->port_num; ++i) {
        td_s32 ret = drv_audio_vendor_set_aef_profile(attr->port_attr[i].out_port, aef_profile);
        if (ret != AUDIO_SUCCESS) {
            sap_err_log_u32(aef_profile);
            sap_err_log_h32(attr->port_attr[i].out_port);
            sap_err_log_fun(drv_audio_vendor_set_aef_profile, ret);
        }
    }
}
#endif

td_s32 sap_ioctl_ao_set_aef_profile(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_ao;
    ao_attr attr = {0};
    drv_ao_inst *ao_inst = TD_NULL;
    sap_ao_port_aef_profile *profile = TD_NULL;

    profile = (sap_ao_port_aef_profile *)arg;
    drv_ao = profile->h_mod;
    ret = drv_ao_find_inst(&ao_inst, drv_ao);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_h32(drv_ao);
        return ret;
    }

    ret = ao_load_aef(ao_inst, profile->aef_profile);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(ao_load_aef, ret);
        return ret;
    }

    ret = drv_msg_ao_set_aef_profile(drv_ao, profile->aef_profile);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_set_aef_profile, ret);
        (td_void)ao_unload_aef(ao_inst);
        return ret;
    }

    if (profile->aef_profile == AO_AEF_PROFILE_NONE || profile->aef_profile == AO_AEF_PROFILE_MAX) {
        return AUDIO_SUCCESS;
    }

    ret = drv_msg_ao_get_attr(drv_ao, &attr);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_get_attr, ret);
        (td_void)ao_unload_aef(ao_inst);
        return ret;
    }

#ifdef SAP_EFFECT_SMARTPA_SUPPORT
    ret = ao_set_aef_param_by_profile(drv_ao, &attr, profile->aef_profile);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(ao_set_aef_param_by_profile, ret);
    }
#endif

#ifdef SAP_VENDOR_DRIVER_SUPPORT
    sap_vendor_driver_set_aef_profile(&attr, profile->aef_profile);
#endif

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_set_spk_cali(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    sap_spk_cali_coefs *flt_coefs = (sap_spk_cali_coefs *)arg;

    ret = drv_msg_ao_set_spk_cali(flt_coefs->h_mod, &flt_coefs->cali_coefs);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_set_spk_cali, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_ao_get_mic_energy(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    sap_audio_mic_energy *energy = (sap_audio_mic_energy *)arg;

    ret = drv_msg_ao_get_mic_energy(energy->h_mod, &energy->mic_energy);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_get_mic_energy, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 drv_ao_get_attr(td_handle drv_ao, ao_attr *attr)
{
    td_s32 ret;

    ret = drv_msg_ao_get_attr(drv_ao, attr);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ao_get_attr, ret);
        return ret;
    }

    return AUDIO_SUCCESS;
}

#ifdef SAP_PROC_SUPPORT
td_s32 ao_read_proc(td_void *file, td_void *private)
{
    td_s32 ret;
    td_u8 ao_ind;
    drv_ao_inst *ao_inst = TD_NULL;
    ao_proc_item *proc = TD_NULL;

    audio_unused(private);

    proc = (ao_proc_item *)audio_osal_kmalloc(sizeof(ao_proc_item));
    if (proc == TD_NULL) {
        return ERR_AO_MEM_ALLOC;
    }

    for (ao_ind = 0; ao_ind < MOD_AO_MAX_NUM; ao_ind++) {
        ao_inst = &g_drv_ao_insts[ao_ind];
        if (ao_inst->ao_handle == 0) {
            continue;
        }

        (td_void)memset_s(proc, sizeof(*proc), 0, sizeof(*proc));
        ret = drv_msg_ao_get_proc_info(ao_inst->ao_handle, proc);
        if (ret != AUDIO_SUCCESS) {
            sap_err_log_fun(drv_msg_ao_get_proc_info, ret);
            continue;
        }

        ret = drv_proc_ao_show(file, ao_inst->ao_handle, proc);
        if (ret != AUDIO_SUCCESS) {
            sap_err_log_fun(drv_proc_ao_show, ret);
            continue;
        }
    }

    audio_osal_kfree(proc);

    return AUDIO_SUCCESS;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
