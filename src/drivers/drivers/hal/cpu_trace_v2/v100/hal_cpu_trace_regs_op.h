/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Provides CPU_TRACE register operation api
 */

#ifndef HAL_CPU_TRACE_REGS_OP_H
#define HAL_CPU_TRACE_REGS_OP_H

#include <stdint.h>
#include "hal_cpu_trace_regs_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_cpu_trace_regs_op  CPU_TRACE Regs Operation
 * @ingroup  drivers_hal_cpu_trace
 * @{
 */

extern cpu_trace_control_regs_t *g_cpu_trace_control_regs;
extern cpu_trace_sample_regs_t *g_cpu_trace_sample_regs;
extern volatile pclr_regs_t *g_cpu_pclr_regs;

static inline void hal_cpu_trace_set_save_location(uint32_t cpu_trace_save_location)
{
    cfg_cpu_trace_save_sel_t cpu_trace_save_reg;
    cpu_trace_save_reg.d32 = g_cpu_trace_control_regs->cfg_cpu_trace_save_sel;
    cpu_trace_save_reg.b.locate_sel = cpu_trace_save_location;
    g_cpu_trace_control_regs->cfg_cpu_trace_save_sel = cpu_trace_save_reg.d32;
}

static inline void hal_cpu_trace_set_enable(uint32_t enable)
{
    cpu_trace_enable_data_t cpu_trace_enable_reg;
    cpu_trace_enable_reg.d32 = g_cpu_trace_control_regs->cpu_trace_enable_data;
    cpu_trace_enable_reg.b.diag_enable = enable;
    g_cpu_trace_control_regs->cpu_trace_enable_data = cpu_trace_enable_reg.d32;
}

static inline void hal_cpu_trace_set_clock(uint32_t clk)
{
    g_cpu_trace_control_regs->diag_monitor_clock = clk;
}

static inline void hal_cpu_trace_set_sample_sync(uint32_t sync)
{
    cfg_diag_sample_mode_data_t cpu_trace_sample_reg;
    cpu_trace_sample_reg.d32 = g_cpu_trace_sample_regs->cfg_diag_sample_mode_data;
    cpu_trace_sample_reg.b.diag_sample_sync = sync;
    g_cpu_trace_sample_regs->cfg_diag_sample_mode_data = cpu_trace_sample_reg.d32;
}

static inline void hal_cpu_trace_set_sample_enable(uint32_t enable)
{
    cfg_diag_sample_mode_data_t cpu_trace_sample_reg;
    cpu_trace_sample_reg.d32 = g_cpu_trace_sample_regs->cfg_diag_sample_mode_data;
    cpu_trace_sample_reg.b.diag_sample_en = enable;
    g_cpu_trace_sample_regs->cfg_diag_sample_mode_data = cpu_trace_sample_reg.d32;
}

static inline void hal_cpu_trace_set_sample_mode(uint32_t mode)
{
    cfg_diag_sample_mode_data_t cpu_trace_sample_reg;
    cpu_trace_sample_reg.d32 = g_cpu_trace_sample_regs->cfg_diag_sample_mode_data;
    cpu_trace_sample_reg.b.diag_sample_mode = mode;
    g_cpu_trace_sample_regs->cfg_diag_sample_mode_data = cpu_trace_sample_reg.d32;
}

static inline void hal_cpu_trace_set_sample_len_l(uint32_t data)
{
    cfg_cpu_trace_locate_data_t cpu_trace_locate_reg;
    cpu_trace_locate_reg.d32 = g_cpu_trace_sample_regs->diag_sample_len_l;
    cpu_trace_locate_reg.b.diag_locate = data;
    g_cpu_trace_sample_regs->diag_sample_len_l = cpu_trace_locate_reg.d32;
}

static inline void hal_cpu_trace_set_sample_len_h(uint32_t data)
{
    cfg_cpu_trace_locate_data_t cpu_trace_locate_reg;
    cpu_trace_locate_reg.d32 = g_cpu_trace_sample_regs->diag_sample_len_h;
    cpu_trace_locate_reg.b.diag_locate = data;
    g_cpu_trace_sample_regs->diag_sample_len_h = cpu_trace_locate_reg.d32;
}

static inline void hal_cpu_trace_set_sample_start_addr_l(uint32_t data)
{
    cfg_cpu_trace_locate_data_t cpu_trace_locate_reg;
    cpu_trace_locate_reg.d32 = g_cpu_trace_sample_regs->diag_sample_start_l;
    cpu_trace_locate_reg.b.diag_locate = data;
    g_cpu_trace_sample_regs->diag_sample_start_l = cpu_trace_locate_reg.d32;
}

static inline void hal_cpu_trace_set_sample_start_addr_h(uint32_t data)
{
    cfg_cpu_trace_locate_data_t cpu_trace_locate_reg;
    cpu_trace_locate_reg.d32 = g_cpu_trace_sample_regs->diag_sample_start_h;
    cpu_trace_locate_reg.b.diag_locate = data;
    g_cpu_trace_sample_regs->diag_sample_start_h = cpu_trace_locate_reg.d32;
}

static inline void hal_cpu_trace_set_sample_end_addr_l(uint32_t data)
{
    cfg_cpu_trace_locate_data_t cpu_trace_locate_reg;
    cpu_trace_locate_reg.d32 = g_cpu_trace_sample_regs->diag_sample_end_l;
    cpu_trace_locate_reg.b.diag_locate = data;
    g_cpu_trace_sample_regs->diag_sample_end_l = cpu_trace_locate_reg.d32;
}

static inline void hal_cpu_trace_set_sample_end_addr_h(uint32_t data)
{
    cfg_cpu_trace_locate_data_t cpu_trace_locate_reg;
    cpu_trace_locate_reg.d32 = g_cpu_trace_sample_regs->diag_sample_end_h;
    cpu_trace_locate_reg.b.diag_locate = data;
    g_cpu_trace_sample_regs->diag_sample_end_h = cpu_trace_locate_reg.d32;
}

static inline uint32_t hal_cpu_trace_get_sample_done_addr_l(void)
{
    return g_cpu_trace_sample_regs->diag_sample_done_l;
}

static inline uint32_t hal_cpu_trace_get_sample_done_addr_h(void)
{
    return g_cpu_trace_sample_regs->diag_sample_done_h;
}

static inline uint32_t hal_cpu_trace_get_sample_done_flag(void)
{
    return g_cpu_trace_sample_regs->diag_sample_done_flag;
}

static inline void hal_cpu_pclr_set_load(uint32_t data)
{
    pc_lr_load_data_t pclr_load_reg;
    pclr_load_reg.d32 = g_cpu_pclr_regs->pclr_load;
    pclr_load_reg.b.load = data;
    g_cpu_pclr_regs->pclr_load = pclr_load_reg.d32;
}

static inline void hal_cpu_pclr_set_clr(uint32_t data)
{
    pc_lr_load_data_t pclr_load_reg;
    pclr_load_reg.d32 = g_cpu_pclr_regs->pclr_load;
    pclr_load_reg.b.load_clr = data;
    g_cpu_pclr_regs->pclr_load = pclr_load_reg.d32;
}

static inline void hal_cpu_pclr_set_mode_sel(uint32_t data)
{
    pc_lr_load_data_t pclr_load_reg;
    pclr_load_reg.d32 = g_cpu_pclr_regs->pclr_load;
    pclr_load_reg.b.pclr_mode_sel = data;
    g_cpu_pclr_regs->pclr_load = pclr_load_reg.d32;
}

static inline uint32_t hal_cpu_pclr_get_load_status(void)
{
    pc_lr_load_data_t pclr_load_reg;
    pclr_load_reg.d32 = g_cpu_pclr_regs->pclr_load;
    return pclr_load_reg.b.load_ok;
}

static inline void hal_cpu_pclr_set_enable(uint32_t data)
{
    pc_lr_enable_t pclr_enable_reg;
    pclr_enable_reg.d32 = g_cpu_pclr_regs->pclr_enable;
    pclr_enable_reg.b.pclr_en = data;
    g_cpu_pclr_regs->pclr_enable = pclr_enable_reg.d32;
}

#ifdef CPU_LOCK_32_WIDTH_SUPPORT
static inline uint32_t hal_cpu_pclr_get_pc_addr(void)
{
    return g_cpu_pclr_regs->pc;
}

static inline uint32_t hal_cpu_pclr_get_lr_addr(void)
{
    return g_cpu_pclr_regs->lr;
}

static inline uint32_t hal_cpu_pclr_get_sp_addr(void)
{
    return g_cpu_pclr_regs->sp;
}

#else
static inline uint32_t hal_cpu_pclr_get_pc_addr_l(void)
{
    return g_cpu_pclr_regs->pc_l;
}

static inline uint32_t hal_cpu_pclr_get_pc_addr_h(void)
{
    return g_cpu_pclr_regs->pc_h;
}

static inline uint32_t hal_cpu_pclr_get_lr_addr_l(void)
{
    return g_cpu_pclr_regs->lr_l;
}

static inline uint32_t hal_cpu_pclr_get_lr_addr_h(void)
{
    return g_cpu_pclr_regs->lr_h;
}

static inline uint32_t hal_cpu_pclr_get_sp_addr_l(void)
{
    return g_cpu_pclr_regs->sp_l;
}

static inline uint32_t hal_cpu_pclr_get_sp_addr_h(void)
{
    return g_cpu_pclr_regs->sp_h;
}
#endif  /* #ifdef CPU_LOCK_32_WIDTH_SUPPORT */

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
