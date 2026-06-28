/******************************************************************************
 * @brief    CRT0 file used for starting up the rest of the code
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd.
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

        la      t0, __irq_stack_top__
        csrw    mscratch, t0
        csrwi   mstatus, 0
        csrwi   mie, 0

        .option push
        .option norelax
        /* initialize global pointer */
        la      gp, _gp_

        # initialize stack pointer
        la      sp, __stack_top__

        /* clear pmp20 PMP_INDEX_BOOTROM_ADDR used in bootrom and flashboot */
        li t0, 0x0
        csrrw x0, pmpaddr20, t0

        /* clear pmp21 PMP_INDEX_ITCM_TEE_ADDR used in bootrom and flashboot */
        li t0, 0x0
        csrrw x0, pmpaddr21, t0

        /* config itcm and dtcm background as rw */
        li t0, 0x9b9b0000
        csrrw x0, pmpcfg5, t0

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

#ifdef USE_PSRAM_INSTEAD_NORFLASH_FOR_CODETEXT
        /* setup .psram_xip_code section */
        la      t0, __psram_code_begin__
        la      t1, __psram_code_load__
        la      t2, __psram_code_size__
        beq     t2, x0, end_set_psramtext_loop

set_psramtext_loop:
        lw      t3, (t1)
        sw      t3, (t0)
        addi    t0, t0, 4
        addi    t1, t1, 4
        addi    t2, t2, -4
        blt     x0, t2, set_psramtext_loop
end_set_psramtext_loop:
#endif
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

#ifdef USE_PSRAM_INSTEAD_NORFLASH_FOR_CODETEXT
        /* use pmp21 to set the read and execute permissions for the PSRAM text section. 0x22800000--0x22FFFFFF */
        li t0, 0x08AFFFFF
        csrrw x0, pmpaddr21, t0
        /* writeback rallocate */
        li t0, 0x00000007
        csrrw x0, memattr21, t0
#endif
        /* config itcm background as rx */
#ifdef USE_PSRAM_INSTEAD_NORFLASH_FOR_CODETEXT
        li t0, 0x9b9d9d00
#else
        li t0, 0x9b9d0000
#endif
        csrrw x0, pmpcfg5, t0

        /* set to initial state of FPU */
        li      t0, 0x00006000
        csrs    mstatus, t0

        j main
