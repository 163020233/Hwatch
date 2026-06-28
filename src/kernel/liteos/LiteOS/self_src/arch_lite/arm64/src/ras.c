/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Arm64 Reliability, Availability and Serviceability Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-12-01
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

#include "arch/ras.h"
#include "arch/cpu.h"

/*
 * Select number of record to containing errors
 */
STATIC INLINE VOID RasRecordSelect(UINT32 record)
{
    if (record == 1) {
        ERRSELR_EL1_WRITE(ERRSELR_EL1_READ() | (1U << ERRSELR_BIT_SEL));
    } else if (record == 0) {
        ERRSELR_EL1_WRITE(ERRSELR_EL1_READ() & (~(1U << ERRSELR_BIT_SEL)));
    }
}

/*
 * Uncorrected error recovery interrupt enable
 */
STATIC INLINE VOID RasUISet(VOID)
{
    ERXCTLR_EL1_WRITE(ERXCTLR_EL1_READ() | (1U << ERXCTRL_BIT_UI));
}

/*
 * Uncorrected error recovery interrupt disable
 */
STATIC INLINE VOID RasUIClear(VOID)
{
    ERXCTLR_EL1_WRITE(ERXCTLR_EL1_READ() & (~(1U << ERXCTRL_BIT_UI)));
}

/*
 * Fault deferred & uncorrected interrupt enabele
 */
STATIC INLINE VOID RasFISet(VOID)
{
    ERXCTLR_EL1_WRITE(ERXCTLR_EL1_READ() | (1U << ERXCTRL_BIT_FI));
}

/*
 * Fault deferred & uncorrected interrupt disabele
 */
STATIC INLINE VOID RasFIClear(VOID)
{
    ERXCTLR_EL1_WRITE(ERXCTLR_EL1_READ() & (~(1U << ERXCTRL_BIT_FI)));
}

/*
 * Fault corrected interrupt enable
 */
STATIC INLINE VOID RasCFISet(VOID)
{
    ERXCTLR_EL1_WRITE(ERXCTLR_EL1_READ() | (1U << ERXCTRL_BIT_CFI));
}

/*
 * Fault corrected interrupt disable
 */
STATIC INLINE VOID RasCFIClear(VOID)
{
    ERXCTLR_EL1_WRITE(ERXCTLR_EL1_READ() & (~(1U << ERXCTRL_BIT_CFI)));
}

/*
 * Error reporting enable
 */
STATIC INLINE VOID RasEDSet(VOID)
{
    ERXCTLR_EL1_WRITE(ERXCTLR_EL1_READ() | (1U << ERXCTRL_BIT_ED));
}

/*
 * Error reporting disable
 */
STATIC INLINE VOID RasEDClear(VOID)
{
    ERXCTLR_EL1_WRITE(ERXCTLR_EL1_READ() & (~(1U << ERXCTRL_BIT_ED)));
}

/*
 * set Error interrupt
 */
STATIC INLINE VOID RasIntSet(VOID)
{
    RasUISet(); /* ERRIRQ */
}

/*
 * set Fault interrupt
 */
STATIC INLINE VOID RasFaultIntSet(VOID)
{
    RasFISet(); /* FAULTIRQ */
    RasCFISet(); /* FAULTIRQ */
}

/*
 * Clear error record
 */
STATIC INLINE VOID RasIntClear(VOID)
{
    ERXSTATUS_EL1_WRITE(ERXSTATUS_EL1_READ() & ~(1U << ERXSTATUS_BIT_UE));
}

STATIC VOID RasCoreInit(VOID)
{
    /* Select record 0 containing errors */
    RasRecordSelect(0);
    /* set error interrupt */
    RasIntSet();
    /* set fault interrupt */
    RasFaultIntSet();
    /* enable error reporting */
    RasEDSet();
}

STATIC VOID RasDsuInit(VOID)
{
    /* Select record 1 containing errors */
    RasRecordSelect(1);
    /* set error interrupt */
    RasIntSet();
    /* set fault interrupt */
    RasFaultIntSet();
    /* enable error reporting */
    RasEDSet();
}

/*
 * Description : Initialize each core's RAS detect function.
 */
VOID ArchRasRegInit(VOID)
{
    if (ArchCurrCpuid() == 0) {
        RasDsuInit();
    }
    RasCoreInit();
}

/*
 * Description : get RAS information from RAS register.
 * Return      : weather or not successfully get RAS info.
 */
UINT32 ArchRasInfoGet(RASErrInfo *getInfoBuf, UINT32 infoBufSize)
{
    if (getInfoBuf == NULL) {
        PRINT_ERR("input pointer is NULL.\n");
        return LOS_NOK;
    }
    if (infoBufSize != ((LOSCFG_KERNEL_CORE_NUM + 1) * sizeof(RASErrInfo))) {
        PRINT_ERR("input Buffer size fault.\n");
        return LOS_NOK;
    }

    RASErrInfo *rasInfo = NULL;

    /* get each core's RAS info, store it to the 1-4th block, depends on the core */
    RasRecordSelect(0);
    rasInfo = &getInfoBuf[ArchCurrCpuid()];
    rasInfo->statusEl1 = (UINT32)ERXSTATUS_EL1_READ();
    rasInfo->misc0El1 = ERXMISC0_EL1_READ();

    /* get shared RAS info between 4 cores, store it to the 5th block */
    RasRecordSelect(1);
    rasInfo = &getInfoBuf[LOSCFG_KERNEL_CORE_NUM];
    rasInfo->statusEl1 = (UINT32)ERXSTATUS_EL1_READ();
    rasInfo->misc0El1 = ERXMISC0_EL1_READ();

    RasIntClear();
    return LOS_OK;
}
