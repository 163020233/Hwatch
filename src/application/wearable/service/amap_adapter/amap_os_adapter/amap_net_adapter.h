/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Create Date : 2025-11
 */

#ifndef AMAP_NET_ADAPTER
#define AMAP_NET_ADAPTER

#include <stdint.h>
#include "awk_adapter.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief 发送请求，请求的处理可以在异步线程中发起
 * @param request aos请求类, request的释放由外部释放
 * @param callback 返回结果回调接口
 * @return 请求id 请求id不能重复，可以自增处理
 */
uint64_t amap_send(awk_http_request_t *request, awk_http_response_callback_t *callback);

/**
 * @brief 取消请求，需要在主流程线程中处理
 * @param request_id 需要取消请求的id
 */
void amap_cancel(uint64_t request_id);

uint64_t get_curr_request_id(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // AMAP_NET_ADAPTER