/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2021. All rights reserved.
 * Description: media hal common
 * Author: Media Software Group
 * Create: 2021-04-02
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#ifdef SUPPORT_DL
#include <errno.h>
#include <dlfcn.h>
#endif
#include <limits.h>
#include <unistd.h>
#include "securec.h"
#include "media_hal_common.h"
#ifdef HI3322_CHIP_V100
#include "osal_addr.h"
#include "chip_init.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define MODULE_NAME "MediaHalCommon"
#define MAX_LINE_SZ  256

#define SS2US  1000000
#define US2MS  1000
#define US2MS_HALF  500
#define S2MS  1000
#define TIME_SCALE 1000
#define TIME_NS_TO_MS 1000000

static MEDIA_HAL_LOG_LEVEL g_enabledLevel = MEDIA_HAL_LOG_LEVEL_ERR;
#ifdef NO_NEED_MPI_SYS_INIT
static bool g_systemInited = false;
static pthread_mutex_t g_systemInitMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static char g_logOut[MAX_LINE_SZ] = {0};

static void LogTime(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm tm;
    localtime_r(&tv.tv_sec, &tm);
    const long USEC_TO_MSEC = 1000;
    printf("[%02d:%02d:%02d:%03ld]", tm.tm_hour, tm.tm_min, tm.tm_sec, (long)tv.tv_usec / USEC_TO_MSEC);
}

int32_t MediaHalLogPrintf(const char *modName, MEDIA_HAL_LOG_LEVEL level, const char *fmt, ...)
{
    if (level < g_enabledLevel) {
        return MEDIA_HAL_OK;
    }
    if (modName == NULL || fmt == NULL) {
        return MEDIA_HAL_ERR;
    }
    char *levelStr;
    switch (level) {
        case MEDIA_HAL_LOG_LEVEL_VERBOSE:
            levelStr = "V";
            break;
        case MEDIA_HAL_LOG_LEVEL_DEBUG:
            levelStr = "D";
            break;
        case MEDIA_HAL_LOG_LEVEL_INFO:
            levelStr = "I";
            break;
        case MEDIA_HAL_LOG_LEVEL_WARN:
            levelStr = "W";
            break;
        case MEDIA_HAL_LOG_LEVEL_ERR:
            levelStr = "E";
            break;
        case MEDIA_HAL_LOG_LEVEL_TRACE:
            levelStr = "T";
            break;
        default:
            levelStr = "E";
            break;
    }
    LogTime();
    va_list args;
    printf("[%s:%s]", modName, levelStr);
    va_start(args, fmt);
    if (vsnprintf_s(g_logOut, MAX_LINE_SZ, MAX_LINE_SZ - 1, fmt, args) < 0) {
        printf("vsnprintf_s failed \n");
        va_end(args);
        return MEDIA_HAL_OK;
    }
    va_end(args);
    printf("%s \n", g_logOut);
    return MEDIA_HAL_OK;
}

bool MediaHalIntMulIsOverflow64(const int64_t a, const int64_t b, int64_t *value)
{
    if (a == 0 || b == 0) {
        return false;
    }
    if (INT64_MAX / a < b) {
        return true;
    }

    int64_t result = (a) * (b);
    if (value != NULL) {
        *value = result;
    }
    return false;
}

int32_t MediaHalConvertUSecToSecAndNSec(int64_t usec, int64_t *sec, int64_t *nsec)
{
    *sec = usec / MICROS_PER_SECOND;
    int64_t remainUs = usec % MICROS_PER_SECOND;
    bool isOverflow = MediaHalIntMulIsOverflow64(remainUs, NANOS_PER_MICROSECOND, nsec);
    if (isOverflow) {
        MEDIA_HAL_LOGE(MODULE_NAME, "remain usec overflow");
        *nsec = INT64_MAX;
        return MEDIA_HAL_ERR;
    }

    return MEDIA_HAL_OK;
}

#ifdef SUPPORT_DL
void *MediaHalDLOpen(const char *filePath)
{
    CHK_NULL_RETURN(filePath, NULL, "Input param filePath null");
    char absPath[PATH_MAX + 1] = {0};
    if (realpath(filePath, absPath) == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "realpath %s failed: %d", filePath, errno);
        return NULL;
    }
    void *soModule = NULL;
    soModule = dlopen(absPath, RTLD_LAZY);
    if (soModule == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "dlopen %s failed: %s", filePath, dlerror());
        return soModule;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "dlopen %s success", filePath);
    return soModule;
}

void *MediaHalDLSym(const VoidPtr module, const char *symbol)
{
    void *moduleAddr = NULL;
    if (module == NULL || symbol == NULL) {
        return NULL;
    }
    moduleAddr = dlsym(module, symbol);
    return moduleAddr;
}

void MediaHalDLClose(VoidPtr dllHdl)
{
    if (dllHdl == NULL) {
        return;
    }
    dlclose(dllHdl);
}
#endif

int32_t SetMediaHalLogEnabledLevel(MEDIA_HAL_LOG_LEVEL level)
{
    if (level < MEDIA_HAL_LOG_LEVEL_VERBOSE ||
        level > MEDIA_HAL_LOG_LEVEL_ERR) {
        return MEDIA_HAL_INVALID_PARAM;
    }
    g_enabledLevel = level;
    return MEDIA_HAL_OK;
}

void *MediaMallocz(uint32_t len)
{
    if (len == 0) {
        return NULL;
    }
#ifdef HI3322_CHIP_V100
    void *ptr = osal_pool_mem_alloc(OS_EXTEND_MEM_ADDR, len);
#else
    void *ptr = malloc(len);
#endif
    if (ptr == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
        return NULL;
    }

    if (memset_s(ptr, len, 0, len) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        MediaHalFree(ptr);
        return NULL;
    }

    return ptr;
}

void *MediaHalMalloc(const size_t bytesNum)
{
    if (bytesNum == 0) {
        return NULL;
    }
#ifdef HI3322_CHIP_V100
    void *ptr = osal_pool_mem_alloc(OS_EXTEND_MEM_ADDR, bytesNum);
#else
    void *ptr = malloc(bytesNum);
#endif
    return ptr;
}

void *MediaHalCalloc(const size_t bytesNum)
{
#ifdef HI3322_CHIP_V100
    void *tmp = osal_pool_mem_alloc(OS_EXTEND_MEM_ADDR, bytesNum);
#else
    void *tmp = malloc(bytesNum);
#endif
    if (tmp == NULL) {
        return NULL;
    }

    if (memset_s(tmp, bytesNum, 0, bytesNum) != EOK) {
        MediaHalFree(tmp);
        return NULL;
    } else {
        return tmp;
    }
}

void MediaHalFree(const void* src)
{
    if (src == NULL) {
        return;
    }
#ifdef HI3322_CHIP_V100
    osal_pool_mem_free(OS_EXTEND_MEM_ADDR, src);
#else
    free((void *)src);
#endif
}

uint64_t GetTimeTicks64(void)
{
    struct timespec stTv = { 0, 0 };

    int32_t result = clock_gettime(CLOCK_MONOTONIC, &stTv);
    if (result != 0) {
        return 0ULL;
    }

    /* similar to MediaGetTimeTicks(void), convert to millisecods */
    uint64_t curTime = (uint64_t)(stTv.tv_sec) * TIME_SCALE +
        (uint64_t)((stTv.tv_nsec + (TIME_NS_TO_MS >> 1)) / TIME_NS_TO_MS);

    return curTime;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
