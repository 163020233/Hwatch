/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Mpu Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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

#include "arch/mpu.h"
#include "los_typedef.h"

#define SIZE_4G 0x100000000ULL
#define SIZE_2G 0x80000000ULL

#define MPU_REG32_READ(addr)           (*(volatile UINT32 *)(UINTPTR)(addr))
#define MPU_REG32_WRITE(addr, val)     (*(volatile UINT32 *)(UINTPTR)(addr) = (val))

/*
 * 1: Region has already been used.
 * 0: Region has already been unused.
 */
STATIC UINT8 g_regionNumBeUsed[MPU_MAX_SUPPORT] = {0};

/*
 * Description: get region capacity
 * Input      : regionSize : (region size
 *                           range: MPU_REGION_SIZE_32B ~ MPU_REGION_SIZE_4GB
 *                           formula: capacity = 1ULL << (regionSize + 1);)
 * Return     : Error Information or capacity(unit: byte)
 */
STATIC UINT64 MpuGetRegionCapacity(UINT32 regionSize)
{
    UINT64 capacity;

    if (regionSize == MPU_REGION_SIZE_4GB) {
        return SIZE_4G;
    } else if (regionSize == MPU_REGION_SIZE_2GB) {
        return SIZE_2G;
    }

    capacity = 1UL << (regionSize + 1);

    return capacity;
}

/*
 * Description: check parameter
 * Input      : mpuInfo: MPU info
 * Return     : Error Information or LOS_OK
 */
STATIC UINT32 MpuCheckParame(const MPU_REGION_INFO *mpuInfo)
{
    UINT64 capacity;

    if (mpuInfo == NULL) {
        return LOS_ERRNO_MPU_PTR_NULL;
    }

    /* Judge whether the chip with MPU */
    if (MPU_REG32_READ(MPU_TYPE) == 0) {
        return LOS_ERRNO_MPU_NOT_CONFIGURED;
    }

    /* Number exceeds the maximum value supported by MPU */
    if (mpuInfo->number > (MPU_MAX_SUPPORT - 1)) {
        return LOS_ERRNO_MPU_INVALID_NUMBER;
    }

    /* Selected number region has been enabled */
    if (g_regionNumBeUsed[mpuInfo->number]) {
        return LOS_ERRNO_MPU_REGION_IS_ENABLED;
    }

    if ((mpuInfo->regionSize < MPU_REGION_SIZE_32B) || (mpuInfo->regionSize > MPU_REGION_SIZE_4GB)) {
        return LOS_ERRNO_MPU_INVALID_CAPACITY;
    }

    capacity = MpuGetRegionCapacity(mpuInfo->regionSize);
    if ((mpuInfo->baseAddress % capacity) != 0) {
        return LOS_ERRNO_MPU_INVALID_BASE_ADDRESS;
    }

    if (mpuInfo->accessPermission > MPU_DEFS_RASR_AP_RO) {
        return LOS_ERRNO_MPU_REGION_INVALID_ACCESS;
    }

    return LOS_OK;
}

UINT32 ArchRegionDisable(UINT8 number)
{
    UINT32 intSave;

    if (number > (MPU_MAX_SUPPORT - 1)) {
        return LOS_ERRNO_MPU_INVALID_NUMBER;
    }

    if (g_regionNumBeUsed[number] == 0) {
        return LOS_ERRNO_MPU_REGION_IS_DISABLED;
    }

    intSave = LOS_IntLock();

    MPU_REG32_WRITE(MPU_RNR, number);
    MPU_REG32_WRITE(MPU_RBAR, 0);
    MPU_REG32_WRITE(MPU_RASR, 0);

    g_regionNumBeUsed[number] = 0;

    LOS_IntRestore(intSave);

    return LOS_OK;
}

STATIC VOID MpuDisable(VOID)
{
    /* Make sure outstanding transfers are done */
    DMB();
    MPU_REG32_WRITE(MPU_CTRL, MPU_DISABLE);
}

/*
 * Description: enable mpu
 * Input      : hfnmiena: Whether in the NMI and hard fault service routine is
 *                        not mandatory in addition to MPU
 */
STATIC VOID MpuEnable(BOOL hfnmiena)
{
    UINT32 enable;

    enable = MPU_CTRL_ENABLE_MASK |
             MPU_CTRL_PRIVDEFENA_MASK |
             (hfnmiena << MPU_CTRL_HFNMIENA_IDX);

    MPU_REG32_WRITE(MPU_CTRL, enable);
    /* Ensure MPU settings take effects */
    DSB();
    /* Sequence instruction fetches using update settings */
    ISB();
}

/*
 * Description: mpu config
 * Input      : mpuInfo: MPU info parameters to be set
 *              The base address must be in the range of RAM
 * Return     : LOS_OK or Error Information
 */
STATIC VOID MpuRegionConfig(const MPU_REGION_INFO *mpuInfo)
{
    UINT32 attributeAndCapacity;

    attributeAndCapacity = (mpuInfo->accessPermission << MPU_RASR_AP_IDX) |
                           (mpuInfo->shareable << MPU_RASR_S_IDX) |
                           (mpuInfo->cacheable << MPU_RASR_C_IDX) |
                           (mpuInfo->buffable << MPU_RASR_B_IDX) |
                           (mpuInfo->xn << MPU_RASR_XN_IDX) |
                           (mpuInfo->regionSize << MPU_RASR_SIZE_IDX) |
                           (MPU_RASR_ENABLE_MASK);

    MPU_REG32_WRITE(MPU_RNR, mpuInfo->number);
    MPU_REG32_WRITE(MPU_RBAR, mpuInfo->baseAddress);
    MPU_REG32_WRITE(MPU_RASR, attributeAndCapacity);
}

UINT32 ArchProtectionRegionSet(MPU_REGION_INFO *mpuInfo)
{
    UINT32 ret;
    UINT32 intSave;

    intSave = LOS_IntLock();

    ret = MpuCheckParame(mpuInfo);
    if (ret != LOS_OK) {
        LOS_IntRestore(intSave);
        return ret;
    }

    /* Dual word alignment mode enable stack */
    MPU_REG32_WRITE(SCB_CCR, MPU_REG32_READ(SCB_CCR) | SCB_CCR_STKALIGN_MASK);
    /* enable MemManage fault */
    MPU_REG32_WRITE(SCB_SHCSR, GET_UINT32(SCB_SHCSR) | SCB_SHCSR_MEMFAULTENA_MASK);

    MpuDisable();
    MpuRegionConfig(mpuInfo);
    MpuEnable(mpuInfo->hfnmiena);

    g_regionNumBeUsed[mpuInfo->number] = 1;

    LOS_IntRestore(intSave);

    return LOS_OK;
}
