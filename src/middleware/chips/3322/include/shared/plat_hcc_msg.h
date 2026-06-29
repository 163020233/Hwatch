/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: PLAT HCC MSG
 * Author: CompanyName
 */

#ifndef PLAT_HCC_MSG_H
#define PLAT_HCC_MSG_H

#include "stdint.h"
#include "errcode.h"

#define WAKEUP_TRY_CNT 3
#define WAKEUP_TRY_INTERVAL_MS 100

typedef enum _plat_hcc_msg_type {
    PLAT_HCC_MSG_BEGIN = 0,
    PLAT_HCC_MSG_WRITE_REG = 0,
    PLAT_HCC_MSG_ACORE_SLEEP_REQ = 1,
    PLAT_HCC_MSG_IPC_CLEAR_PREMEM = 2,
    PLAT_HCC_MSG_END
} plat_hcc_msg_type;

typedef enum _oam_sub_type {
    OAM_HCC_DEAULT_VALUE = 0,
    DUMP_REG = 1,
    DUMP_MEM = 2,
    CHANGE_LOG_LV = 3,
    WIFI_FREQ = 4,
    DEV_WIFI_BUG_ON = 5,
    DEV_BT_BUG_ON = 6,
    BCPU_CRASH_DUMP_INFO = 7,
    BCPU_CRASH_DUMP_STACK = 8,
    PLAT_MSG_TRANSFER = 9,
} oam_sub_type;

typedef struct {
    uint16_t msg_id;
    uint16_t data_len;
    uint8_t data[0];
} plat_hcc_msg_t;

typedef errcode_t (*plat_hcc_msg_callbck)(uint8_t *data, uint16_t len);

void plat_hcc_msg_init(void);
void plat_hcc_msg_deinit(void);
void plat_hcc_msg_do_register(void);
errcode_t plat_hcc_msg_register(uint16_t msg_id, plat_hcc_msg_callbck cb);
errcode_t plat_hcc_msg_send(uint8_t *buf, uint16_t len, uint16_t msg_id);
void plat_hcc_notify_log_level(uint8_t level);
void plat_hcc_notify_connect_state(bool enable);
errcode_t plat_hcc_notify_clear_ipc_premem(void);
void plat_gpio_wakeup_ws53(void);
errcode_t plat_hcc_notify_sleep_req(void);
errcode_t ws53_hcc_host_resume(uint32_t try_cnt, uint32_t delay_ms);
void plat_sdio_gpio_init(void);
void ws53_hcc_host_suspend(void);
uint8_t get_hcc_id(void);
void host_hcc_reinit_trigger(void);
void hcc_reinit_task(void);
#endif
