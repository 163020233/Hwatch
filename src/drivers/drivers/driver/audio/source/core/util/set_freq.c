/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dsp frequency get/set implement.
 * Author: audio
 */

#include "securec.h"
#include "audio_debug.h"
#include "core_interface.h"
#include "core_sys.h"
#include "serial_dw.h"
#include "set_freq.h"

#ifndef SAP_IPC_LOG_SUPPORT
#include "serial_dw.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static td_u32 g_dsp_cur_freq = SAP_DSP_CLK_HZ;
static audio_clock_type g_dsp_cur_clock_src = AUDIO_CLOCK_MAX;
static const td_u32 g_dsp_freq_min = AUDIO_DSP_FREQ_MIN;

static inline td_bool dsp_freq_verify(td_u32 set_freq)
{
    return ((set_freq < g_dsp_freq_min) ? TD_FALSE : TD_TRUE);
}

td_u32 get_dsp_freq(td_void)
{
    return g_dsp_cur_freq;
}

td_s32 set_dsp_freq(td_u32 freq)
{
    td_s32 ret;

    sap_warn_log_u32(freq);
    /* verify dsp frequency get from app core. */
    if (dsp_freq_verify(freq) != TD_TRUE) {
        sap_err_log_u32(freq);
        return ERR_SYS_NOT_SUPPORTED;
    }

    /* set frequency if local value inconsistent */
    if (freq != g_dsp_cur_freq) {
        g_dsp_cur_freq = freq;
        /* restart timer by new dsp freq */
        ret = core_sys_set_total_cps(0, freq);
        if (ret != AUDIO_SUCCESS) {
            sap_err_log_fun(core_sys_set_total_cps, ret);
            return ret;
        }
        /* config clk div param for uart by new dsp freq */
#ifndef SAP_IPC_LOG_SUPPORT
#if (AUDIO_LOG_SWITCH_X != 0)
        (td_void)serial_init();
#endif
#endif
    }

    return AUDIO_SUCCESS;
}

td_void set_dsp_clock_type(audio_clock_type type)
{
    g_dsp_cur_clock_src = type;
}

audio_clock_type get_dsp_clock_type(td_void)
{
    return g_dsp_cur_clock_src;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
