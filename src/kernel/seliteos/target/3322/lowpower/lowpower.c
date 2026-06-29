/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: lowpower source file for SELiteOS.
 */
#include "stdint.h"
#include "chips_porting.h"
#include "chip_io.h"
#include "asm/register_config.h"
#include "rapc.h"
#include "fapc.h"
#include "lowpower.h"

#define CORE_SUSPEND_REGS_CNT                        93
#define START_PC_REG_LEN                             16
#define COM_CTL_RB_CFG_COM_GLB_CLKEN_SEC_OFFSET      0
#define COM_CTL_RB_CFG_COM_GLB_CLKEN_XIP_QSPI_OFFSET 7

uint32_t g_cpu_suspend_regs[CORE_SUSPEND_REGS_CNT] = { 0 };

static void sec_router_and_clock_init(void)
{
    /* init COM_CTRL SEC_CR and C_CRG_CLKEN */
    reg16_clrbit(COM_CTL_RB_C_CRG_CLKEN_REG, COM_CTL_RB_CFG_COM_GLB_CLKEN_SEC_OFFSET);
    writew(COM_CTL_RB_SEC_CR_REG, 1); // 1: enable sec clock router control
    reg16_setbit(COM_CTL_RB_C_CRG_CLKEN_REG, COM_CTL_RB_CFG_COM_GLB_CLKEN_SEC_OFFSET);

    // init COM_CTRL XIP_QSPI_CR and GLB_CLKEN_REG
    reg16_clrbit(COM_CTL_RB_GLB_CLKEN_REG, COM_CTL_RB_CFG_COM_GLB_CLKEN_XIP_QSPI_OFFSET);
    writew(COM_CTL_RB_XIP_QSPI_CR_REG, 1); // 1: enable flash clock router control
    reg16_setbit(COM_CTL_RB_GLB_CLKEN_REG, COM_CTL_RB_CFG_COM_GLB_CLKEN_XIP_QSPI_OFFSET);
}

void lowpower_resume(void)
{
    sec_router_and_clock_init();
    tee_rapc_init();
    tee_fapc_init();
    tee_flash_online_decryption_config();

    /* tee vote to disallow deepsleep */
    writew(TEE_CTL_RB_TEE_DEEPSLEEP_VOTE_REG, 0);
}

void lowpower_suspend(void)
{
    /* set reset_pc = tee_lowpower_resume */
    writew(TEE_CTL_RB_TEE_START_PC_LOCK_REG, 0);
    writew(TEE_CTL_RB_TEE_START_PC_H_REG, (uint16_t)((uint32_t)(uintptr_t)tee_lowpower_resume >> START_PC_REG_LEN));
    writew(TEE_CTL_RB_TEE_START_PC_L_REG, (uint16_t)((uint32_t)(uintptr_t)tee_lowpower_resume));
    writew(TEE_CTL_RB_TEE_START_PC_LOCK_REG, 1);

    /* tee vote to deepsleep */
    writew(TEE_CTL_RB_TEE_DEEPSLEEP_VOTE_REG, 1);
}
