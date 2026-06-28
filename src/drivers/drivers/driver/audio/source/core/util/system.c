/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: hifi system
 * Author: audio
 */

#include <xtensa/hal.h>
#include <xtensa/xtruntime.h>
#include "cache.h"
#include "securec.h"
#include "audio_osal.h"
#include "audio_debug.h"
#include "platform.h"
#include "adsp_interrupt.h"
#include "system.h"
#ifdef SAP_SUPPORT_RT_SMALL_MEM
#include "rt_smem.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef td_void (*handler_exception)(td_void);

typedef struct {
    td_s32 exccause;
    td_void (*handler)(UserFrame *uf, td_u32 cause);
} exception_info;

#define RA_OFFSET 16
#define SP_OFFSET 12
#define MAX_DEEP_TRACE 50

typedef struct {
    td_u32 uf_size;
    td_u32 cause;
    UserFrame uf;
} dump_info;

typedef struct {
    td_u32 cur_ra;
    td_u32 cur_sp;
    td_u32 val_sp;
} trace_info;

typedef struct {
    td_u32 epc;
    td_u32 depc;
    td_u32 exc_cause;
    td_u32 exc_vaddr;
    dump_info dump;
    trace_info trace[MAX_DEEP_TRACE];
    td_u32 trace_count;
} panic_info;

/* min boundary of stack (linker generated) */
extern char _stack_sentry[];
/* initial stack ptr (linker generated) */
extern char __stack[];
/* painc info addr */
extern char _work_bss_start[];

static panic_info *g_panic_info = (panic_info *)_work_bss_start;

static inline td_bool is_validfp(td_u32 sp)
{
    return (sp > ((td_u32)(uintptr_t)(_stack_sentry))) &&
        (sp <= ((td_u32)(uintptr_t)(__stack)));
}

static td_void back_trace(td_u32 sp, td_u32 ra, td_u32 max_deep)
{
    td_u32 cur_sp, prev_sp;
    td_u32 cur_ra, prev_ra;
    td_u32 deep = 0;

    cur_sp = sp;
    cur_ra = ra;

    audio_log0(AUDIO_LOG_LEVEL_FATAL, "/***Trace Beg***/");
    while (is_validfp(cur_sp)) {
        audio_log4(AUDIO_LOG_LEVEL_FATAL, "trace%u ra:0x%x sp:0x%x, 0x%x",
                   deep, cur_ra, cur_sp, *(volatile td_u32 *)(td_uintptr_t)(cur_sp));
        g_panic_info->trace[deep].cur_ra = cur_ra;
        g_panic_info->trace[deep].cur_sp = cur_sp;
        g_panic_info->trace[deep].val_sp = *(volatile td_u32 *)(td_uintptr_t)(cur_sp);

        deep++;
        prev_ra = cur_ra;
        prev_sp = cur_sp;
        cur_ra = *((td_u32 *)(uintptr_t)(prev_sp - RA_OFFSET));
        cur_sp = *((td_u32 *)(uintptr_t)(prev_sp - SP_OFFSET));

        if ((prev_sp == cur_sp) || (prev_ra == cur_ra) || (deep == max_deep)) {
            break;
        }
    }
    audio_log0(AUDIO_LOG_LEVEL_FATAL, "/***Trace End***/");
    g_panic_info->trace_count = deep;
}

static td_void dump_exception_frame(UserFrame *uf, td_u32 cause)
{
    if (!uf) {
        return;
    }

    audio_log0(AUDIO_LOG_LEVEL_FATAL, "/***Exception Dump Beg***/");
    audio_log2(AUDIO_LOG_LEVEL_FATAL, "sizeof(*uf) %u; cause %d", sizeof(*uf), cause);
    audio_log4(AUDIO_LOG_LEVEL_FATAL, "pc=0x%x; ps=0x%x; sar=0x%x vpri=0x%x", uf->pc, uf->ps, uf->sar, uf->vpri);
    audio_log4(AUDIO_LOG_LEVEL_FATAL, "a2=0x%x; a3=0x%x; a4=0x%x; a5=0x%x", uf->a2, uf->a3, uf->a4, uf->a5);
    audio_log3(AUDIO_LOG_LEVEL_FATAL, "lcount=0x%x; lbeg=0x%x lend=0x%x", uf->lcount, uf->lbeg, uf->lend);
    audio_log0(AUDIO_LOG_LEVEL_FATAL, "/***Exception Dump End***/");
    g_panic_info->dump.uf_size = sizeof(*uf);
    g_panic_info->dump.cause = cause;
    (td_void)memcpy_s(&g_panic_info->dump.uf, g_panic_info->dump.uf_size, uf, sizeof(*uf));

    audio_unused(cause);
}

static td_void exception_handler(UserFrame *uf, td_u32 cause)
{
    td_u32 epc;
    td_u32 depc;
    td_u32 exccause;
    td_u32 excvaddr;
    td_u32 sp;
    td_u32 ra;

    /* print the current registers related */
    __asm__ __volatile__("rsr %0, epc1" : "=a"(epc));
    __asm__ __volatile__("rsr %0, depc" : "=a"(depc));
    __asm__ __volatile__("rsr %0, exccause" : "=a"(exccause));
    __asm__ __volatile__("rsr %0, excvaddr" : "=a"(excvaddr));
    audio_log0(AUDIO_LOG_LEVEL_FATAL, "***Core Panic***");
    audio_log2(AUDIO_LOG_LEVEL_FATAL, "epc=0x%x; depc=0x%x", epc, depc);
    audio_log2(AUDIO_LOG_LEVEL_FATAL, "exccause: %d; excvaddr=0x%x", exccause, excvaddr);
    g_panic_info->epc = epc;
    g_panic_info->depc = depc;
    g_panic_info->exc_cause = exccause;
    g_panic_info->exc_vaddr = excvaddr;

    /* print the registers related to the dead point */
    dump_exception_frame(uf, cause);

    /* print call satck */
    __asm__ __volatile__("mov  %0,  a1" : "=a"(sp) : : "a1");
    __asm__ __volatile__("mov  %0,  a0" : "=a"(ra) : : "a0");
    xthal_window_spill();
    back_trace(sp, ra, MAX_DEEP_TRACE);

    while(1) { }
}

static const exception_info g_exception_infos[] = {
    {EXCCAUSE_ILLEGAL, exception_handler},      /* Illegal Instruction */
    {EXCCAUSE_SYSCALL, exception_handler},      /* System Call (SYSCALL instruction) */
    {EXCCAUSE_INSTR_ERROR, exception_handler},  /* Instruction Fetch Error */
    {EXCCAUSE_IFETCHERROR, exception_handler},     /* (backward compatibility macro, deprecated, avoid) */
    {EXCCAUSE_LOAD_STORE_ERROR, exception_handler},     /* Load Store Error */
    {EXCCAUSE_LOADSTOREERROR, exception_handler},       /* (backward compatibility macro, deprecated, avoid) */
    {EXCCAUSE_DIVIDE_BY_ZERO, exception_handler},       /* Integer Divide by Zero */
    {EXCCAUSE_SPECULATION, exception_handler},          /* Use of Failed Speculative Access (not implemented) */
    {EXCCAUSE_PRIVILEGED, exception_handler},           /* Privileged Instruction */
    {EXCCAUSE_UNALIGNED, exception_handler},            /* Unaligned Load or Store */
    /* Reserved 10..11 */
    {EXCCAUSE_INSTR_DATA_ERROR, exception_handler},     /* PIF Data Error on Instruction Fetch (RB-200x and later) */
    {EXCCAUSE_LOAD_STORE_DATA_ERROR, exception_handler},    /* PIF Data Error on Load or Store (RB-200x and later) */
    {EXCCAUSE_INSTR_ADDR_ERROR, exception_handler},     /* PIF Addr Error on Instruction Fetch (RB-200x and later) */
    {EXCCAUSE_LOAD_STORE_ADDR_ERROR, exception_handler},    /* PIF Addr Error on Load or Store (RB-200x and later) */
    {EXCCAUSE_ITLB_MISS, exception_handler},        /* ITLB Miss (no ITLB entry matches, hw refill also missed) */
    {EXCCAUSE_ITLB_MULTIHIT, exception_handler},    /* ITLB Multihit (multiple ITLB entries match) */
    {EXCCAUSE_INSTR_RING, exception_handler},       /* Ring Privilege Violation on Instruction Fetch */
    /* Reserved 19 */
    {EXCCAUSE_INSTR_PROHIBITED, exception_handler},     /* Cache Attribute does not allow Instruction Fetch */
    /* Reserved 21..23 */
    {EXCCAUSE_DTLB_MISS, exception_handler},            /* DTLB Miss (no DTLB entry matches, hw refill also missed) */
    {EXCCAUSE_DTLB_MULTIHIT, exception_handler},        /* DTLB Multihit (multiple DTLB entries match) */
    {EXCCAUSE_LOAD_STORE_RING, exception_handler},      /* Ring Privilege Violation on Load or Store */
    /* Reserved 27 */
    {EXCCAUSE_LOAD_PROHIBITED, exception_handler},      /* Cache Attribute does not allow Load */
    {EXCCAUSE_STORE_PROHIBITED, exception_handler},     /* Cache Attribute does not allow Store */
    /* Reserved 30..31 */
    {EXCCAUSE_CP0_DISABLED, exception_handler},     /* Access to Coprocessor 0 when disabled */
    {EXCCAUSE_CP1_DISABLED, exception_handler},     /* Access to Coprocessor 1 when disabled */
    {EXCCAUSE_CP2_DISABLED, exception_handler},     /* Access to Coprocessor 2 when disabled */
    {EXCCAUSE_CP3_DISABLED, exception_handler},     /* Access to Coprocessor 3 when disabled */
    {EXCCAUSE_CP4_DISABLED, exception_handler},     /* Access to Coprocessor 4 when disabled */
    {EXCCAUSE_CP5_DISABLED, exception_handler},     /* Access to Coprocessor 5 when disabled */
    {EXCCAUSE_CP6_DISABLED, exception_handler},     /* Access to Coprocessor 6 when disabled */
    {EXCCAUSE_CP7_DISABLED, exception_handler}      /* Access to Coprocessor 7 when disable */
    /* Reserved 40..63 */
};

static td_void register_exceptions(td_void)
{
    td_s32 i;
    const td_s32 n = sizeof (g_exception_infos) / sizeof (g_exception_infos[0]);

    for (i = 0; i < n; i++) {
        dsp_set_exception_handler(g_exception_infos[i].exccause,
            (handler_exception)g_exception_infos[i].handler);
    }
}

#ifdef SAP_SUPPORT_RT_SMALL_MEM
extern td_u32 _smem_head_start[];
extern td_u32 _smem_tail_start[];
#endif

#ifdef SAP_MEM_STATISTIC
static td_void mark_stack(td_void)
{
    td_u32 *c = TD_NULL;
    for (c = (td_u32 *)_stack_sentry; c < (td_u32 *)&c; c++) {
        *c = STACK_MARKER;
    }
}
#endif

td_void system_init(td_void)
{
    cache_init();
    audio_osal_serial_init();
    register_exceptions();
#ifdef SAP_SUPPORT_RT_SMALL_MEM
    rt_smem_init((td_void *)(td_uintptr_t)_smem_head_start, (td_uintptr_t)_smem_tail_start -
        (td_uintptr_t)_smem_head_start);
#endif
#ifdef SAP_MEM_STATISTIC
    mark_stack();
#endif
}

td_void system_deinit(td_void)
{
    audio_func_enter();
#ifdef SAP_SUPPORT_RT_SMALL_MEM
    rt_smem_deinit();
#endif
    dsp_int_off_all(); /* disable all interrupts to make sure dsp core in idle state */
    system_idle(); /* dsp enter idle state until power off */
    audio_func_exit();
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
