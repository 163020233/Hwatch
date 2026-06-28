/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides event codes \n
 *
 * History: \n
 * 2025-10-23， Create file. \n
 */

#ifndef EVENT_CODE_H
#define EVENT_CODE_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup common_evtcode Event Code Definition
 * @ingroup  common
 * @{
 */

/**
 * @brief  Definition of event code.
 */
typedef uint32_t evtcode_t;

/*
* 1 Common Event (0x9000_0000 - 0x9000_0100)
*/
#define EVTCODE_BOOT_FINISH                                 0x90000001
#define EVTCODE_SHUTDONW                                    0x90000002

/*
* 2 Power Event (0x9000_0101 - 0x9000_0200)
*/
#define EVTCODE_SUSPEND_ENTER                               0x90000101
#define EVTCODE_SUSPEND_EXIT                                0x90000102
#define EVTCODE_BATTERY_LEVEL_LOW                           0x90000103
#define EVTCODE_BATTERY_LEVEL_CRITICAL                      0x90000104
#define EVTCODE_CHARGING                                    0x90000105

/*
* 3 BT Event (0x9000_0201 - 0x9000_0300)
*/
#define EVTCODE_BT_CONNECT                                  0x90000201
#define EVTCODE_BT_DISCONNECT                               0x90000202
#define EVTCODE_BLE_CONNECT                                 0x90000203
#define EVTCODE_BLE_DISCONNECT                              0x90000204
#define EVECODE_BT_RSSI_LOW                                 0x90000205
#define EVTCODE_BT_ACL_CONNECT                              0x90000206
#define EVTCODE_BT_ACL_DISCONNECT                           0x90000207

/*
* 4 CPU Event (0x9000_0301 - 0x9000_0400)
*/
#define EVTCODE_CPU_LOAD_HIGH                               0x90000301
#define EVTCODE_CPU_LOAD_CRITICAL                           0x90000302
#define EVTCODE_CPU_OVERHEAT                                0x90000303

/*
* 5 Storage Event (0x9000_0401 - 0x9000_0500)
*/
#define EVTCODE_MEM_LOW                                     0x90000401
#define EVTCODE_MEM_CRITICAL                                0x90000402

/*
* 6 UI Event (0x9000_0501 - 0x9000_0600)
*/
#define EVTCODE_SCREEN_ON                                   0x90000501
#define EVTCODE_OTA_FILE_CHECK_FAIL                         0x90000502
#define EVTCODE_OTA_PREPARE_FAIL                            0x90000503
#define EVTCODE_OTA_REQUEST_FAIL                            0x90000504
#define EVTCODE_OTA_BACKUP_FAIL                             0x90000505
#define EVTCODE_OTA_UPGRADE_FAIL                            0x90000506

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}

#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
