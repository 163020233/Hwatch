/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Provides cpu trace register
 */

#include "hal_cpu_trace.h"
#include "chip_io.h"
#include "arch_barrier.h"
#include "hal_cpu_trace_regs_def.h"
#include "hal_cpu_trace_regs_op.h"

/* for bcpu pc/lr lock */
#define HAL_DEMO_CPU_TRACE_CTL_RB_BASE_ADDR                  0x57000400  // demo, not for use.

#define HAL_LOAD_DIAG_REG_MANUAL_TRIGGER_VALUE           0x2
#define HAL_PCLR_ENABLE_MANUAL_TRIGGER_VALUE             0x2
#define HAL_PCLR_ENABLE_PASSTHROUGH_VALUE                0x1
#define HAL_LOAD_MODE_SEL_LAST_ONE_LOCK_VALUE            0x1
#define HAL_CPU_LOAD_LOCK_TIMEOUT 10000
#define HAL_CPU_LOAD_RETRY_COUNT  2

#ifdef CPU_LOCK_32_WIDTH_SUPPORT
#define REG32_MASK_L                                     0xFFFF
#define REG32_MASK_H                                     0xFFFF0000
#define REG32_OFFSET_H                                   16
#endif

#define get_low_16bits(val)          ((uint16_t)((val) & 0xFFFF))
#define get_high_16bits(val)         ((uint16_t)((val) >> 16))
#define comb_to_32bits(val_h, val_l) ((uint32_t)((((uint32_t)((val_h) & 0xFFFF)) << 16) | ((val_l) & 0xFFFF)))

cpu_trace_control_regs_t *g_cpu_trace_control_regs = NULL;
cpu_trace_sample_regs_t *g_cpu_trace_sample_regs = NULL;
volatile pclr_regs_t *g_cpu_pclr_regs = NULL;

// 下面设置弱定义，在porting层上各项目进行重新定义实现
volatile uintptr_t __attribute__((weak)) g_cpu_pclr_base_value[] = {
    HAL_DEMO_CPU_TRACE_CTL_RB_BASE_ADDR,
};
hal_cpu_trace_info_t __attribute__((weak)) g_cpu_trace_info[] = {
    {
        .control_base_addr = 0,
        .sample_base_addr = 0,
        .save_location = HAL_CPU_TRACE_SAVE_LOACATION_INTRNAL,
        .sample_mode = HAL_CPU_TRACE_SAMPLE_MODE_REPEAT,
        .sample_len_l = 0,
        .sample_len_h = 0,
        .sample_start_addr_l = 0,
        .sample_start_addr_h = 0,
        .sample_end_addr_l = 0,
        .sample_end_addr_h = 0,
        .clock = 0
    }
};

bool g_hal_cputrace_running = false;

bool hal_cpu_trace_is_running(void)
{
    return g_hal_cputrace_running;
}

void hal_cpu_trace_enable(uint32_t cpu)
{
    g_cpu_trace_control_regs = (cpu_trace_control_regs_t *)g_cpu_trace_info[cpu].control_base_addr;
    g_cpu_trace_sample_regs = (cpu_trace_sample_regs_t *)g_cpu_trace_info[cpu].sample_base_addr;
    hal_cpu_trace_set_clock(g_cpu_trace_info[cpu].clock);
    hal_cpu_trace_set_sample_sync(0);
    hal_cpu_trace_set_sample_mode(g_cpu_trace_info[cpu].sample_mode);
    hal_cpu_trace_set_sample_len_l(g_cpu_trace_info[cpu].sample_len_l);
    hal_cpu_trace_set_sample_len_h(g_cpu_trace_info[cpu].sample_len_h);
    hal_cpu_trace_set_sample_start_addr_l(g_cpu_trace_info[cpu].sample_start_addr_l);
    hal_cpu_trace_set_sample_start_addr_h(g_cpu_trace_info[cpu].sample_start_addr_h);
    hal_cpu_trace_set_sample_end_addr_l(g_cpu_trace_info[cpu].sample_end_addr_l);
    hal_cpu_trace_set_sample_end_addr_h(g_cpu_trace_info[cpu].sample_end_addr_h);
    hal_cpu_trace_set_save_location(g_cpu_trace_info[cpu].save_location);
    hal_cpu_trace_set_sample_sync(1);
    hal_cpu_trace_set_sample_enable(1);
    hal_cpu_trace_set_enable(1);
    g_hal_cputrace_running = true;
}

void hal_cpu_trace_disable(uint32_t cpu)
{
    g_cpu_trace_control_regs =  (cpu_trace_control_regs_t *)g_cpu_trace_info[cpu].control_base_addr;
    g_cpu_trace_sample_regs = (cpu_trace_sample_regs_t *)g_cpu_trace_info[cpu].sample_base_addr;
    hal_cpu_trace_set_enable(0);
    hal_cpu_trace_set_sample_enable(0);
    g_hal_cputrace_running = false;
}

static void hal_cpu_trace_clear_lock_trigger(uint32_t cpu)
{
    uint32_t lock_count = 0;
    g_cpu_pclr_regs = (pclr_regs_t *)(pclr_regs_t *)g_cpu_pclr_base_value[cpu];
    hal_cpu_pclr_set_clr(0x1);
    do {
        if ((hal_cpu_pclr_get_load_status() & HAL_LOAD_DIAG_REG_MANUAL_TRIGGER_VALUE) == 0x0) {
            hal_cpu_pclr_set_clr(0x0);
            return;
        }
        lock_count++;
    } while (lock_count < HAL_CPU_LOAD_LOCK_TIMEOUT);
}

static bool hal_cpu_trace_lock_trigger(uint32_t cpu)
{
    uint32_t lock_count = 0;

    g_cpu_pclr_regs = (pclr_regs_t *)g_cpu_pclr_base_value[cpu];
    // 锁定最后一次触发的pc/lr
    hal_cpu_pclr_set_mode_sel(HAL_LOAD_MODE_SEL_LAST_ONE_LOCK_VALUE);
    hal_cpu_pclr_set_enable(HAL_PCLR_ENABLE_MANUAL_TRIGGER_VALUE);

    /* trigger pc/lr lock */
    hal_cpu_pclr_set_load(0x1);
    dsb();
    /* check the status */
    do {
        if ((hal_cpu_pclr_get_load_status() & HAL_LOAD_DIAG_REG_MANUAL_TRIGGER_VALUE) ==
            HAL_LOAD_DIAG_REG_MANUAL_TRIGGER_VALUE) {
            hal_cpu_pclr_set_load(0x0);
            hal_cpu_pclr_set_enable(0x0);
            return true;
        }
        lock_count++;
    } while (lock_count < HAL_CPU_LOAD_LOCK_TIMEOUT);
    hal_cpu_pclr_set_enable(0x0); // close enable.
    return false;
}

static void hal_cpu_trace_get_locked_reg(uint32_t cpu, uint32_t *pc, uint32_t *lr, uint32_t *sp)
{
    uint16_t pc_l, pc_h;
    uint16_t lr_l, lr_h;
    uint16_t sp_l, sp_h;
    g_cpu_pclr_regs = (pclr_regs_t *)g_cpu_pclr_base_value[cpu];

#ifdef CPU_LOCK_32_WIDTH_SUPPORT
    /* get pc value */
    pc_l = (uint16_t)(hal_cpu_pclr_get_pc_addr() & REG32_MASK_L);
    pc_h = (uint16_t)((hal_cpu_pclr_get_pc_addr() & REG32_MASK_H) >> REG32_OFFSET_H);
    /* get lr value */
    lr_l = (uint16_t)(hal_cpu_pclr_get_lr_addr() & REG32_MASK_L);
    lr_h = (uint16_t)((hal_cpu_pclr_get_lr_addr() & REG32_MASK_H) >> REG32_OFFSET_H);
    /* get sp value */
    sp_l = (uint16_t)(hal_cpu_pclr_get_sp_addr() & REG32_MASK_L);
    sp_h = (uint16_t)((hal_cpu_pclr_get_sp_addr() & REG32_MASK_H) >> REG32_OFFSET_H);
#else
    pc_l = (uint16_t)(hal_cpu_pclr_get_pc_addr_l());
    pc_h = (uint16_t)(hal_cpu_pclr_get_pc_addr_h());

    lr_l = (uint16_t)(hal_cpu_pclr_get_lr_addr_l());
    lr_h = (uint16_t)(hal_cpu_pclr_get_lr_addr_h());

    sp_l = (uint16_t)(hal_cpu_pclr_get_sp_addr_l());
    sp_h = (uint16_t)(hal_cpu_pclr_get_sp_addr_h());
#endif

    *pc = comb_to_32bits(pc_h, pc_l);
    *lr = comb_to_32bits(lr_h, lr_l);
    *sp = comb_to_32bits(sp_h, sp_l);
    hal_cpu_trace_clear_lock_trigger(cpu);
}

void hal_cpu_trace_set_pclr_passthrough(uint32_t cpu)
{
    g_cpu_pclr_regs = (pclr_regs_t *)g_cpu_pclr_base_value[cpu];
    hal_cpu_pclr_set_enable(HAL_PCLR_ENABLE_PASSTHROUGH_VALUE);
}

void hal_cpu_trace_lock_pclr(uint32_t cpu)
{
    hal_cpu_trace_lock_trigger(cpu);
}

uint32_t hal_cpu_trace_get_locked_pc(uint32_t cpu)
{
    uint16_t pc_l, pc_h;
    g_cpu_pclr_regs = (pclr_regs_t *)g_cpu_pclr_base_value[cpu];

    /* get pc value */
#ifdef CPU_LOCK_32_WIDTH_SUPPORT
    pc_l = (uint16_t)(hal_cpu_pclr_get_pc_addr() & REG32_MASK_L);
    pc_h = (uint16_t)((hal_cpu_pclr_get_pc_addr() & REG32_MASK_H) >> REG32_OFFSET_H);
#else
    pc_l = (uint16_t)(hal_cpu_pclr_get_pc_addr_l());
    pc_h = (uint16_t)(hal_cpu_pclr_get_pc_addr_h());
#endif

    hal_cpu_trace_clear_lock_trigger(cpu);
    return comb_to_32bits(pc_h, pc_l);
}

uint32_t hal_cpu_trace_get_locked_lr(uint32_t cpu)
{
    uint16_t lr_l, lr_h;
    g_cpu_pclr_regs = (pclr_regs_t *)g_cpu_pclr_base_value[cpu];

#ifdef CPU_LOCK_32_WIDTH_SUPPORT
    lr_l = (uint16_t)(hal_cpu_pclr_get_lr_addr() & REG32_MASK_L);
    lr_h = (uint16_t)((hal_cpu_pclr_get_lr_addr() & REG32_MASK_H) >> REG32_OFFSET_H);
#else
    lr_l = (uint16_t)(hal_cpu_pclr_get_lr_addr_l());
    lr_h = (uint16_t)(hal_cpu_pclr_get_lr_addr_h());
#endif
    hal_cpu_trace_clear_lock_trigger(cpu);
    return comb_to_32bits(lr_h, lr_l);
}

uint32_t hal_cpu_trace_get_locked_sp(uint32_t cpu)
{
    uint16_t sp_l, sp_h;
    g_cpu_pclr_regs = (pclr_regs_t *)g_cpu_pclr_base_value[cpu];

#ifdef CPU_LOCK_32_WIDTH_SUPPORT
    sp_l = (uint16_t)(hal_cpu_pclr_get_sp_addr() & REG32_MASK_L);
    sp_h = (uint16_t)((hal_cpu_pclr_get_sp_addr() & REG32_MASK_H) >> REG32_OFFSET_H);
#else
    sp_l = (uint16_t)(hal_cpu_pclr_get_sp_addr_l());
    sp_h = (uint16_t)(hal_cpu_pclr_get_sp_addr_h());
#endif
    return comb_to_32bits(sp_h, sp_l);
}

bool hal_cpu_trace_get_locked_regs(uint32_t cpu, uint32_t *pc, uint32_t *lr, uint32_t *sp)
{
    uint32_t retry = 0;

    if ((pc == NULL) || (lr == NULL) || (sp == NULL)) {
        return false;
    }

    do {
        /* try to lock registers */
        if (hal_cpu_trace_lock_trigger(cpu) != true) {
            return false;
        }

        /* get locked values and check */
        hal_cpu_trace_get_locked_reg(cpu, pc, lr, sp);
        if ((*pc != 0) || (*lr != 0)) {
            return true;
        }
        retry++;
    } while (retry < HAL_CPU_LOAD_RETRY_COUNT);
    return false;
}