/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: define for external use
 * Author: AuthorNameMagicTag
 * Create: 2021-09-22
 */

#ifndef __DRV_VIU_EXT_H__
#define __DRV_VIU_EXT_H__

#include "drv_mcp_dependency.h"
#include "drv_viu_struct.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_s32 (*pfn_viu_get_flash_status)(td_void);
} viu_flash_callback;

typedef struct {
    td_bool discard_frame;
    td_u32 frame_flag;
} viu_snap_status;

typedef struct {
    ext_viu_operating_mode_type mode;
    td_u32 block_num;
    td_u32 over_lap;
    ext_isp_rect rect[VIU_MAX_SPLIT_NODE_NUM];
    ext_isp_rect wch_out_rect;
    ext_isp_pixel_format pix_fmt;
} viu_pipe_split_attr;

typedef td_s32 fn_viu_register_flash_call_back(td_s32 viu_dev_id, viu_flash_callback *flash_cb);
typedef td_s32 fn_viu_get_pipe_wdr_attr(td_s32 viu_pipe_id, ext_viu_pipe_wdr_attr *wdr_attr);
typedef td_s32 fn_viu_get_pipe_hdr_attr(td_s32 viu_pipe_id, ext_viu_pipe_hdr_attr *hdr_attr);
typedef td_s32 fn_viu_get_be_stt_info(td_s32 viu_pipe_id, td_u32 block, td_void *be_stt);
typedef td_s32 fn_viu_update_viu_ppu_mode(ext_viu_operating_mode *viu_ppu_mode);
typedef td_s32 fn_viu_get_pipe_snap_status(td_s32 viu_pipe_id, viu_snap_status *snap_status);
typedef td_s32 fn_viu_get_split_attr(td_s32 viu_pipe_id, viu_pipe_split_attr *split_attr);
typedef td_s32 fn_viu_down_semaphore(td_s32 viu_pipe_id);
typedef td_s32 fn_viu_up_semaphore(td_s32 viu_pipe_id);
typedef td_bool fn_viu_is_pipe_existed(td_s32 viu_pipe_id);
typedef td_s32 fn_viu_get_pipe_bind_dev_size(td_s32 viu_pipe_id, ext_isp_size *dev_size);
typedef td_s32 fn_viu_get_pipe_nrx_status(td_s32 viu_pipe_id, td_bool *nrx_en);
typedef td_s32 fn_viu_set_ppu_frame_interrupt_attr(td_s32 viu_pipe_id,
    const ext_isp_frame_interrupt_attr *frame_int_attr);
typedef td_s32 fn_viu_get_viu_frame_interrupt_attr(td_s32 pipe_id, ext_isp_frame_interrupt_attr *frame_int_attr);
typedef td_s32 fn_viu_isp_clear_input_queue(td_s32 viu_pipe_id);
typedef td_s32 fn_viu_get_isp_config(td_s32 viu_pipe_id, ext_isp_snap_info *snap_info);
typedef td_s32 fn_viu_update_bayer_format(td_s32 viu_pipe_id, ext_isp_bayer_format bayer_format);
typedef td_s32 fn_viu_get_pipe_repeat_mode(td_s32 viu_pipe_id, ext_viu_pipe_repeat_mode *repeat_mode);
typedef td_s32 fn_viu_get_pipe_fpn_attr(td_s32 pipe_id, ext_viu_fpn_attr *fpn_attr);
typedef td_s32 fn_viu_set_pipe_fpn_attr(td_s32 pipe_id, ext_viu_fpn_attr *fpn_attr);
typedef td_s32 fn_viu_get_pipe_transform_attr(td_s32 pipe_id, ext_viu_pipe_transform_attr *transform_attr);

typedef struct {
    fn_viu_get_pipe_wdr_attr *pfn_viu_get_pipe_wdr_attr;
    fn_viu_get_pipe_hdr_attr *pfn_viu_get_pipe_hdr_attr;
    fn_viu_get_be_stt_info *pfn_viu_get_be_stt_info;
    fn_viu_register_flash_call_back *pfn_viu_register_flash_call_back;
    fn_viu_get_pipe_snap_status *pfn_viu_get_pipe_snap_status;
    fn_viu_get_split_attr *pfn_viu_get_split_attr;
    fn_viu_get_pipe_bind_dev_size *pfn_viu_get_pipe_bind_dev_size;
    fn_viu_get_pipe_nrx_status *pfn_viu_get_pipe_nrx_status;
    fn_viu_get_viu_frame_interrupt_attr *pfn_viu_get_viu_frame_interrupt_attr;
    fn_viu_isp_clear_input_queue *pfn_viu_isp_clear_input_queue;
    fn_viu_get_isp_config *pfn_viu_get_isp_config;
    fn_viu_update_bayer_format *pfn_viu_update_bayer_format;
    fn_viu_get_pipe_fpn_attr *pfn_viu_get_pipe_fpn_attr;
    fn_viu_set_pipe_fpn_attr *pfn_viu_set_pipe_fpn_attr;
    fn_viu_get_pipe_transform_attr *pfn_viu_get_pipe_transform_attr;
} viu_export_func;

static inline td_void *viu_get_module_func_by_id(unsigned int module_id)
{
    td_void *func_ext = TD_NULL;

    td_s32 ret = mcp_osal_exportfunc_get(module_id, &func_ext);
    if (ret != TD_SUCCESS) {
        return TD_NULL;
    }
    return func_ext;
}

td_s32 viu_do_mod_init(td_void);
td_void viu_do_mod_exit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* End of __DRV_VIU_EXT_H__ */

