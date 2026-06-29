/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: head file for mpi audio sys
 * Author: audio
 */

#ifndef __MPI_SYS_H__
#define __MPI_SYS_H__

#include "audio_struct.h"
#include "ao_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

td_s32 mpi_audio_init(td_void);

td_s32 mpi_audio_deinit(td_void);

td_s32 mpi_audio_set_debug_cfg(const audio_debug_cfg *cfg);

td_s32 mpi_audio_get_debug_cfg(audio_debug_cfg *cfg);

td_s32 mpi_audio_set_product_form(audio_product_form product_form);

td_s32 mpi_audio_set_tws_mono_mode(td_bool mono);

td_s32 mpi_audio_set_tws_mode(audio_tws_mode tws_mode);

td_s32 mpi_audio_set_tws_role(audio_tws_role tws_role);

td_s32 mpi_audio_set_dsp_log_level(td_u32 log_level);

td_s32 mpi_audio_get_audio_scene(td_handle handle, audio_scene_config *audio_scene_cfg);

td_s32 mpi_audio_set_audio_scene(td_handle handle, const audio_scene_config *audio_scene_cfg);

td_s32 mpi_audio_get_volume(td_handle handle, audio_volume_cfg *vol);

td_s32 mpi_audio_set_volume(td_handle handle, const audio_volume_cfg *vol);

td_s32 mpi_audio_set_dsp_file_path(const td_char *file_path, td_u32 len);

td_s32 mpi_audio_set_mic_mode(audio_mic_mode mic_mode);

td_s32 mpi_audio_set_mic_mix_mode(audio_mic_mix_mode mic_mix_mode);

td_s32 mpi_audio_get_global_mute(td_bool *mute);

td_s32 mpi_audio_set_global_mute(td_bool mute);

td_s32 mpi_audio_set_volume_matrix(ao_port_id port, const audio_volume_matrix *volume_matrix);

td_s32 mpi_audio_get_volume_matrix(ao_port_id port, audio_volume_matrix *volume_matrix);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_SYS_H__ */
