/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Create Date : 2025-11
 */

#include "amap_net_adapter.h"
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include "httpclient.h"
#include "amap_http_response_callback.h"
#include "graphic_service_wrapper.h"
#include "amap_log_adapter.h"
#include "los_memory.h"
#include "amap_thread_adapter.h"

#define DEFAULT_BUF_SIZE 8192
#define HEADER_SEPARATOR_LEN 2
#define HEADER_LINE_END_LEN 2

#define SAFE_FREE(ptr)  \
    do {                \
        if (ptr) {      \
            free(ptr);  \
            ptr = NULL; \
        }               \
    } while (0)

typedef struct _http_request_item_t {
    uint64_t request_id;
    awk_http_request_t *request;
    awk_http_response_callback_t *callback;
    struct _http_request_item_t *next;
} http_request_item;

typedef struct {
    http_request_item *item;
    HttpClient *client;
    HttpClientData *client_data;
    awk_http_info *info;
    int32_t errorCode;
} HttpWorkerContext;

typedef enum {
    AWK_ERR_SUCCESS = 0,  // 0: 成功，无错误

    // --- 内存分配错误
    AWK_ERR_MALLOC_BASE = 600,
    AWK_ERR_MALLOC_CLIENT = 601,        // 内存分配失败：HttpClient 结构体
    AWK_ERR_MALLOC_CLIENT_DATA = 602,   // 内存分配失败：HttpClientData 结构体
    AWK_ERR_MALLOC_HEADER_BUF = 603,    // 内存分配失败：Header 缓冲区
    AWK_ERR_MALLOC_RESPONSE_BUF = 604,  // 内存分配失败：Response 缓冲区
    AWK_ERR_MALLOC_INFO_STRUCT = 605,   // 内存分配失败：awk_http_info 结构体

    // --- 连接和传输错误
    AWK_ERR_CONN_BASE = 700,
    AWK_ERR_CONN_FAILED = 701,  // AWKHttpClientConn 连接阶段失败（如DNS、超时、Socket创建失败）
    AWK_ERR_SEND_FAILED = 702,  // AWKHttpClientSend 发送请求头/Body失败（如连接中断、写入超时）
    AWK_ERR_RECV_FAILED = 703,  // AWKHttpClientRecvResponse 接收 Body 失败（如连接中断、读取超时）

    // --- 内部逻辑和状态管理错误
    AWK_ERR_INTERNAL_BASE = 800,
    AWK_ERR_SET_HEADER_FAILED = 801,   // set_request_header 失败
    AWK_ERR_INSERT_INFO_FAILED = 802,  // insert_request_info 内部状态管理失败
    AWK_ERR_UNSUPPORTED_METHOD = 803,  // 不支持的HTTP方法
    AWK_CANCEL_REQUEST = 900,
} AwkHttpClientError;

static volatile uint64_t g_current_request_id = 0;
static volatile bool g_cancel_current = false;

static inline bool StepExec(HttpWorkerContext *ctx, int32_t ret, const char *step_name)
{
    ctx->errorCode = ret;
    if (ctx->errorCode != AWK_ERR_SUCCESS) {
        AMAP_LOGE("Step failed: %s, err=%d", step_name, ctx->errorCode);
        return false;
    }
    return true;
}

static bool calc_headers_total_len(const awk_http_request_t *request, size_t *total_len)
{
    *total_len = 0;
    for (size_t i = 0; i < request->headers.size; i++) {
        const char *key = request->headers.data[i].key;
        const char *val = request->headers.data[i].value;
        if (!key || !val) {
            AMAP_LOGE("header key or value is null at index %zu", i);
            return false;
        }
        *total_len += strlen(key) + HEADER_SEPARATOR_LEN + strlen(val) + HEADER_LINE_END_LEN;
    }
    return true;
}

static void append_header_field(char *buf, size_t buf_size, size_t *offset, const char *src, size_t len)
{
    size_t remain = buf_size - *offset;
    if (memcpy_s(buf + *offset, remain, src, len) != 0) {
        return;
    }
    *offset += len;
}

static void fill_header_buffer(const awk_http_request_t *request, char *buf, size_t buf_size)
{
    size_t offset = 0;
    for (size_t i = 0; i < request->headers.size; i++) {
        const char *key = request->headers.data[i].key;
        const char *val = request->headers.data[i].value;
        append_header_field(buf, buf_size, &offset, key, strlen(key));
        append_header_field(buf, buf_size, &offset, ": ", HEADER_SEPARATOR_LEN);
        append_header_field(buf, buf_size, &offset, val, strlen(val));
        append_header_field(buf, buf_size, &offset, "\r\n", HEADER_LINE_END_LEN);
    }
    buf[offset] = '\0';
}

static bool set_request_header(awk_http_request_t *request, HttpClient *data)
{
    AMAP_LOGD("set_request_header");
    if (!request || !data) {
        AMAP_LOGE("request or data is null");
        return false;
    }
    size_t total_len = 0;
    if (!calc_headers_total_len(request, &total_len)) {
        return false;
    }

    if (total_len == 0) {
        return true;
    }
    if (data->header) {
        free(data->header);
        data->header = NULL;
    }
    data->header = (char *)malloc(total_len + 1);
    if (!data->header) {
        AMAP_LOGE("malloc failed for header");
        return false;
    }
    fill_header_buffer(request, data->header, total_len + 1);
    return true;
}

static char *trim_leading_whitespace(char *str)
{
    while (*str == ' ' || *str == '\t') {
        str++;
    }
    return str;
}
static void trim_trailing_chars(char *start, const char *chars)
{
    size_t len = strlen(start);
    if (len == 0) {
        return;
    }

    char *end = start + len - 1;
    while (end >= start && strchr(chars, *end)) {
        *end-- = '\0';
    }
}

static size_t count_header_lines(const char *header_str)
{
    char *copy = strdup(header_str);
    if (copy == NULL) {
        AMAP_LOGE("strdup failed for header_str");
        return 0;
    }
    size_t count = 0;
    char *ctx = NULL;
    char *line = strtok_r(copy, "\r\n", &ctx);
    while (line != NULL) {
        char *trimmed = trim_leading_whitespace(line);
        if (*trimmed != '\0' && strchr(trimmed, ':') != NULL) {
            count++;
        }
        line = strtok_r(NULL, "\r\n", &ctx);
    }
    free(copy);
    return count;
}

static bool parse_single_header(char *line, awk_pair_http_header_t *pair)
{
    char *current = trim_leading_whitespace(line);
    char *colon = strchr(current, ':');
    if (colon == NULL) {
        return false;
    }
    *colon = '\0';
    char *key = current;
    char *value = colon + 1;

    trim_trailing_chars(key, " \t");
    value = trim_leading_whitespace(value);
    trim_trailing_chars(value, " \t\r");

    pair->key = strdup(key);
    if (pair->key == NULL) {
        AMAP_LOGE("strdup failed for header key");
        return false;
    }

    pair->value = strdup(value);
    if (pair->value == NULL) {
        AMAP_LOGE("strdup failed for header value");
        free(pair->key);
        pair->key = NULL;
        return false;
    }
    return true;
}

awk_http_header_t awk_http_parse_headers(const char *header_str)
{
    awk_http_header_t map = {.data = NULL, .size = 0};
    if (header_str == NULL || *header_str == '\0') {
        return map;
    }

    size_t line_count = count_header_lines(header_str);
    if (line_count == 0) {
        return map;
    }

    map.data = calloc(line_count, sizeof(awk_pair_http_header_t));
    if (map.data == NULL) {
        AMAP_LOGE("calloc failed for header map (%zu entries)", line_count);
        return map;
    }

    char *copy = strdup(header_str);
    if (copy == NULL) {
        AMAP_LOGE("strdup failed for header_str");
        free(map.data);
        map.data = NULL;
        return map;
    }

    char *ctx = NULL;
    char *line = strtok_r(copy, "\r\n", &ctx);
    size_t idx = 0;
    while (line != NULL && idx < line_count) {
        if (parse_single_header(line, &map.data[idx])) {
            AMAP_LOGD("Header[%zu]: %s: %s", idx, map.data[idx].key, map.data[idx].value);
            idx++;
        }
        line = strtok_r(NULL, "\r\n", &ctx);
    }

    free(copy);
    map.size = idx;
    return map;
}

static void DestroyHttpClient(HttpClient *client, HttpClientData *client_data)
{
    if (client_data) {
        SAFE_FREE(client_data->headerBuf);
        SAFE_FREE(client_data->responseBuf);
        SAFE_FREE(client_data->leftBuffer);
        SAFE_FREE(client_data->redirectUrl);
        free(client_data);
    }
    if (client) {
        SAFE_FREE(client->header);
        free(client);
    }
}

static int32_t CreateHttpClient(HttpClient **client, HttpClientData **client_data)
{
    *client = NULL;
    *client_data = NULL;

    HttpClient *c = (HttpClient *)malloc(sizeof(HttpClient));
    if (!c) {
        AMAP_LOGE("malloc failed for client");
        return AWK_ERR_MALLOC_CLIENT;
    }
    memset_s(c, sizeof(HttpClient), 0, sizeof(HttpClient));

    HttpClientData *cd = (HttpClientData *)malloc(sizeof(HttpClientData));
    if (!cd) {
        AMAP_LOGE("malloc failed for client_data");
        free(c);
        return AWK_ERR_MALLOC_CLIENT_DATA;
    }
    memset_s(cd, sizeof(HttpClientData), 0, sizeof(HttpClientData));

    cd->headerBuf = (char *)malloc(DEFAULT_BUF_SIZE);
    if (!cd->headerBuf) {
        AMAP_LOGE("malloc failed for headerBuf");
        free(cd);
        free(c);
        return AWK_ERR_MALLOC_HEADER_BUF;
    }
    cd->headerBufLen = DEFAULT_BUF_SIZE;

    cd->responseBuf = (char *)malloc(DEFAULT_BUF_SIZE);
    if (!cd->responseBuf) {
        AMAP_LOGE("malloc failed for responseBuf");
        free(cd->headerBuf);
        free(cd);
        free(c);
        return AWK_ERR_MALLOC_RESPONSE_BUF;
    }
    cd->responseBufLen = DEFAULT_BUF_SIZE;

    *client = c;
    *client_data = cd;
    return AWK_ERR_SUCCESS;
}

static int32_t PrepareRequestData(const http_request_item *item, HttpClient *client, HttpClientData *client_data)
{
    if (item->request->body) {
        client_data->postBuf = (char *)item->request->body->buffer;
        client_data->postBufLen = (int)item->request->body->length;
        AMAP_LOGD("Body present, len = %d", client_data->postBufLen);
    } else {
        AMAP_LOGD("No body data");
        client_data->postBuf = NULL;
        client_data->postBufLen = 0;
    }

    if (item->request->method == AWK_HTTP_METHOD_POST) {
        client_data->method = HTTP_POST;
    } else if (item->request->method == AWK_HTTP_METHOD_GET) {
        client_data->method = HTTP_GET;
    } else {
        AMAP_LOGE("Unsupported HTTP method: %d", item->request->method);
        return AWK_ERR_UNSUPPORTED_METHOD;
    }

    if (item->request->headers.data) {
        if (!set_request_header(item->request, client)) {
            AMAP_LOGE("set_request_header failed");
            return AWK_ERR_SET_HEADER_FAILED;
        }
    }
    return AWK_ERR_SUCCESS;
}

static bool IsRequestCancelled(void)
{
    if (g_cancel_current) {
        AMAP_LOGD("Request canceled");
        return true;
    }
    return false;
}

static int32_t Connect(const http_request_item *item, HttpClient *client)
{
    AMAP_LOGD("http request: %s", item->request->url);

    if (IsRequestCancelled()) {
        return AWK_CANCEL_REQUEST;
    }

    int ret = HttpClientConn(client, item->request->url);
    if (ret != HTTP_SUCCESS) {
        AMAP_LOGE("HttpClientConn failed, ret = %d", ret);
        return AWK_ERR_CONN_FAILED;
    }

    return AWK_ERR_SUCCESS;
}

static int32_t SendRequest(const http_request_item *item, HttpClient *client, HttpClientData *client_data)
{
    if (IsRequestCancelled()) {
        return AWK_CANCEL_REQUEST;
    }

    int ret = HttpClientSend(client, item->request->url, client_data->method, client_data);
    if (ret != HTTP_SUCCESS) {
        AMAP_LOGE("HttpClientSend failed, ret = %d", ret);
        return AWK_ERR_SEND_FAILED;
    }

    return AWK_ERR_SUCCESS;
}

awk_http_info *amap_create_header_info(const awk_http_info *parent_info, uint32_t status_code,
                                       awk_http_header_t headers)
{
    awk_http_info *header_info = (awk_http_info *)malloc(sizeof(awk_http_info));
    if (!header_info) {
        AMAP_LOGE("malloc header_info failed");
        return NULL;
    }

    header_info->callback = parent_info->callback;
    header_info->request_id = parent_info->request_id;
    header_info->error = 0;

    header_info->response = (awk_http_response_t *)malloc(sizeof(awk_http_response_t));
    if (!header_info->response) {
        free(header_info);
        AMAP_LOGE("malloc header response failed");
        return NULL;
    }

    memset_s(header_info->response, sizeof(awk_http_response_t), 0, sizeof(awk_http_response_t));
    header_info->response->request_id = parent_info->request_id;
    header_info->response->status_code = status_code;
    header_info->response->headers = headers;
    header_info->response->body = NULL;

    return header_info;
}

int amap_create_body_chunk_info(HttpClientData *clientData, const awk_http_info *parent_info,
                                awk_http_info **out_body_info)
{
    awk_http_info *body_info = (awk_http_info *)malloc(sizeof(awk_http_info));
    if (!body_info) {
        AMAP_LOGE("malloc body_info failed");
        return -1;
    }

    body_info->callback = parent_info->callback;
    body_info->request_id = parent_info->request_id;
    body_info->error = 0;
    body_info->response = NULL;

    /* response */
    awk_http_response_t *resp = (awk_http_response_t *)malloc(sizeof(awk_http_response_t));
    if (!resp) {
        free(body_info);
        AMAP_LOGE("malloc body response failed");
        return -1;
    }
    memset_s(resp, sizeof(awk_http_response_t), 0, sizeof(awk_http_response_t));
    resp->request_id = (uint64_t)parent_info->request_id;
    body_info->response = resp;

    /* body */
    resp->body = (awk_http_buffer_t *)malloc(sizeof(awk_http_buffer_t));
    if (!resp->body) {
        free(resp);
        free(body_info);
        AMAP_LOGE("malloc body buffer struct failed");
        return -1;
    }

    int data_len = clientData->contentBlockLen;
    resp->body->buffer = (uint8_t *)malloc(data_len);
    if (!resp->body->buffer) {
        free(resp->body);
        free(resp);
        free(body_info);
        AMAP_LOGE("malloc body buffer failed");
        return -1;
    }

    memcpy_s(resp->body->buffer, data_len, clientData->responseBuf, data_len);
    resp->body->length = (size_t)data_len;

    AMAP_LOGD("Sending chunk: %d bytes", data_len);
    *out_body_info = body_info;
    return 0;
}

int amap_invoke_header_callback(const awk_http_info *info, uint32_t status_code)
{
    awk_http_info *header_info = amap_create_header_info(info, status_code, info->response->headers);
    if (!header_info) {
        return -1;
    }

    amap_on_receive_header(header_info);
    return 0;
}

int amap_invoke_body_callback(const awk_http_info *info, HttpClientData *clientData)
{
    AMAP_LOGD("amap_invoke_body_callback");
    awk_http_info *body_info = NULL;
    int ret = amap_create_body_chunk_info(clientData, info, &body_info);
    if (ret != 0) {
        return ret;
    }

    amap_on_receive_body(body_info);
    return 0;
}

static int32_t ReceiveResponse(HttpClient *client, HttpClientData *client_data, awk_http_info *info)
{
    int ret = 0;
    bool header_received = false;
    do {
        if (IsRequestCancelled()) {
            return AWK_CANCEL_REQUEST;
        }
        AMAP_LOGD("ReceiveResponse isMore=%d, retrieveLen=%d, contentBlockLen=%d",
            client_data->isMore, client_data->retrieveLen, client_data->contentBlockLen);

        ret = HttpClientRecvResponse(client, client_data);
        AMAP_LOGD("ReceiveResponse after recv: ret=%d, isMore=%d", ret, client_data->isMore);
        if (ret < 0 && ret != HTTP_EAGAIN) {
            AMAP_LOGE("HttpClientRecvResponse failed, ret = %d", ret);
            return AWK_ERR_RECV_FAILED;
        }
        // 第一次接收到响应时，解析并投递 header 事件
        if (!header_received && client->responseCode > 0) {
            header_received = true;

            // 设置状态码
            info->response->status_code = (uint32_t)client->responseCode;

            // 解析 headers
            if (client_data->headerBuf && client_data->headerBuf[0] != '\0') {
                info->response->headers = awk_http_parse_headers(client_data->headerBuf);
            }

            // 投递 header 事件
            int post_ret = amap_invoke_header_callback(info, info->response->status_code);
            if (post_ret != 0) {
                AMAP_LOGE("amap_invoke_header_callback failed");
            }
        }

        // 如果有 body 数据，投递 body 事件
        if (client_data->contentBlockLen > 0) {
            int post_ret = amap_invoke_body_callback(info, client_data);
            if (post_ret != 0) {
                AMAP_LOGE("amap_invoke_body_callback failed");
            }
            // 重置 contentBlockLen 以便下次循环
            client_data->contentBlockLen = 0;
        }
    } while (client_data->isMore && ret >= 0);

    if (client_data->responseContentLen == 0) {
        AMAP_LOGD("recv length = 0");
    }
    HttpClientClose(client);
    return AWK_ERR_SUCCESS;
}

static void amap_invoke_success_callback(const awk_http_info *info)
{
    AMAP_LOGD("amap_invoke_success_callback");
    awk_http_info *success_info = (awk_http_info *)malloc(sizeof(awk_http_info));
    if (!success_info) {
        AMAP_LOGE("malloc failed for success_info");
        return;
    }

    memset_s(success_info, sizeof(awk_http_info), 0, sizeof(awk_http_info));
    success_info->callback = info->callback;
    success_info->request_id = info->request_id;
    success_info->response = info->response;
    amap_on_success(success_info);
}

static void amap_invoke_fail_callback(const http_request_item *item, int32_t errorCode)
{
    AMAP_LOGD("amap_invoke_fail_callback, error=%d", errorCode);
    awk_http_info *fail_info = (awk_http_info *)malloc(sizeof(awk_http_info));
    if (!fail_info) {
        AMAP_LOGE("malloc failed for fail_info");
        return;
    }

    memset_s(fail_info, sizeof(awk_http_info), 0, sizeof(awk_http_info));
    fail_info->request_id = item->request_id;
    fail_info->error = errorCode;
    fail_info->callback = item->callback;
    fail_info->response = NULL;
    amap_on_fail(fail_info);
}

static void amap_invoke_cancel_callback(const http_request_item *item)
{
    AMAP_LOGD("amap_invoke_cancel_callback");
    awk_http_info *info = (awk_http_info *)malloc(sizeof(awk_http_info));
    if (!info) {
        AMAP_LOGE("malloc failed for cancel_info");
        return;
    }

    memset_s(info, sizeof(awk_http_info), 0, sizeof(awk_http_info));
    info->request_id = item->request_id;
    info->error = AWK_CANCEL_REQUEST;
    info->callback = item->callback;
    info->response = NULL;
    amap_on_canceled(info);
}

static void InitHttpWorkerContext(HttpWorkerContext *ctx, http_request_item *item)
{
    memset_s(ctx, sizeof(HttpWorkerContext), 0, sizeof(HttpWorkerContext));
    ctx->item = item;
    ctx->errorCode = AWK_ERR_SUCCESS;
}

static void free_request(awk_http_request_t *request)
{
    if (!request) {
        return;
    }
    AMAP_LOGD("free_request");
    SAFE_FREE(request->url);

    if (request->headers.data) {
        for (size_t i = 0; i < request->headers.size; i++) {
            SAFE_FREE(request->headers.data[i].key);
            SAFE_FREE(request->headers.data[i].value);
        }
        SAFE_FREE(request->headers.data);
        request->headers.size = 0;
    }

    if (request->body) {
        SAFE_FREE(request->body->buffer);
        SAFE_FREE(request->body);
    }

    SAFE_FREE(request);
}

static void CleanupHttpWorkerContext(HttpWorkerContext *ctx)
{
    AMAP_LOGD("CleanupHttpWorkerContext");
    if (ctx->info) {
        SAFE_FREE(ctx->info->response);
        free(ctx->info);
        ctx->info = NULL;
    }

    DestroyHttpClient(ctx->client, ctx->client_data);
    ctx->client = NULL;
    ctx->client_data = NULL;
}

static int32_t StepCreateClient(HttpWorkerContext *ctx)
{
    return CreateHttpClient(&ctx->client, &ctx->client_data);
}

static int32_t StepPrepareRequest(HttpWorkerContext *ctx)
{
    return PrepareRequestData(ctx->item, ctx->client, ctx->client_data);
}

static int32_t StepConnect(HttpWorkerContext *ctx)
{
    return Connect(ctx->item, ctx->client);
}

static int32_t StepSendRequest(HttpWorkerContext *ctx)
{
    return SendRequest(ctx->item, ctx->client, ctx->client_data);
}

static int32_t StepCreateInfo(HttpWorkerContext *ctx)
{
    ctx->info = (awk_http_info *)malloc(sizeof(awk_http_info));
    if (!ctx->info) {
        AMAP_LOGE("malloc failed for awk_http_info");
        return AWK_ERR_MALLOC_INFO_STRUCT;
    }

    memset_s(ctx->info, sizeof(awk_http_info), 0, sizeof(awk_http_info));
    ctx->info->callback = ctx->item->callback;
    ctx->info->request_id = (int64_t)ctx->item->request_id;
    // 预创建 response 对象
    ctx->info->response = (awk_http_response_t *)malloc(sizeof(awk_http_response_t));
    if (!ctx->info->response) {
        AMAP_LOGE("malloc failed for response");
        free(ctx->info);
        ctx->info = NULL;
        return AWK_ERR_MALLOC_INFO_STRUCT;
    }
    memset_s(ctx->info->response, sizeof(awk_http_response_t), 0, sizeof(awk_http_response_t));
    ctx->info->response->request_id = ctx->item->request_id;
    return AWK_ERR_SUCCESS;
}

static int32_t StepReceiveResponse(HttpWorkerContext *ctx)
{
    return ReceiveResponse(ctx->client, ctx->client_data, ctx->info);
}

static void HandleResult(HttpWorkerContext *ctx)
{
    AMAP_LOGD("HandleResult, errorCode=%d", ctx->errorCode);

    if (ctx->errorCode == AWK_ERR_SUCCESS) {
        amap_invoke_success_callback(ctx->info);
    } else if (ctx->errorCode == AWK_CANCEL_REQUEST) {
        amap_invoke_cancel_callback(ctx->item);
    } else {
        amap_invoke_fail_callback(ctx->item, ctx->errorCode);
    }
}

static void free_request_item(http_request_item *item)
{
    if (!item) {
        return;
    }
    free_request(item->request);
    free(item);
}

static void process_request(http_request_item *item)
{
    AMAP_LOGD("process_request start, request_id = %llu", (unsigned long long)item->request_id);
    g_current_request_id = item->request_id;
    g_cancel_current = false;
    HttpWorkerContext *ctx = (HttpWorkerContext *)malloc(sizeof(HttpWorkerContext));
    if (!ctx) {
        AMAP_LOGE("malloc failed for HttpWorkerContext");
        amap_invoke_fail_callback(item, AWK_ERR_MALLOC_CLIENT);
        free_request_item(item);
        return;
    }

    InitHttpWorkerContext(ctx, item);

    if (!StepExec(ctx, StepCreateClient(ctx), "StepCreateClient")) {
        goto cleanup;
    }
    if (!StepExec(ctx, StepPrepareRequest(ctx), "StepPrepareRequest")) {
        goto cleanup;
    }
    if (!StepExec(ctx, StepConnect(ctx), "StepConnect")) {
        goto cleanup;
    }
    if (!StepExec(ctx, StepSendRequest(ctx), "StepSendRequest")) {
        goto cleanup;
    }
    if (!StepExec(ctx, StepCreateInfo(ctx), "StepCreateInfo")) {
        goto cleanup;
    }
    if (!StepExec(ctx, StepReceiveResponse(ctx), "StepReceiveResponse")) {
        goto cleanup;
    }

cleanup:
    HandleResult(ctx);
    CleanupHttpWorkerContext(ctx);
    free(ctx);
    free_request_item(item);
    AMAP_LOGD("process_request end");
}

static int copy_request_url(const awk_http_request_t *src, awk_http_request_t *dst)
{
    if (!src->url) {
        return 0;
    }
    dst->url = strdup(src->url);
    return dst->url ? 0 : -1;
}

static int copy_request_headers(const awk_http_request_t *src, awk_http_request_t *dst)
{
    if (src->headers.size == 0 || !src->headers.data) {
        return 0;
    }
    dst->headers.size = src->headers.size;
    dst->headers.data = calloc(dst->headers.size, sizeof(*dst->headers.data));
    if (!dst->headers.data) {
        return -1;
    }
    for (size_t i = 0; i < dst->headers.size; i++) {
        if (src->headers.data[i].key) {
            dst->headers.data[i].key = strdup(src->headers.data[i].key);
            if (!dst->headers.data[i].key) {
                return -1;
            }
        }
        if (src->headers.data[i].value) {
            dst->headers.data[i].value = strdup(src->headers.data[i].value);
            if (!dst->headers.data[i].value) {
                return -1;
            }
        }
    }
    return 0;
}

static int copy_request_body(const awk_http_request_t *src, awk_http_request_t *dst)
{
    if (!src->body || !src->body->buffer || src->body->length == 0) {
        return 0;
    }
    dst->body = calloc(1, sizeof(*dst->body));
    if (!dst->body) {
        return -1;
    }
    dst->body->length = src->body->length;
    dst->body->buffer = malloc(dst->body->length);
    if (!dst->body->buffer) {
        return -1;
    }
    memcpy_s(dst->body->buffer, dst->body->length, src->body->buffer, dst->body->length);
    return 0;
}

awk_http_request_t *copy_request(const awk_http_request_t *src)
{
    if (!src) {
        return NULL;
    }
    awk_http_request_t *dst = (awk_http_request_t *)malloc(sizeof(awk_http_request_t));
    if (!dst) {
        return NULL;
    }
    dst->url = NULL;
    dst->headers.data = NULL;
    dst->headers.size = 0;
    dst->body = NULL;
    dst->method = src->method;
    dst->priority = src->priority;

    if (copy_request_url(src, dst) != 0 || copy_request_headers(src, dst) != 0 || copy_request_body(src, dst) != 0) {
        free_request(dst);
        return NULL;
    }
    return dst;
}

uint64_t amap_send(awk_http_request_t *request, awk_http_response_callback_t *callback)
{
    AMAP_LOGD("amap_send enter");
    if (!request) {
        AMAP_LOGE("request is null");
        return ULLONG_MAX;
    }
    if (!request->url) {
        AMAP_LOGE("request url is null");
        return ULLONG_MAX;
    }
    if (!callback) {
        AMAP_LOGE("callback is null");
        return ULLONG_MAX;
    }

    http_request_item *item = (http_request_item *)malloc(sizeof(http_request_item));
    if (!item) {
        AMAP_LOGE("malloc failed for item");
        return ULLONG_MAX;
    }

    item->request_id = ++g_current_request_id;
    item->request = request;
    if (!item->request) {
        AMAP_LOGE("copy_request failed");
        free(item);
        return ULLONG_MAX;
    }
    item->callback = callback;
    item->next = NULL;
    uint64_t request_id = item->request_id;
    process_request(item);

    return request_id;
}

void amap_cancel(uint64_t request_id)
{
    AMAP_LOGD("Cancelling request %llu", request_id);

    if (g_current_request_id == request_id) {
        AMAP_LOGD("Cancel current running request");
        g_cancel_current = true;
    } else {
        AMAP_LOGD("Request not running, ignore cancel");
    }
}