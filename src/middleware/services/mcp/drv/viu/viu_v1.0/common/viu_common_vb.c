/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: common function.
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#include "viu_extern.h"
#include "viu_drv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 viu_drv_put_vb_with_ret_value(const ext_isp_video_frame_info *viu_frm_info, ext_vb_uid uid)
{
    td_u32 pool_id;
    td_u64 phys_addr;
    td_s32 ret;

    if (viu_frm_info == NULL) {
        viu_err_trace("NULL pointer\r\n");
        return SOC_ERR_VIU_INVALID_NULL_PTR;
    }

    pool_id = viu_frm_info->pool_id;
    phys_addr = viu_frm_info->v_frame.phy_addr[0];

    if (viu_ext_vb_is_blk_valid(pool_id, phys_addr) == TD_TRUE) {
        ret = viu_ext_vb_user_sub(pool_id, phys_addr, (td_u32)uid);
        if (ret != TD_SUCCESS) {
            ret = SOC_ERR_VIU_NOT_PERM;
            viu_err_trace("Sub vb err! addr 0x%llx, pool_id %u, uid %d, ret 0x%x\r\n", phys_addr, pool_id, uid, ret);
        }
    } else {
        ret = SOC_ERR_VIU_INVALID_PARA;
        viu_err_trace("Sub invalid vb! addr 0x%llx, pool_id %u, uid %d, ret 0x%x\r\n", phys_addr, pool_id, uid, ret);
    }

    return ret;
}

td_void viu_drv_put_vb(const ext_isp_video_frame_info *viu_frm_info, ext_vb_uid uid)
{
    td_s32 ret;

    ret = viu_drv_put_vb_with_ret_value(viu_frm_info, uid);
    if (ret != TD_SUCCESS) {
        viu_err_trace("put vb err! 0x%x\r\n", ret);
    }
}

td_s32 viu_drv_get_vb(const ext_isp_video_frame_info *viu_frm_info, ext_vb_uid uid)
{
    td_u32 pool_id;
    td_u64 phys_addr;
    td_s32 ret;

    if (viu_frm_info == NULL) {
        viu_err_trace("NULL pointer\r\n");
        return SOC_ERR_VIU_INVALID_NULL_PTR;
    }

    pool_id = viu_frm_info->pool_id;
    phys_addr = viu_frm_info->v_frame.phy_addr[0];

    if (viu_ext_vb_is_blk_valid(pool_id, phys_addr) == TD_TRUE) {
        ret = viu_ext_vb_user_add(pool_id, phys_addr, (td_u32)uid);
        if (ret != TD_SUCCESS) {
            ret = SOC_ERR_VIU_NOT_PERM;
            viu_err_trace("Add vb err! addr 0x%llu, pool_id %u, uid %d, ret 0x%x\r\n", phys_addr, pool_id, uid, ret);
        }
    } else {
        ret = SOC_ERR_VIU_INVALID_PARA;
        viu_err_trace("Add invalid vb! addr 0x%llu, pool_id %u, uid %d, ret 0x%x\r\n", phys_addr, pool_id, uid, ret);
    }

    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

