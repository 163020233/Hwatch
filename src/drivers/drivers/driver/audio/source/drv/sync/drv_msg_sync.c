/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: source file for drv audio sync
 * Author: audio
 */

#include "audio_struct.h"
#include "audio_debug.h"
#include "core_msg_struct.h"
#include "core_msg.h"
#include "drv_msg_sync.h"

#ifdef __cplusplus
extern "C" {
#endif

#define drv_sync_send_msg(msg)  core_msg_send(AUDIO_CORE_DSP0, MSG_CORE_SYNC_MODULE, msg, MSG_SYNC_WAIT_TIME_MS)

td_s32 drv_msg_sync_get_proc_info(td_handle sync, sync_proc_item *proc)
{
    core_msg_data msg_data;

    msg_data.cmd = MSG_CORE_SYNC_GET_PROC_INFO;
    msg_data.handle = sync;
    msg_data.input_data = TD_NULL;
    msg_data.input_data_size = 0;
    msg_data.output_data = proc;
    msg_data.output_data_size = (td_u32)sizeof(sync_proc_item);

    return drv_sync_send_msg(&msg_data);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
