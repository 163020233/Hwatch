/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: ab(audio base) driver
 * Author: audio
 */

#include "drv_ab.h"
#include "securec.h"
#include "drv_msg_ab.h"
#include "drv_proc_ab.h"
#include "audio_debug.h"
#include "audio_osal.h"
#include "sys_cmd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef SAP_PROC_SUPPORT
static void drv_ab_get_tws_info(ab_tws_info *tws_info)
{
    td_s32 ret;

    ret = sys_audio_get_product_form(&tws_info->product_form);
    if (ret != AUDIO_SUCCESS) {
        tws_info->product_form = AUDIO_PRODUCT_FORM_MAX;
    }

    ret = sys_audio_get_tws_mono_mode(&tws_info->tws_mono_mode);
    if (ret != AUDIO_SUCCESS) {
        tws_info->tws_mono_mode = TD_FALSE;
    }

    ret = sys_audio_get_tws_mode(&tws_info->tws_mode);
    if (ret != AUDIO_SUCCESS) {
        tws_info->tws_mode = AUDIO_TWS_MODE_MAX;
    }

    ret = sys_audio_get_tws_role(&tws_info->tws_role);
    if (ret != AUDIO_SUCCESS) {
        tws_info->tws_role = AUDIO_TWS_ROLE_MAX;
    }
}

#ifdef SAP_LIVE_MIC_SUPPORT
static void drv_ab_get_bmic_info(ab_mic_info *bmic_info)
{
    bmic_info->mic_mode = AUDIO_MIC_MODE_NULL;
    bmic_info->mic_mix_mode = AUDIO_MIC_MIX_MODE_MONO;

    (td_void)sys_audio_get_mic_mode(&bmic_info->mic_mode);
    (td_void)sys_audio_get_mic_mix_mode(&bmic_info->mic_mix_mode);
}
#endif

td_s32 ab_read_proc(td_void *file, td_void *private)
{
    td_s32 ret;
    ab_proc_item *proc;

    audio_unused(private);

    proc = (ab_proc_item *)audio_osal_calloc(sizeof(ab_proc_item));
    if (proc == TD_NULL) {
        return ERR_SYS_MEM_ALLOC;
    }

    ret = drv_msg_ab_get_proc_info(proc);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ab_get_proc_info, ret);
        audio_osal_kfree(proc);
        return ret;
    }

    drv_ab_get_tws_info(&proc->tws_info);

#ifdef SAP_LIVE_MIC_SUPPORT
    drv_ab_get_bmic_info(&proc->mic_info);
#endif

    ret = drv_proc_ab_show(file, proc);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_proc_ab_show, ret);
        audio_osal_kfree(proc);
        return ret;
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
