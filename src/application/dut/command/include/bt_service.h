/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          bt_service.h
 * Description:  BT Service Data Type Define
 */

#ifndef BT_SERVICE_H
#define BT_SERVICE_H

#include <stdbool.h>
#include "list.h"
#include "common_def.h"
#include "ux_types.h"
#if defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_BMIC_TX)
#include "key_struct.h"
#endif
#if defined(UX_BS_BT_SUPPORT)
#include "bs_api_stru.h"
#endif
#include "service_stack_adapter_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STRUCT_RESERVED_SIZE_32 32
#define STRUCT_RESERVED_SIZE_30 30
#define STRUCT_RESERVED_SIZE_8 8
#define STRUCT_RESERVED_SIZE_6 6
#define BD_ADDR_SIZE 6
#define BD_ADDR_LENGTH 12
#define EARBUD_LINK_0          0
#define EARBUD_LINK_15         15
#define STRUCT_RESERVED_SIZE_4 4
#define STRUCT_RESERVED_SIZE_3 3
#define STRUCT_RESERVED_SIZE_2 2
#define STRUCT_RESERVED_SIZE_1 1
#define MAX_BT_DEVICE_NAME_LENGTH 32
#define BT_TO_UX_HDAP_PARA_AMX_LEN (0xFF)
#define MODEL_ID_LEN (3)
#define LINK_KEY_LEN (16)
#define PD_NUM_MAX (10)
#define TXPWR_MAX_NUM (8)
#define TXPWR_CALI_COFF_NUM (3)
#define BT_RF_LNA_MAX (4)
#define NV_RXDC_BAND_NUM (10)
#define NV_POWER_MODE_NUM (9)
#define PD_NUM_MAX_HALF (5)
#define CLOUD_TOKEN_LEN (32)
#define IRK_HBKP_LEN (16)
#define DEVICE_SN_LEN (64)
#define LOCAL_NAME_LEN (32)
#define PRODUCT_ID_LEN (4)
#define BT_DEVICE_ACCOUNT_HASH_LEN 32
#define MAX_HAM_ATT_VALUE_LEN 512
#define MAX_HAM_SPP_VALUE_LEN 990
#define TWS_SNIFF_TYPE_NMU          5
#define BLE_ADV_DATA_LEN_MAX        (0xFB) /* ble广播TLV最大长度 */
#define BLE_LEN_DATA_OFFSET          0
#define BLE_ADV_SCAN_DATA_OFFSET     0
#define BLE_ADV_DATA_NAME_TYPE_VAL  (0x9)  /* ble广播name类型 */
#define BLE_ADV_DATA_UUID_TYPE_VAL  (0x2)  /* ble广播uuid类型 */
#define BLE_ADV_DATA_FLAG_TYPE_VAL  (0x1)  /* ble广播设备标识类型 */
#define BLE_ADV_DATA_FLAG_VAL  (0x6)       /* ble广播设备标识值 */
#define DEV_BT_NAME_MAX_LEN         40     /* bt name长度为40，受AT输入(包含AT^...)最长56个字节限制 */
#define PEER_DISCOVER_NAME_MAX_LEN  17     /* 对耳扫描的name的长度 */
#define BLE_ADV_DATA_TYPE_LEN       1      /* ble广播扩展type占字节 */
#define BLE_ADV_EXPAND_END_VAL_LEN  1      /* ble广播扩展end val LEN */
#define BLE_ADV_DATA_OFFEST         2      /* ble广播扩展type+len占字节 */
#define BLE_ADV_EXPAND_END_VAL      88     /* ble广播扩展结束符 */

#define BT_DEVICE_LIST_MAX_SIZE 16
#define SERVICE_PROFILE_MAX_SIZE 8
#define MAX_REGISTERED_APP_COUNT 5

#define TRANS_MCU_TO_BT ((TRANS_ID_BT << 4) | TRANS_ID_MCU)

#define UX_TO_BT_CMD_BEGIN_OFFSET 0
#define UX_TO_MF_BEGIN_OFFSET 0x3c
#define UX_TO_MT_BEGIN_OFFSET 0x50
#define UX_TO_UP_BEGIN_OFFSET 0x64
#define UX_TO_SPP_BEGIN_OFFSET 0x78
#define UX_TO_NV_WRITE_BEGIN_OFFSET 0x8c
#define UX_TO_NV_READ_BEGIN_OFFSET 0xa5

#define BT_TO_UX_CMD_BEGIN_OFFSET 0
#define BT_TO_UX_NV_WRITE_BEGIN_OFFSET 0x32
#define BT_TO_UX_NV_READ_BEGIN_OFFSET 0x50
#define MF_TO_UX_BEGIN_OFFSET 0x6e
#define SPP_TO_UX_BEGIN_OFFSET 0x82
#define OTHER_TO_UX_BEGIN_OFFSET 0x96


#define BT_CRC_SIZE 2
#define U16_MSB(x) (((x) >> 8) & 0xFF)
#define U16_LSB(x) ((x)&0xFF)
#define UTILS_UINT16(x) (((*(x)) << 8) | (*((x) + 1)))

#define AVRCP_PROFILE (1 << 2)
#define HSP_PROFILE (1 << 3)
#define SPP_PROFILE (1 << 4)
#define MAX_PDL_NUM 10
#define HALF_OF_MAX_PDL_NUM 5
#define MAX_MASTER_PLUS_SLAVE_PDL_NUM 20
#define NV_DEVICE_CLASS_LEN 2
#define NV_PROFILE_LEN 2
#if !defined(UX_BS_BT_SUPPORT)
#define NV_LINK_KEY_LEN 16
#endif
#define NV_CLOUD_TOKEN_LEN 32
#define NV_DEVICE_SN_LEN 64
#define NV_LOCAL_NAME_LEN 32

#define MODEL_ID_INDEX_ZERO 0
#define MODEL_ID_INDEX_ONE 1
#define MODEL_ID_INDEX_TWO 2

#define PRODUCT_ID_INDEX_ZERO 0
#define PRODUCT_ID_INDEX_ONE 1
#define PRODUCT_ID_INDEX_TWO 2
#define PRODUCT_ID_INDEX_THREE 3
#define HEADSET_VERSION_LEN 32
#define CRC_PASS 0
#define CRC_FAIL 1
#if !defined(UX_BS_BT_SUPPORT)
#define RESERVED_LEN_ONE 1
#define RESERVED_LEN_TWO 2
#define RESERVED_LEN_THR 3
#define RESERVED_LEN_FOUR 4
#define RESERVED_LEN_SIX 6
#define RESERVED_LEN_EIGHT 8
#endif
#define IS_DISCONNECT_FROM_MOBILE_BIT 2
#define IS_CANCEL_PAIRING_FROM_MOBILE_BIT 1
#define IS_CARRY_DEVICE_BIT 4
#define IS_HUAWEI_DEVICE_BIT 1
#define IS_ANDROID_DEVICE_BIT 2
#define IS_IOS_DEVICE_BIT 4
#define IS_WINDOWS_DEVICE_BIT 8
#define BTC_NV_RF_CALI_TX_PWR_PREF_LEN 8
#define BTC_NV_RF_CALI_TX_PWR_FREQ_LEN 8
#define BTC_NV_RF_TX_POWER_COFF_LEN 3
#define BTC_NV_RF_CALI_HARMONICS_PREF_LEN 3
#define BTC_NV_RF_CALI_HARMONICS_FREQ_LEN 3
#define BT_TO_UX_HDAP_PARA_MAX_LEN 0xFF
/* UX_TO_BT_COMMON_SWITCH_TLV_HEADER_LEN: BtAppSwitchInfo TLV header length. 4 = type(2) + length(2) */
#define UX_TO_BT_COMMON_SWITCH_TLV_HEADER_LEN 4
#if !defined(UX_BS_BT_SUPPORT)
#define BT_REMOTE_NAME_MAX_LEN 32
#define BT_DEVICE_HW_ACCOUNT_HASH_LEN 32
#endif
#define MAX_HAM_ATT_VALUE_LEN 512
#define DEVICE_NAME_LEN             (64)
#ifndef LIST_H
#define LIST_H
struct list_head {
    struct list_head *next, *prev;
};
#endif

typedef enum {
    POSITION_KNOWN,
    POSITION_INBOX,
    POSITION_OUTBOX_OUT_EAR,
    POSITION_OUTBOX_IN_EAR,
} ux_position_to_bt;

typedef enum {
    THREE_WAY_EVENT_HOLD_ANSWER_CALL,
    THREE_WAY_EVENT_HOLD_REJECT_CALL,
    THREE_WAY_EVENT_SWITCH_CALL,
    THREE_WAY_EVENT_HANG_UP_CALL,
    THREE_WAY_EVENT_END,
} three_way_calling_event_type;

/* Message type from app */
typedef enum {
    /* UX to BT CMD */
    SERVICE_UX_TO_CMD_BEGIN = 0,                      // (OpCode:0x8100~0x813b) cnt:60
    SERVICE_UX_TO_CMD_INIT = SERVICE_UX_TO_CMD_BEGIN, /* service_init_settings */
    SERVICE_UX_TO_CMD_CLEAN_UP = 0x01,                       /* no param */
    SERVICE_UX_TO_CMD_OPEN_BOX = 0x02,                       /* service_to_bt_open_box_data */
    SERVICE_UX_TO_CMD_CLOSE_BOX = 0x03,                      /* service_to_bt_close_box_data */
    SERVICE_UX_TO_CMD_IN_BOX = 0x04,                         /* no param */
    SERVICE_UX_TO_CMD_OUT_BOX = 0x05,                        /* no param */
    SERVICE_UX_TO_CMD_WEAR_IN = 0x06,                        /* no param */
    SERVICE_UX_TO_CMD_WEAR_OUT = 0x07,                       /* no param */
    SERVICE_UX_TO_CMD_PAIRING_MODE = 0x08,                   /* service_to_bt_open_box_data */
    SERVICE_UX_TO_CMD_RE_CONN = 0x09,                        /* service_to_bt_re_conn */
    SERVICE_UX_TO_CMD_HFP_VOLUME = 0x0A,                     /* service_to_bt_hfp_volume */
    SERVICE_UX_TO_CMD_AVRCP_VOLUME = 0x0B,                   /* service_to_bt_avrcp_volume */
    SERVICE_UX_TO_CMD_SELF_BATTERY_LEVEL = 0x0C,             /* service_tws_ear_battery_info */
    SERVICE_UX_TO_CMD_BOX_BATTERY_LEVEL = 0x0D,              /* service_box_battery_info */
    SERVICE_UX_TO_CMD_PLAY_START = 0x0E,                     /* no param */
    SERVICE_UX_TO_CMD_PLAY_STOP = 0x0F,                      /* no param */
    SERVICE_UX_TO_CMD_PLAY_PAUSE = 0x10,                     /* no param */
    SERVICE_UX_TO_CMD_PLAY_NEXT = 0x11,                      /* no param */
    SERVICE_UX_TO_CMD_PLAY_PREVIOUS = 0x12,                  /* no param */
    SERVICE_UX_TO_CMD_ANSER_CALL = 0x13,                     /* no param */
    SERVICE_UX_TO_CMD_HANGUP_CALL = 0x14,                    /* no param */
    SERVICE_UX_TO_CMD_REJECT_CALL = 0x15,                    /* no param */
    SERVICE_UX_TO_CMD_OPEN_VOICE_ASSIISTANT = 0x16,          /* no param */
    SERVICE_UX_TO_CMD_CLOSE_VOICE_ASSISTANT = 0x17,          /* no param */
    SERVICE_UX_TO_CMD_SET_BTH_CONFIG = 0x18,                 /* ham_set_bth_cfg_ind_t */
    SERVICE_UX_TO_CMD_ACK_AUDIO_A2DP_STATE = 0x19,           /* service_ack_audio_a2dp */
    SERVICE_UX_TO_CMD_ACK_AUDIO_SCO_STATE = 0x1A,            /* service_ack_audio_sco */
    SERVICE_UX_TO_CMD_REGISTER_CALLBACK = 0x1B,
    SERVICE_UX_TO_CMD_LIMIT_HANDOVER = 0x1C,                 // unuse
    SERVICE_UX_TO_CMD_IMPROVE_DATA_TRANSFER_PRIORITY = 0x1D, /* bt_data_transfer_priority */
    SERVICE_UX_TO_CMD_KNOCK = 0x1E,                          /* ux_position_to_bt */
    SERVICE_UX_TO_CMD_DISCONNECT_BT_REQ = 0x1F,
    SERVICE_UX_TO_CMD_DISCONNECT_AUDIO_PROFILE_REQ = 0x20, /* ota:bt_hota_moible_addr */
    SERVICE_UX_TO_CMD_LOW_BATTERY_REQ = 0x21,
    SERVICE_UX_TO_CMD_NOTIFY_TONE_REQ = 0x22, /* bt_sync_tone_type */
    SERVICE_UX_TO_CMD_MAINTENANCE_TEST_HANDOVER = 0x23,
    SERVICE_UX_TO_CMD_MAINTENANCE_TEST_CHECK_RSSI = 0x24,
    SERVICE_UX_TO_CMD_ACK_PLAY_DEVICE_FIND_STATE = 0x25,
    SERVICE_UX_TO_CMD_START_HAND_HELD = 0x26,
    SERVICE_UX_TO_CMD_STOP_HAND_HELD = 0x27,
    SERVICE_UX_TO_CMD_SWITCH_CHANGE = 0x28,
    SERVICE_UX_TO_CMD_MAINTENANCE_TEST_CHECK_DEVICE_RSSI = 0x29,
    SERVICE_UX_TO_CMD_POWER_OFF = 0x2A,          // disconnect phone, keep tws
    SERVICE_UX_TO_CMD_START_BLE_ADV = 0x2B,      // new add, not use now
    SERVICE_UX_TO_CMD_REMAINDER_PDL_INFO = 0x2C, /* bt_remainder_pdl_info */
    SERVICE_UX_TO_CMD_OPEN_BOX_SWITCHES_INFO = 0x2D,
    SERVICE_UX_TO_CMD_CONN_TWS = 0x2E, /* no param ux notify conn pear */
    SERVICE_UX_TO_CMD_TX_POWER_REDUCTION = 0x2F,     /* ServiceTxPowerReductionSettings */
    SERVICE_UX_TO_CMD_DATA_LOCK_MODE = 0x30,
    SERVICE_UX_TO_CMD_COMMON_END = SERVICE_UX_TO_CMD_DATA_LOCK_MODE,

    SERVICE_UX_TO_CMD_QUERY_LOW_LATENCY_INFO = 0x31,
    SERVICE_UX_TO_CMD_AT_CMD = 0x32,
    SERVICE_UX_TO_CMD_DISC_ALL_BLE = 0x33, /* notify bth to disconnect all ble */
    SERVICE_UX_TO_CMD_PREPARE_POWER_OFF_MODE1_REQ = 0x34, /* prepare earphone power off */

    SERVICE_UX_TO_CMD_SET_FACTORY_TEST_MODE = 0x35, /* enable spp service in factory test mode */
    SERVICE_UX_TO_CMD_SPP_DATA_SEND = 0x36,    /* SPP DATA Send Channel */
    SERVICE_UX_TO_CMD_TWS_DISCOVER_INFO = 0x37, /* bt_tws_discover_info */
    SERVICE_UX_TO_CMD_GATT_DATA_SEND = 0x38,   /* GATT DATA Send Channel */
    SERVICE_MEDIA_TO_CMD_ADP_MAP_REQ = 0x39,   /* media */
    SERVICE_MEDIA_TO_CMD_ADP_UNMAP_REQ = 0x3A, /* media */
    SERVICE_UX_TO_CMD_SET_ADV_DATA = 0x3B,  /* ble_adv_data_t */
	SERVICE_UX_TO_CMD_END = SERVICE_UX_TO_CMD_SET_ADV_DATA,

    /* MF CMD */
    SERVICE_MF_TO_CMD_BEGIN = UX_TO_MF_BEGIN_OFFSET,  // 0x3c, (OpCode:0x8200~0x8213) cnt:20
    SERVICE_MF_TO_CMD_SIGNAL_MODE = SERVICE_MF_TO_CMD_BEGIN, /* service_signal_mode */
    SERVICE_MF_TO_CMD_NO_SIGNAL_MODE = 0x3D,                        /* service_nosignal_mode */
    SERVICE_MF_TO_CMD_NO_SIGNAL_MODE_CMD_DATA = 0x3E,               /* service_bt_device_nosignal_mode_cmd */
    // SERVICE_MF_TO_CMD_REGISTER_CALLBACK,
    SERVICE_MF_TO_CMD_REGISTER_CALLBACK = 0x3F,
    SERVICE_MF_TO_CMD_CONN_DUT = 0x40,    /* MLD connect remote device(for dut test) */
    SERVICE_MF_TO_CMD_ENTER_OFFSET_TEST_MODE = 0x41,    /* MLD enter or exit offset test mode */
    SERVICE_MF_TO_CMD_END = SERVICE_MF_TO_CMD_ENTER_OFFSET_TEST_MODE,
    /* *MT CMD */
    SERVICE_MT_TO_CMD_BEGIN = UX_TO_MT_BEGIN_OFFSET,  // 0x50,(OpCode:reserved) cnt:20
    SERVICE_MT_TO_CMD_SEND = SERVICE_MT_TO_CMD_BEGIN,  // unuse
    SERVICE_MT_TO_CMD_REGISTER_CALLBACK = 0x51,               // unuse
    SERVICE_MT_TO_CMD_END = SERVICE_MT_TO_CMD_REGISTER_CALLBACK,
    /* *UP CMD */
    SERVICE_UP_TO_CMD_BEGIN = UX_TO_UP_BEGIN_OFFSET,  // 0x64,(OpCode:reserved) cnt:20
    SERVICE_UP_TO_CMD_SEND = SERVICE_UP_TO_CMD_BEGIN,  // unuse
    SERVICE_UP_TO_CMD_REGISTER_CALLBACK = 0x65,               // unuse
    SERVICE_UP_TO_CMD_END = SERVICE_UP_TO_CMD_REGISTER_CALLBACK,
    /* *SPP CMD */
    SERVICE_SPP_CMD_BEGIN = UX_TO_SPP_BEGIN_OFFSET,  // 0x78,(OpCode:0x8300~0x8313) cnt:20
    SERVICE_SPP_SEND = SERVICE_SPP_CMD_BEGIN, /* data_transport_data */
    SERVICE_SPP_SYNC_SEND = 0x79,                    /* data_transport_data_with_retval */
    SERVICE_SPP_REGISTER_MBB_PROTOCOL_CALLBACK = 0x7A,
    SERVICE_SPP_REGISTER_VOICE_ID_CALLBACK = 0x7B,
    SERVICE_SPP_REGISTER_TWS_COM_CALLBACK = 0x7C,
    SERVICE_SPP_CMD_END = SERVICE_SPP_REGISTER_TWS_COM_CALLBACK,
    /* NV WRITE CMD */
    MSG_BT_NV_WRITE_RSP_BEGIN = UX_TO_NV_WRITE_BEGIN_OFFSET,  // 0x8c,(OpCode:0x8500~0x8518) cnt:25
    MSG_BT_NV_WRITE_EARBUD_PDL_RSP = MSG_BT_NV_WRITE_RSP_BEGIN,
    MSG_BT_NV_WRITE_MOBILE_PD_INFO_RSP = 0x8D,   // single pd
    MSG_BT_NV_WRITE_MOBILE_PDL_INFO_RSP = 0x8E,  // multi pd
    MSG_BT_NV_WRITE_MOBILE_PDL_CONN_ORDER_RSP = 0x8F,
    MSG_BT_NV_WRITE_CLOUD_TOKEN_RSP = 0x90,
    MSG_BT_NV_WRITE_LOG_LEVEL_RSP = 0x91,
    MSG_BT_NV_WRITE_ADV_KEY_RSP = 0x92,
    MSG_BT_NV_WRITE_RSP_END = MSG_BT_NV_WRITE_ADV_KEY_RSP,  // these nv write  cmd unuse

    /* *NV READ CMD */
    MSG_BT_NV_READ_RSP_BEGIN = UX_TO_NV_READ_BEGIN_OFFSET,  // 0xa5,(OpCode:0x8a00~0x8a18) cnt:25
    MSG_BT_NV_READ_DEVICE_ADDR_RSP = 0xA6,           /* bt_nv_device_address */
    MSG_BT_NV_READ_EARBUD_NATURE_RSP = 0xA7,         /* bt_nv_earbud_nature_info */
    MSG_BT_NV_READ_DEVICE_NAME_RSP = 0xA8,           // unuse
    MSG_BT_NV_READ_EARBUD_PDL_RSP = 0xA9,            /* bt_nv_earbud_pdl */
    MSG_BT_NV_READ_MOBILE_PDL_INFO_RSP = 0xAA,       /* bt_nv_mobile_pdl */
    MSG_BT_NV_READ_MOBILE_PDL_CONN_ORDER_RSP = 0xAB, /* bt_nv_mobile_pdl_order */
    MSG_BT_NV_READ_PEER_ADDR_RSP = 0xAC,             /* bt_nv_peer_addr_info */
    MSG_BT_NV_READ_CLOUD_TOKEN_RSP = 0xAD,           /* bt_nv_cloud_token */
    MSG_BT_NV_READ_LOG_LEVEL_RSP = 0xAE,             /* bt_nv_log_level */
    MSG_BT_NV_READ_ALL_DATA_RSP = 0xAF,              /* bt_nv_init_all_data */
    MSG_BT_NV_REQ_WRITE_CALLBACK = 0xB0,
    MSG_BT_NV_READ_NEARBY_RSSI_RSP = 0xB1, /* bt_nv_nearby_rssi */
    MSG_BT_NV_READ_MODEL_ID_RSP = 0xB2,    /* bt_model_id */
    MSG_BT_NV_READ_DEVICE_SN_RSP = 0xB3,   /* bt_device_sn */
    MSG_BT_NV_REQ_READ_CALLBACK = 0xB4,
    MSG_BT_NV_READ_BTC_RF_DATA_RSP = 0xB5, /* bt_calibration_param */
    MSG_BT_NV_READ_SEC_ADV_KEY_RSP = 0xB6,     /* bt_nv_sec_adv_key_data_s */
    MSG_BT_NV_READ_RSP_END = MSG_BT_NV_READ_SEC_ADV_KEY_RSP,

    SERVICE_OHTER_TO_BT_MSG_BEGIN = 0xbe,
#ifdef TWS_POLICY_ENABLE
    SERVICE_FOTA_TO_BT_ENABLE_TWS,
#endif
    SERVICE_TO_BT_SNIFF_FLAG_SET,
} service_to_bt_msg;

/* Message type from BT core to app */
typedef enum {
    /* BT To UX */
    SERVICE_TO_UX_BEGIN = 0,                               // (OpCode:0x8400~0x8431) cnt:50
    SERVICE_TO_UX_BT_MAIN_STATE_MSG = SERVICE_TO_UX_BEGIN, /* service_to_ux_bt_main_state_msg_param */
    SERVICE_TO_UX_PROFILE_STATE_MSG = 0x01,                       /* service_to_ux_bt_profile_state_msg_param */
    SERVICE_TO_UX_A2DP_STATE_MSG = 0x02,                          /* service_to_ux_a2dp_state_msg_param */
    SERVICE_TO_UX_SCO_STATE_MSG = 0x03,                           /* service_to_ux_sco_state_msg_param */
    SERVICE_TO_UX_MOBILE_CONNECT_STATE_MSG = 0x04,                /* service_to_ux_mobile_connect_state_msg_param */
    SERVICE_TO_UX_MOBILE_LINK_MODE_MSG = 0x05,                    /* service_to_ux_mobile_link_mode_msg_param */
    SERVICE_TO_UX_TWS_CONNECT_STATE_MSG = 0x06,                   /* service_to_ux_tws_connect_state_msg_param */
    SERVICE_TO_UX_MOBILE_BONDED_STATE_MSG = 0x07,          /* SERVICE_TO_UX_MOBILE_PAIRING_STATE_MSG_Param_S */
    SERVICE_TO_UX_VOICE_ASSISTANT_STATE_MSG = 0x08,               /* service_to_ux_voice_assisant_state_msg_param */
    SERVICE_TO_UX_HFP_VOLUME_UPDATE_MSG = 0x09,                   /* service_to_ux_hfp_volume_updata_msg_param */
    SERVICE_TO_UX_AVRCP_VOLUME_UPDATE_MSG = 0x0A,                 /* service_to_ux_avrcp_volume_updata_msg_param */
    SERVICE_TO_UX_HANDOVER_STATUS_MSG = 0x0B,                     /* service_to_ux_handover_status_msg_param */
    SERVICE_TO_UX_A2DP_CODEC_SETTING_MSG = 0x0C,                  /* service_to_ux_a2dp_code_setting_msg_param */
    SERVICE_TO_UX_SCO_CODEC_SETTING_MSG = 0x0D,                   /* service_to_ux_sco_code_setting_msg_param */
    SERVICE_TO_UX_MIC_STATE_MSG = 0x0E,                           /* service_to_ux_mic_status_msg_param */
    SERVICE_TO_UX_PLAY_OUTBAND_RING_MSG = 0x0F,                   /* service_outband_ring_param */
    SERVICE_TO_UX_WORK_MODE_MSG = 0x10,                           /* service_to_ux_tws_mode_setting_msg_param */
    SERVICE_TO_UX_TWS_ROLE_MSG = 0x11,                            /* service_to_ux_tws_role_param */
    SERVICE_TO_UX_HIGH_PERF_MODE_TYPE = 0x12,                     /* bt_high_perf_mode */
    SERVICE_TO_UX_CMD_DISCONNECT_BT_RSP = 0x13,
    SERVICE_TO_UX_CMD_DISCONNECT_AUDIO_PROFILE_RSP = 0x14,
    SERVICE_TO_UX_BT_INIT_RSP = 0x15,
    SERVICE_TO_UX_LOW_BATTERY_RSP = 0x16,                         /* no param */
    SERVICE_TO_UX_WARN_TONE_RSP = 0x17,                           /* bt_sync_tone_info */
    SERVICE_TO_UX_MAINTENANCE_TEST_HANDOVER_RSP = 0x18,           /* bt_maintenance_test_handover_res */
    SERVICE_TO_UX_MAINTENANCE_TEST_CHECK_RSSI_RSP = 0x19,         /* bt_maintenance_test_rssi */
    SERVICE_TO_UX_NOTIFY_PEER_POSITION = 0x1A,
    SERVICE_TO_UX_PAIRING_MODE_STATE = 0x1B,                      /* bt_pairing_state */
    SERVICE_TO_UX_HDAP_CODEC_SETTING_MSG = 0x1C,             /* service_to_ux_hdap_codec_setting_msg_param_s */
    SERVICE_TO_UX_HDAP_START = 0x1D,                              /* service_to_ux_hdap_start_param_s */
    SERVICE_TO_UX_HDAP_STOP = 0x1E,
    SERVICE_TO_UX_HDAP_KARAOKE_PARA = 0x1F,
    SERVICE_TO_UX_DEVICE_NAME = 0x20,
    SERVICE_TO_UX_BT_INIT = 0x21,
    SERVICE_TO_UX_PLAY_DEVICE_FIND = 0x22,
    SERVICE_TO_UX_ACK_START_HAND_HELD = 0x23,
    SERVICE_TO_UX_MAINTENANCE_TEST_CHECK_DEVICE_RSSI_RSP = 0x24,
    SERVICE_TO_UX_POWER_OFF_RSP = 0x25,
    SERVICE_TO_UX_COMMON_END = SERVICE_TO_UX_POWER_OFF_RSP,

    SERVICE_TO_UX_DEVICE_TYPE = 0x26,
    SERVICE_TO_UX_MUSIC_INFO = 0x27,
    SERVICE_TO_UX_LOW_LATENCY_INFO_RSP = 0x28,
    SERVICE_TO_UX_ANT_CFG = 0x29,

    SERVICE_TO_HAM_ADP_DATA_IND = 0x2A,  /* BLE Hearing Aid Service data ind */
    SERVICE_TO_UX_BLE_CONN_STATE = 0x2B,  /* BLE Connect state to UX */
    SERVICE_TO_UX_CMD_PEER_ADDR = 0x2C,  /* send peer addr to UX */
    SERVICE_TO_HAM_SPP_DATA_IND = 0x2D,
    SERVICE_TO_UX_SET_FACTORY_TEST_MODE_RSP = 0x2E, /* enable spp service response in factory test mode */
    SERVICE_TO_UX_CMD_CLOSE_BOX_RSP = 0x2F,
    SERVICE_TO_MEDIA_ADP_MAP_RSP = 0x30,   /* media */
    SERVICE_TO_MEDIA_ADP_UNMAP_RSP = 0x31, /* media */
    SERVICE_TO_UX_BES_END = SERVICE_TO_MEDIA_ADP_UNMAP_RSP,
    SERVICE_TO_UX_END = SERVICE_TO_UX_BES_END,

    MSG_BT_NV_WRITE_BEGIN = BT_TO_UX_NV_WRITE_BEGIN_OFFSET,  // 0x32,(OpCode:0x8700~0x871d) cnt:30
    MSG_BT_NV_WRITE_EARBUD_PDL_REQ = MSG_BT_NV_WRITE_BEGIN,
    MSG_BT_NV_WRITE_MOBILE_SINGLE_PDL_INFO_REQ = 0x33,
    MSG_BT_NV_WRITE_MOBILE_MULTIPLE_PDL_INFO_REQ = 0x34,
    MSG_BT_NV_WRITE_MOBILE_PDL_CONN_ORDER_REQ = 0x35,
    MSG_BT_NV_WRITE_CLOUD_TOKEN_REQ = 0x36,
    MSG_BT_NV_WRITE_LOG_LEVEL_REQ = 0x37,
    MSG_BT_NV_WRITE_MOBILE_REMAINDER_PDL_REQ = 0x38,
    MSG_BT_NV_WRITE_SECU_ADV_KEY_REQ = 0x39,
    SERVICE_TO_UX_CMD_SET_BTH_CFG_REQ = 0x3A,
    SERVICE_TO_UX_WORK_STATE_MSG = 0x3B,
    SERVICE_TO_UX_BLE_STACK_STATE_MSG = 0x3C,
    SERVICE_TO_UX_BLE_HANDOVER_STATE_MSG = 0x3D,
    SERVICE_TO_UX_ACTIVE_DEVICE_MSG = 0x3E,
    SERVICE_TO_UX_CMD_OPEN_BOX_RSP = 0x3F,
    MSG_BT_NV_WRITE_END = SERVICE_TO_UX_CMD_OPEN_BOX_RSP,
    SERVICE_TO_UX_TWS_VOLUME_SYNC,
    SERVICE_TO_UX_HAID_INFO_SYNC,

    /* BT READ REQ */
    MSG_BT_NV_DATA_READ_BEGIN = BT_TO_UX_NV_READ_BEGIN_OFFSET,  // 0x50,(OpCode:0x8900~0x891d) cnt:30
    MSG_BT_NV_READ_DEVICE_ADDR_REQ = MSG_BT_NV_DATA_READ_BEGIN,
    MSG_BT_NV_READ_EARBUD_NATURE_REQ = 0x51,
    MSG_BT_NV_READ_DEVICE_NAME_REQ = 0x52,
    MSG_BT_NV_READ_EARBUD_PDL_REQ = 0x53,
    MSG_BT_NV_READ_MOBILE_PDL_INFO_REQ = 0x54,
    MSG_BT_NV_READ_MOBILE_PDL_CONN_ORDER_REQ = 0x55,
    MSG_BT_NV_READ_PEER_ADDR_REQ = 0x56,
    MSG_BT_NV_READ_CLOUD_TOKEN_REQ = 0x57,
    MSG_BT_NV_READ_LOG_LEVEL_REQ = 0x58,
    MSG_BT_NV_READ_ALL_DATA_REQ = 0x59,
    MSG_BT_NV_READ_NEARBY_RSSI_REQ = 0x5A,
    MSG_BT_NV_READ_MODEL_ID_REQ = 0x5B,
    MSG_BT_NV_READ_DEVICE_SN_REQ = 0x5C,
    MSG_BT_NV_READ_BTC_RF_DATA_REQ = 0x5D,
    MSG_BT_NV_READ_SECU_ADV_KEY_REQ = 0x5E,
    MSG_BT_NV_DATA_READ_END = MSG_BT_NV_READ_SECU_ADV_KEY_REQ,

    /* BT to MF */
    SERVICE_TO_MF_BEGIN = MF_TO_UX_BEGIN_OFFSET,  // 0x6e,(OpCode:0x8800~0x8813) cnt:20
    SERVICE_TO_MF_NO_SIGNAL_MODE_MSG = SERVICE_TO_MF_BEGIN,
    SERVICE_TO_MF_FREQ_OFFSET_RPT = 0x6F,    /* MLD report freq offset to app */
    SERVICE_TO_MF_END = SERVICE_TO_MF_FREQ_OFFSET_RPT,

    /* BT SPP */
    SERVICE_RECV_SPP_BEGIN = SPP_TO_UX_BEGIN_OFFSET,  // 0x82(OpCode:0x8600~0x8613) cnt:20
    SERVICE_RECV_SPP_REMOTE_MSG = SERVICE_RECV_SPP_BEGIN,
    SERVICE_RECV_SPP_PORT_STATE_CHANGE_MSG = 0x83,
    SERVICE_RECV_SPP_SEND_BUFFER_STATE_MSG = 0x84,
    SERVICE_RECV_SPP_ALL_BUFFER_STATE_MSG = 0x85,
    SERVICE_RECV_SPP_COMMON_END = SERVICE_RECV_SPP_ALL_BUFFER_STATE_MSG,

    SERVICE_RECV_SPP_ACK_MSG,
    SERVICE_RECV_CCMP_RESP_MSG,
    SERVICE_RECV_SPP_BES_END = SERVICE_RECV_CCMP_RESP_MSG,

    SERVICE_RECV_SPP_END = max(SERVICE_RECV_SPP_COMMON_END, SERVICE_RECV_SPP_BES_END),

    SERVICE_OHTER_TO_UX_MSG_BEGIN = OTHER_TO_UX_BEGIN_OFFSET,  // 0x96
	SERVICE_TO_APP_POINT_MSG = SERVICE_OHTER_TO_UX_MSG_BEGIN,
	SERVICE_OHTER_TO_UX_MSG_END = SERVICE_TO_APP_POINT_MSG,
} service_to_ux_msg;

/* SERVICE_UX_TO_CMD_TWS_DISCOVER_INFO */
typedef struct {
    uint8_t inquiry_device;                     /* 置1为查询方，置0为被查询方 */
    uint8_t device_class;                       /* 设备类型 */
    uint8_t self_device_name[PEER_DISCOVER_NAME_MAX_LEN];  /* 与要配对的对端耳机的peer_device_name一致 */
    uint8_t peer_device_name[PEER_DISCOVER_NAME_MAX_LEN];  /* 与要配对的对端耳机的self_device_name一致 */
    uint8_t reserved;
} bt_tws_discover_info;

/* SERVICE_TO_UX_CMD_PEER_ADDR */
typedef struct {
    bd_addr peer_addr;  /* 对耳地址 */
    uint8_t reserved;
} bt_tws_discover_result;

/* SERVICE_UX_TO_CMD_INIT */
typedef enum {
    TWS_MODE_JITWS,
    TWS_MODE_MITWS
} bt_tws_mode_capbility;

typedef enum {
    BT_WORK_MODE_UNKNOWN,
    BT_WORK_MODE_SOLO,
    BT_WORK_MODE_TWS,
    BT_WORK_MODE_FACTORY
} service_bt_init_work_mode;

typedef struct {
    uint8_t workmode;         /* service_bt_init_work_mode */
    uint8_t tws_mode_capbility; /* bt_tws_mode_capbility */
    bd_addr local_addr;
} service_init_settings;

#if !defined(UX_BS_BT_SUPPORT)
/* SERVICE_UX_TO_CMD_OPEN_BOX */
typedef struct {
    uint16_t is_remote_user_cancel_pairing : 1;
    uint16_t is_remote_user_terminate_connection : 1;
    uint16_t is_first_conn_earbuds : 1;
    uint16_t is_icarry_device : 1;
    uint16_t reserved : 12;
} bt_conn_property_s;

typedef struct {
    uint16_t back_conn_permit : 1;
    uint16_t priority_device : 1;
    uint16_t resevre : 14;
} bt_device_manage_switch;
#endif

typedef enum {
    BT_DEVICE_TYPE_UNKONWN,
    BT_DEVICE_TYPE_HUEWEI,
    BT_DEVICE_TYPE_ANDROID,
    BT_DEVICE_TYPE_IOS,
    BT_DEVICE_TYPE_WINDOWS,
    BT_DEVICE_TYPE_OTHER,
} bt_device_type_m;

typedef enum {
    BT_DEVICE_CLASS_MOBILE,
    BT_DEVICE_CLASS_WATCH,
    BT_DEVICE_CLASS_TWS,
    BT_DEVICE_CLASS_HEADPHONES,
    BT_DEVICE_CLASS_NECKPHONE,
    BT_DEVICE_CLASS_SPEAKER,
    BT_DEVICE_CLASS_PAD,
    BT_DEVICE_CLASS_TV,
    BT_DEVICE_CLASS_CAR,
    BT_DEVICE_CLASS_GLASSES,
} bt_device_class_m;

#if !defined(UX_BS_BT_SUPPORT)
typedef enum {
    AVRCP,
    A2DP_SOURCE,
    A2DP_SINK,
    HFP,
    SPP_SERVER,
    SPP_CLIENT,
    GATT_SERVER,
    GATT_CLIENT,
    BES_TRANSFER
} service_profile;
#endif

typedef struct {
    bd_addr phone_addr;
    uint8_t voice_volume; /* 0-15 */
    uint8_t media_volume; /* 0-127 */
    char device_name[MAX_BT_DEVICE_NAME_LENGTH];
    uint8_t link_key[LINK_KEY_LEN];
    uint8_t key_type;
    uint8_t device_class; /* bt_device_class_m */
    bt_device_manage_switch device_manage_switch;
    bt_conn_property_s conn_property; /* bt_conn_property_s */
    uint16_t bt_audio_conn_property; /* reserved */
    uint8_t profile;              /* service_profile */
    uint8_t device_type;           /* bt_device_type_m */
    uint8_t ver;
    uint8_t sec_level;
    uint16_t subver;
    uint16_t manufacture_name;
    uint8_t device_class_major;                /* major cod, Refer to Bluetooth protocol, 1 byte */
    uint8_t device_class_minor;                /* minor cod, Refer to Bluetooth protocol, 1 byte */
    uint8_t account_hash[BT_DEVICE_ACCOUNT_HASH_LEN];
    uint8_t reserved[STRUCT_RESERVED_SIZE_8];
    uint16_t pdlCrc; /* valid data crc, valid data include all filed in addition to reserved filed and pdlCrc */
} service_to_bt_pd_data;

typedef enum {
    TWS_EARSIDE_INVALID = 0,
    TWS_EARSIDE_LEFT = 1,
    TWS_EARSIDE_RIGHT = 2,
} service_tws_ear_side;

typedef struct {
    uint8_t ear_side_info;          /* service_tws_ear_side */
    uint8_t earphone_battery_level; /* 0-100 */
    uint8_t charging_state;        /* 1:in charging,0:not in charging */
    uint8_t reserved;
} service_tws_ear_battery_info;

typedef struct {
    uint8_t box_battery_level; /* 0-100 */
    uint8_t charging_state;   /* 1:in charging,0:not in charging */
    uint16_t reserved;
} service_box_battery_info;

typedef struct {
    uint8_t peer_ear_state; /* UX_InOrOutBox */
    uint8_t is_coupled;    /* 0x00:not coupled,0x01:coupled */
    uint8_t first_out_box;  /* service_tws_ear_side */
    uint8_t last_in_box;    /* service_tws_ear_side */
    bd_addr coupled_addr;
    bd_addr local_addr;
    service_tws_ear_battery_info local_battery_info;
    service_tws_ear_battery_info peer_battery_info;
    service_box_battery_info box_battery_info;
    uint8_t peer_model_id[MODEL_ID_LEN];
    uint8_t peer_sub_model_id;
    uint8_t multiplte_device_switch;
    uint8_t ismerged; /* is merged pdl info when close box */
    uint8_t pdnum;
    uint32_t device_pair_time;     /* 设备配对时长，单位ms */
    uint8_t is_forced;             /* 1:用户主动触发的强配， 0：非用户主动触发的强配 */
} __attribute__((__packed__)) service_to_bt_open_box_data;

/* SERVICE_UX_TO_CMD_CLOSE_BOX */
typedef struct {
    uint8_t peer_ear_state; /* SERVICE_EARPHONE_STATE */
    uint8_t is_coupled;    /* 0x00:not coupled,0x01:coupled */
    uint8_t reserved[STRUCT_RESERVED_SIZE_2];
#ifdef RUN_WITHOUT_BOX
    uint8_t pdnum;
#endif
} __attribute__((__packed__)) service_to_bt_close_box_data;

/* SERVICE_UX_TO_CMD_RE_CONN */
typedef struct {
    uint8_t reconn_flag; /* 回连标识 1:进入回连 0：终止回连 */
    uint8_t reserved;
} service_to_bt_re_conn;

/* SERVICE_UX_TO_CMD_HFP_VOLUME */
typedef struct {
    uint8_t volume;
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} service_to_bt_hfp_volume;

/* SERVICE_UX_TO_CMD_AVRCP_VOLUME */
typedef struct {
    int8_t volume;
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} service_to_bt_avrcp_volume;

/* SERVICE_UX_TO_CMD_BT_NAME */
typedef struct {
    char *bt_name;
    uint16_t name_size;
    uint8_t reserved;
} service_bt_device_name; // unuse

typedef enum {
    TWS_SNIFF_TYPE_PEER_NO_TRAFFIC = 0,     /* 无业务时，双耳进sniff */
    TWS_SNIFF_TYPE_PEER_HAS_TRAFFIC = 1,    /* 有业务时，双耳进sniff */
    TWS_SNIFF_TYPE_SPEC_MOBILE = 2,         /* 非业务状态下，耳机给特定的手机(支持TWS耳机特有功能)发进sniff命令 */
    TWS_SNIFF_TYPE_NORM_MOBILE = 3,         /* 非业务状态下，耳机给normal手机发进sniff命令 */
    TWS_SNIFF_TYPE_NO_HFP_MOBILE = 4,       /* 不支持SCO的蓝牙设备，如手表等设备需使用较大的sniff周期 */

    TWS_SNIFF_TYPE_MAX              = 0xFFFFFFFF, /* Ensure this type takes 4 bytes */
} tws_enter_sniff_type;

typedef struct {
    tws_enter_sniff_type type;
    uint16_t sniff_max_interval;
    uint16_t sniff_min_interval;
    uint16_t sniff_attempt;
    uint16_t sniff_timeout;
} tws_sniff_para_tbls;

/* SERVICE_UX_TO_CMD_SET_BTH_CONFIG */
typedef struct {
    tws_sniff_para_tbls tws_sniff_para[TWS_SNIFF_TYPE_NMU];  /* 保连间隔 */
    uint32_t ble_interval_time; /* ble广播间隔 */
    uint16_t nearby_uuid;       /* 靠近发现使用的gatt service uuid */
    uint8_t ear_role_cfg;       /* 0:不固定主副耳; 1:固定主副耳，左耳; 2：固定主副耳，右耳 */
    uint8_t ble_adv_tx_pwr;
    uint8_t reserved[STRUCT_RESERVED_SIZE_32];
} ham_set_bth_cfg_ind_t;

#if !defined(UX_BS_BT_SUPPORT)
/* SERVICE_UX_TO_CMD_ACK_AUDIO_A2DP_STATE */
typedef enum {
    A2DP_AUDIO_STATE_NONE,
    A2DP_AUDIO_STATE_PAUSED,
    A2DP_AUDIO_STATE_PLAYING,
    A2DP_AUDIO_STATE_STOPPED
} service_a2dp_audio_state;
#endif

typedef struct {
    uint8_t state; /* service_a2dp_audio_state */
    uint8_t success;
    uint16_t reserved; /* Use later */
} service_ack_audio_a2dp;

/* SERVICE_UX_TO_CMD_ACK_AUDIO_SCO_STATE */
typedef struct {
    uint8_t state; /* SERVICE_HFP_SCO_STATE */
    uint8_t success;
    uint16_t reserved; /* Use later */
} service_ack_audio_sco;

/* SERVICE_UX_TO_CMD_IMPROVE_DATA_TRANSFER_PRIORITY */
typedef enum {
    BT_CLOSE_PRIORITY,
    BT_OPEN_PRIORITY,
} bt_priority_switch;

typedef enum {
    DATA_TRANSPORT_BUSINESS_DEFAULT,       // commucate with remote by default spp uuid
                                           // for hota app and factory test app
    DATA_TRANSPORT_BUSINESS_ICONNECT,      // commucate with remote by ccmp&mbb
    DATA_TRANSPORT_BUSINESS_ICONNECT_CCMP, // commucate with remote by ccmp&mbb
    DATA_TRANSPORT_BUSINESS_VOICE_ID,      // commucate with remote by  uuid 0xfdd1
    DATA_TRANSPORT_BUSINESS_CLOUD_SYNC,
    DATA_TRANSPORT_BUSINESS_GATT_BOX,    // reserved for TWS. It's used by FIJI box only
    DATA_TRANSPORT_BUSINESS_GATT_IPHONE, // commucate with remote IOS device by gatt
    // business type for phone and headset: no big than 31
    DATA_TRANSPORT_BUSINESS_END = 31,
    // business type for left ear and right ear, start from 32
    DATA_TRANSPORT_BUSINESS_APP_BEGIN = 32, // private protocol within master and slave
    DATA_TRANSPORT_BUSINESS_FOTA = DATA_TRANSPORT_BUSINESS_APP_BEGIN,
    DATA_TRANSPORT_BUSINESS_CONFIG,
    DATA_TRANSPORT_BUSINESS_LOG,
    DATA_TRANSPORT_BUSINESS_SENSORS_DATA,
    DATA_TRANSPORT_BUSINESS_SENSORS_SYNC,
    DATA_TRANSPORT_BUSINESS_MULTIPLE_SWITCH,
    DATA_TRANSPORT_BUSINESS_APP_END = DATA_TRANSPORT_BUSINESS_MULTIPLE_SWITCH,
} data_transport_business_type;

typedef struct {
    uint8_t switch_state;  // bt_priority_switch
    uint8_t business_type; // data_transport_business_type
    bd_addr addr;         // remote addr
    uint8_t reserved[STRUCT_RESERVED_SIZE_4];
} bt_data_transfer_priority;

/* SERVICE_UX_TO_CMD_DISCONNECT_AUDIO_PROFILE_REQ */
typedef struct {
    bd_addr remote_addr;
    uint8_t reserved[STRUCT_RESERVED_SIZE_2];
} bt_hota_moible_addr;

/* SERVICE_UX_TO_CMD_NOTIFY_TONE_REQ */
typedef struct {
    uint8_t tone_type_id;     /* ToneTYPE */
    uint8_t not_start_toggle; /* toggle flag, notification_stop not start toggle */
    uint8_t reserved[STRUCT_RESERVED_SIZE_2];
} bt_sync_tone_type;

/* SERVICE_UX_TO_CMD_ACK_PLAY_LOCAL_FIND_STATE */
typedef enum {
    SERVICE_LOCAL_FIND_STOP,
    SERVICE_LOCAL_FIND_PLAY,
} service_play_local_find_state;

/* SERVICE_UX_TO_CMD_SWITCH_CHAGE */
typedef enum {
    SERVICE_SWITCH_TYPE_WEAR_DETECTION = 0,
    SERVICE_SWITCH_TYPE_MULTIPLE_DEVICE = 1,
    SERVICE_SWITCH_TYPE_HD_SCO = 2,
    SERVICE_SWITCH_TYPE_IOS_REJECT_LINK = 3,
    SERVICE_SWITCH_TYPE_UPDATE_BLE_LINK = 5,
} service_switch_type; // userd for rcv app cmd

typedef struct {
    uint16_t switch_type; // service_switch_type
    uint16_t switch_len;
    uint8_t switch_value[0];
} service_common_switch;

typedef enum {
    SERVICE_WEAR_DETECTION_SWITCH_OFF, // 0 for off
    SERVICE_WEAR_DETECTION_SWITCH_ON,  // 1 for on
} service_wear_detection_switch_type;

typedef struct {
    uint8_t wear_detection_switch; // service_wear_detection_switch
    bd_addr device_addr;         // addr len: 6
    uint8_t reserved;           // reserved len: 1
} service_wear_detection_switch;

typedef struct {
    uint8_t feature_type;
    uint8_t multil_conn_switch_flag;
    bd_addr mobile_addr;
} server_multi_conn_switch;

/* SERVICE_UX_TO_CMD_REMAINDER_PDL_INFO */
typedef struct {
    uint8_t curr_pdl_num;
    uint8_t total_pdl_num;
    uint8_t reserved[STRUCT_RESERVED_SIZE_2];
    service_to_bt_pd_data pd_data[0];
} __attribute__((__packed__)) bt_remainder_pdl_info;

/* SERVICE_UX_TO_CMD_OPEN_BOX_SWITCHES_INFO */
typedef struct {
    uint16_t tlvlen;
    uint8_t tlv[0];
} bt_app_switches_info;

typedef enum {
    OPEN_BOX_UNKNOWN_SWITCH = 0x00,
    OPEN_BOX_MULTI_CONN_SWITCH = 0x01,
    OPEN_BOX_HD_VOICE_SWITCH = 0x02,
    OPEN_BOX_SEIZE_SWITCH    = 0x03,
} open_box_switches_type; // used for open box

typedef struct {
    uint8_t switch_status;
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} open_box_multi_conn_switch; // used for open box:

typedef struct {
    uint8_t hd_sco_switch_flg;
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} server_hd_sco_switch; // userd for rcv app cmd & used for open box:

typedef struct {
    uint8_t curr_state; /* device_seize_switch_state */
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} service_seize_switch_state;

typedef enum {
    SERVICE_SEIZE_SWITCH_OFF = 0x00,
    SERVICE_SEIZE_SWITCH_ON  = 0x01,
} device_seize_switch_state;

/* SERVICE_MF_TO_CMD_SIGNAL_MODE */
typedef struct {
    uint8_t mode; /* BT_WORK_MODE */
    bd_addr addr;
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} service_signal_mode;

/* SERVICE_MF_TO_CMD_NO_SIGNAL_MODE */
typedef struct {
    uint8_t mode; /* BT_WORK_MODE */
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} service_nosignal_mode;

/* SERVICE_MF_TO_CMD_NO_SIGNAL_MODE_CMD_DATA */
typedef struct {
    uint8_t length;
    void *payload;
} service_bt_device_nosignal_mode_cmd;

/* SERVICE_MF_TO_BT_DEVICE_CMD */
typedef struct {
    uint16_t cmd;
} bt_rf_test_data;

/* SERVICE_SPP_SEND */
typedef enum {
    DATA_TRANSPORT_DEVICE_TYPE_MOBILE, // data transport between a mobile and a earphone
    DATA_TRANSPORT_DEVICE_TYPE_TWS,    // data transport between the two earphone
} data_transport_device_type;

typedef struct {
    bd_addr remote_addr;       // remote bluetooth device's address
    uint8_t remote_device_type; // data_transport_device_type
    uint8_t business_type;     // data_transport_business_type
    uint16_t service_sppport;  // unique port num distinguish different devices and  different business in one device
                              // SPP PORT: 10bits(id) + 5bits((server channel) + 1bit(1-server,0-client)
    uint16_t len;             // len of data
    uint8_t data[0];
} __attribute__((__packed__)) data_transport_data;

/* SERVICE_SPP_SYNC_SEND */
typedef struct {
    uint16_t ret_buf_len;
    uint16_t *ret_len;
    void *ret_buf;
    data_transport_data *send_data;
} __attribute__((__packed__)) data_transport_data_with_retval;

/* MSG_BT_NV_READ_DEVICE_ADDR_RSP */
typedef enum {
    BT_READ_SUCCESS,
    BT_READ_FAIL,
    BT_READ_INVALID
} bt_nv_read_data_result;

typedef struct {
    uint8_t result; /* bt_nv_read_data_result */
    bd_addr bt_address;
    uint8_t reserved;
} bt_nv_device_address;

/* MSG_BT_NV_READ_EARBUD_NATURE_RSP */
typedef struct {
    uint8_t result;        /* bt_nv_read_data_result */
    uint8_t earbud_nature; /* service_tws_ear_side */
    uint8_t reserved[STRUCT_RESERVED_SIZE_2];
} bt_nv_earbud_nature_info;

/* MSG_BT_NV_READ_EARBUD_PDL_RSP */
typedef struct {
    uint8_t link_key[LINK_KEY_LEN];
    uint8_t key_type;
    uint8_t profile;   /* service_profile */
    uint8_t extra_info; /* bitmap */
    uint8_t reserved;
} bt_nv_earbud_info;

typedef struct {
    uint8_t ear_pdl_data_size;
    bt_nv_earbud_info ear_pdl_data;
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} bt_nv_earbud_pdl;

/* MSG_BT_NV_READ_MOBILE_PDL_INFO_RSP */
typedef struct {
    uint8_t result; /* bt_nv_read_data_result */
    uint8_t pd_num;
    uint8_t curr_pdl_num;
    uint8_t reserved[STRUCT_RESERVED_SIZE_1];
#if defined(UX_BS_BT_SUPPORT)
    bt_nv_data_pdl_s pd_data[PD_NUM_MAX_HALF];
#else
	service_to_bt_pd_data pd_data[PD_NUM_MAX_HALF];
#endif
} bt_nv_mobile_pdl;

/* MSG_BT_NV_READ_MOBILE_PDL_CONN_ORDER_RSP */
typedef struct {
    uint8_t result; /* bt_nv_read_data_result */
    uint8_t pd_num;
    uint8_t pdl_conn_order[0];
} bt_nv_mobile_pdl_order;

typedef struct {
    uint8_t pd_num;
    uint8_t pdl_conn_order[PD_NUM_MAX];
} service_to_bt_pdl_order_data;

/* MSG_BT_NV_READ_PEER_ADDR_RSP */
typedef struct {
    uint8_t result; /* bt_nv_read_data_result */
    bd_addr peer_ear_add;
    uint8_t reserved;
} bt_nv_peer_addr_info;

/* MSG_BT_NV_READ_CLOUD_TOKEN_RSP */
typedef struct {
    uint8_t result; /* bt_nv_read_data_result */
    uint8_t cloud_token[CLOUD_TOKEN_LEN];
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} bt_nv_cloud_token;

/* MSG_BT_NV_READ_LOG_LEVEL_RSP */
typedef struct {
    uint8_t result; /* bt_nv_read_data_result */
    uint8_t log_level;
    uint8_t reserved[STRUCT_RESERVED_SIZE_2];
} bt_nv_log_level;

/* MSG_BT_NV_READ_ADV_KEY_RSP */
typedef struct {
    bool key_vaild_flag;
    bool key_updata_flag;
    uint8_t reserved[STRUCT_RESERVED_SIZE_2];
    uint8_t irk[IRK_HBKP_LEN];
    uint8_t hbkp[IRK_HBKP_LEN];
} bt_nv_sec_adv_key_data_s;

/* MSG_BT_NV_READ_ALL_DATA_RSP */
typedef struct {
    bd_addr local_addr;
    bd_addr ble_addr;
    uint8_t earbud_nature; /* service_tws_ear_side */
    uint8_t log_level;
    uint8_t has_coupled_earbud;
    bd_addr coupled_addr;
    uint8_t has_token;
    uint8_t cloud_token[CLOUD_TOKEN_LEN];
    uint8_t name_size;
    uint8_t device_name[DEVICE_NAME_LEN];
    uint32_t device_name_time_stamp;
    int8_t nearby_rssi;
    int8_t circle_rssi;
    uint8_t model_id[MODEL_ID_LEN];
    uint8_t submodel_id;
#ifdef CONFIG_ADD_PRODUCT_ID_FIELD
    uint8_t product_id[PRODUCT_ID_LEN];
    uint8_t headset_version[VERSION_NAME_LEN];
#endif
    uint8_t device_sn[DEVICE_SN_LEN];
    uint8_t local_name[LOCAL_NAME_LEN];
    bt_nv_earbud_pdl earbud_pd_info;
    bt_nv_sec_adv_key_data_s sec_adv_key_info;
    uint8_t pd_num;
    uint8_t pdl_conn_order[PD_NUM_MAX];
    service_to_bt_pd_data pd_data[0];
} __attribute__((__packed__)) bt_nv_init_all_data;

/* MSG_BT_NV_READ_NEARBY_RSSI_RSP */
typedef struct {
    uint8_t result;
    int8_t nearby_rssi;
    int8_t circle_rssi;
    uint8_t reserved;
} bt_nv_nearby_rssi;

/* MSG_BT_NV_READ_MODEL_ID_RSP */
typedef struct {
    uint8_t result;
    uint8_t model_id[MODEL_ID_LEN];
    uint8_t sub_model_id;
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} bt_model_id;

/* MSG_BT_NV_READ_DEVICE_SN_RSP */
typedef struct {
    uint8_t result;
    uint8_t device_sn[DEVICE_SN_LEN];
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} bt_device_sn;

/* MSG_BT_NV_READ_BTC_RF_DATA_RSP */
typedef struct {
    uint8_t nv_tx_power_cali_num;                                   // 功率校准频点个数（不支持修改）
    int16_t nv_tx_power_cali_pref_offset[TXPWR_MAX_NUM];            // 对应频点pref（不支持修改）
    uint8_t nv_tx_power_cali_freq[TXPWR_MAX_NUM];                   // 需要校准的频点（不支持修改）
    uint8_t nv_tx_power_cali_band[TXPWR_MAX_NUM];                   // 各个band的边界频点（不支持修改）
    int16_t nv_tx_power_coff[TXPWR_CALI_COFF_NUM];                  // 二项式系数（不支持修改）
    uint16_t nv_rxdc_code_lna_noraml[BT_RF_LNA_MAX];                // RX DC 0-3档的校准值,共4（不支持修改）
    uint16_t nv_rxdc_code_lna_four[NV_RXDC_BAND_NUM];               // RX DC 4档各个band的校准值（不支持修改）
    uint16_t nv_max_tx_power_cfg_val[NV_POWER_MODE_NUM];            // 各种模式的最大发射功率（可修改，详看Melody 功能特性配置说明书3.2.2 3.说明)
    uint32_t nv_cali_switch_all;                                    // 校准总开关（8 ：打开功率可配置； 0：关闭功率可配置；）
    int16_t nv_highpower_tx_power_cali_perf_offset[TXPWR_MAX_NUM];  // 高频功率校准对应频点pref（不支持修改）
    int16_t nv_highpower_tx_power_coff[TXPWR_CALI_COFF_NUM];        // 高功率校准二项式二次项系数（不支持修改）
    uint8_t nv_bt_board_config;                                     // 单板特性开关（不支持修改）
    uint32_t nv_bt_feature_config;                                  // feature控制项的开关（不支持修改）
    uint32_t nv_bt_ext_feature_reserved;                            // feature控制开关扩展字段（不支持修改）
    uint32_t nv_bt_private_config;                                  // 私有化特性配置参数（不支持修改）
    uint32_t nv_bt_ext_private_config;                              // 私有化特性扩展字段（不支持修改）
    uint32_t nv_reserved[STRUCT_RESERVED_SIZE_30];                  // 预留（不支持修改）
} __attribute__ ((__packed__)) bt_calibration_param;

#if !defined(UX_BS_BT_SUPPORT)
/* SERVICE_TO_UX_BT_MAIN_STATE_MSG */
typedef enum {
    SERVICE_MAIN_STATE_NOT_READY,
    SERVICE_MAIN_STATE_IDLE,
    SERVICE_MAIN_STATE_INCOMING_CALL,
    SERVICE_MAIN_STATE_CALL_ACTIVE,
    SERVICE_MAIN_STATE_AUDIO_ACTIVE,
    SERVICE_MAIN_STATE_OUTGOING_CALL,
    SERVICE_MAIN_STATE_HELD_CALL,
    SERVICE_MAIN_STATE_3WAY_INCOMING_CALL,
    SERVICE_MAIN_STATE_3WAY_CALL_ACTIVE,
    SERVICE_MAIN_STATE_END
} service_main_state;
#endif

typedef struct {
    uint8_t main_state; /* service_main_state */
    bd_addr addr;      /* Mobile Addr */
    uint8_t reserved1;
} __attribute__((__packed__)) service_to_ux_bt_main_state_msg_param;

/* SERVICE_TO_UX_PROFILE_STATE_MSG */
typedef struct {
    uint8_t profile;      /* service_profile */
    uint8_t profile_state; /* SERVICE_PROFILE_CONNECTION_STATE */
    bd_addr addr;         /* Mobile Addr */
} __attribute__((__packed__)) service_to_ux_bt_profile_state_msg_param;

/* SERVICE_TO_UX_A2DP_STATE_MSG */
typedef struct {
    uint8_t a2dp_audio_state; /* service_a2dp_audio_state */
    uint8_t reserved1;
    uint16_t reserved2;
} __attribute__((__packed__)) service_to_ux_a2dp_state_msg_param;

/* SERVICE_TO_UX_SCO_STATE_MSG */
typedef struct {
    uint8_t sco_state; /* SERVICE_HFP_SCO_STATE */
    uint8_t reserved1;
    uint16_t reserved2;
} __attribute__((__packed__)) service_to_ux_sco_state_msg_param;

/* SERVICE_TO_UX_MOBILE_CONNECT_STATE_MSG */
typedef struct {
    uint8_t acl_state; /* SERVICE_BT_ACL_STATE */
    bd_addr addr;      /* Mobile Addr */
    uint8_t is_has_ble_addr;
    bd_addr ble_addr;
    uint8_t reason_code;
} __attribute__((__packed__)) service_to_ux_mobile_connect_state_msg_param;

/* SERVICE_TO_UX_MOBILE_LINK_MODE_MSG */
typedef struct {
    uint8_t link_mode; /* SERVICE_BT_LINK_MODE */
    bd_addr addr;      /* Mobile Addr */
    uint8_t reserved1;
} __attribute__((__packed__)) service_to_ux_mobile_link_mode_msg_param;

/* SERVICE_TO_UX_TWS_CONNECT_STATE_MSG */
typedef struct {
    uint8_t acl_state; /* SERVICE_BT_ACL_STATE */
    uint8_t reason_code;
    uint16_t reserved2;
} service_to_ux_tws_connect_state_msg_param;

/* SERVICE_TO_UX_MOBILE_PAIRING_STATE_MSG */
typedef struct {
    uint8_t pairing_state; /* SERVICE_BT_BOND_STATE */
    uint8_t reserved1;
    uint16_t reserved2;
} service_to_ux_mobile_pairing_state_msg_param;

/* SERVICE_TO_UX_VOICE_ASSISTANT_STATE_MSG */
typedef enum {
    /* To UX */
    SERVICE_VOICE_ASSISTANT_OFF,
    SERVICE_VOICE_ASSISTANT_ON
} service_voice_assistant_state;

typedef struct {
    uint8_t state; /* service_voice_assistant_state */
    uint8_t reserved1;
    uint16_t reserved2;
} service_to_ux_voice_assisant_state_msg_param;

/* SERVICE_TO_UX_HFP_VOLUME_UPDATE_MSG */
typedef struct {
    uint32_t toggle_cnt;
    uint8_t hfp_vol;
    uint8_t vaild_flag;
    uint16_t reserved;
} service_to_ux_hfp_volume_updata_msg_param;

/* SERVICE_TO_UX_AVRCP_VOLUME_UPDATE_MSG */
typedef struct {
    uint32_t toggle_cnt;
    uint8_t avrcp_vol;
    uint8_t vaild_flag;
    uint16_t reserved;
} service_to_ux_avrcp_volume_updata_msg_param;

/* SERVICE_TO_UX_HANDOVER_STATUS_MSG */
typedef enum {
    SERVICE_BT_HANDOVER_NONE,
    SERVICE_BT_HANDOVER_STARTING,
    SERVICE_BT_HANDOVER_SUCCESS,
    SERVICE_BT_HANDOVER_FAILED
} service_bt_handover_state;

typedef struct {
    uint8_t handover_state; /* service_bt_handover_state */
    uint8_t reserved1;
    uint16_t reserved2;
} service_to_ux_handover_status_msg_param;

/* SERVICE_TO_UX_A2DP_CODEC_SETTING_MSG */
typedef struct {
    service_a2dp_stream_config_s a2dp_codec_setting;
} service_to_ux_a2dp_code_setting_msg_param;

/* SERVICE_TO_UX_SCO_CODEC_SETTING_MSG */
typedef struct {
    service_hfp_config_s sco_codec_setting;
} service_to_ux_sco_code_setting_msg_param;

/* SERVICE_TO_UX_MIC_STATE_MSG */
typedef enum {
    MIC_OPEN,
    MIC_CLOSE
} service_mic_ux_cmd;

typedef struct {
    uint8_t mic_state; /* service_mic_ux_cmd */
    uint8_t reserved1;
    uint16_t reserved2;
} service_to_ux_mic_status_msg_param;

/* SERVICE_TO_UX_PLAY_OUTBAND_RING_MSG */
typedef enum {
    RING_STOP,
    RING_PLAY,
} outband_ring_type;

typedef struct {
    uint8_t ring_type; /* outband_ring_type */
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} service_outband_ring_param;

/* SERVICE_TO_UX_WORK_MODE_MSG */
typedef enum {
    SERVICE_BT_WORK_UNKNOWN,
    SERVICE_BT_WORK_MTWS,
    SERVICE_BT_WORK_SNIFF_TWS,
    SERVICE_BT_WORK_MODE_FREEMAN
} service_bt_work_mode;

typedef struct {
    uint8_t tws_mode; /* service_bt_work_mode */
    bd_addr addr;
    uint8_t reserved1;
} __attribute__((__packed__)) service_to_ux_tws_mode_setting_msg_param;

/* SERVICE_TO_UX_TWS_ROLE_MSG */
typedef struct {
    uint8_t tws_role; /* ServiceTwsLinkRole */
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} service_to_ux_tws_role_param;

/* SERVICE_TO_UX_HIGH_PERF_MODE_TYPE */
typedef enum {
    BT_UNKOWN_MODE,
    BT_RELEASE_HIGH_PERF_MODE,
    BT_TRIGGER_HIGH_PERF_MODE
} bt_high_perf_mode_state;

typedef struct {
    uint8_t mode_type; // BtHightPerModeState
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} bt_high_perf_mode;

/* SERVICE_UX_TO_CMD_DISCONNECT_ALL_PROFILE_RSP */
typedef struct {
    uint8_t result; // BtDisConnectProfileM
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} bt_disconnect_profile_result;

/* SERVICE_UX_TO_CMD_BT_INIT_RSP */
typedef struct {
    uint8_t result;
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} bt_init_result;

/* SERVICE_TO_UX_WARN_TONE_RSP */
typedef struct {
    uint8_t tone_type_id; /* BtToneTYPE */
    uint8_t valid_flag;
    uint8_t reserved[STRUCT_RESERVED_SIZE_2];
    uint32_t toggle_cnt;
    uint32_t toggle_diff_cnt;
    uint32_t tog_period;
} bt_sync_tone_info;

/* SERVICE_TO_UX_MAINTENANCE_TEST_HANDOVER_RSP */
typedef struct {
    uint8_t result; // success:0;fail:1
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} bt_maintenance_test_handover_res;

/* SERVICE_TO_UX_MAINTENANCE_TEST_CHECK_RSSI_RSP */
typedef struct {
    int8_t rssivalue;
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} bt_maintenance_test_rssi;

/* SERVICE_TO_UX_NOTIFY_POSITION */
typedef struct {
    uint8_t position;                        /* ux_position_to_bt */
    uint8_t position_before_adapter_conversion; /* SERVICE_TO_UX_POSITION_S */
    uint8_t reserved[STRUCT_RESERVED_SIZE_2];
} bt_peer_position;

/* SERVICE_TO_UX_PAIRING_MODE_STATE */
typedef enum {
    ENTERING_PAIRING_MODE,
    EXIT_PAIRING_MODE,
    PAIRING_MODE_INIT_VAL
} pairing_mode_type;

typedef struct {
    uint8_t pairing_state_type;
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} bt_pairing_state;

/* SERVICE_TO_UX_HDAP_CODEC_SETTING_MSG */
typedef enum {
    HDAP_LC3_SF_48000,
} bt_hdap_codec_type;

typedef struct {
    uint8_t sample_rate;
    uint8_t codec;
    uint8_t channel;
    uint8_t bit_width;
} service_hdap_stream_config_s;

typedef struct {
    service_hdap_stream_config_s hdap_codec_setting;
} service_to_ux_hdap_codec_setting_msg_param_s;

/* SERVICE_TO_UX_HDAP_START */
typedef struct {
    uint8_t mode;
    uint8_t reserved1;
    uint16_t reserved2;
} service_to_ux_hdap_start_param_s;

/* SERVICE_TO_UX_HDAP_STOP */
typedef struct {
    uint8_t mode;
    uint8_t reserved1;
    uint16_t reserved2;
} service_to_ux_hdap_stop_param_s;

/* SERVICE_TO_UX_HDAP_KARAOKE_PARA */
typedef struct {
    uint8_t datalen;
    uint8_t data[BT_TO_UX_HDAP_PARA_AMX_LEN];
} service_to_ux_karaoke_param_s;

/* SERVICE_TO_UX_DEVICE_NAME */
typedef struct {
    uint8_t result;
    uint8_t name_size;
    uint8_t reserved[STRUCT_RESERVED_SIZE_2];
    uint8_t device_name[DEVICE_NAME_LEN];
    uint32_t time_stamp;
} bt_nv_device_name;

/* SERVICE_TO_UX_PLAY_LOCAL_FIND */
typedef enum {
    SERVICE_STOP_DEVICE_FIND,
    SERVICE_PLAY_DEVICE_FIND,
    SERVICE_QUERY_DEVICE_FIND
} service_play_device_find_mode;

typedef struct {
    uint8_t local_find_state;
    uint8_t reserved1;
    uint16_t reserved2;
} service_play_local_state_param_s;

/* SERVICE_TO_UX_ACK_START_HAND_HELD */
typedef struct {
    uint8_t result; // 0:SUCCESS others:FAIL
    uint8_t reserved1;
    uint16_t reserved2;
} service_to_ux_ack_hand_held_param_s;

/* SERVICE_TO_UX_ACTIVE_DEVICE_MSG */
typedef struct {
    bd_addr addr; /* SERVICE_ACTIVE_DEVICE */
    uint8_t reserved[STRUCT_RESERVED_SIZE_2];
} __attribute__((__packed__)) service_to_ux_active_device;

/* SERVICE_TO_UX_MAINTENANCE_TEST_CHECK_DEVICE_RSSI_RSP */
typedef struct {
    int8_t rssivalue;
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} bt_maintenance_test_device_rssi;

/* SERVICE_TO_UX_ACCESS_MODE_CHANGE_MSG */
typedef struct {
    uint8_t access;
} __attribute__((__packed__)) service_access_mode_change;

/* MSG_BT_NV_WRITE_MOBILE_PD_INFO_REQ */
typedef enum {
    BT_SINGLE_PD_REASON_DEFAULT,
    BT_SINGLE_PD_REASON_ADD_NEW,
    BT_SINGLE_PD_REASON_UPDATE_NAME,
} bt_write_single_pd_to_nv_reason;

typedef struct {
    uint8_t index;
    uint8_t pd_num;
    bt_write_single_pd_to_nv_reason write_reason;
    uint8_t reserved[STRUCT_RESERVED_SIZE_1];
    service_to_bt_pd_data pd_data;
} bt_nv_mobile_pd;

/* SERVICE_TO_MF_NO_SIGNAL_MODE_MSG */
typedef struct {
    uint8_t msgtype;
    uint8_t evcode;
    uint8_t length;  // length of the params
    char payload[0]; // parameters
} service_nosignal_ack;

/* SERVICE_RECV_SPP_PORT_STATE_CHANGE_MSG */
typedef struct {
    bd_addr remote_addr;        // remote bluetooth device's address
    uint8_t business_type;      // data_transport_business_type
    uint8_t remote_device_type; // data_transport_device_type
    uint16_t service_spp_port;  // unique port num distinguish different devices and  different business in one device,
                                // SPP PORT: 10bits(id) + 5bits((server channel) + 1bit(1-server,0-client)
    uint16_t state;             // SERVICE_PROFILE_CONNECTION_STATE
    struct list_head _list_node;
} data_transport_port_state;

/* SERVICE_TO_UX_BT_TIMEOUT_REASON_MSG */
typedef struct {
    uint8_t result;
    uint8_t reserved[STRUCT_RESERVED_SIZE_3];
} service_to_ux_timeout_reason;

/* SERVICE_TO_UX_BT_TIMEOUT_REASON_MSG */
typedef enum {
    BT_PAIR_TIMEOUT,
    BT_PAIR_DISCNNECT_TIMEOUT,
    BT_TIMEOUT_SHUTDOWN,
} bt_timeout_reason_type;
#ifdef __cplusplus
}
#endif

/* SERVICE_UX_TO_CMD_BATTERY_INFO_STATE */
typedef struct {
    uint8_t type;
    uint8_t left_battery_percent;
    uint8_t left_charge_state;
    uint8_t left_in_or_out_box;
    uint8_t right_battery_percent;
    uint8_t right_charge_state;
    uint8_t right_in_or_out_box;
    uint8_t box_battery_percent;
    uint8_t box_charge_state;
    uint8_t reserved1;
    uint16_t reserved2;
} services_battery_info_state;

/* SERVICE_UX_TO_CMD_BATTERY_UPDATE_TYPE */
typedef enum {
    TYPE_UNKNOWN,
    HFP_CONNECTED,
    BATTERY_CHANGE,
} battery_update_type;

typedef enum {
    OUT_BOX = 0,
    IN_BOX
} in_or_out_box;

typedef struct {
    uint32_t type; /* 0:music 1:soc in 2:soc out */
    uint32_t adp;
} media_adp_map_req_t;

typedef struct {
    uint32_t type; /* 0:music 1:soc in 2:soc out */
} media_adp_unmap_req_t;


typedef struct {
    uint32_t result;
    uint32_t type;
} media_adp_map_rsp_t;

typedef struct {
    uint32_t result;
    uint32_t type;
} media_adp_unmap_rsp_t;

typedef struct {
    uint16_t data_len;
    uint8_t data[MAX_HAM_ATT_VALUE_LEN];
} ham_adp_write_data_ind_t;

typedef struct {
    uint16_t data_len;
    uint8_t data[0];
} ham_spp_write_data_ind_t;

/*  */
typedef enum {
    SERVICE_BLE_WORK_NAME,
    SERVICE_BLE_WORK_EXPAND
} service_ble_adv_val_mode;

typedef struct {
    uint8_t data_len;
    uint8_t scan_rsp_len;
    uint8_t data[BLE_ADV_DATA_LEN_MAX];
    uint8_t scan_data[BLE_ADV_DATA_LEN_MAX];
} ble_adv_data_t;

/* MLD SERVICE_MF_TO_CMD_CONN_DUT */
typedef struct {
    bd_addr addr;
    uint8_t action;  /* MLD 2:requst freq offset 1:connet 0:disconnect */
} bt_dut_info;

/* MLD SERVICE_MF_TO_CMD_ENTER_OFFSET_TEST_MODE */
typedef struct {
    uint8_t role;  /* MLD 0:matser 1:slave */
    uint8_t action;  /* MLD 1:enter offset test mode 0:exit offset test mode */
} bt_offset_test_mode_info;

/* MLD SERVICE_TO_MF_FREQ_OFFSET_RPT */
typedef struct {
    bd_addr remote_addr;
    int16_t freq_offset;
    uint8_t conn_state; /* MLD 2:ready(for offset report) 1:connect 0:disconnect */
    uint8_t reason_code; /* disconnect reason: only make sense when conn_state = 0 */
} service_to_mf_dut_freq_offset_param_s;


typedef enum {
    MF_CALC_ACTION_DISCONN = 0,
    MF_CALC_ACTION_CONN = 1,
    MF_CALC_ACTION_REQ = 2,
} mf_calculate_action_type;

typedef enum {
    AUTO_CALI_DISCONNECT = 0,
    AUTO_CALI_CONNECT = 1,
    AUTO_CALI_DATA_REQUEST = 2,
} bt_auto_cali_op_t;
#endif /* BT_SERVICE_H */
