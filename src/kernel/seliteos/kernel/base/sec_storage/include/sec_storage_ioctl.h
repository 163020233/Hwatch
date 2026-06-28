/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: sec_storage ioctl header for SELiteOS BOX.
 */

#ifndef SEC_STORAGE_IOCTL_H
#define SEC_STORAGE_IOCTL_H

#include <errcode.h>
#include "sec_storage.h"

#define IOC_TYPE_SEC_STORAGE 'v'

typedef enum {
    SEC_STORAGE_WRITE_NR,
    SEC_STORAGE_READ_NR,
    SEC_STORAGE_DELETE_NR,
    SEC_STORAGE_IOC_MAX_NR,
} sec_storage_ioc_nr_t;

typedef struct {
    uint16_t sec_storage_id;
    const uint8_t *kvalue;
    uint16_t kvalue_length;
} sec_storage_write_t;

typedef struct {
    uint16_t sec_storage_id;
    uint16_t kvalue_max_length;
    uint16_t kvalue_length;
    uint8_t *kvalue;
} sec_storage_read_t;

#define SEC_STORAGE_IOC_NA 0U
#define SEC_STORAGE_IOC_W  1U
#define SEC_STORAGE_IOC_R  2U
#define SEC_STORAGE_IOC_RW 3U

/* Ioctl definitions */
#define SEC_STORAGE_IOC(dir, type, nr, size) (((dir) << 30) | ((size) << 16) | ((type) << 8) | ((nr) << 0))

#define SEC_STORAGE_IO(nr) SEC_STORAGE_IOC(SEC_STORAGE_IOC_NA, IOC_TYPE_SEC_STORAGE, (nr), 0)
#define SEC_STORAGE_IOWR(nr, struct_name) \
    SEC_STORAGE_IOC(SEC_STORAGE_IOC_RW, IOC_TYPE_SEC_STORAGE, (nr), sizeof(struct_name))
#define SEC_STORAGE_IOW(nr, struct_name) \
    SEC_STORAGE_IOC(SEC_STORAGE_IOC_W, IOC_TYPE_SEC_STORAGE, (nr), sizeof(struct_name))

#define SEC_STORAGE_IOC_DIR(cmd)  (((cmd) >> 30) & 0x03)
#define SEC_STORAGE_IOC_TYPE(cmd) (((cmd) >> 8) & 0xFF)
#define SEC_STORAGE_IOC_NR(cmd)   (((cmd) >> 0) & 0xFF)
#define SEC_STORAGE_IOC_SIZE(cmd) (((cmd) >> 16) & 0x3FFF)

#define CMD_SEC_STORAGE_WRITE  SEC_STORAGE_IOWR(SEC_STORAGE_WRITE_NR, sec_storage_write_t)
#define CMD_SEC_STORAGE_READ   SEC_STORAGE_IOWR(SEC_STORAGE_READ_NR, sec_storage_read_t)
#define CMD_SEC_STORAGE_DELETE SEC_STORAGE_IOWR(SEC_STORAGE_DELETE_NR, uint16_t)

errcode_t LOS_SEC_STORAGE_IOCTL(uint32_t cmd, void *arg);

#endif
