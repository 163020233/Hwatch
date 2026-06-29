/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
 * Description: dsp audio scene driver
 * Author: audio
 */

#ifndef __SCENE_DRIVER_H__
#define __SCENE_DRIVER_H__

#include "osal_list.h"
#include "adp_struct.h"
#include "audio_struct.h"
#include "core_module.h"

typedef struct {
    struct osal_list_head node;
    td_bool (*compatibility)(audio_scene scene);

    td_s32 (*register_adp)(td_handle adp, audio_adp_type adp_type);
    td_void (*unregister_adp)(td_handle adp, audio_adp_type adp_type);

    td_void (*register_module)(sap_mod_handle module);
    td_void (*unregister_module)(sap_mod_handle module);
} audio_scene_driver;

td_void audio_scene_driver_register(audio_scene_driver *driver);

td_s32 audio_scene_register_adp(td_handle adp, audio_adp_type adp_type, audio_scene scene);
td_void audio_scene_unregister_adp(td_handle adp, audio_adp_type adp_type, audio_scene scene);

td_void audio_scene_register_module(sap_mod_handle module, audio_scene scene);
td_void audio_scene_unregister_module(sap_mod_handle module, audio_scene scene);

#endif /* __SCENE_DRIVER_H__ */
