/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
 * Description: buffer raw demuxer
 * Author: Media Software Group
 * Create: 2025-06-16
 */

#include "buffer_raw_demuxer.h"

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

#define MODULE_NAME "DemuxerRaw"
#define DEMUXER_NO_MEDIA_STREAM    (-1)
#define RET_FILE_EOF  2
#define RET_NODATA  3
#define PROBE_SCORE_GOOD 80
#define PROBE_SCORE_PERFECT 100
#define PTS_INTERVAL 33

typedef struct {
    MediaFileInfo fileInfo;
    FormatMediaParam selectInfo;
    DemuxerBufferStream strem;
    bool eof;
    int32_t readLen;
    unsigned char *probeCacheData;
    int32_t probeCacheDatalen;
    int32_t probeDataReadoffset;
    DataType flags;
    int32_t prepared;
    int64_t pts;
} BufferFormatMember;

/* fileName is the buffer address */
static int32_t BufferRawOpen(
    const DemuxerSource *source, const ProtocolFun *protocol, HalHandle *fmtHandle, int32_t *score)
{
    CHK_NULL_RETURN(fmtHandle, ERR_DEMUXER_NULL_PTR, "input param hanle null");
    CHK_NULL_RETURN(source, ERR_DEMUXER_NULL_PTR, "input param hanle null");
    CHK_FAILED_RETURN(source->type, DEMUXER_SOURCE_TYPE_STREAM, -1, "only support stream type");

    if (source->stream.ReadData == NULL || source->stream.GetReadableSize == NULL || source->stream.handle == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "stream param error");
        return MEDIA_HAL_ERR;
    }

    BufferFormatMember *member = (BufferFormatMember *)MediaHalCalloc(sizeof(BufferFormatMember));
    CHK_NULL_RETURN(member, ERR_DEMUXER_MEM_MALLOC, "malloc BufferFormatMember failed");
    member->strem = source->stream;
    member->eof = false;
    member->readLen = 0;
    member->probeCacheData = NULL;
    member->probeCacheDatalen = 0;
    member->probeDataReadoffset = 0;
    member->selectInfo.vidStreamId = DEMUXER_NO_MEDIA_STREAM;
    member->selectInfo.audStreamId = DEMUXER_NO_MEDIA_STREAM;
    member->flags = DATA_TYPE_NONE;
    member->prepared = false;
    member->pts = 0;
    *fmtHandle = (void*)member;
    *score = PROBE_SCORE_PERFECT;
    MEDIA_HAL_LOGI(MODULE_NAME, "buffer Demuxer open success!");
    return MEDIA_HAL_OK;
}

static int32_t ReadFromSource(BufferFormatMember *member, FormatPacket *fmtFrame)
{
    int32_t ret = MEDIA_HAL_OK;
    DataType flags = DATA_TYPE_NONE;
    DemuxerBufferStream *strem = &member->strem;
    int32_t readLen = strem->GetReadableSize(strem->handle);
    if (readLen <= 0) {
        return RET_NODATA;
    }
    fmtFrame->data = (uint8_t *)MediaHalMalloc((uint32_t)readLen);
    if (fmtFrame->data == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc buffer failed");
        return MEDIA_HAL_NO_MEM;
    }
    readLen = strem->ReadData(strem->handle, fmtFrame->data, readLen, 0, &flags);
    if (readLen <= 0 && flags != DATA_TYPE_EOS) {
        ret = MEDIA_HAL_ERR;
        MEDIA_HAL_LOGE(MODULE_NAME, "read data failed = %d", ret);
        goto EXIT;
    }
    if ((readLen == 0) && flags == DATA_TYPE_EOS) {
        member->eof = true;
        ret = RET_FILE_EOF;
        fmtFrame->len = 0;
        MEDIA_HAL_LOGE(MODULE_NAME, "read data DATA_TYPE_EOS = %d", readLen);
        goto EXIT;
    }
    if (flags == DATA_TYPE_AUDIO_FRAME) {
        fmtFrame->packetType = PACKET_TYPE_AUDIO;
        fmtFrame->isKeyFrame = false;
    } else if (flags == DATA_TYPE_VIDEO_FRAME) {
        fmtFrame->packetType = PACKET_TYPE_VIDEO;
        fmtFrame->isKeyFrame = false;
    } else if (flags == DATA_TYPE_VIDEO_KEY_FRAME) {
        fmtFrame->packetType = PACKET_TYPE_VIDEO;
        fmtFrame->isKeyFrame = true;
    } else {
        fmtFrame->packetType = PACKET_TYPE_NONE;
        fmtFrame->isKeyFrame = false;
    }
    fmtFrame->len = (uint32_t)readLen;
    fmtFrame->timestampUs = member->pts;
    member->readLen += readLen;
    member->pts += PTS_INTERVAL;
EXIT:
    if (readLen <= 0 && fmtFrame->data != NULL) {
        MediaHalFree(fmtFrame->data);
        fmtFrame->data = NULL;
    }
    return ret;
}

static int32_t BufferRawRead(void *fmtHandle, FormatPacket *fmtFrame)
{
    BufferFormatMember *member = (BufferFormatMember *)fmtHandle;
    CHK_NULL_RETURN(member, ERR_DEMUXER_INVALID_HANDLE, "input param hanle null");
    CHK_NULL_RETURN(fmtFrame, ERR_DEMUXER_ILLEGAL_PARAM, "input param fmtFrame null");
    CHK_FAILED_RETURN(member->prepared, true, -1, "not prepared");

    if (member->eof == true) {
        fmtFrame->data = NULL;
        fmtFrame->len = 0;
        return RET_FILE_EOF;
    }
    int32_t ret = ReadFromSource(member, fmtFrame);
    CHK_FAILED_RETURN_NO_LOG(ret, 0, ret);
    return MEDIA_HAL_OK;
}

static int32_t BufferRawClose(void *fmtHandle)
{
    CHK_NULL_RETURN(fmtHandle, ERR_DEMUXER_INVALID_HANDLE, "input param hanle null");
    BufferFormatMember *member = (BufferFormatMember *)fmtHandle;
    if (member != NULL) {
        MediaHalFree(member);
        member = NULL;
    }
    return MEDIA_HAL_OK;
}

static int32_t BufferRawFree(void *fmtHandle, FormatPacket *fmtFrame)
{
    BufferFormatMember *member = (BufferFormatMember *)fmtHandle;
    CHK_NULL_RETURN(member, ERR_DEMUXER_INVALID_HANDLE, "input param hanle null");
    CHK_NULL_RETURN(fmtFrame, ERR_DEMUXER_ILLEGAL_PARAM, "input param fmtFrame null");
    CHK_FAILED_RETURN(member->prepared, true, -1, "not prepared");

    if (fmtFrame->data != NULL) {
        MediaHalFree(fmtFrame->data);
        fmtFrame->data = NULL;
    }
    return MEDIA_HAL_OK;
}

static int32_t BufferRawSeek(__attribute__((unused)) HalHandle fmtHandle, __attribute__((unused))int32_t streamIndex,
    __attribute__((unused)) int64_t toMs, __attribute__((unused)) uint32_t flag)
{
    BufferFormatMember *member = (BufferFormatMember *)fmtHandle;
    CHK_NULL_RETURN(member, ERR_DEMUXER_INVALID_HANDLE, "input param fmtHandle null");
    CHK_FAILED_RETURN(member->prepared, true, -1, "not prepared");
    MEDIA_HAL_LOGE(MODULE_NAME, "not support");
    return MEDIA_HAL_ERR;
}

static int32_t BufferRawProbe(void *fmtHandle, void *arg)
{
    MEDIA_HAL_UNUSED(arg);
    BufferFormatMember *member = (BufferFormatMember *)fmtHandle;
    CHK_NULL_RETURN(member, ERR_DEMUXER_INVALID_HANDLE, "input param hanle null");
    CHK_NULL_RETURN(member->strem.handle, -1, "input param hanle null");
    CHK_FAILED_RETURN(member->prepared, false, 0, "have prepared");
    member->prepared = true;
    return MEDIA_HAL_OK;
}

static int32_t BufferRawGetInfo(void *fmtHandle, MediaFileInfo *fmtInfo)
{
    BufferFormatMember *member = (BufferFormatMember *)fmtHandle;

    CHK_NULL_RETURN(member, ERR_DEMUXER_INVALID_HANDLE, "input param hanle null");
    CHK_NULL_RETURN(fmtInfo, ERR_DEMUXER_ILLEGAL_PARAM, "input param fmtInfo null");
    CHK_FAILED_RETURN(member->prepared, true, -1, "not prepared");

    *fmtInfo = member->fileInfo;
    return MEDIA_HAL_OK;
}

static int32_t BufferRawFind(const DemuxerSource *source, const ProtocolFun *protocol)
{
    MEDIA_HAL_UNUSED(protocol);
    if (source == NULL) {
        return ERR_DEMUXER_ILLEGAL_PARAM;
    }
    if (source->type != DEMUXER_SOURCE_TYPE_STREAM) {
        MEDIA_HAL_LOGI(MODULE_NAME, "only support stream source type");
        return ERR_DEMUXER_NOT_SUPPORT;
    }
    return MEDIA_HAL_OK;
}

/* ffmpeg demuxer symbol */
DemuxerPluginEntry g_bufferRawDemuxerEntry = {
    .desc = {
        .libName = "libplugin_buffert_raw_demuxer.so",
        .name = "buffert_raw_demuxer",
        .version = "version 1.0.0",
        .description = "buffert raw demuxer!",
        .supportCapability = "h264 aac",
        .priority = PROBE_SCORE_PERFECT,
    },
    .fmt_find = BufferRawFind,
    .fmt_open = BufferRawOpen,
    .fmt_find_stream = BufferRawProbe,
    .fmt_read = BufferRawRead,
    .fmt_free = BufferRawFree,
    .fmt_seek_pts = BufferRawSeek,
    .fmt_seek_pos = NULL,
    .fmt_getinfo = BufferRawGetInfo,
    .fmt_invoke = NULL,
    .fmt_close = BufferRawClose,
    .fmt_select_track = NULL,
};

#ifdef ENABLE_DL_DEMUXER_PLUGIN
const DemuxerPluginEntry *GetDemuxer(void)
{
    return &g_bufferRawDemuxerEntry;
}
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
