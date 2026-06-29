/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: RISCV32 Aslr Symbol Info Before Reloc
 * Author: Huawei LiteOS Team
 * Create: 2022-12-20
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

#include "asm/memmap_config.h"

#define ASLR_MAGIC 0x5A5A5C5C

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
 * │  │   .rela.dyn        │  │
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

.extern g_aslrCB

.macro ASLR_FUNCTION param0
    .global \param0;
    .type \param0, @function;
    .section ".laslr.text", "ax";
\param0:
    .option push
    .option norelax
.endm

.macro ASLR_FUNC_END param0
    .option pop
    ret
    .size   \param0 , . - \param0
.endm

/*
 * LOSCFG_LASLR_GOT:the ImageLinkInfo structure is defined in symbol.S.
 * LOSCFG_LASLR_ASLR : the ImageLinkInfo structure is defined in link script
 * It's to solve the problem of OS_ASLR_REL_SECTION_TYPE == LD_SHT_RELA && LOSCFG_LASLR_GOT,
 * (.long __image_start) is stored in the rela entry addend variable,
 * The variables in the ImageLinkInfo structure are all 0, and the corresponding relocation entry needs to be parsed.
 */
#ifdef LOSCFG_LASLR_GSPR
.section ".laslr.data", "ax"
.extern __image_start
.extern __image_end
.extern __laslr_start
.extern __laslr_end
.extern __laslr_rel_start
.extern __laslr_rel_end
.extern __startup_start
ImageInfo:
    .long __image_start
    .long __image_end
    .long __laslr_start
    .long __laslr_end
    .long __laslr_rel_start
    .long __laslr_rel_end
    .long __startup_start
    .long ASLR_MAGIC
#else
    .extern ImageInfo
#endif

ASLR_FUNCTION ArchAslrGetCB
    lla a0, g_aslrCB
ASLR_FUNC_END ArchAslrGetCB

ASLR_FUNCTION ArchAslrGetImageLinkInfo
    lla a0, ImageInfo
ASLR_FUNC_END ArchAslrGetImageLinkInfo
