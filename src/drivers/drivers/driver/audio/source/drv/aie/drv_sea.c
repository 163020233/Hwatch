/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: source file for drv sea
 * Author: audio
 */

#include "securec.h"
#include "audio_struct.h"
#include "audio_osal.h"
#include "audio_debug.h"
#include "core_module.h"
#include "core_msg_struct.h"
#include "core_msg.h"
#include "drv_sap_ioctl.h"
#include "drv_overlay_load.h"
#include "drv_msg_sea.h"
#include "drv_proc_sea.h"
#include "drv_sea.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_handle drv_sea;
    td_u32    afe_type;
    td_u64    context;
    td_u64    event_proc;
} drv_sea_inst;

static drv_sea_inst g_drv_sea_insts[MOD_SEA_MAX_NUM];

static td_s32 drv_sea_find_inst(drv_sea_inst **sea_inst, td_handle sea)
{
    td_u8 sea_ind;
    sap_mod_id mod_id;
    drv_sea_inst *sea_inst_p = TD_NULL;

    mod_id.u32 = (td_u32)sea;
    sea_ind = mod_id.bits.index;

    if (sea_ind >= MOD_SEA_MAX_NUM) {
        return ERR_SEA_INVALID_HANDLE;
    }
    sea_inst_p = &g_drv_sea_insts[sea_ind];

    if (sea_inst_p->drv_sea != sea) {
        return ERR_SEA_INVALID_HANDLE;
    }

    *sea_inst = sea_inst_p;
    return AUDIO_SUCCESS;
}

static td_s32 drv_sea_alloc_inst(drv_sea_inst **sea_inst, td_handle sea)
{
    td_u8 sea_ind;
    drv_sea_inst *sea_inst_p = TD_NULL;
    sap_mod_id mod_id;

    mod_id.u32 = (td_u32)sea;
    sea_ind = mod_id.bits.index;

    if (sea_ind >= MOD_SEA_MAX_NUM) {
        return ERR_SEA_INVALID_HANDLE;
    }
    sea_inst_p = &g_drv_sea_insts[sea_ind];

    if (sea_inst_p->drv_sea != 0) {
        return ERR_SEA_NOT_AVAILABLE;
    }

    sea_inst_p->drv_sea = sea;
    *sea_inst = sea_inst_p;
    return AUDIO_SUCCESS;
}

static td_s32 drv_sea_free_inst(td_handle sea)
{
    td_u8 sea_ind;
    sap_mod_id mod_id;
    drv_sea_inst *sea_inst_t = TD_NULL;

    mod_id.u32 = (td_u32)sea;
    sea_ind = mod_id.bits.index;

    if (sea_ind >= MOD_SEA_MAX_NUM) {
        return ERR_SEA_INVALID_HANDLE;
    }
    sea_inst_t = &g_drv_sea_insts[sea_ind];

    if (sea_inst_t->drv_sea != sea) {
        return ERR_SEA_INVALID_HANDLE;
    }

    (td_void)memset_s(sea_inst_t, sizeof(*sea_inst_t), 0, sizeof(drv_sea_inst));

    return AUDIO_SUCCESS;
}

static td_s32 drv_sea_get_lib_id(const sea_lib_id sea_id, audio_lib_id *lib_id, core_overlay_id *overlay_id)
{
    td_u32 i;
    const struct {
        sea_lib_id sea_id;
        audio_lib_id lib_id;
        core_overlay_id overlay_id;
    } overlay_map[] = {
        {SEA_LIB_ID_SEE, AUDIO_LIB_SEA_AFE_SEE, OVERLAY_SEA},
        {SEA_LIB_ID_VQE, AUDIO_LIB_SEA_AFE_VQE, OVERLAY_SEA},
        {SEA_LIB_ID_AHE, AUDIO_LIB_SEA_AFE_AHE, OVERLAY_SEA},
        {SEA_LIB_ID_KWS, AUDIO_LIB_SEA_AAI_KWS, OVERLAY_AAI},
        {SEA_LIB_ID_ASR, AUDIO_LIB_SEA_AAI_ASR, OVERLAY_AAI},
        {SEA_LIB_ID_EXT0, AUDIO_LIB_SEA_AFE_EXT0, OVERLAY_SEA},
        {SEA_LIB_ID_EXT1, AUDIO_LIB_SEA_AFE_EXT1, OVERLAY_SEA},
        {SEA_LIB_ID_EXT2, AUDIO_LIB_SEA_AAI_EXT0, OVERLAY_AAI},
        {SEA_LIB_ID_EXT3, AUDIO_LIB_SEA_AAI_EXT1, OVERLAY_AAI},
    };

    for (i = 0; i < sizeof(overlay_map) / sizeof(overlay_map[0]); i++) {
        if (overlay_map[i].sea_id == sea_id) {
            *lib_id = overlay_map[i].lib_id;
            *overlay_id = overlay_map[i].overlay_id;
            return AUDIO_SUCCESS;
        }
    }

    return ERR_SEA_NOT_SUPPORTED;
}

static td_s32 drv_sea_load_lib(td_handle sea, sea_lib_id sea_id)
{
    td_s32 ret;
    audio_lib_id lib_id;
    core_overlay_id overlay_id;

    ret = drv_sea_get_lib_id(sea_id, &lib_id, &overlay_id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_u32(sea_id);
        return ERR_SEA_INVALID_PARAM;
    }

    ret = drv_overlay_request_firmware(sea, overlay_id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_overlay_request_firmware, ret);
        return ret;
    }

    ret = drv_overlay_load_lib_program(AUDIO_CORE_DSP0, lib_id, overlay_id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_overlay_load_lib_program, ret);
        (td_void)drv_overlay_release_firmware(sea, overlay_id);
        return ret;
    }

    ret = drv_msg_sea_update_lib_status(sea_id, AUDIO_LIB_LOAD);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_u32(sea_id);
        sap_err_log_fun(drv_msg_sea_update_lib_status, ret);
        (td_void)drv_overlay_release_firmware(sea, overlay_id);
        (td_void)drv_overlay_unload_lib_program(overlay_id);
        return ret;
    }

    return AUDIO_SUCCESS;
}

static td_s32 drv_sea_unload_lib(td_handle sea, sea_lib_id sea_id)
{
    td_s32 ret;
    audio_lib_id lib_id;
    core_overlay_id overlay_id;

    ret = drv_msg_sea_update_lib_status(sea_id, AUDIO_LIB_UNLOAD);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_u32(sea_id);
        sap_err_log_fun(drv_msg_sea_update_lib_status, ret);
        return ret;
    }

    ret = drv_sea_get_lib_id(sea_id, &lib_id, &overlay_id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_u32(sea_id);
        return ERR_SEA_INVALID_PARAM;
    }

    (td_void)drv_overlay_release_firmware(sea, overlay_id);
    (td_void)drv_overlay_unload_lib_program(overlay_id);

    audio_unused(sea);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_init(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;

    ret = drv_msg_sea_init();
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_init, ret);
        return ret;
    }

    (td_void)memset_s(&g_drv_sea_insts, sizeof(g_drv_sea_insts), 0, sizeof(g_drv_sea_insts));

    audio_unused(cmd);
    audio_unused(arg);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_deinit(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;

    ret = drv_msg_sea_deinit();
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_deinit, ret);
        return ret;
    }

    (td_void)memset_s(&g_drv_sea_insts, sizeof(g_drv_sea_insts), 0, sizeof(g_drv_sea_insts));

    audio_unused(cmd);
    audio_unused(arg);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_load_engine(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    sap_mod_id mod_id;
    sap_drv_load *drv_load = (sap_drv_load *)arg;

    mod_id.u32 = 0;
    mod_id.bits.type = SAP_MOD_SEA;
    ret = drv_sea_load_lib(mod_id.u32, (sea_lib_id)drv_load->id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_sea_load_lib, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_unload_engine(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    sap_mod_id mod_id;
    sap_drv_load *drv_load = (sap_drv_load *)arg;

    mod_id.u32 = 0;
    mod_id.bits.type = SAP_MOD_SEA;
    ret = drv_sea_unload_lib(mod_id.u32, (sea_lib_id)drv_load->id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_sea_unload_lib, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_get_eng_caps(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    sap_sea_eng_caps *sea_caps;

    sea_caps = (sap_sea_eng_caps *)arg;
    ret = drv_msg_sea_get_eng_caps(sea_caps->lib_id, &sea_caps->eng_caps);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_get_eng_caps, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_get_def_attr(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    sap_sea_attr *sea_attr;

    sea_attr = (sap_sea_attr *)arg;
    ret = drv_msg_sea_get_def_pcm_attr(&sea_attr->eng_sel, &sea_attr->attr);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_get_def_pcm_attr, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_create(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_sea_attr *sea_attr = TD_NULL;
    drv_sea_inst *sea_inst = TD_NULL;

    sea_attr = (sap_sea_attr *)arg;

    ret = drv_msg_sea_create(&drv_sea, &sea_attr->eng_sel, &sea_attr->attr);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_create, ret);
        return ret;
    }

    ret = drv_sea_alloc_inst(&sea_inst, drv_sea);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_sea_alloc_inst, ret);
        drv_msg_sea_destroy(drv_sea);
        return ret;
    }

    sea_inst->afe_type = sea_attr->eng_sel.eng_ind[SEA_ECH_AFE_IND].type;
    sea_inst->drv_sea = drv_sea;
    sea_attr->h_mod = drv_sea;

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_destroy(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_drv_handle *sea_handle = TD_NULL;

    sea_handle = (sap_drv_handle *)arg;
    drv_sea = sea_handle->h_mod;

    ret = drv_msg_sea_destroy(drv_sea);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_destroy, ret);
        return ret;
    }

    ret = drv_sea_free_inst(drv_sea);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_sea_free_inst, ret);
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_register_event_proc(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_drv_event *sea_event = TD_NULL;
    drv_sea_inst *sea_inst = TD_NULL;

    sea_event = (sap_drv_event *)arg;
    drv_sea = sea_event->h_mod;
    ret = drv_sea_find_inst(&sea_inst, drv_sea);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_h32(drv_sea);
        sap_err_log_fun(drv_sea_find_inst, ret);
        return ret;
    }

    sea_inst->event_proc = sea_event->event_proc;
    sea_inst->context = sea_event->context;

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_start(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_drv_handle *sea_handle = TD_NULL;

    sea_handle = (sap_drv_handle *)arg;
    drv_sea = sea_handle->h_mod;

    ret = drv_msg_sea_start(drv_sea);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_start, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_stop(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_drv_handle *sea_handle = TD_NULL;

    sea_handle = (sap_drv_handle *)arg;
    drv_sea = sea_handle->h_mod;

    ret = drv_msg_sea_stop(drv_sea);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_stop, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_get_eng_attr(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_sea_eng_attr *eng_attr = TD_NULL;

    eng_attr = (sap_sea_eng_attr *)arg;
    drv_sea = eng_attr->h_mod;

    ret = drv_msg_sea_get_eng_attr(drv_sea, &eng_attr->attr);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_get_eng_attr, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_set_eng_attr(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_sea_eng_attr *eng_attr = TD_NULL;

    eng_attr = (sap_sea_eng_attr *)arg;
    drv_sea = eng_attr->h_mod;

    ret = drv_msg_sea_set_eng_attr(drv_sea, &eng_attr->attr);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_set_eng_attr, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_get_pcm_attr(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_sea_attr *sea_attr = TD_NULL;

    sea_attr = (sap_sea_attr *)arg;
    drv_sea = sea_attr->h_mod;

    ret = drv_msg_sea_get_pcm_attr(drv_sea, &sea_attr->attr);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_get_pcm_attr, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_set_pcm_attr(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_sea_attr *sea_attr = TD_NULL;

    sea_attr = (sap_sea_attr *)arg;
    drv_sea = sea_attr->h_mod;

    ret = drv_msg_sea_set_pcm_attr(drv_sea, &sea_attr->attr);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_set_pcm_attr, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_get_param(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_sea_param *attr = TD_NULL;

    attr = (sap_sea_param *)arg;
    drv_sea = attr->h_mod;

    ret = drv_msg_sea_get_param(drv_sea, attr->cmd_id, attr->param, attr->param_size);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_get_param, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_set_param(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_sea_param *attr = TD_NULL;

    attr = (sap_sea_param *)arg;
    drv_sea = attr->h_mod;

    ret = drv_msg_sea_set_param(drv_sea, attr->cmd_id, attr->param, attr->param_size);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_set_param, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_set_bypass(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_drv_enable *sea_enable = TD_NULL;

    sea_enable = (sap_drv_enable *)arg;
    drv_sea = sea_enable->h_mod;

    ret = drv_msg_sea_set_bypass(drv_sea, sea_enable->enable);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_set_bypass, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_get_item_count(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_sea_item_count *sea_item = TD_NULL;

    sea_item = (sap_sea_item_count *)arg;
    drv_sea = sea_item->h_mod;

    ret = drv_msg_sea_get_item_count(drv_sea, sea_item->type, &sea_item->count);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_get_item_count, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_get_item_sets(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_sea_item_sets *sea_item = TD_NULL;

    sea_item = (sap_sea_item_sets *)arg;
    drv_sea = sea_item->h_mod;

    ret = drv_msg_sea_get_item_sets(drv_sea, sea_item->type, &sea_item->sets);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_get_item_sets, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_add_item(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_sea_item_node *sea_item = TD_NULL;

    sea_item = (sap_sea_item_node *)arg;
    drv_sea = sea_item->h_mod;

    ret = drv_msg_sea_add_item(drv_sea, sea_item->type, &sea_item->item);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_add_item, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_remove_item(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_sea_item_node *sea_item = TD_NULL;

    sea_item = (sap_sea_item_node *)arg;
    drv_sea = sea_item->h_mod;

    ret = drv_msg_sea_remove_item(drv_sea, sea_item->type, &sea_item->item);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_remove_item, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_enable_vid_enroll(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_drv_enable *sea_enable = TD_NULL;

    sea_enable = (sap_drv_enable *)arg;
    drv_sea = sea_enable->h_mod;

    ret = drv_msg_sea_enable_vid_enroll(drv_sea, sea_enable->enable);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_enable_vid_enroll, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_attach_output(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_drv_attach_para *attach_para = (sap_drv_attach_para *)arg;

    drv_sea = attach_para->h_src;

    ret = drv_msg_sea_attach_output(drv_sea, attach_para->data_port_id, attach_para->h_sink);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_attach_output, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

td_s32 sap_ioctl_sea_detach_output(td_u32 cmd, td_void *arg, td_void *file)
{
    td_s32 ret;
    td_handle drv_sea;
    sap_drv_attach_para *attach_para = (sap_drv_attach_para *)arg;

    drv_sea = attach_para->h_src;

    ret = drv_msg_sea_detach_output(drv_sea, attach_para->data_port_id, attach_para->h_sink);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_detach_output, ret);
        return ret;
    }

    audio_unused(cmd);
    audio_unused(file);
    return AUDIO_SUCCESS;
}

static td_s32 drv_msg_sea_report_event(core_msg_data *msg_data)
{
    td_s32 ret;
    msg_data_sea_event_info *sea_event = (msg_data_sea_event_info *)msg_data->input_data;
    td_handle sea_handle = msg_data->handle;
    drv_sea_inst *sea_inst = TD_NULL;
    audio_event_proc event_proc = TD_NULL;

    ret = drv_sea_find_inst(&sea_inst, sea_handle);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_h32(sea_handle);
        sap_err_log_fun(drv_sea_find_inst, ret);
        return ret;
    }

    event_proc = (audio_event_proc)(td_uintptr_t)(sea_inst->event_proc);
    if (event_proc != TD_NULL) {
        event_proc(sea_handle, sea_event->event, &(sea_event->param), (td_void *)(td_uintptr_t)sea_inst->context);
    }

    return AUDIO_SUCCESS;
}

static const core_msg_module_func g_drv_msg_sea_func[MSG_DRV_SEA_CMD_MAX] = {
    [MSG_DRV_SEA_REPORT_EVENT] = {drv_msg_sea_report_event}
};

static td_s32 drv_msg_sea_callback(core_msg_data *msg_data)
{
    if (msg_data->cmd >= MSG_DRV_SEA_CMD_MAX) {
        sap_err_log_u32(msg_data->cmd);
        return ERR_SEA_INVALID_PARAM;
    }

    return g_drv_msg_sea_func[msg_data->cmd].func(msg_data);
}

td_s32 drv_msg_sea_module_init(audio_core_id core_id)
{
    td_s32 ret;
    ret = core_msg_register_module(core_id, MSG_CORE_SEA_MODULE, drv_msg_sea_callback);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(core_msg_register_module, ret);
        return ERR_SEA_INIT_FAILED;
    }
    return AUDIO_SUCCESS;
}

td_s32 drv_sea_get_afe_type(td_handle drv_sea, td_u32 *afe_type)
{
    td_s32 ret;
    drv_sea_inst *sea_inst = TD_NULL;

    ret = drv_sea_find_inst(&sea_inst, drv_sea);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_h32(drv_sea);
        sap_err_log_fun(drv_sea_find_inst, ret);
        return ret;
    }

    *afe_type = sea_inst->afe_type;
    return AUDIO_SUCCESS;
}

#ifdef SAP_PROC_SUPPORT
td_s32 sea_read_proc(td_void *file, td_void *private)
{
    td_s32 ret;
    td_u8 sea_id;
    drv_sea_inst *sea_inst = TD_NULL;
    sea_proc_item *proc = TD_NULL;

    audio_unused(private);

    proc = (sea_proc_item *)audio_osal_kmalloc(sizeof(sea_proc_item));
    if (proc == TD_NULL) {
        return ERR_SEA_MEM_ALLOC;
    }

    for (sea_id = 0; sea_id < MOD_SEA_MAX_NUM; sea_id++) {
        sea_inst = &g_drv_sea_insts[sea_id];
        if (sea_inst->drv_sea == 0) {
            continue;
        }

        (td_void)memset_s(proc, sizeof(*proc), 0, sizeof(*proc));
        ret = drv_msg_sea_get_proc_info(sea_inst->drv_sea, proc);
        if (ret != AUDIO_SUCCESS) {
            sap_err_log_fun(drv_msg_sea_get_proc_info, ret);
            continue;
        }

        ret = drv_proc_sea_show(file, sea_inst->drv_sea, proc);
        if (ret != AUDIO_SUCCESS) {
            sap_err_log_fun(drv_proc_sea_show, ret);
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
