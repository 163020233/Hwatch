/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu module header
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef __VIU_SNAP__
#define __VIU_SNAP__

#include "drv_viu_ext.h"
#include "drv_viu_struct.h"

td_s32 viu_snap_set_pipe_attr(td_s32 pipe_id, const ext_snap_attr *snap_attr);
td_s32 viu_snap_get_pipe_attr(td_s32 pipe_id, ext_snap_attr *snap_attr);

td_s32 viu_snap_enable_pipe(td_s32 pipe_id);
td_s32 viu_snap_disable_pipe(td_s32 pipe_id);

td_s32 viu_snap_set_pro_sharpen_param(td_s32 pipe_id, const ext_isp_pro_sharpen_param *isp_shp_param);
td_s32 viu_snap_get_pro_sharpen_param(td_s32 pipe_id, ext_isp_pro_sharpen_param *isp_shp_param);

td_s32 viu_snap_set_pro_bnr_param(td_s32 pipe_id, const ext_isp_pro_nr_param *nr_parma);
td_s32 viu_snap_get_pro_bnr_param(td_s32 pipe_id, ext_isp_pro_nr_param *nr_parma);

td_s32 viu_snap_trigger_pipe(td_s32 pipe_id);

td_void viu_snap_get_isp_info(td_s32 pipe_id, ext_isp_video_frame_info *video_frame);
td_s32 viu_snap_get_pipe_status(td_s32 pipe_id, viu_snap_status *snap_status);

td_void viu_snap_update_pipe_attr(td_s32 pipe_id);

#endif /* __VIU_SNAP__ */

