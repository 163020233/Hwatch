/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LingLong Interrupt Controller Driver Implementation
 * Author: Huawei LiteOS Team
 * Create: 2021-11-30
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#include "hal_hwi.h"
#include "los_typedef.h"
#include "los_hwi_pri.h"
#include "los_mp_pri.h"
#include "linglong_ic.h"

STATIC UINT32 g_curIrqNum[LOSCFG_KERNEL_CORE_NUM] = { 0 };
STATIC HwiHandleInfo g_hwiForm[LOSCFG_PLATFORM_HWI_LIMIT] = {{ 0 }};

UINT32 HalIrqMask(UINT32 hwiNum)
{
    UINT32 tmp;
    if (!HWI_NUM_VALID(hwiNum) || (hwiNum == LLIC_INT_ID_NMI)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

#ifdef LOSCFG_KERNEL_SMP
    if (hwiNum <= LLIC_INT_ID_PRIV_MAX) {
        tmp = READ_REG(ICU_MASK(ICU_MASK_N(hwiNum)));
        tmp &= ~(0x01U << (hwiNum % LLIC_REG_WIDTH));
        WRITE_REG(ICU_MASK(ICU_MASK_N(hwiNum)), tmp);
    } else {
        tmp = READ_REG(ICD_MASK(ICD_MASK_N(hwiNum)));
        tmp &= ~(0x01U << (hwiNum % LLIC_REG_WIDTH));
        WRITE_REG(ICD_MASK(ICD_MASK_N(hwiNum)), tmp);
    }
#else
    tmp = READ_REG(ICU_MASK(ICU_MASK_N(hwiNum)));
    tmp &= ~(0x01U << (hwiNum % LLIC_REG_WIDTH));
    WRITE_REG(ICU_MASK(ICU_MASK_N(hwiNum)), tmp);
#endif

    return LOS_OK;
}

UINT32 HalIrqUnmask(UINT32 hwiNum)
{
    UINT32 tmp;

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

#ifdef LOSCFG_KERNEL_SMP
    if (hwiNum <= LLIC_INT_ID_PRIV_MAX) {
        tmp = READ_REG(ICU_MASK(ICU_MASK_N(hwiNum)));
        tmp |= (0x01U << (hwiNum % LLIC_REG_WIDTH));
        WRITE_REG(ICU_MASK(ICU_MASK_N(hwiNum)), tmp);
    } else {
        tmp = READ_REG(ICD_MASK(ICD_MASK_N(hwiNum)));
        tmp |= (0x01U << (hwiNum % LLIC_REG_WIDTH));
        WRITE_REG(ICD_MASK(ICD_MASK_N(hwiNum)), tmp);
    }
#else
    tmp = READ_REG(ICU_MASK(ICU_MASK_N(hwiNum)));
    tmp |= (0x01U << (hwiNum % LLIC_REG_WIDTH));
    WRITE_REG(ICU_MASK(ICU_MASK_N(hwiNum)), tmp);
#endif

    return LOS_OK;
}

UINT32 HalIrqTrigger(UINT32 hwiNum)
{
    /* Software can only trigger SWI */
    if (!HWI_NUM_VALID(hwiNum) || ((hwiNum < LLIC_INTERNAL_SWI_MIN) || (hwiNum > LLIC_INTERNAL_SWI_MAX))) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    WRITE_REG(ICU_SET(ICU_SET_N(hwiNum)), 0x01U << (hwiNum % LLIC_REG_WIDTH));

    return LOS_OK;
}

UINT32 HalIrqClear(UINT32 hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

#ifdef LOSCFG_KERNEL_SMP
    /* SIs are level triggered in linglong, trying to clear level interrupt takes no effect. */
    if (hwiNum <= LLIC_INT_ID_PRIV_MAX) {
        WRITE_REG(ICU_CLEAR(ICU_CLEAR_N(hwiNum)), 0x01U << (hwiNum % LLIC_REG_WIDTH));
    }
#else
    WRITE_REG(ICU_CLEAR(ICU_CLEAR_N(hwiNum)), 0x01U << (hwiNum % LLIC_REG_WIDTH));
#endif
    return LOS_OK;
}

CHAR *HalIrqVersion(VOID)
{
    return "LLIC";
}

UINT32 HalCurIrqGet(VOID)
{
    return g_curIrqNum[ArchCurrCpuid()];
}

STATIC INLINE VOID HalSetPrioReg(UINT32 base, UINT32 hwiNum, UINT32 prio)
{
    UINT32 tmp;
    tmp = READ_REG(base);
    tmp &= ~(LLIC_PER_PRIO_MASK << ((hwiNum % LLIC_PRIO_NUM_PER_REG) * LLIC_PER_PRIO_WIDTH));
    tmp |= prio << ((hwiNum % LLIC_PRIO_NUM_PER_REG) * LLIC_PER_PRIO_WIDTH);
    WRITE_REG(base, tmp);
}

UINT32 HalIrqSetPrio(UINT32 hwiNum, UINT16 priority)
{
    UINT32 archPrio;

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    if (!HWI_PRI_VALID(priority)) {
        return LOS_ERRNO_HWI_PRIO_INVALID;
    }

    /*
     * For LingLong, higher priority number has higher priority.
     * archPrio:
     * 0 = Disabled (invalid param)
     * 1 = lowest priority
     * 15 = highest priority
     */
    archPrio = (UINT32)(LOSCFG_HWI_PRIO_LIMIT - priority);

#ifdef LOSCFG_KERNEL_SMP
    if ((hwiNum <= LLIC_INT_ID_PRIV_MAX) && (hwiNum >= OS_HWI_IPI_BASE)) {
        HalSetPrioReg(ICU_PRIOLEVEL(ICU_PRIOLEVEL_N(hwiNum)), hwiNum, archPrio);
        for (UINT32 cpuId = 0; cpuId < LOSCFG_KERNEL_CORE_NUM; cpuId++) {
            if (cpuId != ArchCurrCpuid()) {
                HalSetPrioReg(GLOBAL_REG(cpuId, ICU_PRIOLEVEL(ICU_PRIOLEVEL_N(hwiNum))), hwiNum, archPrio);
            }
        }
    } else if ((hwiNum <= LLIC_INT_ID_PRIV_MAX) && (hwiNum < OS_HWI_IPI_BASE)) {
        HalSetPrioReg(ICU_PRIOLEVEL(ICU_PRIOLEVEL_N(hwiNum)), hwiNum, archPrio);
    } else {
        HalSetPrioReg(ICD_PRIOLEVEL(ICD_PRIOLEVEL_N(hwiNum)), hwiNum, archPrio);
    }
#else
    HalSetPrioReg(ICU_PRIOLEVEL(ICU_PRIOLEVEL_N(hwiNum)), hwiNum, archPrio);
#endif

    return LOS_OK;
}

STATIC HwiHandleInfo *HalIrqGetHandleForm(HWI_HANDLE_T hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return NULL;
    }
    return &g_hwiForm[hwiNum];
}

STATIC INLINE UINT32 HalGetIntId(void)
{
    UINT32 intId;
    __asm__ __volatile__ (
        "get.irq %0 \n"
        "bextu.i %0, %0, 16, 11\n"
        : "=r"(intId)
    );
    return intId;
}

STATIC INLINE VOID HalClearSI(void)
{
    UINT32 hwiNum;
    __asm__ __volatile__ (
        "get.irq %0 \n"
        "{ st.clear.isr %0, 1 ; sync.str }"
        : "=r"(hwiNum)
    );
}

STATIC VOID HalIrqHandler(VOID)
{
    UINT32 hwiNum = HalGetIntId();
    if (hwiNum >= LOSCFG_PLATFORM_HWI_LIMIT) {
        return;
    }

    g_curIrqNum[ArchCurrCpuid()] = hwiNum;
    OsIntHandle(hwiNum, &g_hwiForm[hwiNum]);
    HalClearSI();

    return;
}

#ifdef LOSCFG_KERNEL_SMP
UINT32 HalIrqSendIpi(UINT32 cpuMask, UINT32 ipi)
{
    if ((ipi < LLIC_INTERNAL_SWI_MIN) || (ipi > LLIC_INTERNAL_SWI_MAX)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    cpuMask = cpuMask & ICD_TARGET_MASK;
    WRITE_REG(ICD_SW_INT_GEN, (cpuMask << ICD_SW_GEN_TARGET_POS) | ipi);

    return LOS_OK;
}

UINT32 HalIrqSetAffinity(UINT32 hwiNum, UINT32 cpuMask)
{
    UINT32 tmp;
    UINT32 offset;

    if (!HWI_NUM_VALID(hwiNum) || (hwiNum < LLIC_EXTERNAL_SI_MIN)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    if (((cpuMask & LOSCFG_KERNEL_CPU_MASK) != cpuMask) || ((ICD_TARGET_MASK & cpuMask) != cpuMask)) {
        return LOS_ERRNO_HWI_AFFI_INVALID;
    }

    offset = ICD_TARGET_OFFSET(hwiNum);
    tmp = READ_REG(ICD_TARGET(ICD_TARGET_N(hwiNum)));
    tmp &= ~(ICD_TARGET_MASK << offset);
    tmp |= (cpuMask & ICD_TARGET_MASK) << offset;
    WRITE_REG(ICD_TARGET(ICD_TARGET_N(hwiNum)), tmp);

    return LOS_OK;
}
#endif

STATIC VOID IcuInit(VOID)
{
    /* disable and clear all PIs */
    for (UINT32 i = 0; i <= LLIC_INT_ID_PRIV_MAX / LLIC_REG_WIDTH; i++) {
        WRITE_REG(ICU_MASK(i), 0);
        WRITE_REG(ICU_CLEAR(i), ICU_CLEAR_ALL_MASK);
    }

#ifdef LOSCFG_KERNEL_SMP
    UINT32 tmp;
    /* forward SI to this core */
    tmp = READ_REG(ICU_MASK(0));
    WRITE_REG(ICU_MASK(0), tmp | 0x1);
#endif
}

#ifdef LOSCFG_KERNEL_SMP
STATIC UINT32 IcdInit(VOID)
{
    UINT32 ret;
    /* disable and clear all SIs */
    for (int i = LLIC_EXTERNAL_SI_MIN; i <= LLIC_EXTERNAL_SI_MAX; i++) {
        ret = HalIrqMask(i);
        if (ret != LOS_OK) {
            return ret;
        }

        ret = HalIrqClear(i);
        if (ret != LOS_OK) {
            return ret;
        }

        HalIrqSetAffinity(i, OS_MP_CPU_ALL);
    }
    return LOS_OK;
}
#endif

VOID HalIrqInitPercpu(VOID)
{
    IcuInit();

#ifdef LOSCFG_KERNEL_SMP
    UINT32 ret = LOS_OK;
    /* enable IPIs */
    ret |= HalIrqSetPrio(LOS_MP_IPI_WAKEUP, OS_HWI_PRIO_LOWEST);
    ret |= HalIrqSetPrio(LOS_MP_IPI_SCHEDULE, OS_HWI_PRIO_LOWEST);
    ret |= HalIrqSetPrio(LOS_MP_IPI_HALT, OS_HWI_PRIO_LOWEST);
    ret |= HalIrqUnmask(LOS_MP_IPI_WAKEUP);
    ret |= HalIrqUnmask(LOS_MP_IPI_SCHEDULE);
    ret |= HalIrqUnmask(LOS_MP_IPI_HALT);
#ifdef LOSCFG_KERNEL_SMP_CALL
    ret |= HalIrqSetPrio(LOS_MP_IPI_FUNC_CALL, OS_HWI_PRIO_LOWEST);
    ret |= HalIrqUnmask(LOS_MP_IPI_FUNC_CALL);
#endif
    if (ret != LOS_OK) {
        PRINT_ERR("Enable IPIs failed, ret:0x%x\n", ret);
    }
#endif
}

#ifdef LOSCFG_KERNEL_LOWPOWER

#define LLIC_REG_NUM(irqPerReg)                 ((LOSCFG_PLATFORM_HWI_LIMIT + (irqPerReg) - 1) / (irqPerReg))
#define ICU_SAVE_MASK_REG_NUM                   LLIC_REG_NUM(32)
#define ICU_SAVE_PRIO_REG_NUM                   LLIC_REG_NUM(8)
#define ICD_SAVE_MASK_REG_NUM                   LLIC_REG_NUM(32)
#define ICD_SAVE_PRIO_REG_NUM                   LLIC_REG_NUM(8)
#define ICD_SAVE_TARGET_REG_NUM                 LLIC_REG_NUM(2)

typedef struct IntRegConfig {
    UINT32 icuMask[ICU_SAVE_MASK_REG_NUM];
    UINT32 icuPrio[ICU_SAVE_PRIO_REG_NUM];
#ifdef LOSCFG_KERNEL_SMP
    UINT32 icdMask[ICD_SAVE_MASK_REG_NUM];
    UINT32 icdPrio[ICD_SAVE_PRIO_REG_NUM];
    UINT32 icdTarget[ICD_SAVE_TARGET_REG_NUM];
#endif
} IntRegConfig;

STATIC IntRegConfig g_intRegConfig = {{0}};

STATIC VOID HalIrqConfigDoSave(UINT32 *regBase, UINT32 *dest, UINT32 num)
{
    for (UINT32 idx = 0; idx < num; idx++) {
        dest[idx] = READ_REG(&regBase[idx]);
    }
}

STATIC VOID HalIrqConfigDoRestore(UINT32 *regBase, UINT32 *dest, UINT32 num)
{
    for (UINT32 idx = 0; idx < num; idx++) {
        WRITE_REG(&regBase[idx], dest[idx]);
    }
}

VOID HalIrqConfigSave(VOID)
{
    HalIrqConfigDoSave((UINT32 *)ICU_MASK_BASE, g_intRegConfig.icuMask, ICU_SAVE_MASK_REG_NUM);
    HalIrqConfigDoSave((UINT32 *)ICU_PRIOLEVEL_BASE, g_intRegConfig.icuPrio, ICU_SAVE_PRIO_REG_NUM);
#ifdef LOSCFG_KERNEL_SMP
    HalIrqConfigDoSave((UINT32 *)ICD_MASK_BASE, g_intRegConfig.icdMask, ICD_SAVE_MASK_REG_NUM);
    HalIrqConfigDoSave((UINT32 *)ICD_PRIOLEVEL_BASE, g_intRegConfig.icdPrio, ICD_SAVE_PRIO_REG_NUM);
    HalIrqConfigDoSave((UINT32 *)ICD_TARGET_BASE, g_intRegConfig.icdTarget, ICD_SAVE_TARGET_REG_NUM);
#endif
}

VOID HalIrqConfigRestore(VOID)
{
    HalIrqConfigDoRestore((UINT32 *)ICU_MASK_BASE, g_intRegConfig.icuMask, ICU_SAVE_MASK_REG_NUM);
    HalIrqConfigDoRestore((UINT32 *)ICU_PRIOLEVEL_BASE, g_intRegConfig.icuPrio, ICU_SAVE_PRIO_REG_NUM);
#ifdef LOSCFG_KERNEL_SMP
    HalIrqConfigDoRestore((UINT32 *)ICD_MASK_BASE, g_intRegConfig.icdMask, ICD_SAVE_MASK_REG_NUM);
    HalIrqConfigDoRestore((UINT32 *)ICD_PRIOLEVEL_BASE, g_intRegConfig.icdPrio, ICD_SAVE_PRIO_REG_NUM);
    HalIrqConfigDoRestore((UINT32 *)ICD_TARGET_BASE, g_intRegConfig.icdTarget, ICD_SAVE_TARGET_REG_NUM);
#endif
}

#endif

STATIC const HwiControllerOps g_llicOps = {
    .triggerIrq         = HalIrqTrigger,
    .clearIrq           = HalIrqClear,
    .enableIrq          = HalIrqUnmask,
    .disableIrq         = HalIrqMask,
    .setIrqPriority     = HalIrqSetPrio,
    .getCurIrqNum       = HalCurIrqGet,
    .getIrqVersion      = HalIrqVersion,
    .getHandleForm      = HalIrqGetHandleForm,
    .handleIrq          = HalIrqHandler,
#ifdef LOSCFG_KERNEL_SMP
    .sendIpi            = HalIrqSendIpi,
    .setIrqCpuAffinity  = HalIrqSetAffinity,
#endif
};

VOID HalIrqInit(VOID)
{
#ifdef LOSCFG_KERNEL_SMP
    UINT32 ret;
    ret = IcdInit();
    if (ret != LOS_OK) {
        PRINT_ERR("IcdInit failed, 0x%x\n", ret);
        return;
    }
#endif

    HalIrqInitPercpu();

    OsHwiControllerReg(&g_llicOps);

#ifdef LOSCFG_KERNEL_SMP
    /* register inter-processor interrupt */
    ret = LOS_HwiCreate(LOS_MP_IPI_WAKEUP, OS_HWI_PRIO_LOWEST, 0, OsMpWakeHandler, 0);
    ret |= LOS_HwiCreate(LOS_MP_IPI_SCHEDULE, OS_HWI_PRIO_LOWEST, 0, OsMpScheduleHandler, 0);
    ret |= LOS_HwiCreate(LOS_MP_IPI_HALT, OS_HWI_PRIO_LOWEST, 0, OsMpHaltHandler, 0);
#ifdef LOSCFG_KERNEL_SMP_CALL
    ret |= LOS_HwiCreate(LOS_MP_IPI_FUNC_CALL, OS_HWI_PRIO_LOWEST, 0, OsMpFuncCallHandler, 0);
#endif
    if (ret != LOS_OK) {
        PRINT_ERR("LOS_HwiCreate failed, ret:0x%x\n", ret);
    }
#endif
}
