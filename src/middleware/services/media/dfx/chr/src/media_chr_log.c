/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
 * Description: chr log mark header
 * Author: Media Software Group
 * Create: 2025-07-15
 */

#include "media_chr_log.h"
#include "media_hal_common.h"
#define MODULE_NAME "mediachrlog"
#define MEDIA_CHR_UNUSED(x) ((void)(x))

#ifdef SUPPORT_KPI_ON
typedef struct {
    struct timeval timeStart;
    int32_t index;
    int32_t recordCnt;
    int64_t totalTime;
} RecordTimeContext;

static void *g_timeRecorderMutex = NULL;

static RecordTimeContext g_timeCtx[] = {
    {.index = 0, .recordCnt = 0, .totalTime = 0 },
    {.index = 1, .recordCnt = 0, .totalTime = 0 },
    {.index = 2, .recordCnt = 0, .totalTime = 0 },
    {.index = 3, .recordCnt = 0, .totalTime = 0 },
    {.index = 4, .recordCnt = 0, .totalTime = 0 },
    {.index = 5, .recordCnt = 0, .totalTime = 0 },
    {.index = 6, .recordCnt = 0, .totalTime = 0 },
    {.index = 7, .recordCnt = 0, .totalTime = 0 },
    {.index = 8, .recordCnt = 0, .totalTime = 0 },
    {.index = 9, .recordCnt = 0, .totalTime = 0 },
    {.index = 10, .recordCnt = 0, .totalTime = 0 },
    {.index = 11, .recordCnt = 0, .totalTime = 0 },
    {.index = 12, .recordCnt = 0, .totalTime = 0 },
};

static RecordTimeContext *GetRecordTime(int32_t index)
{
    MediaHalInitStaticMutexLock(g_timeRecorderMutex);
    MediaHalMutexLock(g_timeRecorderMutex);
    RecordTimeContext *recordTime = NULL;
    uint32_t size = (uint32_t)(sizeof(g_timeCtx) / sizeof(RecordTimeContext));
    for (uint32_t i = 0; i < size; i++) {
        if (g_timeCtx[i].index == index) {
            recordTime = &g_timeCtx[i];
            break;
        }
    }
    MediaHalMutexUnLock(g_timeRecorderMutex);
    return recordTime;
}

static void UpdateRecordTime(int32_t index, struct timeval time)
{
    MediaHalInitStaticMutexLock(g_timeRecorderMutex);
    MediaHalMutexLock(g_timeRecorderMutex);
    uint32_t size = (uint32_t)(sizeof(g_timeCtx) / sizeof(RecordTimeContext));
    for (uint32_t i = 0; i < size; i++) {
        if (g_timeCtx[i].index == index) {
            g_timeCtx[i].timeStart.tv_sec = time.tv_sec;
            g_timeCtx[i].timeStart.tv_usec = time.tv_usec;
            break;
        }
    }
    MediaHalMutexUnLock(g_timeRecorderMutex);
}

static void ResetRecordTime(int32_t index)
{
    MediaHalInitStaticMutexLock(g_timeRecorderMutex);
    MediaHalMutexLock(g_timeRecorderMutex);
    uint32_t size = (uint32_t)(sizeof(g_timeCtx) / sizeof(RecordTimeContext));
    for (uint32_t i = 0; i < size; i++) {
        if (g_timeCtx[i].index == index) {
            g_timeCtx[i].timeStart.tv_sec = 0;
            g_timeCtx[i].timeStart.tv_usec = 0;
            g_timeCtx[i].recordCnt = 0;
            g_timeCtx[i].totalTime = 0;
            break;
        }
    }
    MediaHalMutexUnLock(g_timeRecorderMutex);
}

void MediaDfxStartRecordTime(int32_t index)
{
    struct timeval timeStart = {};
    gettimeofday(&timeStart, NULL);
    UpdateRecordTime(index, timeStart);
}

static int64_t GetDiffTime(int32_t index)
{
    struct timeval timeEnd = {};
    gettimeofday(&timeEnd, NULL);
    const int64_t timeUs = 1000000;
    int64_t value = 0;
    RecordTimeContext *timeCtx = GetRecordTime(index);
    bool isOverflow = MediaHalIntMulIsOverflow64((int64_t)(timeEnd.tv_sec - timeCtx->timeStart.tv_sec), timeUs, &value);
    if (isOverflow) {
        MEDIA_HAL_LOGE(MODULE_NAME, "calculate reslut overflow!");
        return MEDIA_HAL_ERR;
    }
    int64_t diffSecToUs = value;
    int64_t diffUsec = (int64_t)(timeEnd.tv_usec - timeCtx->timeStart.tv_usec);
    int64_t diffTimeUS = diffSecToUs + diffUsec;
    timeCtx->totalTime += diffTimeUS;
    return diffTimeUS;
}

int64_t MediaDfxPrintRecordTime(int32_t index, int32_t recordCnt, const char *name, bool showLog)
{
    RecordTimeContext *timeCtx = GetRecordTime(index);
    if (timeCtx == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "timeCtx is NULL");
        return MEDIA_HAL_ERR;
    }
    int64_t diffTimeUS = GetDiffTime(index);
    if (recordCnt <= 0) {
        if (showLog) {
            MEDIA_HAL_LOGE(MODULE_NAME, "[%s] elapsed time: %lld us", name, diffTimeUS);
        }
        ResetRecordTime(index);
        return diffTimeUS;
    }
    if (timeCtx->recordCnt < recordCnt) {
        if (diffTimeUS != 0) {
            timeCtx->recordCnt++;
        }
        return diffTimeUS;
    }
    if (showLog) {
        MEDIA_HAL_LOGE(MODULE_NAME, "[%s] count[%d] elapsed time: %lld us",
            name, recordCnt, timeCtx->totalTime / recordCnt);
    }
    ResetRecordTime(index);
    return diffTimeUS;
}
#endif

void MediaChrLog(uint8_t eventType, uint8_t info1,
    uint8_t info2, uint8_t info3)
{
#ifdef MEDIA_CHR_SUPPORT
    uapi_massdata_record_system_event(eventType, info1, info2, info3);
#else
    MEDIA_CHR_UNUSED(eventType);
    MEDIA_CHR_UNUSED(info1);
    MEDIA_CHR_UNUSED(info2);
    MEDIA_CHR_UNUSED(info3);
#endif
}

void MediaChrLogEx(uint8_t eventType, uint8_t info1,
    uint8_t info2, uint8_t *info3, uint8_t logInfoLen)
{
#ifdef MEDIA_CHR_SUPPORT
    uapi_massdata_record_system_long_event(eventType, info1, info2, info3, logInfoLen);
#else
    MEDIA_CHR_UNUSED(eventType);
    MEDIA_CHR_UNUSED(info1);
    MEDIA_CHR_UNUSED(info2);
    MEDIA_CHR_UNUSED(info3);
    MEDIA_CHR_UNUSED(logInfoLen);
#endif
}