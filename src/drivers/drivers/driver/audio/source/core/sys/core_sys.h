/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: header file for audio system
 * Author: audio
 */

#ifndef __CORE_SYS_H__
#define __CORE_SYS_H__

#include "sys_struct.h"
#include "audio_struct.h"
#include "ao_struct.h"
#include "core_overlay_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_bool core_sys_check_idle(td_void);

td_s32 core_sys_set_total_cps(td_handle sys, td_u32 total_cps);

td_s32 core_sys_set_idle_timeout(td_handle sys, td_u32 timeout);

td_s32 core_sys_register_event_proc(audio_event_proc event_proc, td_void *context);

td_s32 core_sys_show_profile(td_handle sys, td_bool flag);

td_s32 core_sys_get_proc_info(sys_proc_item *proc);

td_bool core_sys_check_tws_master_ear(td_void);

td_bool core_sys_check_tws_slave_ear(td_void);

td_bool core_sys_check_tws_binaural_sync_enable(td_void);

td_bool core_sys_check_tws_dual_transmit(td_void);

ao_track_mode core_sys_get_tws_channel_mode(td_void);

td_s32 core_sys_set_clock_attr(td_handle sys, const audio_clock_attr *attr);

td_s32 core_sys_clear_overlay_section(core_overlay_id overlay_id);

td_s32 core_sys_set_dsp_log_level(td_u32 log_level);

td_s32 core_sys_get_audio_scene(td_handle handle, audio_scene_config *audio_scene_cfg);

td_s32 core_sys_set_audio_scene(td_handle handle, const audio_scene_config *audio_scene_cfg);

td_s32 core_sys_get_volume(td_handle handle, audio_volume_cfg *vol);

td_s32 core_sys_set_volume(td_handle handle, const audio_volume_cfg *vol);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CORE_SYS_H__ */
