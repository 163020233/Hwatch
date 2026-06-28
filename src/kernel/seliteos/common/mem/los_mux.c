/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: mux implementation file.
 *
 * Create: 2024-07-23
*/


#include "los_mux.h"

#define UNUSED(x) (void)(x)

UINT32 LOS_MuxCreate(UINT32 *sId)
{
    UNUSED(sId);
    return LOS_OK;
}

UINT32 LOS_MuxDelete(UINT32 sId)
{
    UNUSED(sId);
    return LOS_OK;
}

UINT32 LOS_MuxPend(UINT32 sId, UINT32 timeout)
{
    UNUSED(sId);
    UNUSED(timeout);
    return LOS_OK;
}

UINT32 LOS_MuxPost(UINT32 sId)
{
    UNUSED(sId);
    return LOS_OK;
}
