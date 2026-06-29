/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 *
 * Description: Provides ohos sample. \n
 * Author: CompanyName \n
 * History: \n
 * 2026-02-10， Create file. \n
 */
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include "sys/time.h"
#include "time.h"
#include "common_def.h"
#include "wearable_log.h"
#include "at_ohos_process_cpp/ohosfwk_at_netstack_dfx.h"
#include "httpclient.h"
#include "securec.h"
#include "lwip/sockets.h"
#include "net_websocket.h"
#include "net_websocket_type.h"

constexpr const char* PUT_HTTPS_URL = ""; // 测试https服务器url
constexpr const char* GET_HTTPS_URL = ""; // 测试https服务器url
constexpr const char* POST_HTTPS_URL = ""; // 测试https服务器url

constexpr const char* PUT_HTTP_URL = ""; // 测试http服务器url
constexpr const char* GET_HTTP_URL = ""; // 测试http服务器url
constexpr const char* POST_HTTP_URL = ""; // 测试http服务器url

constexpr const char* WSS_URL = ""; // 测试WSS服务器url
constexpr const char* WS_URL = ""; // 测试WS服务器url
constexpr int WS_MAX_DATA_LENGTH  = 20 * 1024;
constexpr const uint32_t RSP_BUF_SIZE = 2048;
constexpr const uint32_t REQ_BUF_SIZE = 2048;

#define DATA_LEN 1024
#define NUM_DATA_LEN 6
#define NANO_SECOND_TO_MILLI_SECOND (1000 * 1000)
#define SECOND_TO_MILLI_SECOND 1000
#define HTTP_HEAD_LEN 100

struct NetStackInfo {
    const char* item;
    const char* url;
    long connTime;
    long sendTime;
    long recvTime;
};

enum NetStackItem {
    HTTPS_GET_128B = 0,
    HTTPS_GET_400B,
    HTTPS_GET_1KB,
    HTTPS_GET_4KB,
    HTTPS_GET_16KB,
    HTTPS_GET_64KB,
    HTTPS_PUT_128B,
    HTTPS_PUT_400B,
    HTTPS_PUT_1KB,
    HTTPS_PUT_4KB,
    HTTPS_PUT_16KB,
    HTTPS_PUT_64KB,
    HTTPS_POST_128B,
    HTTPS_POST_400B,
    HTTPS_POST_1KB,
    HTTPS_POST_4KB,
    HTTPS_POST_16KB,
    HTTPS_POST_64KB,
    HTTP_GET_128B,
    HTTP_GET_400B,
    HTTP_GET_1KB,
    HTTP_GET_4KB,
    HTTP_GET_16KB,
    HTTP_GET_64KB,
    HTTP_PUT_128B,
    HTTP_PUT_400B,
    HTTP_PUT_1KB,
    HTTP_PUT_4KB,
    HTTP_PUT_16KB,
    HTTP_PUT_64KB,
    HTTP_POST_128B,
    HTTP_POST_400B,
    HTTP_POST_1KB,
    HTTP_POST_4KB,
    HTTP_POST_16KB,
    HTTP_POST_64KB,
    WSS_128B,
    WSS_400B,
    WSS_1KB,
    WSS_4KB,
    WSS_16KB,
    WSS_64KB,
    WS_128B,
    WS_400B,
    WS_1KB,
    WS_4KB,
    WS_16KB,
    WS_64KB,
    MAX_NET_STACK_ITEM,
};

static std::vector<char> g_data;
static const int g_dataLen[NUM_DATA_LEN] = {128, 400, 1024, 4096, 16384, 65536}; // 上下行数据长度
static NetStackInfo g_netStackInfo[] = {
    {"HttpsGet  128B", GET_HTTPS_URL, 0, 0, 0},
    {"HttpsGet  400B", GET_HTTPS_URL, 0, 0, 0},
    {"HttpsGet   1KB", GET_HTTPS_URL, 0, 0, 0},
    {"HttpsGet   4KB", GET_HTTPS_URL, 0, 0, 0},
    {"HttpsGet  16KB", GET_HTTPS_URL, 0, 0, 0},
    {"HttpsGet  64KB", GET_HTTPS_URL, 0, 0, 0},
    {"HttpsPut  128B", PUT_HTTPS_URL, 0, 0, 0},
    {"HttpsPut  400B", PUT_HTTPS_URL, 0, 0, 0},
    {"HttpsPut   1KB", PUT_HTTPS_URL, 0, 0, 0},
    {"HttpsPut   4KB", PUT_HTTPS_URL, 0, 0, 0},
    {"HttpsPut  16KB", PUT_HTTPS_URL, 0, 0, 0},
    {"HttpsPut  64KB", PUT_HTTPS_URL, 0, 0, 0},
    {"HttpsPost 128B", POST_HTTPS_URL, 0, 0, 0},
    {"HttpsPost 400B", POST_HTTPS_URL, 0, 0, 0},
    {"HttpsPost  1KB", POST_HTTPS_URL, 0, 0, 0},
    {"HttpsPost  4KB", POST_HTTPS_URL, 0, 0, 0},
    {"HttpsPost 16KB", POST_HTTPS_URL, 0, 0, 0},
    {"HttpsPost 64KB", POST_HTTPS_URL, 0, 0, 0},
    {"HttpGet   128B", GET_HTTP_URL, 0, 0, 0},
    {"HttpGet   400B", GET_HTTP_URL, 0, 0, 0},
    {"HttpGet    1KB", GET_HTTP_URL, 0, 0, 0},
    {"HttpGet    4KB", GET_HTTP_URL, 0, 0, 0},
    {"HttpGet   16KB", GET_HTTP_URL, 0, 0, 0},
    {"HttpGet   64KB", GET_HTTP_URL, 0, 0, 0},
    {"HttpPut   128B", PUT_HTTP_URL, 0, 0, 0},
    {"HttpPut   400B", PUT_HTTP_URL, 0, 0, 0},
    {"HttpPut    1KB", PUT_HTTP_URL, 0, 0, 0},
    {"HttpPut    4KB", PUT_HTTP_URL, 0, 0, 0},
    {"HttpPut   16KB", PUT_HTTP_URL, 0, 0, 0},
    {"HttpPut   64KB", PUT_HTTP_URL, 0, 0, 0},
    {"HttpPost  128B", POST_HTTP_URL, 0, 0, 0},
    {"HttpPost  400B", POST_HTTP_URL, 0, 0, 0},
    {"HttpPost   1KB", POST_HTTP_URL, 0, 0, 0},
    {"HttpPost   4KB", POST_HTTP_URL, 0, 0, 0},
    {"HttpPost  16KB", POST_HTTP_URL, 0, 0, 0},
    {"HttpPost  64KB", POST_HTTP_URL, 0, 0, 0},
    {"WSS       128B", WSS_URL, 0, 0, 0},
    {"WSS       400B", WSS_URL, 0, 0, 0},
    {"WSS        1KB", WSS_URL, 0, 0, 0},
    {"WSS        4KB", WSS_URL, 0, 0, 0},
    {"WSS       16KB", WSS_URL, 0, 0, 0},
    {"WSS       64KB", WSS_URL, 0, 0, 0},
    {"WS        128B", WS_URL, 0, 0, 0},
    {"WS        400B", WS_URL, 0, 0, 0},
    {"WS         1KB", WS_URL, 0, 0, 0},
    {"WS         4KB", WS_URL, 0, 0, 0},
    {"WS        16KB", WS_URL, 0, 0, 0},
    {"WS        64KB", WS_URL, 0, 0, 0},
};
static std::vector<char> g_httpResponseBuf;
static std::vector<char> g_httpHeaderBuf;

static struct timespec g_wsStart;
static struct timespec g_wsEnd;
static NetStackItem g_item = MAX_NET_STACK_ITEM;
static bool g_end = false;
static int g_clientId = -1;

static void DataInit(void)
{
    srand((unsigned int)time(nullptr)); // 初始化随机种子
    for (int i = 0; i < DATA_LEN; i++) {
        g_data.push_back(rand() % 256);
    }

    g_httpResponseBuf.reserve(RSP_BUF_SIZE);
    g_httpHeaderBuf.reserve(REQ_BUF_SIZE);
}

static void DataDeinit(void)
{
    g_data.clear();
}

static void PrintItemResult(void)
{
    // 打印耗时时间
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Test Item      conn           send           recv           ");
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "------------   ------------   ------------   ------------   ");
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "%s %12ld ms%12ld ms%12ld ms\n", g_netStackInfo[g_item].item,
        g_netStackInfo[g_item].connTime, g_netStackInfo[g_item].sendTime, g_netStackInfo[g_item].recvTime);
}

static int GetDataLen(NetStackItem item)
{
    switch (item) {
        case HTTPS_GET_128B:
        case HTTPS_PUT_128B:
        case HTTPS_POST_128B:
        case HTTP_GET_128B:
        case HTTP_PUT_128B:
        case HTTP_POST_128B:
        case WSS_128B:
        case WS_128B:
            return 128;
        case HTTPS_GET_400B:
        case HTTPS_PUT_400B:
        case HTTPS_POST_400B:
        case HTTP_GET_400B:
        case HTTP_PUT_400B:
        case HTTP_POST_400B:
        case WSS_400B:
        case WS_400B:
            return 400;
        case HTTPS_GET_1KB:
        case HTTPS_PUT_1KB:
        case HTTPS_POST_1KB:
        case HTTP_GET_1KB:
        case HTTP_PUT_1KB:
        case HTTP_POST_1KB:
        case WSS_1KB:
        case WS_1KB:
            return 1024;
        case HTTPS_GET_4KB:
        case HTTPS_PUT_4KB:
        case HTTPS_POST_4KB:
        case HTTP_GET_4KB:
        case HTTP_PUT_4KB:
        case HTTP_POST_4KB:
        case WSS_4KB:
        case WS_4KB:
            return 4096;
        case HTTPS_GET_16KB:
        case HTTPS_PUT_16KB:
        case HTTPS_POST_16KB:
        case HTTP_GET_16KB:
        case HTTP_PUT_16KB:
        case HTTP_POST_16KB:
        case WSS_16KB:
        case WS_16KB:
            return 16384;
        case HTTPS_GET_64KB:
        case HTTPS_PUT_64KB:
        case HTTPS_POST_64KB:
        case HTTP_GET_64KB:
        case HTTP_PUT_64KB:
        case HTTP_POST_64KB:
        case WSS_64KB:
        case WS_64KB:
            return 65536;
        default:
            return 0;
    }
}

static std::vector<char> GetDataByDataLen(int dataLen)
{
    std::vector<char> data;

    if (dataLen <= DATA_LEN) {
        data.assign(g_data.begin(), g_data.begin() + dataLen);
        return data;
    }

    // 预分配内存，避免多次扩容
    data.reserve(dataLen);

    // 重复追加数据
    int times = dataLen / DATA_LEN;
    for (int i = 0; i < times; ++i) {
        data.insert(data.end(), g_data.begin(), g_data.end());
    }
    return data;
}

static void HttpStream(const char* url, int method, char* head, char* data, int dataLen)
{
    struct timespec start;
    struct timespec end;
    long duration;
    int ret;
    NetStackInfo* netStackInfo = &(g_netStackInfo[g_item]);

    HttpClient client;
    (void)memset_s(&client, sizeof(HttpClient), 0, sizeof(HttpClient));
    client.header = head;
    HttpClientData clientData;
    (void)memset_s(&clientData, sizeof(HttpClientData), 0, sizeof(HttpClientData));
    clientData.responseBuf = g_httpResponseBuf.data();
    clientData.responseBufLen = RSP_BUF_SIZE;
    clientData.headerBuf = g_httpHeaderBuf.data();
    clientData.headerBufLen = REQ_BUF_SIZE;
    clientData.postBuf = data;
    clientData.postBufLen = dataLen;
    memset_s(clientData.responseBuf, RSP_BUF_SIZE, 0, RSP_BUF_SIZE);
    memset_s(clientData.headerBuf, REQ_BUF_SIZE, 0, REQ_BUF_SIZE);

    clock_gettime(CLOCK_MONOTONIC, &start);
    ret = HttpClientConn(&client, url);
    if (ret != HTTP_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Internal error! HttpClientConn failed, ret = %d", ret);
        goto Exit;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    netStackInfo->connTime = end.tv_sec * SECOND_TO_MILLI_SECOND + end.tv_nsec / NANO_SECOND_TO_MILLI_SECOND -
        (start.tv_sec * SECOND_TO_MILLI_SECOND + start.tv_nsec / NANO_SECOND_TO_MILLI_SECOND);

    clock_gettime(CLOCK_MONOTONIC, &start);
    ret = HttpClientSend(&client, url, method, &clientData);
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Internal error! HttpClientSend failed, ret = %d", ret);
        goto Exit;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    netStackInfo->sendTime = end.tv_sec * SECOND_TO_MILLI_SECOND + end.tv_nsec / NANO_SECOND_TO_MILLI_SECOND -
        (start.tv_sec * SECOND_TO_MILLI_SECOND + start.tv_nsec / NANO_SECOND_TO_MILLI_SECOND);

    clock_gettime(CLOCK_MONOTONIC, &start);
    do {
        // receive response data from server
        ret = HttpClientRecvResponse(&client, &clientData); // 一次最大读取2048字节
        if (ret < 0) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Internal error! HttpClientRecvResponse failed, ret = %d", ret);
            goto Exit;
        }
    } while (clientData.isMore);

    if (client.responseCode != 200) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Internal error! responseCode is %d", client.responseCode);
        goto Exit;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    netStackInfo->recvTime = end.tv_sec * SECOND_TO_MILLI_SECOND + end.tv_nsec / NANO_SECOND_TO_MILLI_SECOND -
        (start.tv_sec * SECOND_TO_MILLI_SECOND + start.tv_nsec / NANO_SECOND_TO_MILLI_SECOND);

Exit:
    (void)memset_s(clientData.headerBuf, REQ_BUF_SIZE, 0, REQ_BUF_SIZE);
    (void)memset_s(clientData.responseBuf, RSP_BUF_SIZE, 0, RSP_BUF_SIZE);
    HttpClientClose(&client);
}

static void WebSocketConnect(const char* url);

static void WebsocketSend(int clientId, char* data, int dataLen)
{
    NetStackInfo* netStackInfo = &(g_netStackInfo[g_item]);
    clock_gettime(CLOCK_MONOTONIC, &g_wsStart);
    int ret = 0;
    long duration = 0;
    if (dataLen <= WS_MAX_DATA_LENGTH * 2) {
        int startLength = dataLen / 2;
        int leftLength = dataLen - dataLen / 2;

        ret = WebSocketClientSendBinStart(g_clientId, data, dataLen / 2);
        if (ret != WEBSOCKET_OK) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WebSocketClientSendBinStart failed, ret = %d", ret);
            goto Exit;
        }

        ret = WebSocketClientSendBinEnd(g_clientId, data + startLength, leftLength);
    } else {
        ret = WebSocketClientSendBinStart(g_clientId, data, WS_MAX_DATA_LENGTH);
        if (ret != WEBSOCKET_OK) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WebSocketClientSendBinStart failed, ret = %d", ret);
            goto Exit;
        }

        int leftLength = dataLen - WS_MAX_DATA_LENGTH;
        do {
            ret = WebSocketClientSendBinContinue(g_clientId, data + dataLen - leftLength, WS_MAX_DATA_LENGTH);
            if (ret != WEBSOCKET_OK) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WebSocketClientSendBinContinue failed, ret = %d", ret);
                goto Exit;
            }
            leftLength -= WS_MAX_DATA_LENGTH;
        } while (leftLength > WS_MAX_DATA_LENGTH);

        ret = WebSocketClientSendBinEnd(g_clientId, data + dataLen - leftLength, leftLength);
    }
    if (ret != WEBSOCKET_OK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WebSocketClientSendBinEnd failed, ret = %d", ret);
        goto Exit;
    }

    clock_gettime(CLOCK_MONOTONIC, &g_wsEnd);
    netStackInfo->sendTime = g_wsEnd.tv_sec * SECOND_TO_MILLI_SECOND + g_wsEnd.tv_nsec / NANO_SECOND_TO_MILLI_SECOND -
        (g_wsStart.tv_sec * SECOND_TO_MILLI_SECOND + g_wsStart.tv_nsec / NANO_SECOND_TO_MILLI_SECOND);
    clock_gettime(CLOCK_MONOTONIC, &g_wsStart);
    return;

Exit:
    ret = WebSocketClientClose(g_clientId);
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WebSocketClientClose failed, ret = %d", ret);
    }
    g_end = true;
}

static void OnError(int clientId, struct WebSocketErrorResult errorResult)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OnError ok");
    DataDeinit();
    g_end = true;
}

static void OnClose(int clientId, struct WebSocketErrorResult errorResult)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OnClose ok");
    PrintItemResult();
    DataDeinit();
    g_end = true;
}

static void OnMessage(int clientId, const char *data, size_t length, bool isFinished, bool isBinary)
{
    clock_gettime(CLOCK_MONOTONIC, &g_wsEnd);
    // 统计接收时间
    NetStackInfo* netStackInfo = &(g_netStackInfo[g_item]);
    netStackInfo->recvTime = g_wsEnd.tv_sec * SECOND_TO_MILLI_SECOND + g_wsEnd.tv_nsec / NANO_SECOND_TO_MILLI_SECOND -
        (g_wsStart.tv_sec * SECOND_TO_MILLI_SECOND + g_wsStart.tv_nsec / NANO_SECOND_TO_MILLI_SECOND);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OnMessage ok");
    g_end = true;
}

static void OnOpen(int clientId)
{
    clock_gettime(CLOCK_MONOTONIC, &g_wsEnd);
    // 统计连接耗时时间
    NetStackInfo* netStackInfo = &(g_netStackInfo[g_item]);
    netStackInfo->connTime = g_wsEnd.tv_sec * SECOND_TO_MILLI_SECOND + g_wsEnd.tv_nsec / NANO_SECOND_TO_MILLI_SECOND -
        (g_wsStart.tv_sec * SECOND_TO_MILLI_SECOND + g_wsStart.tv_nsec / NANO_SECOND_TO_MILLI_SECOND);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OnOpen ok");

    int dataLen = GetDataLen(g_item);
    std::vector<char> data = GetDataByDataLen(dataLen);
    WebsocketSend(g_clientId, data.data(), dataLen);
}

static void WebSocketConnect(const char* url)
{
    g_end = false;
    clock_gettime(CLOCK_MONOTONIC, &g_wsStart);
    g_clientId = WebSocketClientConstructor(OnOpen, OnMessage, OnError, OnClose);
    if (g_clientId < 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WebSocketClientConstructor failed");
        return;
    }

    int ret = WebSocketClientConnect(g_clientId, url, nullptr);
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WebSocketClientConnect failed, ret = %d", ret);
        ret = WebSocketClientClose(g_clientId);
        if (ret != 0) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WebSocketClientClose failed, ret = %d", ret);
        }
        return;
    }

    while (!g_end) {
        sleep(1); /* 1 seconds */
    }

    ret = WebSocketClientClose(g_clientId);
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WebSocketClientClose failed, ret = %d", ret);
    }
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WebSocketClientClose");
    return;
}

static void HttpGet(void)
{
    int dataLen = GetDataLen(g_item);
    HttpStream(g_netStackInfo[g_item].url, HTTP_GET, nullptr, nullptr, 0);
    PrintItemResult();
}

static void HttpPut(void)
{
    int dataLen = GetDataLen(g_item);
    std::vector<char> data = GetDataByDataLen(dataLen);
    char header[HTTP_HEAD_LEN] = {0};
    (void)sprintf_s(header, HTTP_HEAD_LEN,
        "Content-Type: application/octet-stream\r\nX-Upload-Size: %d\r\nConnection: keep-alive\r\n", dataLen);
    HttpStream(g_netStackInfo[g_item].url, HTTP_PUT, header, data.data(), dataLen);
    PrintItemResult();
}

static void HttpPost(void)
{
    int dataLen = GetDataLen(g_item);
    std::vector<char> data = GetDataByDataLen(dataLen);
    char header[] = "Content-Type: application/octet-stream\r\nConnection: keep-alive\r\n";
    HttpStream(g_netStackInfo[g_item].url, HTTP_POST, header, data.data(), dataLen);
    PrintItemResult();
}

static void PrintAllItem(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Test Item      conn           send           recv           ");
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "------------   ------------   ------------   ------------   ");
    for (int i = 0; i < MAX_NET_STACK_ITEM; i++) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "%s %12ld ms%12ld ms%12ld ms\n", g_netStackInfo[i].item,
            g_netStackInfo[i].connTime, g_netStackInfo[i].sendTime, g_netStackInfo[i].recvTime);
    }
}

uint32_t OhosfwkATNetstackDfx(uint8_t argc, char *argv[])
{
    // 参数校验
    if (argc != 1 || argv[0] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Set Parameter Failed\n");
        return ERRCODE_FAIL;
    }
    g_item = static_cast<NetStackItem>(atoi(argv[0]));
    if (g_item < HTTPS_GET_128B || g_item > MAX_NET_STACK_ITEM) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Set Parameter Failed\n");
        return ERRCODE_FAIL;
    }

    if (g_item == MAX_NET_STACK_ITEM) {
        PrintAllItem();
        return ERRCODE_SUCC;
    }

    // 初始化参数
    DataInit();
    g_netStackInfo[g_item].connTime = 0;
    g_netStackInfo[g_item].sendTime = 0;
    g_netStackInfo[g_item].recvTime = 0;

    switch (g_item) {
        case HTTPS_GET_128B:
        case HTTPS_GET_400B:
        case HTTPS_GET_1KB:
        case HTTPS_GET_4KB:
        case HTTPS_GET_16KB:
        case HTTPS_GET_64KB:
        case HTTP_GET_128B:
        case HTTP_GET_400B:
        case HTTP_GET_1KB:
        case HTTP_GET_4KB:
        case HTTP_GET_16KB:
        case HTTP_GET_64KB:
            HttpGet();
            DataDeinit();
            break;
        case HTTPS_PUT_128B:
        case HTTPS_PUT_400B:
        case HTTPS_PUT_1KB:
        case HTTPS_PUT_4KB:
        case HTTPS_PUT_16KB:
        case HTTPS_PUT_64KB:
        case HTTP_PUT_128B:
        case HTTP_PUT_400B:
        case HTTP_PUT_1KB:
        case HTTP_PUT_4KB:
        case HTTP_PUT_16KB:
        case HTTP_PUT_64KB:
            HttpPut();
            DataDeinit();
            break;
        case HTTPS_POST_128B:
        case HTTPS_POST_400B:
        case HTTPS_POST_1KB:
        case HTTPS_POST_4KB:
        case HTTPS_POST_16KB:
        case HTTPS_POST_64KB:
        case HTTP_POST_128B:
        case HTTP_POST_400B:
        case HTTP_POST_1KB:
        case HTTP_POST_4KB:
        case HTTP_POST_16KB:
        case HTTP_POST_64KB:
            HttpPost();
            DataDeinit();
            break;
        case WSS_128B:
        case WSS_400B:
        case WSS_1KB:
        case WSS_4KB:
        case WSS_16KB:
        case WSS_64KB:
            WebSocketConnect(WSS_URL);
            break;
        case WS_128B:
        case WS_400B:
        case WS_1KB:
        case WS_4KB:
        case WS_16KB:
        case WS_64KB:
            WebSocketConnect(WS_URL);
            break;
        default:
            DataDeinit();
            return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}