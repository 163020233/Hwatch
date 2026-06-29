/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: NON OS Utils
 * Author:
 * Create:  2018-10-15
 */

#include "product.h"
#include "non_os.h"
#include "core.h"
#include "chip_io.h"
#include "non_os_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define NON_OS_BT_ROM_VERSION_LEN   16

#define FLASH_INIT_FLAG_REG 0x5700001c

static volatile uint64_t g_driver_init_bits = 0;

#if defined CHIP_LIBRA
static const char g_bt_rom_version[NON_OS_BT_ROM_VERSION_LEN] = "LIBRAV100";
#elif defined CHIP_SOCMN1
const char g_bt_rom_version[NON_OS_BT_ROM_VERSION_LEN] = "SOCMN1V100";
#elif defined CHIP_BRANDY
const char g_bt_rom_version[NON_OS_BT_ROM_VERSION_LEN] = "BRANDYV100";
#else
const char g_bt_rom_version[NON_OS_BT_ROM_VERSION_LEN] = "XXXXXXV100";
#endif

bool non_os_is_driver_initialised(driver_init_bit_e driver)
{
    return ((g_driver_init_bits & (uint64_t)((uint64_t)0x01UL << (uint32_t)driver)) != 0);
}

void non_os_set_driver_initalised(driver_init_bit_e driver, bool value)
{
    non_os_enter_critical();
    uint64_t mask = (uint64_t)0x01UL << (uint32_t)driver;
    if (value) {
        g_driver_init_bits |= mask;
    } else {
        g_driver_init_bits &= (~mask);
    }
    non_os_exit_critical();
}

/**
* check aon flag is set
* flag save on aon only rst when all rst(not only cpu reset)
* @return true set already
*         false not set
*/
bool non_os_is_aon_flag_initialised(aon_init_bit_e driver)
{
    return (reg16_getbits(FLASH_INIT_FLAG_REG, (uint8_t)driver, 1) != 0) ? true : false;
}

/**
* set aon flag
* flag save on aon only rst when all rst(not only cpu reset)
* @return NULL
*/
void non_os_set_aon_flag_initalised(aon_init_bit_e driver, bool value)
{
    non_os_enter_critical();
    if (value) {
        reg16_setbit(FLASH_INIT_FLAG_REG, driver);
    } else {
        reg16_clrbit(FLASH_INIT_FLAG_REG, driver);
    }
    non_os_exit_critical();
}

const char *non_os_get_bt_rom_version(void)
{
    return g_bt_rom_version;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
