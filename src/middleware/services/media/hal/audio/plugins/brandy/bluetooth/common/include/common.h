/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
* Description: bluetooth common header
* Author: Media Software Group
* Create: 2025-10-08
*/

#ifndef COMMON_H
#define COMMON_H

#include "audio_port_plugin.h"

#include <unistd.h>
#include "securec.h"

#ifdef AUDIO_SDK_SUPPORT
#include "soc_uapi_adp.h"
#include "soc_uapi_adec.h"
#include "soc_uapi_aenc.h"
#include "soc_uapi_audio_sys.h"
#include "soc_uapi_audio.h"
#include "soc_uapi_sound.h"
#endif
#ifdef BT_HAL_SUPPORT
#include "bt_audio_hal_interface.h"
#endif
#include "product.h"
#if defined(BRANDY_PRODUCT_EVB)
#include "product_evb_standard.h"
#elif defined(BRANDY_PRODUCT_EVB4)
#include "product_evb4_standard.h"
#endif
#include "plugin_common.h"
#include "media_hal_common.h"
#include "latm_simple_muxer.h"
#include "media_hal_thread_adapt.h"
#include "sample_audio_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sub port track api */

typedef enum {
    BLUETOOTH_SCO = 0,
    BLUETOOTH_IN_A2DP_NORMAL,
    BLUETOOTH_OUT_A2DP_NORMAL,
    NEARLINK_IN_NORMAL,
    NEARLINK_OUT_NORMAL
} WorkScene;

typedef struct {
    bool used;
    bool paused;
    int32_t trackId;
} TrackStatus;

#define BT_PORT_MAX_TRACK_NUM 4
#define ANEC_HANDLE_MAX_NUM   4
#define OUTPUTADP_HANDLE_MAX_NUM   4
#define MAX_KEY_STR_LEN 10

typedef struct {
    /* track impl */
    AudioPortTrack common;
#ifdef AUDIO_SDK_SUPPORT
    /* adp context */
    td_handle inputAdpHandle;
    td_handle outputAdpHandle;
    td_handle inputUplinkAdpHandle;
    td_handle outputUplinkAdpHandle;
    /* adec context */
    td_handle adecHandle;
    /* adec context */
    td_handle adecUplinkHandle;
    /* aenc context */
    td_handle aencHandle;
    td_handle aiHandle;
    td_handle outputAdpHandles[OUTPUTADP_HANDLE_MAX_NUM];
    /* aenc context */
    td_handle aencHandles[ANEC_HANDLE_MAX_NUM];
    /* sound context */
    td_handle castSndHandle;
    /* sound track context */
    td_handle trackHandle;
    bool isReleased;
    float volume;
#endif
    /* port track context */
    int32_t trackId;
    WorkScene workScene;
    TrackCallback callback;
    AudioHandle caller;
#ifdef BT_HAL_SUPPORT
    /* bluetooth context */
    bool btStarted;
    td_pvoid btStreamHandle;
    uint32_t a2dpSinkMemId;
#endif
    enum SubPortType subPortType;
    /* sub port context */
    struct SubPortContext *subPortCtx;
    /* attr context */
    bool isRaw;
    int32_t trackCount;
    struct PortPluginAttr inputAttr;
    /* audio basic param for latm when low delay scene */
    AudioBasicParam audBasicParamForLATM;
    /* Record whether public resources need to be processed due to miniaturization and Bluetooth limitations. */
    bool isHandlePublicResource;
    bool aencStarted;
} SubPortTrackContext;

typedef struct SubPortContext {
    /****************************************************************************************/
    /* 1. castSndHandle/aencHandle/outputAdpHandle for BLUETOOTH_A2DP_NORMAL WorkScene      */
    /* 2. sdk usage sample as follows:                                                      */
    /* track1 --                                                                            */
    /*          |--> sound(mixing) + aenc + adp(output)                                     */
    /* track2 --                                                                            */
    /****************************************************************************************/
#ifdef AUDIO_SDK_SUPPORT
    /* sound context */
    td_handle castSndHandle;
    /* aenc context */
    td_handle aencHandle;
    /* adp context */
    td_handle outputAdpHandle;
    uapi_audio_sample_rate sampleRate;
    uapi_audio_channel channelCount;
#endif
#ifdef BT_HAL_SUPPORT
    /* bluetooth context */
    bool btScoStarted;
    bool btA2dpStarted;
    td_pvoid btScoStreamHandle;
    td_pvoid btA2dpStreamHandle;
    td_pvoid btA2dpSinkStreamHandle;
    uint8_t audioCap[4];
    bt_audio_a2dp_config_chg_data btA2dpConfigChgdata;
    bt_audio_event_type btDevState;
#endif
    /* record track info */
    MediaHalMutexHandle a2dpTrackMutex;
    int32_t a2dpTrackCount;
    int32_t scoTrackCount;
    SubPortTrackContext *portTrack[BT_PORT_MAX_TRACK_NUM];
    AudioA2dpAencAttr aencAttr;
    MediaHalMutexHandle aencMutex;
    bool btAencAttrReceived;
} SubPortContext;

bool GetSubPortType(enum AudioPortPin portPin, enum SubPortType *subPortType);

bool IsNeedHandlePublicResource(void);

int32_t SubPortTrackManagerInit(void);

int32_t SubPortTrackManagerDeInit(void);

int32_t SubPortTrackSetPause(int32_t trackId, bool pause);

int32_t SubPortGetFreeTrack(int32_t *trackCount);

int32_t SubPortFreeTrack(int32_t trackId, int32_t *trackCount);

#ifdef __cplusplus
}
#endif
#endif
