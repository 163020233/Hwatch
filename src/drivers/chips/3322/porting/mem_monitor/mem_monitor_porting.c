/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides mem_monitor port \n
 *
 * History: \n
 * 2023-03-15, Create file. \n
 */
#include "soc_osal.h"
#include "common_def.h"
#include "tcxo.h"
#include "chip_core_irq.h"
#include "hal_mem_monitor_v150.h"
#include "mem_monitor.h"
#include "memory_config_common.h"
#include "psram.h"
#include "memory_config_shared.h"
#include "mem_monitor_porting.h"

#define SHARED_MEM_END_ADDR (PSRAM_BASE_ADDR + PSRAM_LENTH)
#define MONITOR_L2MEM_G_START_ADDR (0x20340000)
#define MONITOR_L2MEM_G_END_ADDR (0x204E097F)
#define MONITOR_L2MEM_NPU_START_ADDR (0x20000000)
#define MONITOR_L2MEM_NPU_END_ADDR (0x2005FFFF)
#if CORE == BT
#define MONITOR_BT_START_ADDR (0x0)
#define MONITOR_BT_END_ADDR (0xFFFFF)
#endif

uintptr_t g_mem_monitor_base[MEM_MONITOR_NUM_MAX] = {
    COM_CTL_RB_COM_AHB_MONITOR_EN_0_REG, // COMAHB0
    COM_CTL_RB_COM_AHB_MONITOR_EN_1_REG, // COMAHB1
#if CORE == APPS
    XIP_CACHE_CTL_RB_AXI_MONITOR_EN_0_REG, // PSRAM0
    XIP_CACHE_CTL_RB_AXI_MONITOR_EN_2_REG, // L2RAM GMMU & GPU
    XIP_CACHE_CTL_RB_AXI_MONITOR_EN_3_REG, // L2RAM DSP
#endif
#if CORE == BT
    B_CTL_RB_AHB_MONITOR_EN_0_REG, // BSUB0
    B_CTL_RB_AHB_MONITOR_EN_1_REG, // BSUB0
#endif
};

mem_monitor_region_t g_mem_monitor_region[MEM_MONITOR_NUM_MAX] = {
    {SHARED_MEM_START, SHARED_MEM_END}, // COMAHB0 xx00 xx08
    {SHARED_MEM_START, SHARED_MEM_END}, // COMAHB1 xx04 xx0c
#if CORE == APPS
    {PSRAM_BASE_ADDR, SHARED_MEM_END_ADDR}, // PSRAM0
    {MONITOR_L2MEM_G_START_ADDR, MONITOR_L2MEM_G_END_ADDR}, // L2RAM GMMU & GPU
    {MONITOR_L2MEM_NPU_START_ADDR, MONITOR_L2MEM_NPU_END_ADDR}, // L2RAM NPU
#endif
#if CORE == BT
    {MONITOR_BT_START_ADDR, MONITOR_BT_END_ADDR}, // BSUB0 xx00 xx08
    {MONITOR_BT_START_ADDR, MONITOR_BT_END_ADDR}, // BSUB1 xx04 xx0c
#endif
};

#ifdef CONFIG_SUPPORT_MEM_MONITOR_MULTI_ADDRNUM
mem_monitor_addrnum_t g_mem_monitor_maxaddrnum[MEM_MONITOR_NUM_MAX] = {
    MEM_MONITOR_ADDRNUM_4, // COMAHB0
    MEM_MONITOR_ADDRNUM_4, // COMAHB1
#if CORE == APPS
    MEM_MONITOR_ADDRNUM_2, // PSRAM0
    MEM_MONITOR_ADDRNUM_2, // L2RAM GMMU
    MEM_MONITOR_ADDRNUM_2, // L2RAM DSP
#endif
#if CORE == BT
    MEM_MONITOR_ADDRNUM_4, // BSUB0
    MEM_MONITOR_ADDRNUM_4, // BSUB1
#endif
};
#endif

uintptr_t *mem_monitor_port_base_addr_get(void)
{
    return (uintptr_t *)g_mem_monitor_base;
}

static int mem_monitor_handler(int a, const void *tmp)
{
    unused(a);
    unused(tmp);

    hal_mem_monitor_irq_handler();
    return 0;
}

void mem_monitor_port_register_hal_funcs(void)
{
    hal_mem_monitor_register_funcs(hal_mem_monitor_v150_funcs_get());
}

void mem_monitor_port_unregister_hal_funcs(void)
{
    hal_mem_monitor_unregister_funcs();
}

void mem_monitor_port_register_irq(void)
{
#if CORE == APPS
    osal_irq_request(COMRAM_MONITOR_IRQN, mem_monitor_handler, NULL, NULL, NULL);
    osal_irq_enable(COMRAM_MONITOR_IRQN);
    osal_irq_request(MEM_SUB_MONITOR_IRQN, mem_monitor_handler, NULL, NULL, NULL);
    osal_irq_enable(MEM_SUB_MONITOR_IRQN);
    osal_irq_request(B_SUB_MONITOR_IRQN, mem_monitor_handler, NULL, NULL, NULL);
    osal_irq_enable(B_SUB_MONITOR_IRQN);
#endif
}

void mem_monitor_port_unregister_irq(void)
{
#if CORE == APPS
    osal_irq_disable(COMRAM_MONITOR_IRQN);
    osal_irq_free(COMRAM_MONITOR_IRQN, NULL);
    osal_irq_disable(MEM_SUB_MONITOR_IRQN);
    osal_irq_free(MEM_SUB_MONITOR_IRQN, NULL);
    osal_irq_disable(B_SUB_MONITOR_IRQN);
    osal_irq_free(B_SUB_MONITOR_IRQN, NULL);
#endif
}

static uint32_t g_irq;

void mem_monitor_port_irq_lock(void)
{
    g_irq = osal_irq_lock();
}

void mem_monitor_port_irq_unlock(void)
{
    osal_irq_restore(g_irq);
}

bool mem_monitor_port_check_addr(mem_monitor_t monitor, uint32_t addr_start, uint32_t addr_end)
{
    if ((addr_start < g_mem_monitor_region[monitor].start_addr) ||
        (addr_end > g_mem_monitor_region[monitor].end_addr)) {
        return false;
    }
    return true;
}

#ifdef CONFIG_SUPPORT_MEM_MONITOR_MULTI_ADDRNUM
void mem_monitor_port_reg_addr_en_r(uint32_t monitor, uint32_t section)
{
    uint16_t addrnum = g_mem_monitor_maxaddrnum[monitor];
    if (addrnum != MEM_MONITOR_SECTION_MAX) {
        reg_setbit(g_base_addr[monitor], MEM_MONITOR_ADDREN_REG_OFFSET, addrnum + section);
    } else {
        hal_mem_monitor_reg_addr_en_r(monitor, section);
    }
}

void mem_monitor_port_reg_addr_en_w(uint32_t monitor, uint32_t section)
{
    uint16_t addrnum = g_mem_monitor_maxaddrnum[monitor];
    if (addrnum != MEM_MONITOR_SECTION_MAX) {
        reg_setbit(g_base_addr[monitor], MEM_MONITOR_ADDREN_REG_OFFSET, section);
    } else {
        hal_mem_monitor_reg_addr_en_w(monitor, section);
    }
}
#endif

#if CORE == APPS
#define COM_CTL_RB_CFG_COM_GLB_CLKEN_MONITOR_OFFSET 6
#define TCM_MONITOR_ADDR_LEN 4
#define TCM_MONITOR_ADDR_NUM 4

uapi_tcm_monitor_callback_t g_tcm_monitor_callback = NULL;
void mem_monitor_enable_rec_pc_trigger(void)
{
    reg16_setbit(M_CTL_RB_M_CLKEN0_REG, M_CTL_RB_CFG_M_TRIGGER_CLKEN_OFFSET);              // psram l2mem pc
    reg16_setbit(COM_CTL_RB_C_CRG_CLKEN_REG, COM_CTL_RB_CFG_COM_GLB_CLKEN_MONITOR_OFFSET); // combus clk en
    reg16_setbit(B_CTL_RB_AHB_MONITOR_EN_0_REG, B_CTL_RB_AHB_MONITOR_CLKEN_0_OFFSET);      // b ctl clk en
}

void mem_monitor_trigger_cb_proc(void)
{
    uint32_t intpc = readl(M_CTL_RB_MEM_MONITOR_INT_PC_REG);
    uint32_t intlr = readl(M_CTL_RB_MEM_MONITOR_INT_LR_REG);
    uint32_t intsp = readl(M_CTL_RB_MEM_MONITOR_INT_SP_REG);
    osal_printk("pc:0x%x,lr:%x,sp:0x%x.\r\n", intpc, intlr, intsp);
    reg16_setbit(M_CTL_RB_ACCESS_MEM_INT_OR_CLR_REG, M_CTL_RB_ACCESS_MEM_INT_OR_CLR_OFFSET);
}

static int tcm_monitor_handler(int a, const void *tmp)
{
    unused(a);
    unused(tmp);

    uint16_t interrupt_status = reg16_getbits(M_CTL_RB_MONITOR_TCM_REAG_BAK_2_REG, 0, M_CTL_RB_TCM_MONITOR_INT_STS_LEN);
    uint16_t capture_master_id =
        reg16_getbits(M_CTL_RB_MONITOR_TCM_REAG_BAK_2_REG, M_CTL_RB_TCM_REC_MASTER_OFFSET, M_CTL_RB_TCM_REC_MASTER_LEN);
    uint16_t capture_wr =
        reg16_getbits(M_CTL_RB_MONITOR_TCM_REAG_BAK_2_REG, M_CTL_RB_TCM_REC_WR_OFFSET, M_CTL_RB_TCM_REC_WR_LEN);
    uint32_t intaddr = readl(M_CTL_RB_TCM_REC_ADDR_REG);

    uint32_t intpc = readl(M_CTL_RB_TCM_REC_PC_REG);
    uint32_t intlr = readl(M_CTL_RB_TCM_REC_LR_REG);
    uint32_t intsp = readl(M_CTL_RB_TCM_REC_SP_REG);

    osal_printk("Status:0x%x\n", interrupt_status);
    osal_printk("ID: 0x%x\n", capture_master_id);
    osal_printk("wr status:%u\n", capture_wr);

    osal_printk("addr: 0x%x\n", intaddr);
    osal_printk("pc: 0x%x\n", intpc);
    osal_printk("intlr: 0x%x\n", intlr);
    osal_printk("intsp: 0x%x\n", intsp);

    if (g_tcm_monitor_callback && (interrupt_status != 0)) {
        g_tcm_monitor_callback(intaddr, capture_master_id);
    }
    reg16_setbit(M_CTL_RB_MONITOR_TCM_CFG1_REG, M_CTL_RB_TCM_MONITOR_INT_CLR_OFFSET);

    return 0;
}
errcode_t uapi_tcm_monitor_init(void)
{
    osal_irq_request(TCM_MONITOR_IRQN, tcm_monitor_handler, NULL, NULL, NULL);
    osal_irq_enable(TCM_MONITOR_IRQN);
    return ERRCODE_SUCC;
}

void uapi_tcm_monitor_deinit(void)
{
    osal_irq_disable(TCM_MONITOR_IRQN);
    osal_irq_free(TCM_MONITOR_IRQN, NULL);
}
void uapi_tcm_monitor_enable(uapi_tcm_monitor_callback_t callback)
{
    reg16_setbit(M_CTL_RB_M_CLKEN0_REG, M_CTL_RB_CFG_M_TRIGGER_CLKEN_OFFSET);

    reg16_setbit(M_CTL_RB_MONITOR_TCM_CFG1_REG, M_CTL_RB_CPUTCM_MONITOR_EN_OFFSET);
    g_tcm_monitor_callback = callback;
}

void uapi_tcm_monitor_disable(void)
{
    reg16_setbit(M_CTL_RB_MONITOR_TCM_CFG1_REG, M_CTL_RB_TCM_MONITOR_INT_CLR_OFFSET);
    reg16_clrbit(M_CTL_RB_MONITOR_TCM_CFG1_REG, M_CTL_RB_CPUTCM_MONITOR_EN_OFFSET);
    reg16_clrbit(M_CTL_RB_M_CLKEN0_REG, M_CTL_RB_CFG_M_TRIGGER_CLKEN_OFFSET);
}

errcode_t uapi_tcm_monitor_set_section(tcm_monitor_config_t *config)
{
    writel(M_CTL_RB_MONITOR_TCM_ADDR0_L_REG + config->section * TCM_MONITOR_ADDR_LEN, config->addr_start);
    writel(M_CTL_RB_MONITOR_TCM_ADDR0_H_REG + config->section * TCM_MONITOR_ADDR_LEN, config->addr_end);
    if (config->mode == (tcm_monitor_mode_t)HAL_MEM_MONITOR_MODE_READ) {
        reg16_setbit(M_CTL_RB_MONITOR_TCM_CFG1_REG, config->section + TCM_MONITOR_ADDR_NUM);
    } else {
        reg16_setbit(M_CTL_RB_MONITOR_TCM_CFG1_REG, config->section);
    }
    return ERRCODE_SUCC;
}

#endif