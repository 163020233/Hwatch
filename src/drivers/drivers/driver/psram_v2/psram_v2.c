/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: psram v2 driver source file \n
 *
 * History: \n
 * 2024-05-15, Create file. \n
 */

#include "common_def.h"
#include "tcxo.h"
#include "hal_psram.h"
#include "chip_io.h"
#include "debug_print.h"
#include "systick.h"
#include "psram_porting.h"
#include "psram_v2.h"

__attribute__((weak)) void psram_port_enter_half_refresh(void)
{
}
__attribute__((weak)) void psram_port_exit_half_refresh(void)
{
}

static errcode_t uapi_psram_set_device_register(uint8_t id, uint8_t mr_index, uint8_t val)
{
    uint32_t set_val = 0;
    if (mr_index == MR4) {
        set_val = val << OFFSET_8;
    } else if (mr_index == MR6) {
        set_val = val << OFFSET_16;
    } else if (mr_index == MR8) {
        set_val = val << OFFSET_24;
    } else if (mr_index == MR0) {
        set_val = val;
    }
    hal_psram_init(id);
    hal_psram_dmc_cfg_mr(set_val);
    errcode_t ret = hal_dmac_mr_cfg(mr_index);
    if (ret != ERRCODE_SUCC) {
        PRINT("set id=%d mr%d val=%d error=%x\r\n", id, mr_index, val, ret);
        return ret;
    }
    return ERRCODE_SUCC;
}

static errcode_t uapi_psram_get_device_register(uint8_t id, uint8_t mr_index, uint8_t* val)
{
    uint32_t mr_value = 0;
    if (mr_index > OFFSET_8) {
        return ERRCODE_INVALID_PARAM;
    }
    hal_psram_init(id);
    errcode_t ret = hal_dmac_read_mr_reg((psram_mr_t)mr_index, &mr_value);
    if (ret != ERRCODE_SUCC) {
        PRINT("PSRAM get mr%d error=%x\r\n", mr_index, ret);
        return ret;
    }
    if (psram_port_winbond_flag_get()) {
        *val = (uint8_t)(mr_value >> OFFSET_16);
    } else {
        *val = (uint8_t)mr_value;
    }
    return ERRCODE_SUCC;
}

// 项目在porting添加优化的寄存器配置
__attribute__((weak)) int8_t psram_get_cfg_for_vendor_id(uint8_t id, uint8_t **config_reg, uint8_t *sum)
{
    unused(id);
    *config_reg = NULL;
    *sum = 0;
    return ERRCODE_SUCC;
}

__attribute__((weak)) int8_t psram_winbond_get_cfg_for_vendor_id(uint8_t id, uint16_t **config_reg, uint8_t *sum)
{
    unused(id);
    *config_reg = NULL;
    *sum = 0;
    return ERRCODE_SUCC;
}

static errcode_t uapi_psram_winbond_set_device_register(uint8_t id, uint8_t mr_index, uint32_t val)
{
    hal_psram_init(id);
    hal_psram_dmc_cfg_mr(val);
    errcode_t ret = hal_dmac_mr_cfg(mr_index);
    if (ret != ERRCODE_SUCC) {
        PRINT("set id=%d cr%d val=%d error=%x\r\n", id, mr_index == MR6 ? 1 : 0, val, ret);
        return ret;
    }
    return ERRCODE_SUCC;
}

static errcode_t uapi_psram_winbond_optimize_device_config(uint8_t id)
{
#define PSRAM_WINBOND_VENDOR_ID_MASK    0xF
    errcode_t ret = 0;
    uint8_t vendor_id = 0;
    uint16_t *config;
    uint8_t sum = 0;
    ret = uapi_psram_get_device_register(id, 0x0, &vendor_id); // 0 for getting vendor id
    vendor_id &= PSRAM_WINBOND_VENDOR_ID_MASK;
    if (ret != ERRCODE_SUCC) {
        PRINT("PSRAM id=%d get ir0 error=%x\r\n", id, ret);
        return ret;
    }
    int8_t ret_tmp = psram_winbond_get_cfg_for_vendor_id(vendor_id, &config, &sum);
    if (ret_tmp != ERRCODE_SUCC) {
        PRINT("psram vendor_id=%x get config failed error=%x with id=%d\r\n", vendor_id, ret_tmp, id);
        return ERRCODE_FAIL;
    }
    for (uint8_t i = 0; i < sum; i++) {
        ret = uapi_psram_winbond_set_device_register(id, *(config + i * sum + 0), *(config + i * sum + 1));
        if (ret != ERRCODE_SUCC) {
            PRINT("psram set reg error=%x in index=%d id=%d sum=%d\r\n", ret, i, id, sum);
            return ret;
        }
        PRINT("id=%x, vendorid=%x, set cr%x=%x\r\n",
            id, vendor_id, *(config + i * sum + 0) == MR6 ? 1 : 0, *(config + i * sum + 1));
    }
    return ERRCODE_SUCC;
}

static errcode_t uapi_psram_optimize_device_config(uint8_t id)
{
    errcode_t ret = 0;
    uint8_t vendor_id = 0;
    uint8_t *config;
    uint8_t sum = 0;
    ret = uapi_psram_get_device_register(id, 0x1, &vendor_id); // 1 for getting vendor id
    if (ret != ERRCODE_SUCC) {
        PRINT("PSRAM id=%d get mr2 error=%x\r\n", id, ret);
        return ret;
    }
    vendor_id &= PSRAM_VENDOR_ID_MASK;
    int8_t ret_tmp = psram_get_cfg_for_vendor_id(vendor_id, &config, &sum);
    if (ret_tmp != ERRCODE_SUCC) {
        PRINT("psram vendor_id=%x get config failed error=%x with id=%d\r\n", vendor_id, ret_tmp, id);
        return ERRCODE_FAIL;
    }

    for (uint8_t i = 0; i < sum; i++) {
        ret = uapi_psram_set_device_register(id, *(config + i * sum + 0), *(config + i * sum + 1));
        if (ret != ERRCODE_SUCC) {
            PRINT("psram set reg error=%x in index=%d id=%d sum=%d\r\n", ret, i, id, sum);
            return ret;
        }
        PRINT("id=%x, vendorid=%x, set mr%x=%x\r\n", id, vendor_id, *(config + i * sum + 0), *(config + i * sum + 1));
    }
    return ERRCODE_SUCC;
}

void uapi_psram_controller_init(uint8_t id)
{
    hal_psram_phy_init(id);
    hal_psram_dmac_init();
    hal_psram_axi_init(id);
}

static errcode_t uapi_psram_peripheral_init(uint8_t id)
{
    unused(id);
    psram_port_power_on();
    uapi_tcxo_delay_us(PSRAM_INIT_DELAY_US);
    hal_psram_dmac_mr_reset();
    if (psram_port_winbond_flag_get()) {
        hal_psram_dmc_cfg_mr(0x8f25);
    } else {
        hal_psram_dmc_cfg_mr(PSRAM_MR_VAL);
    }
    errcode_t ret = hal_dmac_mr_cfg(MR0);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uapi_tcxo_delay_us(PSRAM_MR_DELAY_US);
    if (!psram_port_winbond_flag_get()) {
        ret = hal_dmac_mr_cfg(MR8);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        ret = hal_dmac_mr_cfg(MR4);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        uapi_tcxo_delay_us(PSRAM_MR_DELAY_MS);
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_psram_init(uint8_t id)
{
    errcode_t ret = 0;
    psram_port_freq_flag_set();
    hal_psram_init(id);  // 初始化寄存器地址

    uapi_psram_controller_init(id);

    ret = uapi_psram_peripheral_init(id);
    if (ret != ERRCODE_SUCC) {
        PRINT("PSRAM init id=%d error=%x\r\n", id, ret);
        return ret;
    }
    if (psram_port_winbond_flag_get()) {
        ret = uapi_psram_winbond_optimize_device_config(id);
    } else {
        ret = uapi_psram_optimize_device_config(id);
    }
    if (ret != ERRCODE_SUCC) {
        PRINT("PSRAM optimize device config error=%x\r\n", ret);
        return ret;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_psram_suspend(uintptr_t arg)
{
    unused(arg);
#ifdef CONFIG_PSRAM_SUPPORT_HALF_SLEEP
    for (uint32_t i = PSRAM_ID_0; i < PSRAM_ID_MAX; i++) {
        hal_psram_init(i);
        hal_psram_regs_dmac_cfg_hs_enter_req();
    }
#endif
    return ERRCODE_SUCC;
}

errcode_t uapi_psram_resume(uintptr_t arg)
{
    unused(arg);
#ifdef CONFIG_SUPPORT_DLYM_1
    hal_psram_regs_phy_rxfifo_rst();
#endif
    psram_port_freq_flag_set();
    for (uint8_t i = PSRAM_ID_0; i < PSRAM_ID_MAX; i++) {
        hal_psram_init(i);
        uapi_psram_controller_init(i);
#ifdef CONFIG_PSRAM_SUPPORT_HALF_SLEEP
        hal_psram_regs_dmac_cfg_hs_exit_req();
#endif
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_psram_wait_hs_status(uintptr_t arg)
{
#define PSRAM_EXIT_HALFSLEEP_TIMEOUT_COUNT     50ULL   // 50 * 1000 / 32768 = 1500us
    unused(arg);
    if (hal_psram_regs_dmac_cur_hs_status() == false) {
        return ERRCODE_SUCC;
    }

    uint64_t time = uapi_systick_get_count();
    while (hal_psram_regs_dmac_cur_hs_status() == true) {
        if ((time + PSRAM_EXIT_HALFSLEEP_TIMEOUT_COUNT) < uapi_systick_get_count()) {
            return ERRCODE_FAIL;
        }
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_psram_exec_cmd_bp(uintptr_t arg)
{
    unused(arg);
    for (uint8_t i = PSRAM_ID_0; i < PSRAM_ID_MAX; i++) {
        hal_psram_init(i);
        hal_psram_regs_dmac_cfg_hs_exec_cmd_bp();
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_psram_close_cmd_bp(uintptr_t arg)
{
    unused(arg);
    for (uint8_t i = PSRAM_ID_0; i < PSRAM_ID_MAX; i++) {
        hal_psram_init(i);
        hal_psram_regs_dmac_cfg_hs_close_cmd_bp();
    }

    return ERRCODE_SUCC;
}