/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: hard riscv interface
 * Author: @CompanyNameTag
 * Create:  2021-11-04
 */
#include "platform_core.h"
#include "exception.h"
#include "chip_io.h"
#include "preserve.h"
#include "debug_print.h"
#include "reboot_porting.h"
#include "securec.h"
#include "non_os_reboot.h"
#include "hal_reboot.h"
#include "watchdog_porting.h"
#include "crash_data.h"
#include "sys/time.h"
#include "time.h"
#if defined (SUPPORT_CPU_TRACE) || defined (SUPPORT_CPU_TRACE_V2)
#include "cpu_trace_unified.h"
#include "memory_config_common.h"
#include "cpu_trace_porting.h"
#endif
#ifdef SUPPORT_WATCHDOG
#include "watchdog.h"
#endif
#ifdef SUPPORT_CRASH_DATA_RAM
#include "dfx_adapt_layer.h"
#include "crash_data.h"
#endif
#ifdef IPC_NEW
#include "ipc.h"
#include "ipc_new_shared.h"
#endif
#if (CORE == APPS)
#include "cpu_utils.h"
#endif

#include "non_os.h"
#include "oam_trace.h"
#ifndef USE_CMSIS_OS
#include "arch_encoding.h"
#include "vectors.h"
#include "interrupt_handler.h"
#endif
#include "tcxo.h"
#if defined(HSO_SUPPORT)
#include "diag_log.h"
#endif
#include "print_config.h"
#if defined(__LITEOS__)
#include "soc_osal.h"
#include "osal_adapt.h"
#include "los_sched_pri.h"
#include "los_task_pri.h"
#include "los_memstat_pri.h"
#include "los_memory_pri.h"
#include "los_exc.h"
#include "los_task.h"
#include "osal_task.h"
#endif
#include "fcntl.h"
#include "unistd.h"
#if defined SAVE_EXC_INFO
#include "dfx_adapt_layer.h"
#if defined(DFX_YES) && defined(CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM) && \
    (CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM == DFX_YES)
#include "sys/stat.h"
#include "dfx_file_operation.h"
#if defined CFG_DRIVERS_MMC
#include "block.h"
#endif
#endif
#endif
#if defined(CFG_DRIVERS_NANDFLASH)
#include "nand_fmc.h"
#endif

#define USER_STACK_OFFSET_PER   4
#define USER_STACK_PRINT_DEPTH  30
#define MAX_CALL_STACK          20
#define CALLSTACK_RA_POS        4
#define CALLSTACK_FP_POS        8
#define STACK_BOTTROM_INIT_VAL  0xf0f0f0f
#define MCAUSE_MASK             0x80000FFF
#define NMI_INTERRUPT           0x00000FFF
#define SP_OFFSET               0x10
#define CONTEXT_OFFSET          (4 * 4 + 33 * 4 + 4)
#define  RISCV_TRACE_MEM_REGION_START   TRACE_MEM_REGION_START
#define  RISCV_TRACE_MEM_REGION_LENGTH  TRACE_MEM_REGION_LENGTH

#if defined(SAVE_EXC_INFO) && defined(CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM) && \
    (CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM == DFX_YES)
#define DEFAULT_READ_WRITE_PERMISSIONS   0755
#define SHARED_MEM_SAVE_LENGTH           0x10000  // 64K
char *g_comram_copy_path = 0;
#endif

#if defined(HSO_SUPPORT) && CORE == BT
#define exc_info_log(fmt, arg...)  uapi_diag_info_log(0, fmt, ##arg)
#else
#define exc_info_log(fmt, args...) PRINT(fmt, ##args)
#endif

#ifdef LOSCFG_EXC_SIMPLE_INFO
static const char *g_mcause_info[] = {
    "Instruction address misaligned",
    "Instruction access fault",
    "Illegal instruction",
    "Breakpoint",
    "Load address misaligned",
    "Load access fault",
    "Store/AMO address misaligned",
    "Store/AMO access fault",
    "Environment call from U-mode",
    "Environment call from S-mode",
    "Reserved",
    "Environment call from M-mode",
    "Instruction page fault",
    "Load page fault",
    "Reserved",
    "Store page fault",
    "Hard fault",         /* Reserved exception code */
    "Lock up"             /* Reserved exception code */
};
#ifdef LOSCFG_ARCH_LINX_M
#define REG_CCAUSE_CAUSE_MASK   0x1F
static const char *g_ccause_info[] = {
    "Not available",
    "Memory map region access fault",
    "MMEM BUS error response",
    "PMEM BUS error response",
    "Crossing PMP entries",
    "System register access fault",
    "No PMP entry matched",
    "PMP access fault",
    "Reserved",
    "CSR access fault",
    "Reserved",
    "Reserved",
    "I/D-cache parity check error",
    "Dcache evict BUS error response",
    "Asynchronous Dcache parity check error",
    "Reserved",
    "Dcache 2bit ECC error",
    "Reserved",
    "Asynchronous Dcache 2bit ECC error",
    "Device access fault",
    "Atomic access fault",
    "Uopcnt out of range",
#ifndef LOSCFG_ARCH_RISCV_TES
    "TCM error response"
#else
    "TCM error response",
    "Trust related access violation",
    "Untrusted TESVEC table",
    "Illegal TES transition",
    "Marker expected",
    "Stack pointer limit exceeded"
#endif
};
#else
#define REG_CCAUSE_CAUSE_MASK   0xF
STATIC const char *g_ccause_info[] = {
    "Not available",
    "Memory map region access fault",
    "AXIM error response",
    "AHBM error response",
    "Crossing PMP entries",
    "Reserved",
    "No PMP entry matched",
    "PMP access fault",
    "CMO access fault",
    "CSR access fault",
    "LDM/STMIA instruction fault",
    "ITCM write access fault"
};
#endif
#endif

#if defined(USE_CMSIS_OS) && defined(__LITEOS__)
extern VOID OsExcInfoDisplay(uint32_t excType, const ExcContext *excBufAddr);
extern ExcInfo g_excInfo;
#endif
static void deal_with_exception(exc_context_t *exc_buf_addr, uint32_t exc_type, reboot_cause_t reset_cause);

static hal_exception_dump_callback g_exception_dump_callback = NULL;

nmi_proc_func g_nmi_hook = do_hard_fault_handler;

exc_info_t g_exc_info;
// Check whether the exception is repeated
#if (CORE == MASTER_BY_ALL) && (CORE != WIFI)
bool g_is_repeat_exc = false;
#endif

#ifdef SUPPORT_CRASH_DATA_RAM
/* g_xregs_map中存放各寄存器在task_context_t结构体中的偏移量（基于成员s11）  */
static const uint8_t g_xregs_map[] = {
/*  t0  t1  t2  s0  s1  a0 */
    26, 25, 24, 11, 10, 23,
/*  a1  a2  a3  a4  a5  a6  a7 */
    22, 21, 20, 19, 18, 17, 16,
/*  s2  s3  s4  s5  s6  s7  s8 */
    9, 8, 7, 6, 5,  4,  3,
/*  s9  s10 s11 t3  t4  t5  t6 */
    2,  1,  0,  15, 14, 13, 12
};

#if defined(DFX_YES) && defined(CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM) && \
    (CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM == DFX_YES) && defined(__LITEOS__) && defined(LOSCFG_MEM_DFX_SHOW_CALLER_RA)
#define MAX_INFO_BUF_LEN        256
static int g_dump_os_info_file = -1;
static char *g_dump_os_info_path = "/user/exc";
static char *g_dump_os_info_name = "dump_os_info.log";
char g_dump_os_buf[MAX_INFO_BUF_LEN];
#endif
#endif

void exc_info_display(const exc_info_t *exc)
{
    if (exc == NULL) {
        return;
    }
    PRINT("**************Exception Information************** \n");
    PRINT("uwExcType = 0x%x\n", exc->type);

    PRINT("mepc       = 0x%x\n", exc->context->task_context.mepc);
    PRINT("mstatus    = 0x%x\n", exc->context->task_context.mstatus);
    PRINT("mtval      = 0x%x\n", exc->context->mtval);
    PRINT("mcause     = 0x%x\n", exc->context->mcause);
    PRINT("ccause     = 0x%x\n", exc->context->ccause);
    PRINT("ra         = 0x%x\n", exc->context->task_context.ra);
    PRINT("sp         = 0x%x\n", exc->context->task_context.sp);
    PRINT("gp         = 0x%x\n", exc->context->gp);
    PRINT("tp         = 0x%x\n", exc->context->task_context.tp);
    PRINT("t0         = 0x%x\n", exc->context->task_context.t0);
    PRINT("t1         = 0x%x\n", exc->context->task_context.t1);
    PRINT("t2         = 0x%x\n", exc->context->task_context.t2);
    PRINT("s0         = 0x%x\n", exc->context->task_context.s0);
    PRINT("s1         = 0x%x\n", exc->context->task_context.s1);
    PRINT("a0         = 0x%x\n", exc->context->task_context.a0);
    PRINT("a1         = 0x%x\n", exc->context->task_context.a1);
    PRINT("a2         = 0x%x\n", exc->context->task_context.a2);
    PRINT("a3         = 0x%x\n", exc->context->task_context.a3);
    PRINT("a4         = 0x%x\n", exc->context->task_context.a4);
    PRINT("a5         = 0x%x\n", exc->context->task_context.a5);
    PRINT("a6         = 0x%x\n", exc->context->task_context.a6);
    PRINT("a7         = 0x%x\n", exc->context->task_context.a7);
    PRINT("s2         = 0x%x\n", exc->context->task_context.s2);
    PRINT("s3         = 0x%x\n", exc->context->task_context.s3);
    PRINT("s4         = 0x%x\n", exc->context->task_context.s4);
    PRINT("s5         = 0x%x\n", exc->context->task_context.s5);
    PRINT("s6         = 0x%x\n", exc->context->task_context.s6);
    PRINT("s7         = 0x%x\n", exc->context->task_context.s7);
    PRINT("s8         = 0x%x\n", exc->context->task_context.s8);
    PRINT("s9         = 0x%x\n", exc->context->task_context.s9);
    PRINT("s10        = 0x%x\n", exc->context->task_context.s10);
    PRINT("s11        = 0x%x\n", exc->context->task_context.s11);
    PRINT("t3         = 0x%x\n", exc->context->task_context.t3);
    PRINT("t4         = 0x%x\n", exc->context->task_context.t4);
    PRINT("t5         = 0x%x\n", exc->context->task_context.t5);
    PRINT("t6         = 0x%x\n", exc->context->task_context.t6);

    PRINT("**************Exception Information end************** \n");
    return;
}

// for loaderboot&ssb with non-os
#ifndef USE_CMSIS_OS
static void do_process_exception(exc_context_t *exc_buff_addr)
{
    g_exc_info.type = (uint16_t)(exc_buff_addr->mcause);
    g_exc_info.context = exc_buff_addr;
    exc_info_display(&g_exc_info);
}

void do_trap_unknown(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - unknown exception\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_insn_misaligned(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - instruction address misaligned\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_insn_fault(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - instruction access fault\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_insn_illegal(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - illegal instruction\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_load_misaligned(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - load address misaligned\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_load_fault(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - load access fault\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_store_misaligned(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - store (or AMO) address misaligned\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_store_fault(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - store (or AMO) access fault\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_ecall_u(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - environment call from U-mode\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_ecall_s(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - environment call from S-mode\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_ecall_m(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - environment call from M-mode\r\n");
    do_process_exception(exc_buff_addr);
}

void do_trap_break(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - ebreak\r\n");
    do_process_exception(exc_buff_addr);
}

void do_insn_page_fault(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - insn page fault\r\n");
    do_process_exception(exc_buff_addr);
}

void do_load_page_fault(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - load page fault\r\n");
    do_process_exception(exc_buff_addr);
}

void do_store_page_fault(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - store page fault\r\n");
    do_process_exception(exc_buff_addr);
}

void do_hard_fault(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - hard fault\r\n");
    do_process_exception(exc_buff_addr);
}

void do_lockup(exc_context_t *exc_buff_addr)
{
    PRINT("Oops - lock up\r\n");
    do_process_exception(exc_buff_addr);
}

void default_handler(void)
{
    uint32_t hwi_index = interrupt_number_get();
    PRINT("default_handler : interrupt idx[%d]\n", hwi_index);
    while (true) { }
    unused(hwi_index);
}

void nmi_handler(void)
{
    uint32_t hwi_index = interrupt_number_get();
    PRINT("nmi_handler : interrupt idx[%d]\n", hwi_index);
    while (true) { }
    unused(hwi_index);
}
#endif

#if defined(SUPPORT_CRASH_DATA_RAM) && defined(USE_CMSIS_OS) && defined(__LITEOS__)
static void print_exec_stack_info(exc_info_save_t *exc_info)
{
#ifdef SUPPORT_CALLSTACK
    exc_info_log("*******dump call stack begin : fp = 0x%x*******\n", exc_info->s0);
    for (uint32_t i = 0; i < exc_info->stack_size; i++) {
        exc_info_log("call stack %d -- ra = 0x%x    fp = 0x%x\n", i, exc_info->stack[i].ra, exc_info->stack[i].fp);
    }
    exc_info_log("*******dump call stack end*******\n");
#else
    exc_info_log("*******backtrace begin*******\n");
    for (uint32_t i = 0; i < exc_info->backtrace_size; i++) {
        exc_info_log("traceback %d -- sp addr= 0x%x    sp content= 0x%x\n",
            i, exc_info->backtrace[i].sp_addr, exc_info->backtrace[i].sp_content);
    }
    exc_info_log("*******backtrace end*******\n");
#endif
}

static void print_system_exec_info(exc_info_save_t *exc_info)
{
    exc_info_log("**************Exception Information************** \n");
    exc_info_log("task_name  = %s\n", exc_info->task_name);
    exc_info_log("thrd_pid   = 0x%x\n", exc_info->thrd_pid);
    exc_info_log("type       = 0x%x\n", exc_info->type);
    exc_info_log("nest_cnt   = 0x%x\n", exc_info->nest_cnt);
#ifndef LOSCFG_EXC_SIMPLE_INFO
    exc_info_log("phase      = %s\n", exc_info->phase);
#else
    exc_info_log("phase      = 0x%x\n", exc_info->phase);
#endif
    exc_info_log("ccause     = 0x%x\n", exc_info->ccause);
    exc_info_log("mcause     = 0x%x\n", exc_info->mcause);
    exc_info_log("mtval      = 0x%x\n", exc_info->mtval);
    exc_info_log("gp         = 0x%x\n", exc_info->gp);
    exc_info_log("mstatus    = 0x%x\n", exc_info->mstatus);
    exc_info_log("mepc       = 0x%x\n", exc_info->mepc);
    exc_info_log("ra         = 0x%x\n", exc_info->ra);
    exc_info_log("sp         = 0x%x\n", exc_info->sp);
    exc_info_log("tp         = 0x%x\n", exc_info->tp);
    exc_info_log("t0         = 0x%x\n", exc_info->t0);
    exc_info_log("t1         = 0x%x\n", exc_info->t1);
    exc_info_log("t2         = 0x%x\n", exc_info->t2);
    exc_info_log("s0         = 0x%x\n", exc_info->s0);
    exc_info_log("s1         = 0x%x\n", exc_info->s1);
    exc_info_log("a0         = 0x%x\n", exc_info->a0);
    exc_info_log("a1         = 0x%x\n", exc_info->a1);
    exc_info_log("a2         = 0x%x\n", exc_info->a2);
    exc_info_log("a3         = 0x%x\n", exc_info->a3);
    exc_info_log("a4         = 0x%x\n", exc_info->a4);
    exc_info_log("a5         = 0x%x\n", exc_info->a5);
    exc_info_log("a6         = 0x%x\n", exc_info->a6);
    exc_info_log("a7         = 0x%x\n", exc_info->a7);
    exc_info_log("s2         = 0x%x\n", exc_info->s2);
    exc_info_log("s3         = 0x%x\n", exc_info->s3);
    exc_info_log("s4         = 0x%x\n", exc_info->s4);
    exc_info_log("s5         = 0x%x\n", exc_info->s5);
    exc_info_log("s6         = 0x%x\n", exc_info->s6);
    exc_info_log("s7         = 0x%x\n", exc_info->s7);
    exc_info_log("s8         = 0x%x\n", exc_info->s8);
    exc_info_log("s9         = 0x%x\n", exc_info->s9);
    exc_info_log("s10        = 0x%x\n", exc_info->s10);
    exc_info_log("s11        = 0x%x\n", exc_info->s11);
    exc_info_log("t3         = 0x%x\n", exc_info->t3);
    exc_info_log("t4         = 0x%x\n", exc_info->t4);
    exc_info_log("t5         = 0x%x\n", exc_info->t5);
    exc_info_log("t6         = 0x%x\n", exc_info->t6);

    print_exec_stack_info(exc_info);
    exc_info_log("**************Exception Information end************** \n");
}
#endif

static void disable_watchdog(void)
{
#ifdef SUPPORT_WATCHDOG
    /* If the clock of the watchdog is not enabled, the next operation register triggers an exception. */
    watchdog_turnon_clk();
    uapi_watchdog_disable();
    watchdog_turnoff_clk();
#endif
}

static reboot_cause_t hal_set_reset_cause(uint32_t id)
{
#if CORE == APPS
    return (id == NMI_INTERRUPT) ? REBOOT_CAUSE_APPLICATION_NNMIFAULT : REBOOT_CAUSE_APPLICATION_HARDFAULT;
#else
    return (id == NMI_INTERRUPT) ? REBOOT_CAUSE_BT_NNMIFAULT : REBOOT_CAUSE_BT_HARDFAULT;
#endif
}

static void hal_save_reg_info(exc_context_t *exc_buf_addr)
{
    exc_buf_addr->task_context.sp += SP_OFFSET;
    set_exception_info_riscv(exc_buf_addr);
}

#if defined(USE_CMSIS_OS) && defined(SUPPORT_CRASH_DATA_RAM)
#ifdef SUPPORT_CALLSTACK
#if defined(__LITEOS__)
static INLINE bool fp_align(uint32_t value)
{
    return (((uint32_t)(value) & (uint32_t)(LOSCFG_STACK_POINT_ALIGN_SIZE - 1)) == 0);
}

static bool is_valid_fp(uint32_t regFP, uint32_t start, uint32_t end)
{
    return (regFP > start) && (regFP <= end) && fp_align(regFP);
}

static bool find_suitable_stack(uint32_t fp, uint32_t *start, uint32_t *end)
{
    if (fp == 0) {
        preserve_task_context_t *context = get_exc_task_context();
        if (context != NULL) {
            fp = context->s0;
            oam_info_log("fp = %x\r\n", fp);
        } else {
            oam_info_log("not find vaild fp.\r\n");
            return FALSE;
        }
    }
    uint32_t stack_start;
    uint32_t stack_end;
    bool found = FALSE;
    LosTaskCB *task_cb = NULL;
    /* Search in the task stacks */
    for (uint32_t index = 0; index < g_taskMaxNum; index++) {
        task_cb = OS_TCB_FROM_TID(index);
        if (task_cb->taskStatus & OS_TASK_STATUS_UNUSED) {
            continue;
        }
        stack_start = task_cb->topOfStack;
        stack_end = task_cb->topOfStack + (uint32_t)task_cb->stackSize;
        if (is_valid_fp(fp, stack_start, stack_end)) {
            found = TRUE;
            goto FOUND;
        }
    }
    /* Search in the exc stacks */
    stack_start = (uint32_t)(uintptr_t)&__int_stack_start;
    stack_end = (uint32_t)(uintptr_t)&__int_stack_end;
    if (is_valid_fp(fp, stack_start, stack_end)) {
        found = TRUE;
    }
FOUND:
    if (found == TRUE) {
        *start = stack_start;
        *end = stack_end;
    }
    return found;
}
#endif

static void get_stack_size(uint32_t fp, uint16_t *stack_size)
{
    uint32_t back_fp = fp;
    uint32_t tmp_fp;

#if defined(__LITEOS__)
    uint32_t stack_start;
    uint32_t stack_end;
    if (find_suitable_stack(fp, &stack_start, &stack_end) == FALSE) {
        PRINT("fp error, backtrace failed!\n");
        return ;
    }
#endif
    while ((back_fp != 0) && (back_fp != STACK_BOTTROM_INIT_VAL)) {
        tmp_fp = back_fp;
        back_fp = *((uint32_t *)(uintptr_t)(tmp_fp - CALLSTACK_FP_POS));
        (*stack_size)++;
#if defined(__LITEOS__)
        if (!is_valid_fp(back_fp, stack_start, stack_end)) {
            break;
        }
#endif
        if ((*stack_size == MAX_CALL_STACK) || (back_fp == tmp_fp)) {
            break;
        }
    }
}

static void set_exc_stack_info(exc_info_save_t *exc_info_val, uint32_t fp)
{
    exc_stack_info_t *stack = exc_info_val->stack;
    uint32_t back_fp = fp;
    uint32_t tmp_fp;
    uint32_t back_ra;
    uint32_t stack_size = exc_info_val->stack_size;
    for (uint32_t i = 0; i < stack_size; i++) {
        tmp_fp = back_fp;
        back_ra = *((uint32_t *)(uintptr_t)(tmp_fp - CALLSTACK_RA_POS));
        back_fp = *((uint32_t *)(uintptr_t)(tmp_fp - CALLSTACK_FP_POS));
        stack[i].ra = back_ra;
        stack[i].fp = back_fp;
    }
}
#else
static void get_stack_size(uint32_t sp, uint16_t *stack_size)
{
    uint32_t back_sp = sp;
    while (back_sp != 0) {
        back_sp = back_sp + USER_STACK_OFFSET_PER;
        (*stack_size)++;
        if (*stack_size == USER_STACK_PRINT_DEPTH) {
            break;
        }
    }
}

static void set_exc_stack_info(exc_info_save_t *exc_info_val, uint32_t sp)
{
    exc_stack_info_t *stack = exc_info_val->backtrace;
    uint32_t back_sp = sp;
    uint32_t backtrace_size = exc_info_val->backtrace_size;
    for (uint32_t i = 0; i < backtrace_size; i++) {
        back_sp = back_sp + USER_STACK_OFFSET_PER;
        stack[i].sp_addr = back_sp;
        stack[i].sp_content = *((uint32_t *)(back_sp));
    }
}
#endif

static void set_exc_context_info(const task_context_t *task_context, exc_info_save_t *exc_info)
{
    exc_info->mstatus = task_context->mstatus;
    exc_info->mepc = task_context->mepc;
    exc_info->ra = task_context->ra;
    exc_info->sp = task_context->sp;
    exc_info->tp = task_context->tp;

    uint32_t t0_offset = offsetof(exc_info_save_t, t0);
    uint32_t s11_offset = offsetof(task_context_t, s11);
    const uint32_t *context_regs = (const uint32_t *)((uintptr_t)task_context + s11_offset);
    uint32_t *context_exc_info = (uint32_t *)((uintptr_t)exc_info + t0_offset);

    for (uint32_t i = 0; i < sizeof(g_xregs_map) / sizeof(g_xregs_map[0]); i++) {
        context_exc_info[i] = context_regs[g_xregs_map[i]];
    }

#ifdef SUPPORT_CALLSTACK
    set_exc_stack_info(exc_info, task_context->s0);
#elif defined(__LITEOS__)
    set_exc_stack_info(exc_info, task_context->sp);
#endif
}

#ifndef LOSCFG_EXC_SIMPLE_INFO
static const CHAR *g_exc_occur_stage[] = {
    "Init",
    "Task",
    "Irq",
    "Exc"
};
#endif

static errcode_t exc_info_write(uint32_t exc_type, uint32_t phase, exception_info_t *exception_info,
    const exc_context_t *exc_buf_addr, exc_info_save_t *exc_info)
{
    uint16_t stack_size = 0;
#ifdef SUPPORT_CALLSTACK
    get_stack_size(exc_buf_addr->task_context.s0, &stack_size);
#elif defined(__LITEOS__)
    get_stack_size(exc_buf_addr->task_context.sp, &stack_size);
#endif
    uint32_t exc_size = (uint32_t)EXC_INFO_SAVE_SIZE + (stack_size * EXC_STACK_INFO_SIZE);
    if (memset_s(exc_info, exc_size, 0, exc_size) != EOK) {
        return ERRCODE_FAIL;
    }

#ifdef SUPPORT_CALLSTACK
    exc_info->stack_size = stack_size;
#elif defined(__LITEOS__)
    exc_info->backtrace_size = stack_size;
#endif

#if defined(__LITEOS__)
    char *task_name = OsCurTaskNameGet();
    if (strcpy_s(exc_info->task_name, sizeof(exc_info->task_name), task_name) != EOK) {
        PRINT("%s %d strcpy_s failed\r\n", __FUNCTION__, __LINE__);
    }
    exc_info->thrd_pid = exception_info->exp_task_id;
    exc_info->type = (uint16_t)exc_type;
    exc_info->nest_cnt = g_excInfo.nestCnt;
#ifndef LOSCFG_EXC_SIMPLE_INFO
    if (strcpy_s(exc_info->phase, sizeof(exc_info->phase), g_exc_occur_stage[phase])) {
        PRINT("%s %d strcpy_s failed\r\n", __FUNCTION__, __LINE__);
    }
#else
    exc_info->phase = (uint16_t)phase;
#endif
#endif /* __LITEOS__ */
    exc_info->ccause = exc_buf_addr->ccause;
    exc_info->mcause = exc_buf_addr->mcause;
    exc_info->mtval = exc_buf_addr->mtval;
    exc_info->gp = exc_buf_addr->gp;
    set_exc_context_info((const task_context_t *)&exc_buf_addr->task_context, exc_info);
    crash_data_set_exception_info(exception_info);

    return ERRCODE_SUCC;
}
#endif

#if defined(__LITEOS__)
static errcode_t get_exception_task_info(exception_info_t *exception_info, uint32_t *phase)
{
    uint32_t phase_tmp;
    uint32_t thrd_pid;
    osal_tsk_info_s taskinfo = {0};
    if (g_excInfo.nestCnt > 1) { /* Exception nesting level is 1 */
        phase_tmp = OS_EXC_STAGE_EXC;
        thrd_pid = OS_EXC_STAGE_INIT_VALUE;
    } else if (!OS_SCHEDULER_ACTIVE) {
        phase_tmp = OS_EXC_STAGE_INIT;
        thrd_pid = OS_EXC_STAGE_INIT_VALUE;
    } else if (OS_INT_ACTIVE != 0) {
        phase_tmp = OS_EXC_STAGE_IRQ;
        thrd_pid = OS_EXC_STAGE_INIT_VALUE;
    } else {
        phase_tmp = OS_EXC_STAGE_TASK;
        thrd_pid = (UINT32)osal_get_current_tid();
        (void)osal_taskinfo_get(thrd_pid, &taskinfo);
    }

    exception_info->exp_task_id = thrd_pid;
    exception_info->sp = (UINTPTR)taskinfo.uwSP;
    exception_info->sp_bottom = (UINTPTR)taskinfo.uwBottomOfStack;
    exception_info->task_array = (unsigned long)(uintptr_t)OS_TCB_FROM_TID(0);
    exception_info->task_max_num = g_taskMaxNum + 1;
    *phase = phase_tmp;
    return ERRCODE_SUCC;
}
#endif

#if defined(__LITEOS__) && defined(LOSCFG_MEM_DFX_SHOW_CALLER_RA) && defined(SAVE_EXC_INFO) && \
    defined(DFX_YES) && defined(CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM) && \
    (CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM == DFX_YES)
static VOID print_info_to_file(const char *fmt, ...)
{
    va_list args;

    if (g_dump_os_info_file < 0) {
        PRINT("%s %d file is null\r\n", __FUNCTION__, __LINE__);
        return;
    }
    // 格式化字符串
    va_start(args, fmt);
    int len = vsnprintf_s(g_dump_os_buf, sizeof(g_dump_os_buf), sizeof(g_dump_os_buf) - 1, fmt, args);
    va_end(args);

    if (len <= 0) {
        return;
    }

    int write_len = write(g_dump_os_info_file, g_dump_os_buf, len);
    if (write_len < len) {
        PRINT("%s %d write_len:%d len:%d\r\n", __FUNCTION__, __LINE__, write_len, len);
    }
}

static void printBatchRaStats(CallerRaMemStat *raStats, uint32_t raStatsCnt)
{
    for (uint32_t i = 0; i < raStatsCnt; i++) {
        print_info_to_file("callerRA=0x%08x : totalSize=%08d Bytes, blockCount=% 2d\r\n",
            raStats[i].callerRa,
            raStats[i].totalSize,
            raStats[i].blockCount);
    }
}

static bool updateRaStats(CallerRaMemStat *raStats, uint32_t *raStatsCnt, uintptr_t callerRa, uint32_t size)
{
    uint32_t raCnt = *raStatsCnt;
    for (uint32_t i = 0; i < raCnt; i++) {
        if (raStats[i].callerRa == callerRa) {
            raStats[i].totalSize += size;
            raStats[i].blockCount++;
            return TRUE;
        }
    }
    if (raCnt < RA_BATCH_SIZE) {
        raStats[raCnt].callerRa = callerRa;
        raStats[raCnt].totalSize = size;
        raStats[raCnt].blockCount = 1;
        (raCnt)++;
        *raStatsCnt = raCnt;
        return TRUE;
    }
    return FALSE;
}


static bool OsMemNodeIsUsed(LosMemDynNode *node)
{
    if (node == NULL) {
        return FALSE;
    }
    return ((OS_MEM_NODE_GET_USED_FLAG(node->selfNode.sizeAndFlag) == OS_MEM_NODE_USED_FLAG) ? TRUE : FALSE);
}

static bool checkUnflaggedNodes(LosMemDynNode *endNode, uint8_t taskId)
{
    LosMemDynNode *tmpNode;
    for (tmpNode = endNode->selfNode.preNode; tmpNode != endNode; tmpNode = tmpNode->selfNode.preNode) {
        if ((!RA_IS_FLAGGED(tmpNode->selfNode.callerRa)) && (tmpNode->selfNode.taskId == taskId) &&
            OsMemNodeIsUsed(tmpNode)) {
            return FALSE;
        }
    }
    return TRUE;
}

static uint32_t collectCallerRaStatsBatch(
    LosMemDynNode *endNode, uint8_t taskId, CallerRaMemStat *raStats,
    uint32_t *totalCostSize, uint32_t *totalPoolUsedSize)
{
    uint32_t raStatsCnt = 0;
    LosMemDynNode *tmpNode;
    uintptr_t callerRa;

    for (tmpNode = endNode->selfNode.preNode; tmpNode != endNode; tmpNode = tmpNode->selfNode.preNode) {
        if (RA_IS_FLAGGED(tmpNode->selfNode.callerRa)) {
            continue;
        }
        if ((tmpNode->selfNode.taskId != taskId) || (!OsMemNodeIsUsed(tmpNode))) {
            continue;
        }
        uint32_t size = OS_MEM_NODE_GET_SIZE(tmpNode->selfNode.sizeAndFlag);
        callerRa = tmpNode->selfNode.callerRa;

        bool ret = updateRaStats(raStats, &raStatsCnt, callerRa, size);
        if (ret) {
            *totalCostSize += OS_MEM_NODE_HEAD_SIZE;
            *totalPoolUsedSize += size;
            tmpNode->selfNode.callerRa = RA_SET_FLAG(tmpNode->selfNode.callerRa);
        }
    }
    return raStatsCnt;
}

/* traverse node and record RaStats each batch */
static uint32_t traverseRaStats(uint8_t taskId, LosMemDynNode *endNode, uint32_t *totalCostSize)
{
    uint32_t totalPoolUsedSize = 0;
    uint32_t raStatsCnt = 0;
    CallerRaMemStat *raStats = GET_IRQSTACK_RASTATS();
    bool allNodeFlagged = FALSE;

    while (!allNodeFlagged) {
        raStatsCnt = collectCallerRaStatsBatch(endNode, taskId, raStats, totalCostSize, &totalPoolUsedSize);
        if (raStatsCnt > 0) {
            printBatchRaStats(raStats, raStatsCnt);
        }
        allNodeFlagged = checkUnflaggedNodes(endNode, taskId);
    }
    return totalPoolUsedSize;
}

static uint32_t OsHeapTaskHeapInfoGet(VOID *pool, uint8_t taskId)
{
    uint32_t totalPoolUsedSize = 0;
    uint32_t totalSlabUsedSize = 0;
    uint32_t totalCostSize = 0;
    LosTaskCB *taskCB = OS_TCB_FROM_TID(taskId);
    LosMemPoolInfo *poolInfo = (LosMemPoolInfo *)pool;
    LosMemDynNode *endNode = OS_MEM_END_NODE(pool, poolInfo->poolSize);
    LosMemDynNode *tmpNode;

    if (poolInfo == NULL || taskId > TASK_NUM - 1) {
        return OSAL_FAILURE;
    }
    if (taskCB->taskName == NULL) {
        return OSAL_FAILURE;
    }
    if (taskId == TASK_NUM - 1) {
        print_info_to_file("Other malloc details:\r\n");
    } else {
        print_info_to_file("Task:%s(taskId=%02d) malloc details:\r\n", taskCB->taskName, taskId);
    }
    // get total slab used mem
    totalSlabUsedSize = OsSlabMemInfoGet(pool, taskId);
    totalPoolUsedSize = traverseRaStats(taskId, endNode, &totalCostSize);

    // restore origin callerRa
    for (tmpNode = endNode->selfNode.preNode; tmpNode != endNode; tmpNode = tmpNode->selfNode.preNode) {
        tmpNode->selfNode.callerRa = RA_CLR_FLAG(tmpNode->selfNode.callerRa);
    }
    print_info_to_file("Current total pool alloc size=%d Bytes\r\n", totalPoolUsedSize);
    print_info_to_file("Current total slab alloc size=%d Bytes\r\n", totalSlabUsedSize);
    print_info_to_file("Current total node cost size=%d Bytes\r\n\n", totalCostSize);
    return OSAL_SUCCESS;
}

static void print_os_all_sys_task_heap(void *pool)
{
    print_info_to_file("========== [all task heap] pool start: 0x%x ==========\n", (uint32_t)(uintptr_t)pool);
    for (uint8_t idx = 0; idx < g_taskMaxNum + 1; idx++) {
        print_info_to_file("========== [task_id: %d heap] pool start: 0x%x ==========\n",
            idx, (uint32_t)(uintptr_t)pool);
        uint32_t intSave;
        MEM_LOCK(intSave, pool);
        OsHeapTaskHeapInfoGet(pool, idx);
        MEM_UNLOCK(intSave, pool);
    }
}

static char *task_status_to_string(uint32_t status)
{
    char *str = "none";

    switch (status & 0xff) {
        case OS_TASK_STATUS_UNUSED:
            str =  "unused";
            break;
        case OS_TASK_STATUS_SUSPEND:
            str =  "suspend";
            break;
        case OS_TASK_STATUS_READY:
            str =  "ready";
            break;
        case OS_TASK_STATUS_PEND:
            str =  "pend";
            break;
        case OS_TASK_STATUS_RUNNING:
            str =  "runing";
            break;
        case OS_TASK_STATUS_DELAY:
            str =  "delay";
            break;
        case OS_TASK_STATUS_TIMEOUT:
            str =  "timeout";
            break;
        case OS_TASK_STATUS_PEND_TIME:
            str =  "pendtime";
            break;
        default:
            break;
    }

    return str;
}

static void print_os_sys_time(void)
{
    struct timeval cur_time = { 0 };
    struct tm lm = { 0 };

    gettimeofday(&cur_time, NULL);
    localtime_r(&cur_time.tv_sec, &lm);

    print_info_to_file("Crash Time: %d/%d/%d %d:%d:%d",
        (lm.tm_year + 1900), (lm.tm_mon + 1), lm.tm_mday, lm.tm_hour, lm.tm_min, lm.tm_sec);  /* tm_year: since 1900 */
}

static void print_os_all_sys_task_info(void)
{
    osal_tsk_info_s task_info;

    print_info_to_file("========== content of task info ==========\n");
    print_info_to_file("%4s|%20s|%4s|%10s|%10s|%s|%10s|%10s|%10s|%4s|%15s|\n", "id", "task_name", "pri",
        "stackSize", "topOfStack", "ButtomOfStack", "sp", "cur_used", "peak_used", "ovf", "status");
    for (uint32_t idx = 0; idx < g_taskMaxNum; idx++) {
        (void)memset_s(&task_info, sizeof(osal_tsk_info_s), 0, sizeof(osal_tsk_info_s));
        uint32_t ret = osal_taskinfo_get(idx, &task_info);
        if (ret != OSAL_SUCCESS) {
            continue;
        }

        print_info_to_file("%4d|%20s|%4d|0x%08x|0x%08x|0x%08x   |0x%08x|0x%08x|0x%08x|%4d|%15s|\n", task_info.uwTaskID,
            task_info.acName, task_info.usTaskPrio, task_info.uwStackSize, task_info.uwTopOfStack,
            task_info.uwBottomOfStack, task_info.uwSP, task_info.uwCurrUsed, task_info.uwPeakUsed,
            task_info.bOvf, task_status_to_string(task_info.usTaskStatus));
    }
}

void crash_data_os_info_display(void)
{
    int ret;

    if (access(g_dump_os_info_path, F_OK) != 0) {
        return;
    }

#if CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS == DFX_YES
    int file_index = crash_data_get_file_index();
    ret = snprintf_s(g_dump_os_buf, sizeof(g_dump_os_buf), sizeof(g_dump_os_buf) - 1, "%s/%s_%d/%s",
        g_dump_os_info_path, CRASH_DATA_EXE_FILE_PREFIX, file_index, g_dump_os_info_name);
    if (ret < 0) {
        return;
    }
#else
    ret = snprintf_s(g_dump_os_buf, sizeof(g_dump_os_buf), sizeof(g_dump_os_buf) - 1, "%s/%s",
        g_dump_os_info_path, g_dump_os_info_name);
    if (ret < 0) {
        return;
    }
#endif

    g_dump_os_info_file = open(g_dump_os_buf, O_CREAT | O_WRONLY | O_TRUNC, S_IREAD | S_IWRITE);
    if (g_dump_os_info_file < 0) {
        PRINT("%s open failed\r\n", g_dump_os_buf);
        return;
    }

    print_os_sys_time();
    print_os_all_sys_task_info();
    print_os_all_sys_task_heap(OS_EXTEND_MEM_ADDR);
    print_os_all_sys_task_heap(OS_SYS_MEM_ADDR);

    ret = close(g_dump_os_info_file);
    if (ret < 0) {
        PRINT("%s %d close file failed\r\n", __FUNCTION__, __LINE__);
        return;
    }
    g_dump_os_info_file = -1;
}
#endif

#if defined(SUPPORT_CRASH_DATA_RAM) && defined(SAVE_EXC_INFO) && defined(CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM) && \
    (CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM == DFX_YES)
static bool item_should_skip(const char* path)
{
    if (strcasestr(path, "SHARED_MEM") != NULL ||
        strcasestr(path, "APP_PSRAM_HEAP") != NULL ||
        strcasestr(path, "ORIGIN") != NULL) {
        return true;
    }
    return false;
}

static void exc_info_write_norflash(cores_t core)
{
    uint32_t data_num;
    uint32_t current_offset = 0;

    crash_data_mem_t *crash_data_info = crash_data_get_info(core, &data_num);

    int32_t ret = (int32_t)dfx_flash_erase(FLASH_OP_TYPE_DUMP_INFO, 0, DFX_FLASH_REGION_LENGTH);
    if (ret != ERRCODE_SUCC) {
        PRINT("erase norflash failed!\r\n");
        return;
    }

    for (uint32_t i = 0; i < data_num; i++) {
        if (item_should_skip(crash_data_info[i].path) == true ||
            crash_data_info[i].size > DFX_FLASH_REGION_LENGTH - current_offset) {
            PRINT("Skip item: %s \n", crash_data_info[i].path);
            continue;
        }
        ret = dfx_flash_write(FLASH_OP_TYPE_DUMP_INFO, current_offset,
            (uint8_t *)(uintptr_t)crash_data_info[i].addr, crash_data_info[i].size, 0);
        if (ret != (int32_t)crash_data_info[i].size) {
            PRINT("%s %d wirte %x failed, ret:%x expect:%x\r\n",
                __FUNCTION__, __LINE__, crash_data_info[i].addr, ret, crash_data_info[i].size);
            continue;
        }
        current_offset += crash_data_info[i].size;
    }
}
#endif
#if defined(USE_CMSIS_OS) && defined(__LITEOS__)
static void OsExcInfoSave(UINT32 excType, const ExcContext *excBufAddr)
{
    if (g_excInfo.nestCnt > 1) { /* Exception nesting level is 1 */
        g_excInfo.phase = OS_EXC_STAGE_EXC;
        g_excInfo.thrdPid = OS_EXC_STAGE_INIT_VALUE;
    } else if (!OS_SCHEDULER_ACTIVE) {
        g_excInfo.phase = OS_EXC_STAGE_INIT;
        g_excInfo.thrdPid = OS_EXC_STAGE_INIT_VALUE;
    } else if (OS_INT_ACTIVE) {
        g_excInfo.phase = OS_EXC_STAGE_IRQ;
        g_excInfo.thrdPid = OS_EXC_STAGE_INIT_VALUE;
    } else {
        g_excInfo.phase = OS_EXC_STAGE_TASK;
        g_excInfo.thrdPid = (UINT32)osal_get_current_pid();
    }
    g_excInfo.faultAddr = excBufAddr->mtval;
    g_excInfo.type = (UINT16)excType;
    g_excInfo.context = (ExcContext *)excBufAddr;
}
#endif
static void exc_fault_info_display(uint32_t exc_type, uint32_t phase, exception_info_t *exce_info,
    const exc_context_t *exc_buf_addr)
{
#if defined(USE_CMSIS_OS) && defined(__LITEOS__)
    OsExcInfoSave(exc_type, (const ExcContext *)(uintptr_t)exc_buf_addr);
#endif
#if (CORE == BT)
    if (exc_buf_addr != NULL) {
        g_exc_info.type = (uint16_t)(exc_buf_addr->mcause);
        g_exc_info.context = (exc_context_t *)exc_buf_addr;
        exc_info_display(&g_exc_info);
    }
#endif
    duplicate_app_preserve_mem();
#ifdef SUPPORT_CRASH_DATA_RAM
    exc_info_save_t *exc_info = crash_data_get_exc_info_save();
    crash_data_clear();
    crash_data_set_time_s();
#ifdef USE_CMSIS_OS
    /* 将死机信息写入share memory中的crash data中 */
    exc_info_write(exc_type, phase, exce_info, (const exc_context_t *)exc_buf_addr, exc_info);

    /* 将crash data中的死机信息从串口输出 */
    print_system_exec_info(exc_info);
#endif
#endif

#if defined(SUPPORT_CRASH_DATA_RAM) && defined (SAVE_EXC_INFO) && defined(CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM) && \
    (CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM == DFX_YES)
    exc_info_write_norflash(CORE);
#endif

    unused(exc_type);
    unused(phase);
    unused(exce_info);
    unused(exc_buf_addr);
}

#ifdef LOSCFG_EXC_SIMPLE_INFO
void exc_cause_info_display(uint32_t cause, uint32_t exc_type)
{
    if ((exc_type < sizeof(g_mcause_info) / sizeof(g_mcause_info[0])) &&
        (cause < sizeof(g_ccause_info) / sizeof(g_ccause_info[0]))) {
        PRINT("%s\n""%s\n", g_mcause_info[exc_type], g_ccause_info[cause]);
    } else {
        PRINT("Unknown exc:%u, ccause:%u.\n", exc_type, cause);
    }
}
#endif

void do_hard_fault_handler(exc_context_t *exc_buf_addr)
{
    if (exc_buf_addr == NULL) { return; }
#if defined (SUPPORT_CPU_TRACE) || defined (SUPPORT_CPU_TRACE_V2)
    uapi_cpu_trace_disable(cpu_trace_get_self_id());
#endif
#if defined(__LITEOS__)
    osal_kthread_lock();
#endif
#if (CORE == MASTER_BY_ALL) && (CORE != WIFI)
    if (g_is_repeat_exc == true) {
        cpu_utils_reset_chip_with_log((cores_t)MASTER_BY_ALL, REBOOT_CAUSE_APPLICATION_SECOND_FAULT);
    }
    /* If the process is restarted at the end of an abnormal process, the value of this variable is false. */
    /* Do not need to manually set false at the end. */
    g_is_repeat_exc = true;
#endif
    uint32_t exc_type = exc_buf_addr->mcause;
    exc_type &= MCAUSE_MASK;
#ifdef LOSCFG_EXC_SIMPLE_INFO
    exc_cause_info_display(exc_buf_addr->ccause, exc_type);
#endif
    PRINT("exception:%x\r\n", exc_type);
    if (exc_type == NMI_INTERRUPT) {
        PRINT("Oops:NMI\n");
        PRINT("nmi raw interrupt is 0x%x"NEWLINE, non_os_get_nmi_raw_status());
    }
    reboot_cause_t reset_cause = hal_set_reset_cause(exc_type);
    hal_save_reg_info(exc_buf_addr);
    deal_with_exception(exc_buf_addr, exc_type, reset_cause);
}

static fs_terminate_handler g_fs_term_handler = NULL;
void regist_file_system_terminate_handler(fs_terminate_handler handler)
{
    if (handler != NULL) {
        g_fs_term_handler = handler;
    }
}
#if defined(BUILD_APPLICATION_STANDARD) && (CORE == APPS)

static uint8_t g_file_mount_rets = 1;

void set_file_system_mount_ret(int32_t rets)
{
    g_file_mount_rets = rets;
}

void reset_deal_with_fs(void)
{
    if (g_file_mount_rets != 0) {
        return;
    }
    /* 重启前关闭所有文件句柄，防止文件异常 */
    for (uint32_t fd = 0; fd < FS_MAX_FILE_HANDLES; fd++) {
        close(fd);
    }
    if (g_fs_term_handler != NULL) {
        g_fs_term_handler();
    } else {
#ifdef CFG_DRIVERS_NANDFLASH
        /* 重启前进行yaffs sync操作，防止文件系统异常 */
        nandflash_yaffs_sync();
#endif
    }
}
#endif

static void dead_cycle_wait(void)
{
#if defined(SUPPORT_DONOT_RESET_AFTER_EXCEPTION)
    volatile uint8_t i = 1;
    while (i != 0) {
    }
#endif /* SUPPORT_DONOT_RESET_AFTER_EXCEPTION */
}

static void deal_with_exception(exc_context_t *exc_buf_addr, uint32_t exc_type, reboot_cause_t reset_cause)
{
    unused(reset_cause);

    set_crash_core_type(CORE);
#if defined(BUILD_APPLICATION_STANDARD) && (CORE == APPS)
    // Record the reason before wait, so apps can get the reason.
    set_exception_time_stamp();
    set_cpu_utils_reset_cause(reset_cause);
#endif

    exception_info_t exception_info = {0};
    uint32_t phase = 0;
#if defined(__LITEOS__)
    get_exception_task_info(&exception_info, &phase);
    set_exception_info(&exception_info);
#endif
    exc_fault_info_display(exc_type, phase, &exception_info, exc_buf_addr);

#if (CORE == APPS) && defined(SUPPORT_CRASH_DATA_RAM) && defined(SAVE_EXC_INFO)
    /* 将share memory中的crash data存入flash */
    crash_data_save(CORE);
#endif

#if defined(SAVE_EXC_INFO) && defined(CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM) && \
    (CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM == DFX_YES)
    crash_data_os_info_display();
#endif
    if (g_exception_dump_callback != NULL) {
        g_exception_dump_callback(exc_type, exc_buf_addr);
    }
#if defined(BUILD_APPLICATION_STANDARD) && (CORE == APPS)
    reset_deal_with_fs();
#endif
    dead_cycle_wait();
    disable_watchdog();
#if (CORE == APPS)
    cpu_utils_reset_chip_with_log((cores_t)APPS, reset_cause);
#elif (CORE == BT)
    exception_notify(reset_cause);
#endif
}

#if defined(__LITEOS__)
void do_fault_handler(uint32_t exc_type, exc_context_t *exc_buff_addr)
{
    unused(exc_type);
    do_hard_fault_handler(exc_buff_addr);
}
#endif

void hal_register_exception_dump_callback(hal_exception_dump_callback callback)
{
    if (callback != NULL) {
        g_exception_dump_callback = callback;
    }
}

void hal_exception_mcu_callback(int32_t src)
{
    unused(src);
    hal_reboot_chip();
}

#if defined(SAVE_EXC_INFO)
#if defined(CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM) && (CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM == DFX_YES)
static void c_wdt_lock_pclr_rec(void)
{
#define C_WDT_REC_LENGTH 64
    char wdt_info[C_WDT_REC_LENGTH] = {0};
    if (sprintf_s(wdt_info, C_WDT_REC_LENGTH, "cwdt:pc:0x%04x%04x,lr:0x%04x%04x,sp:0x%04x%04x.\n",
        readw(GLB_CTL_M_RB_MCPU_PC_H_REG), readw(GLB_CTL_M_RB_MCPU_PC_L_REG), readw(GLB_CTL_M_RB_MCPU_LR_H_REG),
        readw(GLB_CTL_M_RB_MCPU_LR_L_REG), readw(GLB_CTL_M_RB_MCPU_SP_H_REG), readw(GLB_CTL_M_RB_MCPU_SP_L_REG)) < 0) {
        return;
    }
    if (strlen(wdt_info) < C_WDT_REC_LENGTH) {
        if (sprintf_s(wdt_info + strlen(wdt_info), C_WDT_REC_LENGTH - strlen(wdt_info), "pm:0x%04x.\n",
            readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_17_REG)) < 0) {
            PRINT("pmstep rec fail.\n");
        }
    }
    int32_t ret = dfx_file_write("/user/exc/hard_watchdog_lock.bin", 0, (const uint8_t *)wdt_info, C_WDT_REC_LENGTH);
    if (ret < 0) {
        PRINT("hard wdt write pclrsp fail.\n");
    }
}
#endif

void hard_watchdog_reset_save_info(void)
{
#if defined(CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM) && (CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM == DFX_YES)
    reboot_cause_t cause = get_cpu_utils_reset_cause();
    if (cause == REBOOT_CAUSE_APPLICATION_STD_CHIP_WDT_FRST) {
        PRINT("nmi reboot cause: 0x%x"NEWLINE, cause);

        cpu_trace_port_enable();
        reg16_setbit(MCU_DIAG_CTL_RB_CFG_MCU_DIAG_MONITOR_CLOCK_REG,
            MCU_DIAG_CTL_RB_CFG_MCU_DIAG_TRACE_MEM_CLKEN_OFFSET);
        int32_t ret = dfx_file_write("/user/exc/hard_watchdog_error.bin",
            0, (const uint8_t *)MCPU_TRACE_MEM_REGION_START, CPU_TRACE_MEM_REGION_LENGTH);
        if (ret < 0) {
            PRINT("hard wdt rst write fileinfo fail.\n");
        }
        c_wdt_lock_pclr_rec();

        ret = dfx_file_write("/user/exc/hard_watchdog_comram.bin",
            0, (const uint8_t *)g_comram_copy_path, SHARED_MEM_SAVE_LENGTH);
        if (ret < 0) {
            PRINT("hard wdt rst comram write fileinfo fail.\n");
        }
        osal_kfree(g_comram_copy_path);
    }
#endif
}

void hard_watchdog_reset_copy_comram(void)
{
#if defined(CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM) && (CONFIG_DFX_LOG_FILE_SAVE_ON_FILESYSTEM == DFX_YES)
    uint16_t wdg_timeout_flag = readw(HAL_PMU_PROTECT_STATUS_REG);
    if ((wdg_timeout_flag & BIT(HAL_PMU_PROTECT_CHIP_WDG_BIT)) != 0) {
        g_comram_copy_path = (char *)osal_kmalloc((uint32_t)SHARED_MEM_SAVE_LENGTH, OSAL_GFP_KERNEL);
        if (g_comram_copy_path == NULL) {
            return;
        }

        if (memcpy_s((uint8_t *)(uintptr_t)g_comram_copy_path, SHARED_MEM_SAVE_LENGTH,
                     (uint8_t *)(uintptr_t)SHARED_MEM_START, SHARED_MEM_SAVE_LENGTH) != EOK) {
            PRINT("share memcopy_s error");
        }
    }
#endif
}
#endif /* SAVE_EXC_INFO */