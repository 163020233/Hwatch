/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: hal interface
 * Author: audio
 */

#include "audio_osal.h"
#include "audio_debug.h"
#include "mem_addr.h"
#include "core_interface.h"
#include "core_msg.h"
#include "drv_audio_osal.h"
#include "drv_audio_dev.h"
#include "drv_sys.h"
#include "drv_adp.h"
#include "drv_adec.h"
#include "drv_ao.h"
#include "drv_ai.h"
#include "drv_haid.h"
#include "drv_vad.h"
#include "drv_aenc.h"
#include "drv_sea.h"
#include "drv_ab.h"
#include "drv_hal.h"
#include "drv_overlay_load.h"
#include "ipc.h"
#ifndef SAP_IPC_DRV_VERSION_V0
#include "ipc_porting.h"
#endif
#include "drv_sap_msg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef SOC_SUPPORT_DSP_MSG_TO_APPS

#define SAP_MSG_TASK_PRIO 7
#define SAP_MSG_TASK_NAME "sap_msg"

#define SAP_MSG_EVENT_NUM 128
#define SAP_MSG_EVENT_SIZE sizeof(td_u16)
#define SAP_MSG_QUEUE_TIMEOUT 0 /* Set 0 for ISR */
#define SAP_MSG_WAIT_FOREVER 0xFFFFFFFF

#define sap_msg_gen_event_id(mod, event) ((((mod) << 8) & 0xFF00) | ((event) & 0x00FF))

typedef struct {
    dsp_int_cfg int_cfg;
    td_ulong msg_qid;
    osal_task *msg_task;
#ifdef SAP_DEBUG_RUN_STATUS_INFO
    td_u32 event_cnt_rcv;
    td_u32 event_cnt_diff;
#endif
} dsp_int_mgr;

static dsp_int_mgr g_dsp_int_mgr = {
    .msg_qid = 0L,
    .msg_task = TD_NULL,
#ifdef SAP_DEBUG_RUN_STATUS_INFO
    .event_cnt_rcv = 0,
    .event_cnt_diff = 0,
#endif
};

#ifdef SAP_IPC_DRV_VERSION_V0
static bool sap_isr_proc(ipc_action_t message,
    const volatile ipc_payload *payload,
    cores_t src, uint32_t id)
{
    td_u16 msg_data = 0;
    dsp_int_mgr *int_mgr = &g_dsp_int_mgr;

    osal_msg_queue_write_copy(int_mgr->msg_qid, &msg_data, sizeof(msg_data), SAP_MSG_QUEUE_TIMEOUT);

    audio_unused(message);
    audio_unused(payload);
    audio_unused(src);
    audio_unused(id);
    return TD_TRUE;
}

static td_s32 sap_register_int(dsp_int_cfg *int_cfg)
{
    td_s32 ret;

    ret = ipc_register_handler(IPC_ACTION_DSP_EVENT_REPORT, sap_isr_proc);
    if (ret != 0) {
        sap_err_log_fun(ipc_register_handler, ret);
    }

    audio_unused(int_cfg);
    return ret;
}

static td_void sap_unreg_int(dsp_int_cfg *int_cfg)
{
    td_s32 ret;

    ret = ipc_unregister_handler(IPC_ACTION_DSP_EVENT_REPORT);
    if (ret != 0) {
        sap_err_log_fun(ipc_unregister_handler, ret);
    }

    audio_unused(int_cfg);
}
#else
#ifdef SAP_DEBUG_RUN_STATUS_INFO
static void sap_isr_event_cnt_dfx(dsp_int_mgr *int_mgr, td_u8 *payload_addr, td_u32 payload_len)
{
    td_u32 event_cnt_cur;
    td_u32 *g_event_cnt_dsp = TD_NULL;

    if (payload_addr == TD_NULL || payload_len < sizeof(td_u32)) {
        sap_err_log_u32(payload_len);
        return;
    }

    int_mgr->event_cnt_rcv++;
    g_event_cnt_dsp = (td_u32 *)payload_addr;

    event_cnt_cur = audio_osal_cali_time_diff_us(*g_event_cnt_dsp, int_mgr->event_cnt_rcv);
    if (event_cnt_cur != int_mgr->event_cnt_diff) {
        audio_log3(AUDIO_LOG_LEVEL_WARN,
            "*g_event_cnt_dsp: %u |int_mgr->event_cnt_rcv: %u |int_mgr->event_cnt_diff: %u",
            *g_event_cnt_dsp, int_mgr->event_cnt_rcv, int_mgr->event_cnt_diff);
        int_mgr->event_cnt_diff = event_cnt_cur;
    }
}
#endif

static void sap_isr_proc(td_u8 *payload_addr, td_u32 payload_len)
{
    td_u16 msg_data = 0;
    dsp_int_mgr *int_mgr = &g_dsp_int_mgr;

#ifdef SAP_DEBUG_RUN_STATUS_INFO
    sap_isr_event_cnt_dfx(int_mgr, payload_addr, payload_len);
#endif

    osal_msg_queue_write_copy(int_mgr->msg_qid, &msg_data, sizeof(msg_data), SAP_MSG_QUEUE_TIMEOUT);

    audio_unused(payload_addr);
    audio_unused(payload_len);
}

static td_s32 sap_register_int(dsp_int_cfg *int_cfg)
{
    ipc_rx_handler_info_t info;
    td_s32 ret;

    info.msg_id = IPC_MSG_DSP_EVENT_REPORT;
    info.pre_cb = TD_NULL;
    info.cb = sap_isr_proc;

    ret = (td_s32)uapi_ipc_register_rx_handler(&info);
    if (ret != 0) {
        sap_err_log_fun(uapi_ipc_register_rx_handler, ret);
    }

    audio_unused(int_cfg);
    return ret;
}

static td_void sap_unreg_int(dsp_int_cfg *int_cfg)
{
    audio_unused(int_cfg);
}
#endif

static td_s32 sap_msg_proc(td_void *param)
{
    td_s32 ret;
    td_u16 vad_event = 0;
    td_u16 msg_data = 0;
    td_u32 msg_size = (td_u32)sizeof(msg_data);
    dsp_int_mgr *int_mgr = (dsp_int_mgr *)param;

    while (TD_TRUE) {
        ret = osal_msg_queue_read_copy(int_mgr->msg_qid, &msg_data, &msg_size, SAP_MSG_WAIT_FOREVER);
        if (ret != OSAL_SUCCESS) {
            continue;
        }

        if (msg_data == 0) {
            core_msg_process(AUDIO_CORE_DSP0);
        } else {
            vad_event = (td_u16)sap_msg_gen_event_id(MSG_CORE_AI_MODULE, VAD_EVENT_VAD_VALID);
            if (msg_data == vad_event) {
                drv_vad_intr_proc();
            }
        }
    }

    audio_unused(param);
    return AUDIO_SUCCESS;
}

static td_s32 sap_msg_task_create(dsp_int_mgr *int_mgr)
{
    td_s32 ret;

    osal_msg_queue_create(SAP_MSG_TASK_NAME, SAP_MSG_EVENT_NUM, &int_mgr->msg_qid, 0, SAP_MSG_EVENT_SIZE);
    int_mgr->msg_task = osal_kthread_create(sap_msg_proc, (td_void *)int_mgr, SAP_MSG_TASK_NAME, SAP_MSG_TASK_SIZE);
    if (int_mgr->msg_task == TD_NULL) {
        sap_err_log_fun(osal_kthread_create, TD_NULL);
        goto out0;
    }

    (td_void)osal_kthread_set_priority(int_mgr->msg_task, SAP_MSG_TASK_PRIO);

    ret = sap_register_int(&int_mgr->int_cfg);
    if (ret != AUDIO_SUCCESS) {
        goto out1;
    }

    return AUDIO_SUCCESS;
out1:
    osal_kthread_destroy(int_mgr->msg_task, TD_TRUE);
out0:
    osal_msg_queue_delete(int_mgr->msg_qid);
    int_mgr->msg_task = TD_NULL;
    int_mgr->msg_qid = 0L;
    return ERR_SYS_INIT_FAILED;
}

static td_void sap_msg_task_destroy(dsp_int_mgr *int_mgr)
{
    sap_unreg_int(&int_mgr->int_cfg);
    osal_kthread_destroy(int_mgr->msg_task, TD_TRUE);
    osal_msg_queue_delete(int_mgr->msg_qid);
    int_mgr->msg_task = TD_NULL;
    int_mgr->msg_qid = 0L;
}

#endif /* SOC_SUPPORT_DSP_MSG_TO_APPS */

td_s32 drv_sap_msg_start(audio_core_id core_id)
{
#ifdef SOC_SUPPORT_DSP_MSG_TO_APPS
    td_s32 ret;
    dsp_int_mgr *int_mgr = &g_dsp_int_mgr;

    ret = drv_msg_sys_module_init(core_id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sys_module_init, ret);
        return ret;
    }

    ret = drv_msg_ai_module_init(core_id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_ai_module_init, ret);
        return ret;
    }
#ifdef SAP_HAID_SUPPORT
    ret = drv_msg_haid_module_init(core_id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_haid_module_init, ret);
        return ret;
    }
#endif
    ret = drv_msg_sea_module_init(core_id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sea_module_init, ret);
        return ret;
    }

    ret = drv_msg_adp_module_init(core_id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_adp_module_init, ret);
        return ret;
    }
#ifndef TEST_SUITE
    if (core_id == AUDIO_CORE_DSP0) {
        hal_dsp_get_int_cfg(core_id, &int_mgr->int_cfg);
        ret = sap_msg_task_create(int_mgr);
        if (ret != AUDIO_SUCCESS) {
            sap_err_log_fun(sap_msg_task_create, ret);
            return ret;
        }
    }
#endif
#endif /* SOC_SUPPORT_DSP_MSG_TO_APPS */
    audio_unused(core_id);
    return AUDIO_SUCCESS;
}

td_s32 drv_sap_msg_stop(audio_core_id core_id)
{
#ifdef SOC_SUPPORT_DSP_MSG_TO_APPS
    dsp_int_mgr *int_mgr = &g_dsp_int_mgr;
#ifndef TEST_SUITE
    if (core_id == AUDIO_CORE_DSP0) {
        sap_msg_task_destroy(int_mgr);
    }
#endif
#endif /* SOC_SUPPORT_DSP_MSG_TO_APPS */
    audio_unused(core_id);
    return AUDIO_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
