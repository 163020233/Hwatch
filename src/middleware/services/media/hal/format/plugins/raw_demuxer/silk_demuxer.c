/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: silk demuxer module file
 * Author: Media Software Group
 * Create: 2022-12-10
 */

#include "silk_demuxer.h"

#include <errno.h>
#include <unistd.h>
#include "securec.h"
#include "demuxer_common_err.h"
#include "demuxer_interface.h"
#include "media_hal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "silk_demuxer"

#define SHIFT_24 24
#define SHIFT_16 16
#define SHIFT_8 8
#define MASK_BITS_3 0x07
#define MASK_BYTE 0xFF

#define URL_LEN_MAX 256
#define PROBE_CACHE_BUF_SIZE 16
#define SILK_HEAD_SIZE 9
#define SILK_DEFAULT_DURATION_PER_FRAME 20000 /* 20ms */
#define SILK_SAMPLRATE_8K 8000
#define SILK_SAMPLRATE_12K 12000
#define SILK_SAMPLRATE_24K 24000
#define SILK_DEFAULT_SAMPLRATE_16K 16000 /* Default sampling rate of silk */
#define SILK_DEFAULT_CHANNEL 1 /* Default number of silk channels */
#define SILK_DEMUXER_NAME "silk_v3"
#define SILK_HEAD_INFO "#!SILK_V3"
#define PROBE_SCORE_PERFECT 100

typedef struct {
    DemuxerOpsIntf intf;
    char url[URL_LEN_MAX];
    MediaFileInfo fileInfo;
    uint8_t silkHead[SILK_HEAD_SIZE];
    char name[SILK_HEAD_SIZE];
    bool eof;
    bool probeSuccessed;
    bool firstFrame;
    int64_t position;
    int32_t sampleRate;
    int32_t channel;
    int32_t readLen;
    int32_t prepared;
    FILE *file;
    int64_t fileSeekStartPosition;
} SilkRawDemuxer;

static HalHandle SilkDemuxerOpen(const char *url)
{
    if (url == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "input param uri null");
        return (HalHandle)NULL;
    }

    SilkRawDemuxer *demuxer = (SilkRawDemuxer *)MediaHalMalloc(sizeof(SilkRawDemuxer));
    if (demuxer == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
        return (HalHandle)NULL;
    }
    if (memset_s(demuxer, sizeof(SilkRawDemuxer), 0, sizeof(SilkRawDemuxer)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        goto FREE;
    }
    if (strncpy_s(demuxer->url, URL_LEN_MAX, url, URL_LEN_MAX - 1) != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "strncpy_s failed");
        goto FREE;
    }
    if (snprintf_s(demuxer->name, SILK_HEAD_SIZE, SILK_HEAD_SIZE - 1, SILK_DEMUXER_NAME)  < 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "snprintf_s failed");
        goto FREE;
    }

    demuxer->file = fopen(demuxer->url, "rb");
    if (demuxer->file == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "silk file fopen failed, errno:%d", errno);
        goto FREE;
    }
    demuxer->eof = false;
    demuxer->probeSuccessed = false;
    demuxer->sampleRate = SILK_DEFAULT_SAMPLRATE_16K;
    demuxer->channel = SILK_DEFAULT_CHANNEL;
    demuxer->prepared = false;
    demuxer->firstFrame = true;
    demuxer->position = 0;
    demuxer->fileSeekStartPosition = 0;
    MEDIA_HAL_LOGI(MODULE_NAME, "buffer Demuxer open");
    return (HalHandle)demuxer;
FREE:
    MediaHalFree(demuxer);
    return NULL;
}

static void SilkDemuxerClose(HalHandle fmtHandle)
{
    if (fmtHandle == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "input hanle null");
        return;
    }

    SilkRawDemuxer *demuxer = (SilkRawDemuxer *)fmtHandle;

    if (demuxer->fileInfo.programInfo != NULL) {
        if (demuxer->fileInfo.programInfo[0].track != NULL) {
            MediaHalFree(demuxer->fileInfo.programInfo[0].track);
            demuxer->fileInfo.programInfo[0].track = NULL;
        }
        MediaHalFree(demuxer->fileInfo.programInfo);
        demuxer->fileInfo.programInfo = NULL;
    }

    if (demuxer->file != NULL) {
        if (fclose(demuxer->file) != 0) {
            MEDIA_HAL_LOGE(MODULE_NAME, "fclose falied");
        }
    }

    MediaHalFree(demuxer);
}

static void FillSilkHeadInfo(SilkRawDemuxer *demuxer, uint8_t *data, uint32_t *len)
{
    for (uint32_t i = 0; i < SILK_HEAD_SIZE; i++) {
        data[i] = demuxer->silkHead[i];
    }
    *len += SILK_HEAD_SIZE;
}

static void FillFrameCommonInfo(FormatPacket *fmtFrame, int64_t timestamp)
{
    fmtFrame->packetType = PACKET_TYPE_AUDIO;
    fmtFrame->trackId = 0;
    fmtFrame->isKeyFrame = true;
    fmtFrame->timestampUs = timestamp;
    fmtFrame->durationUs = SILK_DEFAULT_DURATION_PER_FRAME;
    fmtFrame->frameIndex = 0;
    fmtFrame->itemCnt = 0;
    fmtFrame->item = NULL;
}
static int32_t SilkDemuxerReadFrameSizeAndPlayData(SilkRawDemuxer *demuxer, FormatPacket *fmtFrame,
    uint32_t *len, uint16_t length, uint16_t lengthSize)
{
    uint32_t bufLen = length + lengthSize + SILK_HEAD_SIZE;
    if (demuxer->firstFrame) {
        FillSilkHeadInfo(demuxer, fmtFrame->data, len);
    }
    for (uint16_t i = 0; i < lengthSize; ++i) {
        if ((*len + i) < bufLen) {
            fmtFrame->data[*len + i] = (length >> (SHIFT_8 * i)) & MASK_BYTE;
        }
    }
    *len += lengthSize;
    size_t readLen = fread(fmtFrame->data + *len, 1, length, demuxer->file);
    if (readLen < length) {
        MediaHalFree(fmtFrame->data);
        fmtFrame->data = NULL;
        fmtFrame->len = 0;
        MEDIA_HAL_LOGI(MODULE_NAME, "fread failed, eof:%d", feof(demuxer->file));
        demuxer->eof = true;
        return RET_FILE_EOF;
    }
    *len += length;
    return MEDIA_HAL_OK;
}

static int32_t SilkDemuxerReadFrame(HalHandle fmtHandle, FormatPacket *fmtFrame)
{
    SilkRawDemuxer *demuxer = (SilkRawDemuxer *)fmtHandle;
    if (demuxer->eof) {
        fmtFrame->data = NULL;
        fmtFrame->len = 0;
        return RET_FILE_EOF;
    }
    uint16_t length;
    size_t lengthSize = sizeof(uint16_t);
    size_t readLen = fread(&length, 1, lengthSize, demuxer->file);
    if (readLen < lengthSize) {
        fmtFrame->data = NULL;
        fmtFrame->len = 0;
        MEDIA_HAL_LOGI(MODULE_NAME, "fread failed, eof:%d", feof(demuxer->file));
        demuxer->eof = true;
        return RET_FILE_EOF;
    }
    uint32_t bufLen = length + lengthSize + SILK_HEAD_SIZE;
    fmtFrame->data = (uint8_t*)MediaHalMalloc(bufLen);
    if (fmtFrame->data == NULL) {
        fmtFrame->len = 0;
        fseek(demuxer->file, -lengthSize, SEEK_CUR);
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc buffer failed");
        return MEDIA_HAL_NO_MEM;
    }
    uint32_t len = 0;
    int32_t ret = SilkDemuxerReadFrameSizeAndPlayData(demuxer, fmtFrame, &len, length, lengthSize);
    if (ret != MEDIA_HAL_OK) {
        return ret;
    }
    fmtFrame->len = len;
    FillFrameCommonInfo(fmtFrame, -1);
    demuxer->firstFrame = false;
    demuxer->position += (int64_t)fmtFrame->len;
    return MEDIA_HAL_OK;
}

static int32_t SilkDemuxerFreeFrame(HalHandle fmtHandle, FormatPacket *fmtFrame)
{
    if (fmtFrame->data != NULL) {
        MediaHalFree(fmtFrame->data);
        fmtFrame->data = NULL;
    }
    MEDIA_HAL_UNUSED(fmtHandle);
    return MEDIA_HAL_OK;
}

static int32_t SilkDemuxerSeek(__attribute__((unused)) HalHandle fmtHandle, __attribute__((unused)) int64_t toMs,
    __attribute__((unused)) uint32_t flag)
{
    SilkRawDemuxer *demuxer = (SilkRawDemuxer *)fmtHandle;
    if (toMs == 0) {
        int32_t ret = fseek(demuxer->file, demuxer->fileSeekStartPosition, SEEK_SET);
        if (ret != 0) {
            MEDIA_HAL_LOGE(MODULE_NAME, "failed to seek to the silk file header.");
            return MEDIA_HAL_ERR;
        }
        demuxer->eof = false;
        demuxer->position = demuxer->fileSeekStartPosition;
        MEDIA_HAL_LOGI(MODULE_NAME, "SilkRawDemuxer seek success = %d!!", ftell(demuxer->file));
    } else {
        MEDIA_HAL_LOGE(MODULE_NAME, "not support");
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static int32_t ProbeScore(SilkRawDemuxer *demuxer, int32_t *score, int32_t offset)
{
    demuxer->probeSuccessed = true;
    demuxer->position = offset + SILK_HEAD_SIZE;
    demuxer->fileSeekStartPosition = offset + SILK_HEAD_SIZE;
    if (fseek(demuxer->file, offset + SILK_HEAD_SIZE, SEEK_SET) != 0) {
        return MEDIA_HAL_ERR;
    }
    if (score != NULL) {
        *score = PROBE_SCORE_PERFECT;
    }
    return MEDIA_HAL_OK;
}
static int32_t SilkDemuxerProbe(HalHandle fmtHandle, int32_t *score)
{
    SilkRawDemuxer *demuxer = (SilkRawDemuxer *)fmtHandle;
    CHK_FAILED_RETURN(demuxer->prepared, false, 0, "have prepared");
    CHK_NULL_RETURN(demuxer->file, ERR_DEMUXER_INVALID_HANDLE, "file handle null");
    uint8_t data[PROBE_CACHE_BUF_SIZE];
    int32_t offset = 0;
    size_t readLen = fread(data, 1, SILK_HEAD_SIZE, demuxer->file);
    if (readLen < SILK_HEAD_SIZE) {
        MEDIA_HAL_LOGI(MODULE_NAME, "file size < %d", PROBE_CACHE_BUF_SIZE);
        return MEDIA_HAL_ERR;
    }
    data[SILK_HEAD_SIZE] = '\0';
    if (strcmp((const char*)&data[offset], SILK_HEAD_INFO) != 0) {
        MEDIA_HAL_LOGI(MODULE_NAME, "not silk_v3 file");
        return MEDIA_HAL_ERR;
    }
    if (memcpy_s(demuxer->silkHead, SILK_HEAD_SIZE, &data[offset], SILK_HEAD_SIZE) != 0) {
        MEDIA_HAL_LOGI(MODULE_NAME, "memcpy_s failed");
        return MEDIA_HAL_ERR;
    }
    demuxer->sampleRate = SILK_DEFAULT_SAMPLRATE_16K;
    return ProbeScore(demuxer, score, offset);
}

static int32_t SilkDemuxerFindStreamInfo(HalHandle fmtHandle)
{
    SilkRawDemuxer *demuxer = (SilkRawDemuxer *)fmtHandle;
    if (demuxer->prepared != 0) {
        return MEDIA_HAL_OK;
    }
    demuxer->fileInfo.formatName = demuxer->name;
    demuxer->fileInfo.bitrate = 0;
    demuxer->fileInfo.programNum = 1;
    demuxer->fileInfo.programInfo =
        (MediaProgramInfo *)MediaHalMalloc(sizeof(MediaProgramInfo) * demuxer->fileInfo.programNum);
    if (demuxer->fileInfo.programInfo == NULL) {
        demuxer->fileInfo.programNum = 0;
        MEDIA_HAL_LOGI(MODULE_NAME, "silk programInfo malloc failed");
        return MEDIA_HAL_NO_MEM;
    }
    demuxer->fileInfo.programInfo[0].programId = 0;
    demuxer->fileInfo.programInfo[0].durationMs = -1;
    demuxer->fileInfo.programInfo[0].trackNum = 1;
    demuxer->fileInfo.programInfo[0].track = (MediaTrackInfo *)MediaHalMalloc(sizeof(MediaTrackInfo) *
        demuxer->fileInfo.programInfo[0].trackNum);
    if (demuxer->fileInfo.programInfo[0].track == NULL) {
        demuxer->fileInfo.programInfo[0].trackNum = 0;
        MEDIA_HAL_LOGI(MODULE_NAME, "silk track malloc failed");
        return MEDIA_HAL_NO_MEM;
    }
    MediaTrackInfo *track = demuxer->fileInfo.programInfo[0].track;
    track[0].trackType = MEDIA_TRACK_TYPE_AUDIO;
    track[0].trackId = 0;
    track[0].audTrack.format = CODEC_TYPE_SILK;
    track[0].audTrack.profile = 0;
    track[0].audTrack.sampleRate = (uint32_t)demuxer->sampleRate;
    track[0].audTrack.sampleFmt = 1;
    track[0].audTrack.channels = demuxer->channel;
    track[0].audTrack.subStreamID = -1;
    track[0].audTrack.bitrate = 0;
    track[0].audTrack.language[0] = '\0';
    track[0].audTrack.durationMs = -1;
    track[0].itemCnt = 0;
    track[0].item = NULL;

    demuxer->prepared = true;
    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t SilkDemuxerGetInfo(const HalHandle fmtHandle, MediaFileInfo *fmtInfo)
{
    SilkRawDemuxer *demuxer = (SilkRawDemuxer *)fmtHandle;
    *fmtInfo = demuxer->fileInfo;
    return MEDIA_HAL_OK;
}

static DemuxerOpsIntf g_silkDemuxerOps = {
    .Open = SilkDemuxerOpen,
    .Probe = SilkDemuxerProbe,
    .FindStreamInfo = SilkDemuxerFindStreamInfo,
    .GetInfo = SilkDemuxerGetInfo,
    .ReadFrame = SilkDemuxerReadFrame,
    .FreeFrame = SilkDemuxerFreeFrame,
    .Seek = SilkDemuxerSeek,
    .Close = SilkDemuxerClose,
};

DemuxerOpsIntf *GetSilkDemuxer(void)
{
    return &g_silkDemuxerOps;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
