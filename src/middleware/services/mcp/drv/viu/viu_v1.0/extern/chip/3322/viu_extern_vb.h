/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_extern_vb.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef VIU_ADAPT_VB_H
#define VIU_ADAPT_VB_H

#include "drv_vb_ext.h"
#include "viu_public.h"
#include "viu_extern_mem.h"

inline static td_s32 viu_ext_vb_create_pool(td_u32 *pool_id, td_u32 blk_cnt, td_u64 blk_size,
    const td_char *buf_name, ext_vb_remap_mode remap_mode)
{
    viu_unused(pool_id);
    viu_unused(buf_name);
    viu_unused(remap_mode);

    return vi_mem_create_private_pool(blk_cnt, blk_size);
}

inline static td_s32 viu_ext_vb_destroy_pool(td_u32 pool_id)
{
    viu_unused(pool_id);

    return vi_mem_destroy_private_pool();
}

inline static vb_blk_handle viu_ext_vb_get_blk_by_pool_id(td_u32 pool_id, td_u32 uid)
{
    viu_unused(uid);

    return pool_id;
}

inline static vb_blk_handle viu_ext_vb_get_blk_by_size(td_u64 blk_size, td_u32 uid)
{
    return viu_mem_get_blk_by_size(blk_size, uid);
}

inline static vb_blk_handle viu_ext_vb_get_blk_by_size_and_pool_id(td_u32 pool_id, td_u64 blk_size, td_u32 uid)
{
    viu_unused(pool_id);

    return viu_ext_vb_get_blk_by_size(blk_size, uid);
}

inline static td_s32 viu_ext_vb_get_pool_info(ext_vb_pool_info *info)
{
    viu_unused(info);

    return TD_FAILURE;
}

inline static vb_blk_handle viu_ext_vb_phy_to_handle(td_u64 phy_addr)
{
    return viu_mem_phy_to_handle(phy_addr);
}

inline static td_void *viu_ext_vb_phy_to_dmabuf(td_u64 phy_addr)
{
    viu_unused(phy_addr);
    return TD_NULL;
}

inline static td_s32 viu_ext_vb_handle_to_pool_id(vb_blk_handle handle)
{
    viu_unused(handle);
    return handle;
}

inline static td_u64 viu_ext_vb_handle_to_kern(vb_blk_handle handle)
{
    return viu_mem_handle_to_kern(handle);
}

inline static td_u64 viu_ext_vb_handle_to_phys(vb_blk_handle handle)
{
    return viu_mem_handle_to_phys(handle);
}

inline static td_s32 viu_ext_vb_user_add(td_u32 pool_id, td_u64 phy_addr, td_u32 uid)
{
    return viu_mem_user_add(pool_id, phy_addr, uid);
}

inline static td_s32 viu_ext_vb_user_sub(td_u32 pool_id, td_u64 phy_addr, td_u32 uid)
{
    return viu_mem_user_sub(pool_id, phy_addr, uid);
}

inline static td_bool viu_ext_vb_is_blk_valid(td_u32 pool_id, td_u64 phy_addr)
{
    return viu_mem_is_blk_valid(pool_id, phy_addr);
}

inline static td_bool viu_ext_vb_is_pool_id_valid(td_u32 pool_id)
{
    if (pool_id >= VICAP_BUF_NUM) {
        return TD_FALSE;
    }
    return TD_TRUE;
}

inline static td_s32 viu_ext_vb_inquire_pool_user_cnt(td_u32 pool_id, td_u32 uid, td_u32 *cnt)
{
    viu_unused(pool_id);
    viu_unused(uid);
    viu_unused(cnt);

    return TD_FAILURE;
}

inline static ext_video_supplement *viu_ext_vb_handle_to_supplement(vb_blk_handle handle)
{
    viu_unused(handle);
    return TD_NULL;
}

inline static td_void viu_ext_vb_copy_supplement(ext_isp_video_frame *dst_v_frame,
    const ext_isp_video_frame *src_v_frame)
{
    viu_unused(dst_v_frame);
    viu_unused(src_v_frame);

    return;
}

inline static td_s32 viu_ext_vb_get_vb_cfg(ext_vb_base_info *vb_info, ext_isp_vb_cal_config *vb_cal_config)
{
    viu_mem_get_calc_cfg(vb_info, vb_cal_config);
    return TD_SUCCESS;
}

inline static td_s32 viu_ext_vb_get_cmp_bayer_cfg(ext_vb_global_cmp_input *cmp_in,
    ext_vb_cmp_para_output *cmp_cfg, ext_vb_cmp_para_output *dcmp_cfg)
{
    viu_unused(cmp_in);
    viu_unused(cmp_cfg);
    viu_unused(dcmp_cfg);

    return TD_FAILURE;
}

inline static td_s32 viu_ext_vb_get_cmp_seg_cfg(ext_vb_global_cmp_input *cmp_in,
    ext_vb_cmp_para_output *y_cmp_cfg, ext_vb_cmp_para_output *cmp_cfg)
{
    viu_unused(cmp_in);
    viu_unused(y_cmp_cfg);
    viu_unused(cmp_cfg);

    return TD_FAILURE;
}

#endif

