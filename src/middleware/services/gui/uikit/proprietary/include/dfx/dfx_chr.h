/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: dfx chr
 * Author: Hisi Team
 * Created: 2025-07
 */

#ifndef GRAPHIC_LITE_DFX_CHR_H
#define GRAPHIC_LITE_DFX_CHR_H
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "securec.h"
#include "massdata_buffer.h"

namespace OHOS {
enum class GraphicChrInfo1 : uint8_t {
    CHR_LEVEL_FATAL,
    CHR_LEVEL_ERROR,
    CHR_LEVEL_WARN,
    CHR_LEVEL_INFO,
    CHR_LEVEL_DEBUG,
};

extern char g_GraphicLogBuf[MAX_INFO3_DATA_SIZE];
void GraphicChrRecord(GraphicChrInfo1 info);

#define GRAPHIC_CHR_F(fmt, ...)                                                                                   \
    do {                                                                                                          \
        snprintf_s(OHOS::g_GraphicLogBuf, MAX_INFO3_DATA_SIZE, MAX_INFO3_DATA_SIZE - 1, fmt "\n", ##__VA_ARGS__); \
        OHOS::GraphicChrRecord(OHOS::GraphicChrInfo1::CHR_LEVEL_FATAL);                                           \
    } while (0)

#define GRAPHIC_CHR_E(fmt, ...)                                                                                   \
    do {                                                                                                          \
        snprintf_s(OHOS::g_GraphicLogBuf, MAX_INFO3_DATA_SIZE, MAX_INFO3_DATA_SIZE - 1, fmt "\n", ##__VA_ARGS__); \
        OHOS::GraphicChrRecord(OHOS::GraphicChrInfo1::CHR_LEVEL_ERROR);                                           \
    } while (0)

#define GRAPHIC_CHR_W(fmt, ...)                                                                                   \
    do {                                                                                                          \
        snprintf_s(OHOS::g_GraphicLogBuf, MAX_INFO3_DATA_SIZE, MAX_INFO3_DATA_SIZE - 1, fmt "\n", ##__VA_ARGS__); \
        OHOS::GraphicChrRecord(OHOS::GraphicChrInfo1::CHR_LEVEL_WARN);                                            \
    } while (0)

#define GRAPHIC_CHR_I(fmt, ...)                                                                                   \
    do {                                                                                                          \
        snprintf_s(OHOS::g_GraphicLogBuf, MAX_INFO3_DATA_SIZE, MAX_INFO3_DATA_SIZE - 1, fmt "\n", ##__VA_ARGS__); \
        OHOS::GraphicChrRecord(OHOS::GraphicChrInfo1::CHR_LEVEL_INFO);                                            \
    } while (0)

#define GRAPHIC_CHR_D(fmt, ...)                                                                                   \
    do {                                                                                                          \
        snprintf_s(OHOS::g_GraphicLogBuf, MAX_INFO3_DATA_SIZE, MAX_INFO3_DATA_SIZE - 1, fmt "\n", ##__VA_ARGS__); \
        OHOS::GraphicChrRecord(OHOS::GraphicChrInfo1::CHR_LEVEL_DEBUG);                                           \
    } while (0)
}  // namespace OHOS
#endif  // GRAPHIC_LITE_DFX_CHR_H