/******************************************************************************
 * @brief    CRT0 file used for starting up the rest of the code
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
******************************************************************************/

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/
        .section .text.entry
        .global Reset_Handler
        .option norvc
Reset_Handler:
        j _dfu_hook

_dfu_hook:
        /* read WDT_NO_RST_REG AS 0x5a5a */
        la t0, 0x57000154 /* GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_15_REG */
        la t1, 0x5a5a
        lw t2, (t0)
        bne t1, t2, _start

        /* clear WDT_NO_RST_REG */
        la      t1, 0x5a5b
        sw      t1, (t0)

        /* copy text to 0x2F000 */
        la      t0, __jump_text_begin__
        la      t1, __jump_text_load__
        la      t2, __jump_text_size__
        beq     t2, x0, end_set_jump_text_loop

set_jump_text_loop:
        lw      t3, (t1)
        sw      t3, (t0)
        addi    t0, t0, 4
        addi    t1, t1, 4
        addi    t2, t2, -4
        blt     x0, t2, set_jump_text_loop
end_set_jump_text_loop:

        /* config pmp16 as pmp rx 0x9d and jump */
        li t0, 0x15FFF                           /* addr:0x5_0000 - 0x6_0000   .text.jump_to_rom_usb_dfu  rx */
        csrrw x0, pmpaddr16, t0

        li t0, 0x9d
        csrrw x0, pmpcfg4, t0

        j jump_to_rom_usb_dfu

_start:
        la t0, trap_excp
        addi t0, t0, 3
        csrw tmtvec, t0
        la t0, trap_vector
        csrrw x0, tmtvt, t0
        csrwi tmstatus, 0
        csrwi mie, 0

        .option push
        .option norelax
        # initialize global pointer
        la gp, _gp_
        .option pop

        # initialize stack pointer
        la sp, __stack_top__

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

        j main

/**
 * @brief  This is the code that jump to bootrom usb_dfuboot_check handle
 * @param  None
 * @retval : None
*/
        .section .text.jump_to_rom_usb_dfu
        .option norvc
jump_to_rom_usb_dfu:
        /* 0. modify pmpaddr21 rw 0x9b, 0x10000 - 0x20000 */
        li t0, 0x9b9b9b9d
        csrrw x0, pmpcfg5, t0

        la ra, _reset

        /* 1. jump to bootrom usb_dfuboot_check */
        li t0, 0x2288
        jalr x0, t0, 0

_reset:
        la t0, 0x57000204
        la t1, 0
        sw t1, (t0)

_while:
        j _while
