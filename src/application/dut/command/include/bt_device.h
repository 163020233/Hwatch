/*******************************************************************************
 *
 *  Filename:      bt_device.h
 *
 *  Description:   BT Service Open API
 *
 *  Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 *
 ******************************************************************************/

#ifndef BT_SERVICE_INTERFACE
#define BT_SERVICE_INTERFACE

#include "autoconf.h"
#include "list.h"
#include "service_stack_adapter_base.h"
#include "ux_types.h"
#include "protocol.h"
#include "audio.h"
#include "bt_service.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRANS_MCU_TO_BT   ((TRANS_ID_BT  << 4) | TRANS_ID_MCU)
#define TRANS_BT_TO_MCU   ((TRANS_ID_MCU <<4) | TRANS_ID_BT)

#define SOF_NORMAL               (0x5A)
#define CTRL_FSN_NONE            (0x00)
#define CTRL_FSN_START           (0x01)
#define CTRL_FSN_MASK            (0x03)
#define UX_TO_BT_CMD_OPCODE      (0x81)
#define UX_TO_BT_MF_OPCODE       (0x82)
#define UX_TO_BT_SPP_OPCODE      (0x83)
#define BT_TO_UX_CMD_OPCODE      (0x84)
#define UX_TO_BT_NV_WRITE_OPCODE (0x85)
#define BT_TO_UX_SPP_OPCODE      (0x86)
#define BT_TO_UX_NV_WRITE_OPCODE (0x87)
#define BT_TO_UX_MF_OPCODE       (0x88)
#define BT_TO_UX_NV_READ_OPCODE  (0x89)
#define UX_TO_BT_NV_READ_OPCODE  (0x8A)
#define BT_TO_APP_POINT_OPCODE   (0x8B)
#define BT_TO_UX_MSG_OPCODE      (0x8400)
#define BT_REQ_MSG_OPCODE        (0x8900)

#ifdef CONFIG_MELODY_CHIPSET
#define BT_TID_SIZE              (1)
#define BT_LEN_OFFS              (2)
#else
#define BT_TID_SIZE              (0)
#define BT_LEN_OFFS              (1)
#endif


#define PRODUCT_ID_LEN           (4)
#define CRC_ACK_LENGTH           (7)
#define u16_msb(x)               (((x) >> 8) & 0xFF)
#define u16_lsb(x)               ((x) & 0xFF)
#define utils_uint16(x)          (((*(x)) << 8) | (*((x) + 1)))

/* TWS link role */
typedef enum {
    TWS_ROLE_MASTER,
    TWS_ROLE_SLAVE,
    TWS_ROLE_UNKNOWN,
    ROLE_FREEMAN
} service_twslink_role;

typedef enum {
    BT_DEVICE_IPC_DATA,
    BT_DEVICE_END,
} bt_device_event;

typedef struct {
    pkt_header_t hd;
    uint8_t data[];
} bt_ipc_data;

typedef struct {
    uint8_t  tran_id; // the higher four bits is DESTINATION ID; the lower four bits is SOURCE ID
    uint8_t  sof;    // start of frame
    uint16_t len;    // Length,  from control to opcode , exclude  CRC
    uint8_t  control_info;
    uint8_t  fsn;    // Frame Sequence Number, if include fsn in the control, it is must in the header
    uint16_t opcode;
} bt_common_data;

/**
 * BT service event callback function
 *
 * @param   msg - msg type. See SERVICE_TO_APP_MSG_T
 * @param   param - msg parameters.
 */
typedef void (*bt_service_event_callback)(service_to_ux_msg msg, void *param);

/* BT service spp event for MBB protocl callback function */
typedef struct {
    struct list_head cb_list;
    uint32_t business_type_mask;
    uint32_t service_id;
    bt_service_event_callback callback;
    uint32_t para;
} bt_service_event_spp_mbb_protocol_callback;

typedef struct {
    struct list_head cb_list;
    uint8_t business_type;
    bt_service_event_callback callback;
} bt_service_event_spp_tws_com_callback;

typedef struct {
    uint16_t cmd;
    uint16_t payloadlen;
    void *payload;
} device_ctl_d;

typedef enum {
    BT_MAX_POWER_TEST_START = 0,
    BT_MAX_POWER_TEST_STOP = 1,
    BT_MAX_POWER_TEST_QUERY = 2
} bt_max_power_test;

typedef int32_t(*BT_DEVICE_EVENT_HANDLE_FUNC)(void*);

typedef struct {
    uint32_t event_id;
    BT_DEVICE_EVENT_HANDLE_FUNC func;
} bt_device_event_handle_pair;

typedef struct {
    uint8_t len;
    uint8_t payload[0];
} bt_device_nv_rf_data;

typedef void (*BtSppFrameCallback)(const void *data, uint32_t len);
typedef void (*BtSppAckCallback)(const void *data, uint32_t len);

typedef service_to_ux_msg bt_event_id_t;

void RegisterSppFrameCallback(void *sppFrameCallback, void *sppAckCallback);
void BtSppReceiverDisconnect(uint32_t businessType);
void AdapterBtToUXMassData(uint8_t eventType, uint8_t value, uint8_t mainState);
void btdevice_equip_atcmd_set(void);
errcode_t work_mode_bt_test(uint32_t txen);
void *get_spp_header(void);
int32_t bt_device_construct_ctrl_data(uint16_t cmd, void *payload, uint16_t payloadlen);
errcode_t bt_device_handle_nosignal_mode_resp(void *para);
uint8_t bt_device_get_txmode_status(void);
void bt_device_set_txmode_status(uint8_t status);
int32_t BtDeviceMfSignalingStart(void);
errcode_t spp_send_data_to_opposite(const uint8_t *ack, uint16_t acklen);
#ifdef __cplusplus
}
#endif

#endif  /* BT_SERVICE_INTERFACE */
