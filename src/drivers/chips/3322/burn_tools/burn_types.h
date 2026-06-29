/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Provides burn uart port template \n
 * History: \n
 * 2022-06-06， Create file. \n
 */
#ifndef BURN_TYPES_H
#define BURN_TYPES_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t s32;

#ifndef BURN_MAX_FILES
#define BURN_MAX_FILES 10
#endif

#ifndef BURN_MAX_PATH
#define BURN_MAX_PATH 180
#endif

#ifndef BURN_RX_BUF_SIZE
#define BURN_RX_BUF_SIZE    (2 * 1024)
#endif

#ifndef BURN_CMD_BUF_SIZE
#define BURN_CMD_BUF_SIZE 256
#endif

#define BURN_UNUSED(x) (void)(x)

/* YModem constants */
enum {
    MODEM_SOH = 0x01,
    MODEM_STX = 0x02,
    MODEM_STXA = 0x0a,
    MODEM_STXB = 0x0b,
    MODEM_STXC = 0x0c,
    MODEM_EOT = 0x04,
    MODEM_ACK = 0x06,
    MODEM_NAK = 0x15,
    MODEM_CAN = 0x18,
    MODEM_C = 0x43,
};

enum {
    MODEM_PACKET_SIZE = 1024,
    MODEM_PACKET_SIZEA = 2048,
    MODEM_PACKET_SIZEB = 4096,
    MODEM_PACKET_SIZEC = 8192,
};

/* Error codes aligned with original values where relevant */
enum {
    BURN_ERR_OK = 0,
    BURN_ERR_INVALID_PARAM = 2,
    BURN_ERR_BUSY = 8,
    BURN_ERR_FILE_NOT_FOUND = 51,
    BURN_ERR_FILE_ACCESS_FAIL = 52,
    BURN_ERR_OPEN_FILE = 16,
    BURN_ERR_COM_INVALID = 17,
    BURN_ERR_TIMEOUT_WAIT_HISILICON = 1,
    BURN_ERR_TIMEOUT_WAIT_RANDOM = 2,
    BURN_ERR_TIMEOUT_WAIT_SEC_RESULT = 3,
    BURN_ERR_TIMEOUT_WAIT_BURN_START_C = 4,
    BURN_ERR_TIMEOUT_WAIT_BURN_INIT_ACK = 5,
    BURN_ERR_TIMEOUT_WAIT_BURN_INTER_ACK = 6,
    BURN_ERR_TIMEOUT_WAIT_BURN_EOT_C = 7,
    BURN_ERR_TIMEOUT_WAIT_ZERO_ACK = 8,
    BURN_ERR_TIMEOUT_WAIT_SELOADER_ACK = 14,
    BURN_ERR_SELOADER_ACK_IS_FAIL = 15,
    BURN_ERR_WAITING = 0xfffffffd,
    BURN_ERR_DOING = 0xfffffffe,
    BURN_ERR_FAILURE = 0xfffffff0
};

/* FSM states (subset used by CLI) */
typedef enum {
    FSM_NONE = 0,
    FSM_OPEN_PORT,
    FSM_WAIT_SHAKE_HANDS_ACK,
    FSM_WAIT_FLASH_SHAKE_HANDS_ACK,
    FSM_WAIT_INTERRUPT_FLAG,
    FSM_WAIT_RANDOM,
    FSM_WAIT_SEC_RESULT,
    FSM_WORK,
    FSM_CHECK_IF_CONNECT,
    FSM_BURN_WAIT_START_C,
    FSM_BURN_WAIT_INIT_ACK,
    FSM_BURN_WAIT_INTER_ACK,
    FSM_BURN_WAIT_FINAL_ACK,
    FSM_BURN_WAIT_EOT_C,
    FSM_BURN_WAIT_ZERO_ACK,
    FSM_BURN_WAIT_SELOADR_END_ACK,
    FSM_BURN_UPLOAD_START_C,
    FSM_BURN_UPLOAD_INIT_ACK,
    FSM_BURN_UPLOAD_EOT_C,
    FSM_BURN_UPLOAD_END_ACK,
    FSM_BURN_WAIT_SELOADR_UPLOAD_ACK,
    FSM_BURN_WAIT_SELOADR_DOWN_ACK,
    FSM_BURN_WAIT_SELOADR_READ_EFUSE_ACK,
    FSM_BURN_WAIT_SELOADR_LOCK_ACK,
    FSM_BURN_WAIT_SELOADR_RESET_ACK,
    FSM_BURN_WAIT_SELOADR_CHANGE_BAUD,
    FSM_BURN_WAIT_MAX,
} burn_fsm_t;

typedef enum {
    BOOT_ROM = 0,
    BOOT_LOADER = 1
} boot_type_t;

typedef enum {
    IMAGE_TYPE_LOADER = 0,
    IMAGE_TYPE_NORMAL = 1,
    IMAGE_TYPE_KV_NV = 2,
    IMAGE_TYPE_EFUSE = 3,
    IMAGE_TYPE_OTP = 4,
    IMAGE_TYPE_FLASHBOOT = 5,
    IMAGE_TYPE_FACTORY = 6,
    IMAGE_TYPE_VERSION = 7,
    IMAGE_TYPE_SECURITY_A = 8,
    IMAGE_TYPE_SECURITY_B = 9,
    IMAGE_TYPE_SECURITY_C = 10,
    IMAGE_TYPE_PROTOCOL_A = 11,
    IMAGE_TYPE_APPS_A = 12,
    IMAGE_TYPE_RADIOCONFIG = 13,
    IMAGE_TYPE_ROM = 14,
    IMAGE_TYPE_EMMC = 15,
    IMAGE_TYPE_DATABASE = 16,
    IMAGE_TYPE_FLASHBOOT_3892 = 17,
} image_type_tt;

typedef struct {
    char path[BURN_MAX_PATH];
    char file_name[80];
    u32 file_index;
    u32 file_size;
    u32 burn_addr;
    u32 burn_size;
    image_type_tt type;
} bin_info_t;

#ifdef __cplusplus
}
#endif

#endif /* BURN_TYPES_H */
