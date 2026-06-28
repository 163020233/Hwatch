/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_audio_uac.h
 * Description:  provide ux audio uac header file.
 */

#ifndef UX_AUDIO_UAC_H
#define UX_AUDIO_UAC_H
#include "audio.h"
#include "feature_config.h"
#ifdef CONFIG_SUPPORT_USB_DEVICE
#include "usb_drv.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_SUPPORT_USB_DEVICE

void ux_audio_uac_init(void);
void ux_audio_uac_deinit(void);

// 上行
errcode_t ux_audio_uac_uplink_set_param(usb_uac_info_t *uac_param);
errcode_t ux_audio_uac_uplink_start(void);
errcode_t ux_audio_uac_uplink_stop(void);

// 下行
errcode_t ux_audio_uac_downlink_set_param(usb_uac_info_t *uac_param);
errcode_t ux_audio_uac_downlink_start(void);
errcode_t ux_audio_uac_downlink_stop(void);

#endif

#ifdef __cplusplus
}
#endif
#endif