/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: wdr_drv.c
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */
#include "wdr_drv.h"
#include "viu_drv.h"
#include "wdr_buf.h"
#include "drv_vicap.h"
#include "hal_vicap_intf.h"
#include "hal_viproc_intf.h"
#include "hal_capability_intf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define WDR_MAX_BUF_NUM_PER_OFF_PIPE    4
#define WDR_MAX_BUF_NUM_PER_ON_PIPE     1

td_bool viu_drv_is_wdr_mode(ext_isp_wdr_mode wdr_mode)
{
#if defined(CONFIG_CHIP_3322)
    viu_unused(wdr_mode);
    return TD_FALSE;
#else
    td_bool wdr_mode_en = TD_FALSE;

    if ((wdr_mode != EXT_ISP_WDR_MODE_NONE) && (wdr_mode != EXT_ISP_WDR_MODE_BUILT_IN)) {
        wdr_mode_en = TD_TRUE;
    }

    return wdr_mode_en;
#endif
}

/*****************************************************************************
  1.Date         : 2016/11/26
    Create       : 根据pipe返回wdr模式
*****************************************************************************/
ext_isp_wdr_mode viu_drv_get_wdr_mode_from_dev(td_s32 viu_pipe_id)
{
#if defined(CONFIG_CHIP_3322)
    viu_unused(viu_pipe_id);
    return EXT_ISP_WDR_MODE_NONE;
#else
    td_s32 viu_dev_id;
    viu_dev_drv_ctx *viu_dev_ctx = TD_NULL;
    ext_isp_wdr_mode wdr_mode;

    viu_dev_id = viu_drv_get_pipe_bind_dev_id_before_init(viu_pipe_id);
    if ((viu_dev_id >= VIU_MAX_DEV_NUM) || (viu_dev_id < 0)) {
        wdr_mode = EXT_ISP_WDR_MODE_NONE;
    } else {
        viu_dev_ctx = viu_drv_get_dev_ctx(viu_dev_id);
        wdr_mode = viu_dev_ctx->viu_dev_attr.wdr_attr.wdr_mode;
    }

    return wdr_mode;
#endif
}

td_u32 viu_drv_get_wdr_vc_num(ext_isp_wdr_mode wdr_mode)
{
#if defined(CONFIG_CHIP_3322)
    viu_unused(wdr_mode);
    return 0;
#else
    td_u32 vc_num;

    if ((wdr_mode == EXT_ISP_WDR_MODE_2TO1_LINE) || (wdr_mode == EXT_ISP_WDR_MODE_2TO1_FRAME)) {
        vc_num = 1;
    } else if (wdr_mode == EXT_ISP_WDR_MODE_BUILT_IN) {
        vc_num = 0;
    } else {
        vc_num = 1;
    }

    return vc_num;
#endif
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
