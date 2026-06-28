/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V150 HAL ipc\n
 *
 * History: \n
 * 2022-11-16, Create file. \n
 */
#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "soc_osal.h"
#include "hal_ipc_v150_regs_op.h"
#include "hal_ipc_v150.h"

#define IPC_ENABLE_ALL_INT_CONFIG  0xFFFF

static hal_ipc_irq_callback g_hal_ipc_v150_irq_callback = NULL;

STATIC errcode_t hal_ipc_v150_init(void)
{
    return hal_ipc_v150_regs_init();
}

STATIC void hal_ipc_v150_deinit(void)
{
    hal_ipc_v150_regs_deinit();
}

STATIC void hal_ipc_enable_all_interrupt(void)
{
    hal_ipc_x2self_soft_int_en_write((uint16_t)IPC_ENABLE_ALL_INT_CONFIG);
}

STATIC void hal_ipc_disable_all_interrupt(void)
{
    hal_ipc_x2self_soft_int_en_write(0);
}

/*
 * Get the interrupt status from a source core.
 */
STATIC bool hal_ipc_get_x2self_int_status(uint8_t core, uint8_t channel)
{
    uint16_t value = ipc_port_get_int_mask(core, channel);
    uint16_t status = hal_ipc_x2self_soft_int_sts_read();
    status &= value;
    return (status == 0 ? false : true);
}

/*
 * Get the interrupt triggering status to a destination  core.
 */
STATIC bool hal_ipc_clear_self2x_int(uint8_t core, uint8_t channel)
{
    uint16_t value = ipc_port_get_int_mask(core, channel);
    uint16_t status = hal_ipc_self2x_soft_int_sts_read();
    status &= value;
    return (status == 0 ? false : true);
}

/*
 * Trigger the IPC interrupt to a destination core.
 */
STATIC errcode_t hal_ipc_set_self2x_int(uint8_t core, uint8_t channel)
{
    uint16_t value = ipc_port_get_int_mask(core, channel);
    uint32_t irq_status = ipc_port_irq_lock();
#if defined(CONFIG_IPC_SUPPORT_RST) && (CONFIG_IPC_SUPPORT_RST == 1)
#define IPC_RST_RETRY_CNT_THRESHOLD             10
    static uint16_t err_cnt = 0;
    if (hal_ipc_clear_self2x_int(core, channel)) {
        hal_ipc_self2x_soft_int_set_write(value);
        err_cnt++;
        if (err_cnt > IPC_RST_RETRY_CNT_THRESHOLD) {
            err_cnt = 0;
            hal_ipc_self2x_soft_rst_set_write(~value);
            hal_ipc_self2x_soft_rst_set_write(0xFF);
        }
        ipc_port_irq_restore(irq_status);
        return ERRCODE_SUCC;
    } else {
        err_cnt = 0;
    }
#else
    if (hal_ipc_clear_self2x_int(core, channel)) {
        ipc_port_irq_restore(irq_status);
        return ERRCODE_SUCC;
    }
#endif

    hal_ipc_self2x_soft_int_set_write(value);
    ipc_port_irq_restore(irq_status);

#if defined(CONFIG_IPC_SUPPORT_RST) && (CONFIG_IPC_SUPPORT_RST == 1)
    osal_dsb();

    for (uint32_t i = 0; i < 0x100; i++) {
        if (hal_ipc_clear_self2x_int(core, channel)) {
            return ERRCODE_SUCC;
        }
    }
#endif
    return ERRCODE_SUCC;
}

/*
 * Clear the IPC interrupt from a source core.
 */
STATIC void hal_ipc_clear_x2self_int(uint8_t core, uint8_t channel)
{
    uint16_t value = ipc_port_get_int_mask(core, channel);
    hal_ipc_x2self_soft_int_clr_write(value);
}

STATIC void hal_ipc_register_irq_callback(hal_ipc_irq_callback callback)
{
    g_hal_ipc_v150_irq_callback = callback;
}

STATIC void hal_ipc_unregister_irq_callback(void)
{
    g_hal_ipc_v150_irq_callback = NULL;
}

static hal_ipc_funcs_t g_hal_ipc_v150_funcs = {
    .init = hal_ipc_v150_init,
    .deinit = hal_ipc_v150_deinit,
    .enable_all_int = hal_ipc_enable_all_interrupt,
    .disable_all_int = hal_ipc_disable_all_interrupt,
    .set_core_int = hal_ipc_set_self2x_int,
    .clear_core_int = hal_ipc_clear_x2self_int,
    .get_core_int_sts = hal_ipc_get_x2self_int_status,
    .register_callback = hal_ipc_register_irq_callback,
    .unregister_callback = hal_ipc_unregister_irq_callback
};

hal_ipc_funcs_t *hal_ipc_v150_funcs_get(void)
{
    return &g_hal_ipc_v150_funcs;
}

errcode_t hal_ipc_v150_irq_callback(uint8_t src_core, uint8_t channel)
{
    if (g_hal_ipc_v150_irq_callback == NULL) {
        return ERRCODE_FAIL;
    }
    return g_hal_ipc_v150_irq_callback(src_core, channel);
}