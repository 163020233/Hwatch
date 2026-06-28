/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_bs_sle_grant.h
 * Description:  provide ux bg sle header file.
 */
#ifndef UX_BS_SLE_GRANT_H
#define UX_BS_SLE_GRANT_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "ux_bs_sle_wrap.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

ux_bs_sle_wrap_funcs *ux_bg_sle_get_grant_opts(void);

errcode_t ux_bs_sle_grant_set_default_volume(int16_t host_curr_volume);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UX_BS_SLE_GRANT_H */