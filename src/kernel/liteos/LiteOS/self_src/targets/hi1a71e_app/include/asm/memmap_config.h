/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: os memmap header.
 * Author: Huawei LiteOS Team
 * Create: 2025-01-24
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
#ifndef MEMMAP_CONFIG_H
#define MEMMAP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern VOID *g_intheap_begin;
extern VOID *g_intheap_size;
extern VOID *g_ram_begin;
extern VOID *g_ram_size;
extern VOID *g_extend_heap_begin;
extern VOID *g_extend_heap_size;
extern VOID *g_usb_ram_begin;
extern VOID *g_usb_ram_size;

#ifdef LOSCFG_LIB_CONFIGURABLE
extern VOID *g_sysMemAddr;
extern unsigned int g_sysMemSize;
#define OS_SYS_MEM_ADDR     g_sysMemAddr
#define OS_SYS_MEM_SIZE     g_sysMemSize

#else
#define OS_SYS_MEM_ADDR       ((VOID *)&g_intheap_begin)
#define OS_SYS_MEM_SIZE       ((unsigned)&g_intheap_size)

#define OS_EXTEND_MEM_ADDR    ((VOID *)&g_extend_heap_begin)
#define OS_EXTEND_MEM_SIZE    ((unsigned)&g_extend_heap_size)

#endif
#define OS_USB_MEM_ADDR     ((VOID *)&g_usb_ram_begin)
#define OS_USB_MEM_SIZE     ((UINTPTR)&g_usb_ram_size)

#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
extern UINTPTR              __irq_stack_top__;
#ifndef APP_IRQ_STACK_LEN
#define APP_IRQ_STACK_LEN   0xC00
#endif
#define IRQ_STACK_TOP       ((UINTPTR)&__irq_stack_top__)
#define IRQ_STACK_SIZE      APP_IRQ_STACK_LEN
#endif

#ifdef LOSCFG_USB_MASS_STORAGE_INDEPENDENT_MEMORY_POOL
extern void *__usb_mass_storage_heap_start;
extern CHAR __usb_mass_storage_heap_size;
#define USB_MASS_STORAGE_MEM_ADDR       ((void *)&__usb_mass_storage_heap_start)
#define USB_MASS_STORAGE_MEM_LENGTH     ((UINTPTR)&__usb_mass_storage_heap_size)
#endif

#ifdef LOSCFG_SHELL_ADAPT_AT
extern void *__ramtext_size__;
#define RAM_TEXT_SIZE ((unsigned)&__ramtext_size__)
extern void *__sdma_data_size__;
#define SDMA_DATA_SIZE ((unsigned)&__sdma_data_size__)
extern void *g_ram_size;
#define RAM_SIZE ((unsigned)&g_ram_size)

#define OS_SYS_TEXT_SIZE (RAM_TEXT_SIZE + RAM_SIZE)
extern void *__data_size__;
#define OS_SYS_DATA_SIZE (((unsigned)&__data_size__) + SDMA_DATA_SIZE)
extern void *__bss_size__;
#define OS_SYS_BSS_SIZE ((unsigned)&__bss_size__)
extern void *__rodata_size__;
#define OS_SYS_RODATA_SIZE (RAM_TEXT_SIZE)
#endif

#ifdef __cplusplus
}
#endif

#endif /* MEMMAP_CONFIG_H */
