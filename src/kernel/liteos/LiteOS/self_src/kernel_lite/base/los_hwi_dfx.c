/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#include "los_hwi_dfx_pri.h"
#include "los_hwi_pri.h"
#include "los_exc.h"
#include "arch/regs.h"

#define STACK_CANARY_VALUE  0xCCCCCCCC

UINT32 *g_hwiStackCanary = NULL;
UINT32  g_stackCanaryValue = 0;

VOID OsHwiStackProtectInit(VOID)
{
    UINTPTR stackBase;
    UINT32 stackSize;

    /* 获取中断栈信息 */
    /* 栈起始地址(高地址) */
    stackBase = IRQ_STACK_TOP;
    /* 栈大小 */
    stackSize = IRQ_STACK_SIZE;

    /* 计算金丝雀位置(栈的最低地址) */
    g_hwiStackCanary = (UINT32 *)(stackBase - stackSize);

    /* 设置金丝雀值 */
    *g_hwiStackCanary = STACK_CANARY_VALUE;
    g_stackCanaryValue = STACK_CANARY_VALUE;
}

VOID OsHwiStackOverflowHandler(VOID)
{
    UINT32 hwiNum;
    HwiHandleInfo *hwiForm;

    hwiNum = (READ_CSR(mcause)) & 0xffU;

    hwiForm = OsGetHwiForm(hwiNum);

    LOS_Panic("HWI Stack Overflow, irqNum = %d, handle = 0x%x\n", hwiNum, hwiForm->hook);
}