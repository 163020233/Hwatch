/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description: ipc protocal common interface
 *
 * Create:
 */
#ifndef IPC_NEW_SHARED_H
#define IPC_NEW_SHARED_H

#include "ipc_actions.h"
#include "ipc_protocol.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct ipc_core_int_value {
    uint8_t dest_core;
    uint16_t chl_mask;
} ipc_core_int_mask_t;

/**
 * @if Eng
 * @brief  IPC memory management configuration.
 * @else
 * @brief  IPC内存管理配置。
 * @endif
 */
typedef struct ipc_memory_manage {
    uint8_t  src_core;          /*!< @if Eng IPC interrupt source core.
                                     @else   IPC中断源核。  @endif */
    uint8_t  dst_core;          /*!< @if Eng IPC interrupt destination core.
                                     @else   IPC中断目的核。  @endif */
    uint8_t  channel;           /*!< @if Eng IPC interrupt destination channel.
                                     @else   IPC中断通道。  @endif */
    uint8_t  reserved;          /*!< @if Eng Reserved.
                                     @else   保留。  @endif */
    uint32_t start_addr;        /*!< @if Eng Start address of IPC messages in the shared memory.
                                     @else   IPC消息在共享内存中的开始地址。  @endif */
    uint32_t total_len;         /*!< @if Eng Total length of IPC messages in the shared memory.
                                     @else   IPC消息在共享内存中的总长度。  @endif */
} ipc_memory_manage_t;

/**
 * @brief  Enum specifying ipc direction.
 */
typedef enum {
    IPC_DIR_TX = 0,
    IPC_DIR_RX,
    IPC_DIR_MAX_NUM,
} ipc_dir_e;

/**
 * @brief  Enum specifying the relative priority of the IPC message, with 0 indicating the highest priority.
 * Using the incrementing values allows for easy loops
 *      for (ipc_priority_e p = IPC_PRIORITY_HIGHEST; p <= IPC_PRIORITY_LOWEST; p++)
 */
typedef enum {
    IPC_PRIORITY_HIGHEST = 0, // !< Highest priority (lowest numeric value).
    IPC_PRIORITY_LOWEST,      // !< Lowest priority (highest numeric value).
    IPC_PRIORITY_MAX_NUM,     // !< Used to size arrays.
} ipc_priority_e;


typedef enum ipc_msg_id {
    IPC_MSG_CORE_STARTED = 0,                 // !< Sent to the security core if another core has started
    IPC_MSG_PROTOCOL_REBOOT_APPS = 1,         // !< Protocol Send Reboot Apps To security Implement
    IPC_MSG_EXCEPTION_TEST = 2,               // !< Sent to security core to test exception handle
    IPC_MSG_UNUSED_3 = 3,                     // !<
    IPC_MSG_FLASH_OP_CNF = 4,                 // !<
    IPC_MSG_FLASH_OP_IND = 5,                 // !<
    IPC_MSG_SYS_REBOOT_REQ = 6,               // !< Request a complete system reboot.
    IPC_MSG_LOG_INFO = 7,                     // !< Inform there are logs available.
    IPC_MSG_RPC_COMMAND = 8,                  // !< Used to send rpc style commands between the cores
    IPC_MSG_DATA = 9,                         // !< Used to send data
    IPC_MSG_WAKUP_INFO = 10,                  // !< Apps ipc wakeup interrupt.
    IPC_MSG_PLL_MICRO_REQ = 11,               // !< DSP ipc request PLL micro adjust interrupt.
    IPC_MSG_SET_LOG_LEVEL = 12,               // !< Used by App core to set the log level of BT core or DSP core.
    IPC_MSG_EXCEPTION_IND = 13,               // !< Used by Any core to inform the current exception.
    IPC_MSG_AP_BT_HOOK_DATA = 14,             // !< Used by App core to hook the data.
    IPC_MSG_BT_AP_HOOK_DATA = 15,             // !< Used by BT core to send the result of hook data.
    IPC_MSG_BT_HIFI_MEM_REQ = 16,             // !< Used by BT core to request the memory.
    IPC_MSG_HIFI_BT_MEM_CNF = 17,             // !< Used by DSP core to response the memory request.
    IPC_MSG_AP_BT_CALIBRATION_PARAM_REQ = 18, // !< Used by App core to send the calibration params to BT core.
    IPC_MSG_SET_SYSTEM_CLOCKS = 19,           // !< Used to switch system clocks.
    IPC_MSG_MASS_DATA_INFORM = 20,            // !< Used to inform there are coming mass data from bt&dsp core.
    IPC_MSG_SET_LOG_TIME = 21,                // !< Used to set the rtc time of compress log.
    IPC_MSG_REDUCE_VOLTAGE_AFTER_WAKEUP = 22, // !< Used to reduce voltage from 0.9v to 0.8v after wakeup.
    IPC_MSG_AUDIO_HOOK_DATA = 23,             // !< Used to hook audio data
    IPC_MSG_GET_CAL_FREQ_REQ = 24,            // !< Used by App core to get the freq of clock calibration.
    IPC_MSG_GET_CAL_FREQ_CNF = 25,            // !< Used by BT core to report the freq of clock calibration.
    IPC_MSG_HCI_INFORM = 26,                  // !< Used to inform there are coming hci data from app core.
    IPC_MSG_TS_MESSAGE_REQ = 27,              // !< Used by the test suite to send a command.
    IPC_MSG_TS_MESSAGE_CNF = 28,    // !< Used by the test suite to send the response to a previous command.
    IPC_MSG_SET_LOG_SWITCH = 29,    // !< Used to config log switch by AT cmd.
    IPC_MSG_BT_POWER_ON = 30,       // !< Used by BT core to inform App core no xip access
    IPC_MSG_UPDATA_TRNG = 31,       // !< Used to BT core to inform App core updata trng share memory
    IPC_MSG_SLAVE_ADJUST_FREQ = 32, // !< Used by Master core to inform slave core updata freq
    IPC_MSG_TEMP_CHANGE = 33,       // !< Used by BT core to inform App core temperature change
    IPC_MSG_DSP_CURRENT_FREQ = 34,  // !< Used by Master core to inform dsp core current freq
    IPC_MSG_BT_POWER_OFF = 35,      // !< Used by Master core to inform bt core power off.
    IPC_MSG_BT_HIGH_POWER = 36,     // !< Used by BT core to inform app core open or close high power.
    IPC_MSG_MASS_DATA_INFORM_WITH_EXTEND = 37, // !< Used to inform mass data by slave core in wear.
    IPC_MSG_BT_WRITE_NV = 39,                  // !< Used by BT core to inform app core bt write nv in product line.
    IPC_MSG_BT_READ_NV = 40,                   // !< Used by BT core to inform app core bt read nv before power on.
    IPC_MSG_APP_LOAD_NV = 41,                  // !< Used by APP core to inform bt core app read nv before power on.
    IPC_MSG_DSP_POWER_STATE = 42,              // !< Used by DSP core to inform App core POWER state
    IPC_MSG_BT_READ_NV_CUSTOMIZE = 43,  // !< Used by BT core to inform app core bt needs to load customized nv.
    IPC_MSG_APP_LOAD_NV_CUSTOMIZE = 44, // !< Used by APP core to inform bt core customized nv exist or not.
    IPC_MSG_BT_ATCMD = 45, // !< Used to get atcmd from bt
    IPC_MSG_BT_CALI = 46,
#if defined(TEST_IPC) || defined(AT_IPC_V1_TEST)
    IPC_MSG_TEST = 47,
#endif

    IPC_MSG_BT_AT_ADAPTER = 48,  // !< Used by App core to send at command to BT core,
                                    //            BT core send at log to App Core.
    IPC_MSG_BT_AP_ACCE_ECDH = 49,  // !< Used by BT core to call App core to calc ecdh
    IPC_MSG_AP_BT_GET_ECDH = 50,   // !< Used by App core to return ecdh result to BT core
    IPC_MSG_NPU_IPC_CMD = 51,      // !< Used by NPU to send cmd through IPC.
    IPC_MSG_HCI_BT_DATA = 52,
    IPC_MSG_BT_HEARTBEAT = 53,
    IPC_MSG_BT_CUSTOMIZE = 54,
    IPC_MSG_CORE_POWER_OFF = 55,
#if defined(TEST_IPC) || defined(AT_IPC_V1_TEST)
    IPC_MSG_TEST_HOST = 60,
    IPC_MSG_TEST_SLAVE = 61,
    IPC_MSG_AT_A2X_TEST = 62,

    // 异步测试
    IPC_ASYNC_TEST_MSG = 63,
    IPC_ASYNC_TEST_NOTIFY = 64,
    IPC_ASYNC_TEST_REPORT = 65,
#endif

    IPC_MSG_SET_LOG_LIMIT = 79,  // !< Used by App core to set the log limit of other core
    IPC_MSG_GET_LOG_LIMIT = 80,  // !< Used by App core to get the log limit of other core

    IPC_MSG_DSP_EVENT_REPORT = 83, // !< Used by DSP core to report event to MCU.
    IPC_MSG_NUM_MSGS,         // !< Used to size arrays.
#ifdef IPC_NEW_DEBUG
    IPC_MSG_A2B,
    IPC_MSG_A2S,
    IPC_MSG_A2D,
    IPC_MSG_A2G,
    IPC_MSG_B2A,
    IPC_MSG_D2A,
    IPC_MSG_G2A,
    IPC_MSG_D2B,
    IPC_MSG_B2D,
    IPC_MSG_D2S,
    IPC_MSG_G2S,
    IPC_MSG_S2A,
    IPC_MSG_S2D,
    IPC_MSG_S2G,
#endif
    IPC_MSG_FORCE_SIZE = 0x7f, // !< Used to force enum to 2 bytes
    IPC_MSG_ID_MAX
} ipc_msg_id_t;

#ifdef AT_IPC_V1_TEST
#define IPC_TEST_TYPE_PFMS_DATA_SEND    0
#define IPC_TEST_TYPE_PFMS_ONLY_COPY    1
#define IPC_TEST_TYPE_BASIC_COM_CMD     2
#define IPC_TEST_TYPE_BASIC_COM_RES     3
// 与 sizeof ipc_mailbox_ring_header_t 保持一致
#define SIZE_OF_RING_BUFFER_HEADER      12
// 与 sizeof ipc_ring_element_header_t 保持一致
#define SIZE_OF_RING_ELEMENT_HEADER     4
#if (CORE == APPS)
#define DATA_CHANNEL_LEN                (16 * 1024)
#else
#define DATA_CHANNEL_LEN                (3 * 1024)
#endif

typedef enum {
    IPC_TEST_SYNC = 0,
    IPC_TEST_ASYNC,
    IPC_TEST_ASYNC_MULTI,
} ipc_test_cmd_type;

typedef struct {
    int32_t src_core;
    int32_t dst_core;
    int32_t cmd_type;
    int32_t channel;
    int32_t times;
    int32_t size;
} ipc_test_params;

typedef struct {
    uint16_t cmd;
    uint16_t times;
    uint16_t len;
    uint8_t core;
    uint8_t channel;
} ipc_test_header;
#define IPC_TEST_DATA_LEN \
    (DATA_CHANNEL_LEN - SIZE_OF_RING_BUFFER_HEADER - SIZE_OF_RING_ELEMENT_HEADER - sizeof(ipc_test_header))
typedef struct {
    ipc_test_header header;
    uint8_t  data[IPC_TEST_DATA_LEN];
} ipc_test_struct;

void test_ipc_init(void);
void test_ipc_basic_send(ipc_test_params *argc);
void test_ipc_async(ipc_test_params *argc);

#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
