/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: header file for drv sys msg
 * Author: audio
 */

#ifndef __DRV_MSG_SYS_H__
#define __DRV_MSG_SYS_H__

#include "audio_msg.h"
#include "audio_struct.h"
#include "sys_struct.h"
#include "ao_struct.h"
#include "core_overlay_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 drv_msg_sys_init(td_void);

td_s32 drv_msg_sys_deinit(td_void);

td_s32 drv_msg_sys_set_total_cps(td_handle sys, td_u32 total_cps);

td_s32 drv_msg_sys_set_idle_timeout(td_handle sys, td_u32 timeout);

td_s32 drv_msg_sys_get_proc_info(td_handle sys, sys_proc_item *proc);

td_s32 drv_msg_sys_set_clock_attr(td_handle sys, const audio_clock_attr *attr);

td_s32 drv_msg_sys_clear_overlay_section(core_overlay_id overlay_id);

td_s32 drv_msg_sys_set_dsp_log_level(td_u32 log_level);

td_s32 drv_msg_sys_get_audio_scene(td_handle handle, audio_scene_config *audio_scene_cfg);

td_s32 drv_msg_sys_set_audio_scene(td_handle handle, const audio_scene_config *audio_scene_cfg);

td_s32 drv_msg_sys_get_volume(td_handle handle, audio_volume_cfg *vol);

td_s32 drv_msg_sys_set_volume(td_handle handle, const audio_volume_cfg *vol);

td_s32 drv_msg_sys_get_global_mute(td_bool *mute);

td_s32 drv_msg_sys_set_global_mute(td_bool mute);

td_s32 drv_msg_sys_get_volume_matrix(ao_port_id port, audio_volume_matrix *volume_matrix);

td_s32 drv_msg_sys_set_volume_matrix(ao_port_id port, const audio_volume_matrix *volume_matrix);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_MSG_SYS_H__ */
