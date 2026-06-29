/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#include "vdec_priv_type.h"
#include "securec.h"
#include "drv_vdec_sdk.h"

td_s32 vdec_drv_get_vfmw_func(vfmw_export_fun *func)
{
    func->fn_vfmw_mod_init = vfmw_mod_init;
    func->fn_vfmw_mod_exit = vfmw_mod_exit;
    func->fn_vfmw_init = vfmw_init;
    func->fn_vfmw_exit = vfmw_exit;
    func->fn_vfmw_suspend = vfmw_suspend;
    func->fn_vfmw_resume = vfmw_resume;
    func->fn_vfmw_get_frame = vfmw_get_image;
    func->fn_vfmw_rels_frame = vfmw_release_image;
    func->fn_vfmw_control = vfmw_control;

    return EXT_SUCCESS;
}

td_s32 vdec_drv_register(td_void *func)
{
    vdec_unused(func);
    return EXT_SUCCESS;
}

td_void vdec_drv_unregister(td_void)
{
    return;
}

td_void vdec_drv_stat_event(td_u32 event_id, td_u32 value)
{
    vdec_unused(event_id);
    vdec_unused(value);
    return;
}

td_u8 *vdec_drv_map(td_void *dma_buf)
{
    vdec_unused(dma_buf);
    return TD_NULL;
}

td_void vdec_drv_unmap(td_void *dma_buf, td_u8 *vir_addr)
{
    vdec_unused(dma_buf);
    vdec_unused(vir_addr);
}
