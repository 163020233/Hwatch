/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Board config before os start.
 * Author: Huawei LiteOS Team
 * Create: 2024-03-09
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

#include "memory_config.h"
#include "platform_core.h"

#include "los_init.h"
#include "los_task_pri.h"
#include "los_tick_pri.h"
#include "los_init_pri.h"


void *g_sysMemAddr;
uint32_t g_sysMemSize;

uintptr_t g_usb_mem_addr_start __attribute__((section(".data"))) = 0;
unsigned long g_usb_mem_size __attribute__((section(".data"))) = 0;

void dma_cache_clean(unsigned int start, unsigned int end)
{
    ArchDCacheCleanByAddr(start, end);
}

void dma_cache_inv(unsigned int start, unsigned int end)
{
    ArchDCacheFlushByAddr(start, end);
}

VOID BoardCacheInit(VOID)
{
    ArchICacheFlush();

    (void)ArchICacheEnable(CACHE_16KB);
    (void)ArchICachePrefetchEnable(CACHE_PREF_1_LINES);
}

VOID BoardConfig(VOID)
{
#ifdef LOSCFG_LIB_CONFIGURABLE
    g_sysMemAddr = (void *)&g_intheap_begin;
    g_sysMemSize = (unsigned)&g_ram_begin + (unsigned)&g_ram_size - (unsigned)&g_intheap_begin;
#endif

    g_usb_mem_addr_start = (uintptr_t)0;
    g_usb_mem_size = 0;

    BoardCacheInit();

    /* config start memory before startup usb_init */

    OsSetMainTask();
    OsCurrTaskSet(OsGetMainTask());
}
#define reg32(register)                             (*(volatile uint32_t*)(uintptr_t)(register))
#define reg32_write(register, data)                  (reg32(register) = (data))
#define BOOT_UART_TX_FIFO 0x30008004
VOID UartPuts(const CHAR *s, UINT32 len, BOOL isLock)
{
    (VOID) isLock;
    if ((s == NULL) || (strlen(s) == 0)) {
        return;
    }
    // todo: add uart
    for (int i = 0; i < len; i++) {
        reg32_write(BOOT_UART_TX_FIFO, *s++);
    }
}

LITE_OS_SEC_TEXT_INIT UINT32 OsAdaptInit(VOID)
{
    uint8_t clicintattr = 0;

    /* clicintattr[2:1] = 1 edge-triggered;
     * clicintattr[2:1] = 0 level-triggered;
     * clicintattr[7:6] = 3 M-mode
     */
    clicintattr = ((0 << 0x1) | (0x3 << 0x6));
    for (uint8_t index = 0; index < LOSCFG_PLATFORM_HWI_LIMIT; index++) {
        HalIrqSetClicIntAttr(index, clicintattr);
    }

    return LOS_OK;
}
LOS_SYS_INIT(OsAdaptInit, SYS_INIT_LEVEL_APP, SYS_INIT_SYNC_1);
