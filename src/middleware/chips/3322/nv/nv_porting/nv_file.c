/*================================================================
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
*
* Description: NV on different filesystem
* Author: @CompanyNameTag
* History:
* 2025-10-31， Create file.
================================================================*/

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "sys/statfs.h"
#include "memory_config.h"
#include "nv_porting.h"
#include "soc_osal.h"
#include "std_def.h"
#include "dfx_file_operation.h"

int32_t kv_file_read(const char *path, uint32_t offset, uint8_t *buf, uint32_t size)
{
    int len;
    int ret;
    char *real_path = NULL;

    real_path = realpath(path, NULL);
    if (real_path == NULL) {
        nv_log_err("[NV] error:realpath failed, errno is %d. \r\n", get_errno());
        return -1;
    }

    int fd = open(real_path, O_RDONLY);
    free(real_path);
    if (fd < 0) {
        nv_log_err("[NV] error:open failed, errno is %d. \r\n", get_errno());
        return -1;
    }

    ret = (int)lseek(fd, offset, SEEK_SET);
    if (ret < 0) {
        nv_log_err("[NV] error:lseek failed, errno is %d. \r\n", get_errno());
    }
    len = read(fd, buf, size);
    if (len <= 0) {
        nv_log_err("[NV] error:read failed, errno is %d. \r\n", get_errno());
    }
    ret = close(fd);
    if (ret < 0) {
        nv_log_err("[NV] error:close failed, errno is %d. \r\n", get_errno());
    }

    return len;
}

int32_t kv_file_write(const char *path, uint32_t offset, const uint8_t *buf, uint32_t size)
{
    int fd;
    int ret;
    ssize_t len;

    char *real_path = realpath(path, NULL);
    if (real_path == NULL) {
        return -1;
    }

    fd = open(real_path, O_RDWR | O_CREAT, 0666); /* 0666: open file with rw-rw-rw- mode */
    if (fd < 0) {
        if (dfx_file_mkdir(real_path) != 0) {
            free(real_path);
            nv_log_err("[NV] error:dfx_file_mkdir failed. \r\n");
            return -1;
        }
        fd = open(real_path, O_RDWR | O_CREAT, 0666); /* 0666: open file with rw-rw-rw- mode */
        if (fd < 0) {
            free(real_path);
            nv_log_err("[NV] error:open failed, errno is %d. \r\n", get_errno());
            return -1;
        }
    }

    int pos = (int)lseek(fd, offset, SEEK_SET);
    if (pos < 0) {
        nv_log_err("[NV] error:lseek failed, errno is %d. \r\n", get_errno());
    }

    len = write(fd, buf, size);
    if (len <= 0) {
        nv_log_err("[NV] error:write failed, errno is %d. \r\n", get_errno());
    }

    ret = close(fd);
    if (ret < 0) {
        nv_log_err("[NV] error:close failed, errno is %d. \r\n", get_errno());
    }

    free(real_path);
    return len;
}
