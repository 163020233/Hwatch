/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: display common
 * Author: Hisi Graphic Team
 * Created: 2025-10
 */

#ifndef DISPLAY_COMMON_H
#define DISPLAY_COMMON_H
#ifdef USE_DIAG_LOG
#include "soc_diag_util.h"
#include "log_oam_logger.h"
#include "log_def.h"
#endif

#define DISP_BYTE_ALIGNMENT 16
#define DISP_STRIDE_DIVIDER 8
#ifdef USE_DIAG_LOG
#define DISP_LOGE(fmt, arg...) uapi_diag_error_log(0, fmt, ##arg)
#define DISP_LOGW(fmt, arg...) uapi_diag_warning_log(0, fmt, ##arg)
#define DISP_LOGI(fmt, arg...) uapi_diag_info_log(0, fmt, ##arg)
#define DISP_LOGD(fmt, arg...) uapi_diag_debug_log(0, fmt, ##arg)
#else
#define DISP_LOGE(fmt, arg...) printf("[DISPLAY E/] %s: " fmt, __FUNCTION__, ##arg)
#define DISP_LOGW(fmt, arg...) printf("[DISPLAY W/] %s: " fmt, __FUNCTION__, ##arg)
#define DISP_LOGI(fmt, arg...) printf("[DISPLAY I/] %s: " fmt, __FUNCTION__, ##arg)
#define DISP_LOGD(fmt, arg...) printf("[DISPLAY D/] %s: " fmt, __FUNCTION__, ##arg)
#endif
#define CHECK_NULLPOINTER_RETURN_VALUE(pointer, ret) do { \
    if ((pointer) == NULL) { \
        DISP_LOGE("pointer is null and return ret\n"); \
        return (ret); \
    } \
} while (0)

#define CHECK_NULLPOINTER_RETURN(pointer) do { \
    if ((pointer) == NULL) { \
        DISP_LOGE("pointer is null and return\n"); \
        return; \
    } \
} while (0)

#endif /* DISPLAY_COMMON_H */
