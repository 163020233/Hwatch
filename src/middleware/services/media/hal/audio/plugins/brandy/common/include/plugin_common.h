/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2021. All rights reserved.
* Description: audio plugin common header
* Author: Media Software Group
* Create: 2021-05-18
*/

#ifndef AUDIO_PORT_COMMON_H
#define AUDIO_PORT_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include "securec.h"

#ifdef AUDIO_SDK_SUPPORT
#include "soc_uapi_ai.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_aenc.h"
#include "soc_uapi_adec.h"
#include "soc_uapi_sea.h"
#include "soc_uapi_sound.h"
#include "soc_uapi_audio.h"
#include "soc_uapi_acodec.h"
#include "soc_uapi_audio_sys.h"
#endif

#include "audio_types.h"
#include "media_chr_log.h"
#include "audio_port_plugin.h"
#include "media_hal_thread_adapt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
    AUDIO_MOD_AI = 0,
    AUDIO_MOD_AI_SEA,
    AUDIO_MOD_ADP,
    AUDIO_MOD_ADEC,
    AUDIO_MOD_AENC,
    AUDIO_MOD_SEA,
    AUDIO_MOD_SEA_REC,
    AUDIO_MOD_SOUND
} AudioSDKModuleType;

typedef enum {
    MEDIA_VOLUME_MODULE_AI = 0,
    MEDIA_VOLUME_MODULE_AENC = 1,
    MEDIA_VOLUME_MODULE_AO = 2,
    MEDIA_VOLUME_MODULE_ADEC = 3,
    MEDIA_VOLUME_MODULE_TRACK = 4,
} AudioModule;

#define LOG_MEDIA(type, category) \
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER, type, category)

#ifdef AUDIO_SDK_SUPPORT
typedef struct {
    enum AudioFormat audioFormat;
    uapi_acodec_id codecId;
} AudioFormatMapping;
#endif

#define INVALID_ID (-1)

// convert sample rate to number of samples per frame
// need fix: AI set this parameter based on the audio encoding protocol
#define SAMPLERATE_TO_PTNUMPERFRM 100

#define VOLUME_MIN 0.0f
#define VOLUME_MAX 1.0f
#define MAX_KEY_STR_SIZE 30
#define MAX_KEY_VALUE_SIZE 512
#define VOLUME_ACCURACY 1000
#define LPWK_PHRASE_COUNT 20
#define SET_DB_VOLUME "setdbvol"
#define GET_DB_VOLUME "getdbvol"

#define GET_AUDIO_POSITION_INTERVAL_US 10000
#define GET_AUDIO_CACHED_DATA_TIMEOUT_MS 1000
#define AUDIO_STREAM_CONFIG_L2HC_CMD   1

typedef struct {
    uint16_t sampleRate;
    uint32_t bitRate;
    uint8_t codecType;
    uint8_t payload;
    uint8_t channel;
    uint8_t channelMode;
    uint8_t blockLength;
    uint8_t subband;
    uint8_t allocMethod;
    uint8_t minBitpool;
    uint8_t maxBitpool;
    uint8_t objectType;
    uint8_t rfa;
    uint8_t vbr;
} AudioA2dpAencAttr;
bool IsRawFormat(enum AudioFormat audioFormat);

#ifdef AUDIO_SDK_SUPPORT
void VolumePercentToDb(float volume, uapi_snd_gain *sndGain);

typedef struct {
    enum AudioCategory audioCategory;
    uint32_t micType;
    bool isNeedOpenSea;
    bool isNeedSetRXType;
    struct PortPluginAttr inputAttr;
} AIOpenAttr;

typedef struct {
    enum AudioCategory audioCategory;
    int32_t sndDevId;
    td_handle sndHandle1;
    td_handle sndHandle0;
    int32_t snd0UsedCnt;
    int32_t snd1UsedCnt;
} AOdevContext;

typedef enum {
    AENC_TYPE_MODEM = 0,
    AENC_TYPE_PRIMARY,
    AENC_TYPE_USB,
    AENC_TYPE_BT,
    AENC_TYPE_DONGLE_UPLINK,
} AencType;

typedef struct {
    uint32_t phsCount;
    uapi_sea_item phrase[LPWK_PHRASE_COUNT];
} LpwkPhsSets;

bool AudioConvertAudioFormatToCodecId(enum AudioFormat audioFormat, uapi_acodec_id *codecId);

bool FindSupportSound1Scene(enum AudioCategory audioCategory);

int32_t AudioSDKInit(void);

int32_t AudioSDKDeInit(void);

LpwkPhsSets *GetLpwkPhsSets(void);

int32_t AudioSDKAIOpen(td_handle *aiHandle, const AIOpenAttr *aiOpenAttr);

int32_t AudioSDKSetAIScene(td_handle aihandle, const struct PortPluginAttr *inputAttr, bool isNearLinkUplink);

int32_t AudioSDKAIClose(td_handle aiHandle);

int32_t AudioSDKAODevOpen(AOdevContext *aoDevCtx, const struct PortPluginAttr *inputAttr);

uapi_audio_scene GetScencTypeByCategory(const struct PortPluginAttr *inputAttr, bool toggleValid, bool isNearLinkUplink);

int32_t AudioSDKSetAOScene(td_handle adpHandle, td_handle sndHandle, const struct PortPluginAttr *inputAttr,
    bool toggleValid);

int32_t AudioSDKAODevClose(AOdevContext *aoDevCtx);

td_handle GetSoundHandle(const AOdevContext *aoDevCtx);

int32_t AudioSDKAdpOpen(td_handle *adpHandle);

int32_t AudioSDKAdpClose(td_handle adpHandle);

int32_t AudioSDKAdecOpen(td_handle *adecHandle, struct PortPluginAttr inputAttr);

int32_t AudioSDKAdecClose(td_handle adecHandle);

int32_t AudioSDKSeaOpen(td_handle *seaHandle, struct PortPluginAttr inputAttr);

int32_t AudioSDKSeaClose(td_handle seaHandle, enum AudioCategory type);

int32_t AudioSDKAencOpen(td_handle *aencHandle, uapi_aenc_attr aencAttr, AencType anecType, struct PortPluginAttr *inputAttr);

uapi_aenc_attr GetAencAttrInfo(uapi_acodec_id aencCodecId, struct PortPluginAttr inputAttr);

int32_t AudioSDKAencClose(td_handle aencHandle);

int32_t AudioSDKAttach(AudioSDKModuleType modType, td_handle srcHandle, td_handle dstHandle);

int32_t AudioSDKDetach(AudioSDKModuleType modType, td_handle srcHandle, td_handle dstHandle);

int32_t SaveToggleConfig(uapi_audio_toggle_config *toggleConfig, const char *value, uint32_t valueLen);

int32_t UpdateCurrentPosition(td_handle trackHandle, struct AudioTimeStamp *time);

bool isMultipleSeaOpen(void);

bool isMultipleAIOpen(void);

int32_t AudioSDKSetAIMute(td_handle aiHandle, bool mute);

int32_t AudioSDKGetAIMute(td_handle aiHandle, bool *mute);

int32_t AudioSDKSetTrackVolume(td_handle trackHandle, float vloume, enum AudioCategory type);

#ifdef MELODY_AUDIO_SUPPORT

typedef struct {
    AudioModule audioModule;
    uint32_t deviceId;
    uint32_t audioType;
    uint32_t audioPort;
    uint32_t audioChannel;
    uint32_t audioVolType;
    int32_t volume;
} AudioVolumeContext;

typedef enum {
    PLUGIN_MIC_AMIC_0 = 0x1,
    PLUGIN_MIC_AMIC_1 = 0x2,
    PLUGIN_MIC_AMIC_2 = 0x4,
    PLUGIN_MIC_AMIC_3 = 0x8,
    PLUGIN_MIC_DMIC_0 = 0x10,
    PLUGIN_MIC_DMIC_1 = 0x20,
    PLUGIN_MIC_DMIC_2 = 0x40,
    PLUGIN_MIC_DMIC_3 = 0x80,
    PLUGIN_MIC_TYPE_MAX,
} PluginMicType;

void RecordAudioDbVolume(const AudioVolumeContext *ctx);

bool ReadAudioDbVolume(AudioVolumeContext *ctx);

int32_t GetRealDbVolume(uapi_snd_gain *sndGain);

int32_t AudioSDKRecordAudioDbVolume(char *value, enum AudioCategory type);

#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)
int32_t AudioSDKSetDbVolume(td_handle handle, char *value, enum AudioCategory type);

int32_t AudioSDKGetDbVolume(char *value, uint32_t valueLen, enum AudioCategory type);

int32_t AudioSDKSetEncodeDataSourceParam(char *value, uint32_t valueLen);
#endif

#if (defined(MELODY_DEMO_DONGLE) || defined(MELODY_DEMO_EARPHONE)) || defined(MELODY_DEMO_SOUNDBAR)
int32_t AudioSetL2HCParam(char *value, uint32_t valueLen);
#endif
#endif

#ifdef AUDIO_HAID_SUPPORT
typedef enum {
    PLUGIN_HAID_VOLTAGE_1P2 = 0x2,  /*!<voltage level 2, correspond to voltage 1.2V */
    PLUGIN_HAID_VOLTAGE_1P3 = 0x4,  /*!<voltage level 4, correspond to voltage 1.3V */
    PLUGIN_HAID_VOLTAGE_1P4 = 0x6,  /*!<voltage level 6, correspond to voltage 1.4V */
    PLUGIN_HAID_VOLTAGE_1P5 = 0x8,  /*!<voltage level 8, correspond to voltage 1.5V */
    PLUGIN_HAID_VOLTAGE_1P6 = 0xa,  /*!<voltage level 10,correspond to voltage 1.6V */
    PLUGIN_HAID_VOLTAGE_MAX = 0x10, /*!<maximum voltage level */
} PluginHaidVoltage;

typedef struct {
    uint16_t usedLength;
    PluginMicType micType[PLUGIN_MIC_AMIC_1];
    PluginHaidVoltage voltage;
} PluginHaidAttr;
#endif

int32_t GetVolumeGain(int32_t volume, td_s32 *integer, td_s32 *decimal);

int32_t GetParamByValue(uint8_t *param, uint32_t paramLen, const char *value,
    uint32_t valueLen, uint32_t configLen);

int32_t SetSeaEnable(td_handle seaHandle, const char *value, uint32_t valueLen);

#ifdef MELODY_AUDIO_SUPPORT
int32_t GetMaxVolumeDb(void);
#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || \
    defined(MELODY_DEMO_HA)
void SetMicArrangementOrder(uapi_ai_attr *aiAttr);
#endif
#endif
int32_t ExtraParamsInvoke(AudioHandle portHandle, struct InvokeAttr *invokeAttr,
    struct AudioPortInvoke invokeList[], uint32_t listSize);
#endif

bool AudioPluginIsValidPin(enum AudioPortPin pin, const char *pluginName);

int32_t ParseKeyValueForExtra(const struct InvokeAttr *invokeAttr,
    char *key, int32_t keyLen, char *value, int32_t valueLen);

int32_t IsNeedSetExtraFlag(enum AudioFormat audioFormat, bool *IsSetExtraParam);

int32_t SetExtraData(struct PortPluginAttr *inputAttr, const char *value, uint32_t valueLen);
#ifdef AUDIO_SDK_SUPPORT
int32_t GetBuiltinAIAttr(uapi_ai_attr *aiAttr, const AIOpenAttr *aiOpenAttr);
int32_t GetBuiltinAOAttr(uapi_snd_attr *sndAttr, const struct PortPluginAttr *inputAttr);
#ifndef MELODY_AUDIO_SUPPORT
int32_t GetExternAIAttr(uapi_ai_attr *aiAttr);
int32_t GetExternAOAttr(uapi_snd_attr *sndAttr);
#endif
#endif
void CreateAudioLinkChr(enum AudioCategory audioCategory, bool isOutput, bool isBt);
void StartAudioLinkChr(enum AudioCategory audioCategory, bool isOutput, bool isBt);
void PauseAudioLinkChr(enum AudioCategory audioCategory, bool isBt);
void ResumeAudioLinkChr(enum AudioCategory audioCategory, bool isBt);
void StopAudioLinkChr(enum AudioCategory audioCategory, bool isOutput, bool isBt);
void DestoryAudioLinkChr(enum AudioCategory audioCategory, bool isOutput, bool isBt);
void SetAefExtraParam(td_handle soundHandle);
#ifdef SUPPORT_SEA_AHE_PARAM
void SetSeaLibId(int32_t libId);
int32_t GetSeaLibId(void);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
