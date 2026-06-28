/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: ppu drv ext header file
 * Author: AuthorNameMagicTag
 * Create: 2021-9-1
 */

#ifndef __DRV_PPU_EXT_H__
#define __DRV_PPU_EXT_H__

#include "drv_mcp_dependency.h"
#include "drv_ppu_struct.h"
#include "drv_dis_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PPU_FILED_NUM              2
#define PPU_DOUBLE_FRAME_NUM       2

typedef struct {
    ext_isp_video_frame_info video_frame;
    mcp_soc_mod_id mod_id;
    td_bool  block_mode; /* flashed video frame or not. */
    td_u64   start_time;
    td_u64   node_index;
    td_u64   pts[PPU_MAX_PHY_CHN_NUM];
} ext_ppu_pic_info;

typedef struct {
    ext_ppu_pic_info *src_pic_info; /* information of source pic */
    ext_ppu_pic_info *old_pic_info; /* information of backup pic */
    td_bool scale_cap; /* whether scaling */
    td_bool trans_cap; /* whether the frame rate is doubled */
    td_bool malloc_buffer; /* whether malloc frame buffer */
} ext_ppu_query_info;

typedef struct {
    td_bool new_frame; /* whether use new pic to query */
    td_bool ppu_proc; /* whether ppu need to process */
    td_bool double_frame; /* whether the frame rate is doubled */
    td_bool update_backup; /* whether update backup pic */
    ext_isp_compress_mode compress_mode; /* compress mode */
    ext_ppu_pic_info      dest_pic_info[PPU_DOUBLE_FRAME_NUM]; /* double frame */
    ext_isp_aspect_ratio  aspect_ratio; /* aspect ratio configuration */
} ext_ppu_inst_info;

typedef struct {
    td_bool suc; /* whether successful completion */
    td_s32   grp;
    td_s32   chn;
    ext_ppu_pic_info *dest_pic_info[PPU_FILED_NUM]; /* pic processed by ppu.0:top field 1:bottom field */
    td_u64 node_index;
} ext_ppu_send_info;

typedef struct {
    mcp_soc_mod_id mod_id;
    td_s32(*ppu_query)(td_s32 dev_id, td_s32 chn_id, ext_ppu_query_info *query_info,
           ext_ppu_inst_info *inst_info);
    td_s32 (*ppu_send)(td_s32 dev_id, td_s32 chn_id, ext_ppu_send_info *send_info);
    td_s32 (*reset_call_back)(td_s32 dev_id, td_s32 chn_id, td_void *pv_data);
} ext_ppu_register_info;

typedef struct {
    td_bool ppu_en[PPU_IP_NUM];
} ext_ppu_ip_enable;

typedef struct {
    td_s32(*pfn_ppu_online_prepare_task)(td_s32 grp, const ext_isp_video_frame_info *vi_frame, td_bool lost_frame,
           ext_isp_pixel_format pixel_format);
    td_s32 (*pfn_ppu_online_start_task)(td_u32 ppu_id);
    td_s32 (*pfn_ppu_online_complete_task)(td_u32 ppu_id, td_s32 state);
    td_s32 (*pfn_ppu_get_power_en)(td_bool *ppu_power_en);
    td_s32 (*pfn_ppu_viu_early_irq_proc)(td_s32 grp);
    td_s32 (*pfn_ppu_online_submit_task)(td_s32 grp, td_s32 ppu_id);
    td_s32 (*pfn_ppu_queue_frame)(td_s32 grp, const ext_isp_video_frame_info *frame);
    td_s32 (*pfn_ppu_dequeue_frame)(td_s32 grp, ext_isp_video_frame_info *frame);
    td_s32 (*pfn_ppu_set_dis_config)(td_s32 grp, const ext_dis_config *dis_config);
    td_s32 (*pfn_ppu_set_dis_attr)(td_s32 grp, const ext_dis_attr *dis_attr);
} drv_ppu_export_func;

int drv_ppu_module_init(td_void);
td_void drv_ppu_module_exit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __DRV_PPU_EXT_H__ */
