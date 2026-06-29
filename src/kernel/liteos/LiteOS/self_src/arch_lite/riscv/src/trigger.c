/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: trigger
 * Author: Huawei LiteOS Team
 * Create: 2025-06-12
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

#include "arch/trigger.h"
#include "arch/regs.h"
#include "los_typedef.h"
#include "los_printf.h"
#include "los_task_pri.h"

#define TRIGGER_TYPE_ADDR_OR_DATA_MARCH     (0x2)
#define TRIGGER_MARCH_NAPOT_MODE            (0x1)
#define TRIGGER_COMMON_ENABLE               (0x1)
#define TRIGGER_ADDR_MASK                   (0xfffffffe)
#define TRIGGER_MAX_INDEX                   (4)
#define TRIGGER_INDEX_ALL                   (0xffffffff)

TriggerCtrl g_trigger_ctrl = { 0 };
TriggerInfo g_trigger_info[TRIGGER_MAX_INDEX] = { 0 };
UINT32      g_trigger_swap_td1 = 0;
UINT32      g_trigger_swap_td2 = 0;
UINT32      g_trigger_caller = 0;
UINT32      g_trigger_mepc = 0;
UINT32      g_trigger_presave_mepc = 0;
UINT32      g_trigger_presave_mcause = 0;
UINT32      g_trigger_next_pc = 0;
UINT32      g_trigger_swap_sp = 0;


STATIC INLINE UINT32 TRIGGER_ALIGN_UP(UINT32 x, UINT32 align)
{
    return ((x) + (align)-1) & ~((align)-1);
}

STATIC INLINE UINT32 InnerTriggerCheck(UINT32 addr, UINT32 size)
{
    if (addr == 0 || addr == 0xffffffff) {
        PRINTK("ERR: addr(%#x) is not avalible.\n", addr);
        return LOS_NOK;
    }
    if ((size != 1 && size != 4) || size == 0) {   // the range of size is (1, 4)
        PRINTK("ERR: size(%#x) must be one of the (1, 4).\n", size);
        return LOS_NOK;
    }
    if ((size == 4) && (addr != TRIGGER_ALIGN_UP(addr, 4))) { // size is 4
        PRINTK("ERR: addr(%#x) must be aligned with %#x(need to be size(%#x)).\n", addr, size, size);
        return LOS_NOK;
    }
    return LOS_OK;
}

STATIC VOID InnerTriggerReset(UINT32 trigger_id)
{
    UINT32 reset_value = 0;
    WRITE_CSR(TRIGGER_SELECT, trigger_id);
    WRITE_CSR(TRIGGER_TDATA1, reset_value);
    WRITE_CSR(TRIGGER_TDATA2, reset_value);
}

STATIC UINT32 InnerTriggerSet(UINT32 trigger_id, UINT32 addr, UINT32 size)
{
    UINT32 td1 = 0;
    UINT32 td2 = addr;
    td1 |= (TRIGGER_TYPE_ADDR_OR_DATA_MARCH << TRIGGER_TDATA1_TYPE_BIT);
    td1 |= (TRIGGER_COMMON_ENABLE << TRIGGER_TDATA1_STORE_BIT);
    td1 |= (TRIGGER_COMMON_ENABLE << TRIGGER_TDATA1_M_MODE_BIT);
    if (size == 4) {    // when size=4, the matching is fuzzy.
        td1 |= (TRIGGER_MARCH_NAPOT_MODE << TRIGGER_TDATA1_MATCH_BIT);
        td2 |= 0x1;
    }

    WRITE_CSR(TRIGGER_SELECT, trigger_id);
    WRITE_CSR(TRIGGER_TDATA1, td1);
    WRITE_CSR(TRIGGER_TDATA2, td2);
    return LOS_OK;
}

STATIC INLINE UINT32 InnerTriggerGetFree(VOID)
{
    for (UINT32 i = 0; i < TRIGGER_MAX_INDEX; ++i) {
        if (((g_trigger_ctrl.used_map >> i) & 0x1) == 0) {
            return i;
        }
    }
    return TRIGGER_MAX_INDEX;
}

STATIC VOID InnerTriggerGetInfos(VOID)
{
    for (UINT32 i = 0; i < TRIGGER_MAX_INDEX; ++i) {
        if (((g_trigger_ctrl.used_map >> i) & 0x1) == 0) {
            continue;
        }
        WRITE_CSR(TRIGGER_SELECT, i);
        g_trigger_info[i].size = READ_CSR(TRIGGER_TDATA2);
        g_trigger_info[i].addr = g_trigger_info[i].size & TRIGGER_ADDR_MASK;
        g_trigger_info[i].size = ((g_trigger_info[i].size & 0x1) == 0x1) ? 0x4 : 0x1;
        if (g_trigger_info[i].size == 0x1) {
            g_trigger_info[i].origin_value = *(volatile UINT8 *)g_trigger_info[i].addr;
        } else if (g_trigger_info[i].size == 0x4) {
            g_trigger_info[i].origin_value = *(volatile UINT32 *)g_trigger_info[i].addr;
        } else {
            g_trigger_info[i].origin_value = 0;
        }
    }
}

STATIC VOID InnerTriggerResetInfos(VOID)
{
    for (UINT32 i = 0; i < TRIGGER_MAX_INDEX; ++i) {
        g_trigger_info[i].size = 0;
        g_trigger_info[i].addr = 0;
        g_trigger_info[i].origin_value = 0;
    }
}

STATIC VOID InnerTriggerDisable(UINT32 index)
{
    WRITE_CSR(TRIGGER_SELECT, index);
    UINT32 td1 = READ_CSR(TRIGGER_TDATA1);
    td1 &= ~(1 << TRIGGER_TDATA1_M_MODE_BIT);
    WRITE_CSR(TRIGGER_TDATA1, td1);
}

STATIC VOID InnerTriggerEnable(UINT32 index)
{
    WRITE_CSR(TRIGGER_SELECT, index);
    UINT32 td1 = READ_CSR(TRIGGER_TDATA1);
    td1 |= (1 << TRIGGER_TDATA1_M_MODE_BIT);
    WRITE_CSR(TRIGGER_TDATA1, td1);
}

STATIC VOID InnerTriggerDisableAll(VOID)
{
    for (UINT32 i = 0; i < TRIGGER_MAX_INDEX; ++i) {
        InnerTriggerDisable(i);
    }
}

STATIC VOID InnerTriggerEnableAllByUseMap(VOID)
{
    for (UINT32 i = 0; i < TRIGGER_MAX_INDEX; ++i) {
        if (((g_trigger_ctrl.used_map >> i) & 0x1) == 0x1) {
            InnerTriggerEnable(i);
        }
    }
}

STATIC VOID InnerTriggerEnablePcProtect(VOID)
{
    UINT32 trigger_id = InnerTriggerGetFree();
    if (trigger_id >= TRIGGER_MAX_INDEX) {
        WRITE_CSR(TRIGGER_SELECT, 0);
        g_trigger_swap_td1 = READ_CSR(TRIGGER_TDATA1);
        g_trigger_swap_td2 = READ_CSR(TRIGGER_TDATA2);
        trigger_id = 0;
        g_trigger_ctrl.swap_used_st_trigger = 1;
    }
    g_trigger_ctrl.swap_trigger_num = trigger_id;
    UINT32 td1 = (1 << TRIGGER_TDATA1_EXEC_BIT) | (1 << TRIGGER_TDATA1_M_MODE_BIT);
    WRITE_CSR(TRIGGER_SELECT, trigger_id);
    WRITE_CSR(TRIGGER_TDATA2, g_trigger_next_pc);
    WRITE_CSR(TRIGGER_TDATA1, td1);
    g_trigger_ctrl.used_map |= 1 << trigger_id;
}

STATIC VOID InnerTriggerReEnableStoreProtect(VOID)
{
    if (g_trigger_ctrl.swap_used_st_trigger == 0) {
        g_trigger_ctrl.used_map &= ((~(1 << g_trigger_ctrl.swap_trigger_num)) & 0xf);
        InnerTriggerReset(g_trigger_ctrl.swap_trigger_num);
        return;
    }
    g_trigger_ctrl.swap_used_st_trigger = 0;
    WRITE_CSR(TRIGGER_SELECT, 0);
    WRITE_CSR(TRIGGER_TDATA2, g_trigger_swap_td2);
    WRITE_CSR(TRIGGER_TDATA1, g_trigger_swap_td1);
}

STATIC UINT32 InnerTriggerGetDiff(VOID)
{
    for (UINT32 i = 0; i < TRIGGER_MAX_INDEX; ++i) {
        if (g_trigger_info[i].addr == 0) {
            continue;
        }
        volatile UINT32 val = 0;
        if (g_trigger_info[i].size == 1) {
            val = *(volatile UINT8 *)g_trigger_info[i].addr;
        } else if (g_trigger_info[i].size == 4) {   // size = 4
            val = *(volatile UINT32 *)g_trigger_info[i].addr;
        }
        if (g_trigger_info[i].origin_value != val) {
            return i;
        }
    }
    return TRIGGER_MAX_INDEX;
}

STATIC VOID InnerTriggerFirst(VOID)
{
    InnerTriggerGetInfos();
    InnerTriggerDisableAll();
    InnerTriggerEnablePcProtect();
    g_trigger_mepc = READ_CSR(mepc);
}

STATIC VOID InnerTriggerSecond(VOID)
{
    UINT32 trigger_id = InnerTriggerGetDiff();
    if (trigger_id == TRIGGER_MAX_INDEX) {
        return;
    }
    LosTaskCB *task_cb = OsCurrTaskGet();
    volatile UINT32 new_val = 0;
    if (g_trigger_info[trigger_id].size == 1) {
        new_val = *(UINT8 *)(g_trigger_info[trigger_id].addr);
    } else if (g_trigger_info[trigger_id].size == 4) { // size=4 Use fuzzy match
        new_val = *(UINT32 *)(g_trigger_info[trigger_id].addr);
    }
    PRINTK("[Trigger] task_name: %s, mepc: %#x, caller: %#x, diff_addr: %#x, old_val: 0x%08x, new_val: 0x%08x\n",
        task_cb->taskName, g_trigger_mepc, g_trigger_caller, g_trigger_info[trigger_id].addr,
        g_trigger_info[trigger_id].origin_value, new_val);
    InnerTriggerReEnableStoreProtect();
    InnerTriggerEnableAllByUseMap();
    InnerTriggerResetInfos();
    g_trigger_mepc = 0;
}

VOID TriggerSaveNextPC(UINT32 next_pc)
{
    if (g_trigger_ctrl.pingpang_flag == 0) {
        g_trigger_next_pc = next_pc;
    }
}

VOID TriggerSaveCSR(VOID)
{
    g_trigger_presave_mepc = READ_CSR(mepc);
    g_trigger_presave_mcause = READ_CSR(mcause);
}

VOID TriggerRestoreCSR(VOID)
{
    WRITE_CSR(mepc, g_trigger_presave_mepc);
    WRITE_CSR(mcause, g_trigger_presave_mcause);
}

UINT32 ArchProtectionSetAddrRo(UINT32 addr, UINT32 size)
{
    if (InnerTriggerCheck(addr, size) != LOS_OK) {
        return LOS_NOK;
    }
    UINT32 trigger_id = InnerTriggerGetFree();
    if (trigger_id >= TRIGGER_MAX_INDEX) {
        return LOS_NOK;
    }
    InnerTriggerReset(trigger_id);
    g_trigger_ctrl.used_map |= (1 << trigger_id);
    return InnerTriggerSet(trigger_id, addr, size);
}

UINT32 ArchProtectionUnSetAddrRo(UINT32 index)
{
    if (index == TRIGGER_INDEX_ALL) {
        for (UINT32 i = 0; i < TRIGGER_MAX_INDEX; ++i) {
            InnerTriggerReset(i);
            g_trigger_ctrl.used_map &= ~(1 << i);
        }
    } else if (index >= TRIGGER_MAX_INDEX) {
        return LOS_NOK;
    } else {
        InnerTriggerReset(index);
        g_trigger_ctrl.used_map &= ~(1 << index);
    }
    return LOS_OK;
}

VOID GetTriggerInfo(UINT32 index)
{
    if (index == TRIGGER_INDEX_ALL) {
        for (UINT32 i = 0; i < TRIGGER_MAX_INDEX; ++i) {
            WRITE_CSR(TRIGGER_SELECT, i);
            UINT32 addr_and_size = READ_CSR(TRIGGER_TDATA2);
            UINT32 addr = addr_and_size & TRIGGER_ADDR_MASK;
            UINT32 size = ((addr_and_size & 0x1) == 0x1) ? 0x4 : 1;
            PRINTK("index: 0x%x, protected addr: 0x%x, and protected size: 0x%x \n", i, addr, size);
        }
    } else if (index >= TRIGGER_MAX_INDEX) {
        return;
    } else {
        WRITE_CSR(TRIGGER_SELECT, index);
        UINT32 addr_and_size = READ_CSR(TRIGGER_TDATA2);
        UINT32 addr = addr_and_size & TRIGGER_ADDR_MASK;
        UINT32 size = ((addr_and_size & 0x1) == 0x1) ? 0x4 : 1;
        PRINTK("index: 0x%x, protected addr: 0x%x, and protected size: 0x%x \n", index, addr, size);
    }
}

VOID ArchTriggerCallBack(VOID)
{
    if (g_trigger_ctrl.pingpang_flag == 0) {
        InnerTriggerFirst();
        g_trigger_ctrl.pingpang_flag = 1;
    } else {
        InnerTriggerSecond();
        g_trigger_ctrl.pingpang_flag = 0;
    }
}