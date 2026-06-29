/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides ipc port \n
 *
 * History: \n
 * 2022-11-26， Create file. \n
 */

#include "platform_types.h"
#include "memory_config_shared.h"
#include "soc_osal.h"
#include "chip_core_irq.h"
#include "hal_ipc_v150.h"
#include "ipc_porting.h"

static const uintptr_t g_ipc_ctrl_base_addr = 0x52000020;

ipc_memory_manage_t g_ipc_memory_init_tx_config[CORE_IPC_TX_PIPE_NUM] = {
    {
        .src_core = CORES_APPS_CORE,
        .dst_core = CORES_BT_CORE,
        .channel = 0,
        .start_addr = M2B_CH0_REGION_START,
        .total_len = M2B_CH0_REGION_LEN
    },
    {
        .src_core = CORES_APPS_CORE,
        .dst_core = CORES_BT_CORE,
        .channel = 1,
        .start_addr = M2B_CH1_REGION_START,
        .total_len = M2B_CH1_REGION_LEN
    },
    {
        .src_core = CORES_APPS_CORE,
        .dst_core = CORES_PROTOCOL_CORE, // dsp
        .channel = 0,
        .start_addr = M2D_CH0_REGION_START,
        .total_len = M2D_CH0_REGION_LEN
    },
    {
        .src_core = CORES_APPS_CORE,
        .dst_core = CORES_PROTOCOL_CORE,
        .channel = 1,
        .start_addr = M2D_CH1_REGION_START,
        .total_len = M2D_CH1_REGION_LEN
    },

};
ipc_memory_manage_t g_ipc_memory_init_rx_config[CORE_IPC_RX_PIPE_NUM] = {
    {
        .src_core = CORES_BT_CORE,
        .dst_core = CORES_APPS_CORE,
        .channel = 0,
        .start_addr = B2M_CH0_REGION_START,
        .total_len = B2M_CH0_REGION_LEN
    },
    {
        .src_core = CORES_BT_CORE,
        .dst_core = CORES_APPS_CORE,
        .channel = 1,
        .start_addr = B2M_CH1_REGION_START,
        .total_len = B2M_CH1_REGION_LEN
    },
    {
        .src_core = CORES_PROTOCOL_CORE,
        .dst_core = CORES_APPS_CORE, // dsp
        .channel = 0,
        .start_addr = D2M_CH0_REGION_START,
        .total_len = D2M_CH0_REGION_LEN
    },
    {
        .src_core = CORES_PROTOCOL_CORE,
        .dst_core = CORES_APPS_CORE,
        .channel = 1,
        .start_addr = D2M_CH1_REGION_START,
        .total_len = D2M_CH1_REGION_LEN
    },
};

static ipc_core_int_mask_t g_ipc_core_int_mask_config[] = {
    {CORES_BT_CORE, 0x1},
    {CORES_PROTOCOL_CORE, 0x4}, // dsp
};

static hal_ipc_funcs_t *g_hal_funcs = NULL;

uintptr_t ipc_porting_base_addr_get(void)
{
    return g_ipc_ctrl_base_addr;
}

void ipc_port_register_hal_funcs(void)
{
    g_hal_funcs = hal_ipc_v150_funcs_get();
    hal_ipc_register_funcs(g_hal_funcs);
}

void ipc_port_unregister_hal_funcs(void)
{
    hal_ipc_unregister_funcs();
    g_hal_funcs = NULL;
}

volatile static uint32_t g_ipc_from_bt = 0;
void irq_bt_int0_handler(void)
{
    g_hal_funcs->clear_core_int(CORES_BT_CORE, 0);
    g_ipc_from_bt++;
    hal_ipc_v150_irq_callback(CORES_BT_CORE, 0);
}

void irq_bt_int1_handler(void)
{
    g_hal_funcs->clear_core_int(CORES_BT_CORE, 1);
    hal_ipc_v150_irq_callback(CORES_BT_CORE, 1);
}

void irq_dsp_int0_handler(void)
{
    g_hal_funcs->clear_core_int(CORES_PROTOCOL_CORE, 0);
    hal_ipc_v150_irq_callback(CORES_PROTOCOL_CORE, 0);
}

void irq_dsp_int1_handler(void)
{
    g_hal_funcs->clear_core_int(CORES_PROTOCOL_CORE, 1);
    hal_ipc_v150_irq_callback(CORES_PROTOCOL_CORE, 1);
}

void ipc_port_register_irq(void)
{
    osal_irq_request(BT_INT0_IRQN, (osal_irq_handler)irq_bt_int0_handler, NULL, NULL, NULL);
    osal_irq_request(BT_INT1_IRQN, (osal_irq_handler)irq_bt_int1_handler, NULL, NULL, NULL);
    osal_irq_request(DSP0_INT0_IRQN, (osal_irq_handler)irq_dsp_int0_handler, NULL, NULL, NULL);
    osal_irq_request(DSP0_INT1_IRQN, (osal_irq_handler)irq_dsp_int1_handler, NULL, NULL, NULL);

    osal_irq_enable(BT_INT0_IRQN);
    osal_irq_enable(BT_INT1_IRQN);
    osal_irq_enable(DSP0_INT0_IRQN);
    osal_irq_enable(DSP0_INT1_IRQN);
}

void ipc_port_unregister_irq(void)
{
    osal_irq_disable(BT_INT0_IRQN);
    osal_irq_disable(BT_INT1_IRQN);
    osal_irq_disable(DSP0_INT0_IRQN);
    osal_irq_disable(DSP0_INT1_IRQN);

    osal_irq_free(BT_INT0_IRQN, NULL);
    osal_irq_free(BT_INT1_IRQN, NULL);
    osal_irq_free(DSP0_INT0_IRQN, NULL);
    osal_irq_free(DSP0_INT1_IRQN, NULL);
}

uint32_t ipc_port_irq_lock(void)
{
    return osal_irq_lock();
}

void ipc_port_irq_restore(uint32_t status)
{
    osal_irq_restore(status);
}

uint16_t ipc_port_get_int_mask(uint8_t core, uint8_t channel)
{
    uint8_t count = (uint8_t)(sizeof(g_ipc_core_int_mask_config) / sizeof(ipc_core_int_mask_t));
    uint8_t i;
    for (i = 0; i < count; i++) {
        if (g_ipc_core_int_mask_config[i].dest_core == core) {
            return g_ipc_core_int_mask_config[i].chl_mask << channel;
        }
    }
    return 0;
}
