/******************************************************************************
 * @brief    CRT0 file used for starting up the rest of the code
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd.
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
        /* norvc 伪操作表示接下来的汇编程序不可以被汇编生成16位宽的压缩指令 */
        .option norvc
Reset_Handler:
        j _startboot

_startboot:

        la t0, trap_excp
        /* linx170 only support CLIC mode, mtvec[1:0]= 0x3, and 64B aligned */
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

        la t0, __boot_uart_begin__
        mv t1, a0
        mv t2, a1

set_arg_loop:
        lw     t3, (t1)
        sw     t3, (t0)
        addi   t0, t0, 4
        addi   t1, t1, 4
        addi   t2, t2, -4
        blt    x0, t2, set_arg_loop

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

        /* set to initial state of FPU */
        li      t0, 0x00006000
        csrs    mstatus, t0

        j start_loaderboot
