/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu module header
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef __VIU_WDR__
#define __VIU_WDR__

#include "viu.h"

td_void viu_wdr_online_update_cfg(td_s32 pipe_id, td_s32 mast_pipe);

td_void viu_wdr_offline_clear_pic_node(td_s32 mast_pipe, viu_wdr_pic_node *send_this_wdr_pic);

td_s32 viu_wdr_offline_sync_frame(td_s32 pipe_id, td_s32 mast_pipe);

td_u64 viu_wdr_get_frame_interval(td_s32 pipe_id);

#endif /* __VIU_WDR__ */

