/*******************************************************************************
 *
 *  Filename:      bt_rf_test.h
 *
 *  Description:   BT RF TEST API
 *
 *  Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 *
 ******************************************************************************/

#ifndef BT_RF_TEST
#define BT_RF_TEST

#include "stdint.h"
#include "bt_service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BT_GRADE_0 = 0,
    BT_GRADE_1 = 1,
    BT_GRADE_2,
    BT_GRADE_3,
    BT_GRADE_4,
    BT_GRADE_5,
} bt_tx_power_grade;

typedef enum {
    BT_OFF,
    BT_NORMAL,
    BT_NO_SIGNAL_TEST_MODE,
    BT_SIGNAL_TEST_MODE,
} bt_work_mode;

typedef enum {
    MODU_PRBS9,
    MODU_AA,
    MODU_F0
} bt_test_modu_type_t;

typedef enum {
    BT_RF_UNKNOWN,
    BT_DM1,
    BT_DH1,
    BT_DM3,
    BT_DH3,
    BT_DM5,
    BT_DH5,
    BT_2DH1,
    BT_2DH3,
    BT_2DH5,
    BT_3DH1,
    BT_3DH3,
    BT_3DH5,
    BLE_1MPHY,
    BLE_2MPHY,
    BLE_8SPHY,
    BLE_2SPHY,
    BLE_4MQPHY,
    BLE_2MQPHY,
    BLE_2M8PPHY,
} bt_test_pkg_type;

typedef enum {
    BT_TX_QUERY = 0xFC0B,
    BT_TX = 0xFC51,
    BT_RX = 0xFC52,
    BT_RX_QUERY = 0xFC53,
    BLE_TX = 0x2034,
    BLE_RX = 0x2033,
    BLE_RX_QUERY = 0x201F,
    BT_RESET = 0x0C03,
    BT_CAL_TX_PWR = 0xFC61,
    BT_DEBUG = 0xFCB9,
} bt_test_op_code;

#define TXPWR_LEN 3
#define BT_MAX_POWER_MODE_QUERY_CMD  (0x9101)

typedef enum {
    APP_TO_BT_TEST_CMD_TYPE = 0x01,
    BT_TO_APP_TEST_CMD_TYPE = 0x04
} app_bt_test_cmd_type;

#pragma pack(1)
typedef struct {
    uint8_t msg_type;
    uint16_t op_code;
    uint8_t length;
    uint8_t bt_addr[BD_ADDR_SIZE];
    uint8_t hop_mode;
    uint8_t channel;
    uint8_t mod;            /* modu type */
    uint8_t local_channel;  /* mode */
    uint8_t pkg_type;
    uint16_t pkg_len;
    uint8_t txpwr[TXPWR_LEN];
} bt_test_tx_mode_cmd_t;

typedef struct {
    uint8_t msg_type;
    uint16_t op_code;
    uint8_t length;
    uint16_t cmd;
} bt_test_tx_mode_query_cmd_t;

typedef struct {
    uint8_t msg_type;
    uint16_t op_code;
    uint8_t length;
    uint8_t src_addr[BD_ADDR_SIZE];
    uint16_t period;
    uint8_t channel;
    uint8_t mod;
    uint8_t local_channel;
    uint8_t pkg_type;
    uint16_t pkg_len;
    uint16_t reserved;
} bt_test_rx_mode_cmd_t;

typedef struct {
    uint8_t msg_type;
    uint16_t op_code;
    uint8_t length;
    uint8_t channel;
    uint8_t ble_length;
    uint8_t mod;
    uint8_t ble_phy;
} bt_test_ble_tx_mode_cmd_s;

typedef struct {
    uint8_t msg_type;
    uint16_t op_code;
    uint8_t length;
    uint8_t channel;
    uint8_t ble_phy;
    uint8_t mod;
    uint8_t reserved;
} bt_test_ble_rx_mode_cmd_s;

typedef struct {
    uint8_t msg_type;
    uint16_t op_code;
    uint8_t length;
    uint32_t para1;
} bt_test_query_bt_rx_pkg_cmd_s;

typedef struct {
    uint8_t msg_type;
    uint16_t op_code;
    uint8_t length;
} bt_test_query_ble_rx_pkg_cmd_s;

typedef struct {
    uint8_t msg_type;
    uint16_t op_code;
    uint8_t length;
} bt_test_rst_mode_cmd_s;

typedef struct {
    uint8_t msg_type;
    uint16_t op_code;
    uint8_t length;
    uint8_t para_length;
    uint8_t payload[0];
} bt_test_debug_cmd_header_s;
#pragma pack()

typedef struct {
    uint8_t bt_work_mode;
    uint8_t bt_package_type;
    uint8_t bt_channel;
    uint8_t bt_frequency_shift;
    uint8_t bt_modu_type;
    uint16_t bt_tx_power;
} bt_test_para_t;

bt_test_para_t* get_bt_testpara(void);
void set_bt_test_work_mode(uint8_t work_mode);
uint8_t get_bt_test_work_mode(void);
/* ********************************************************************************************************** */
#ifdef __cplusplus
}
#endif

#endif  /* BT_RF_TEST */
