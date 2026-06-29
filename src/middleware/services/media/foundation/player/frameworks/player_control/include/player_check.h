/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef PLAYER_CHECK_H
#define PLAYER_CHECK_H

#include "media_log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#if defined(SW_UART_DEBUG) || defined(SW_RTT_DEBUG)
#define CHECK_NULL_RETURN(ptr, ret, outStr) \
do { \
    if ((ptr) == nullptr) { \
        MEDIA_ERR_LOG("%s ", ((outStr) != nullptr) ? (outStr) : " "); \
        return (ret); \
    } \
} while (0)

#define CHECK_NULL_RETURN_NONE(ptr, outStr) \
do { \
    if ((ptr) == nullptr) { \
        MEDIA_ERR_LOG("%s ", ((outStr) != nullptr) ? (outStr) : " "); \
        return; \
    } \
} while (0)

#define CHECK_FAILED_RETURN(value, target, ret, outStr) \
do { \
    auto temp = value; \
    if ((temp) != (target)) { \
        MEDIA_ERR_LOG("%s %d != %d", ((outStr) != nullptr) ? (outStr) : " ", (value), (target)); \
        return (ret); \
    } \
} while (0)

#define CHECK_FAILED_RETURN_NONE(value, target, outStr) \
do { \
    auto temp = value; \
    if ((temp) != (target)) { \
        MEDIA_ERR_LOG("%s, %d != %d", ((outStr) != nullptr) ? (outStr) : " ", (value), (target)); \
        return; \
    } \
} while (0)

#define CHECK_TRUE_CONDITION_RETURN(value, ret, outStr) \
do { \
    if ((value)) { \
        MEDIA_ERR_LOG("%s", ((outStr) != nullptr) ? (outStr) : " "); \
        return (ret); \
    } \
} while (0)

#define CHECK_FALSE_CONDITION_RETURN(value, ret, outStr) \
do { \
    if (!(value)) { \
        MEDIA_ERR_LOG("%s", ((outStr) != nullptr) ? (outStr) : " "); \
        return (ret); \
    } \
} while (0)

#define CHECK_FALSE_CONDITION_RETURN_NONE(value, outStr) \
do { \
    if (!(value)) { \
        MEDIA_ERR_LOG("%s", ((outStr) != nullptr) ? (outStr) : " "); \
        return; \
    } \
} while (0)

#define CHECK_TRUE_CONDITION_RETURN_NONE(value, outStr) \
do { \
    if ((value)) { \
        MEDIA_ERR_LOG("%s", ((outStr) != nullptr) ? (outStr) : " "); \
        return; \
    } \
} while (0)

#define CHECK_FAILED_PRINT(value, target, outStr) \
do { \
    auto temp = value; \
    if ((temp) != (target)) { \
        MEDIA_ERR_LOG("%s, %d != %d", (outStr) ? (outStr) : " ", (value), (target)); \
    } \
} while (0)

#define CHECK_STATE_SAME_RETURN(srcState, dstState, outStr) \
do { \
    if ((dstState) == (srcState)) { \
        MEDIA_INFO_LOG("%s, current play state already be %d", (outStr) ? (outStr) : " ", (dstState)); \
        return MEDIA_OK; \
    } \
} while (0)

#define CHECK_FAILED_RETURN_WITH_UNLOCK(value, target, ret, printfString, mutex) \
do { \
    if ((value) != (target)) { \
        MEDIA_ERR_LOG("%s, ret:%d", (printfString) ? (printfString) : " ", ret); \
        MediaMutexUnLock((mutex)); \
        return ret; \
    } \
} while (0)

#define CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(value, ret, printfString, mutex) \
do { \
    if ((value)) { \
        MEDIA_ERR_LOG("%s, ret:%d", (printfString) ? (printfString) : " ", ret); \
        MediaMutexUnLock((mutex)); \
        return ret; \
    } \
} while (0)

#define CHK_NULL_RETURN(ptr, outStr) \
do { \
    if ((ptr) == nullptr) { \
        MEDIA_ERR_LOG("%s ", ((outStr) != nullptr) ? (outStr) : " "); \
        return (-1); \
    } \
} while (0)

#define CHK_NULL_RETURN_NONE(ptr, outStr) \
do { \
    if ((ptr) == nullptr) { \
        MEDIA_ERR_LOG("%s ", ((outStr) != nullptr) ? (outStr) : " "); \
        return; \
    } \
} while (0)

#define CHK_NULL_RETURN_WITH_UNLOCK(ptr, mutex, printfString) \
do { \
    if ((ptr) == nullptr) { \
        MEDIA_ERR_LOG("%s ", ((printfString) != nullptr) ? (printfString) : " "); \
        MediaMutexUnLock((mutex)); \
        return (-1); \
    } \
} while (0)
#else
#define CHECK_NULL_RETURN(ptr, ret, outStr) \
do { \
    if ((ptr) == nullptr) { \
        uapi_diag_error_log(0, outStr); \
        return (ret); \
    } \
} while (0)

#define CHECK_NULL_RETURN_NONE(ptr, outStr) \
do { \
    if ((ptr) == nullptr) { \
        uapi_diag_error_log(0, outStr); \
        return; \
    } \
} while (0)

#define CHECK_FAILED_RETURN(value, target, ret, outStr) \
do { \
    if ((value) != (target)) { \
        uapi_diag_error_log(0, outStr); \
        return (ret); \
    } \
} while (0)

#define CHECK_FAILED_RETURN_NONE(value, target, outStr) \
do { \
    if ((value) != (target)) { \
        uapi_diag_error_log(0, outStr); \
        return; \
    } \
} while (0)

#define CHECK_TRUE_CONDITION_RETURN(value, ret, outStr) \
do { \
    if ((value)) { \
        uapi_diag_error_log(0, outStr); \
        return (ret); \
    } \
} while (0)

#define CHECK_FALSE_CONDITION_RETURN(value, ret, outStr) \
do { \
    if (!(value)) { \
        uapi_diag_error_log(0, outStr); \
        return (ret); \
    } \
} while (0)

#define CHECK_FALSE_CONDITION_RETURN_NONE(value, outStr) \
do { \
    if (!(value)) { \
        uapi_diag_error_log(0, outStr); \
        return; \
    } \
} while (0)

#define CHECK_TRUE_CONDITION_RETURN_NONE(value, outStr) \
do { \
    if ((value)) { \
        uapi_diag_error_log(0, outStr); \
        return; \
    } \
} while (0)

#define CHECK_FAILED_PRINT(value, target, outStr) \
do { \
    if ((value) != (target)) { \
        uapi_diag_error_log(0, outStr); \
    } \
} while (0)

#define CHECK_STATE_SAME_RETURN(srcState, dstState, outStr) \
do { \
    if ((dstState) == (srcState)) { \
        uapi_diag_info_log(0, outStr); \
        return MEDIA_OK; \
    } \
} while (0)

#define CHECK_FAILED_RETURN_WITH_UNLOCK(value, target, ret, printfString, mutex) \
do { \
    if ((value) != (target)) { \
        uapi_diag_error_log(0, printfString); \
        MediaMutexUnLock((mutex)); \
        return ret; \
    } \
} while (0)

#define CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(value, ret, printfString, mutex) \
do { \
    if ((value)) { \
        uapi_diag_error_log(0, printfString); \
        MediaMutexUnLock((mutex)); \
        return ret; \
    } \
} while (0)

#define CHK_NULL_RETURN(ptr, outStr) \
do { \
    if ((ptr) == nullptr) { \
        uapi_diag_error_log(0, outStr); \
        return (-1); \
    } \
} while (0)

#define CHK_NULL_RETURN_NONE(ptr, outStr) \
do { \
    if ((ptr) == nullptr) { \
        uapi_diag_error_log(0, outStr); \
        return; \
    } \
} while (0)

#define CHK_NULL_RETURN_WITH_UNLOCK(ptr, mutex, printfString) \
do { \
    if ((ptr) == nullptr) { \
        uapi_diag_error_log(0, printfString); \
        MediaMutexUnLock((mutex)); \
        return (-1); \
    } \
} while (0)
#endif

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif /* __cplusplus */

#endif /* PLAYER_CHECK_H */
