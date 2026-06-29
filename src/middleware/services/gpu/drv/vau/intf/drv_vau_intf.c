/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: vau interface
 */

#include "drv_vau_intf.h"
#include "platform_core.h"
#include "soc_osal.h"
#include "drv_vau_ioctl.h"
#include "vau_ioctl.h"
#include "vau_isr.h"
#include "vau_mem.h"
#include "vau_job.h"
#include "vau_list.h"
#include "vau_hal.h"
#include "vau_dfx.h"

#define DRV_VAU_IOCTL_CMD_NUM 20

STATIC td_bool g_vau_suspend_flag = TD_FALSE;

STATIC td_s32 drv_vau_open(td_void *private_data)
{
    uapi_unused(private_data);

    return EXT_SUCCESS;
}

STATIC td_s32 drv_vau_close(td_void *private_data)
{
    uapi_unused(private_data);

    return EXT_SUCCESS;
}

td_s32 uapi_vau_suspend(td_void *private_data)
{
    uapi_unused(private_data);

    vau_dfx_info *dfx_info = vau_dfx_get_info();
    dfx_info->suspend_cnt++;

    gpu_dbg("vau suspend start");
    vau_clk_scene_info scene_info = {.cfg_scene = GPU_CLK_CFG_SUSPEND, .work_scene = DRV_VAU_WORK_SCENE_PERFORMANCE_MODE};
    vau_hal_suspend(&scene_info);
    g_vau_suspend_flag = TD_TRUE;
    gpu_dbg("vau suspend ok");

    return EXT_SUCCESS;
}

PLT_SEC_ITCM_T td_s32 uapi_vau_resume(td_void *private_data)
{
    uapi_unused(private_data);

    vau_dfx_info *dfx_info = vau_dfx_get_info();
    dfx_info->resume_cnt++;

    gpu_dbg("vau resume start");
    vau_clk_scene_info scene_info = {.cfg_scene = GPU_CLK_CFG_RESUME, .work_scene = vau_get_clk_work_scene()};
    vau_hal_resume(&scene_info);
#ifdef CONFIG_CLDO1PD_SUPPORT
    vau_mem_draw_pool_init();
#endif
    g_vau_suspend_flag = TD_FALSE;
    gpu_dbg("vau resume ok");

    return EXT_SUCCESS;
}

td_s32 vau_open(const td_char *path_name, td_s32 flags, td_s32 mode)
{
    uapi_unused(path_name);
    uapi_unused(flags);
    uapi_unused(mode);

    return EXT_SUCCESS;
}

td_s32 vau_close(td_s32 module_idx)
{
    uapi_unused(module_idx);

    return EXT_SUCCESS;
}

STATIC osal_ioctl_cmd g_vau_ioctl_cmd_list[DRV_VAU_IOCTL_CMD_NUM] = {
    { DRV_VAU_CMD_CREATE,        vau_ioctl_create        },
    { DRV_VAU_CMD_DESTROY,       vau_ioctl_destroy       },
    { DRV_VAU_CMD_FILL,          vau_ioctl_fill          },
    { DRV_VAU_CMD_BLIT,          vau_ioctl_blit          },
    { DRV_VAU_CMD_COMPOSE,       vau_ioctl_compose       },
    { DRV_VAU_CMD_SUBMIT,        vau_ioctl_submit        },
    { DRV_VAU_CMD_CANCEL,        vau_ioctl_cancel        },
    { DRV_VAU_CMD_WAIT_FOR_DONE, vau_ioctl_wait_for_done },
    { DRV_VAU_CMD_BLIT_HDR,      vau_ioctl_blit_hdr      },
    { DRV_VAU_CMD_SET_CUBIC_THD, vau_ioctl_set_cubic_threshold },
    { DRV_VAU_CMD_GET_DRAW_RECT, vau_ioctl_get_draw_rect },
    { DRV_VAU_CMD_WAIT_ALL_DONE, vau_ioctl_wait_all_done },
    { DRV_VAU_IOCTL_CMD_NUM - 1, TD_NULL }
};

STATIC osal_fileops g_vau_file_fops = {
    .open     = drv_vau_open,
    .release  = drv_vau_close,
    .cmd_list = g_vau_ioctl_cmd_list,
    .cmd_cnt  = DRV_VAU_IOCTL_CMD_NUM,
};

STATIC osal_pmops g_vau_ops = {
    .pm_suspend        = uapi_vau_suspend,
    .pm_resume         = uapi_vau_resume,
};

STATIC osal_dev g_vau_register_data = {
    .minor = 0,
    .fops  = &g_vau_file_fops,
    .pmops = &g_vau_ops,
};

td_s32 vau_ioctl(td_s32 module_idx, td_ulong ioctl_cmd, td_void *para)
{
    uapi_unused(module_idx);

    if (ioctl_cmd >= DRV_VAU_IOCTL_CMD_NUM) {
        gpu_err("Invalid cmd, cmd = %lu!", ioctl_cmd);
        return EXT_FAILURE;
    }

    if (g_vau_suspend_flag == TD_TRUE) {
        gpu_err("vau is in suspend state!, suspend_flag = %u", g_vau_suspend_flag);
        return EXT_FAILURE;
    }

    return (td_s32)g_vau_ioctl_cmd_list[ioctl_cmd].handler((td_u32)ioctl_cmd, para, TD_NULL);
}

STATIC td_s32 vau_register_device(td_void)
{
    td_s32 ret;

    ret = snprintf_s(g_vau_register_data.name, OSAL_DEV_NAME_LEN - 1, strlen(VAU_DEV_NAME), "%s", VAU_DEV_NAME);
    if (ret <= 0) {
        gpu_err("snprintf_s failed! ret=%d.\n", ret);
        return EXT_FAILURE;
    }

    if (osal_dev_register(&g_vau_register_data) < 0) {
        gpu_err("register vau failed.\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

STATIC td_void vau_unregister_device(td_void)
{
    osal_dev_unregister(&g_vau_register_data);
}

td_s32 uapi_vau_module_init(void)
{
    td_s32 ret;

    ret = vau_mem_init();
    if (ret != EXT_SUCCESS) {
        gpu_err("vau_mem_init failed, ret=%#x!", ret);
        return ret;
    }

    ret = vau_list_init();
    if (ret != EXT_SUCCESS) {
        vau_mem_deinit();
        gpu_err("vau_list_init failed, ret=%#x!", ret);
        return ret;
    }

    ret = vau_register_device();
    if (ret != EXT_SUCCESS) {
        vau_mem_deinit();
        vau_list_deinit();
        gpu_err("vau_register_device failed, ret=%#x!", ret);
        return ret;
    }

    vau_clk_scene_info scene_info = {.cfg_scene = GPU_CLK_CFG_INIT, .work_scene = DRV_VAU_WORK_SCENE_PERFORMANCE_MODE};
    vau_set_clk_work_scene(scene_info.work_scene);
    vau_hal_init(&scene_info);

    ret = vau_isr_init();
    if (ret != EXT_SUCCESS) {
        vau_clk_scene_info deinit_scene_info = {.cfg_scene = GPU_CLK_CFG_INIT, .work_scene = DRV_VAU_WORK_SCENE_BALANCE_MODE};
        vau_hal_deinit(&deinit_scene_info);
        vau_mem_deinit();
        vau_list_deinit();
        vau_unregister_device();
        gpu_err("vau_isr_init fail, ret=%#x!", ret);
        return ret;
    }

    return EXT_SUCCESS;
}

td_void uapi_vau_module_exit(void)
{
    vau_isr_deinit();
    vau_clk_scene_info scene_info = {.cfg_scene = GPU_CLK_CFG_INIT, .work_scene = DRV_VAU_WORK_SCENE_BALANCE_MODE};
    vau_hal_deinit(&scene_info);
    vau_unregister_device();
    vau_list_deinit();
    vau_mem_deinit();
}
