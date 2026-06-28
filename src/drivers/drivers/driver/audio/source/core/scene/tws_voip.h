/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: tws_voip
 * Author: audio
 */

#ifndef TWS_VOIP_H
#define TWS_VOIP_H

#include "td_type.h"
#include "audio_struct.h"

enum {
    TWS_VOIP_TOGGLE_INTERVAL_HALF_PACKET = 3750,
    TWS_VOIP_TOGGLE_INTERVAL_WHOLE_PACKET = 7500,
};

td_void tws_voip_set_packet_length(td_u32 packet_length);

td_void tws_voip_dl_send_stream(const audio_stream_buf *stream);

td_void tws_voip_dl_enable_packet_parser(td_bool enable);

#endif /* TWS_VOIP_H */
