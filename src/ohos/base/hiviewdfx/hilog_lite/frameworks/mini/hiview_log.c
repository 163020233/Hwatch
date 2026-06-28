/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "ohos_init.h"
#include "hiview_def.h"
#include "hiview_util.h"
#include "hiview_config.h"
#include "hiview_service.h"
#include "hiview_log.h"
#include "hiview_log_limit.h"
#include "hiview_output_log.h"
#include <time.h>
#include "securec.h"

#define LOG_IS_OUTPUT(mod) (g_hiviewConfig.logOutputModule & (((uint64_t)1) << (mod)))
#define TIME_BUF_LEN 32
#define MAX_LOG_LENGTH 256
#define TRUNCATION_MARKER_LENGTH 4
static HiLogModuleInfo g_logModuleInfo[HILOG_MODULE_MAX];
const char * const FUN_ARG_S = "0123456I";

static boolean CheckParameters(uint8 module, uint8 level);

/* The first step does not involve memory allocation. */
static void HiLogInit(void)
{
    HIVIEW_UartPrint("hilog will init.\n");
    InitCoreLogOutput();

    /* The module that is not registered cannot print the log. */
    if (HiLogRegisterModule(HILOG_MODULE_HIVIEW, "HIVIEW") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_SAMGR, "SAMGR") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_UPDATE, "UPDATE") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_ACE, "ACE") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_AAFWK, "AAFWK") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_APP, "APP") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_GRAPHIC, "GRAPHIC") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_MEDIA, "MEDIA") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_DMS, "DMS") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_SEN, "SEN") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_SCY, "SCY") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_SOFTBUS, "SOFTBUS") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_POWERMGR, "POWERMGR") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_UIKIT, "UIKIT") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_GLOBAL, "GLOBAL") == FALSE ||
        HiLogRegisterModule(HILOG_MODULE_DATAMGR, "DATAMGR") == FALSE) {
        return;
    }
#ifndef LITEWEARABLE_SUPPORT_ENABLED
    HiviewRegisterInitFunc(HIVIEW_CMP_TYPE_LOG, InitLogOutput);
    HiviewRegisterInitFunc(HIVIEW_CMP_TYPE_LOG_LIMIT, InitLogLimit);
#endif
    HILOG_DEBUG(HILOG_MODULE_HIVIEW, "hilog init success.");
}
CORE_INIT_PRI(HiLogInit, 0);

static boolean CheckParameters(uint8 module, uint8 level)
{
    if ((level < g_hiviewConfig.level) ||  (level < HILOG_COMPILE_LEVEL) ||
        (module >= HILOG_MODULE_MAX) || (g_logModuleInfo[module].name == NULL)) {
        return FALSE;
    }

    return TRUE;
}

boolean HiLogRegisterModule(uint16 id, const char *name)
{
    if ((id >= HILOG_MODULE_MAX) || name == NULL || g_logModuleInfo[id].name != NULL) {
        return FALSE;
    }

    uint32 len = (uint32)strnlen(name, LOG_MODULE_NAME_LEN + 1);
    if (len >= LOG_MODULE_NAME_LEN - 1) {
        return FALSE;
    }
    uint32 i = 0;
    while (i < len && name[i] != 0) {
        if (!((name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z'))) {
            return FALSE;
        }
        i++;
    }

    g_logModuleInfo[id].name = name;
    g_logModuleInfo[id].id = id;
    return TRUE;
}

const char *HiLogGetModuleName(uint8 id)
{
    if (id >= HILOG_MODULE_MAX) {
        return "";
    }
    const char *name = g_logModuleInfo[id].name;
    return (name == NULL) ? "" : name;
}

void HiLogPrintf(uint8 module, uint8 level, const char *nums, const char *fmt, ...)
{
    static char newFmt[] = "The number of parameters is invalid.";
    HiLogContent logContent = { 0 };
    int32 argsNum = (nums - FUN_ARG_S);
    if (argsNum < 0 || argsNum > LOG_MULTI_PARA_MAX) {
        fmt = newFmt;
        argsNum = 0;
    }

    if (g_hiviewConfig.logSwitch == HIVIEW_FEATURE_OFF || !CheckParameters(module, level) ||
        !LOG_IS_OUTPUT(module)) {
        return;
    }

    HiLogCommon *pCommon = &(logContent.commonContent);
    pCommon->head = LOG_INFO_HEAD;
    pCommon->module = module;
    pCommon->level = level;
    pCommon->fmt = fmt;
    pCommon->valueNumber = (uint8)argsNum;
    pCommon->task = (uint8)HIVIEW_GetTaskId();
    uint64 cur = HIVIEW_GetCurrentTime();
    pCommon->time = (uint32)(cur / MS_PER_SECOND);
    pCommon->milli = (uint16)(cur % MS_PER_SECOND);

    uint8 i = 0;
    va_list args;
    va_start(args, fmt);
    while (i < argsNum) {
        logContent.values[i++] = va_arg(args, uint32);
    }
    va_end(args);

    OutputLog((uint8 *)&logContent, sizeof(HiLogCommon) + sizeof(uint32) * argsNum);
}

// Log level string (no stack operations, directly return global constant string)
static const char *HiLogGetLevelStr(uint8 level)
{
    // Use array instead of switch to reduce local variables, return global constant (no stack usage)
    static const char *const LEVEL_STR[] = {"UNKNOWN", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    return (level >= HILOG_LV_DEBUG && level < HILOG_LV_MAX) ? LEVEL_STR[level] : LEVEL_STR[0];
}

// Get time string (only use heap buffer, no stack buffer)
static char* HiLogGetTimeStr(void)
{
    // Heap allocate 32 bytes (exactly enough for "YYYY-MM-DD HH:MM:SS" format)
    char* time_buf = (char*)malloc(TIME_BUF_LEN);
    if (time_buf == NULL) {
        return NULL;
    }

    time_t now;
    struct tm* tm_info;
    now = time(NULL);          // Operate on global time_t variable, no large stack object
    if (now == (time_t) -1) {
        free(time_buf);
        return NULL;
    }
    tm_info = localtime(&now);  // Return global tm pointer, do not store tm struct on stack
    if (tm_info == NULL) {
        free(time_buf);
        return NULL;
    }
    // Directly format into heap buffer, no stack transfer
    size_t bytes_written = strftime(time_buf, TIME_BUF_LEN, "%Y-%m-%d %H:%M:%S", tm_info);
    if (bytes_written == 0) {
        free(time_buf);
        return NULL;
    }
    return time_buf;
}

// Function: Remove all "{public}" substrings from the input string
static void RemovePublic(char *str)
{
    if (str == NULL) {
        return; // Handle null pointer to avoid crashes
    }

    const char *target = "%{public}"; // Target substring to be replaced
    int targetLen = strlen(target);   // Length of target substring (9 characters)
    const char replacement = '%';     // Replacement character
    char *pos;

    // Loop to find and remove all "{public}" substrings (handles multiple occurrences)
    while ((pos = strstr(str, target)) != NULL) {
        // Replace the first character of the target with '%'
        *pos = replacement;
        // Move characters after the substring forward by subLen positions to overwrite "{public}"
        if (memmove_s(pos + 1, strlen(pos), pos + targetLen, strlen(pos + targetLen) + 1) != 0) {
            return;
        }
        // +1 to include the null terminator '\0' in the move
    }
    return;
}

static char *build_log_format_string(const char *time_str, const char *level_str,
                                     const char *module_name, const char *fmt)
{
    // 4. Calculate lengths (only use size_t variables to store results, no stack buffer)
    size_t time_len, level_len, module_len, fmt_len, full_fmt_size;
    char *full_fmt;
    time_len = strlen(time_str);
    level_len = strlen(level_str);
    module_len = strlen(module_name);
    fmt_len = strlen(fmt);
    // 5. Calculate full format string length (only integer operations, result stored on stack)
    full_fmt_size = LOG_MULTI_PARA_5 + time_len + LOG_MULTI_PARA_3 + level_len +
                           LOG_MULTI_PARA_3 + module_len + LOG_MULTI_PARA_2 + fmt_len + 1 + 1;
    // 6. Allocate full format string (heap allocation, no stack buffer)
    full_fmt = (char*)malloc(full_fmt_size);
    if (full_fmt == NULL) {
        return NULL;
    }

    if (snprintf_s(full_fmt, full_fmt_size, full_fmt_size - 1, "OHOS [%s] [%s] %s: %s\n",
                   time_str, level_str, module_name, fmt) <= 0) {
        free(full_fmt);
        return NULL;
    }
    return full_fmt;
}

// Core logging function: stack only stores pointers/integers, no stack buffers
void HiLogPrintfExt(uint8 module, uint8 level, const char *nums, const char *fmt, ...)
{
    // Stack variables only include: pointers (8 bytes each), integers (4 bytes each), no arrays
    const char *module_name, *level_str;
    char *time_str, *full_fmt, *logBuffer;
    va_list args;
    // 1. Lightweight calculation and level filtering (only use integer variables, no stack consumption)
    if (level < HILOG_LV_INVALID) {
        return;  // Return early to reduce stack usage
    }
    // 2. Get time string (heap allocation, no stack buffer)
    time_str = HiLogGetTimeStr();
    if (time_str == NULL) {
        return;
    }
    // 3. Get module name/level string (only store pointers, no string content on stack)
    module_name = HiLogGetModuleName(module);
    level_str = HiLogGetLevelStr(level);
    if (module_name == NULL || level_str == NULL || fmt == NULL) {
        free(time_str);
        return;
    }
    // 7. Build format string (directly write to heap buffer, no stack transfer)
    full_fmt = build_log_format_string(time_str, level_str, module_name, fmt);
    free(time_str);
    if (full_fmt == NULL) {
        return;
    }
    RemovePublic(full_fmt);
    // 8. Allocate log buffer (heap allocation, size on demand, no fixed stack buffer)
    logBuffer = malloc(MAX_LOG_LENGTH + 1);  // +1 for terminator
    if (logBuffer == NULL) {
        free(full_fmt);
        return;
    }
    // 9. Format and print log
    va_start(args, fmt);
    int fmt_ret = vsnprintf_s(logBuffer, MAX_LOG_LENGTH + 1, MAX_LOG_LENGTH, full_fmt, args);
    va_end(args);
    if (fmt_ret > 0 && fmt_ret <= MAX_LOG_LENGTH) {
        printf("%s", logBuffer);
    } else if (strlen(logBuffer) == MAX_LOG_LENGTH) {
        size_t safe_len = MAX_LOG_LENGTH - TRUNCATION_MARKER_LENGTH;
        logBuffer[safe_len] = '\0';
        errno_t err = strcat_s(logBuffer, MAX_LOG_LENGTH + 1, "...\n");
        if (err == 0) {
            printf("%s", logBuffer);
        }
    }
    // 10. Release (only pointer operations, no stack consumption)
    free(logBuffer);
    free(full_fmt);
}


void HILOG_HashPrintf(uint8 module, uint8 level, const char *nums, uint32 hash, ...)
{
    HiLogContent logContent = {0};
    int32 argsNum = (nums - FUN_ARG_S);
    if (argsNum < 0 || argsNum > LOG_MULTI_PARA_MAX) {
        argsNum = 0;
    }

    if (g_hiviewConfig.logSwitch == HIVIEW_FEATURE_OFF || !CheckParameters(module, level) ||
        !LOG_IS_OUTPUT(module)) {
        return;
    }

    HiLogCommon *pCommon = &(logContent.commonContent);
    pCommon->head = LOG_INFO_HEAD;
    pCommon->module = module;
    pCommon->level = SET_HASH_FLAG(level);
    pCommon->fmt = (const char*)hash;
    pCommon->valueNumber = (uint8)argsNum;
    pCommon->task = (uint8)HIVIEW_GetTaskId();
    uint64 cur = HIVIEW_GetCurrentTime();
    pCommon->time = (uint32)(cur / MS_PER_SECOND);
    pCommon->milli = (uint16)(cur % MS_PER_SECOND);

    uint8 i = 0;
    va_list args;
    va_start(args, hash);
    while (i < argsNum) {
        logContent.values[i++] = va_arg(args, uint32);
    }
    va_end(args);

    OutputLog((uint8 *)&logContent, sizeof(HiLogCommon) + sizeof(uint32) * argsNum);
}

void HiLogFlush(boolean syncFlag)
{
    FlushLog(syncFlag);
}

uint32 HiLogGetConfigOption(void)
{
    return HiviewGetConfigOption();
}

void HiLogRegisterProc(HilogProc func)
{
    HiviewRegisterHilogProc(func);
}

void HiLogUnRegisterProc(HilogProc func)
{
    HiviewUnRegisterHilogProc(func);
}

void HiLogFileAddWatcher(FileProc func, const char *path)
{
    HiviewRegisterHiLogFileWatcher(func, path);
}

void HiLogFileRemoveWatcher(FileProc func)
{
    HiviewUnRegisterHiLogFileWatcher(func);
}

int HiLogFileProc(const char *dest, uint8 mode)
{
    return HiLogFileProcImp(dest, mode);
}

void HiLogOutputFileLock(void)
{
    HiLogOutputFileLockImp();
}

void HiLogOutputFileUnLock(void)
{
    HiLogOutputFileUnLockImp();
}

uint32 HiLogGetLogLevel(void)
{
    return g_hiviewConfig.level;
}

boolean HiLogSetLogLevel(uint8 level)
{
    if (level >= HILOG_LV_DEBUG && level < HILOG_LV_MAX) {
        g_hiviewConfig.level = level;
        printf("Set log level: %d\n", level);
        return TRUE;
    }
    return FALSE;
}
