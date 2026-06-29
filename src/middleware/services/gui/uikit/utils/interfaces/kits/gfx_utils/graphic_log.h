/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GRAPHIC_LITE_GRAPHIC_LOG_H
#define GRAPHIC_LITE_GRAPHIC_LOG_H

#include "graphic_config.h"
#if ENABLE_GRAPHIC_LOG
#include <cstdio>
#endif
#ifdef USE_DIAG_LOG
#include "soc_diag_util.h"
#include "log_oam_logger.h"
#include "log_def.h"
#endif
#include "dfx/dfx_chr.h"

namespace OHOS {
#if ENABLE_GRAPHIC_LOG
#ifdef USE_DIAG_LOG
#define GRAPHIC_LOGF(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#define GRAPHIC_LOGE(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#define GRAPHIC_LOGW(fmt, args...)
#define GRAPHIC_LOGI(fmt, args...)
#define GRAPHIC_LOGD(fmt, args...)
#define GRAPHIC_LOGP(fmt, args...) uapi_diag_info_log(0, fmt, ##args)
#elif defined VERSION_IOT
typedef enum {
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_MAX
} LOG_LEVEL;

#define GRAPHIC_LOGF(fmt, ...)                                                                    \
    do {                                                                                          \
        if ((OHOS::LOG_LEVEL::LOG_LEVEL_FATAL <= GRAPHIC_LOG_LEVEL) &&                            \
            (OHOS::LOG_LEVEL::LOG_LEVEL_FATAL < OHOS::LOG_LEVEL::LOG_LEVEL_MAX)) {                \
            printf("[Graphic Fatal] [%s: %d]" fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);   \
            GRAPHIC_CHR_F("[%s: %d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);                 \
        }                                                                                         \
    } while (0)

#define GRAPHIC_LOGE(fmt, ...)                                                                    \
    do {                                                                                          \
        if ((OHOS::LOG_LEVEL::LOG_LEVEL_ERROR <= GRAPHIC_LOG_LEVEL) &&                            \
            (OHOS::LOG_LEVEL::LOG_LEVEL_ERROR < OHOS::LOG_LEVEL::LOG_LEVEL_MAX)) {                \
            printf("[Graphic Error] [%s: %d]" fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);   \
            GRAPHIC_CHR_E("[%s: %d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);                 \
        }                                                                                         \
    } while (0)

#define GRAPHIC_LOGW(fmt, ...)                                                                    \
    do {                                                                                          \
        if ((OHOS::LOG_LEVEL::LOG_LEVEL_WARN <= GRAPHIC_LOG_LEVEL) &&                             \
            (OHOS::LOG_LEVEL::LOG_LEVEL_WARN < OHOS::LOG_LEVEL::LOG_LEVEL_MAX)) {                 \
            printf("[Graphic Warning] [%s: %d]" fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            GRAPHIC_CHR_W("[%s: %d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);                 \
        }                                                                                         \
    } while (0)

#define GRAPHIC_LOGI(fmt, ...)                                                                    \
    do {                                                                                          \
        if ((OHOS::LOG_LEVEL::LOG_LEVEL_INFO <= GRAPHIC_LOG_LEVEL) &&                             \
            (OHOS::LOG_LEVEL::LOG_LEVEL_INFO < OHOS::LOG_LEVEL::LOG_LEVEL_MAX)) {                 \
            printf("[Graphic Info] [%s: %d]" fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);    \
            GRAPHIC_CHR_I("[%s: %d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);                 \
        }                                                                                         \
    } while (0)

#define GRAPHIC_LOGD(fmt, ...)                                                                    \
    do {                                                                                          \
        if ((OHOS::LOG_LEVEL::LOG_LEVEL_DEBUG <= GRAPHIC_LOG_LEVEL) &&                            \
            (OHOS::LOG_LEVEL::LOG_LEVEL_DEBUG < OHOS::LOG_LEVEL::LOG_LEVEL_MAX)) {                \
            printf("[Graphic Debug] [%s: %d]" fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);   \
            GRAPHIC_CHR_D("[%s: %d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);                 \
        }                                                                                         \
    } while (0)

#define GRAPHIC_LOGP(fmt, ...)  printf(fmt "\n", ##__VA_ARGS__)
#endif
#else // ENABLE_GRAPHIC_LOG
#define GRAPHIC_LOGF(...)
#define GRAPHIC_LOGE(...)
#define GRAPHIC_LOGW(...)
#define GRAPHIC_LOGI(...)
#define GRAPHIC_LOGD(...)
#define GRAPHIC_LOGP(...)
#endif // ENABLE_GRAPHIC_LOG
} // namespace OHOS
#endif // GRAPHIC_LITE_GRAPHIC_LOG_H
