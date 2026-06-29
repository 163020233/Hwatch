/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Create Date : 2025-11
 */

#ifndef AMAP_HTTP_RESPONSE_CALLBACK
#define AMAP_HTTP_RESPONSE_CALLBACK

#include "awk_adapter.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    awk_http_response_callback_t *callback;
    awk_http_response_t *response;
    int64_t request_id;
    int32_t error;
} awk_http_info;

void amap_on_receive_header(awk_http_info *http_info);
void amap_on_receive_body(awk_http_info *http_info);
void amap_on_fail(awk_http_info *http_info);
void amap_on_success(awk_http_info *http_info);
void amap_on_canceled(awk_http_info *http_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // AMAP_HTTP_RESPONSE_CALLBACK