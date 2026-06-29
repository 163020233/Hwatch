/*
Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
*/
#include <securec.h>
#include "soc_gmmu_api.h"
#include "display_gmmu_internal.h"
#include "display_common.h"
#include "graphic_hardware_config.h"

#if ENABLE_GMMU
td_s32 GmmuInit(const GmmuAttr *gmmu_attr)
{
    struct gmmu_attr_struct uapi_gmmu_attr = {0};

    CHECK_NULLPOINTER_RETURN_VALUE(gmmu_attr, DISPLAY_FAILURE);
    uapi_gmmu_attr.bpp = gmmu_attr->bpp;
    uapi_gmmu_attr.height = gmmu_attr->height;
    uapi_gmmu_attr.width = gmmu_attr->width;
    uapi_gmmu_attr.offset = gmmu_attr->offset;

    return uapi_gmmu_init(&uapi_gmmu_attr);
}

td_void GmmuAlloc(GmmuBuf *gmmu_buf)
{
    struct gmmu_buf_struct uapi_gmmu_buf = {0};

    CHECK_NULLPOINTER_RETURN(gmmu_buf);
    uapi_gmmu_buf.stride = gmmu_buf->stride;
    uapi_gmmu_buf.size = gmmu_buf->size;
    memcpy_s(uapi_gmmu_buf.buffer, sizeof(uapi_gmmu_buf.buffer), gmmu_buf->buffer, sizeof(gmmu_buf->buffer));

    uapi_gmmu_alloc(&uapi_gmmu_buf);

    gmmu_buf->stride = uapi_gmmu_buf.stride;
    gmmu_buf->size = uapi_gmmu_buf.size;
    memcpy_s(gmmu_buf->buffer, sizeof(gmmu_buf->buffer), uapi_gmmu_buf.buffer, sizeof(uapi_gmmu_buf.buffer));
}

td_void GmmuFree(GmmuBuf *gmmu_buf)
{
    struct gmmu_buf_struct uapi_gmmu_buf = {0};

    CHECK_NULLPOINTER_RETURN(gmmu_buf);
    uapi_gmmu_buf.stride = gmmu_buf->stride;
    uapi_gmmu_buf.size = gmmu_buf->size;
    memcpy_s(uapi_gmmu_buf.buffer, sizeof(uapi_gmmu_buf.buffer), gmmu_buf->buffer, sizeof(gmmu_buf->buffer));

    uapi_gmmu_free(&uapi_gmmu_buf);

    gmmu_buf->stride = uapi_gmmu_buf.stride;
    gmmu_buf->size = uapi_gmmu_buf.size;
    memcpy_s(gmmu_buf->buffer, sizeof(gmmu_buf->buffer), uapi_gmmu_buf.buffer, sizeof(uapi_gmmu_buf.buffer));
}

td_void GmmuDeinit(td_void)
{
    uapi_gmmu_deinit();
}
#endif