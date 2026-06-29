/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: message for capturing data between core a and the dsp.
 * Author: audio
 */

#ifndef __CORE_HOOK_DATA_H__
#define __CORE_HOOK_DATA_H__

typedef enum {
    HOOK_AUDIO_DATA_START,
    HOOK_AUDIO_DATA_STOP,
} hook_audio_data_status;

typedef struct {
    td_u32 cnt; /* 数据帧计数 */
    td_u32 size;
    td_u32 node_id;
} audio_hook_header;

typedef struct {
    audio_hook_header hdr;
    td_s32 data[];
} audio_hook_frame;

typedef struct audio_hook_node {
    hook_audio_data_status status;
    td_u32 id;
} audio_hook_node;

#ifdef SAP_HOOK_DATA_SUPPORT
td_void hook_audio_data(audio_hook_data_node id, const td_s32 *data, td_u32 size);
td_bool hook_audio_data_get_enable(audio_hook_data_node id);
#else
#define hook_audio_data(id, data, size)
#endif

#endif /* __CORE_HOOK_DATA_H__ */
