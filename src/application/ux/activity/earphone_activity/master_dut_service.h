/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux master service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef MASTER_DUT_SERVICE_H
#define MASTER_DUT_SERVICE_H
#include "master_service.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MASTER_DUT_RESET_DELAY      400
#define MASTER_DUT_INDEX_BASE       0xA0

typedef enum {
    DUT_ACC_ON = MASTER_DUT_INDEX_BASE + 1,
    DUT_EXIT,
    DUT_RECOVER_START,
    DUT_RESET,
    DUT_INIT,
    DUT_APP_INFO,
    DUT_WEAR_CONTROL,
    DUT_WEAR_INFO,
    DUT_POSITION_INFO,
    DUT_INIT_RSP,

    DUT_BUTT
} service_dut_master_index_t;

void master_dut_service_execute(uint8_t index, uint8_t *param, uint16_t length);
void dut_timer_set_init(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
