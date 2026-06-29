/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux cmd utils at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-05-02， Create file. \n
 */
#ifndef UX_CMD_UTILS_H
#define UX_CMD_UTILS_H
#include <stdint.h>
#include "errcode.h"
#include "ux_api_communication.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BT_STR_END_LEN   1

#define NO_PAIR "no pair"
#define NO_ADDR "no addr"
#define NUM_INVALID "num is invalid"

errcode_t ux_cmd_utils_commu_send_msg_to_device(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_utils_commu_register_msg_handler(ux_api_commu_cmd_t cmd, ux_api_commu_msg_handler handler);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
