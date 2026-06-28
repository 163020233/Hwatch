/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description:  BRANDY Application Core Vector Table
 *
 * Create:  2022-03-22
 */
#ifndef USE_CMSIS_OS
#include "vectors.h"

void trap_entry(void);
void nmi_handler(void);
void reserve_handler(void) __attribute__((weak, alias("isr_not_implemented")));

/**
 * Default interrupt handler, used for when no specific handler has been implemented.
 * Needed for weak aliasing (an aliased function must have static linkage).
 */
//lint -esym(528, isr_not_implemented)
static void isr_not_implemented(void)
{
    while (1) { //lint !e716
    }
}

/**
 * The interrupt vector table
 */
//lint -esym(714, vector_table) Not unused
//lint -esym(785, vector_table) Too few initialises. Other slots are unused
isr_function g_ram_exception_table[ISR_VECTOR_MAX_SUPPORTED + 1] = {
    [USER_SOFTWARE_INT_IRQN] = default_handler,
    [SUPERVISOR_SOFTWARE_INT_IRQN] = default_handler,
    [RESERVED_INT2_IRQN] = default_handler,
    [MACHINE_SOFTWARE_INT_IRQN] = default_handler,
    [USER_TIMER_INT_IRQN] = default_handler,
    [SUPERVISOR_TIMER_INT_IRQN] = default_handler,
    [RESERVED_INT6_IRQN] = default_handler,
    [MACHINE_TIMER_INT_IRQN] = default_handler,
    [USER_EXTERNAL_INT_IRQN] = default_handler,
    [SUPERVISOR_EXTERNAL_INT_IRQN] = default_handler,
    [RESERVED_INT10_IRQN] = default_handler,
    [MACHINE_EXTERNAL_INT_IRQN] = default_handler,
    [NON_MASKABLE_INT_IRQN] = default_handler,
    [RESERVED_INT13_IRQN] = default_handler,
    [RESERVED_INT14_IRQN] = default_handler,
    [RESERVED_INT15_IRQN] = default_handler,

    [ISR_VECTOR_IRQ_0] = reserve_handler,
    [ISR_VECTOR_IRQ_1] = reserve_handler,
    [ISR_VECTOR_IRQ_2] = reserve_handler,
    [ISR_VECTOR_IRQ_3] = reserve_handler,
    [ISR_VECTOR_IRQ_4] = reserve_handler,
    [ISR_VECTOR_IRQ_5] = reserve_handler,

    [ISR_VECTOR_IRQ_6] = reserve_handler,
    [ISR_VECTOR_IRQ_7] = reserve_handler,
    [ISR_VECTOR_IRQ_8] = reserve_handler,
    [ISR_VECTOR_IRQ_9] = reserve_handler,
    [ISR_VECTOR_IRQ_10] = reserve_handler,
    [ISR_VECTOR_IRQ_11] = reserve_handler,
    [ISR_VECTOR_IRQ_12] = reserve_handler,
    [ISR_VECTOR_IRQ_13] = reserve_handler,
    [ISR_VECTOR_IRQ_14] = reserve_handler,
    [ISR_VECTOR_IRQ_15] = reserve_handler,
    [ISR_VECTOR_IRQ_16] = reserve_handler,
    [ISR_VECTOR_IRQ_17] = reserve_handler,
    [ISR_VECTOR_IRQ_18] = reserve_handler,
    [ISR_VECTOR_IRQ_19] = reserve_handler,
    [ISR_VECTOR_IRQ_20] = reserve_handler,
    [ISR_VECTOR_IRQ_21] = reserve_handler,
    [ISR_VECTOR_IRQ_22] = reserve_handler,
    [ISR_VECTOR_IRQ_23] = reserve_handler,
    [ISR_VECTOR_IRQ_24] = reserve_handler,
    [ISR_VECTOR_IRQ_25] = reserve_handler,
    [ISR_VECTOR_IRQ_26] = reserve_handler,
    [ISR_VECTOR_IRQ_27] = reserve_handler,
    [ISR_VECTOR_IRQ_28] = reserve_handler,
    [ISR_VECTOR_IRQ_29] = reserve_handler,
    [ISR_VECTOR_IRQ_30] = reserve_handler,
    [ISR_VECTOR_IRQ_31] = reserve_handler,
    [ISR_VECTOR_IRQ_32] = reserve_handler,
    [ISR_VECTOR_IRQ_33] = reserve_handler,
    [ISR_VECTOR_IRQ_34] = reserve_handler,
    [ISR_VECTOR_IRQ_35] = reserve_handler,
    [ISR_VECTOR_IRQ_36] = reserve_handler,
    [ISR_VECTOR_IRQ_37] = reserve_handler,
    [ISR_VECTOR_IRQ_38] = reserve_handler,
    [ISR_VECTOR_IRQ_39] = reserve_handler,
    [ISR_VECTOR_IRQ_40] = reserve_handler,
    [ISR_VECTOR_IRQ_41] = reserve_handler,
    [ISR_VECTOR_IRQ_42] = reserve_handler,
    [ISR_VECTOR_IRQ_43] = reserve_handler,
    [ISR_VECTOR_IRQ_44] = reserve_handler,
    [ISR_VECTOR_IRQ_45] = reserve_handler,
    [ISR_VECTOR_IRQ_46] = reserve_handler,
    [ISR_VECTOR_IRQ_47] = reserve_handler,
    [ISR_VECTOR_IRQ_48] = reserve_handler,
    [ISR_VECTOR_IRQ_49] = reserve_handler,
    [ISR_VECTOR_IRQ_50] = reserve_handler,
    [ISR_VECTOR_IRQ_51] = reserve_handler,
    [ISR_VECTOR_IRQ_52] = reserve_handler,
    [ISR_VECTOR_IRQ_53] = reserve_handler,
    [ISR_VECTOR_IRQ_54] = reserve_handler,
    [ISR_VECTOR_IRQ_55] = reserve_handler,
    [ISR_VECTOR_IRQ_56] = reserve_handler,
    [ISR_VECTOR_IRQ_57] = reserve_handler,
    [ISR_VECTOR_IRQ_58] = reserve_handler,
    [ISR_VECTOR_IRQ_59] = reserve_handler,
    [ISR_VECTOR_IRQ_60] = reserve_handler,
    [ISR_VECTOR_IRQ_61] = reserve_handler,
    [ISR_VECTOR_IRQ_62] = reserve_handler,
    [ISR_VECTOR_IRQ_63] = reserve_handler,
    [ISR_VECTOR_IRQ_64] = reserve_handler,
    [ISR_VECTOR_IRQ_65] = reserve_handler,
    [ISR_VECTOR_IRQ_66] = reserve_handler,
    [ISR_VECTOR_IRQ_67] = reserve_handler,
    [ISR_VECTOR_IRQ_68] = reserve_handler,
    [ISR_VECTOR_IRQ_69] = reserve_handler,
    [ISR_VECTOR_IRQ_70] = reserve_handler,
    [ISR_VECTOR_IRQ_71] = reserve_handler,
    [ISR_VECTOR_IRQ_72] = reserve_handler,
    [ISR_VECTOR_IRQ_73] = reserve_handler,
    [ISR_VECTOR_IRQ_74] = reserve_handler,
    [ISR_VECTOR_IRQ_75] = reserve_handler,
    [ISR_VECTOR_IRQ_76] = reserve_handler,
    [ISR_VECTOR_IRQ_77] = reserve_handler,
    [ISR_VECTOR_IRQ_78] = reserve_handler,
    [ISR_VECTOR_IRQ_79] = reserve_handler,
    [ISR_VECTOR_IRQ_80] = reserve_handler,
    [ISR_VECTOR_IRQ_81] = reserve_handler,
    [ISR_VECTOR_IRQ_82] = reserve_handler,
    [ISR_VECTOR_IRQ_83] = reserve_handler,
    [ISR_VECTOR_IRQ_84] = reserve_handler,
    [ISR_VECTOR_IRQ_85] = reserve_handler,
    [ISR_VECTOR_IRQ_86] = reserve_handler,
    [ISR_VECTOR_IRQ_87] = reserve_handler,
};

const isr_function *isr_get_ramexceptiontable_addr(void)
{
    UNUSED(isr_not_implemented);
    return g_ram_exception_table;
}

#endif
