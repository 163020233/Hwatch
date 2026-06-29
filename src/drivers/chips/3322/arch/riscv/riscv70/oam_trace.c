/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description:  OAM TRACE for RISCV70
 *
 * Create: 2022-03-22
 */
#include "oam_trace.h"
#ifdef __LITEOS__
#include "print_config.h"
#endif
#include "arch_encoding.h"
#include "arch_trace.h"

#define  USER_STACK_OFFSET_PER   4
#define  USER_STACK_PRINT_DEPTH  30

#ifdef __LITEOS__
EXC_PROC_FUNC g_exec_hook;
#endif

#if defined(USE_CMSIS_OS) && defined(__LITEOS__)
static void back_trace(uint32_t sp)
{
    uint32_t back_sp = sp;
    uint32_t count = 0;

    oam_info_log("*******backtrace begin*******\n");

    while (back_sp != 0) {
        oam_info_log("traceback %d -- sp addr= 0x%x sp content= 0x%x\n",
            count, back_sp,  *((uint32_t *)(back_sp)));
        back_sp = back_sp + USER_STACK_OFFSET_PER;
        count++;
        if (count == USER_STACK_PRINT_DEPTH) {
            break;
        }
    }

    oam_info_log("*******backtrace end*******\n");
}

static void riscv_cpu_trace_print(void)
{
    riscv_cpu_trace_item_t *item = (riscv_cpu_trace_item_t *)(uintptr_t)(RISCV_TRACE_MEM_REGION_START);
    uint32_t cpu_trace_end_addr = RISCV_TRACE_MEM_REGION_START + RISCV_TRACE_MEM_REGION_LENGTH;

    oam_info_log("**************CPU Trace Information************** \n");

    for (uint32_t i = 0; &item[i] < (riscv_cpu_trace_item_t *)(uintptr_t)(cpu_trace_end_addr); i++) {
        oam_info_log("cpu trace %d:\r\n", i);
        oam_info_log("step time:0x%x   LR:0x%x   PC:0x%x\r\n", item[i].time, item[i].lr, item[i].pc);
    }

    oam_info_log("**************CPU Trace end*********************** \n");
}

static void print_excptsc(void)
{
    oam_info_log("cxcptsc = 0x%x\n", read_custom_csr(CXCPTSC));
}

void exec_fault_handler(uint32_t exc_type, const ExcContext *exc_buff_addr)
{
    riscv_cpu_trace_disable();
    if (g_exec_hook != NULL) {
        g_exec_hook(exc_type, exc_buff_addr);
    }
    // custom oam information
    print_excptsc();
    if (exc_buff_addr != NULL) {
        back_trace(exc_buff_addr->taskContext.sp);
    }
    riscv_cpu_trace_print();
}
#endif

void register_os_exec_hook(void)
{
#ifdef __LITEOS__
    g_exec_hook = ArchGetExcHook();
    ArchSetExcHook((EXC_PROC_FUNC)exec_fault_handler);
#endif
}

