/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ipc driver component api
 * Author: audio
 */

#ifndef __IPC_COMP_H__
#define __IPC_COMP_H__

#include "audio_type.h"
#include "ipc.h"
#include "ipc_protocol.h"

/* ipc rx */
/* IPC_ACTION_DATA */
td_void ipc_msg_data_entry_init(td_void);
/* IPC_ACTION_DSP_CURRENT_FREQ */
td_void ipc_msg_set_dsp_freq_entry_init(td_void);
/* IPC_ACTION_AUDIO_HOOK_DATA */
td_void ipc_msg_audio_hook_data_entry_init(td_void);
/* IPC_ACTION_SET_LOG_LEVEL */
td_void ipc_msg_set_dsp_log_level_entry_init(td_void);

/* ipc tx */
ipc_msg_t *ipc_msg_to_bth_init_head(td_u32 *msg_buf, td_u32 msg_buf_length, td_u32 msg_length);
ipc_msg_t *ipc_msg_to_btc_init_head(td_u32 *msg_buf, td_u32 msg_buf_length, td_u32 msg_length);
ipc_msg_t *ipc_msg_to_app_init_head(td_u32 *msg_buf, td_u32 msg_buf_length, td_u32 msg_length);

/* IPC_ACTION_DATA */
td_s32 ipc_msg_send_data_to_bth(const ipc_msg_t *ipc_msg);
/* IPC_ACTION_DATA */
td_s32 ipc_msg_send_data_to_btc(const ipc_msg_t *ipc_msg);
/* IPC_ACTION_DATA */
td_s32 ipc_msg_send_data_to_app(const ipc_msg_t *ipc_msg);

/* IPC_ACTION_AUDIO_HOOK_DATA */
td_s32 ipc_msg_send_audio_hook_data_to_app(const uint8_t *data, uint32_t data_length);

/* IPC_ACTION_LOG_INFO */
td_void ipc_msg_send_audio_log_trigger_to_app(td_void);

#endif /* __IPC_COMP_H__ */
