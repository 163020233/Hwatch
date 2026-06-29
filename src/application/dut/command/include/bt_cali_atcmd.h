/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: bt cali header file \n
 */
#ifndef BT_CALI_ATCMD_H
#define BT_CALI_ATCMD_H

#include "errcode.h"
#include "service_stack_adapter_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    BT_AUTO_CALI_SUCC,
    BT_AUTO_CALI_CONTINUE,
    BT_AUTO_CALI_FAILED
} bt_auto_cali_result_t;

typedef struct {
    int16_t last_freq;
    bool is_final_check;
    int8_t last_op;
} bt_auto_cali_check;

typedef enum {
    BT_AUTO_CALI_MODE_EFFICACIOUS = 0,
    BT_AUTO_CALI_MODE_ACCURACY = 1,
} bt_auto_cali_mode_t;

void bt_calibration_init(void);


#define BD_ADDR_LEN   6
typedef enum {
    BT_AUTO_CALI_CMD_CONN_DUT = 0x40, /* bt_dut_info */
    BT_AUTO_CALI_CMD_ENTER_OFFSET_TEST_MODE = 0x41, /* MLD enter or exit offset test mode */
} bt_auto_cali_cmd_t;

typedef enum {
    AUTO_CALI_DISCONNECT = 0,
    AUTO_CALI_CONNECT = 1,
    AUTO_CALI_DATA_REQUEST = 2,
} bt_auto_cali_op_t;

typedef struct {
    uint8_t addr[BD_ADDR_LEN];
    uint8_t action;
} bt_dut_info;

typedef struct {
    uint8_t role;
    uint8_t action;
} bt_offset_test_mode_info;


/* MLD SERVICE_TO_MF_FREQ_OFFSET_RPT */
typedef struct {
    uint8_t addr[BD_ADDR_LEN];
    uint16_t freq_offset;
    uint8_t conn_state; /* MLD 2:ready(for offset report) 1:connect 0:disconnect */
    uint8_t reason_code; /* disconnect reason: only make sense when conn_state = 0 */
} bt_dut_freq_offset_param;

errcode_t bt_cali_at_cmd_init(void);
void bt_calibration_init(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
