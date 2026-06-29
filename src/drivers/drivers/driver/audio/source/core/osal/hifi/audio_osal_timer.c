/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: API of audio osal timer
 * Author: audio
 */
#include <sys/times.h>
#include <xtensa/hal.h>
#include "hal_systick.h"
#include "mem_addr.h"
#include "audio_osal.h"
#include "set_freq.h"

#define CONVERT_LEVEL 1000000

static td_void dsp_mdelay(td_u32 msecs)
{
    td_ulong old_cnt, new_cnt;
    td_u32 delay_cnt = msecs * (get_dsp_freq() / 1000); /* 1000 is num */
    old_cnt = xthal_get_ccount();
    do {
        new_cnt = xthal_get_ccount();
    } while ((new_cnt - old_cnt) < delay_cnt);
}

static td_void dsp_udelay(td_u32 usecs)
{
    td_ulong old_cnt, new_cnt;
    td_u32 delay_cnt = usecs * (get_dsp_freq() / 1000000); /* 1000000 is num */
    old_cnt = xthal_get_ccount();
    do {
        new_cnt = xthal_get_ccount();
    } while ((new_cnt - old_cnt) < delay_cnt);
}

td_void audio_osal_mdelay(td_u32 ms)
{
    dsp_mdelay(ms);
}

td_void audio_osal_udelay(td_u32 us)
{
    dsp_udelay(us);
}

td_u32 audio_osal_gettime(td_void)
{
    return xthal_get_ccount();
}

td_u32 audio_osal_gettime_us(td_void)
{
    struct tms ustimes;
    td_u32 time_us;

    times(&ustimes);
    time_us = (td_u32)(ustimes.tms_utime / (get_dsp_freq() / CONVERT_LEVEL));

    return time_us;
}

td_u32 audio_osal_cali_time_diff_us(td_u32 begin_us, td_u32 end_us)
{
    /* begin_us和end_us不能跨越1个回绕周期，否则计算不准确，仅适合短时间耗时的计算 */
    if (end_us < begin_us) {
        /* 时间戳回绕处理，单位为us */
        td_u32 new_end = end_us;
        new_end += 0xFFFFFFFF / (get_dsp_freq() / CONVERT_LEVEL);
        return (new_end - begin_us);
    } else {
        return (end_us - begin_us);
    }
}

td_u64 audio_osal_get_systick_ms(td_void)
{
    return hal_systick_get_ms();
}
