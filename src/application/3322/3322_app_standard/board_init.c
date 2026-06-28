/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: Application core board init function for standard
 * Author:
 * Create:
 */

#include "board_init.h"
#include "stddef.h"
#include "uart.h"
#include "gpio.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "sfc_v2.h"

static void rc_32k_ulp_gpio_process(void)
{
    uapi_pin_set_mode(S_UGPIO0, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_ds(S_UGPIO0, (pin_drive_strength_t)HAL_PIO_DRIVE_3);
    uapi_pin_set_pull(S_UGPIO0, (pin_pull_t)HAL_PIO_PULL_NONE);
    uapi_gpio_set_dir(S_UGPIO0, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_UGPIO0, GPIO_LEVEL_HIGH);
    uapi_pin_set_mode(S_UGPIO1, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_ds(S_UGPIO1, (pin_drive_strength_t)HAL_PIO_DRIVE_3);
    uapi_pin_set_pull(S_UGPIO1, (pin_pull_t)HAL_PIO_PULL_NONE);
    uapi_gpio_set_dir(S_UGPIO1, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_UGPIO1, GPIO_LEVEL_HIGH);
    return;
}

static void pio_func_init(void)
{
    size_t pin_num;
    hal_pio_config_t *pio_func = NULL;

    get_pio_func_config(&pin_num, &pio_func);

    for (pin_t i = 0; i < pin_num; i++) {
        if (pio_func[i].func == HAL_PIO_FUNC_DEFAULT_HIGH_Z) {
            uapi_pin_set_mode(i, (pin_mode_t)HAL_PIO_FUNC_GPIO);
            uapi_gpio_set_dir(i, GPIO_DIRECTION_INPUT);
        } else {
            uapi_pin_set_mode(i, (pin_mode_t)pio_func[i].func);
        }

        uapi_pin_set_ds(i, (pin_drive_strength_t)pio_func[i].drive);
        uapi_pin_set_pull(i, (pin_pull_t)pio_func[i].pull);
        uapi_pin_set_ie(i, (pin_input_enable_t)pio_func[i].ie);
    }
    if (reg16_getbit(LP_CTL_ULP_A_RB_RC_XO_CLK_MUX_REG, LP_CTL_ULP_A_RB_RC_XO_SEL_OFFSET) == 0) {
        rc_32k_ulp_gpio_process();
        PRINT("rc 32k, disable swd func.\r\n");
    }
}

__attribute__((weak)) void board_init(void)
{
    /* flash init */
    sfc_flash_config_t sfc_cfg = {
        .read_type = FAST_READ_QUAD_IO,
        .write_type = QUAD_INPUT_PAGE_PROGRAM,
        .mapping_addr = FLASH_START,
        .mapping_size = FLASH_LENGTH,
    };

    /* Need first flash power on */
    errcode_t ret = uapi_sfc_init(SFC_ID_0, &sfc_cfg);
    if (ret != ERRCODE_SUCC) {
        PRINT("flash err = 0x%x \r\n", ret);
    }
    pio_func_init();
}
