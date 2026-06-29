/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: chips ioctl header for SELiteOS.
 */

#ifndef __CHIPS_IOCTL_H__
#define __CHIPS_IOCTL_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    uint32_t byte_address;
    uint8_t value;
} efuse_read_byte_arg_t;

typedef struct {
    uint32_t byte_address;
    uint8_t value;
    uint32_t region;
} efuse_write_op_arg_t;

typedef struct {
    uint32_t address;
    uint8_t *buffer;
    uint16_t length;
} efuse_write_buffer_op_arg_t;

typedef struct {
    uint8_t *buffer;
    uint16_t length;
} efuse_get_die_id_arg_t;

typedef struct {
    uint32_t *version;
} get_tee_ver_arg_t;

typedef struct {
    uint32_t *key_ver;
    uint32_t *code_ver;
} get_key_code_ver_arg_t;

typedef enum {
    TESVEC_EFUSE_READ_BYTE_NR,
    TESVEC_EFUSE_WRITE_OP_NR,
    TESVEC_EFUSE_WRITE_BUFFER_OP_NR,
    TESVEC_EFUSE_GET_DIE_ID_NR,
    TESVEC_READ_TEE_VER_NR,
    TESVEC_READ_TEE_KEY_CODE_VER_NR,
    TESVEC_READ_SSB_KEY_CODE_VER_NR,
    CHIPS_PORTING_IOC_MAX_NR,
} chips_porting_ioc_nr_t;

typedef struct {
    uint16_t efuse_addr;
    uint16_t efuse_addr_len;
} efuse_range_t;

#define IOC_TYPE_CHIPS_PORTING 'a'
#define CHIPS_PORTING_IOC_NA   0U
#define CHIPS_PORTING_IOC_W    1U
#define CHIPS_PORTING_IOC_R    2U
#define CHIPS_PORTING_IOC_RW   3U

#define CHIPS_PORTING_IOC(dir, type, nr, size) (((dir) << 30) | ((size) << 16) | ((type) << 8) | ((nr) << 0))

#define CHIPS_PORTING_IO(nr) CHIPS_PORTING_IOC(CHIPS_PORTING_IOC_NA, IOC_TYPE_CHIPS_PORTING, (nr), 0)
#define CHIPS_PORTING_IOWR(nr, struct_name) \
    CHIPS_PORTING_IOC(CHIPS_PORTING_IOC_RW, IOC_TYPE_CHIPS_PORTING, (nr), sizeof(struct_name))
#define CHIPS_PORTING_IOW(nr, struct_name) \
    CHIPS_PORTING_IOC(CHIPS_PORTING_IOC_W, IOC_TYPE_CHIPS_PORTING, (nr), sizeof(struct_name))

#define CMD_EFUSE_READ_BYTE          CHIPS_PORTING_IOWR(TESVEC_EFUSE_READ_BYTE_NR, efuse_read_byte_arg_t)
#define CMD_EFUSE_WRITE_OP           CHIPS_PORTING_IOW(TESVEC_EFUSE_WRITE_OP_NR, efuse_write_op_arg_t)
#define CMD_EFUSE_WRITE_BUFFER_OP    CHIPS_PORTING_IOW(TESVEC_EFUSE_WRITE_BUFFER_OP_NR, efuse_write_buffer_op_arg_t)
#define CMD_EFUSE_GET_DIE_ID         CHIPS_PORTING_IOWR(TESVEC_EFUSE_GET_DIE_ID_NR, efuse_get_die_id_arg_t)
#define CMD_READ_TEE_VER             CHIPS_PORTING_IOWR(TESVEC_READ_TEE_VER_NR, get_tee_ver_arg_t)
#define CMD_READ_TEE_KEY_CODE_VER_NR CHIPS_PORTING_IOWR(TESVEC_READ_TEE_KEY_CODE_VER_NR, get_key_code_ver_arg_t)
#define CMD_READ_SSB_KEY_CODE_VER_NR CHIPS_PORTING_IOWR(TESVEC_READ_SSB_KEY_CODE_VER_NR, get_key_code_ver_arg_t)

#define EFUSE_LIFECYCLE_ADDR     204
#define EFUSE_LIFECYCLE_ADDR_LEN 1
#define FLASHBOOT_VER_ADDR       206
#define FLASHBOOT_VER_ADDR_LEN   2
#define EFUSE_TEE_VER_ADDR       280
#define EFUSE_TEE_VER_ADDR_LEN   2
#define EFUSE_DIE_ID_LENGTH      20

int32_t dispatch_efuse_read_byte(uint32_t cmd, void *arg);

int32_t dispatch_efuse_write_op(uint32_t cmd, void *arg);

int32_t dispatch_efuse_write_buffer_op(uint32_t cmd, void *arg);

int32_t dispatch_efuse_get_die_id(uint32_t cmd, void *arg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CHIPS_IOCTL_H__ */
