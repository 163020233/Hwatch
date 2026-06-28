/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL ipc \n
 *
 * History: \n
 * 2022-11-16, Create file. \n
 */

#include "common_def.h"
#include "ipc_porting.h"
#include "hal_ipc_v150_regs_def.h"
#include "hal_ipc.h"

ipc_soft_int_ctrl_t *g_ipc_soft_int_regs = NULL;
static hal_ipc_funcs_t *g_hal_ipc_funcs = NULL;

errcode_t hal_ipc_v150_regs_init(void)
{
    if (ipc_porting_base_addr_get() == 0) {
        return ERRCODE_IPC_REG_ADDR_INVALID;
    }
    g_ipc_soft_int_regs = (ipc_soft_int_ctrl_t *)ipc_porting_base_addr_get();
    return ERRCODE_SUCC;
}

void hal_ipc_v150_regs_deinit(void)
{
    g_ipc_soft_int_regs = NULL;
}

errcode_t hal_ipc_register_funcs(hal_ipc_funcs_t *funcs)
{
    if (funcs == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_ipc_funcs = funcs;
    return ERRCODE_SUCC;
}

void hal_ipc_unregister_funcs(void)
{
    g_hal_ipc_funcs = NULL;
}

hal_ipc_funcs_t *hal_ipc_get_funcs(void)
{
    return g_hal_ipc_funcs;
}