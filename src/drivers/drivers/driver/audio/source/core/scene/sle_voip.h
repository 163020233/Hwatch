/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: sle_voip
 * Author: audio
 */

#ifndef SLE_VOIP_H
#define SLE_VOIP_H

#include "audio_struct.h"

td_void sle_voip_set_packet_length(td_u32 packet_length);
td_void sle_voip_dl_send_stream(const audio_stream_buf *stream);
td_s32 sle_voip_ul_send_stream(td_void);
td_void sle_voip_process_ul_data(td_void);

#endif /* SLE_VOIP_H */
