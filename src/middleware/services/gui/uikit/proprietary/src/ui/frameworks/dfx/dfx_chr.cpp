/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: dfx chr
 * Author: Hisi Team
 * Created: 2025-07
 */

#include <cstring>
#include "dfx/dfx_chr.h"
namespace OHOS {
char g_GraphicLogBuf[MAX_INFO3_DATA_SIZE];
void GraphicChrRecord(GraphicChrInfo1 info)
{
    uapi_massdata_record_system_long_event(
        MT_GRAPHIC_LOG, static_cast<uint8_t>(info), 0,
        reinterpret_cast<uint8_t*>(g_GraphicLogBuf), strnlen(g_GraphicLogBuf, sizeof(g_GraphicLogBuf)));
}

} // namespace OHOS