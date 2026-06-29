/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Arm64 Mmu Hardware Config Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-06-09
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

#include "arch/mmu.h"
#include "arch/mmu_pri.h"
#include "pagetable_manager.h"
#include "arch/regs.h"

UINT64 MmuDecodeFlags(UINT32 osFlags)
{
    UINT64 mmuFlags;
    UINT32 memType;

    /* Unprivileged mode: user el0 mode never execute */
#ifdef LOSCFG_ARCH_SECURE_MONITOR_MODE
    mmuFlags = MMU_PTE_ATTR_AF;
#else
    mmuFlags = MMU_PTE_ATTR_AF | MMU_PTE_ATTR_UXN;
#endif

    /*
     * Unprivileged mode: user el0 mode has no access to mems any time
     * Privileged mode: kernel el123 mode has read/write or read-only access
     */
    mmuFlags |= (osFlags & MMU_FLAGS_RW) ? MMU_PTE_ATTR_AP_P_RW_U_NA : MMU_PTE_ATTR_AP_P_RO_U_NA;

    /* Privileged mode: kernel el123 mode optionally execute */
    mmuFlags |= (osFlags & MMU_FLAGS_EXECUTE_DISABLE) ? MMU_PTE_ATTR_PXN : 0;

    /*
     * Non-secure bit:
     * For memory accesses from Secure state, specifies whether the output address is in Secure or Non-secure memory.
     * For memory accesses from Non-secure state, this bit is RES0 and is ignored by the PE.
     */
    mmuFlags |= (osFlags & MMU_FLAGS_NON_SECURE) ? MMU_PTE_ATTR_NON_SECURE : 0;

    memType = osFlags & MMU_FLAGS_MEM_TYPE_MASK;
    mmuFlags |= MMU_PTE_ATTR_MAIR_INDEX(memType);

    /* Config shareability according to mem type */
    switch (memType) {
        case MMU_MAIR_DEVICE_nGnRnE_IDX:
        case MMU_MAIR_DEVICE_nGnRE_IDX:
            mmuFlags |= MMU_PTE_ATTR_SH_OUTER_SHAREABLE;
            break;
        case MMU_MAIR_NORMAL_NC_IDX:
        case MMU_MAIR_NORMAL_WB_IDX:
            mmuFlags |= MMU_PTE_ATTR_SH_INNER_SHAREABLE;
            break;
        default:
            break;
    }

    return mmuFlags;
}

VOID ArchMmuSwitch(VOID)
{
#ifdef LOSCFG_ARCH_SECURE_MONITOR_MODE
    /* Initialize Memory Attribute Indirection Register */
    AARCH64_SYSREG_WRITE(mair_el3, MMU_MEMORY_TYPES_ATTRS);
    /* Initialize TCR_EL3 */
    /* set cacheable attributes on translation walk */
    /* non-shareable, inner write-back write-allocate */
    AARCH64_SYSREG_WRITE(tcr_el3, MMU_TCR_EL3_CONFIG);
    /* Set translation table base addr */
    AARCH64_SYSREG_WRITE(ttbr0_el3, g_firstPageTable);
#else
    /* Initialize Memory Attribute Indirection Register */
    AARCH64_SYSREG_WRITE(mair_el1, MMU_MEMORY_TYPES_ATTRS);
    /* Initialize TCR_EL1 */
    /* set cacheable attributes on translation walk */
    /* non-shareable, inner write-back write-allocate */
    AARCH64_SYSREG_WRITE(tcr_el1, MMU_TCR_EL1_CONFIG);
#ifdef LOSCFG_ARCH_MMU_KERNEL_SPACE
    /* Set translation table base addr */
    AARCH64_SYSREG_WRITE(ttbr1_el1, (UINTPTR*)MMU_VA_TO_PA(g_firstPageTable));
#else
    /* Set translation table base addr */
    AARCH64_SYSREG_WRITE(ttbr0_el1, (UINTPTR*)MMU_VA_TO_PA(g_firstPageTable));
#endif

#endif

    /* All cores process */
    MmuInvalidLocalTLB();
}

VOID ArchMmuConfig(VOID)
{
    ArchMmuSwitch();
    MmuEnable();
}

STATIC UINTPTR CacheRemap(UINTPTR physAddr, size_t size, UINT32 attrs)
{
#ifndef LOSCFG_ARCH_MMU_VA_MANAGE
    UINT32 ret;
    if (physAddr < SYS_MEM_BASE) {
        return 0;
    }
    ret = ArchMmuUnmap(physAddr, size);
    if (ret != LOS_OK) {
        return 0;
    }
    ret = ArchMmuMap(physAddr, physAddr, size, attrs);
    if (ret != LOS_OK) {
        PRINT_ERR("[%s] failed, ret:0x%x.\n", __FUNCTION__, ret);
        return 0;
    }
    return physAddr;
#else
    (void)physAddr;
    (void)size;
    PRINTK("Warning! OS does not support virtual addresses to be different from physical addresses.\n");
    return 0;
#endif
}

UINTPTR OsCachedRemap(UINTPTR physAddr, size_t size)
{
    return CacheRemap(physAddr, size, MMU_RW_NX_NORMAL_WB);
}

UINTPTR OsNoCachedRemap(UINTPTR physAddr, size_t size)
{
    return CacheRemap(physAddr, size, MMU_RW_NX_NORMAL_NC);
}
