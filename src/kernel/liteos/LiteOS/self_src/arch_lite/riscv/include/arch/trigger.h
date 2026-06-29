/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : LiteOS PMP Module Headfile.
 * Author: Huawei LiteOS Team
 * Create : 2022-12-20
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
 * ---------------------------------------------------------------------------- */
#ifndef _ARCH_TRIGGER_H
#define _ARCH_TRIGGER_H

#define TRIGGER_SELECT      tselect
#define TRIGGER_TDATA1      tdata1
#define TRIGGER_TDATA2      tdata2
#define TRIGGER_MCTRL       mcontrol
#define TRIGGER_ETRIGGER    etrigger

#define TRIGGER_TDATA1_TYPE_BIT         (28)
#define TRIGGER_TDATA1_DMODE_BIT        (27)
#define TRIGGER_TDATA1_MASKMAX_BIT      (21)
#define TRIGGER_TDATA1_HIT_BIT          (20)
#define TRIGGER_TDATA1_SELECT_BIT       (19)
#define TRIGGER_TDATA1_TIMING_BIT       (18)
#define TRIGGER_TDATA1_SIZELO_BIT       (16)
#define TRIGGER_TDATA1_ACTION_BIT       (12)
#define TRIGGER_TDATA1_CHAIN_BIT        (11)
#define TRIGGER_TDATA1_MATCH_BIT        (7)
#define TRIGGER_TDATA1_M_MODE_BIT       (6)
#define TRIGGER_TDATA1_S_MODE_BIT       (4)
#define TRIGGER_TDATA1_U_MODE_BIT       (3)
#define TRIGGER_TDATA1_EXEC_BIT         (2)
#define TRIGGER_TDATA1_STORE_BIT        (1)
#define TRIGGER_TDATA1_LOAD_BIT         (0)

#define TRIGGER_TDATA1_TYPE_MASK        (0xf)
#define TRIGGER_TDATA1_DMODE_MASK       (0x1)
#define TRIGGER_TDATA1_MASKMAX_MASK     (0x3f)
#define TRIGGER_TDATA1_HIT_MASK         (0x1)
#define TRIGGER_TDATA1_SELECT_MASX      (0x1)
#define TRIGGER_TDATA1_TIMING_MASK      (0x1)
#define TRIGGER_TDATA1_SIZELO_MASK      (0x3)
#define TRIGGER_TDATA1_ACTION_MASK      (0xf)
#define TRIGGER_TDATA1_CHAIN_MASK       (0x1)
#define TRIGGER_TDATA1_MATCH_MASK       (0xf)
#define TRIGGER_TDATA1_M_MODE_MASK      (0x1)
#define TRIGGER_TDATA1_S_MODE_MASK      (0x1)
#define TRIGGER_TDATA1_U_MODE_MASK      (0x1)
#define TRIGGER_TDATA1_EXEC_MASK        (0x1)
#define TRIGGER_TDATA1_STORE_MASK       (0x1)
#define TRIGGER_TDATA1_LOAD_MASK        (0x1)

typedef union {
    struct {
        UINT8 pingpang_flag : 1;
        UINT8 used_map : 4;
        UINT8 swap_trigger_num : 2;
        UINT8 swap_used_st_trigger : 1;
    };
    UINT8 ctrl;
} TriggerCtrl;

typedef struct {
    UINT32 addr;
    UINT32 size;
    UINT32 origin_value;
}TriggerInfo;

UINT32 ArchProtectionUnSetAddrRo(UINT32 index);
UINT32 ArchProtectionSetAddrRo(UINT32 addr, UINT32 size);
VOID GetTriggerInfo(UINT32 index);

#endif