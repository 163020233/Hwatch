/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TP CST820 Driver
 * Create: 2025-11-11
 */

#include "stdio.h"
#include "soc_errno.h"
#include "soc_osal.h"
#include "platform_core.h"
#include "gpio.h"
#include "cmsis_os.h"
#include "touch_cst820.h"

#define EC_SUCCESS 0
#define EC_FAILURE (-1)

static cst820_chip_data_t g_cst820_chip_data;

static void cst820_hardware_reset(void)
{
    uapi_gpio_set_dir(TOUCH_RESET_GPIO, GPIO_DIRECTION_OUTPUT);

    uapi_gpio_set_val(TOUCH_RESET_GPIO, GPIO_LEVEL_HIGH);
    /* delay 1ms */
    osDelay(1);
    uapi_gpio_set_val(TOUCH_RESET_GPIO, GPIO_LEVEL_LOW);
    /* delay 10ms */
    osDelay(10);
    uapi_gpio_set_val(TOUCH_RESET_GPIO, GPIO_LEVEL_HIGH);
    /* delay 50ms */
    osDelay(50);
}

static void cst820_power_control(BOOL enable)
{
    UNUSED(enable);
    if (enable == TRUE) {
        cst820_hardware_reset();
    }
}

int32_t cst820_reboot(void)
{
    INPUT_FEATURE_PRINT_DEBUG(APP_TOUCH, "TOUCH:TPIC4_Reboot enter\r\n");

    cst820_power_control(FALSE);
    cst820_power_control(TRUE);

    return EC_SUCCESS;
}

static int32_t cst820_get_chip_info(touch_chip_info_t *touch_chip_info)
{
    int32_t ret;
    uint16_t firmware_version = 0;

    if (touch_chip_info == NULL) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "TOUCH:TPIC4_GetChipInfo null err\r\n");
        return EC_FAILURE;
    }

    ret = tp_i2c_data_read(CST820_REG_FW_VERSION, (uint8_t *)&firmware_version, 1);
    if (ret != EC_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "TOUCH:TPIC4_GetChipInfo main fw version err\r\n");
        return EC_FAILURE;
    }

    g_cst820_chip_data.firmware_version = firmware_version;
    return EC_SUCCESS;
}


int32_t cst820_detect(void)
{
    int32_t ret;
    uint16_t chip_id = 0;

    cst820_power_control(FALSE);
    cst820_power_control(TRUE);

    /* Read Chip Id */
    ret = tp_i2c_data_read(CST820_REG_CHIP_ID, (uint8_t *)&chip_id, 1);
    if ((ret != EC_SUCCESS) || (chip_id != CST820_CHIP_ID)) {
        cst820_power_control(FALSE);
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "TOUCH:TPIC4_Detect err\r\n");
        return EC_FAILURE;
    }
    g_cst820_chip_data.chip_id = chip_id;

    return EC_SUCCESS;
}

int32_t cst820_init(touch_chip_info_t *touch_chip_info)
{
    int32_t ret;
    if (touch_chip_info == NULL) {
        return EC_FAILURE;
    }

    ret = cst820_get_chip_info(touch_chip_info);
    if (ret != EC_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "TOUCH:TPIC4Init getChipInfo err\r\n");
        return EC_FAILURE;
    }

    return EC_SUCCESS;
}

static int32_t cst820_set_standby_mode(void)
{
    uint8_t data = CST820_WORK_SLEEP;
    tp_i2c_data_write(CST820_REG_SLEEP_MODE, &data, 1);
    uapi_gpio_set_val(TOUCH_RESET_GPIO, GPIO_LEVEL_LOW);
    uapi_gpio_disable_interrupt(TOUCH_INT_GPIO);
    g_cst820_chip_data.mode_status = CST820_WORK_STANDBY;
    return EC_SUCCESS;
}

static int32_t cst820_set_normal_mode(void)
{
    cst820_reboot();
    uapi_gpio_enable_interrupt(TOUCH_INT_GPIO);
    g_cst820_chip_data.mode_status = CST820_WORK_NORMAL;
    return EC_SUCCESS;
}

static int32_t cst820_set_sleep_mode(void)
{
    uint8_t data = CST820_WORK_SLEEP;
    tp_i2c_data_write(CST820_REG_SLEEP_MODE, &data, 1);
    uapi_gpio_set_val(TOUCH_RESET_GPIO, GPIO_LEVEL_LOW);
    uapi_gpio_disable_interrupt(TOUCH_INT_GPIO);
    g_cst820_chip_data.mode_status = CST820_WORK_SLEEP;
    return EC_SUCCESS;
}

int32_t cst820_set_power_mode(input_dev_workmode_t powerMode)
{
    switch (powerMode) {
        case INPUT_DEV_SLEEP_MODE:
            return cst820_set_sleep_mode();
        case INPUT_DEV_NORMAL_MODE:
            return cst820_set_normal_mode();
        case INPUT_DEV_STANDBY_MODE:
            return cst820_set_standby_mode();
    }

    return EC_SUCCESS;
}

static BOOL cst820_check_int_valid(void)
{
    if ((g_cst820_chip_data.mode_status != CST820_WORK_STANDBY) &&
        (g_cst820_chip_data.mode_status != CST820_WORK_NORMAL)) {
        INPUT_FEATURE_PRINT_DEBUG(APP_TOUCH, "TOUCH:CST820 invalid int err status %{public}d\r\n",
            g_cst820_chip_data.mode_status);
        return FALSE;
    }

    return TRUE;
}

static int32_t cst820_parse_coord(input_event_info_t *input_event_info)
{
    ext_errno ret;
    uint8_t tp_data[CST820_TOUCHINFO_LEN];
    uint32_t x_axis = 0;
    uint32_t y_axis = 0;
    uint8_t state_id = 0;

    ret = tp_i2c_data_read(0x1, &tp_data[0], CST820_TOUCHINFO_LEN);
    if (ret != EC_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "TOUCH:CST820 read input fail \r\n");
        input_event_info->tp_event = MC_TP_INVALID;
    } else {
        x_axis =
            (((tp_data[CST820_XH_OFFSET] & CST820_TOUCHINFO_MASK) << CST820_WIDTH_OFFSET) | tp_data[CST820_XL_OFFSET]);
        y_axis =
            (((tp_data[CST820_YH_OFFSET] & CST820_TOUCHINFO_MASK) << CST820_WIDTH_OFFSET) | tp_data[CST820_YL_OFFSET]);
        state_id = (tp_data[CST820_XH_OFFSET] & CST820_TOUCH_STATE_MASK) >> CST820_TOUCH_STATE_BYTE_SHIFT;
        input_event_info->tp_event_info.x_axis[0] = x_axis;
        input_event_info->tp_event_info.y_axis[0] = y_axis;
        switch (state_id) {
            case 0x0: {
                input_event_info->tp_event = MC_TP_PRESS;
                break;
            }
            case 0x2: {
                input_event_info->tp_event = MC_TP_MOVE;
                break;
            }
            default: {
                input_event_info->tp_event = MC_TP_RELEASE;
                break;
            }
        }
    }

    return EC_SUCCESS;
}

int32_t cst820_irq_callback(input_event_info_t *eventInfo, uint8_t data_len)
{
    int32_t ret;

    if (eventInfo == NULL) {
        return EC_FAILURE;
    }

    if (cst820_check_int_valid() == FALSE) {
        return EC_SUCCESS;
    }

    if (g_cst820_chip_data.mode_status == CST820_WORK_NORMAL) {
        ret = cst820_parse_coord(eventInfo);
    } else {
        ret = EC_FAILURE;
    }

    return ret;
}

static int32_t cst820_deinit(void)
{
    return EC_SUCCESS;
}
