/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Hal riscv
 * Author: @CompanyNameTag
 * Create: 2022-03-22
 */

#include "tcxo.h"
#include "vectors.h"
#include "boot_serial.h"
#include "chip_io.h"
#include "interrupt_handler.h"

#define TMTVEC                  0x7E4
#define TMTVT                   0x7E5
#define TMINTTHRESH             0x7E6
#define TMSTATUS                0x7E7
#define TMEPC                   0x7E8
#define TMCAUSE                 0x7E9
#define TCXCPTSC                0xFC3
#define TMTVAL                  0x7EB
#define TMSCRATCH               0x7EC
#define MCAUSE_NUM_MASK         0xFFF
#define NMI_INTERRUPT           0xFFF
#define SYS_SOFT_WDT_RST    0x57000204
#define WDT_ADDR            0x57020000
#define WDT_LOCK            (WDT_ADDR)
#define WDT_LOAD            (WDT_ADDR + 0x4)
#define WDT_RESTART         (WDT_ADDR + 0x8)
#define WDT_EOI             (WDT_ADDR + 0xC)  /* bit0:W0 clear interrupt */
#define WDT_EOI_OFFSET      0
#define CFG_GLBM_SOFT_RST_SYS_N_IDX 0
#define EXE_INT_MSK 0x80000000

#define read_custom_csr(reg) ({ unsigned long __tmp; \
        asm volatile ("csrr %0, %1" : "=r"(__tmp) : "i"(reg)); \
        __tmp; })

exc_info_t g_tee_exc_info;

void default_handler(void)
{
    uint32_t hwi_index = interrupt_number_get();
    boot_msg1("hwi:", hwi_index);
    boot_msg1("mcause:", read_custom_csr(TMCAUSE));
    boot_msg1("mstatus:", read_custom_csr(TMSTATUS));
    while (1) { }
    UNUSED(hwi_index);
}

static void do_exc_info_display(exc_info_t *exc)
{
    boot_msg1("mepc=", exc->context->task_context.mepc);
    boot_msg1("tmtval=", exc->context->mtval);
    boot_msg1("mstatus=", exc->context->task_context.mstatus);
    boot_msg1("ra=", exc->context->task_context.ra);
    boot_msg1("tmcause=", exc->context->mcause);
    boot_msg2("Exc=", (exc->context->mcause & EXE_INT_MSK) == 0 ? 0x1 : 0x0,
        exc->type);
}

void do_trap_handler(exc_context_t *exc_buff_addr)
{
    g_tee_exc_info.type = (uint16_t)(exc_buff_addr->mcause);
    g_tee_exc_info.context = exc_buff_addr;
    do_exc_info_display(&g_tee_exc_info);
}

void do_hard_fault_handler_for_boot(exc_context_t *exc_buf_addr)
{
    uint32_t exc_type = exc_buf_addr->mcause;
    boot_msg1("mcause:", exc_type);
    if ((exc_type & MCAUSE_NUM_MASK) == NMI_INTERRUPT) {
        boot_msg0("NMI");
        reg32_setbit(WDT_EOI, WDT_EOI_OFFSET); /* clear wdt interrupt */
        uapi_tcxo_delay_us(64); /* 64us */
    }
    do_trap_handler(exc_buf_addr);
    uapi_tcxo_delay_ms(1000); /* 1000ms 延时 1s 保证异常打印完成 */
    reg16_clrbit(SYS_SOFT_WDT_RST, CFG_GLBM_SOFT_RST_SYS_N_IDX);
}
