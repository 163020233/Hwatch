/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: watch music
 * Author: audio
 */

#ifndef WATCH_MUSIC_H
#define WATCH_MUSIC_H

#include "td_type.h"
#include "core_module.h"

td_void watch_music_src_inst_init(audio_pcm_format *pcm_attr);
td_void watch_music_set_packet_size(td_u32 toggle_len, td_u16 packet_data_length);
td_void watch_music_update_data_size_info(td_u32 input_size, td_u32 output_size);
td_void watch_music_send_data(td_void);
td_void watch_music_process_data(sap_mod_handle aenc);
#ifdef SAP_BT_MUSIC_FLOW_CTRL_SUPPORT
td_void watch_music_set_flow_ctrl(td_u32 flow_ctrl_en);
#endif /* SAP_BT_MUSIC_FLOW_CTRL_SUPPORT */

#endif /* WATCH_MUSIC_H */
