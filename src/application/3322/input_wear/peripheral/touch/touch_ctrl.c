/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: touch touch driver.
 * Create: 2025-11-11
 */

#include "touch_ctrl.h"
#include "securec.h"
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "common_def.h"
#include "pinctrl.h"
#include "chip_io.h"
#include "tcxo.h"
#include "time.h"
#include "i2c.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "pinctrl_porting.h"

bool g_host_dev_init_flag = TD_FALSE;
touch_ctrl_attr_t g_ctrl_attr;
touch_callback g_touch_callback = NULL;
static uint8_t g_tp_send_buf[TOUCH_WRITE_LEN] = {0};
static uint8_t g_tp_recv_buf[TOUCH_INPUT_SIZE] = {0};

static void touch_gpio_isr_handle(pin_t pin, uintptr_t param)
{
    unused(pin);
    unused(param);
    if (g_touch_callback != NULL) {
        g_touch_callback();
    }
}

ext_errno touch_register_handle(const touch_ctrl_ops_t *peri_data, touch_callback func)
{
    unused(peri_data);
    ext_errno ret;

    g_touch_callback = func;
    /* register int io interrupt handler */
    uapi_gpio_init();
    uapi_gpio_set_dir(TOUCH_INT_GPIO, GPIO_DIRECTION_INPUT);
    ret = uapi_gpio_register_isr_func(TOUCH_INT_GPIO, GPIO_INTERRUPT_FALLING_EDGE, touch_gpio_isr_handle);
    if (ret != 0) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "touch int callback register fail! ret=0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno touch_unregister_handle(const touch_ctrl_ops_t *peri_data)
{
    unused(peri_data);
    ext_errno ret;

    /* unregister interrupt handler */
    uapi_gpio_unregister_isr_func(TOUCH_INT_GPIO);
    g_touch_callback = NULL;

    return EXT_ERR_SUCCESS;
}

static ext_errno tp_peripheral_init_status_check(void)
{
    if (g_host_dev_init_flag != TD_TRUE) {
        return EXT_ERR_TP_DEV_INFO_NOT_REGISTER;
    }

    return EXT_ERR_SUCCESS;
}

static ext_errno touch_ctrl_attr_check(const touch_ctrl_attr_t *device_attr)
{
    unused(device_attr);
    return EXT_ERR_SUCCESS;
}

static ext_errno tp_i2c_rd_wr_param_check(const uint8_t *data_buf, uint32_t data_len)
{
    unused(data_buf);
    unused(data_len);
    ext_errno ret = tp_peripheral_init_status_check();
    if (ret != EXT_ERR_SUCCESS) {
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno tp_i2c_data_read(uint16_t reg_addr, uint8_t *data_buf, uint32_t data_len)
{
    errcode_t ret;
    uint32_t retry;
    i2c_data_t data;

    ret = tp_i2c_rd_wr_param_check(data_buf, data_len);
    if (ret != 0) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tp data read param invalid! ret=0x%x", ret);
        return ret;
    }

    if (memset_s(g_tp_recv_buf, data_len, 0, data_len) != EOK) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tp read buff set fail! ret=0x%x", ret);
        return EXT_ERR_FAILURE;
    }
    g_tp_send_buf[TOUCH_I2C_SEND_INDEX0] = reg_addr & 0xff;
    g_tp_send_buf[TOUCH_I2C_SEND_INDEX1] = (reg_addr >> OFFSET_8_BITS) & 0xff;

    data.send_buf = g_tp_send_buf;
    data.send_len = TOUCH_CMD_SEND_LEN;
    data.receive_buf = g_tp_recv_buf;
    data.receive_len = data_len;
    for (retry = 0; retry < TOUCH_I2C_TIME_MAX; retry++) {
        ret = uapi_i2c_master_writeread(TOUCH_I2C_BUS, TOUCH_I2C_ADDR, &data);
        if (ret == EXT_ERR_SUCCESS) {
            break;
        }
    }
    if (ret != EXT_ERR_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tp data read fail! ret=0x%x", ret);
        return ret;
    }

    ret = (uint32_t)memcpy_s(data_buf, data_len, g_tp_recv_buf, data_len);
    if (ret != EOK) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tp data buf content copy fail! ret=0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno tp_i2c_cmd_write(uint16_t cmd)
{
    errcode_t ret;
    uint32_t retry;
    i2c_data_t data;

    ret = tp_peripheral_init_status_check();
    if (ret != 0) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "host peripheral not init! ret=0x%x", ret);
        return ret;
    }
    g_tp_send_buf[TOUCH_I2C_SEND_INDEX0] = cmd & 0xff;
    g_tp_send_buf[TOUCH_I2C_SEND_INDEX1] = (cmd >> OFFSET_8_BITS) & 0xff;

    data.send_buf = g_tp_send_buf;
    data.send_len = TOUCH_CMD_SEND_LEN;
    data.receive_buf = NULL;
    data.receive_len = 0;

    for (retry = 0; retry < TOUCH_I2C_TIME_MAX; retry++) {
        ret = uapi_i2c_master_write(TOUCH_I2C_BUS, TOUCH_I2C_ADDR, &data);
        if (ret == EXT_ERR_SUCCESS) {
            break;
        }
    }
    if (ret != EXT_ERR_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tp data write fail! ret=0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno tp_i2c_reg_write(uint16_t reg_addr, uint16_t reg_cfg)
{
    errcode_t ret;
    uint32_t retry;
    i2c_data_t data;

    ret = tp_peripheral_init_status_check();
    if (ret != 0) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "host peripheral not init! ret=0x%x", ret);
        return ret;
    }

#if TOUCH_REG_WRITE_LEN > 2
    g_tp_send_buf[TOUCH_I2C_SEND_INDEX0] = reg_addr & 0xff;
    g_tp_send_buf[TOUCH_I2C_SEND_INDEX1] = (reg_addr >> OFFSET_8_BITS) & 0xff;
    g_tp_send_buf[TOUCH_I2C_SEND_INDEX2] = reg_cfg & 0xff;
    g_tp_send_buf[TOUCH_I2C_SEND_INDEX3] = (reg_cfg >> OFFSET_8_BITS) & 0xff;
#else
    g_tp_send_buf[TOUCH_I2C_SEND_INDEX0] = reg_addr & 0xff;
    g_tp_send_buf[TOUCH_I2C_SEND_INDEX1] = reg_cfg & 0xff;
#endif

    data.send_buf = g_tp_send_buf;
    data.send_len = TOUCH_REG_WRITE_LEN;
    data.receive_buf = NULL;
    data.receive_len = 0;

    for (retry = 0; retry < TOUCH_I2C_TIME_MAX; retry++) {
        ret = uapi_i2c_master_write(TOUCH_I2C_BUS, TOUCH_I2C_ADDR, &data);
        if (ret == EXT_ERR_SUCCESS) {
            break;
        }
    }
    if (ret != EXT_ERR_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tp reg write fail! ret=0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

ext_errno tp_i2c_data_write(uint16_t reg_addr, uint8_t *data_buf, uint32_t data_len)
{
    errcode_t ret;
    uint32_t retry;
    i2c_data_t data;
    uint8_t *write_data = NULL;
    uint32_t send_len;

    ret = tp_i2c_rd_wr_param_check(data_buf, data_len);
    if (ret != 0) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tp data write param invalid! ret=0x%x", ret);
        return ret;
    }

    send_len = data_len + TOUCH_REG_DATA_LEN;

    write_data = (uint8_t *)osal_kmalloc(send_len, OSAL_GFP_KERNEL);
    if (write_data == NULL) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tp send buff malloc fail!");
        return EXT_ERR_MALLOC_FAILURE;
    }

    write_data[TOUCH_I2C_SEND_INDEX0] = reg_addr & 0xff;
    write_data[TOUCH_I2C_SEND_INDEX1] = (reg_addr >> OFFSET_8_BITS) & 0xff;
    ret = (uint32_t)memcpy_s(write_data + TOUCH_REG_DATA_LEN, send_len - TOUCH_REG_DATA_LEN, data_buf, data_len);
    if (ret != EOK) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tp send_buf content copy fail! ret=0x%x", ret);
        osal_kfree(write_data);
        return ret;
    }

    data.send_buf = write_data;
    data.send_len = send_len;
    data.receive_buf = NULL;
    data.receive_len = 0;

    for (retry = 0; retry < TOUCH_I2C_TIME_MAX; retry++) {
        ret = uapi_i2c_master_write(TOUCH_I2C_BUS, TOUCH_I2C_ADDR, &data);
        if (ret == EXT_ERR_SUCCESS) {
            break;
        }
    }
    if (ret != EXT_ERR_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tp data write fail! ret=0x%x", ret);
        osal_kfree(write_data);
        return ret;
    }

    osal_kfree(write_data);
    return EXT_ERR_SUCCESS;
}

ext_errno touch_host_peripheral_init(touch_ctrl_attr_t *attr)
{
    ext_errno ret;

    if (attr == NULL) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "touch communication interface cfg is empty!");
        return EXT_ERR_FAILURE;
    }
    ret = touch_ctrl_attr_check(attr);
    if (ret != EXT_ERR_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tp invalid dev attr! ret=0x%x", ret);
        return ret;
    }

    ret = (ext_errno)memcpy_s(&(g_ctrl_attr), sizeof(touch_ctrl_attr_t), attr, sizeof(touch_ctrl_attr_t));
    if (ret != EOK) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tp attr content copy fail! ret=0x%x", ret);
        return ret;
    }

    /* init i2c */
    errcode_t result;
    uapi_pin_set_mode(S_MGPIO10, (pin_mode_t)HAL_PIO_FUNC_I2C3_M1);
    uapi_pin_set_mode(S_MGPIO11, (pin_mode_t)HAL_PIO_FUNC_I2C3_M1);
    result = uapi_i2c_master_init(TOUCH_I2C_BUS, TOUCH_I2C_SPEED_BAUDRATE, 0); /* 400000: 400KHz */
    if (result != EXT_ERR_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_TOUCH, "tp i2c init fail! ret=0x%x", result);
        return EXT_ERR_SUCCESS;
    }

    PRINT("TOUCH:i2c_id = %d  i2c_speed = %d  slave_addr  = 0x%x \r\n", g_ctrl_attr.i2c_id, g_ctrl_attr.i2c_speed,
        TOUCH_I2C_ADDR);
    g_host_dev_init_flag = true;

    return EXT_ERR_SUCCESS;
}

touch_ctrl_ops_t g_touch_ctrl = {
    .attr = {
        .int_gpio = TOUCH_INT_GPIO,
        .i2c_id = TOUCH_I2C_BUS,
        .i2c_speed = I2C_SPEED_MODE_SS,
    },
    .bus_init = (touch_bus_init)touch_host_peripheral_init,
};

void *touch_screen_get_peri_attr(void)
{
    return ((void *)&g_touch_ctrl);
}