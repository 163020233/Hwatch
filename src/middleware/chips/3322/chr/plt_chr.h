/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description:  plt chr
 */

#ifndef PLT_CHR_H
#define PLT_CHR_H

#ifdef PLT_SUPPORT_CHR
#define plt_chr_log(event_id, info1, info2, info3) \
    uapi_massdata_record_system_event((uint8_t)(event_id), (uint8_t)(info1), \
                                      (uint8_t)(info2), (uint32_t)(info3))
#define plt_chr_log_long(event_id, info1, info2, info3, info3_len) \
    uapi_massdata_record_system_long_event((uint8_t)(event_id), (uint8_t)(info1), (uint8_t)(info2), \
                                           (uint8_t *)(info3), (uint8_t)(info3_len))
#else
#define plt_chr_log(event_id, info1, info2, info3)
#define plt_chr_log_long(event_id, info1, info2, info3, info3_len)
#endif

typedef enum {
    PLT_CHR_OS = 0x1,
    PLT_CHR_LPM,
    PLT_CHR_PMU,
    PLT_CHR_INPUT,
    PLT_CHR_USB,
    PLT_CHR_FS,
    PLT_CHR_INFO1_MAX = 0xFF
}plt_chr_info1;

typedef enum {
    PLT_CHR_OS_START = 0x1,

    PLT_CHR_LPM_START = 0x1,
    PLT_CHR_WAKEUP_STATICS = PLT_CHR_LPM_START,

    PLT_CHR_PMU_START = 0x1,
    PLT_CHR_POWER_STATE_STATICS = PLT_CHR_PMU_START,

    PLT_CHR_INFO2_MAX = 0xFF
} plt_chr_info2;

void system_chr_statistics_process(void);

#endif