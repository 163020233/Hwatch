/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TP CST820 Driver header
 * Create: 2025-11-11
 */

#ifndef MC_TOUCH_CST820_H
#define MC_TOUCH_CST820_H

#include "stdint.h"
#include "input_feature.h"
#include "touch_ctrl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define CST820_REG_CHIP_ID 0xA7
#define CST820_REG_FW_VERSION 0xA9
#define CST820_REG_SLEEP_MODE 0xE5
#define CST820_CHIP_ID 0xB7

#define CST820_TOUCHINFO_HEAD_LEN 2
#define CST820_POINTINFO_LEN 4
#define CST820_TOUCHINFO_LEN (CST820_TOUCHINFO_HEAD_LEN + CST820_POINTINFO_LEN)

#define CST820_XH_OFFSET 2
#define CST820_XL_OFFSET 3
#define CST820_YH_OFFSET 4
#define CST820_YL_OFFSET 5
#define CST820_WIDTH_OFFSET 8

#define CST820_TOUCHINFO_MASK 0x0F
#define CST820_TOUCH_STATE_MASK 0xC0
#define CST820_TOUCH_STATE_BYTE_SHIFT 6

typedef enum {
    CST820_WORK_NORMAL = 0,
    CST820_WORK_STANDBY,
    CST820_WORK_GETCMCP,
    CST820_WORK_SLEEP,
    CST820_WORK_SUSPEND,
    CST820_WORK_RESUME,
    CST820_WORK_INVALID,
} cst820_mode_status_t;

typedef struct {
    uint16_t chip_id;
    uint16_t firmware_version;
    cst820_mode_status_t mode_status;
} cst820_chip_data_t;

int32_t cst820_detect(void);
int32_t cst820_init(touch_chip_info_t *touch_chip_info);
int32_t cst820_set_power_mode(input_dev_workmode_t powerMode);
int32_t cst820_irq_callback(input_event_info_t *eventInfo, uint8_t data_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* MC_TOUCH_CST820_H */
