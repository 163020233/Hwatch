/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: CPU Register Defines Headfile
 * Author: Huawei LiteOS Team
 * Create: 2021-11-18
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

#ifndef _ARCH_REGS_H
#define _ARCH_REGS_H

#ifndef __ASSEMBLER__
#include "asm/register_config.h"
#include "arch/barrier.h"
#include "los_typedef.h"
#endif /* __ASSEMBLER__ */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DSS_PRIVATE_REGS_BASE       (MPSCU_MP_REG_BASE)
#define DSS_SHARED_REGS_BASE        (MPSCU_MP_REG_BASE + 32 * 1024)
#define MP_REGS_BASE                (MPSCU_MP_REG_BASE + 64 * 1024)

#define SCU_REGS_OFFSET             0x5000
#define SCU_REGS_PRIV_BASE          (MPSCU_MP_REG_BASE + SCU_REGS_OFFSET)
#define SCU_REGS_SHARE_BASE         (DSS_SHARED_REGS_BASE + SCU_REGS_OFFSET)
#define SCU_MP_IDS                  (SCU_REGS_PRIV_BASE + 0)
#define SCUP_WAKE_REQ               (SCU_REGS_PRIV_BASE + 0x180)
#define SCU_DSS_STATUS              (SCU_REGS_SHARE_BASE + 0x20)
#define SCU_HW_RESET                (SCU_REGS_SHARE_BASE + 0xA0)

#define SCU_HW_RESET_CORE_RESET     0x1
#define SCU_HW_RESET_NORMAL_RESET   (0x1 << 3)

#define STATUS_CORE_RESET_MASK      (0x1 << 24)
#define STATUS_CORE_MODE_MASK       0x03
#define STATUS_CORE_MODE_WAIT       0x01

#define SCUP_WAKE_REQ_WRITE_ENABLE_FOR_BIT0 0x00010000
#define SCUP_WAKE_REQ_CORE_FORCE_WAKEUP     0x00000001

/* convert DSS register address to MP DSS register space */
#define GLOBAL_REG(core, reg)       ((reg) + 64 * 1024 * ((core) + 1))

#define WRITE_REG(reg, val)         do { *(volatile UINT32 *)(UINTPTR)(reg) = (val); DSB(); } while (0)
#define READ_REG(reg)               ((UINT32)(*(volatile UINT32 *)(UINTPTR)(reg)))


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_REGS_H */
