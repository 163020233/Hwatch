/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description:  PMU interrupt FUNCTIONS
 */

#include "pmu_interrupt.h"
#include "tcxo.h"
#include "pmu_cmu.h"
#include "debug_print.h"
#include "hal_pmu_cmu.h"
#ifdef BUILD_APPLICATION_STANDARD
#include "log_oam_logger.h"
#include "log_def.h"
#include "log_printf.h"
#endif
#include "los_hwi.h"
#include "panic.h"
#include "hal_reg_config.h"
#include "osal_interrupt.h"

#define CHECK_FREQUENCY 1000
#define PMU_LDO_TIMEOUT 1000
#define PMU_LDO_UNLOCK_CLEAR_PANIC 1UL

#define PMU_LPM_MCPU_ULP_GPIO_WAKEUP        13
#define PMU_LPM_MCPU_SDIO_WAKEUP            12

#define SHIFT_16          16UL

static void pmu_cmu_interrupt_handler(void)
{
    hal_pmu_cmu_clear_ana_int_status();

    while (hal_pmu_cmu_get_ana_status() != 0) {
        hal_pmu_cmu_clear_ana_int_status();
    }

    uint64_t time = uapi_tcxo_get_ms();
    uint64_t check_frq = CHECK_FREQUENCY;
    while (1) { //lint !e716
        if (hal_pmu_cmu_get_ana_status() != 0) {
            // Clear interrupt status
            hal_pmu_cmu_clear_ana_int_status();
        } else {
            return;
        }

        check_frq--;
        if (check_frq == 0) {
            if ((time + PMU_LDO_TIMEOUT) < uapi_tcxo_get_ms()) {
                uint16_t ana_status = hal_pmu_cmu_get_ana_status();
                PRINT("[LOW_POWER], ana_status:0x%x\r\n", ana_status);
#ifdef BUILD_APPLICATION_STANDARD
                oml_pf_log_print1(LOG_BCORE_PLT_LPM_SWITCH_CLK, LOG_NUM_LPM_SWITCH_CLK, LOG_LEVEL_ERROR, \
                    "[LOW_POWER], ana_status:0x%x\r\n", ana_status);
#endif
                uint32_t error_code = ((ana_status << SHIFT_16) | PMU_LDO_UNLOCK_CLEAR_PANIC);
                panic(PANIC_PMU_LDO, error_code);
            } else {
                check_frq = CHECK_FREQUENCY;
            }
        }
    }
}

void pmu_init_interrupts(void)
{
    // Register and enable pmu pll unlock status interrupts
    osal_irq_request(PMU_CMU_ERR_IRQN, (osal_irq_handler)pmu_cmu_interrupt_handler, NULL, NULL, NULL);
    osal_irq_enable(PMU_CMU_ERR_IRQN);
}
