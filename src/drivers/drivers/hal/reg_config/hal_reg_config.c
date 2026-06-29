/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   HAL PMU CPU DRIVER.
 *
 * Create: 2020-01-15
 */

#include "hal_reg_config.h"
#include "soc_osal.h"
#include "systick.h"
#include "debug_print.h"
#include "panic.h"

#define CHECK_FREQUENCY                         1000

uint32_t g_pmu_check_reg = 0;
uint32_t g_pmu_check_reg_value = 0;
uint32_t g_pmu_check_reg_bit = 0;

#ifdef HAL_REG_CFG_TIMEOUT_HANDLE
hal_reg_config_timeout_handle g_pmu_check_timeout_handler = NULL;
void hal_reg_config_timeout_handle_register(hal_reg_config_timeout_handle handle)
{
    g_pmu_check_timeout_handler = handle;
}

static void hal_reg_config_timeout_check(uint32_t addr, uint16_t offset, switch_type_t on)
{
    if (g_pmu_check_timeout_handler != NULL) {
        if (g_pmu_check_timeout_handler(addr, offset, on) == 0) {
            panic(PANIC_PMU_LDO, g_pmu_check_reg);
        }
    } else {
        panic(PANIC_PMU_LDO, g_pmu_check_reg);
    }
}
#endif

void hal_reg_status_check_timeout(uint32_t addr, uint16_t offset, switch_type_t on, uint32_t timeout)
{
    uint64_t time = uapi_systick_get_ms();
    uint64_t check_frq = CHECK_FREQUENCY;

    g_pmu_check_reg = addr;
    g_pmu_check_reg_bit = offset;

    while (reg32_getbit((void *)((uintptr_t)addr), offset) != on) {
        check_frq--;
        if (check_frq == 0) {
            if ((time + timeout) < uapi_systick_get_ms()) {
                g_pmu_check_reg_value = readl((void *)((uintptr_t)g_pmu_check_reg));
                PRINT("pmu status timeout!! pmu addr is %x, with value %x\r\n", g_pmu_check_reg, g_pmu_check_reg_value);
#ifdef HAL_REG_CFG_TIMEOUT_HANDLE
                hal_reg_config_timeout_check(addr, offset, on);
#else
                panic(PANIC_PMU_LDO, g_pmu_check_reg);
#endif
            } else {
                check_frq = CHECK_FREQUENCY;
            }
        }
    }

    g_pmu_check_reg = 0;
    g_pmu_check_reg_value = 0;
    g_pmu_check_reg_bit = 0;
}

void hal_reg_config_bit(uint32_t addr, switch_type_t on, REG16_POS position)
{
    unsigned int irq_sts = osal_irq_lock();
    if (on != 0) {
        reg32_setbit ((void *)((uintptr_t) addr), position);
    } else {
        reg32_clrbit ((void *)((uintptr_t) addr), position);
    }
    osal_irq_restore(irq_sts);
}
