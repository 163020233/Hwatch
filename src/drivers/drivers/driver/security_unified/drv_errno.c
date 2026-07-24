/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: DFX error-chain implementation for security_unified.
 */
#include "crypto_osal_lib.h"
#include "drv_errno.h"

int32_t drv_err_top = -1;
uint32_t drv_err_stack[DRV_ERR_STACK_DEPTH];

void drv_set_err_info(uint32_t err_code)
{
    uint32_t return_pc = (uint32_t)(uintptr_t)__builtin_return_address(0);
    uint32_t err_info = drv_error_pc(return_pc, err_code);

    if (drv_err_top < (DRV_ERR_STACK_DEPTH - 1)) {
        drv_err_top++;
        drv_err_stack[drv_err_top] = err_info;
    }
}

void drv_print_err_chain(void)
{
    int32_t index;
    /* All security_unified call sites must share this 16 MiB region. */
    uint32_t pc_region = (uint32_t)(uintptr_t)&drv_print_err_chain & DRV_ERR_PC_REGION_MASK;

    if (drv_err_top >= 0) {
        osal_printk("[DRV_ERR] Error chain (top=%d):\n", drv_err_top);
        for (index = drv_err_top; index >= 0; index--) {
            uint32_t err_val = drv_err_stack[index];
            uint32_t return_pc = pc_region | drv_err_get_return_pc_low(err_val);
            osal_printk("[DRV_ERR] [%d] return_pc=0x%08X err_code=0x%02X\n",
                index, (unsigned int)return_pc,
                (unsigned int)drv_err_get_errcode(err_val));
        }
    }
    drv_err_top = -1;
}
