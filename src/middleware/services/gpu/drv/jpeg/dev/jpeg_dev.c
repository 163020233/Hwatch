/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: jpeg dev
 */

#include "jpeg_dev.h"
#include "platform_core.h"
#include "gpu_crg_intf.h"
#include "jpeg_context.h"
#include "jpeg_dfx.h"
#include "drv_gpu_intf.h"

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

/*-------------------------------- par define ---------------------------------------------*/

/*-------------------------------- func release -------------------------------------------*/

td_s32 jpeg_dev_open(td_void)
{
    td_s32 open_cnt = 0;
    td_ulong lock_flag = 0;
    drv_jpeg_context *jpeg_context = jpeg_context_get();
    drv_jpeg_dfx_info *jpeg_dfx_info = jpeg_dfx_get_info();

    if (jpeg_context == TD_NULL) {
        gpu_err("jpeg is not init!");
        return EXT_FAILURE;
    }

    osal_spin_lock_irqsave(&jpeg_context->lock, &lock_flag);

    open_cnt = osal_atomic_read(&jpeg_context->ref_count);
    osal_atomic_inc(&jpeg_context->ref_count);
    jpeg_dfx_info->open_times++;

    if (open_cnt != 0) {
        osal_spin_unlock_irqrestore(&jpeg_context->lock, &lock_flag);
        return EXT_SUCCESS;
    }

    osal_spin_unlock_irqrestore(&jpeg_context->lock, &lock_flag);

    gpu_crg_init(GPU_ID_JPEG, TD_NULL);

    return EXT_SUCCESS;
}

td_s32 jpeg_dev_close(td_void)
{
    td_s32 open_cnt = 0;
    td_ulong lock_flag = 0;
    drv_jpeg_context *jpeg_context = jpeg_context_get();
    drv_jpeg_dfx_info *jpeg_dfx_info = jpeg_dfx_get_info();
    osal_spin_lock_irqsave(&jpeg_context->lock, &lock_flag);

    open_cnt = osal_atomic_read(&jpeg_context->ref_count);
    if (open_cnt <= 0) {
        osal_spin_unlock_irqrestore(&jpeg_context->lock, &lock_flag);
        return EXT_FAILURE;
    }

    jpeg_dfx_info->close_times++;
    osal_atomic_dec(&jpeg_context->ref_count);

    open_cnt = osal_atomic_read(&jpeg_context->ref_count);
    if (open_cnt != 0) {
        osal_spin_unlock_irqrestore(&jpeg_context->lock, &lock_flag);
        return EXT_SUCCESS;
    }
    osal_spin_unlock_irqrestore(&jpeg_context->lock, &lock_flag);

    gpu_crg_deinit(GPU_ID_JPEG, TD_NULL);
    jpeg_context_unlock();

    return EXT_SUCCESS;
}

td_void jpeg_suspend(td_void)
{
    gpu_crg_deinit(GPU_ID_JPEG, TD_NULL);
    uapi_video_power_ctrl(GPU_ID_JPEG, VIDEO_POWER_OFF);
}

PLT_SEC_ITCM_T td_void jpeg_resume(td_void)
{
    uapi_video_power_ctrl(GPU_ID_JPEG, VIDEO_POWER_ON);
    gpu_crg_init(GPU_ID_JPEG, TD_NULL);
}
