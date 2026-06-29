/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_intf.c
 * Author: AuthorNameMagicTag
 * Create: 2025-06-03
 */

#include "drv_mcp_dependency.h"
#include "drv_viu_ioctl.h"
#include "drv_ioctl_viu.h"
#include "viu_vicap.h"
#include "memory_config.h"

static td_bool g_viu_open = TD_FALSE;

static mcp_osal_ioctl_cmd g_viu_ioctl_cmd_list[] = {
    { VIU_INIT_CTRL,                    viu_ioctl_drv_init },
    { VIU_DEINIT_CTRL,                  viu_ioctl_drv_deinit },

    { VIU_DEV_ATTR_SET_CTRL,            viu_ioctl_dev_set_attr },
    { VIU_DEV_ATTR_GET_CTRL,            viu_ioctl_dev_get_attr },

    { VIU_DEV_ENABLE_CTRL,              viu_ioctl_dev_enable },
    { VIU_DEV_DISABLE_CTRL,             viu_ioctl_dev_disable },

    { VIU_DEV_SET_TIMING_ATTR_CTRL,     viu_ioctl_set_dev_timing_attr },
    { VIU_DEV_GET_TIMING_ATTR_CTRL,     viu_ioctl_get_dev_timing_attr },

    { VIU_DEV_ATTACH,                   viu_ioctl_dev_attach_pipe },
    { VIU_DEV_DETACH,                   viu_ioctl_dev_detach_pipe },

    { VIU_PIPE_SET_WORK_MODE,           viu_ioctl_pipe_set_work_mode},
    { VIU_PIPE_GET_WORK_MODE,           viu_ioctl_pipe_get_work_mode},

    { VIU_PIPE_CREATE_CTRL,             viu_ioctl_pipe_create},
    { VIU_PIPE_DESTROY_CTRL,            viu_ioctl_pipe_destroy},

    { VIU_PIPE_ATTR_SET_CTRL,           viu_ioctl_pipe_set_attr},
    { VIU_PIPE_ATTR_GET_CTRL,           viu_ioctl_pipe_get_attr},

    { VIU_PIPE_START_CTRL,              viu_ioctl_pipe_start},
    { VIU_PIPE_STOP_CTRL,               viu_ioctl_pipe_stop},

    { VIU_PIPE_DUMP_ATTR_SET_CTRL,      viu_ioctl_set_pipe_dump_attr},
    { VIU_PIPE_DUMP_ATTR_GET_CTRL,      viu_ioctl_get_pipe_dump_attr},

    { VIU_PIPE_RAW_GET_CTRL,            viu_ioctl_get_pipe_raw},
    { VIU_MOD_FRAME_RELEASE_CTRL,       viu_ioctl_mod_release_frame},

    {VIU_PIPE_ATTACH,                   viu_ioctl_chn_attach},
    {VIU_PIPE_DETACH,                   viu_ioctl_chn_detach},
};

td_s32 viu_ioctl(td_s32 module_idx, td_ulong cmd, td_void *para)
{
    td_u32 i;
    td_u32 max_cmd_num = sizeof(g_viu_ioctl_cmd_list) / sizeof(mcp_osal_ioctl_cmd);
    mcp_osal_ioctl_cmd *ioctl_cmd = TD_NULL;

    if (g_viu_open != TD_TRUE) {
        osal_printk("viu not init!\n");
        return TD_FAILURE;
    }

    for (i = 0; i < max_cmd_num; i++) {
        if (g_viu_ioctl_cmd_list[i].cmd == cmd) {
            ioctl_cmd = &g_viu_ioctl_cmd_list[i];
            break;
        }
    }

    if (ioctl_cmd == TD_NULL) {
        osal_printk("ioctl cmd does not exist!\n");
        return TD_FAILURE;
    }

    return ioctl_cmd->handler(cmd, para, &module_idx);
}

static int viu_mem_pool_init(void)
{
    td_u32 mem_length;
    td_u32 mem_start_addr;
    int ret = 0;

    mem_start_addr = uapi_sys_get_memory_pool_addr(MEM_POOL_VICAP);
    mem_length = uapi_sys_get_memory_pool_length(MEM_POOL_VICAP);

    ret = osal_pool_mem_init((void *)mem_start_addr, mem_length);
    if (ret != 0) {
        viu_err_trace("===> vicap osal_pool_mem_init init failed! 0x%x===> ADDR 0x%x, size 0x%x\n",
            ret, mem_start_addr, mem_length);
        return TD_FAILURE;
    }
    osal_printk("vicap mem init succ!\n");
    return ret;
}

static void viu_mem_pool_deinit(void)
{
    td_u32 mem_start_addr = uapi_sys_get_memory_pool_addr(MEM_POOL_VICAP);
    if (osal_pool_mem_deinit((void *)mem_start_addr) != 0) {
        viu_err_trace("===> vicap osal_pool_mem deinit failed! ===> ADDR 0x%x\n",
            mem_start_addr);
        return;
    }
}

td_s32 viu_do_mod_init(td_void)
{
    if (viu_mem_pool_init() != 0) {
        return TD_FAILURE;
    }

    viu_set_default_mod_param();

    if (viu_synch_ctx_init() != TD_SUCCESS) {
        osal_printk("viu atomic init failed!\n");
        viu_mem_pool_deinit();
        return TD_FAILURE;
    }

    g_viu_open = TD_TRUE;
    osal_printk("load viu.ko ...OK!\n");
    return TD_SUCCESS;
}

td_void viu_do_mod_exit(td_void)
{
    viu_synch_ctx_exit();

    g_viu_open = TD_FALSE;

    viu_mem_pool_deinit();

    osal_printk("unload viu.ko ...OK!\n");
    return;
}
