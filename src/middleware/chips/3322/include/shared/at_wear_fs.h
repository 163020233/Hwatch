/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Provides fs test api
 */

#ifndef _APP_TEST_FS_H
#define _APP_TEST_FS_H

#include <stdint.h>
#include <stdio.h>
#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define TEST_PARA_2             2
#define BYTES_LEN_128           128
#define BYTES_LEN_512           512
#define BYTES_LEN_1K            1024
#define BYTES_LEN_2K            2048
#define BYTES_LEN_4K            4096
#define BYTES_LEN_8K            8192
#define BUFF_LEN_48K            (48 * 1024)
#define BUFF_LEN_64K            (64 * 1024)
#define FS_TEST_1M_BYTES        1048576
#define FS_TEST_100M_BYTES      104857600
#define MAX_FILE_PATH_SIZE      25
#define TCXO_ERROR              40
#define BYTES_LEN_128K          (128 * 1024)

#define FS_PARTITION_NAME_SYSTEM "/system"
#define FS_PARTITION_NAME_USER "/user"
#define FS_PARTITION_NAME_UPDATE "/update"
#define FS_PARTITION_NAME_MUSIC "/music"

typedef struct {
    uint32_t randm;
    uint32_t rw_bytes;
} emmc_test_param;

typedef struct {
    uint32_t             para_map;
    char                *name;
    uint32_t             len;
} fs_write_check_args_t;

typedef struct boot_nand_file_desc {
    const char *name;
    int32_t start;
} boot_nand_file_desc;

at_ret_t wear_fs_format(void);
at_ret_t wear_fs_info(void);
at_ret_t wear_fs_rw_test(void);
uint32_t fs_test_rw_speed(const char* path, uint16_t width);
at_ret_t wear_fs_boot_nand_write(void);
void nand_erase_force(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _APP_TEST_FS_H */

