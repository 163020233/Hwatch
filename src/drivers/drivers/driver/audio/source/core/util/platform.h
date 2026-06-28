/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: hifi platform device and platform driver
 * Author: audio
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "audio_struct.h"

#include "osal_list.h"
#include "module.h"
#include "core_module.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    struct osal_list_head node;
    td_u32 module_id;
    td_s32 (*init)(td_void);
    td_s32 (*deinit)(td_void);
    td_s32 (*suspend)(td_void);
    td_s32 (*resume)(td_void);
    td_void (*process)(td_void);

    sap_mod_handle (*get_mod_handle)(td_handle handle);

    td_s32 (*get_input_adp_attr)(td_handle handle, audio_adp_attr *adp_attr);
    td_s32 (*get_output_adp_attr)(td_handle handle, td_u32 data_source, audio_adp_attr *adp_attr);

    td_s32 (*get_input_pcm_attr)(td_handle handle, audio_pcm_format *in_pcm,
        audio_pcm_format *ref_pcm);
    td_s32 (*set_input_pcm_attr)(td_handle handle, const audio_pcm_format *in_pcm,
        const audio_pcm_format *ref_pcm);

    td_s32 (*get_audio_scene)(td_handle handle, audio_scene_config *audio_scene_cfg);
    td_s32 (*set_audio_scene)(td_handle handle, const audio_scene_config *audio_scene_cfg);
    td_s32 (*get_volume)(td_handle handle, audio_volume_cfg *vol);
    td_s32 (*set_volume)(td_handle handle, const audio_volume_cfg *vol);
} platform_driver;

#define module_driver(__driver)                \
    static td_void __driver##_init(td_void)    \
    {                                          \
        platform_driver_register(&(__driver)); \
    }                                          \
    module_initcall(__driver##_init);

td_void platform_driver_register(platform_driver *driver);
td_void platform_driver_call_deinit(td_void);
td_void platform_driver_call_suspend(td_void);
td_void platform_driver_call_resume(td_void);
sap_mod_handle platform_get_module_handle(td_handle handle);
td_void platform_driver_call_process(td_void);

td_s32 platform_get_input_adp_attr(td_handle handle, audio_adp_attr *adp_attr);
td_s32 platform_get_output_adp_attr(td_handle handle, td_u32 data_port_id, audio_adp_attr *adp_attr);

td_s32 platform_set_input_pcm_attr(td_handle handle, const audio_pcm_format *in_pcm,
    const audio_pcm_format *ref_pcm);
td_s32 platform_get_input_pcm_attr(td_handle handle, audio_pcm_format *in_pcm,
    audio_pcm_format *ref_pcm);

td_s32 platform_get_audio_scene(td_handle handle, audio_scene_config *audio_scene_cfg);
td_s32 platform_set_audio_scene(td_handle handle, const audio_scene_config *audio_scene_cfg);

td_s32 platform_get_volume(td_handle handle, audio_volume_cfg *vol);
td_s32 platform_set_volume(td_handle handle, const audio_volume_cfg *vol);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
