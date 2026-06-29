/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ARM Aslr Symbol Info Before Reloc
 * Author: Huawei LiteOS Team
 * Create: 2023-10-15
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
 * The following variables must be obtained through the Arch interface,
 * do not directly access variables to obtain data!
 */

 #define ASLR_MAGIC 0x5A5A5C5C

.extern g_aslrCB
.extern __image_start
.extern __image_end
.extern __laslr_start
.extern __laslr_end
.extern __reldyn_start
.extern __reldyn_end
.extern __startup_start

.section ".laslr.data", "ax"
ImageInfo:
    .quad __image_start
    .quad __image_end
    .quad __laslr_start
    .quad __laslr_end
    .quad __reldyn_start
    .quad __reldyn_end
    .quad __startup_start
    .long  ASLR_MAGIC

ASLR_FUNCTION(ArchAslrGetCB)
    ldr x1, =g_aslrCB
    add x0, x0, x1
    ret

ASLR_FUNCTION(ArchAslrGetImageLinkInfo)
    ldr x1, =ImageInfo
    add x0, x0, x1
    ret
