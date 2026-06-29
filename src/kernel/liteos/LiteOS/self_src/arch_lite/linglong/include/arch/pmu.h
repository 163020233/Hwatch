/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS Perf LingLong Pmu Module HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2022-07-18
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

#ifndef _ARCH_PMU_H
#define _ARCH_PMU_H

#include "arch/regs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* OCD Registers Mapping */
#define OCD_BASE                            (DSS_PRIVATE_REGS_BASE + 0x1000)

#define OCD_DPCNT_BASE                      (OCD_BASE + 0x518)
#define OCD_DPESEL_BASE                     (OCD_BASE + 0x544)

#define OCD_DPCTL                           (OCD_BASE + 0x500)
#define OCD_DPSTAT                          (OCD_BASE + 0x504)
#define OCD_DPCLR                           (OCD_BASE + 0x508)
#define OCD_DPCNT(n)                        (OCD_DPCNT_BASE + (n) * 4)
#define OCD_DPESEL(n)                       (OCD_DPESEL_BASE + (n) * 4)

/* Hardware Event ID */
#define PMU_EVENT_CORE_CLOCK                                            86
#define PMU_EVENT_VLIW_EXECUTED                                         87
#define PMU_EVENT_DCACHE_CACHEABLE_LOAD_A                               10
#define PMU_EVENT_DCACHE_CACHEABLE_LOAD_B                               24
#define PMU_EVENT_DCACHE_MISS_A                                         12
#define PMU_EVENT_DCACHE_MISS_B                                         26
#define PMU_EVENT_ICACHE_CACHEABLE_ACCESS                                1
#define PMU_EVENT_ICACHE_MISS                                            3
#define PMU_EVENT_PREDICTABLE_COF_INSTRUCTIONS                          58
#define PMU_EVENT_MISPREDICTED_COF_INSTRUCTIONS                         61

#define PMU_MAX_COUNTER_NUM                                             6

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_PMU_H */