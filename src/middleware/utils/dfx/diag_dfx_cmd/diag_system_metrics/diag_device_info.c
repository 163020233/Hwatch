/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: diag device info
 * This file should be changed only infrequently and with great care.
 */

#include <sys/stat.h>
#include "fcntl.h"
#include "unistd.h"
#include "exception.h"
#include "crash_data.h"
#include "preserve.h"
#include "dfx_adapt_layer.h"
#include "soc_diag_cmd_id.h"
#include "diag_data_store.h"
#include "diag_device_info.h"

#define MAX_CRASH_FILE_SIZE                 512
#define CRASH_PATH_MAX_LEN                  50

static bool g_crash_sent = false; // 是否上报过异常信息
static bool g_boot_sent = false; // 是否上报过启动信息
static const char* APP_EXC_INFO_BIN_PATH = "/user/exc";
static const char* APP_EXC_INFO_BIN_NAME = "app_exc_info.bin";

static diag_device_info_t *g_diag_device_info = NULL;

// 构建设备信息
static errcode_t dfx_build_device_info(void)
{
    if (g_diag_device_info == NULL) {
        return ERRCODE_FAIL;
    }

    // 重启信息
    if (g_boot_sent) {
        g_diag_device_info->reboot_cause = 0xFFFFU;
        g_diag_device_info->reboot_count = 0xFFFFU;
        g_diag_device_info->exception_reboot_count = 0xFFFFU;
    } else {
        g_diag_device_info->reboot_cause = (uint16_t)get_cpu_utils_reset_cause();
        g_diag_device_info->reboot_count = (uint16_t)get_cpu_utils_reset_count();
        g_diag_device_info->exception_reboot_count = (uint16_t)get_cpu_utils_epreset_count();
    }
    return ERRCODE_SUCC;
}

void diag_device_info_clear(void)
{
    if (g_diag_device_info != NULL) {
        dfx_free(0, g_diag_device_info);
        g_diag_device_info = NULL;
    }
}

errcode_t diag_device_info_set(const diag_device_model_t* device_model)
{
    if (device_model == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    if (g_diag_device_info == NULL) {
        g_diag_device_info = (diag_device_info_t *)dfx_malloc(0, sizeof(diag_device_info_t));
        if (g_diag_device_info == NULL) {
            return ERRCODE_MALLOC;
        }
        if (memset_s(g_diag_device_info, sizeof(diag_device_info_t), 0, sizeof(diag_device_info_t)) != 0) {
            diag_device_info_clear();
            return ERRCODE_MEMSET;
        }
    }

    if (memcpy_s(&g_diag_device_info->device_model, sizeof(g_diag_device_info->device_model),
        device_model, sizeof(g_diag_device_info->device_model)) != 0) {
        dfx_log_err("[DIAG_REPORT]Device info set fail!");
        diag_device_info_clear();
        return ERRCODE_MEMCPY;
    }
    return ERRCODE_SUCC;
}

errcode_t diag_device_info_get(diag_device_info_t* device_info)
{
    if (device_info == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    if (g_diag_device_info == NULL) {
        return ERRCODE_FAIL;
    }

    if (memcpy_s(device_info, sizeof(diag_device_info_t), g_diag_device_info, sizeof(diag_device_info_t)) != 0) {
        dfx_log_err("[DIAG_REPORT]Device info get fail!\n");
        return ERRCODE_MEMCPY;
    }
    return ERRCODE_SUCC;
}

// 上报设备信息
errcode_t diag_device_info_report(void)
{
    errcode_t ret;
    ret = dfx_build_device_info();
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("[DIAG_REPORT]Device info build fail!\n");
        return ret;
    }

    diag_data_store_ctx_t store_ctx = {
        .enable_timestamp = true,
    };
    // 设备信息写入关键数据存储
    ret = diag_data_store_critical_write(DIAG_CMD_DIAG_REPORT_DEVICE_INFO, g_diag_device_info,
                                         sizeof(diag_device_info_t), &store_ctx);
    if (ret == ERRCODE_SUCC) {
        g_boot_sent = true;
    }
    return ret;
}

/* 读取bin文件指定区域 */
static errcode_t load_bin_file(const char *path, uint16_t offset, uint16_t size, uint8_t *out_buf)
{
    int fd;
    struct stat st;
    // 1. 获取文件信息
    if (stat(path, &st) != 0) {
        perror("stat failed");
        return ERRCODE_FAIL;
    }

    // 2. 越界检查
    if ((uint32_t)offset + (uint32_t)size > st.st_size) {
        dfx_log_err("read out of range");
        return ERRCODE_FAIL;
    }

    // 3. 打开文件
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        return ERRCODE_FAIL;
    }

    // 4. 定位
    if (lseek(fd, offset, SEEK_SET) < 0) {
        perror("lseek failed");
        close(fd);
        return ERRCODE_FAIL;
    }

    // 5. 读取
    ssize_t ret = read(fd, out_buf, size);
    if (ret != size) {
        perror("read failed");
        close(fd);
        return ERRCODE_FAIL;
    }
    close(fd);
    return ERRCODE_SUCC;
}

// 处理异常文件, 加载并上报
static void process_crash_file(const char* file_path, uint16_t offset, uint16_t size, uint16_t cmd_id)
{
    struct stat st;
    // 检查文件是否存在
    if (file_path == NULL || stat(file_path, &st) != 0) {
        dfx_log_err("[DIAG_REPORT] process_crash_file file not exist");
        return;
    }
 
    if (size >= MAX_CRASH_FILE_SIZE) {
        dfx_log_err("[DIAG_REPORT] process_crash_file size too big, size = %u", size);
        return;
    }

    diag_data_store_ctx_t store_ctx = {
        .enable_timestamp = true,
    };
    
    // 1. 加载文件
    uint8_t* data = dfx_malloc(0, size);
    if (data == NULL) {
        dfx_log_err("[DIAG_REPORT] process_crash_file malloc failed for size: %u", size);
        return;
    }

    errcode_t ret = load_bin_file(file_path, offset, size, data);
    // 2. 如果加载成功，则上报
    if (ret == ERRCODE_SUCC) {
        // 关联 cmd_id 并调用上报接口
        // crash信息写入关键数据存储
        ret = diag_data_store_critical_write(cmd_id, data, size, &store_ctx);
        dfx_log_err("[DIAG_REPORT] process_crash_file file exist,cmd_id = 0x%x, ret = %d", cmd_id, ret);
    }
    dfx_free(0, data);
    data = NULL;
}

static void report_single_crash(const char* file_path)
{
    if (file_path == NULL) {
        return;
    }
    process_crash_file(file_path,
                       0,
                       PRESERVED_REGION_LENGTH,
                       DIAG_CMD_DIAG_REPORT_BT_CRASH);
    process_crash_file(file_path,
                       PRESERVED_REGION_LENGTH,
                       APP_PRESERVED_REGION_LENGTH,
                       DIAG_CMD_DIAG_REPORT_BT_CRASH);
    process_crash_file(file_path,
                       PRESERVED_REGION_LENGTH + APP_PRESERVED_REGION_LENGTH + offsetof(crash_data_t, exc_info_save),
                       APP_PRESERVED_REGION_LENGTH,
                       DIAG_CMD_DIAG_REPORT_APP_CRASH);

    if (unlink(file_path) != 0) {
        perror("unlink crash failed");
    }
}

// 上报异常信息
errcode_t diag_crash_info_report(void)
{
    if (g_crash_sent) {
        return ERRCODE_SUCC;
    }

#if defined (CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM) && (CONFIG_DFX_EXCEPTION_SAVE_ON_FILESYSTEM == DFX_YES)
    char file_path[CRASH_PATH_MAX_LEN] = {0};
#if defined (CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS) && (CONFIG_DFX_SUPPORT_MULTIPLE_CRASH_RECOREDS == DFX_YES)
    for (int i = 0; i < DUMP_INFO_MAX_NUM; i++) {
        if (snprintf_s(file_path, sizeof(file_path), sizeof(file_path) - 1, "%s%s_%d/%s", APP_EXC_INFO_BIN_PATH,
                       CRASH_DATA_EXE_FILE_PREFIX, i, APP_EXC_INFO_BIN_NAME) < 0) {
            continue;
        }

        report_single_crash(file_path);
    }
#else
    if (snprintf_s(file_path, sizeof(file_path), sizeof(file_path) - 1,
                   "%s%s", APP_EXC_INFO_BIN_PATH, APP_EXC_INFO_BIN_NAME) < 0) {
        return ERRCODE_FAIL;
    }
    report_single_crash(file_path);
#endif
#endif
    g_crash_sent = true;
    return ERRCODE_SUCC;
}