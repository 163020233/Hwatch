/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_display_interface_define.h
 * Description:  provide ux display graphic service header file.
 */
 
#ifndef UX_JACK_H
#define UX_JACK_H

#include "errcode.h"
#include "ux_jack_customer.h"
#include "ux_api_jack.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

errcode_t ux_jack_init(void);

errcode_t ux_jack_deinit(void);

errcode_t ux_jack_set_plug_state(ux_api_jack_state_t state);
ux_api_jack_state_t ux_jack_get_state(void);

 
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
