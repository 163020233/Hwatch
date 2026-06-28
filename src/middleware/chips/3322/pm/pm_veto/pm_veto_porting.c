/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provides pm veto port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-13， Create file. \n
 */

#include "chip_io.h"
#include "core.h"
#include "uart.h"
#include "pm_veto_porting.h"

static bool pm_get_ipc_clr_status(void)
{
#if CORE == BT
    return (readw(B_CTL_RB_B2X_SOFT_INT_CLR_STS_REG) != 0);
#endif

#if CORE == APPS
    return (readw(M_CTL_RB_M2X_SOFT_INT_CLR_STS_REG) != 0);
#endif
}

bool pm_port_get_customized_sleep_veto(void)
{
#if defined(CONFIG_UART_SUPPORT_CUSTOMIZED_LPM)
    for (uint32_t i = 0; i < PM_RESUME_UART_BUS_NUM; i++) {
        if (hal_uart_get_funcs(i) == NULL) {
            continue;
        }

        if (!uapi_uart_tx_fifo_is_empty(i)) {
            return true; // uart busy
        }
    }
#endif

    if (pm_get_ipc_clr_status()) {
        return true;     // ipc busy
    }

    return false;
}
