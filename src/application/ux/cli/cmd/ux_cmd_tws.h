/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux cmd manager at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-05-02， Create file. \n
 */
#ifndef UX_CMD_TWS_H
#define UX_CMD_TWS_H
#include <stdint.h>
#include "errcode.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
errcode_t ux_cmd_tws_set_language(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_try_handover(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_earphone_wear_status(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_tws_get_language(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_wear_detection_switch(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_wear_detection_switch(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_low_latency_switch(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_low_latency_switch(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_dual_connection_switch(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_dual_connection_switch(uint8_t *data, uint16_t data_len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
