/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_device_property.h
 * Description:  provide ux device property header file.
 */
#ifndef UX_DEVICE_PROPERTY_H
#define UX_DEVICE_PROPERTY_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "ux_errcode.h"
#include "ux_types.h"
#include "ux_api_device.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_device_set_channel(const ux_api_device_audio_channel_t channel);
ux_api_device_audio_channel_t ux_device_get_channel(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // end UX_DEVICE_PROPERTY_H