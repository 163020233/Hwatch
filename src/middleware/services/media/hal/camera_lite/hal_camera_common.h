/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: camera common define
 */

#ifndef HAL_CAMERA_COMMON_H
#define HAL_CAMERA_COMMON_H

#include <stdint.h>
#include "debug_print.h"
#include "dfx/dfx_traceui.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define CAM_ERR(fmt, ...) PRINT(fmt, ##__VA_ARGS__)
#define CAM_WARN(fmt, ...) PRINT(fmt, ##__VA_ARGS__)
#define CAM_INFO(fmt, ...) PRINT(fmt, ##__VA_ARGS__)
#define CAM_DBG(fmt, ...)

#define CAM_FW_ENTER() CAM_DBG("%s() enter\n", __FUNCTION__)
#define CAM_FW_EXIT() CAM_DBG("%s() exit\n", __FUNCTION__)
#define CAM_PASS do {} while (0)

#define CAM_TRACE_BEGIN(nameStaticStr) TraceuiEventTraceBegin(nameStaticStr)
#define CAM_TRACE_END() TraceuiEventTraceEnd()
#define CAM_TRACE_LOG(fmt, ...) TraceuiEventLog(fmt, ##__VA_ARGS__)

#ifndef UNUSED
#define UNUSED(a) ((void)(1 || (a)))
#endif

#define IF_FUNC_ERR(call, err_action, log_fmt, ...) do { \
    int32_t _ret = (int32_t)(call); \
    if (_ret != 0) { \
        CAM_ERR("CAM ERROR: " log_fmt ", ret: %d \n", ##__VA_ARGS__, _ret); \
        err_action; \
    } \
} while (0)

#define LOG_FUNC_ERR(call, log_fmt, ...) IF_FUNC_ERR(call, CAM_PASS, log_fmt, ##__VA_ARGS__)

#define ERR_IF(cond, cond_action, log_fmt, ...) do { \
    if (cond) { \
        CAM_ERR("CAM ERROR " log_fmt "\n", ##__VA_ARGS__); \
        cond_action; \
    } \
} while (0)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // HAL_CAMERA_COMMON_H
