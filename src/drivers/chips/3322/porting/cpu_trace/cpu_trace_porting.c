/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description:  \n
 *
 * History: \n
 * 2024-05-31, Create file. \n
 */
#include "cpu_trace_porting.h"
#include "soc_reg.h"
#include "chip_io.h"
#include "tcxo.h"
#include "cpu_trace_unified.h"

#define CPU_TRACE_SAMPLE_START_ADDR_L   0
#define MCPU_TRACE_CONTROL_BASE_ADDR     MCU_DIAG_CTL_RB_CFG_MCU_DIAG_TRACE_SAVE_SEL_REG
#define MCPU_TRACE_SAMPLE_BASE_ADDR      MCU_DIAG_CTL_RB_CFG_MCU_DIAG_SAMPLE_MODE_REG
#define BCPU_TRACE_CONTROL_BASE_ADDR     BCPU_TRACE_CTL_RB_CFG_TRACE_SAVE_SEL_REG
#define BCPU_TRACE_SAMPLE_BASE_ADDR      BCPU_TRACE_CTL_RB_CFG_SAMPLE_MODE_REG

#define BCPU_PCLR_BASE_ADDR             (GLB_CTL_B_RB_BASE + 0x3A0)
#define MCPU_PCLR_BASE_ADDR             (GLB_CTL_M_RB_BASE + 0x3A0)
#define MAX_CPU_TRACE_NMU_CNT           4

uintptr_t g_cpu_pclr_base_value[] = {
    BCPU_PCLR_BASE_ADDR,
    MCPU_PCLR_BASE_ADDR,
};

#if CORE == BT
#define CPU_TYPE CPU_TRACE_TRACED_PORTING_BCPU
#elif CORE == APPS
#define CPU_TYPE CPU_TRACE_TRACED_PORTING_MCPU
#endif
static uint32_t g_cur_cpu_type = CPU_TYPE;

uint32_t cpu_trace_get_self_id(void)
{
    return g_cur_cpu_type;
}

uint32_t *cpu_tace_get_pclr_base_addr(void)
{
    return g_cpu_pclr_base_value;
}

hal_cpu_trace_info_t g_cpu_trace_info[] = {
    {
        .control_base_addr = BCPU_TRACE_CONTROL_BASE_ADDR,
        .sample_base_addr = BCPU_TRACE_SAMPLE_BASE_ADDR,
        .save_location = HAL_CPU_TRACE_SAVE_LOACATION_INTRNAL,
        .sample_mode = HAL_CPU_TRACE_SAMPLE_MODE_REPEAT,
        .sample_len_l = (CPU_TRACE_MEM_REGION_LENGTH / 4),
        .sample_len_h = 0,
        .sample_start_addr_l = CPU_TRACE_SAMPLE_START_ADDR_L,
        .sample_start_addr_h = 0,
        .sample_end_addr_l = CPU_TRACE_SAMPLE_START_ADDR_L + CPU_TRACE_MEM_REGION_LENGTH - 8,
        .sample_end_addr_h = 0,
        .clock = BCPU_TRACE_CLOCK
    },
    {
        .control_base_addr = MCPU_TRACE_CONTROL_BASE_ADDR,
        .sample_base_addr = MCPU_TRACE_SAMPLE_BASE_ADDR,
        .save_location = HAL_CPU_TRACE_SAVE_LOACATION_INTRNAL,
        .sample_mode = HAL_CPU_TRACE_SAMPLE_MODE_REPEAT,
        .sample_len_l = (CPU_TRACE_MEM_REGION_LENGTH / 4),
        .sample_len_h = 0,
        .sample_start_addr_l = CPU_TRACE_SAMPLE_START_ADDR_L,
        .sample_start_addr_h = 0,
        .sample_end_addr_l = CPU_TRACE_SAMPLE_START_ADDR_L + CPU_TRACE_MEM_REGION_LENGTH - 8,
        .sample_end_addr_h = 0,
        .clock = MCPU_TRACE_CLOCK
    },
};

void cpu_trace_get_trace_info_addr(hal_cpu_trace_info_t **info, uint8_t *length)
{
    *length = (uint8_t)CPU_TRACE_CPU_CORE_NUMS;
    *info = g_cpu_trace_info;
}

void cpu_trace_port_enable(void)
{
#if CORE == APPS
    reg16_setbit(M_CTL_RB_M_SOFT_RST_N_REG, M_CTL_RB_SOFT_RST_MDIAG_N_OFFSET); // dereset mdiag
    reg16_setbit(M_CTL_RB_M_CLKEN0_REG, M_CTL_RB_MDIAG_CLKEN_OFFSET); // open mdiag clock enable
    // cpu trace enable for mcu
    reg16_setbit(MCU_DIAG_CTL_RB_CFG_MCU_DIAG_CPU_TRACE_REG, MCU_DIAG_CTL_RB_MCPU_TRACE_ENABLE_OFFSET);
    reg16_clrbit(MCU_DIAG_CTL_RB_CFG_MONITOR_SEL_REG, 0x0); // config monitor sel to cpu trace.
#elif CORE == BT
    reg32_setbit(B_CTL_RB_B_CRG_CLKEN_REG, B_CTL_RB_CFG_B_BT_DIAG_CLKEN_OFFSET); // open bt diag clock
    reg32_setbit(B_CTL_RB_B_CRG_CLKEN_REG, B_CTL_RB_CFG_B_BT_CPU_TRACE_CLKEN_OFFSET); // open bt cpu trace clock
    reg32_setbit(B_CTL_RB_B_CRG_CLKEN_REG, B_CTL_RB_CFG_B_GLUE_TRIGGER_CLKEN_OFFSET); // open bt pclr lock clock
#endif
}

#if CORE == APPS
void cpu_trace_lock_pclr_open_clock(void)
{
    reg16_setbit(M_CTL_RB_M_CLKEN0_REG, M_CTL_RB_CFG_M_TRIGGER_CLKEN_OFFSET);
}

void cpu_trace_lock_pclr_close_clock(void)
{
    reg16_clrbit(M_CTL_RB_M_CLKEN0_REG, M_CTL_RB_CFG_M_TRIGGER_CLKEN_OFFSET);
}
#endif