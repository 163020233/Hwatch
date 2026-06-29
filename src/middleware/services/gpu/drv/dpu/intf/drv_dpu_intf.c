/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: dpu interface
 */

#include "drv_dpu_intf.h"
#include "platform_core.h"
#include "soc_osal.h"
#include "drv_dpu_ioctl.h"
#include "dpu_disp.h"
#include "dpu_layer.h"
#include "dpu_isr.h"
#include "dpu_ioctl.h"
#include "dpu_dfx.h"
#include "dpu_qspi.h"
#include "dpu_param_print.h"

STATIC td_bool g_is_dma_qspi = TD_FALSE;
STATIC td_bool g_dpu_suspend_flag = TD_FALSE;

#define DRV_DPU_IOCTL_CMD_NUM 21

td_bool uapi_dpu_get_suspend_flag(void)
{
    return g_dpu_suspend_flag;
}

STATIC td_s32 drv_dpu_open(td_void *private_data)
{
    uapi_unused(private_data);

    return EXT_SUCCESS;
}

STATIC td_s32 drv_dpu_close(td_void *private_data)
{
    uapi_unused(private_data);

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_suspend(td_void *private_data)
{
    uapi_unused(private_data);

    if (g_is_dma_qspi == TD_TRUE) {
        return EXT_SUCCESS;
    }

    dpu_dfx_info *dfx_info = dpu_dfx_get_info();
    dfx_info->suspend_cnt++;

    gpu_dbg("dpu suspend start");
    dpu_disp_info *disp_info = dpu_disp_get_info();
    dpu_clk_scene_info scene_info = {.cfg_scene = GPU_CLK_CFG_SUSPEND, .work_scene = disp_info->scene_info.work_scene,
        .is_cmp_out = disp_info->scene_info.is_cmp_out, .is_rotate = disp_info->scene_info.is_rotate};
    dpu_hal_deinit(&scene_info);
    dpu_layer_suspend();
    g_dpu_suspend_flag = TD_TRUE;
    gpu_dbg("dpu suspend ok");

    return EXT_SUCCESS;
}

PLT_SEC_ITCM_T td_s32 uapi_dpu_resume(td_void *private_data)
{
    uapi_unused(private_data);

    if (g_is_dma_qspi == TD_TRUE) {
        return EXT_SUCCESS;
    }

    dpu_dfx_info *dfx_info = dpu_dfx_get_info();
    dfx_info->resume_cnt++;

    gpu_dbg("dpu resume start");
    dpu_disp_info *disp_info = dpu_disp_get_info();
    dpu_clk_scene_info scene_info = {.cfg_scene = GPU_CLK_CFG_RESUME, .work_scene = disp_info->scene_info.work_scene,
        .is_cmp_out = disp_info->scene_info.is_cmp_out, .is_rotate = disp_info->scene_info.is_rotate};
    dpu_hal_init(&scene_info);
    dpu_layer_resume();
    dpu_disp_chl_cfg();
    g_dpu_suspend_flag = TD_FALSE;
    gpu_dbg("dpu resume ok");

    return EXT_SUCCESS;
}

td_s32 dpu_open(const td_char *path_name, td_s32 flags, td_s32 mode)
{
    uapi_unused(path_name);
    uapi_unused(flags);
    uapi_unused(mode);

    return EXT_SUCCESS;
}

td_s32 dpu_close(td_s32 module_idx)
{
    uapi_unused(module_idx);

    return EXT_SUCCESS;
}

STATIC osal_ioctl_cmd g_dpu_ioctl_cmd_list[DRV_DPU_CMD_NUM] = {
    { DRV_DPU_CMD_CREATELAYER,         dpu_ioctl_create_layer },
    { DRV_DPU_CMD_DESTROYLAYER,        dpu_ioctl_destroy_layer },
    { DRV_DPU_CMD_PUTLAYERALPHA,       dpu_ioctl_set_layer_alpha },
    { DRV_DPU_CMD_GETLAYERALPHA,       dpu_ioctl_get_layer_alpha },
    { DRV_DPU_CMD_PUTLAYERPOS,         dpu_ioctl_set_layer_pos },
    { DRV_DPU_CMD_GETLAYERPOS,         dpu_ioctl_get_layer_pos },
    { DRV_DPU_CMD_PUTLAYERZORDER,      dpu_ioctl_set_layer_zorder },
    { DRV_DPU_CMD_GETLAYERZORDER,      dpu_ioctl_get_layer_zorder },
    { DRV_DPU_CMD_SHOWLAYER,           dpu_ioctl_show_layer },
    { DRV_DPU_CMD_HIDELAYER,           dpu_ioctl_hide_layer },
    { DRV_DPU_CMD_PUTLAYERINFO,        dpu_ioctl_set_layer_info },
    { DRV_DPU_CMD_GETLAYERINFO,        dpu_ioctl_get_layer_info },
    { DRV_DPU_CMD_PUTYLAYERCOLORKYE,   dpu_ioctl_set_layer_colorkey },
    { DRV_DPU_CMD_GETYLAYERCOLORKYE,   dpu_ioctl_get_layer_colorkey },
    { DRV_DPU_CMD_REFRESHLAYERS,       dpu_ioctl_refresh_layers },
    { DRV_DPU_CMD_WAITLAYERVSYNC,      dpu_ioctl_wait_layer_vsync },
    { DRV_DPU_CMD_PUTLAYERVBLANK,      dpu_ioctl_set_layer_vblank },
    { DRV_DPU_CMD_GETLAYERVBLANK,      dpu_ioctl_get_layer_vblank },
    { DRV_DPU_CMD_QUERYLAYERSTATE,     dpu_ioctl_query_layer_state },
    { DRV_DPU_CMD_SETOUTPUTINFO,       dpu_ioctl_set_output_info },
    { DRV_DPU_CMD_WAITFORDONE,         dpu_ioctl_wait_for_done },
};

STATIC osal_fileops g_dpu_file_fops = {
    .open     = drv_dpu_open,
    .release  = drv_dpu_close,
    .cmd_list = g_dpu_ioctl_cmd_list,
    .cmd_cnt  = DRV_DPU_CMD_NUM,
};

STATIC osal_pmops g_dpu_ops = {
    .pm_suspend        = uapi_dpu_suspend,
    .pm_resume         = uapi_dpu_resume,
};

STATIC osal_dev g_dpu_register_data = {
    .minor = 0,
    .fops  = &g_dpu_file_fops,
    .pmops = &g_dpu_ops,
};

td_s32 dpu_ioctl(td_s32 module_idx, td_ulong ioctl_cmd, td_void *para)
{
    uapi_unused(module_idx);

    if (ioctl_cmd >= DRV_DPU_CMD_NUM) {
        gpu_err("Invalid cmd, cmd = %lu!", ioctl_cmd);
        return EXT_FAILURE;
    }

    if (g_dpu_suspend_flag == TD_TRUE) {
        gpu_err("dpu is in suspend state!, suspend_flag = %u", g_dpu_suspend_flag);
        return EXT_FAILURE;
    }
    if (dpu_dfx_check_mask(DPU_DFX_MASK_PRINT_PARAMS)) {
        dpu_para_print((td_u32)ioctl_cmd, para);
    }
    if (g_is_dma_qspi == TD_TRUE) {
        return dpu_qspi_ioctl(ioctl_cmd, para);
    }

    return (td_s32)g_dpu_ioctl_cmd_list[ioctl_cmd].handler((td_u32)ioctl_cmd, para, TD_NULL);
}

STATIC td_s32 dpu_register_device(td_void)
{
    td_s32 ret;

    ret = snprintf_s(g_dpu_register_data.name, OSAL_DEV_NAME_LEN - 1, strlen(DPU_DEV_NAME), "%s", DPU_DEV_NAME);
    if (ret <= 0) {
        gpu_err("snprintf_s failed! ret=%d.\n", ret);
        return EXT_FAILURE;
    }

    if (osal_dev_register(&g_dpu_register_data) < 0) {
        gpu_err("register vau failed.\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_void dpu_unregister_device(td_void)
{
    osal_dev_unregister(&g_dpu_register_data);
}

td_s32 uapi_dpu_module_init(void)
{
#ifdef GPU_DMA_QSPI
    g_is_dma_qspi = TD_TRUE;
#endif
    if (g_is_dma_qspi == TD_TRUE) {
        dpu_qspi_init();
    }

    dpu_disp_timing timing = {};
    td_s32 ret = disp_get_panel_timing(TD_NULL, &timing, TD_FALSE);
    if (ret != EXT_SUCCESS) {
        gpu_err("disp_get_panel_timing fail, ret=%#x!", ret);
    }

    gpu_crg_set_dpu_normal_pixel_clk(timing.pixel_clk);

    dpu_clk_scene_info scene_info = {.cfg_scene = GPU_CLK_CFG_INIT, .work_scene = DRV_DPU_WORK_SCENE_NORMAL,
        .is_cmp_out = TD_FALSE, .is_rotate = TD_FALSE};
    ret = dpu_hal_init(&scene_info);
    if (ret != EXT_SUCCESS) {
        gpu_err("dpu_hal_init fail, ret=%#x!", ret);
        return ret;
    }

    ret = dpu_isr_init();
    if (ret != EXT_SUCCESS) {
        dpu_hal_deinit(&scene_info);
        gpu_err("dpu_isr_init fail, ret=%#x!", ret);
        return ret;
    }

    ret = dpu_disp_init();
    if (ret != EXT_SUCCESS) {
        dpu_isr_deinit();
        dpu_hal_deinit(&scene_info);
        gpu_err("dpu_disp_init fail, ret=%#x!", ret);
        return ret;
    }

    ret = dpu_layer_init();
    if (ret != EXT_SUCCESS) {
        dpu_disp_deinit();
        dpu_isr_deinit();
        dpu_hal_deinit(&scene_info);
        gpu_err("dpu_layer_init fail, ret=%#x!", ret);
        return ret;
    }

    ret = dpu_register_device();
    if (ret != EXT_SUCCESS) {
        uapi_dpu_module_exit();
        gpu_err("dpu_register_device failed, ret=%#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_void uapi_dpu_module_exit(void)
{
    dpu_unregister_device();
    dpu_layer_deinit();
    dpu_disp_deinit();
    dpu_isr_deinit();
    dpu_disp_info *disp_info = dpu_disp_get_info();
    dpu_clk_scene_info scene_info = {.cfg_scene = GPU_CLK_CFG_INIT, .work_scene = DRV_DPU_WORK_SCENE_AOD,
        .is_cmp_out = disp_info->scene_info.is_cmp_out, .is_rotate = disp_info->scene_info.is_rotate};
    dpu_hal_deinit(&scene_info);
}
