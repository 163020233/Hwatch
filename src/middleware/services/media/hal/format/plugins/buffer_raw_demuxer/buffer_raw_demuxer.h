/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
 * Description: buffer raw demuxer header
 * Author: Media Software Group
 * Create: 2025-06-16
 */

#ifndef BUFFER_RAW_DEMUXER_H
#define BUFFER_RAW_DEMUXER_H

#include "demuxer_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#ifdef ENABLE_DL_DEMUXER_PLUGIN
    const DemuxerPluginEntry *GetDemuxer(void);
#else
    extern DemuxerPluginEntry g_bufferRawDemuxerEntry;
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HI_BUFFER_FORMATH_H */
