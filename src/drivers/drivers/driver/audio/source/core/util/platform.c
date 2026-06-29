/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: hifi platform device and platform driver
 * Author: audio
 */

#include "platform.h"
#include "audio_errno.h"
#include "audio_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

static OSAL_LIST_HEAD(g_platform_driver_list);

td_void platform_driver_register(platform_driver *driver)
{
    if (driver == TD_NULL) {
        return;
    }

    osal_list_add_tail(&driver->node, &g_platform_driver_list);

    if (driver->init != TD_NULL) {
        driver->init();
    }
}

td_void platform_driver_call_deinit(td_void)
{
    platform_driver *driver = TD_NULL;

    osal_list_for_each_entry(driver, &g_platform_driver_list, node) {
        if (driver->deinit == TD_NULL) {
            continue;
        }

        driver->deinit();
    }

    osal_list_del_init(&g_platform_driver_list);
}

td_void platform_driver_call_suspend(td_void)
{
    platform_driver *driver = TD_NULL;

    osal_list_for_each_entry(driver, &g_platform_driver_list, node) {
        if (driver->suspend == TD_NULL) {
            continue;
        }

        driver->suspend();
    }
}

td_void platform_driver_call_resume(td_void)
{
    platform_driver *driver = TD_NULL;

    osal_list_for_each_entry(driver, &g_platform_driver_list, node) {
        if (driver->resume == TD_NULL) {
            continue;
        }

        driver->resume();
    }
}

td_void platform_driver_call_process(td_void)
{
    platform_driver *driver = TD_NULL;

    osal_list_for_each_entry(driver, &g_platform_driver_list, node) {
        if (driver->process == TD_NULL) {
            continue;
        }

        driver->process();
    }
}

static platform_driver *get_platform_driver(td_handle handle)
{
    platform_driver *driver = TD_NULL;
    td_u32 mod_type = get_sap_mod_type(handle);

    osal_list_for_each_entry(driver, &g_platform_driver_list, node) {
        if (driver->module_id != mod_type) {
            continue;
        }

        return driver;
    }

    sap_err_log_u32(mod_type);
    return TD_NULL;
}

sap_mod_handle platform_get_module_handle(td_handle handle)
{
    platform_driver *driver = get_platform_driver(handle);
    if (driver == TD_NULL || driver->get_mod_handle == TD_NULL) {
        return TD_NULL;
    }

    return driver->get_mod_handle(handle);
}

td_s32 platform_get_input_adp_attr(td_handle handle, audio_adp_attr *adp_attr)
{
    platform_driver *driver = get_platform_driver(handle);
    if (driver == TD_NULL) {
        return ERR_SYS_INVALID_HANDLE;
    }

    if (driver->get_input_adp_attr == TD_NULL) {
        sap_err_log_u32(get_sap_mod_type(handle));
        return ERR_SYS_NOT_SUPPORTED;
    }

    return driver->get_input_adp_attr(handle, adp_attr);
}

td_s32 platform_get_output_adp_attr(td_handle handle, td_u32 data_port_id, audio_adp_attr *adp_attr)
{
    platform_driver *driver = get_platform_driver(handle);
    if (driver == TD_NULL) {
        return ERR_SYS_INVALID_HANDLE;
    }

    if (driver->get_output_adp_attr == TD_NULL) {
        sap_err_log_u32(get_sap_mod_type(handle));
        return ERR_SYS_NOT_SUPPORTED;
    }

    return driver->get_output_adp_attr(handle, data_port_id, adp_attr);
}

td_s32 platform_set_input_pcm_attr(td_handle handle, const audio_pcm_format *in_pcm,
    const audio_pcm_format *ref_pcm)
{
    platform_driver *driver = get_platform_driver(handle);
    if (driver == TD_NULL) {
        return ERR_SYS_NOT_SUPPORTED;
    }

    if (driver->set_input_pcm_attr == TD_NULL) {
        sap_err_log_u32(get_sap_mod_type(handle));
        return ERR_SYS_NOT_SUPPORTED;
    }

    return driver->set_input_pcm_attr(handle, in_pcm, ref_pcm);
}

td_s32 platform_get_input_pcm_attr(td_handle handle, audio_pcm_format *in_pcm,
    audio_pcm_format *ref_pcm)
{
    platform_driver *driver = get_platform_driver(handle);
    if (driver == TD_NULL) {
        return ERR_SYS_NOT_SUPPORTED;
    }

    if (driver->get_input_pcm_attr == TD_NULL) {
        sap_err_log_u32(get_sap_mod_type(handle));
        return ERR_SYS_NOT_SUPPORTED;
    }

    return driver->get_input_pcm_attr(handle, in_pcm, ref_pcm);
}

td_s32 platform_get_audio_scene(td_handle handle, audio_scene_config *audio_scene_cfg)
{
    platform_driver *driver = get_platform_driver(handle);
    if (driver == TD_NULL) {
        return ERR_SYS_NOT_SUPPORTED;
    }

    if (driver->get_audio_scene == TD_NULL) {
        sap_err_log_u32(get_sap_mod_type(handle));
        return ERR_SYS_NOT_SUPPORTED;
    }

    return driver->get_audio_scene(handle, audio_scene_cfg);
}

td_s32 platform_set_audio_scene(td_handle handle, const audio_scene_config *audio_scene_cfg)
{
    platform_driver *driver = get_platform_driver(handle);
    if (driver == TD_NULL) {
        return ERR_SYS_NOT_SUPPORTED;
    }

    if (driver->set_audio_scene == TD_NULL) {
        sap_err_log_u32(get_sap_mod_type(handle));
        return ERR_SYS_NOT_SUPPORTED;
    }

    return driver->set_audio_scene(handle, audio_scene_cfg);
}

td_s32 platform_get_volume(td_handle handle, audio_volume_cfg *vol_cfg)
{
    platform_driver *driver = get_platform_driver(handle);
    if (driver == TD_NULL) {
        return ERR_SYS_NOT_SUPPORTED;
    }

    if (driver->get_volume == TD_NULL) {
        sap_err_log_u32(get_sap_mod_type(handle));
        return ERR_SYS_NOT_SUPPORTED;
    }

    return driver->get_volume(handle, vol_cfg);
}

td_s32 platform_set_volume(td_handle handle, const audio_volume_cfg *vol_cfg)
{
    platform_driver *driver = get_platform_driver(handle);
    if (driver == TD_NULL) {
        return ERR_SYS_NOT_SUPPORTED;
    }

    if (driver->set_volume == TD_NULL) {
        sap_err_log_u32(get_sap_mod_type(handle));
        return ERR_SYS_NOT_SUPPORTED;
    }

    return driver->set_volume(handle, vol_cfg);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
