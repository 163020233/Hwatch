/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:  plt chr
 */

#include "platform_core.h"
#include "pm_sleep_porting.h"
#include "massdata_buffer.h"
#include "pmu_sub_control.h"
#include "pm_pmu_porting.h"
#include "securec.h"
#include "plt_chr.h"

#define PLT_LPM_RECORD_MAX 256

void uapi_system_chr_sleep_record(void)
{
    char slp_tmp[PLT_LPM_RECORD_MAX] = {0};
    pm_sleep_record_t *sleep_record = get_pm_sleep_record();
    if (sprintf_s(slp_tmp, PLT_LPM_RECORD_MAX, "cnts:ipc:%u, rtc:%u, gpio:%u, button:%u.\n", sleep_record->ipc_cnts,
        sleep_record->rtc_cnts, sleep_record->m_gpio_cnts, sleep_record->button_cnts) < 0) {
        return;
    }
    plt_chr_log_long(MT_PLT_CHR_EVENT, PLT_CHR_LPM, PLT_CHR_WAKEUP_STATICS, slp_tmp, PLT_LPM_RECORD_MAX);
    memset_s(sleep_record, sizeof(pm_sleep_record_t), 0, sizeof(pm_sleep_record_t));
}

void uapi_system_chr_pmu_record(void)
{
    char pmu_tmp[PLT_LPM_RECORD_MAX] = {0};
    pmu_sub_info_t *pmu_record = get_pmu_sub_info();
    if (sprintf_s(pmu_tmp, PLT_LPM_RECORD_MAX, "poweron:dsp:%u, display:%u, vau:%u, h264:%u.\n",
        pmu_record[PMU_AUDIO_SUB].poweron_cnt, pmu_record[PMU_DISPLAY_SUB].poweron_cnt,
        pmu_record[PMU_VAU_SUB].poweron_cnt, pmu_record[PMU_H264_SUB].poweron_cnt) < 0) {
        return;
    }
    if (sprintf_s(pmu_tmp + strlen(pmu_tmp), PLT_LPM_RECORD_MAX, "poweroff:dsp:%u, display:%u, vau:%u, h264:%u.\n",
        pmu_record[PMU_AUDIO_SUB].poweroff_cnt, pmu_record[PMU_DISPLAY_SUB].poweroff_cnt,
        pmu_record[PMU_VAU_SUB].poweroff_cnt, pmu_record[PMU_H264_SUB].poweroff_cnt) < 0) {
        return;
    }
    if (sprintf_s(pmu_tmp + strlen(pmu_tmp), PLT_LPM_RECORD_MAX, "dsp runtime:%u.\n",
        pmu_record[PMU_AUDIO_SUB].runtime) < 0) {
        return;
    }
    plt_chr_log_long(MT_PLT_CHR_EVENT, PLT_CHR_PMU, PLT_CHR_POWER_STATE_STATICS, pmu_tmp, PLT_LPM_RECORD_MAX);
}

void system_chr_statistics_process(void)
{
    uapi_system_chr_sleep_record();
    uapi_system_chr_pmu_record();
}