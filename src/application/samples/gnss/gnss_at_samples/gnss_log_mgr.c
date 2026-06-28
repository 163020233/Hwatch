/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: gnss sample log mgr
 */

#include "gnss_log_mgr.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "securec.h"
#include "errno.h"
#include "gnss_log.h"
#include "gnss_datatypes.h"

#define LOG_DIR_PATH                "/user/gnss/"
#define XGNSS_DATA_PATH             "/user/gnss/xgnss/"
#define LOG_BASED_PATH              "/user/gnss/log/"
#define GNSS_DATA_PATH              "/user/gnss/log/gnss_data"
#define GNSS_LOG_PATH               "/user/gnss/log/gnss"
#define LOG_FILE_NAME_LEN           100
#define BYTE_TO_MB                  1048576 // 1MB大小 1024 * 1024
#define DEFAULT_DATA_LOG_NUM        5
#define DEFAULT_DATA_LOG_SIZE       5 // 单位：MB
#define DEFAULT_LOG_NUM             5
#define DEFAULT_LOG_SIZE            5 // 单位：MB
#define LOG_POSTFIX_LEN             128
#define LOG_BUFFER_MAX_LEN          1024
#define GNSS_LOG_BUFF_LEN  16384  // 16k

typedef struct {
    uint32_t logNumLimit; // 日志最大保存个数
    uint32_t logSizeLimit; // 日志大小限制，单位：MB
    uint32_t logNum; // 当前日志个数
    uint32_t logSize; // 最新日志文件大小
    uint32_t usedBuffLen;
    char fileName[LOG_FILE_NAME_LEN]; // with index
    uint8_t buff[GNSS_LOG_BUFF_LEN];
} LogCtrlInfo;

static LogCtrlInfo g_logCtrlInfo = {0};
static LogCtrlInfo g_dataLogCtrlInfo = {0};

static void check_log_base_path(void)
{
    static const int32_t modetype = 0775; // 文件权限设置
    int32_t ret = access(LOG_DIR_PATH, F_OK);
    if (ret != 0) {
        ret = mkdir(LOG_DIR_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (ret != 0) {
            GNSS_LOG_ERROR("Failed to create path %s, errno:%s\n", LOG_DIR_PATH, strerror(errno));
            return;
        }

        if (chmod(LOG_DIR_PATH, modetype) < 0) {
            GNSS_LOG_WARNING("Failed to change file permission on path %s, errno:%s\n",
                LOG_DIR_PATH, strerror(errno));
        }
    }

    ret = access(LOG_BASED_PATH, F_OK);
    if (ret != 0) {
        ret = mkdir(LOG_BASED_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (ret != 0) {
            GNSS_LOG_ERROR("Failed to create path %s, errno:%s\n", LOG_BASED_PATH, strerror(errno));
            return;
        }

        if (chmod(LOG_BASED_PATH, modetype) < 0) {
            GNSS_LOG_WARNING("Failed to change file permission on path %s, errno:%s\n",
                LOG_BASED_PATH, strerror(errno));
        }
    }

    ret = access(XGNSS_DATA_PATH, F_OK);
    if (ret != 0) {
        ret = mkdir(XGNSS_DATA_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (ret != 0) {
            GNSS_LOG_ERROR("Failed to create path %s, errno:%s\n", XGNSS_DATA_PATH, strerror(errno));
            return;
        }

        if (chmod(XGNSS_DATA_PATH, modetype) < 0) {
            GNSS_LOG_WARNING("Failed to change file permission on path %s, errno:%s\n",
                XGNSS_DATA_PATH, strerror(errno));
        }
    }
}

errcode_t update_gnss_log_ctrl_info(uint32_t logNumLimit, uint32_t logSizeLimit)
{
    g_logCtrlInfo.logNumLimit = logNumLimit;
    g_logCtrlInfo.logSizeLimit = logSizeLimit;
    g_dataLogCtrlInfo.logNumLimit = logNumLimit;
    g_dataLogCtrlInfo.logSizeLimit = logSizeLimit;
    GNSS_LOG_DEBUG("[GNSS DEBUG] logNumLimit: %u, "
                   "logSizeLimit: %u\n",
                   g_logCtrlInfo.logNumLimit, g_logCtrlInfo.logSizeLimit);
    return ERRCODE_SUCC;
}

static void init_gnss_log_ctrl_file_info(LogCtrlInfo *logCtrl, const char* dataPath)
{
    char logName[LOG_FILE_NAME_LEN];
    for (uint32_t i = logCtrl->logNumLimit; i > 0; i--) {
        (void)memset_s(logName, sizeof(logName), 0, sizeof(logName));
        (void)sprintf_s(logName, LOG_FILE_NAME_LEN * sizeof(char), "%s_%u.log", dataPath, i);
        int32_t ret = access(logName, F_OK);
        if (ret == 0) {
            logCtrl->logNum = i; // 确认下当前日志文件数量
            break;
        }
    }
    (void)sprintf_s(logName, LOG_FILE_NAME_LEN * sizeof(char), "%s_1.log", dataPath);
    (void)memcpy_s(logCtrl->fileName, LOG_FILE_NAME_LEN * sizeof(char), logName, sizeof(logName));
    FILE* logFile = fopen(logCtrl->fileName, "ab");
    if (logFile == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR]error: open log file %s error\n", logCtrl->fileName);
        return;
    }
    int result = fseek(logFile, 0, SEEK_END);
    if (result != 0) {
        /* 处理错误，记录日志 */
        GNSS_LOG_ERROR("[GNSS ERROR] fseek failed, errno:%d\n", result);
    }
    logCtrl->logSize = (uint32_t)ftell(logFile);
    (void)fclose(logFile);
}

static uint32_t FindMaxLogIndex(uint32_t logNumLimit, const char* dataPath)
{
    uint32_t maxIndex = 0;
    char currentLogName[LOG_FILE_NAME_LEN];
    for (uint32_t i = logNumLimit; i > 0; i--) {
        (void)memset_s(currentLogName, sizeof(currentLogName), 0, sizeof(currentLogName));
        (void)sprintf_s(currentLogName, LOG_FILE_NAME_LEN * sizeof(char), "%s_%u.log", dataPath, i);
        if (access(currentLogName, F_OK) == 0) {
            maxIndex = i;
            break;
        }
    }
    return maxIndex;
}

static int RenameLogs(uint32_t maxIndex, const char* dataPath)
{
    for (uint32_t i = maxIndex; i > 0; i--) {
        char oldName[LOG_FILE_NAME_LEN];
        char newName[LOG_FILE_NAME_LEN];
        (void)memset_s(oldName, sizeof(oldName), 0, sizeof(oldName));
        (void)memset_s(newName, sizeof(newName), 0, sizeof(newName));
        (void)sprintf_s(oldName, LOG_FILE_NAME_LEN * sizeof(char), "%s_%u.log", dataPath, i);
        (void)sprintf_s(newName, LOG_FILE_NAME_LEN * sizeof(char), "%s_%u.log", dataPath, i + 1);

        if (rename(oldName, newName) != 0) {
            GNSS_LOG_ERROR("Failed to rename %s to %s, errno:%s", oldName, newName, strerror(errno));
            return -1;
        }
    }
    return 0;
}

static int DeleteOldestLog(uint32_t logNumLimit, const char* dataPath)
{
    char oldestLogName[LOG_FILE_NAME_LEN];
    (void)memset_s(oldestLogName, sizeof(oldestLogName), 0, sizeof(oldestLogName));
    (void)sprintf_s(oldestLogName, LOG_FILE_NAME_LEN * sizeof(char),
                    "%s_%d.log", dataPath, logNumLimit + 1);
    if (access(oldestLogName, F_OK) == 0) {
        if (remove(oldestLogName) != 0) {
            GNSS_LOG_ERROR("Failed to remove %s, errno: %s", oldestLogName, strerror(errno));
            return -1;
        }
    }
    return 0;
}

static int CreateNewLogFile(const char* dataPath, char* fileName)
{
    (void)sprintf_s(fileName, LOG_FILE_NAME_LEN * sizeof(char), "%s_1.log", dataPath);
    FILE* newLogFile = fopen(fileName, "ab");
    if (newLogFile == NULL) {
        GNSS_LOG_ERROR("Failed to create new log file %s, errno: %s", fileName, strerror(errno));
        return -1;
    }

    if (fclose(newLogFile) != 0) {
        GNSS_LOG_ERROR("Failed to close new log file %s, errno: %s", fileName, strerror(errno));
        return -1;
    }
    
    return 0;
}

static void gnss_check_log_size(LogCtrlInfo *logCtrl, const char* dataPath)
{
    if (logCtrl->logSize >= logCtrl->logSizeLimit * BYTE_TO_MB) {
        uint32_t maxIndex = FindMaxLogIndex(logCtrl->logNumLimit, dataPath);

        int renameResult = RenameLogs(maxIndex, dataPath);
        if (renameResult != 0) {
            GNSS_LOG_ERROR("Failed to rename files, log management aborted");
            return;
        }

        if (maxIndex >= logCtrl->logNumLimit) {
            int deleteResult = DeleteOldestLog(logCtrl->logNumLimit, dataPath);
            if (deleteResult != 0) {
                GNSS_LOG_ERROR("Failed to delete oldest log file, log management aborted");
                return;
            }
        }

        int createResult = CreateNewLogFile(dataPath, logCtrl->fileName);
        if (createResult != 0) {
            GNSS_LOG_ERROR("Failed to create new log file, log management aborted");
            return;
        }

        logCtrl->logSize = 0;
        logCtrl->logNum++;
    }
}

void init_gnss_log_ctrl_info(void)
{
    check_log_base_path();
    // gnss 日志
    g_logCtrlInfo.logNumLimit = DEFAULT_LOG_NUM;
    g_logCtrlInfo.logSizeLimit = DEFAULT_LOG_SIZE;
    g_logCtrlInfo.logNum = 1;
    g_logCtrlInfo.logSize = 0;
    g_logCtrlInfo.usedBuffLen = 0;
    init_gnss_log_ctrl_file_info(&g_logCtrlInfo, GNSS_LOG_PATH);
    gnss_check_log_size(&g_logCtrlInfo, GNSS_LOG_PATH);

    // 回放和nmea数据
    g_dataLogCtrlInfo.logNumLimit = DEFAULT_DATA_LOG_NUM;
    g_dataLogCtrlInfo.logSizeLimit = DEFAULT_DATA_LOG_SIZE;
    g_dataLogCtrlInfo.logNum = 1;
    g_dataLogCtrlInfo.logSize = 0;
    g_logCtrlInfo.usedBuffLen = 0;
    init_gnss_log_ctrl_file_info(&g_dataLogCtrlInfo, GNSS_DATA_PATH);
    gnss_check_log_size(&g_dataLogCtrlInfo, GNSS_DATA_PATH);
}

static void gnss_write_data(LogCtrlInfo *logCtrl, uint8_t *buff, uint32_t size)
{
    FILE* logFile = fopen(logCtrl->fileName, "ab");
    if (logFile == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR]error: open log file error\n");
        return;
    }

    size_t written = fwrite(buff, sizeof(uint8_t), size, logFile);
    if (written != size) {
        GNSS_LOG_ERROR("[GNSS ERROR]write binary fail, size is not equal, written:%d, msgLen:%d\n", written, size);
    }

    int result = fseek(logFile, 0, SEEK_END);
    if (result != 0) {
        /* 处理错误，记录日志 */
        GNSS_LOG_ERROR("[GNSS ERROR] fseek failed, error code: %d\n", result);
    }
    logCtrl->logSize = (uint32_t)ftell(logFile);
    (void)fseek(logFile, 0, SEEK_SET);

    (void)fclose(logFile);
}

static void update_gnss_log_data(LogCtrlInfo *logCtrl, uint8_t *buff, uint32_t size, char *path)
{
    uint32_t len = GNSS_LOG_BUFF_LEN - logCtrl->usedBuffLen;
    if (len < size) {
        // 写到文件中
        gnss_write_data(logCtrl, logCtrl->buff, logCtrl->usedBuffLen);
        gnss_check_log_size(logCtrl, path);
        // 从buff头开始记录新日志
        memset_s(logCtrl->buff, GNSS_LOG_BUFF_LEN, 0, GNSS_LOG_BUFF_LEN);
        memcpy_s(logCtrl->buff, GNSS_LOG_BUFF_LEN, buff, size);
        logCtrl->usedBuffLen = size;
    } else {
        // 存到buff中
        errno_t ret = memcpy_s(logCtrl->buff + logCtrl->usedBuffLen, len, buff, size);
        if (ret != EOK) {
            GNSS_LOG_ERROR("[GNSS ERROR]copy log buff fail\n");
            return;
        }
        logCtrl->usedBuffLen += size;
    }
}

void gnss_save_data(uint8_t *buff, uint32_t size)
{
    update_gnss_log_data(&g_dataLogCtrlInfo, buff, size, GNSS_DATA_PATH);
}

void gnss_save_log(uint8_t *buff, uint32_t size)
{
    update_gnss_log_data(&g_logCtrlInfo, buff, size, GNSS_LOG_PATH);
}

void flash_gnss_log(void)
{
    gnss_write_data(&g_dataLogCtrlInfo, g_dataLogCtrlInfo.buff, g_dataLogCtrlInfo.usedBuffLen);
    gnss_write_data(&g_logCtrlInfo, g_logCtrlInfo.buff, g_logCtrlInfo.usedBuffLen);

    g_dataLogCtrlInfo.usedBuffLen = 0;
    memset_s(g_dataLogCtrlInfo.buff, GNSS_LOG_BUFF_LEN, 0, GNSS_LOG_BUFF_LEN);

    g_logCtrlInfo.usedBuffLen = 0;
    memset_s(g_logCtrlInfo.buff, GNSS_LOG_BUFF_LEN, 0, GNSS_LOG_BUFF_LEN);
}

uint32_t AssembleLog(char *targetBuf, const char* logLevel, const char* fmt, va_list va)
{
    int32_t targetLen = 0;
    int32_t availableLen = LOG_BUFFER_MAX_LEN;

    // 1、组装时间和级别
    GnssUtcTime utcTime;
    get_sys_utc_time(&utcTime);
    int32_t timeLen = sprintf_s(targetBuf, LOG_BUFFER_MAX_LEN, "%02d-%02d-%02d.%03d %s: ",
        utcTime.hour, utcTime.minute, utcTime.second, utcTime.ms, logLevel);
    if (timeLen == -1) {
        return 0;
    }
    targetLen += timeLen;
    availableLen -= timeLen;

    // 2、组装log正文
    int32_t bodyLen = vsnprintf_s(targetBuf + targetLen, (uint32_t)availableLen,
        (uint32_t)(availableLen - 3), fmt, va);
    if (bodyLen == -1) {
        return 0;
    }
    targetLen += bodyLen;
    availableLen -= bodyLen;
    
    // 3、组装换行符
    int32_t newlineLen = snprintf_s(targetBuf + targetLen, (uint32_t)availableLen, 2, "\r\n");
    if (newlineLen == -1) {
        return 0;
    }
    targetLen += newlineLen;
    return targetLen;
}

void output_log(const char* logLevel, const char* fmt, ...)
{
    char targetBuf[LOG_BUFFER_MAX_LEN] = { 0 };
    va_list va;
    va_start(va, fmt);
    uint32_t len = AssembleLog(targetBuf, logLevel, fmt, va);
    va_end(va);
    if (len == 0) {
        return;
    }

    gnss_save_log((uint8_t *)targetBuf, len);
}
