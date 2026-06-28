/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2024. All rights reserved.
 * Description: Linglong Reset Vector Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-01-05
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

#ifdef LOSCFG_LASLR
#define RESET_HANDLER     reset_handler
#else
#define RESET_HANDLER     reset_vector
#endif

.section .ResetVector.text, "ax"
.align 16
.global RESET_HANDLER
.type RESET_HANDLER, @function
RESET_HANDLER:
/* self-boot procedure */
    /* wait 100 cycles */
    mov.i                   a0, 100
    add.i                   a0, a0, -1
    b.eqz                   a0, RESET_HANDLER

    WAIT_BOOT_DONE

boot_done:
#ifdef LOSCFG_KERNEL_DEEPSLEEP
    GET_STACK               __stack, STACK_SIZE, a1, a2, a3, a0
    mov.ssr.a               sp, a0
    call                    OsLowpowerWakeupFromReset
#endif

    mov.i                   a0, 0x23F00
    mov.ssr.a               sr, a0
    intra                   1

    /* set vector base address (vecbase) */
    mov.i                   a0, exception_handlers
    mov.late.ssr.a          vecbase, a0
    sync

#ifdef LOSCFG_ARCH_FPU_ENABLE
    mov.i a0, 0
    mov.late.ssr.a fpctl, a0
#endif

    /* initialize sp, then subroutine calling is available.
     * NOTE: SOUP is NOT enabled here because interrupt handling is not initialized yet.
     */
    GET_STACK __stack, STACK_SIZE, a1, a2, a3, a0
    mov.ssr.a               sp, a0
#ifdef LOSCFG_ARCH_SOUP_ENABLE
    mov.late.ssr.a sup_lspb, a3
    mov.late.ssr.a sup_uspb, a0
#endif

    j                       cpu_start   /* jump to mapped address */

cpu_start:
    /* get core id */
    GET_CORE_ID             a0

    /* assuming CORE 0 is primary core */
    tst.eqz                 a0
    b.ct                    _primaryInit
    b                       _secondaryInit

_secondaryInit:
    wait
_primaryInit:
    call                    main
    b                       .
