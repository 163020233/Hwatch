/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_dev_id.c
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#include "viu_dev.h"
#include "viu_drv.h"
#include "viu_vicap.h"

#include "hal_videv_intf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 viu_dev_check_id(td_s32 viu_dev_id)
{
    if ((viu_dev_id >= VIU_MAX_DEV_NUM) || (viu_dev_id < 0)) {
        viu_err_trace("vi dev ID[%d] is invalid\r\n", viu_dev_id);
        return SOC_ERR_VIU_INVALID_DEVID;
    }

    return TD_SUCCESS;
}

td_bool viu_dev_check_configed(td_void)
{
    td_s32 viu_dev_id;

    for (viu_dev_id = 0; viu_dev_id < VIU_MAX_DEV_NUM; viu_dev_id++) {
        if (g_ast_dev_mkp_ctx[viu_dev_id].dev_cfged == TD_TRUE) {
            return TD_TRUE;
        }
    }

    return TD_FALSE;
}

/******************************** 模块初始化相关 start**********************************/
td_void viu_dev_mkp_ctx_global_init(td_void)
{
    (td_void)memset_s(g_ast_dev_mkp_ctx, sizeof(viu_dev_mkp_ctx) * VIU_MAX_DEV_NUM,
        0, sizeof(viu_dev_mkp_ctx) * VIU_MAX_DEV_NUM);
}

static td_u32 videv_calc_data_mask(td_u32 port_mask)
{
    td_u32 temp_mask = port_mask;
    td_u32 bit_cnt = 0;

    while (temp_mask != 0) {
        if ((temp_mask & 0x80000000U) == 0x80000000U) { /* 0x80000000 [31]bit */
            bit_cnt++;
        }
        temp_mask <<= 1;
    }

    return bit_cnt;
}

td_s32 viu_dev_set_attr(td_s32 viu_dev_id, const ext_viu_dev_attr *dev_attr)
{
    td_s32 ret;
    td_ulong flags;

    viu_dev_mkp_ctx *viu_dev_ctx = TD_NULL;
    viu_dev_drv_ctx *drv_dev_ctx = TD_NULL;

    /* 参数检查 */
    viu_dev_check_id_return_val(viu_dev_id);
    viu_check_null_ptr_return(dev_attr);

    viu_dev_down_interruptible_sem(viu_dev_id);
    viu_dev_spin_lock(viu_dev_id, flags);

    viu_dev_ctx = viu_mkp_get_dev_ctx(viu_dev_id);
    /* 设备在不使能时才可设置属性，设备不使能时可任意设置属性 */
    if (viu_dev_ctx->dev_en == TD_TRUE) {
        viu_dev_spin_unlock(viu_dev_id, flags);
        viu_dev_up_sem(viu_dev_id);
        viu_err_trace("can't set attr of vi dev[%d], because it has been enabled\n", viu_dev_id);
        return SOC_ERR_VIU_FAILED_NOTDISABLE;
    }

    /* 先配时钟。配置VI系统时钟: CRG12 */
    ret = hal_videv_init(viu_dev_id, dev_attr);
    if (ret != TD_SUCCESS) {
        viu_dev_spin_unlock(viu_dev_id, flags);
        viu_dev_up_sem(viu_dev_id);
        viu_err_trace("videv%d init fail.\n", viu_dev_id);
        return ret;
    }

    hal_videv_process(viu_dev_id, dev_attr);

    /* 在drv层记录设备属性，以便drv层其它函数获取该dev属性 */
    drv_dev_ctx = viu_drv_get_dev_ctx(viu_dev_id);
    (td_void)memcpy_s(&(drv_dev_ctx->viu_dev_attr), sizeof(ext_viu_dev_attr), dev_attr, sizeof(ext_viu_dev_attr));
    drv_dev_ctx->sensor_bit_width = videv_calc_data_mask(dev_attr->component_mask[0]);

    viu_dev_ctx->dev_cfged = TD_TRUE;
    viu_dev_spin_unlock(viu_dev_id, flags);
    viu_dev_up_sem(viu_dev_id);

    return TD_SUCCESS;
}

td_s32 viu_dev_get_attr(td_s32 viu_dev_id, ext_viu_dev_attr *dev_attr)
{
    viu_dev_mkp_ctx *viu_dev_ctx = TD_NULL;
    /* 参数检查 */
    viu_dev_check_id_return_val(viu_dev_id);
    viu_check_null_ptr_return(dev_attr);

    viu_dev_down_interruptible_sem(viu_dev_id);

    viu_dev_ctx = viu_mkp_get_dev_ctx(viu_dev_id);
    if (viu_dev_ctx->dev_cfged == TD_FALSE) {
        viu_dev_up_sem(viu_dev_id);
        viu_err_trace("attr of vi dev %d not configured\n", viu_dev_id);
        return SOC_ERR_VIU_FAILED_NOTCONFIG;
    }

    viu_drv_get_dev_attr(viu_dev_id, dev_attr);

    viu_dev_up_sem(viu_dev_id);

    return TD_SUCCESS;
}

td_s32 viu_dev_enable(td_s32 viu_dev_id, td_void *unique)
{
    td_s32 ret;
    td_ulong flags;
    viu_dev_mkp_ctx *viu_dev_ctx = TD_NULL;

    /* 参数检查 */
    viu_dev_check_id_return_val(viu_dev_id);

    viu_dev_down_interruptible_sem(viu_dev_id);

    viu_dev_spin_lock(viu_dev_id, flags);

    viu_dev_ctx = viu_mkp_get_dev_ctx(viu_dev_id);
    viu_dev_ctx->unique = *(td_u32*)unique;

    if (viu_dev_ctx->dev_en == TD_TRUE) {
        ret = TD_SUCCESS;
        goto end;
    }

    /* 设置设备属性后才可以使能设备 */
    if (viu_dev_ctx->dev_cfged != TD_TRUE) {
        viu_err_trace("not allowed enable dev because dev%d not configured\n", viu_dev_id);
        ret = SOC_ERR_VIU_FAILED_NOTCONFIG;
        goto end;
    }

    ret = viu_drv_check_chip_id(viu_dev_id);
    if (ret != TD_SUCCESS) {
        goto end;
    }

    hal_videv_enable(viu_dev_id, TD_TRUE);

    viu_dev_ctx->dev_en = TD_TRUE;

end:
    viu_dev_spin_unlock(viu_dev_id, flags);

    viu_dev_up_sem(viu_dev_id);

    return ret;
}

td_s32 viu_dev_disable(td_s32 viu_dev_id, td_void *unique)
{
    td_ulong flags;
    viu_dev_mkp_ctx *viu_dev_ctx = TD_NULL;
    ext_viu_dev_bind_pipe dev_bind_pipe;
    td_u32 i;

    /* 参数检查 */
    viu_dev_check_id_return_val(viu_dev_id);

    viu_dev_down_interruptible_sem(viu_dev_id);

    viu_dev_spin_lock(viu_dev_id, flags);

    viu_dev_ctx = viu_mkp_get_dev_ctx(viu_dev_id);
    /* 其他进程不能退出本进程的dev */
    if (viu_dev_ctx->unique != *(td_u32*)unique) {
        viu_dev_spin_unlock(viu_dev_id, flags);
        viu_dev_up_sem(viu_dev_id);
        return TD_SUCCESS;
    }

    /* 设备在不使能时才可设置属性，设备不使能时可任意设置属性 */
    if (viu_dev_ctx->dev_en != TD_TRUE) {
        viu_dev_spin_unlock(viu_dev_id, flags);
        viu_dev_up_sem(viu_dev_id);
        return TD_SUCCESS;
    }

    viu_drv_get_dev_bind_pipe(viu_dev_id, &dev_bind_pipe);

    for (i = 0; i < dev_bind_pipe.num; i++) {
        if (viu_is_pipe_existed(dev_bind_pipe.pipe_id[i]) == TD_TRUE) {
            viu_dev_spin_unlock(viu_dev_id, flags);
            viu_dev_up_sem(viu_dev_id);
            viu_err_trace("the pipe %d of binding dev %d is not destroyed.\n", dev_bind_pipe.pipe_id[i], viu_dev_id);
            return SOC_ERR_VIU_PIPE_EXIST;
        }
    }

    /* 后续添加关闭wdr,snap等 */
    hal_videv_enable(viu_dev_id, TD_FALSE);

    hal_videv_deinit(viu_dev_id);

    for (i = 0; i < dev_bind_pipe.num; i++) {
        viu_drv_set_pipe_bind_dev_id(dev_bind_pipe.pipe_id[i], INVALID_DEV);
    }

    viu_dev_ctx->dev_en = TD_FALSE;
    viu_dev_ctx->dev_cfged = TD_FALSE;

    viu_dev_spin_unlock(viu_dev_id, flags);

    viu_dev_up_sem(viu_dev_id);

    return TD_SUCCESS;
}

td_s32 viu_dev_attach_pipe(td_u8 dev_id, td_u8 pipe_id)
{
    td_s32 ret = TD_SUCCESS;
    td_ulong flags;
    viu_dev_mkp_ctx *viu_dev_ctx = TD_NULL;

    /* 参数检查 */
    viu_dev_check_id_return_val(dev_id);

    viu_dev_down_interruptible_sem(dev_id);

    viu_dev_ctx = viu_mkp_get_dev_ctx(dev_id);
    if (viu_dev_ctx->dev_en != TD_TRUE) {
        viu_err_trace("not allowed bind pipe because dev%d not enable\n", dev_id);
        ret = SOC_ERR_VIU_FAILED_NOTENABLE;
        goto end;
    }

    viu_dev_spin_lock(dev_id, flags);
    viu_drv_set_dev_attach_pipe(dev_id, pipe_id);
    viu_dev_spin_unlock(dev_id, flags);

end:
    viu_dev_up_sem(dev_id);
    return ret;
}

td_s32 viu_dev_detach_pipe(td_u8 dev_id, td_u8 pipe_id)
{
    td_s32 ret = TD_SUCCESS;
    td_ulong flags;
    viu_dev_mkp_ctx *viu_dev_ctx = TD_NULL;

    /* 参数检查 */
    viu_dev_check_id_return_val(dev_id);

    viu_dev_down_interruptible_sem(dev_id);

    viu_dev_ctx = viu_mkp_get_dev_ctx(dev_id);
    if (viu_dev_ctx->dev_en != TD_TRUE) {
        viu_err_trace("not allowed get bind pipe because dev%d not enable\n", dev_id);
        ret = SOC_ERR_VIU_FAILED_NOTENABLE;
        goto detach_end;
    }

    viu_dev_spin_lock(dev_id, flags);
    viu_drv_set_dev_detach_pipe(dev_id, pipe_id);
    viu_dev_spin_unlock(dev_id, flags);

detach_end:
    viu_dev_up_sem(dev_id);
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

