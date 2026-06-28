/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: touch ctrl driver.
 * Create: 2025-11-11
 */

#ifndef TOUCH_CTRL_H
#define TOUCH_CTRL_H

#include "stdio.h"
#include "debug_print.h"
#include "stdint.h"
#include "product.h"
#include "td_base.h"
#include "platform_core.h"
#include "touch_def.h"
#include "product_pin.h"
#include "product_evb_standard.h"

/* host peripheral definitions */
#ifdef CONFIG_INPUT_USING_TPTYPE_CST820
#define TOUCH_I2C_ADDR 0x15
#else
#define TOUCH_I2C_ADDR 0x20
#endif
#define TOUCH_I2C_SPEED I2C_SPEED_MODE_SS
#define TOUCH_I2C_SPEED_BAUDRATE 400000
#define TOUCH_REG_BYTE_LEN 2
#define OFFSET_8_BITS 8
#define TOUCH_WRITE_LEN 4
#ifdef CONFIG_INPUT_USING_TPTYPE_CST820
#define TOUCH_REG_WRITE_LEN 2
#define TOUCH_CMD_SEND_LEN 1
#define TOUCH_REG_DATA_LEN 1
#else
#define TOUCH_REG_WRITE_LEN 4
#define TOUCH_CMD_SEND_LEN 2
#define TOUCH_REG_DATA_LEN 2
#endif
#define TOUCH_I2C_TIME_MAX 3
#define TOUCH_INPUT_SIZE 156
#define TOUCH_I2C_SEND_INDEX0 0
#define TOUCH_I2C_SEND_INDEX1 1
#define TOUCH_I2C_SEND_INDEX2 2
#define TOUCH_I2C_SEND_INDEX3 3

typedef struct {
    uint32_t int_gpio;
    i2c_bus_t i2c_id;
    uint32_t i2c_speed;
} touch_ctrl_attr_t;

typedef ext_errno (*touch_bus_init)(touch_ctrl_attr_t *attr);

typedef struct {
    touch_ctrl_attr_t attr;
    touch_bus_init bus_init;
} touch_ctrl_ops_t;

ext_errno touch_host_peripheral_init(touch_ctrl_attr_t *attr);
ext_errno touch_register_handle(const touch_ctrl_ops_t *peri_data, touch_callback func);
ext_errno touch_unregister_handle(const touch_ctrl_ops_t *peri_data);
ext_errno touch_reset(void);
void touch_resume_reset(void);
void touch_delay(uint32_t ms);

ext_errno tp_i2c_data_read(uint16_t reg_addr, uint8_t *data_buf, uint32_t data_len);
ext_errno tp_i2c_cmd_write(uint16_t cmd);
ext_errno tp_i2c_reg_write(uint16_t reg_addr, uint16_t reg_cfg);
ext_errno tp_i2c_data_write(uint16_t reg_addr, uint8_t *data_buf, uint32_t data_len);
void *touch_screen_get_peri_attr(void);

#endif
