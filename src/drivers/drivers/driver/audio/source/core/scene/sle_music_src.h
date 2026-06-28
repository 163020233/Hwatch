/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sle music
 * Author: audio
 */

#ifndef SLE_MUSIC_SRC_H
#define SLE_MUSIC_SRC_H

#include "td_type.h"

td_void sle_music_src_process_data(td_void);

td_void sle_music_src_bitrate_reduced(td_void);

td_void sle_music_src_update_conhdl(sap_mod_handle aenc, td_u16 conhdl);

td_s32 sle_music_src_update_bgc_rate_cfg(sap_mod_handle aenc, const td_void *param, td_u32 param_size);

td_void sle_music_src_update_buffer_credit(td_u16 conhdl, td_u8 nb);

td_void sle_music_src_request_lpc_freq(td_u32 sample_rate, td_u32 samples_per_frame);

#endif /* SLE_MUSIC_SRC_H */
