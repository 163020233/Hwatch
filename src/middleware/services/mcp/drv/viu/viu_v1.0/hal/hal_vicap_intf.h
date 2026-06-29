/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: head file of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-04-23
 */

#ifndef HAL_VICAP_INTF_H
#define HAL_VICAP_INTF_H

#include "drv_mcp_dependency.h"
#include "viu_base_define.h"
#include "drv_isp_video_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

typedef struct {
    ext_isp_size valid_size;
    ext_isp_size total_size;
} hal_vicap_status;

td_s32 hal_vicap_init(viu_cap_id cap_id);
td_void hal_vicap_deinit(viu_cap_id cap_id);

td_void hal_vicap_work_mode(viu_cap_id cap_id, td_bool online);

td_void hal_vicap_create(viu_cap_id cap_id, ext_isp_rect *crop_rect);
td_void hal_vicap_destroy(viu_cap_id pipe_id);

td_void hal_vicap_start(viu_cap_id cap_id, ext_isp_frame_interrupt_type int_type, td_bool offline);
td_void hal_vicap_stop(viu_cap_id pipe_id);
td_void hal_vicap_disable(viu_cap_id pipe_id);

td_void hal_vicap_get_online_status(viu_cap_id pipe_id, td_u32 *value);
td_void hal_vicap_regnew(viu_cap_id cap_id);

td_u32 hal_vicap_get_int_status(td_void);
td_bool hal_vicap_get_int_report(viu_cap_id cap_id, td_u32 cap_int_status);
td_u32 hal_vicap_get_ch_int_status(viu_cap_id cap_id);
td_void hal_vicap_clear_ch_int(viu_cap_id cap_id, td_u32 wch_int);

td_void hal_vicap_set_isp_sel(viu_cap_id cap_id, td_u32 isp_sel);
td_void hal_vicap_set_wch_cmp_mode(viu_cap_id cap_id, td_u32 bayer_mode);

td_void hal_vicap_process(viu_cap_id cap_id, td_u32 skip_en, td_u32 buf_ok, ext_isp_crop_info *crop_info,
    ext_isp_video_frame_info *frame);
td_void hal_vicap_wdr_process(viu_cap_id cap_id, td_u32 skip_en, td_u32 buf_ok, ext_isp_video_frame_info *frame,
    td_u32 wdr_idx);

td_void hal_vicap_set_wrap(viu_cap_id cap_id, td_bool enable, td_u32 cache_line);
td_void hal_vicap_get_status(viu_cap_id cap_id, hal_vicap_status *status);
td_void hal_vicap_set_isp_mode(viu_cap_id cap_id, td_bool isp_bypass, td_bool isp_in_yuv);
td_void hal_vicap_set_isp_id_sel(viu_cap_id cap_id, td_u32 isp_id_sel);

td_u64 hal_vicap_reg_get_timestamp(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

