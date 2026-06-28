/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
* Description: bluetooth common header
* Author: Media Software Group
* Create: 2025-10-08
*/

#include "common.h"
#include "bluetooth_port.h"
#include "plugin_common.h"
#include "media_hal_common.h"
#include "latm_simple_muxer.h"
#include "media_hal_thread_adapt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "AudioBluetoothCommonPort"

typedef struct {
    enum AudioPortPin portPin;
    enum SubPortType subPortType;
} PortPair;

static const PortPair g_portPairs[] = {
    { PIN_IN_BLUETOOTH_SCO, SUB_INPUT_PORT_BT_SCO },
    { PIN_OUT_BLUETOOTH_SCO, SUB_OUTPUT_PORT_BT_SCO },
    { PIN_IN_BLUETOOTH_A2DP, SUB_INPUT_PORT_A2DP },
    { PIN_OUT_BLUETOOTH_A2DP, SUB_OUTPUT_PORT_A2DP },
    { PIN_IN_NEARLINK_DONGLE, SUB_INPUT_PORT_NEARLINK_DONGLE },
    { PIN_OUT_NEARLINK_DONGLE, SUB_OUTPUT_PORT_NEARLINK_DONGLE }
};

#define BT_PORT_MAX_TRACK_NUM 4
static TrackStatus g_trackStatusList[BT_PORT_MAX_TRACK_NUM];
static int32_t g_trackActiveCount = 0;
static MediaHalMutexHandle g_trackResourceLock = NULL;
static bool g_trackMngInit = false;


bool GetSubPortType(enum AudioPortPin portPin, enum SubPortType *subPortType)
{
    bool isFound = false;
    for (uint32_t i = 0; i < ARRAY_SIZE(g_portPairs); i++) {
        if (portPin == g_portPairs[i].portPin) {
            *subPortType = g_portPairs[i].subPortType;
            isFound = true;
            break;
        }
    }

    if (!isFound) {
        MEDIA_HAL_LOGE(MODULE_NAME, "port pin 0x%x is invalid", portPin);
    }
    return isFound;
}

bool IsNeedHandlePublicResource(void)
{
    MediaHalMutexLock(g_trackResourceLock);
    for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
        if (g_trackStatusList[i].used) {
            if (!g_trackStatusList[i].paused) {
                MediaHalMutexUnLock(g_trackResourceLock);
                return false;
            }
        }
    }
    MediaHalMutexUnLock(g_trackResourceLock);
    return true;
}

int32_t SubPortTrackManagerInit(void)
{
    MediaHalInitStaticMutexLock(&g_trackResourceLock);
    MediaHalMutexLock(g_trackResourceLock);
    if (g_trackMngInit == false) {
        for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
            g_trackStatusList[i].trackId = i;
            g_trackStatusList[i].used = false;
            g_trackStatusList[i].paused = false;
        }
        g_trackMngInit = true;
    }
    MediaHalMutexUnLock(g_trackResourceLock);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t SubPortTrackManagerDeInit(void)
{
    MediaHalInitStaticMutexLock(&g_trackResourceLock);
    MediaHalMutexLock(g_trackResourceLock);
    if (g_trackMngInit) {
        for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
            if (g_trackStatusList[i].used) {
                MEDIA_HAL_LOGE(MODULE_NAME, "bluetooth port track %d not free", g_trackStatusList[i].trackId);
            }
        }
        g_trackMngInit = false;
    }
    MediaHalMutexUnLock(g_trackResourceLock);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t SubPortTrackSetPause(int32_t trackId, bool pause)
{
    MediaHalMutexLock(g_trackResourceLock);
    for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
        if (g_trackStatusList[i].trackId != trackId) {
            continue;
        }
        if (!g_trackStatusList[i].used) {
            MediaHalMutexUnLock(g_trackResourceLock);
            MEDIA_HAL_LOGE(MODULE_NAME, "bluetooth port track id: %d not used can't pause", trackId);
            return MEDIA_HAL_INVALID_PARAM;
        }
        g_trackStatusList[i].paused = pause;
        MediaHalMutexUnLock(g_trackResourceLock);
        return MEDIA_HAL_OK;
    }
    MediaHalMutexUnLock(g_trackResourceLock);
    MEDIA_HAL_LOGE(MODULE_NAME, "track id: %d is invalid", trackId);
    return MEDIA_HAL_ERR;
}

int32_t SubPortGetFreeTrack(int32_t *trackCount)
{
    MediaHalInitStaticMutexLock(&g_trackResourceLock);
    MediaHalMutexLock(g_trackResourceLock);
    if (!g_trackMngInit) {
        MediaHalMutexUnLock(g_trackResourceLock);
        MEDIA_HAL_LOGE(MODULE_NAME, "bluetooth port track manager not init");
        return INVALID_ID;
    }

    for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
        if (!g_trackStatusList[i].used) {
            g_trackStatusList[i].used = true;
            *trackCount = ++g_trackActiveCount;
            int32_t trackId = g_trackStatusList[i].trackId;
            MediaHalMutexUnLock(g_trackResourceLock);
            return trackId;
        }
    }
    MediaHalMutexUnLock(g_trackResourceLock);
    MEDIA_HAL_LOGE(MODULE_NAME, "bluetooth port no free track");
    return INVALID_ID;
}

int32_t SubPortFreeTrack(int32_t trackId, int32_t *trackCount)
{
    MediaHalInitStaticMutexLock(&g_trackResourceLock);
    MediaHalMutexLock(g_trackResourceLock);
    for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
        if (g_trackStatusList[i].trackId == trackId) {
            if (!g_trackStatusList[i].used) {
                MediaHalMutexUnLock(g_trackResourceLock);
                MEDIA_HAL_LOGE(MODULE_NAME, "bluetooth port track id: %d not used can't free", trackId);
                return MEDIA_HAL_ERR;
            }
            g_trackStatusList[i].used = false;
            *trackCount = --g_trackActiveCount;
            MediaHalMutexUnLock(g_trackResourceLock);
            return MEDIA_HAL_OK;
        }
    }
    MediaHalMutexUnLock(g_trackResourceLock);
    MEDIA_HAL_LOGE(MODULE_NAME, "track id: %d is invalid", trackId);
    return MEDIA_HAL_ERR;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

