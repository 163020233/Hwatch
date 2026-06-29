/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Linglong LASLR Reset Vector Implementation
 * Author: Huawei LiteOS Team
 * Create: 2024-07-24
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

#include "asm/asm.h"
#include "asm/memmap_config.h"
#include "arch/exception.h"
#include "arch/asm.h"

.section .laslr.text, "ax"
.align 16
.global reset_vector
.type reset_vector, @function
reset_vector:
/* self-boot procedure */
    /* wait 100 cycles */
    mov.i                   a0, 100
    add.i                   a0, a0, -1
    b.eqz                   a0, reset_vector

    WAIT_BOOT_DONE

boot_done:
    mov.i                   a0, 0x23F00
    mov.ssr.a               sr, a0
    intra                   1

#ifdef LOSCFG_ARCH_FPU_ENABLE
    mov.i                   a0, 0
    mov.late.ssr.a          fpctl, a0
#endif

    /* config aslr stack */
    GET_STACK  __aslr_stack, 512, a1, a2, a3, a4  /* a4: stack bottom */
    PLATFORM_GET_RANDOM_OFFSET(a0)                /* a0: random offset */
    add             a4, a4, a0                    /* a4: stack bottom after randomize */
    mov.ssr.a       sp, a4                        /* set stack pointer */

    mov.i           a1, laslr_start
    add             a1, a1, a0
    j.a             a1                            /* jump to mapped address */

laslr_start:
    call    AspAslrRequired
    b.eqz   a0, aslr_done

    call    AspAslrCacheEnable

    /* do aslr relocation */
    call    OsAslrProcess
    b.neqz  a0, aslr_exc_entry
    /* OsAslrProcess only write itcm backup because of itcm can't access directly, copy itcm now */
    call    AspAslrCopyItcm
    call    AspAslrCacheDisable
    call    AspAslrFinish

aslr_done:
    /* ASLR info shall not cleared cause we still needs to use startup addess while next reboot */
    call    OsGetStartupAddr
    /* jump to main image */
    j.a     a0

aslr_exc_entry:
    b       .

.section ".laslr.stack", "wa"
.align 16
__aslr_stack:
    .skip 512 * CORE_NUM
__aslr_stack_top:
