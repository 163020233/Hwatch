/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: 3322 application core os memory dfx
 */
#ifndef _APP_DFX_MEM_H
#define _APP_DFX_MEM_H
#include "stdint.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

uint32_t dfx_show_task_heap(uint32_t tid);
uint32_t dfx_show_all_task_heap(void);
uint32_t dfx_show_mem_free(void);
uint32_t dfx_show_heap_statistic(void);
uint32_t dfx_show_mem_info(void);
uint32_t dfx_show_task_info(void);
uint32_t dfx_show_stack_waterline(void);

uint32_t dfx_get_callerRa(uint32_t tid);
uint32_t dfx_detect_callerRa(uint32_t tid);
uint32_t cleanup_all_callerRa_state(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* _APP_DFX_MEM_H */