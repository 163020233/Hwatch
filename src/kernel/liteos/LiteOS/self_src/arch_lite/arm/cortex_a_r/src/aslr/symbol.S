/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: ARM Aslr Symbol Info Before Reloc
 * Author: Huawei LiteOS Team
 * Create: 2022-07-15
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

#include "arch/asm.h"

/*
 *           image
 * ┌──────────────────────────┐  ────►   __image_start
 * │                          │
 * │  ┌────────────────────┐  │  ────►   __laslr_start
 * │  │    aslr image      │  │
 * │  │   .laslr.init      │  │
 * │  │   .laslr.text      │  │
 * │  │   .laslr.data      │  │
 * │  └────────────────────┘  │  ────►   __laslr_end
 * │                          │
 * │  ┌────────────────────┐  │  ────►   __startup_start
 * │  │    kernel_startup  │  │
 * │  │                    │  │
 * │  │    main()          │  │
 * │  │    func ...        │  │
 * │  └────────────────────┘  │
 * │                          │
 * │  ┌────────────────────┐  │  ────►  __reldyn_start(reloc info for GOT)
 * │  │                    │  │
 * │  │     .rel.dyn       │  │
 * │  │                    │  │
 * │  └────────────────────┘  │  ────►  __reldyn_end
 * │                          │
 * │  ┌────────────────────┐  │  ────►  __laslr_rel_start(reloc info for GSPR)
 * │  │   .laslr_rel       │  │
 * │  │    g_aslrTableInfo │  │
 * │  │    g_aslrRelInfo   │  │
 * │  └────────────────────┘  │  ────►   __laslr_rel_end
 * │                          │
 * └──────────────────────────┘  ────►   __image_end
 *
 * The following variables must be obtained through the Arch interface,
 * do not directly access variables to obtain data!
 */

 #define ASLR_MAGIC 0x5A5A5C5C

.extern g_aslrCB
.extern __image_start
.extern __image_end
.extern __laslr_start
.extern __laslr_end
.extern __laslr_rel_start
.extern __laslr_rel_end
.extern __startup_start

.section ".laslr.data", "ax"
/* For details, see the ImageLinkInfo structure. */
ImageInfo:
    .long __image_start
    .long __image_end
    .long __laslr_start
    .long __laslr_end
#if defined(LOSCFG_LASLR_GOT)
    .long __reldyn_start
    .long __reldyn_end
#else
    .long __laslr_rel_start
    .long __laslr_rel_end
#endif
    .long __startup_start
    .long ASLR_MAGIC

ASLR_FUNCTION(ArchAslrGetCB)
    ldr r1, =g_aslrCB
    add r0, r0, r1
    bx lr

ASLR_FUNCTION(ArchAslrGetImageLinkInfo)
    ldr r1, =ImageInfo
    add r0, r0, r1
    bx lr

