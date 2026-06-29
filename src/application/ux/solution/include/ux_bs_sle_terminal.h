/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_bs_sle_terminal.h
 * Description:  provide ux bg sle header file.
 */
#ifndef UX_BS_SLE_TERMINAL_H
#define UX_BS_SLE_TERMINAL_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "ux_bs_sle_wrap.h"
#include "ux_api_tws.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_bs_sle_term_set_pdl_config(void);
ux_bs_sle_wrap_funcs *ux_bg_sle_get_term_opts(void);
errcode_t ux_bs_sle_set_audio_channel(ux_api_device_audio_channel_t audio_channel);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UX_BS_SLE_TERMINAL_H */
