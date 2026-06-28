/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Arm64 Reliability, Availability and Serviceability HeadFile
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

#ifndef _ARCH_RAS_H
#define _ARCH_RAS_H

#include "arch/regs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ERXADDR_EL1                     s3_0_c5_c4_3
#define ERXMISC1_EL1                    s3_0_c5_c5_1

/* ACTLR_EL3, Auxiliary Control Register, EL3 */
#define ACTLR_EL3                       s3_6_c1_c0_1
#define ACTLR_EL3_READ()                AARCH64_SYSREG_READ(ACTLR_EL3)
#define ACTLR_EL3_WRITE(val)            AARCH64_SYSREG_WRITE(ACTLR_EL3, val)

/* ACTLR_EL2, Auxiliary Control Register, EL2 */
#define ACTLR_EL2                       s3_4_c1_c0_1
#define ACTLR_EL2_READ()                AARCH64_SYSREG_READ(ACTLR_EL2)
#define ACTLR_EL2_WRITE(val)            AARCH64_SYSREG_WRITE(ACTLR_EL2, val)

/* ERRSELR_EL1, Error Record Select Register, EL1 */
#define ERRSELR_EL1                     s3_0_c5_c3_1
#define ERRSELR_EL1_READ()              AARCH64_SYSREG_READ(ERRSELR_EL1)
#define ERRSELR_EL1_WRITE(val)          AARCH64_SYSREG_WRITE(ERRSELR_EL1, val)
/* bit field detail */
#define ERRSELR_BIT_SEL                 0

/* ERXCTLR_EL1, Selected Error Record Control Register, EL1 */
#define ERXCTLR_EL1                     s3_0_c5_c4_1
#define ERXCTLR_EL1_READ()              AARCH64_SYSREG_READ(ERXCTLR_EL1)
#define ERXCTLR_EL1_WRITE(val)          AARCH64_SYSREG_WRITE(ERXCTLR_EL1, val)
/* bit field detail */
#define ERXCTRL_BIT_UI                  2
#define ERXCTRL_BIT_FI                  3
#define ERXCTRL_BIT_CFI                 8
#define ERXCTRL_BIT_ED                  0

/* ERXMISC0_EL1, Selected Error Record Miscellaneous Register 0 */
#define ERXMISC0_EL1                    s3_0_c5_c5_0
#define ERXMISC0_EL1_READ()             AARCH64_SYSREG_READ(ERXMISC0_EL1)
#define ERXMISC0_EL1_WRITE(val)         AARCH64_SYSREG_WRITE(ERXMISC0_EL1, val)

/* ERR0STATUS, Error Record Primary Status Register */
#define ERXSTATUS_EL1                   s3_0_c5_c4_2
#define ERXSTATUS_EL1_READ()            AARCH64_SYSREG_READ(ERXSTATUS_EL1)
#define ERXSTATUS_EL1_WRITE(val)        AARCH64_SYSREG_WRITE(ERXSTATUS_EL1, val)
/* bit field detail */
#define ERXSTATUS_BIT_V                 30
#define ERXSTATUS_BIT_UE                29

/* Selected Error Pseudo Fault Generation Count Down Register */
#define ERXPFGCDNR_EL1                  s3_0_c15_c2_2
#define ERXPFGCDNR_EL1_READ()           AARCH64_SYSREG_READ(ERXPFGCDNR_EL1)
#define ERXPFGCDNR_EL1_WRITE(val)       AARCH64_SYSREG_WRITE(ERXPFGCDNR_EL1, val)

/* Selected Error Pseudo Fault Generation Control Register */
#define ERXPFGCTLR_EL1                  s3_0_c15_c2_1
#define ERXPFGCTLR_EL1_READ()           AARCH64_SYSREG_READ(ERXPFGCTLR_EL1)
#define ERXPFGCTLR_EL1_WRITE(val)       AARCH64_SYSREG_WRITE(ERXPFGCTLR_EL1, val)

#define ERXPFGCTLR_BIT_UC               1
#define ERXPFGCTLR_BIT_CDNEN            31

/* Selected Pseudo Fault Generation Feature Register */
#define ERXPFGFR_EL1                    s3_0_c15_c2_0
#define ERXPFGFR_EL1_READ()             AARCH64_SYSREG_READ(ERXPFGFR_EL1)
#define ERXPFGFR_EL1_WRITE(val)         AARCH64_SYSREG_WRITE(ERXPFGFR_EL1, val)

/*
 * buf store info format
 * -------------------------------------
 * | core0 | core1 | core2 | core3 | scu |
 * -------------------------------------
 */
typedef struct {
    UINT32      statusEl1;
    UINT64      misc0El1;
} RASErrInfo;

extern VOID ArchRasRegInit(VOID);

extern UINT32 ArchRasInfoGet(RASErrInfo *getInfoBuf, UINT32 infoBufSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_RAS_H */