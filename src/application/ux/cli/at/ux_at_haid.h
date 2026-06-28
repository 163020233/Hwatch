/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux bt at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-13， Create file. \n
 */
#ifndef UX_AT_HAID_H
#define UX_AT_HAID_H
#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HAID_VOLUME                         "volume"
#define HAID_MODE                           "mode"
#define HAID_WORK_TYPE                      "worktype"
#define HAID_MEDICAL                        "medical"
#define HAID_MIX_VOL_RANGE                  "mix_vol_range"
#define HAID_ENABLE                         "enable"

#define HAID_DOING_TYPE                     "doing"
#define HAID_FITTING_TYPE                   "fitting"
#define HAID_MEDICAL_TYPE                   "medical"
#define HAID_AFC_TYPE                       "afc"

at_ret_t at_haid_set_process(const ux_at_para2_args_t *args);
at_ret_t at_haid_read_process(void);
at_ret_t at_haid_query_process(const ux_at_para2_args_t *args);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
