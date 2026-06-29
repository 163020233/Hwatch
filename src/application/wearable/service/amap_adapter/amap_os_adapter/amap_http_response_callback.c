/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Create Date : 2025-11
 */

#include "amap_http_response_callback.h"
#include <stdlib.h>
#include "amap_log_adapter.h"

#define SAFE_FREE(ptr)  \
    do {                \
        if (ptr) {      \
            free(ptr);  \
            ptr = NULL; \
        }               \
    } while (0)

static void free_http_response_header(awk_http_response_t *response)
{
    AMAP_LOGD("free_http_response_header");
    if (response == NULL) {
        AMAP_LOGE("free_http_response_header response is null");
        return;
    }
    if (response->headers.data != NULL) {
        for (size_t i = 0; i < response->headers.size; ++i) {
            SAFE_FREE(response->headers.data[i].key);
            SAFE_FREE(response->headers.data[i].value);
        }
        SAFE_FREE(response->headers.data);
    }
}

static void free_http_response_body(awk_http_response_t *response)
{
    AMAP_LOGD("free_http_response_body");
    if (response == NULL) {
        AMAP_LOGE("free_http_response_body response is null");
        return;
    }
    if (response->body != NULL) {
        SAFE_FREE(response->body->buffer);
        SAFE_FREE(response->body);
    }
}

void amap_on_receive_header(awk_http_info *info)
{
    AMAP_LOGD("amap_on_receive_header");
    if (info && info->callback && info->callback->on_receive_header) {
        info->callback->on_receive_header(info->callback, info->response);
    } else {
        AMAP_LOGE("amap_on_receive_header: Invalid callback pointer or info is NULL");
    }

    free_http_response_header(info->response);
    SAFE_FREE(info->response);
    SAFE_FREE(info);
}

void amap_on_receive_body(awk_http_info *info)
{
    AMAP_LOGD("amap_on_receive_body");
    if (info && info->callback && info->callback->on_receive_body) {
        info->callback->on_receive_body(info->callback, info->response);
    } else {
        AMAP_LOGE("amap_on_receive_body: Invalid callback pointer or info is NULL");
    }

    free_http_response_body(info->response);
    SAFE_FREE(info->response);
    SAFE_FREE(info);
}

void amap_on_fail(awk_http_info *info)
{
    AMAP_LOGD("amap_on_fail");
    if (info && info->callback && info->callback->on_fail) {
        info->callback->on_fail(info->callback, info->response, info->error);
    } else {
        AMAP_LOGE("amap_on_fail: Invalid callback pointer");
    }

    if (info) {
        free_http_response_header(info->response);
        free_http_response_body(info->response);
        SAFE_FREE(info->response);
        SAFE_FREE(info);
    }
}

void amap_on_success(awk_http_info *info)
{
    AMAP_LOGD("amap_on_success");
    if (!info) {
        AMAP_LOGE("amap_on_success: http_info is NULL, cannot process event.");
        return;
    }
    if (info->callback && info->callback->on_success) {
        info->callback->on_success(info->callback, info->response);
    } else {
        AMAP_LOGE("amap_on_success: Callback function 'on_success' or structure is NULL.");
    }

    SAFE_FREE(info);
}

void amap_on_canceled(awk_http_info *info)
{
    AMAP_LOGD("amap_on_canceled");
    if (!info) {
        AMAP_LOGE("amap_on_canceled: http_info is NULL, cannot process event.");
        return;
    }
    if (info->callback && info->callback->on_canceled) {
        info->callback->on_canceled(info->callback, info->response);
    } else {
        AMAP_LOGE("amap_on_canceled: Callback function 'on_canceled' or structure is NULL.");
    }

    free_http_response_header(info->response);
    free_http_response_body(info->response);
    SAFE_FREE(info->response);
    SAFE_FREE(info);
}
