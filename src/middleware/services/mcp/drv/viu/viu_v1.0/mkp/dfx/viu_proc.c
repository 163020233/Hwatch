/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2016-2021. All rights reserved.
 * Description: viu_proc.c
 * Author: AuthorNameMagicTag
 * Create: 2016-10-08
 */
#include "viu.h"
#include "viu_pipe.h"
#include "viu_pic_queue.h"
#include "viu_drv.h"
#include "drv_vicap.h"
#include "drv_viu.h"
#include "viu_dev.h"
#include "hal_videv_intf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
td_void viu_isp_dev_dfx(td_void)
{
    viu_dev_mkp_ctx *viu_dev_ctx = TD_NULL;
    ext_viu_dev_attr dev_attr = {0};

    viu_err_trace("--------------------vi dev info-----------------------\n");
    for (int dev_id = 0; dev_id < VIU_MAX_DEV_NUM; dev_id++) {
        viu_dev_ctx = viu_mkp_get_dev_ctx(dev_id);
        if (viu_dev_ctx->dev_cfged == TD_FALSE) {
            continue;
        }
        viu_dev_get_attr(dev_id, &dev_attr);
        viu_err_trace("%-10s%-10s%-10s\n", "dev id", "dev_en", "intf_mode");
        viu_err_trace("%-10d%-10d%-10d\n",
            dev_id,
            viu_dev_ctx->dev_en,
            dev_attr.intf_mode);
    }
}

td_void viu_isp_pipe_dfx(td_void)
{
    viu_wch_dbg_info *wch_info = TD_NULL;
    viu_pipe_mkp_ctx *pipe_mkp_ctx = NULL;
    viu_pipe_mkp_attr *pipe_mkp_attr_working_ctx = TD_NULL;

    ext_viu_pipe_attr pipe_attr = {0};

    viu_err_trace("--------------------vi pipe info-----------------------\n");
    for (int pipe_id = 0; pipe_id < ISP_MAX_PIPE_NUM; pipe_id++) {
        if (is_pipe_unexist(pipe_id)) {
            continue;
        }
        pipe_mkp_ctx = viu_get_pipe_mkp_ctx(pipe_id);
        pipe_mkp_attr_working_ctx = viu_get_pipe_mkp_working_attr_ctx(pipe_id);
        wch_info = &pipe_mkp_ctx->wch_dbg;
        viu_pipe_get_attr(pipe_id, &pipe_attr);

        viu_err_trace("%-10s%-10s%-10s%-10s%-10s%-10s\n", "pipe id", "start", "frameRate", "intCnt", "width", "height");
        viu_err_trace("%-10d%-10d%-10d%-10d%-10d%-10d\n",
            pipe_id,
            pipe_mkp_attr_working_ctx->start,
            wch_info->pipe_frm_rate,
            wch_info->int_cnt,
            pipe_attr.max_w,
            pipe_attr.max_h);
    }
    viu_err_trace("-----------------------------------------------------\n");
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
