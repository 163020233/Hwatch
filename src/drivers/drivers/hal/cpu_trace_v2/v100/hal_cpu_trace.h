/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description:  CPU Trace HAL Header file for CPU Trace driver
 */
#ifndef HAL_CPU_TRACE_H
#define HAL_CPU_TRACE_H

#include <stdint.h>
#include "std_def.h"

/** @defgroup connectivity_drivers_hal_cpu_trace CPU Trace
  * @ingroup  connectivity_drivers_hal
  * @{
  */

typedef enum {
    HAL_CPU_TRACE_SAMPLE_MODE_ONCE = 0,  // store until full
    HAL_CPU_TRACE_SAMPLE_MODE_REPEAT,    // repeat store
} hal_cpu_trace_sample_mode_t;

typedef enum {
    HAL_CPU_TRACE_SAVE_LOACATION_INTRNAL = 0,   // internal memory
    HAL_CPU_TRACE_SAVE_LOACATION_AXI,           // external memory by axi bus
} hal_cpu_trace_save_loacation_t;

typedef struct {
    uint32_t control_base_addr;
    uint32_t sample_base_addr;
    uint32_t save_location;
    uint32_t sample_mode;
    uint32_t sample_len_l;
    uint32_t sample_len_h;
    uint32_t sample_start_addr_l;
    uint32_t sample_start_addr_h;
    uint32_t sample_end_addr_l;
    uint32_t sample_end_addr_h;
    uint32_t clock;
} hal_cpu_trace_info_t;

typedef struct {
    uint32_t base_addr;
    uint32_t pclr_load_offset;
    uint32_t pclr_en_enable;
    uint32_t pc_low_offset;
    uint32_t pc_high_offset;
    uint32_t lr_low_offset;
    uint32_t lr_high_offset;
    uint32_t sp_low_offset;
    uint32_t sp_high_offset;
} hal_cpu_pclr_info_t;

/**
 * @brief  Enable CPU Trace
 * @param  cpu The cpu to get pointer
 */
void hal_cpu_trace_enable(uint32_t cpu);

/**
 * @brief  Disable CPU Trace
 * @param  cpu The cpu to get pointer
 */
void hal_cpu_trace_disable(uint32_t cpu);

/**
 * @brief  Enable cpu lock
 * @param  cpu The cpu to get pointer
 */
void hal_cpu_trace_lock_enable(uint32_t cpu);

/**
 * @brief  Lock cpu pc and lr
 * @param  cpu The cpu to get pointer
 */
void hal_cpu_trace_lock_pclr(uint32_t cpu);

/**
 * @brief  Get locked pc
 * @param  cpu The cpu to get pointer
 * @return pc
 */
uint32_t hal_cpu_trace_get_locked_pc(uint32_t cpu);

/**
 * @brief  Get locked lr
 * @param  cpu The cpu to get pointer
 * @return lr
 */
uint32_t hal_cpu_trace_get_locked_lr(uint32_t cpu);

/**
 * @brief  Get stack pointer
 * @param  cpu HAL_CPU_TRACE_TRACED_GCPU or HAL_CPU_TRACE_TRACED_MCPU
 * @return stack pointer value
 */
uint32_t hal_cpu_trace_get_locked_sp(uint32_t cpu);

/**
 * @brief  Get locked LR & PC
 * @param  cpu The core need to get
 * @param  pc The PC pointer
 * @param  lr The LR pointer
 * @param  sp The SP pointer
 * @return true or false
 */
bool hal_cpu_trace_get_locked_regs(uint32_t cpu, uint32_t *pc, uint32_t *lr, uint32_t *sp);

/**
 * @brief  the state of cpu trace running.
 * @return true running
 *         false stoped
 */
bool hal_cpu_trace_is_running(void);

/**
 * @brief  Set pclr pass through
 * @param  cpu The core need to get
 */
void hal_cpu_trace_set_pclr_passthrough(uint32_t cpu);
/**
  * @}
  */
#endif
