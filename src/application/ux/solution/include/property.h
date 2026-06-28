/**
  Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
*
* Description: Provide property interface that base on KV \n
*/

#ifndef DUT_PROPERTY_H
#define DUT_PROPERTY_H

#include <stdint.h>
#include <stdbool.h>
#include <errcode.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PROPERTY_OK                    0
#define PROPERTY_TASK_STKDEPTH         0x640

#define PROPERTY_KEY_BT_ADDRESS         "rom.system.device.bt_address"
#define PROPERTY_KEY_EAR_SIDE           "rom.system.device.ear_side"
#define PROPERTY_KEY_PSN                "rom.system.device.psn"
#define PROPERTY_FACTORY_DEVICE_INFO    "rom.system.dev.info"
// rootkey
#define PROPERTY_KEY_OLD_ROOTKEY_COMPONENT1    "rom.system.device.rootkey"
#define PROPERTY_KEY_ROOTKEY_COMPONENT1        "persist.rootkey"
// record battery before restore factory
#define PROPERTY_KEY_BATTERY_BEFORE     "persist.system.record.battery"
// fota
#define PROPERTY_KEY_SILENT_UPGRADE     "system.ota.silent_flag"
// device sn
#define PROPERTY_KEY_OLD_DSN            "rom.system.device.dsn"
#define PROPERTY_KEY_DSN                "persist.system.record.dsn"
// model ID
#define PROPERTY_KEY_OLD_MODELID        "rom.system.device.modelid"
#define PROPERTY_KEY_MODELID            "persist.modelid"
// submodel ID
#ifdef CONFIG_UX_POPUPS_FEATURE
#define PROPERTY_KEY_OLD_SUBMODELID     "persist.submodelid"
#define PROPERTY_KEY_SUBMODELID         "user.setting.submodelid"
#else
#define PROPERTY_KEY_OLD_SUBMODELID     "rom.system.device.submodelid"
#define PROPERTY_KEY_SUBMODELID         "persist.submodelid"
#endif
// advanced version
#define PROPERTY_KEY_OLD_ADVANCED_VERSION   "rom.system.device.advance_version"
#define PROPERTY_KEY_ADVANCED_VERSION       "persist.advance_version"
// market
#define PROPERTY_KEY_OLD_MARKET         "rom.system.device.market"
#define PROPERTY_KEY_MARKET             "persist.market"
// ota image check time
#define PROPERTY_KEY_OTA_CHECKTIME      "persist.otachecktime"
// multiple switch
#define PROPERTY_KEY_MULTIPLE_DEVICE_SWITCH   "persist.multipledevice"

#define PROPERTY_VALUE_BT_ADDRESS_LEN   (3 * 6 - 1)
#define PROPERTY_VALUE_EAR_SIDE_LEFT    "1"
#define PROPERTY_VALUE_EAR_SIDE_RIGHT   "2"
#define PROPERTY_VALUE_PSN_LEN          EFUSE_ROM_PSN_SIZE
#define PROPERTY_VALUE_ROOTKEY_LEN      EFUSE_ROM_ROOTKEY_SIZE

typedef enum {
    PROPERTY_INFO = 0,
    PROPERTY_INFO_INIT_START,
    PROPERTY_INFO_DATA_SYNC_TIME,
    PROPERTY_INFO_DATA_PREPARE_TIME,
    PROPERTY_INFO_DATA_LOAD_TIME,
    PROPERTY_INFO_DATA_SAVE_TIME,
    PROPERTY_INFO_DATA_BACKUP_TIME,
    PROPERTY_INFO_INIT_POWER_OFF_HOOK,
    PROPERTY_ERR,
    PROPERTY_ERR_NULL,
    PROPERTY_ERR_BOOT_INFO_GET_FILE_OPEN,
    PROPERTY_ERR_BOOT_INFO_GET_FILE_READ,
    PROPERTY_ERR_BOOT_INFO_GET_FILE_CLOSE,
    PROPERTY_ERR_BOOT_INFO_SET_FILE_OPEN,
    PROPERTY_ERR_BOOT_INFO_SET_FILE_WRITE,
    PROPERTY_ERR_BOOT_INFO_SET_FILE_CLOSE,
    PROPERTY_ERR_DATA_RESET_SET_FLAG,
    PROPERTY_ERR_DATA_PREPARE_LOAD_FILE_BOOT,
    PROPERTY_ERR_DATA_PREPARE_CREATE_FILE_BOOT,
    PROPERTY_ERR_DATA_PREPARE_SET_BOOT_COUNT,
    PROPERTY_ERR_DATA_PREPARE_UPDATE_BOOT_COUNT,
    PROPERTY_ERR_DATA_PREPARE_COPY_FILE,
    PROPERTY_ERR_DATA_PREPARE_SET_KEY_RESET_FLAG,
    PROPERTY_ERR_DATA_PREPARE_SET_KEY_RUN_MODE,
    PROPERTY_ERR_DATA_LOAD,
    PROPERTY_ERR_DATA_CREATE,
    PROPERTY_ERR_DATA_LOAD_FACTORY,
    PROPERTY_ERR_DATA_CREATE_FACTORY,
    PROPERTY_ERR_DATA_LOAD_ROM,
    PROPERTY_ERR_DATA_CREATE_ROM,
    PROPERTY_ERR_DATA_NEW_RAM_RW,
    PROPERTY_ERR_DATA_NEW_RAM_RO,
    PROPERTY_ERR_DATA_UPDATE_INI_INVALIDE_ID,
    PROPERTY_ERR_DATA_UPDATE_INI,
    PROPERTY_ERR_DATA_UPDATE,
    PROPERTY_ERR_DATA_SAVE,
    PROPERTY_ERR_DATA_SAVE_FACTORY,
    PROPERTY_ERR_DATA_SAVE_ROM,
    PROPERTY_ERR_DATA_TRANS,
    PROPERTY_ERR_DATA_BACKUP_NULL,
    PROPERTY_ERR_DATA_BACKUP_FILE,
    PROPERTY_ERR_DATA_BACKUP_FLASH,
    PROPERTY_ERR_DATA_SYNC_FILE_SYNC,
    PROPERTY_ERR_DATA_SYNC_FILE_RESTORE,
    PROPERTY_ERR_DATA_SYNC_FILE_FLASH_SYNC,
    PROPERTY_ERR_DATA_SYNC_FILE_FLASH_RESTORE,
    PROPERTY_ERR_TASK_EVENT_SET_KEY_DATA_UPDATE,
    PROPERTY_ERR_TASK_EVENT_TIMEOUT_DATA_BACKUP,
    PROPERTY_ERR_TASK_EVENT_NOT_SUPPORT,
    PROPERTY_ERR_INIT_DATA_SYNC,
    PROPERTY_ERR_INIT_DATA_PREPARE,
    PROPERTY_ERR_INIT_DATA_LOAD,
    PROPERTY_ERR_GET_INI,
    PROPERTY_ERR_ON_REBOOT_SET_REBOOT_COUNT,
    PROPERTY_ERR_ON_REBOOT_UPDATE_REBOOT_COUNT,
    PROPERTY_ERR_ON_REBOOT_DATA_SAVE,
    PROPERTY_ERR_ON_REBOOT_DATA_BACKUP,
    PROPERTY_ERR_CHECK_GET_PERMISSION,
    PROPERTY_ERR_CHECK_SET_PERMISSION,
    PROPERTY_ERR_CHECK_SET_PERMISSION_RO,
    PROPERTY_ERR_CHECK_SET_PERMISSION_ROM,
    PROPERTY_ERR_GET_INI_NULL,
    PROPERTY_ERR_GET_INI_NO_SECTION,
    PROPERTY_ERR_GET_KEY_BYTE_ARRAY_COUNT_ZERO,
    PROPERTY_ERR_SET_EFUSE_ROM,
    PROPERTY_ERR_DUMP_SYNTAX,
    PROPERTY_ERR_DUMP_MEMCPY,
    PROPERTY_ERR_SEND_EVENT_TIMEOUT,
    PROPERTY_ERR_SEND_EVENT_SET_KEY,
    PROPERTY_ERR_SEND_EVENT_SAVE_KEY,
    PROPERTY_ERR_DEINIT_DELETE_MESSAGE_QUEUE,
    PROPERTY_EMG,
    PROPERTY_EMG_TASK_NEW_TIMER,
    PROPERTY_EMG_INIT_CREATE_TASK,
    PROPERTY_EMG_INIT_CREATE_MESSAGE_QUEUE,
    PROPERTY_ERR_RESERVED
} property_help_code_t;

int32_t property_get_int(uint16_t key, int32_t dval);
bool property_get_bool(uint16_t key, bool dval);
uint8_t* property_get_bytes(uint16_t key, uint8_t *values, uint16_t vlen);

errcode_t property_set_int(uint16_t key, int32_t value);
errcode_t property_set_bool(uint16_t key, bool value);
errcode_t property_set_bytes(uint16_t key, uint8_t *values, uint16_t vlen);
errcode_t property_set_bytes_with_notify(uint16_t key, uint8_t *values, uint16_t vlen);

errcode_t property_set_encrypted_bytes(uint16_t key, uint8_t *values, uint16_t vlen);
uint8_t *property_get_encrypted_bytes(uint16_t key, uint8_t *values, uint16_t vlen);

errcode_t property_set_non_upgrade_bytes(uint16_t key, uint8_t *values, uint16_t vlen);
errcode_t property_set_non_upgrade_int(uint16_t key, int32_t value);
errcode_t property_set_non_upgrade_bool(uint16_t key, bool value);

errcode_t property_reset(void);

#ifdef __cplusplus
}
#endif

#endif
