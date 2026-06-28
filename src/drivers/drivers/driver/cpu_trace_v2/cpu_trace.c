/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description:  CPU Trace Driver
 *
 * Create:
 */
#include "cpu_trace_unified.h"
#include "arch_barrier.h"
#include "common_def.h"
#include "hal_cpu_trace.h"
#include "securec.h"
#include "cpu_trace_porting.h"
#include "soc_osal.h"

void cpu_trace_port_enable(void);

#define CPUTRACE_MEM var_segment("cputrace_mem")

CPUTRACE_MEM cpu_trace_item_t cpu_trace_arry[CPU_TRACE_MEM_REGION_LENGTH / sizeof(cpu_trace_item_t)];
/* Store for mem analyze */
volatile uint32_t g_cpu_trace_start_addr = 0x0;
volatile uint32_t g_cpu_trace_end_addr = 0x0;
static bool g_cpu_enable_flag = false;
#if defined(CONFIG_CPU_TRACE_SUPPORT_LPM)
static bool g_cpu_suspend_flag = false;
#endif

void __attribute__((weak)) cpu_trace_lock_pclr_open_clock(void)
{
}

void __attribute__((weak)) cpu_trace_lock_pclr_close_clock(void)
{
}

void uapi_cpu_trace_enable(uint32_t cpu)
{
    cpu_trace_port_enable();
    g_cpu_trace_start_addr = (uint32_t)cpu_trace_arry;
    g_cpu_trace_end_addr = (uint32_t)cpu_trace_arry + (uint32_t)(sizeof(cpu_trace_arry) - 1);
    memset_s((void *)cpu_trace_arry, sizeof(cpu_trace_arry), 0x0, sizeof(cpu_trace_arry));
    hal_cpu_trace_enable(cpu);
    g_cpu_enable_flag = true;
}

void uapi_cpu_trace_disable(uint32_t cpu)
{
    dsb(); /* flush cpu before disable trace */
    hal_cpu_trace_disable(cpu);
    g_cpu_enable_flag = false;
}

void uapi_cpu_trace_enable_repeat(uint32_t cpu)
{
    memset_s((void *)cpu_trace_arry, sizeof(cpu_trace_arry), 0x0, sizeof(cpu_trace_arry));
    hal_cpu_trace_enable(cpu);
}

void uapi_cpu_trace_pclr_set_passthrough(uint32_t cpu)
{
    hal_cpu_trace_set_pclr_passthrough(cpu);
}

void uapi_cpu_trace_lock_pclr(uint32_t cpu)
{
    cpu_trace_lock_pclr_open_clock();
    hal_cpu_trace_lock_pclr(cpu);
    cpu_trace_lock_pclr_close_clock();
}

uint32_t uapi_cpu_trace_get_locked_pc(uint32_t cpu)
{
    return hal_cpu_trace_get_locked_pc(cpu);
}

uint32_t uapi_cpu_trace_get_locked_lr(uint32_t cpu)
{
    return hal_cpu_trace_get_locked_lr(cpu);
}

uint32_t uapi_cpu_trace_get_locked_sp(uint32_t cpu)
{
    return hal_cpu_trace_get_locked_sp(cpu);
}

bool uapi_cpu_trace_get_locked_regs(uint32_t cpu, uint32_t *pc, uint32_t *lr, uint32_t *sp)
{
    bool flag;
    uint32_t status = osal_irq_lock();
    flag = hal_cpu_trace_get_locked_regs(cpu, pc, lr, sp);
    osal_irq_restore(status);
    return flag;
}

#if defined(CONFIG_CPU_TRACE_SUPPORT_LPM)
errcode_t uapi_cpu_trace_suspend(uintptr_t arg)
{
    unused(arg);
    if (g_cpu_enable_flag == true) {
        g_cpu_suspend_flag = true;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_cpu_trace_resume(uintptr_t arg)
{
    unused(arg);
    if (g_cpu_suspend_flag == true) {
        hal_cpu_trace_enable(cpu_trace_get_self_id());
        g_cpu_suspend_flag = false;
    }
    return ERRCODE_SUCC;
}
#endif