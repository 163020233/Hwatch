/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: Function of dvp_cam_comm.h
 * Author: AuthorNameMagicTag
 * Create: 2026-03-3
 */

#ifndef MCP_DVP_CAMERA_COMM_H
#define MCP_DVP_CAMERA_COMM_H
#include <stdint.h>
typedef struct {
    uint32_t bus_id;
    uint32_t baudrate;
    uint8_t addr;
} i2c_config_t;

typedef struct {
    uint8_t reg_addr;
    uint8_t value;
} sensor_reg;

typedef struct {
    const char *name;
    i2c_config_t i2c_cfg;
    int pwdn_gpio;
    const sensor_reg *init_regs;
    int init_regs_count;
} sensor_config_t;

#endif