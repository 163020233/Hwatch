/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides pinctrl port \n
 *
 * History: \n
 * 2022-08-25， Create file. \n
 */
#include "pinctrl_porting.h"
#include "hal_pinctrl.h"
#include "hal_pinctrl_3322.h"
#if CHIP_FPGA
#include "board_fpga.h"
#elif CHIP_ASIC
#ifdef  CONFIG_PRODUCT_EVB_DITING
#include "board_evb_diting.h"
#else
#include "board_evb.h"
#endif
#endif
#include "pinctrl.h"

#ifndef BIT
#define BIT(x) (1UL << (uint32_t)(x))
#endif

typedef enum {
    PIN_FUNC_0 = BIT(PIN_MODE_0),
    PIN_FUNC_1 = BIT(PIN_MODE_1),
    PIN_FUNC_2 = BIT(PIN_MODE_2),
    PIN_FUNC_3 = BIT(PIN_MODE_3),
    PIN_FUNC_NONE = 0,
} pin_mode_func_t;

typedef enum {
    PIO_MODE_0 = BIT(0),
    PIO_MODE_1 = BIT(1),
    PIO_MODE_2 = BIT(2),
    PIO_MODE_3 = BIT(3),
    PIO_MODE_NONE = 0,
} pio_mode_t;

static uint8_t const g_pio_pins_avaliable_mode[PIN_MAX_NUMBER] = {
    PIO_MODE_0 | PIO_MODE_1, // S_EGPIO0
    PIO_MODE_0 | PIO_MODE_1, // S_EGPIO1
    PIO_MODE_0 | PIO_MODE_1, // S_EGPIO2
    PIO_MODE_0 | PIO_MODE_1, // S_EGPIO3
    PIO_MODE_0 | PIO_MODE_1, // S_EGPIO4
    PIO_MODE_0 | PIO_MODE_1, // S_EGPIO5
    PIO_MODE_0 | PIO_MODE_1, // S_EGPIO6
    PIO_MODE_0 | PIO_MODE_1, // S_EGPIO7
    PIO_MODE_0 | PIO_MODE_1, // S_EGPIO8
    PIO_MODE_0 | PIO_MODE_1, // S_EGPIO9
    PIO_MODE_0 | PIO_MODE_1, // S_EGPIO10
    PIO_MODE_0 | PIO_MODE_1, // S_EGPIO11
    PIO_MODE_0,              // S_EGPIO12
    PIO_MODE_0,              // S_EGPIO13
    PIO_MODE_0,              // S_EGPIO14
    PIO_MODE_0,              // S_EGPIO15
    PIO_MODE_0,              // S_EGPIO16
    PIO_MODE_0,              // S_EGPIO17
    PIO_MODE_0,              // S_EGPIO18
    PIO_MODE_0,              // S_EGPIO19
    PIO_MODE_0,              // S_EGPIO20
    PIO_MODE_0,              // S_EGPIO21
    PIO_MODE_0,              // S_EGPIO22
    PIO_MODE_0,              // S_EGPIO23
    PIO_MODE_0,              // S_EGPIO24
    PIO_MODE_0,              // S_EGPIO25
    PIO_MODE_0,              // S_EGPIO26
    PIO_MODE_0,              // S_EGPIO27
    PIO_MODE_0,              // S_EGPIO28
    PIO_MODE_0,              // S_EGPIO29
    PIO_MODE_0,              // S_EGPIO30
    PIO_MODE_0,              // S_EGPIO31

    PIO_MODE_0 | PIO_MODE_1,                           // S_MGPIO0
    PIO_MODE_0 | PIO_MODE_1,                           // S_MGPIO1
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,              // S_MGPIO2
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,              // S_MGPIO3
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,              // S_MGPIO4
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,              // S_MGPIO5
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,              // S_MGPIO6
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,              // S_MGPIO7
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,              // S_MGPIO8
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,              // S_MGPIO9
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,              // S_MGPIO10
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,              // S_MGPIO11
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_MGPIO12
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_MGPIO13
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_MGPIO14
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_MGPIO15
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_MGPIO16
    PIO_MODE_0 | PIO_MODE_1,                           // S_MGPIO17
    PIO_MODE_0 | PIO_MODE_1,                           // S_MGPIO18
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,              // S_MGPIO19
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,              // S_MGPIO20
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_MGPIO21
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_MGPIO22
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_MGPIO23
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_MGPIO24
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_MGPIO25
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_MGPIO26

    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2, // EMMC_D0
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2, // EMMC_D1
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2, // EMMC_D2
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2, // EMMC_D3
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2, // EMMC_D4
    PIO_MODE_0 | PIO_MODE_1,              // EMMC_D5TX
    PIO_MODE_0 | PIO_MODE_1,              // EMMC_D6RX
    PIO_MODE_0 | PIO_MODE_1,              // EMMC_D7
    PIO_MODE_0 | PIO_MODE_1,              // EMMC_CMD
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2, // EMMC_CLK
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3, // SDIO_D0
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3, // SDIO_D1
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3, // SDIO_D2
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3, // SDIO_D3
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3, // SDIO_CLK
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3, // SDIO_CMD

    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO0
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO1
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO2
    PIO_MODE_0 | PIO_MODE_2 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO3
    PIO_MODE_0 | PIO_MODE_2 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO4
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO5
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO6
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO7
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO8
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO9
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO10
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO11
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO12
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO13
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO14
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO15
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2,              // S_AGPIO16
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO17
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO18
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO19
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO20
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO21
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO22
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO23
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO24
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO25
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO26
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO27
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO28
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO29
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO30
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO31
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_3,              // S_AGPIO32
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO33
    PIO_MODE_0 | PIO_MODE_1 | PIO_MODE_2 | PIO_MODE_3, // S_AGPIO34

    PIO_MODE_0 | PIO_MODE_1, // S_UGPIO0
    PIO_MODE_0 | PIO_MODE_1, // S_UGPIO1

};

bool pin_check_mode_is_valid(pin_t pin, pin_mode_t mode)
{
    return (bool)((g_pio_pins_avaliable_mode[pin] >> (uint8_t)mode) & 1U);
}

#if CORE == APPS || CORE == BT
void get_pio_func_config(size_t *pin_num, hal_pio_config_t **pin_func_array)
{
    if (pin_num == NULL || pin_func_array == NULL) {
        return;
    }
    *pin_num = sizeof(g_pio_function_config) / sizeof(g_pio_function_config[0]);
    *pin_func_array = g_pio_function_config;
}

void pin_port_register_hal_funcs(void)
{
    hal_pin_register_funcs(hal_pin_3322_funcs_get());
}

void pin_port_unregister_hal_funcs(void)
{
    hal_pin_unregister_funcs();
}
#endif