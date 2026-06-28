/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: core ab module
 * Author: audio
 */

#include "platform.h"
#include "system.h"
#include "securec.h"
#include "audio_debug.h"
#include "audio_osal.h"
#include "dsp_timer.h"
#include "common_profile.h"
#include "core_cps.h"
#include "core_msg.h"
#include "core_overlay_info.h"
#include "core_adp.h"
#include "core_util.h"
#ifdef SAP_IPC_LOG_SUPPORT
#include "log_dsp.h"
#endif
#include "sys_cmd.h"
#include "set_freq.h"
#include "core_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    sap_mod_head head;

    td_bool inited;
    td_bool enable;
    td_bool cps_change;
    sys_state_type state;
    dsp_timer_id state_timer;
    dsp_timer_id work_timer;
    td_u32 total_cps;
    td_u32 interval;
    td_u32 idle_times_cnt;
    td_u32 idle_times_max;
    td_void *context;
    audio_event_proc event_proc;
} core_sys_inst;

static core_sys_inst g_sys_inst = {
    .inited = TD_FALSE,
    .enable = TD_FALSE,
    .cps_change = TD_FALSE,
    .idle_times_cnt = 0,
    .idle_times_max = 4,
    .state = SYS_STATE_DEEP_STANDBY,
    .total_cps = SAP_DSP_CLK_HZ,
    .state_timer = DSP_TIMER0,
    .work_timer = DSP_TIMER1,
    .interval = 1000,
    .context = TD_NULL,
    .event_proc = TD_NULL,
};

static td_s32 sys_cps_change_proc(td_void *context)
{
    core_sys_inst *sys_inst = (core_sys_inst *)context;
    sys_inst->cps_change = TD_TRUE;
    return AUDIO_SUCCESS;
}

static td_s32 sys_cps_idle_proc(td_void *context)
{
    core_sys_inst *sys_inst = (core_sys_inst *)context;
    sap_debug_module_handle("[module] sys_cps_idle_proc: sys_inst handle: 0x%08X", sys_inst);
    sys_inst->idle_times_cnt = 0;
    return AUDIO_SUCCESS;
}

td_bool core_sys_check_idle(td_void)
{
    if (core_cps_check_no_inst() == TD_FALSE) {
        return TD_FALSE;
    }
#ifdef CORE_ADP_SUPPORT
    if (core_adp_check_no_inst() == TD_FALSE) {
        return TD_FALSE;
    }
#endif /* CORE_ADP_SUPPORT */
    if (core_msg_check_empty(AUDIO_CORE_CPU_K) == TD_FALSE) {
        return TD_FALSE;
    }

    return TD_TRUE;
}

static td_void sys_state_check_proc(core_sys_inst *sys_inst)
{
    td_u32 mcps_quota = 0;

    if (sys_inst->event_proc == TD_NULL) {
        return;
    }

    if (sys_inst->cps_change) {
        sys_inst->cps_change = TD_FALSE;
        core_cps_query_total_quota(&mcps_quota);
        sys_inst->event_proc(0, SYS_EVENT_CPS_CHANGE, (td_void *)&mcps_quota, sys_inst->context);
    }

    if (core_sys_check_idle() == TD_TRUE) {
        sys_inst->idle_times_cnt++;
    } else {
        sys_inst->idle_times_cnt = 0;
    }

    if (sys_inst->idle_times_cnt >= sys_inst->idle_times_max) {
        sys_inst->event_proc(0, SYS_EVENT_NO_SERVICE, TD_NULL, sys_inst->context);
        sys_inst->idle_times_cnt = 0;
    }
}

static td_void sys_timer_proc(td_void *context)
{
    core_sys_inst *sys_inst = (core_sys_inst *)context;

    if (sys_inst->enable == TD_FALSE) {
        return;
    }

    sys_state_check_proc(sys_inst);

    cps_stat_period_proc(sys_inst->total_cps, sys_inst->interval);
}

static td_s32 core_sys_init(td_void)
{
    td_s32 ret;
    td_u32 interval_cycles;
    core_sys_inst *sys_inst = &g_sys_inst;

    audio_func_enter();
    if (sys_inst->inited == TD_TRUE) {
        audio_func_exit();
        return AUDIO_SUCCESS;
    }

    ret = core_cps_init();
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(core_cps_init, ret);
        return ret;
    }

    (td_void)memset_s(&sys_inst->head, sizeof(sys_inst->head), 0, sizeof(sys_inst->head));
    sys_inst->head.mid.bits.type = SAP_MOD_SYS;

    if (system_get_core_id() == AUDIO_CORE_DSP0) {
        interval_cycles = (sys_inst->total_cps / AUDIO_MS_NUM_PER_SECOND) * sys_inst->interval;
        timer_init(sys_inst->state_timer, interval_cycles);
        timer_reg_isr_func(sys_inst->state_timer, sys_timer_proc, (td_void *)sys_inst);
        timer_set_enable(sys_inst->state_timer);
    }

    interval_cycles = (sys_inst->total_cps / AUDIO_MS_NUM_PER_SECOND);
    timer_init(sys_inst->work_timer, interval_cycles);
    timer_set_enable(sys_inst->work_timer);

    sys_inst->inited = TD_TRUE;
    sys_inst->enable = TD_TRUE;

    core_cps_register_change_proc(sys_cps_change_proc, (td_void *)sys_inst);
    core_cps_register_idle_proc(sys_cps_idle_proc, (td_void *)sys_inst);
    audio_func_exit();
    return AUDIO_SUCCESS;
}

static td_s32 core_sys_deinit(td_void)
{
    td_s32 ret;
    core_sys_inst *sys_inst = &g_sys_inst;

    audio_func_enter();
    if (sys_inst->inited == TD_FALSE) {
        audio_func_exit();
        return AUDIO_SUCCESS;
    }

    timer_set_disable(sys_inst->state_timer);
    timer_deinit(sys_inst->state_timer);

    timer_set_disable(sys_inst->work_timer);
    timer_deinit(sys_inst->work_timer);

    ret = core_cps_deinit();
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(core_cps_deinit, ret);
    }

    sys_inst->event_proc = TD_NULL;
    sys_inst->context = TD_NULL;
    sys_inst->inited = TD_FALSE;
    sys_inst->enable = TD_FALSE;

    audio_func_exit();
    return AUDIO_SUCCESS;
}

td_s32 core_sys_suspend(td_void)
{
    audio_func_enter();
    audio_func_exit();
    return AUDIO_SUCCESS;
}

td_s32 core_sys_resume(td_void)
{
    audio_func_enter();
    audio_func_exit();
    return AUDIO_SUCCESS;
}

AUDIO_STATIC sap_mod_handle core_sys_get_mod_handle(td_handle sys)
{
    audio_unused(sys);
    return (sap_mod_handle)&g_sys_inst;
}

td_s32 core_sys_set_total_cps(td_handle sys, td_u32 total_cps)
{
    td_u32 interval_cycles;
    core_sys_inst *sys_inst = &g_sys_inst;
    if (sys_inst->inited == TD_FALSE) {
        return ERR_SYS_NOT_INIT;
    }

    audio_func_enter();
    sys_inst->total_cps = total_cps;

    timer_set_disable(sys_inst->work_timer);
    timer_init(sys_inst->work_timer, (sys_inst->total_cps / AUDIO_MS_NUM_PER_SECOND));
    timer_set_enable(sys_inst->work_timer);

    if (sys_inst->enable == TD_TRUE) {
        timer_set_disable(sys_inst->state_timer);
    }

    interval_cycles = (sys_inst->total_cps / AUDIO_MS_NUM_PER_SECOND) * sys_inst->interval;
    timer_init(sys_inst->state_timer, interval_cycles);

    if (sys_inst->enable == TD_TRUE) {
        timer_set_enable(sys_inst->state_timer);
    }

    audio_unused(sys);
    audio_func_exit();
    return AUDIO_SUCCESS;
}

td_s32 core_sys_set_idle_timeout(td_handle sys, td_u32 timeout)
{
    core_sys_inst *sys_inst = &g_sys_inst;
    if (sys_inst->inited == TD_FALSE) {
        return ERR_SYS_NOT_INIT;
    }

    audio_func_enter();
    sys_inst->idle_times_max = timeout;
    audio_unused(sys);
    audio_func_exit();
    return AUDIO_SUCCESS;
}

td_s32 core_sys_register_event_proc(audio_event_proc event_proc, td_void *context)
{
    core_sys_inst *sys_inst = &g_sys_inst;
    audio_func_enter();
    sys_inst->context = context;
    sys_inst->event_proc = event_proc;
    audio_func_exit();
    return AUDIO_SUCCESS;
}

td_s32 core_sys_get_proc_info(sys_proc_item *proc)
{
    cps_stat_query_total(proc);
    core_cps_query_mod_status(proc->mcps_list, MCPS_BLOCK_NUM);

    mem_stat_query_detail(proc);

    shm_stat_query_detail(proc);

    return AUDIO_SUCCESS;
}

td_bool core_sys_check_tws_master_ear(td_void)
{
    td_s32 ret;
    audio_tws_role tws_role = AUDIO_TWS_ROLE_FREEMAN;

    ret = sys_audio_get_tws_role(&tws_role);
    if (ret != AUDIO_SUCCESS) {
        return TD_FALSE;
    }

    return (tws_role == AUDIO_TWS_ROLE_MASTER);
}

td_bool core_sys_check_tws_slave_ear(td_void)
{
    td_s32 ret;
    audio_tws_role tws_role = AUDIO_TWS_ROLE_FREEMAN;

    ret = sys_audio_get_tws_role(&tws_role);
    if (ret != AUDIO_SUCCESS) {
        return TD_FALSE;
    }

    return (tws_role == AUDIO_TWS_ROLE_SLAVE);
}

td_bool core_sys_check_tws_binaural_sync_enable(td_void)
{
    td_s32 ret;
    audio_tws_role tws_role = AUDIO_TWS_ROLE_FREEMAN;

    ret = sys_audio_get_tws_role(&tws_role);
    if (ret != AUDIO_SUCCESS) {
        return TD_FALSE;
    }

    return ((tws_role == AUDIO_TWS_ROLE_MASTER) || (tws_role == AUDIO_TWS_ROLE_SLAVE));
}

td_bool core_sys_check_tws_dual_transmit(td_void)
{
    td_s32 ret;
    audio_tws_mode tws_mode;

    ret = sys_audio_get_tws_mode(&tws_mode);
    if (ret != AUDIO_SUCCESS) {
        return TD_FALSE;
    }

    return (tws_mode == AUDIO_TWS_MODE_DUAL_TRANSMIT);
}

ao_track_mode core_sys_get_tws_channel_mode(td_void)
{
    td_s32 ret;
    td_bool tws_mono = TD_FALSE;
    audio_product_form product_form = AUDIO_PRODUCT_FORM_NON_TWS;

    ret = sys_audio_get_product_form(&product_form);
    if (ret != AUDIO_SUCCESS) {
        return AO_TRACK_DOUBLE_MONO;
    }

    ret = sys_audio_get_tws_mono_mode(&tws_mono);
    if (ret != AUDIO_SUCCESS) {
        return AO_TRACK_DOUBLE_MONO;
    }

    /* 单耳模式，左右声道混音输出 */
    if (tws_mono == TD_TRUE) {
        return AO_TRACK_DOUBLE_MONO;
    }

    /* 非TWS模式，双声道输出 */
    if (product_form == AUDIO_PRODUCT_FORM_NON_TWS) {
        return AO_TRACK_STEREO;
    }

    /* TWS左耳，只出左声道 */
    if (product_form == AUDIO_PRODUCT_FORM_TWS_LEFT) {
        return AO_TRACK_DOUBLE_LEFT;
    }

    /* TWS右耳，只出右声道 */
    if (product_form == AUDIO_PRODUCT_FORM_TWS_RIGHT) {
        return AO_TRACK_DOUBLE_RIGHT;
    }

    /* 默认行为：左右声道混音输出 */
    return AO_TRACK_DOUBLE_MONO;
}

td_s32 core_sys_set_clock_attr(td_handle sys, const audio_clock_attr *attr)
{
    td_s32 ret;
    sys_check_null_ptr_return(attr);

    audio_func_enter();
    ret = set_dsp_freq(attr->clock_freq);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(set_dsp_freq, ret);
        return ret;
    }
    set_dsp_clock_type(attr->clock_type);

    audio_unused(sys);
    audio_func_exit();
    return AUDIO_SUCCESS;
}

td_s32 core_sys_clear_overlay_section(core_overlay_id overlay_id)
{
    td_s32 ret;
    td_u32 section_addr;
    td_u32 section_size;
    audio_func_enter();

    ret = overlay_get_section_attr(overlay_id, &section_addr, &section_size);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(overlay_get_section_attr, ret);
        return ret;
    }

    audio_osal_icache_region_inv((td_void *)section_addr, (td_ulong)section_size);
    audio_osal_dcache_region_inv((td_void *)section_addr, (td_ulong)section_size);

    audio_func_exit();
    return AUDIO_SUCCESS;
}

td_s32 core_sys_set_dsp_log_level(td_u32 log_level)
{
    td_s32 ret;
    audio_func_enter();

#ifdef SAP_IPC_LOG_SUPPORT
    ret = set_dsp_log_level(log_level);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_u32(log_level);
        audio_func_exit();
        return ret;
    }
#else
    audio_unused(log_level);
    audio_unused(ret);
#endif

    audio_func_exit();
    return AUDIO_SUCCESS;
}

td_s32 core_sys_get_audio_scene(td_handle handle, audio_scene_config *audio_scene_cfg)
{
    return platform_get_audio_scene(handle, audio_scene_cfg);
}

td_s32 core_sys_set_audio_scene(td_handle handle, const audio_scene_config *audio_scene_cfg)
{
    return platform_set_audio_scene(handle, audio_scene_cfg);
}

td_s32 core_sys_get_volume(td_handle handle, audio_volume_cfg *vol_cfg)
{
    return platform_get_volume(handle, vol_cfg);
}

td_s32 core_sys_set_volume(td_handle handle, const audio_volume_cfg *vol_cfg)
{
    return platform_set_volume(handle, vol_cfg);
}

static platform_driver g_core_sys_driver = {
    .module_id = SAP_MOD_SYS,
    .init = core_sys_init,
    .deinit = core_sys_deinit,
    .suspend = core_sys_suspend,
    .resume = core_sys_resume,
    .get_mod_handle = core_sys_get_mod_handle,
    .process = TD_NULL,
    .get_input_adp_attr = TD_NULL,
    .get_output_adp_attr = TD_NULL,
    .get_input_pcm_attr = TD_NULL,
    .set_input_pcm_attr = TD_NULL,
};
module_driver(g_core_sys_driver);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
