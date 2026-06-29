/******************************************************************************
 * @brief    CRT0 file used for starting up the rest of the code
 * Copyright (c) 2025 HiSilicon (Shanghai) Technologies Co., Ltd.
******************************************************************************/
/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied sec_main() routine is called.
 * @param  None
 * @retval : None
*/
        .extern TrapVector
        .extern g_vectorIntTable
        .section .text.entry
        .global reset_vector
        .option norvc
reset_vector:
        j _start

_start:
        la t0, TrapVector
        csrw   mtvec, t0
        la t0, g_vectorIntTable
        csrw   mtvt, t0

#ifndef CONFIG_SUPPORT_SELITEOS_STARTUP
        la t0, tee_nmi_handler
        addi t0, t0, 3
        csrw tmtvec, t0

        /* config pmp tes */
        li t0, 0x001bffff
        csrrw x0, pmpte0, t0
#endif

_pmp_non_tee_cfg:
        li t0, 0x9b9b0000
        csrrw x0, pmpcfg5, t0

        la      t0, __irq_stack_top__
        csrw    mscratch, t0
        csrwi   mstatus, 0
        csrwi   mie, 0

        .option push
        .option norelax
        /* initialize global pointer */
        la t0, _gp_
        csrw tugp, t0

        # initialize stack pointer
        la t0, __stack_top__
        csrw tusp, t0

        /* init stack */
        la      t0, g_system_stack_begin
        la      t1, g_system_stack_end
        beq     t0, t1, end_set_stack_loop
        li      t2, 0xefbeadde

set_stack_loop:
        sw      t2, (t0)
        addi    t0, t0, 4
        blt     t0, t1, set_stack_loop
end_set_stack_loop:

        /* set data section */
        la      t0, __data_begin__
        la      t1, __data_load__
        la      t2, __data_size__
        beq     t2, x0, end_set_data_loop

set_data_loop:
        lw      t3, (t1)
        sw      t3, (t0)
        addi    t0, t0, 4
        addi    t1, t1, 4
        addi    t2, t2, -4
        blt     x0, t2, set_data_loop
end_set_data_loop:

        /* setup .ramtext section */
        la      t0, __ramtext_begin__
        la      t1, __ramtext_load__
        la      t2, __ramtext_size__
        beq     t2, x0, end_set_ramtext_loop

set_ramtext_loop:
        lw      t3, (t1)
        sw      t3, (t0)
        addi    t0, t0, 4
        addi    t1, t1, 4
        addi    t2, t2, -4
        blt     x0, t2, set_ramtext_loop
end_set_ramtext_loop:

        /* clear bss section */
        la      t0, __bss_begin__
        la      t1, __bss_end__
        beq     t0, t1, end_clear_bss_loop
        li      t2, 0x00000000

clear_bss_loop:
        sw      t2, (t0)
        addi    t0, t0, 4
        blt     t0, t1, clear_bss_loop
end_clear_bss_loop:
        .option pop

        /* set to initial state of FPU */
        li      t0, 0x00006000
        csrs    mstatus, t0

_pmp_cfg_end:
        /* config PMP_INDEX_ITCM_BACKGROUND_ADDR = 22 as rx, config PMP_INDEX_BOOTROM_ADDR = 20 as inaccessible */
        li t0, 0x9b9d0098
        csrrw x0, pmpcfg5, t0

        li t0, 0x0                       /* clear pmp21 PMP_INDEX_ITCM_TEE_ADDR used in bootrom and flashboot */
        csrrw x0, pmpaddr21, t0

#ifndef CONFIG_SUPPORT_SELITEOS_STARTUP
        /* config PMP_INDEX_SFC_CODE_ADDR = 18 as rx 0x9d and cache */
        li t0, 0x9BFFFFF
        csrrw x0, pmpaddr18, t0         /* addr:0x2600_0000 - 0x2800_0000 */

        li t0, 0x1FFFF
        csrrw x0, memattr18, t0

        li t0, 0x009d9b9d
        csrrw x0, pmpcfg4, t0
#endif

        /* set resetpc */
        la      t0, 0x52018060
        la      t1, lowpower_cpu_resume
        srl     t2, t1, 16         /* t0 >> 16 */

        la      t3, 0              /* lock en = 0 */
        sw      t3, (t0)

        sh      t2, 4(t0)          /* set reset pc */
        sh      t1, 8(t0)

        la      t3, 1              /* lock en = 1 */
        sw      t3, (t0)

        la      t0, 0x52018084     /* tee deepsleep veto = 1 */
        sw      t3, (t0)

        /* tmedeleg 0x7E3 表示在非安全状态下将异常委托给非安全的接口处理 */
        la t0, 0xffffffff
        csrrw x0, tmedeleg, t0

        /* clicinttidlg readcsr(0xFBF)+0a00 表示在非安全状态下将中断委托给非安全的接口处理 */
        la t1, 0xb0000a00
        sw t0, (t1)
        la t1, 0xb0000a04
        sw t0, (t1)
        la t1, 0xb0000a08
        sw t0, (t1)
        la t1, 0xb0000a0c
        sw t0, (t1)

        /* tmstatus.mie = 1 */
        li t0, 0x200008
        csrrw x0, tmstatus, t0

        /* config pmpaddr and rwx permission, only support NAPOT mode */
        /* pmpaddr = (start_addr + length >> 1 - 1) >> 2, Region start addr 2^n Byte align, length is 2^n Byte */

        li t0, 0x9ffffff                        /* addr:0x2000_0000 - 0x3000_0000   l2ram/sfc/psram ree rw */
        csrrw x0, pmpaddr27, t0

        li t0, 0x15ffffff                       /* addr:0x5000_0000 - 0x6000_0000   reg             ree rw */
        csrrw x0, pmpaddr28, t0

        li t0, 0x21ffffff                       /* addr:0x8000_0000 - 0x9000_0000   bram com_ram stcm gnss_tcmm ree rw */
        csrrw x0, pmpaddr29, t0

        li t0, 0x2dffffff                       /* addr:0xb000_0000 - 0xc000_0000   cpu system reg  ree rw */
        csrrw x0, pmpaddr30, t0

        li t0, 0x0
        csrrw x0, pmpaddr24, t0

        li t0, 0x0
        csrrw x0, pmpaddr25, t0

        li t0, 0x0
        csrrw x0, pmpaddr26, t0

        li t0, 0x9b000000
        csrrw x0, pmpcfg6, t0

        li t0, 0x9d9b9b9b
        csrrw x0, pmpcfg7, t0

        /* icache invalid */
        fence iorw, iorw
        li  t0, 4
        csrw icmaint, t0
        fence iorw, iorw

        /* dcache invalid */
        li  t0, 4
        csrw dcmaint, t0
        fence iorw, iorw

        /* set to initial state of FPU */
        li      t0, 0x00006000
        csrs    mstatus, t0
        fssr    x0

        la  t1, 0x57000014
        la  t2, 0x5a05
        sw  t2, (t1)

        /* From secure to non-secure */
        la t0, sec_main
        jalr x0, t0, 0

.global g_nmi_stack
.align 6
tee_nmi_handler:
    csrw    tmscratch, t0

    la t0, g_nmi_stack
    /* store s0~s11. */
    sw s0, 0 * 4(t0)
    sw s1, 1 * 4(t0)
    sw s2, 2 * 4(t0)
    sw s3, 3 * 4(t0)
    sw s4, 4 * 4(t0)
    sw s5, 5 * 4(t0)
    sw s6, 6 * 4(t0)
    sw s7, 7 * 4(t0)
    sw s8, 8 * 4(t0)
    sw s9, 9 * 4(t0)
    sw s10, 10 * 4(t0)
    sw s11, 11 * 4(t0)

    /* store t0~t6. */
    sw t1, 13 * 4(t0)
    sw t2, 14 * 4(t0)
    sw t3, 15 * 4(t0)
    sw t4, 16 * 4(t0)
    sw t5, 17 * 4(t0)
    sw t6, 18 * 4(t0)

    csrr t1, tmscratch
    sw  t1, 12 * 4(t0)

    /* set mepc to tmepc. */
    csrr    t0, tmepc
    csrw    mepc, t0

    /* set mcause.code = tmcause.code. */
    li      t0, 0xFFF
    csrc    mcause, t0
    csrr    t1, tmcause
    and     t0, t0, t1
    csrs    mcause, t0

    /* set mcause.int = 0. */
    li      t0, 0x80000000
    csrc    mcause, t0

    /* Jump to the untrusted handler. */
    la      t0, ree_nmi_handler
    jr      t0

.section .text
ree_nmi_handler:
    la t0, g_nmi_stack
    /* restore s0~s11. */

    /* store s0~s11. */
    lw s0, 0 * 4(t0)
    lw s1, 1 * 4(t0)
    lw s2, 2 * 4(t0)
    lw s3, 3 * 4(t0)
    lw s4, 4 * 4(t0)
    lw s5, 5 * 4(t0)
    lw s6, 6 * 4(t0)
    lw s7, 7 * 4(t0)
    lw s8, 8 * 4(t0)
    lw s9, 9 * 4(t0)
    lw s10, 10 * 4(t0)
    lw s11, 11 * 4(t0)

    /* store t0~t6. */
    lw t1, 13 * 4(t0)
    lw t2, 14 * 4(t0)
    lw t3, 15 * 4(t0)
    lw t4, 16 * 4(t0)
    lw t5, 17 * 4(t0)
    lw t6, 18 * 4(t0)

    lw t0, 12 * 4(t0)

    /* restore t0~t7. */
    j       TrapVector