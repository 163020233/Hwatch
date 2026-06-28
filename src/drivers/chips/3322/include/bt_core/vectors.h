/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description:  Vector Table Definitions
 *
 * Create:  2018-10-15
 */
#ifndef VECTORS_H
#define VECTORS_H

#include <stdint.h>
#include "core.h"
#include "interrupt.h"

/** @addtogroup connectivity_config_vectors VECTORS
  * @{
  */
/**
 * @brief  Defined to be the highest ISR_VECTOR supported by the core
 */
#define ISR_VECTOR_MAX_SUPPORTED (BUTT_IRQN - 1)

#define LOCIEN_IRQ_NUM    32
#define LOCIPRI_IRQ_NUM   8
#define LOCIPRI_IRQ_BITS  4
#define LOCIPRI_DEFAULT_VAL 0x11111111 /* The default interrupt priority is 1. */

/**
 * Highest priority of a hardware interrupt.
 */
#define  INTERRUPT_PRIO_HIGHEST        7

/**
 * Lowest priority of a hardware interrupt.
 */
#define  INTERRUPT_PRIO_LOWEST         1

/**
 * Count of HimiDeer system interrupt vector.
 */
#define RISCV_SYS_VECTOR_CNT        16

/**
 * @brief  Interrupt vector identifiers
 */
typedef enum {
    RESERVED_INT0_IRQN           = 0,   // !< RISCV Trap Entry
    RESERVED_INT1_IRQN           = 1,
    RESERVED_INT2_IRQN           = 2,
    RESERVED_INT3_IRQN           = 3,
    RESERVED_INT4_IRQN           = 4,
    RESERVED_INT5_IRQN           = 5,
    RESERVED_INT6_IRQN           = 6,
    RESERVED_INT7_IRQN           = 7,
    RESERVED_INT8_IRQN           = 8,
    RESERVED_INT9_IRQN           = 9,
    RESERVED_INT10_IRQN          = 10,
    RESERVED_INT11_IRQN          = 11,
    CLIC_SOFTWARE_IRQN           = 12,
    RESERVED_INT13_IRQN          = 13,
    RESERVED_INT14_IRQN          = 14,
    RESERVED_INT15_IRQN          = 15,
    ISR_VECTOR_IRQ_0,               // !< RISCV External Interrupt request 0 vector
    ISR_VECTOR_IRQ_1,              // !< RISCV External Interrupt request 1 vector
    ISR_VECTOR_IRQ_2,              // !< RISCV External Interrupt request 2 vector
    ISR_VECTOR_IRQ_3,              // !< RISCV External Interrupt request 3 vector
    ISR_VECTOR_IRQ_4,              // !< RISCV External Interrupt request 4 vector
    ISR_VECTOR_IRQ_5,              // !< RISCV External Interrupt request 5 vector
    ISR_VECTOR_IRQ_6,              // !< RISCV External Interrupt request 6 vector
    ISR_VECTOR_IRQ_7,              // !< RISCV External Interrupt request 7 vector
    ISR_VECTOR_IRQ_8,              // !< RISCV External Interrupt request 8 vector
    ISR_VECTOR_IRQ_9,              // !< RISCV External Interrupt request 9 vector
    ISR_VECTOR_IRQ_10,             // !< RISCV External Interrupt request 10 vector
    ISR_VECTOR_IRQ_11,             // !< RISCV External Interrupt request 11 vector
    ISR_VECTOR_IRQ_12,             // !< RISCV External Interrupt request 12 vector
    ISR_VECTOR_IRQ_13,             // !< RISCV External Interrupt request 13 vector
    ISR_VECTOR_IRQ_14,             // !< RISCV External Interrupt request 14 vector
    ISR_VECTOR_IRQ_15,             // !< RISCV External Interrupt request 15 vector
    ISR_VECTOR_IRQ_16,             // !< RISCV External Interrupt request 16 vector
    ISR_VECTOR_IRQ_17,             // !< RISCV External Interrupt request 17 vector
    ISR_VECTOR_IRQ_18,             // !< RISCV External Interrupt request 18 vector
    ISR_VECTOR_IRQ_19,             // !< RISCV External Interrupt request 19 vector
    ISR_VECTOR_IRQ_20,             // !< RISCV External Interrupt request 20 vector
    ISR_VECTOR_IRQ_21,             // !< RISCV External Interrupt request 21 vector
    ISR_VECTOR_IRQ_22,             // !< RISCV External Interrupt request 22 vector
    ISR_VECTOR_IRQ_23,             // !< RISCV External Interrupt request 23 vector
    ISR_VECTOR_IRQ_24,             // !< RISCV External Interrupt request 24 vector
    ISR_VECTOR_IRQ_25,             // !< RISCV External Interrupt request 25 vector

    ISR_VECTOR_IRQ_26,             // !< RISCV External Interrupt request 26 vector
    ISR_VECTOR_IRQ_27,             // !< RISCV External Interrupt request 27 vector
    ISR_VECTOR_IRQ_28,             // !< RISCV External Interrupt request 28 vector
    ISR_VECTOR_IRQ_29,             // !< RISCV External Interrupt request 29 vector
    ISR_VECTOR_IRQ_30,             // !< RISCV External Interrupt request 30 vector
    ISR_VECTOR_IRQ_31,             // !< RISCV External Interrupt request 31 vector

    ISR_VECTOR_IRQ_32,             // !< RISCV External Interrupt request 32 vector
    ISR_VECTOR_IRQ_33,             // !< RISCV External Interrupt request 33 vector
    ISR_VECTOR_IRQ_34,             // !< RISCV External Interrupt request 34 vector
    ISR_VECTOR_IRQ_35,             // !< RISCV External Interrupt request 35 vector
    ISR_VECTOR_IRQ_36,             // !< RISCV External Interrupt request 36 vector
    ISR_VECTOR_IRQ_37,             // !< RISCV External Interrupt request 37 vector
    ISR_VECTOR_IRQ_38,             // !< RISCV External Interrupt request 38 vector
    ISR_VECTOR_IRQ_39,             // !< RISCV External Interrupt request 39 vector
    ISR_VECTOR_IRQ_40,             // !< RISCV External Interrupt request 40 vector
    ISR_VECTOR_IRQ_41,             // !< RISCV External Interrupt request 41 vector
    ISR_VECTOR_IRQ_42,             // !< RISCV External Interrupt request 42 vector
    ISR_VECTOR_IRQ_43,             // !< RISCV External Interrupt request 43 vector
    ISR_VECTOR_IRQ_44,             // !< RISCV External Interrupt request 44 vector
    ISR_VECTOR_IRQ_45,             // !< RISCV External Interrupt request 45 vector
    ISR_VECTOR_IRQ_46,             // !< RISCV External Interrupt request 46 vector
    ISR_VECTOR_IRQ_47,             // !< RISCV External Interrupt request 47 vector
    ISR_VECTOR_IRQ_48,             // !< RISCV External Interrupt request 48 vector
    ISR_VECTOR_IRQ_49,             // !< RISCV External Interrupt request 49 vector
    ISR_VECTOR_IRQ_50,             // !< RISCV External Interrupt request 50 vector
    ISR_VECTOR_IRQ_51,             // !< RISCV External Interrupt request 51 vector
    ISR_VECTOR_IRQ_52,             // !< RISCV External Interrupt request 52 vector
    ISR_VECTOR_IRQ_53,             // !< RISCV External Interrupt request 53 vector
    ISR_VECTOR_IRQ_54,             // !< RISCV External Interrupt request 54 vector
} isr_vector_t;

/**
 * @brief  Get the ram exception table address.
 * @return The ram exception table address.
 */
const isr_function* isr_get_ramexceptiontable_addr(void);

typedef struct {
    uint32_t mstatus;
    uint32_t mepc;
    uint32_t tp;    /* X4 */
    uint32_t sp;    /* X2 */

    uint32_t s11;   /* X27 */
    uint32_t s10;   /* X26 */
    uint32_t s9;    /* X25 */
    uint32_t s8;    /* X24 */
    uint32_t s7;    /* X23 */
    uint32_t s6;    /* X22 */
    uint32_t s5;    /* X21 */
    uint32_t s4;    /* X20 */
    uint32_t s3;    /* X19 */
    uint32_t s2;    /* X18 */

    uint32_t s1;    /* X9 */
    uint32_t s0;    /* X8 */

    uint32_t t6;    /* X31 */
    uint32_t t5;    /* X30 */
    uint32_t t4;    /* X29 */
    uint32_t t3;    /* X28 */

    uint32_t a7;    /* X17 */
    uint32_t a6;    /* X16 */
    uint32_t a5;    /* X15 */
    uint32_t a4;    /* X14 */
    uint32_t a3;    /* X13 */
    uint32_t a2;    /* X12 */
    uint32_t a1;    /* X11 */
    uint32_t a0;    /* X10 */

    uint32_t t2;    /* X7 */
    uint32_t t1;    /* X6 */
    uint32_t t0;    /* X5 */

    uint32_t ra;    /* X1 */
} task_context_t;

typedef struct {
    uint32_t ccause;
    uint32_t mcause;
    uint32_t mtval;
    uint32_t gp;
    task_context_t task_context;
} exc_context_t;

typedef struct exc_info {
    uint16_t phase;
    uint16_t type;
    uint32_t fault_addr;
    uint32_t thrd_pid;
    uint16_t nest_cnt;
    uint16_t reserved;
    exc_context_t *context;
} exc_info_t;

/**
 * @brief  Reserve handler process.
 */
void reserve_handler(void);

/**
 * @brief  Default handler process.
 */
void default_handler(void);

/**
 * @brief  Print exception info.
 * @param  exc Exception info.
 */
void exc_info_display(const exc_info_t *exc);
/**
  * @}
  */
#endif
