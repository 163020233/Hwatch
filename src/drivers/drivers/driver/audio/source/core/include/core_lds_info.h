/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: overlay information
 */

#ifndef __CORE_LDS_INFO_H__
#define __CORE_LDS_INFO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*
 * main .bss section
 * 编译器要求bss段放在最后
 * heap堆内存的起点
 * main静态内存的终点
 */
extern td_u32 _bss_end[];

/*
 * main HEAP section
 * heap堆内存终点指针，堆内存申请不能超过这个地址
 * main sram内存终点
 */
extern td_u32 _heap_sentry[];

/*
 * main .iram.text section
 * iram_start为iram内存起点
 * _iram_text_end设置为iram内存的终点
 */
extern td_u32 _iram_text_end[];

/*
 * main .dram.bss section
 * dram_start为dram内存起点
 * _dram_bss_end为dram内存的终点，编译器要求bss段放在最后
 */
extern td_u32 _dram_bss_end[];

/* Pointer to next available space, maintained by this sbrk() function in libgloss. */
extern td_char *heap_ptr;

extern td_u32 _memmap_mem_sram_start[];
extern td_u32 _memmap_mem_sram_end[];

extern td_u32 _memmap_mem_iram_start[];
extern td_u32 _memmap_mem_iram_end[];

#ifndef SAP_AUDIO_DSP_BSS_IN_DRAM_SUPPORT
extern td_u32 _memmap_mem_dram_start[];
extern td_u32 _memmap_mem_dram_end[];
#endif

#ifdef SAP_DSP_LSP_AEF
extern td_u32 _memmap_mem_aef_start[];
extern td_u32 _memmap_mem_aef_end[];
#endif

#ifdef SAP_DSP_LSP_CODEC
extern td_u32 _memmap_mem_codec_start[];
extern td_u32 _memmap_mem_codec_end[];
#endif

#ifdef SAP_DSP_LSP_SEA
extern td_u32 _memmap_mem_sea_start[];
extern td_u32 _memmap_mem_sea_end[];
#endif

#ifdef SAP_DSP_LSP_AAI
extern const td_u32 _memmap_mem_aai_start[];
extern const td_u32 _memmap_mem_aai_end[];
#endif

#ifdef SAP_DSP_LSP_AEF_PRE
extern const td_u32 _memmap_mem_aef_pre_start[];
extern const td_u32 _memmap_mem_aef_pre_end[];
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CORE_OVERLAY_INFO_H__ */
