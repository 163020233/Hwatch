/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Super interrupt controller (SIC).
 * Author: Huawei LiteOS Team
 * Create: 2023-09-15
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

#include "sic.h"
#include "gic_common.h"
#include "gic_v3.h"

VOID HalSicrIrqMask(UINT32 hwiNum)
{
    UINT32 i;
    UINT32 offset;
    const UINT32 mask = 1U << (hwiNum % INT_COUNTS_32_PER_REG);

    offset = (hwiNum >> BIT_SHIFT_5) * sizeof(UINT32);
    for (i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
        GIC_REG_32(GICR_ICENABLER0(i + GICR_CLUSTER_CPU_ID_OFFSET) + offset) = mask;
        GicWaitForRwp(GICR_CTLR(i + GICR_CLUSTER_CPU_ID_OFFSET));
    }
}

VOID HalSicrIrqUnmask(UINT32 hwiNum)
{
    UINT32 i;
    UINT32 offset;
    const UINT32 mask = 1U << (hwiNum % INT_COUNTS_32_PER_REG);

    offset = (hwiNum >> BIT_SHIFT_5) * sizeof(UINT32);
    for (i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
        GIC_REG_32(GICR_ISENABLER0(i + GICR_CLUSTER_CPU_ID_OFFSET) + offset) = mask;
        GicWaitForRwp(GICR_CTLR(i + GICR_CLUSTER_CPU_ID_OFFSET));
    }
}

VOID HalSicrTrigger(UINT32 hwiNum)
{
    UINT32 offset = (hwiNum >> BIT_SHIFT_5) * 4;
    const UINT32 mask = 1U << (hwiNum % INT_COUNTS_32_PER_REG);
    UINT32 cpuId = GicrCpuIdGet();
    GIC_REG_32(GICR_ISPENDR0(cpuId) + offset) = mask;
}

UINT32 SicrGetEnableStatus(HWI_HANDLE_T hwiNum)
{
    UINT32 cpuId = GicrCpuIdGet();
    UINT32 maskPos = hwiNum % INT_COUNTS_32_PER_REG;
    UINT32 offset = (hwiNum >> BIT_SHIFT_5) * sizeof(UINT32);
    return ((GIC_REG_32(GICR_ISENABLER0(cpuId + GICR_CLUSTER_CPU_ID_OFFSET)  + offset) >> maskPos) & 1U);
}

UINT32 SicrGetPendingStatus(HWI_HANDLE_T hwiNum)
{
    UINT32 cpuId = GicrCpuIdGet();
    UINT32 maskPos = hwiNum % INT_COUNTS_32_PER_REG;
    UINT32 offset = (hwiNum >> BIT_SHIFT_5) * 4;

    return ((GIC_REG_32(GICR_ISPENDR0(cpuId)  + offset) >> maskPos) & 1U);
}

STATIC INLINE VOID SicrSetGroup(UINT32 cpu, UINT32 offset)
{
    /* configure sgi/ppi as group 0 on secure mode and group 1 on insecure mode */
    GicIntGroupSet(GICR_IGRPMOD0(cpu) + offset, GICR_IGROUPR0(cpu) + offset);
}

VOID HalSicrInitPercpu(VOID)
{
    UINT32 idx;
    UINT32 offset;
    UINT32 cpu = GicrCpuIdGet();

    /* config distributer, mask and clear all spis, set group x */
    for (idx = 0; idx < GIC_MIN_SPI_NUM; idx += INT_COUNTS_32_PER_REG) {
        offset = (idx >> BIT_SHIFT_5) * sizeof(UINT32);
        SicrSetGroup(cpu, offset);
    }
    GicWaitForRwp(GICR_CTLR(cpu));

    for (idx = 0; idx < GIC_MIN_SPI_NUM; idx += INT_COUNTS_32_PER_REG) {
        /* GICR: clear and mask sgi/ppi */
        offset = (idx >> BIT_SHIFT_5) * sizeof(UINT32);
        GIC_REG_32(GICR_ICENABLER0(cpu) + offset) = 0xffffffff;
        GIC_REG_32(GICR_ICPENDR0(cpu) + offset) = 0xffffffff;
        GIC_REG_32(GICR_ISENABLER0(cpu) + offset) = 0xffffffff;
    }
}

