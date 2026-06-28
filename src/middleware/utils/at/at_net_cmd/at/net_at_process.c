/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides net sample. \n
 * Author: CompanyName \n
 * History: \n
 * 2022-11-17， Create file. \n
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "sys/time.h"
#include "time.h"
#include "common_def.h"
#include "wearable_log.h"
#include "lwip_public.h"
#include "lwip/ip_addr.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#ifdef AT_COMMAND_MQTT
#include "MQTTClient.h"
#endif
#ifdef AT_COMMAND_HTTP
#include "httpclient.h"
#endif
#ifdef AT_COMMAND_WEBSOCKET
#include "net_websocket.h"
#endif
#include "apps/ping.h"
#include "apps/sntp.h"
#include "connection_manage/connection_manage.h"
#include "network_listener.h"
#include "avc.h"
#include "cmsis_os.h"
#include "nettool/misc.h"

#define CLIENTID    "ExampleClientPub"
#define TOPIC       "MQTT Examples"
#define PAYLOAD     "Hello World!, this is a message from 3322"
#define QOS         1
#define TIMEOUT     10000L
#define HTTP_STATUS_OK   200
#define IMG_HEADER_SIZE  8
#define IMG_FOOTER_SIZE  6
#define BASE_DECIMAL     10
#define RECV_DATA_SLEEP_TIME  10000

uint32_t NetATOpenBtPan(uint8_t argc, char *argv[])
{
    (void)argc;
    (void)argv;
    lwip_open_bt_pan();
    return ERRCODE_SUCC;
}

#ifdef AT_COMMAND_MQTT
static void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
}

static int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("     message length:%d\n", message->payloadlen);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

static void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int mqtt_client_test(char *url)
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    if (url == NULL) {
        printf("URL is empty, please enter it\n");
        return -1;
    }
    if ((rc = MQTTClient_create(&client, url, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) !=
        MQTTCLIENT_SUCCESS) {
        printf("Failed to create client, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto exit;
    }

    if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    conn_opts.keepAliveInterval = 10; /* 10 seconds */
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    pubmsg.payload = (char *)PAYLOAD;
    pubmsg.payloadlen = (int)strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    if ((rc = MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish message, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n",
        TOPIC,
        CLIENTID,
        QOS);
    if ((rc = MQTTClient_subscribe(client, TOPIC, QOS)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to subscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }
    printf("subscribe, return code %d\n", rc);

    if ((rc = MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish message, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    sleep(5); /* 5 seconds */

    if ((rc = MQTTClient_unsubscribe(client, TOPIC)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to unsubscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    if ((rc = MQTTClient_disconnect(client, TIMEOUT)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to disconnect, return code %d\n", rc);
        rc = EXIT_FAILURE;
    }

destroy_exit:
    MQTTClient_destroy(&client);
exit:
    return rc;
}

int mqtt_secure_client_test(char *url)
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
    if (url == NULL) {
        printf("URL is empty, please enter it\n");
        return -1;
    }
    if ((rc = MQTTClient_create(&client, url, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) !=
        MQTTCLIENT_SUCCESS) {
        printf("Failed to create client, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto exit;
    }

    conn_opts.ssl = &ssl_opts;

    if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    conn_opts.keepAliveInterval = 10;  /* 10 seconds */
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    pubmsg.payload = (char *)PAYLOAD;
    pubmsg.payloadlen = (int)strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    if ((rc = MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish message, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n",
        TOPIC,
        CLIENTID,
        QOS);
    if ((rc = MQTTClient_subscribe(client, TOPIC, QOS)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to subscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }
    printf("subscribe, return code %d\n", rc);

    if ((rc = MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish message, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }
    sleep(10); /* 10 seconds */

    if ((rc = MQTTClient_unsubscribe(client, TOPIC)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to unsubscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    if ((rc = MQTTClient_disconnect(client, TIMEOUT)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to disconnect, return code %d\n", rc);
        rc = EXIT_FAILURE;
    }

destroy_exit:
    MQTTClient_destroy(&client);
exit:
    return rc;
}

uint32_t NetATMqttSecure(uint8_t argc, char *argv[])
{
    if (argc > 0) {
        mqtt_secure_client_test(argv[0]);
    } else {
        mqtt_secure_client_test(NULL);
    }
    return ERRCODE_SUCC;
}

uint32_t NetATMqtt(uint8_t argc, char *argv[])
{
    if (argc > 0) {
        mqtt_client_test(argv[0]);
    } else {
        mqtt_client_test(NULL);
    }
    return ERRCODE_SUCC;
}
#endif
#ifdef AT_COMMAND_HTTP
#define REQ_BUF_SIZE 2048    /* @brief http request buffer */
#define RSP_BUF_SIZE 2048    /* @brief http response buffer */

typedef struct {
    char *url;
    char *filepath;
} HttpDownloadConfig;

#define HTTP_STACK_SIZE 0X1000
#define TASK_PRIORITY_HTTP_TASK (osPriority_t)(33)

static void http_test_01(void *arg)
{
    HttpDownloadConfig *config = (HttpDownloadConfig *)arg;
    HttpClient client = {0};
    HttpClientData client_data = {0};
    int ret;
    if (config == NULL) {
        return;
    }

    if (config->url == NULL) {
        printf("URL is empty, please enter it\n");
        return;
    }
    printf("http request %s\r\n", config->url);
    int fileSize;

    client_data.headerBuf = malloc(REQ_BUF_SIZE);
    if (client_data.headerBuf == NULL) {
        printf("headerBuf malloc fail\n");
        ret = -1;
        goto EXIT;
    }
    client_data.headerBufLen = REQ_BUF_SIZE;

    client_data.responseBuf = malloc(RSP_BUF_SIZE);
    if (client_data.responseBuf == NULL) {
        printf("responseBuf malloc fail\n");
        ret = -1;
        goto EXIT;
    }
    client_data.responseBufLen = RSP_BUF_SIZE;

    client.timeoutMs = (40000); /* 40000 : timeout */
    fileSize = 0;
    ret = HttpClientConn(&client, config->url);
    if (ret == 0) {
        ret = HttpClientSend(&client, config->url, HTTP_GET, &client_data);
        do {
            ret = HttpClientRecvResponse(&client, &client_data);
            if (client_data.contentBlockLen > 0) {
                fileSize += client_data.contentBlockLen;
            }
            usleep(RECV_DATA_SLEEP_TIME);
        } while (client_data.isMore && ret >= 0);
        if (client_data.responseContentLen == 0) {
            printf("recv length=0, fail\n");
        }
    } else {
        printf("connect fail, ret=0x%x\n", ret);
        goto EXIT;
    }
    if (client.responseCode == HTTP_STATUS_OK && fileSize == client_data.responseContentLen && fileSize > 0) {
        ret = 0;
    } else {
        ret = -1;
    }

EXIT:
    if (ret != 0) {
        printf("\ntest case fail, filesize=%d\n", fileSize);
    } else {
        printf("\ntest case ok, filesize=%d\n", fileSize);
    }
    if (client_data.responseBuf) {
        free(client_data.responseBuf);
        client_data.responseBuf = NULL;
    }
    if (client_data.headerBuf) {
        free(client_data.headerBuf);
        client_data.headerBuf = NULL;
    }
    free(config->url);
    free(config);
    HttpClientClose(&client);
}

uint32_t NetATHttp(uint8_t argc, char *argv[])
{
    osThreadAttr_t attr = {0};
    HttpDownloadConfig *config = (HttpDownloadConfig*)malloc(sizeof(HttpDownloadConfig));
    if (config == NULL) {
        return ERRCODE_FAIL;
    }
    memset_s(config, sizeof(HttpDownloadConfig), 0, sizeof(HttpDownloadConfig));

    if (argc > 0) {
        config->url = strdup(argv[0]);
    }
    if (config->url == NULL) {
        printf("URL is empty, please enter it\n");
        return ERRCODE_FAIL;
    }

    attr.stack_size = HTTP_STACK_SIZE;
    attr.priority = TASK_PRIORITY_HTTP_TASK;
    attr.name = "http_download_task";
    if (osThreadNew(http_test_01, (void *)config, &attr) == NULL) {
        printf("osThreadNew fail\n");
        free(config->url);
        free(config);
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}
#endif
#ifdef AT_COMMAND_WEBSOCKET
static bool g_test_end = false;
static bool g_test_open = false;

static void OnOpenWs(int client)
{
    printf("OnOpen ok, client=%d\n", client);
    g_test_open = true;
}

static void OnErrorWs(int client, struct WebSocketErrorResult error)
{
    printf("OnError,errcode=%d, messg=%s\n", error.errorCode, error.errorMessage);
}

static void OnCloseWs(int client, struct WebSocketErrorResult errorResult)
{
    printf("client=%d, OnClose callback,errcode=%d, messg=%s\n", client, errorResult.errorCode, errorResult.errorMessage);
    g_test_end = true;
}

static void OnMessageWs(int client, const char *data, size_t length, bool isFinished, bool isBinary)
{
    printf("client(%d) onMesssage, length=%u, isFinished=%d\n", client, length, isFinished);
    printf("\n");
}

int ws_test_case_01(char *url)
{
    int clientFd = 0;
    int ret;
    g_test_open = false;
    g_test_end = false;
    const char *text = "hello world";
    if (url == NULL) {
        printf("URL is empty, please enter it\n");
        return -1;
    }
    printf("url=%s\n", url);

    clientFd = WebSocketClientConstructor(OnOpenWs, OnMessageWs, OnErrorWs, OnCloseWs);
    if (clientFd < 0) {
        printf("client fd fail=%d\n", clientFd);
        return -1;
    }

    ret = WebSocketClientConnect(clientFd, url, NULL);
    if (ret != 0) {
        printf("connect fail, ret=0x%x\n", ret);
        return ret;
    }

    while (!g_test_open) {
        sleep(1); /* 1 seconds */
        if (g_test_end) {
            goto END;
        }
    }
    ret = WebSocketClientSendText(clientFd, text, strlen(text));
    if (ret != 0) {
        printf("WebSocketClientSendText fail, ret=0x%x\n", ret);
    }
    printf("send finish\r\n");

    sleep(5); /* 5 seconds */
END:
    ret = WebSocketClientClose(clientFd);
    if (ret != 0) {
        printf("close fail, ret=0x%x\n", ret);
    } else {
        printf("close ok\n");
    }
    return 0;
}

uint32_t NetATWebsocket(uint8_t argc, char *argv[])
{
    int ret;
    if (argc > 0) {
        ws_test_case_01((char *)argv[0]);
    } else {
        ws_test_case_01(NULL);
    }
    return ERRCODE_SUCC;
}
#endif

uint32_t NetATSwitch(uint8_t argc, char *argv[])
{
    if (argc > 0 && strcmp((char *)argv[0], "PAN") == 0) {
        lwip_netif_switch_bt_pan();
    } else if (argc > 0 && strcmp((char *)argv[0], "CAT1") == 0) {
        lwip_netif_switch_volte();
    }
    return ERRCODE_SUCC;
}

uint32_t NetATPing(uint8_t argc, char *argv[])
{
#ifdef __LITEOS__
    if (argc > 0) {
        os_start_ping(argv[0], strlen(argv[0]));
    }
#endif
    return ERRCODE_SUCC;
}

#define MAX_URL_LEN 128
static char sntp_server[MAX_URL_LEN] = {0};

uint32_t NetATSntp(uint8_t argc, char *argv[])
{
    if (argc == 1 && strcmp((char *)argv[0], "stop") == 0) {
        sntp_stop();
        printf("sntp stop\n");
    } else if (argc == 1) {
        if (memcpy_s(sntp_server, MAX_URL_LEN, argv[0], strlen(argv[0]) + 1) != 0) {
            printf("memcpy_s failed\n");
            return ERRCODE_FAIL;
        }

        sntp_setservername(0, sntp_server);
        sntp_init();
        printf("sntp start\n");
    }

    return ERRCODE_SUCC;
}

#ifdef __LITEOS__
#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t CmdIperf(uint8_t argc, char *argv[]);
extern uint32_t CmdIfconfig(uint8_t argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif
uint32_t NetATIperf(uint8_t argc, char *argv[])
{
#ifdef __LITEOS__
    return CmdIperf(argc, argv);
#else
    return ERRCODE_SUCC;
#endif
}

uint32_t NetATIfconfig(uint8_t argc, char *argv[])
{
#ifdef __LITEOS__
    return CmdIfconfig(argc, argv);
#else
    return ERRCODE_SUCC;
#endif
}

uint32_t NetATGetDnsConfig(uint8_t argc, char *argv[])
{
    (void)argc;
    (void)argv;
#ifdef __LITEOS__
    return os_shell_show_dns();
#else
    return ERRCODE_SUCC;
#endif
}

uint32_t NetATSetDnsConfig(uint8_t argc, char *argv[])
{
#ifdef __LITEOS__
    return os_shell_dns(argc, (const char **)argv);
#else
    return ERRCODE_SUCC;
#endif
}

uint32_t NetATTcpServer(uint8_t argc, char *argv[])
{
#ifdef __LITEOS__
    return os_tcpserver(argc, (const char **)argv);
#else
    return ERRCODE_SUCC;
#endif
}

uint32_t NetATTcpClient(uint8_t argc, char *argv[])
{
#ifdef __LITEOS__
    return os_tcpclient(argc, (const char **)argv);
#else
    return ERRCODE_SUCC;
#endif
}

uint32_t NetATUdpServer(uint8_t argc, char *argv[])
{
#ifdef __LITEOS__
    return udpserver(argc, (const char **)argv);
#else
    return ERRCODE_SUCC;
#endif
}

uint32_t NetATUdpClient(uint8_t argc, char *argv[])
{
#ifdef __LITEOS__
    return udpclient(argc, (const char **)argv);
#else
    return ERRCODE_SUCC;
#endif
}

#ifdef CHBA_SUPPORT
#define LOCAL_IP_ARG_INDEX    1
#define GATEWAY_ARG_INDEX     2
#define NETMASK_ARG_INDEX     3

uint32_t NetATSLECHBA(uint8_t argc, char *argv[])
{
    uint8_t local_mac[6] = {0};  /* 6: mac length */
    uint8_t remote_mac[6] = {0}; /* 6: mac length */
    int mac_count = 0;

    if (argc == 0) {
        return ERRCODE_FAIL;
    }
    if (argc < 4) { /* 4 : param number */
        printf(
            "you need input transmode(0 or 1), ip addr, gateway, netmask, sle local mac addr(option), sle remote mac "
            "addr(option)\n");
        return ERRCODE_FAIL;
    }

    if (argc == 6) { /* 6 : number */
        mac_count = sscanf_s(argv[4], /* 4 : index */
            "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
            &local_mac[0], /* 0 : index */
            &local_mac[1], /* 1 : index */
            &local_mac[2], /* 2 : index */
            &local_mac[3], /* 3 : index */
            &local_mac[4], /* 4 : index */
            &local_mac[5]); /* 5 : index */
        if (mac_count != 6) { /* 6 : number */
            printf("local mac parse fail\n");
            return ERRCODE_FAIL;
        }
        mac_count = sscanf_s(argv[5], /* 5 : index */
            "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
            &remote_mac[0], /* 0 : index */
            &remote_mac[1], /* 1 : index */
            &remote_mac[2], /* 2 : index */
            &remote_mac[3], /* 3 : index */
            &remote_mac[4], /* 4 : index */
            &remote_mac[5]); /* 5 : index */
        if (mac_count != 6) { /* 6 : index */
            printf("remote mac parse fail\n");
            return ERRCODE_FAIL;
        }
    }

    int trans_mode = SLE_NONE_MODE;
    if (strcmp(argv[0], "0") == 0) {
        trans_mode = SLE_RECEIVER_MODE;
    } else {
        trans_mode = SLE_TRANSMITTER_MODE;
    }

    ip4_addr_t local_ip;
    ip4_addr_t gateway;
    ip4_addr_t netmask;

    ip4addr_aton(argv[LOCAL_IP_ARG_INDEX], &local_ip);
    ip4addr_aton(argv[GATEWAY_ARG_INDEX], &gateway);
    ip4addr_aton(argv[NETMASK_ARG_INDEX], &netmask);

    if (argc == 6) { /* 6 : mac length */
        sle_chba_set_mac_addr(local_mac, sizeof(local_mac), remote_mac, sizeof(remote_mac));
    }

    errcode_t ret = sle_chba_init(trans_mode, local_ip, gateway, netmask);
    if (ret != 0) {
        printf("sle chba init fail, ret=0x%x\n", ret);
    }

    return ERRCODE_SUCC;
}
#endif

#ifdef AT_COMMAND_HTTP

const char *temp_path = "/user/download.bin";

static void http_download(void *arg)
{
    HttpDownloadConfig *config = (HttpDownloadConfig *)arg;
    HttpClient client = {0};
    HttpClientData client_data = {0};
    int ret = -1;
    size_t fileSize = 0;
    FILE *fd = NULL;
    char *real_path = NULL;

    if (config->filepath == NULL) {
        config->filepath = strdup(temp_path);
        if (config->filepath == NULL) {
            goto EXIT;
        }
    }

    real_path = realpath(config->filepath, NULL);
    if (real_path == NULL) {
        printf("Invalid file path:%s\n", config->filepath);
        goto EXIT;
    }
    free(config->filepath);
    config->filepath= real_path;

    client_data.headerBuf = malloc(REQ_BUF_SIZE);
    if (client_data.headerBuf == NULL) {
        goto EXIT;
    }
    client_data.headerBufLen = REQ_BUF_SIZE;

    client_data.responseBuf = malloc(RSP_BUF_SIZE);
    if (client_data.responseBuf == NULL) {
        goto EXIT;
    }
    client_data.responseBufLen = RSP_BUF_SIZE;
    client.timeoutMs = (40000); /* 40000 : timeout */
    printf("\n http request url: %s, path: %s\r\n", config->url, config->filepath);
    fd = fopen(config->filepath, "wb");
    if (fd == NULL) {
        printf("fopen fail\n");
        goto EXIT;
    }

    ret = HttpClientConn(&client, config->url);
    if (ret != 0) {
        printf("connect fail\n");
        goto EXIT;
    }
    ret = HttpClientSend(&client, config->url, HTTP_GET, &client_data);
    if (ret != 0) {
        printf("http send fail\n");
        goto EXIT;
    }

    do {
        ret = HttpClientRecvResponse(&client, &client_data);
        if (ret < 0) {
            printf("http recv fail\n");
            goto EXIT;
        }
        size_t writeSize = 0;
        if (client_data.contentBlockLen > 0) {
            writeSize = fwrite(client_data.responseBuf, 1, client_data.contentBlockLen, fd);
        }
        if (writeSize < (size_t)client_data.contentBlockLen) {
            printf("write data fail\n");
            goto EXIT;
        }
        fileSize += writeSize;
        usleep(RECV_DATA_SLEEP_TIME);
    } while (client_data.isMore);
    
    if (client.responseCode == HTTP_STATUS_OK && fileSize == (size_t)client_data.responseContentLen && fileSize > 0) {
        printf("http download ok, filesize=%d\n", fileSize);
    } else {
        printf("http download fail, filesize=%d\n", fileSize);
    }

EXIT:
    if (fd) {
        fclose(fd);
    }
    if (client_data.responseBuf) {
        free(client_data.responseBuf);
        client_data.responseBuf = NULL;
    }
    if (client_data.headerBuf) {
        free(client_data.headerBuf);
        client_data.headerBuf = NULL;
    }
    if (config->url) {
        free(config->url);
    }
    if (config->filepath) {
        free(config->filepath);
    }
    free(config);
    HttpClientClose(&client);
}

uint32_t NetATHttpDownLoad(uint8_t argc, char *argv[])
{
    osThreadAttr_t attr = {0};
    HttpDownloadConfig *config = (HttpDownloadConfig*)malloc(sizeof(HttpDownloadConfig));
    if (config == NULL) {
        return ERRCODE_FAIL;
    }
    memset_s(config, sizeof(HttpDownloadConfig), 0, sizeof(HttpDownloadConfig));

    if (argc >= 1) {
        config->url = strdup(argv[0]);
    }
    if (config->url == NULL) {
        printf("URL is empty, please enter it\n");
        return ERRCODE_FAIL;
    }
    if (argc >= 2) {    /* 2: param number */
        config->filepath = strdup(argv[1]);
    } else {
        config->filepath = NULL;
    }

    attr.stack_size = HTTP_STACK_SIZE;
    attr.priority = TASK_PRIORITY_HTTP_TASK;
    attr.name = "http_download_task";
    if (osThreadNew(http_download, (void *)config, &attr) == NULL) {
        printf("osThreadNew fail\n");
        free(config->url);
        free(config->filepath);
        free(config);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
#endif

#ifdef NET_AT_TEST_NETSTACK

#define VIDEO_STACK_SIZE 0x1000
#define TASK_PRIORITY_VIDEO_TASK (osPriority_t)(17)
static char *g_video_file_path = NULL;
static char *g_recv_file_path = NULL;
static char *g_server_ip = NULL;
#define SERVER_PORT 80
#define VIDEO_FILE_PATH "/user/data/video.h264"
#define BUFFER_SIZE (1400)
#define SEND_DATA_DELAY_US 33000

static void video_send_task(void *arg)
{
    StreamSourceHandle ssh = {0};
    uint8_t keyFrame = 0;
    int32_t frameLen = 0;
    int sock_fd;
    struct sockaddr_in server_addr;
    int bytes_sent;
    uint8_t *frame = NULL;
    int bytes_received;
    char ack_response[4] = {0};
    uint8_t isFirstSend = 1;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        printf("Socket creation failed\n");
        return;
    }
    // 2. 配置服务器地址
    (void)memset_s(&server_addr, sizeof(server_addr), 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(g_server_ip);

    // 3. 连接服务器
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed\n");
        goto ERROR;
    }
    printf("Connected to server %s:%d\n", g_server_ip, SERVER_PORT);

    int ret;
    FILE *pfd = fopen(g_video_file_path, "r");
    if (pfd == NULL) {
        printf("avc open fail, file path=%s\n", g_video_file_path);
        goto ERROR;
    }
    frame = (uint8_t *)malloc(BUFFER_SIZE); /* 21 * 1024 : frame size */
    if (frame == NULL) {
        goto FCLOSE;
    }
    while (true) {
        frameLen = fread(frame, 1, BUFFER_SIZE, pfd);
        if (frameLen <= 0) {
            printf("fread failed, frameLen:%d\n", frameLen);
            break;
        }
        bytes_sent = send(sock_fd, frame, frameLen, 0);
        if (bytes_sent < 0) {
            printf("Send failed, bytes_sent:%d\n", bytes_sent);
            break;
        } else {
            printf("send bytes=%d Bytes\n", bytes_sent);
        }
        if (frameLen != BUFFER_SIZE) {
            break;
        }
        usleep(SEND_DATA_DELAY_US);
    }
    printf("send avc end\n");
FCLOSE:
    fclose(pfd);
ERROR:
    closesocket(sock_fd);
    free(g_video_file_path);
    g_video_file_path = NULL;
    free(g_server_ip);
    g_server_ip = NULL;
    if (frame != NULL) {
        free(frame);
    }
}

static void video_recv_task(void *arg)
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    int bytes_received;
    FILE *fp = NULL;

    // 1. 创建socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        printf("Failed to create socket\n");
        return;
    }

    // 2. 设置地址重用（可选）
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3. 配置服务器地址
    (void)memset_s(&server_addr, sizeof(server_addr), 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // 4. 绑定socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Bind failed\n");
        closesocket(server_fd);
        return;
    }

    // 5. 开始监听
    if (listen(server_fd, 5) < 0) {
        printf("Listen failed\n");
        closesocket(server_fd);
        return;
    }

    printf("TCP Server started on port %d\n", SERVER_PORT);

    // 使用动态设置的接收文件路径
    if (g_recv_file_path == NULL) {
        printf("Receive file path is not set.\n");
        return;
    }

    printf("Received data will be saved to: %s\n", g_recv_file_path);

    // 6. 接受客户端连接
    client_addr_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0) {
        printf("Accept failed\n");
        closesocket(server_fd);
        return;
    }
    // 显示客户端信息
    printf("Client connected\n");
    // 8. 接收数据并保存到文件
    size_t total_bytes = 0;
    uint8_t *recv_buffer = (uint8_t *)malloc(BUFFER_SIZE);
    if (recv_buffer == NULL) {
        goto ERROR;
    }
    (void)memset_s(recv_buffer, BUFFER_SIZE, 0, BUFFER_SIZE);

    fp = fopen(g_recv_file_path, "wb");
    if (fp == NULL) {
        printf("Failed to open file for writing\n");
        goto ERROR;
    }

    while (1) {
        bytes_received = recv(client_fd, recv_buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0) {
            // 将接收到的数据写入文件
            size_t bytes_written = fwrite(recv_buffer, 1, bytes_received, fp);
            if (bytes_written != (size_t)bytes_received) {
                printf("File write error\n");
                break;
            }
            total_bytes += bytes_received;
            printf("Received %ld bytes, total: %zu bytes\n", bytes_received, total_bytes);

        } else if (bytes_received == 0) {
            printf("Client disconnected\n");
            break;
        } else {
            printf("Receive error\n");
            break;
        }
    }
ERROR:
    // 9. 清理资源
    if (fp != NULL) {
        fclose(fp);
        printf("Data saved to %s, total %zu bytes\n", g_recv_file_path, total_bytes);
    }

    // 清理接收文件路径
    if (g_recv_file_path != NULL) {
        free(g_recv_file_path);
        g_recv_file_path = NULL;
    }
    closesocket(server_fd);
    closesocket(client_fd);
    free(recv_buffer);
}

uint32_t NetATSendAvc(uint8_t argc, char *argv[])
{
    osThreadAttr_t attr = {0};

    if (argc == 0) {
        printf("you need input video file path, server ip address\n");
        return ERRCODE_FAIL;
    }
    if (argc > 1) { /* 1: para num */
        g_video_file_path = strdup(argv[0]);
        g_server_ip = strdup(argv[1]);
    } else {
        return ERRCODE_FAIL;
    }
    attr.stack_size = VIDEO_STACK_SIZE;
    attr.priority = TASK_PRIORITY_VIDEO_TASK;
    attr.name = "video_send_test";
    osThreadNew(video_send_task, NULL, &attr);

    return ERRCODE_SUCC;
}

uint32_t NetATRecvAvc(uint8_t argc, char *argv[])
{
    osThreadAttr_t attr = {0};
    if (argc == 0) {
        printf("You need to input the file path to save the received video.\n");
        return 0;
    }

    // 从AT命令参数中获取接收文件的路径
    g_recv_file_path = strdup(argv[0]);
    attr.stack_size = VIDEO_STACK_SIZE;
    attr.priority = TASK_PRIORITY_VIDEO_TASK;
    attr.name = "video_recv_test";
    osThreadNew(video_recv_task, NULL, &attr);
    return ERRCODE_SUCC;
}

#ifdef AT_COMMAND_MQTT
static int mqtt_client_sendtext_test(char *url, size_t url_len, char *file_path, size_t file_path_len)
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    if (url == NULL || file_path == NULL || url_len == 0 || file_path_len == 0) {
        printf("URL or file_path is empty, please enter it\n");
        return -1;
    }

    FILE* txtFile = fopen(file_path, "r");
    if (txtFile == NULL) {
        printf("failed to open text file:%s\n", file_path);
        return EXIT_FAILURE;
    }

    (void)fseek(txtFile, 0, SEEK_END);
    long tempSize = ftell(txtFile);
    if (tempSize < 0) {
        printf("File size overflow or error\n");
        (void)fclose(txtFile);
        return EXIT_FAILURE;
    }
    size_t txtFileSize = (size_t)tempSize;
    (void)fseek(txtFile, 0, SEEK_SET);

    char* data = (char*)malloc(txtFileSize + 1);
    if (data == NULL) {
        printf("Failed to allocate memory for text content\n");
        (void)fclose(txtFile);
        return EXIT_FAILURE;
    }

    if (fread(data, 1, txtFileSize, txtFile) != txtFileSize) {
        printf("Failed to read text file\n");
        free(data);
        (void)fclose(txtFile);
        return EXIT_FAILURE;
    }

    data[txtFileSize] = '\0';
    (void)fclose(txtFile);

    if ((rc = MQTTClient_create(&client, url, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) !=
        MQTTCLIENT_SUCCESS) {
        printf("Failed to create client, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto exit;
    }

    if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    conn_opts.keepAliveInterval = 10;  /* 10 seconds */
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    pubmsg.payload = data;
    pubmsg.payloadlen = (int)txtFileSize;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    if ((rc = MQTTClient_subscribe(client, TOPIC, QOS)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to subscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }
    printf("subscribe, return code %d\n", rc);
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n",
        TOPIC,
        CLIENTID,
        QOS);

    if ((rc = MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish message, return code %d\n", rc);
        goto destroy_exit;
    }

    if ((rc = MQTTClient_waitForCompletion(client, token, TIMEOUT)) != MQTTCLIENT_SUCCESS) {
        printf("Data waiting timed out, return code %d\n", rc);
        goto destroy_exit;
    }

    if ((rc = MQTTClient_unsubscribe(client, TOPIC)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to unsubscribe, return code %d\n", rc);
        goto destroy_exit;
    }

    if ((rc = MQTTClient_disconnect(client, TIMEOUT)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to disconnect, return code %d\n", rc);
        goto destroy_exit;
    }

destroy_exit:
    MQTTClient_destroy(&client);
exit:
    free(data);
    return rc;
}

static int mqtt_client_sendbin_test(char *url, size_t url_len, char *file_path, size_t file_path_len)
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    if (url == NULL || file_path == NULL || url_len == 0 || file_path_len == 0) {
        printf("URL or file_path is empty, please enter it\n");
        return -1;
    }

    FILE* imgFile = fopen(file_path, "rb");
    if (imgFile == NULL) {
        printf("Failed to open image file\n");
        return EXIT_FAILURE;
    }

    (void)fseek(imgFile, 0, SEEK_END);
    long tempSize = ftell(imgFile);
    if (tempSize < 0) {
        printf("File size overflow or error\n");
        (void)fclose(imgFile);
        return EXIT_FAILURE;
    }
    size_t imgFileSize = (size_t)tempSize;
    (void)fseek(imgFile, 0, SEEK_SET);

    unsigned char* imgContent = (unsigned char*)malloc(imgFileSize);
    if (imgContent == NULL) {
        printf("Failed to allocate memory for image content\n");
        (void)fclose(imgFile);
        return EXIT_FAILURE;
    }

    if (fread(imgContent, 1, imgFileSize, imgFile) != imgFileSize) {
        printf("Failed to read image file:%s\n", file_path);
        free(imgContent);
        (void)fclose(imgFile);
        return EXIT_FAILURE;
    }
    (void)fclose(imgFile);

    if ((rc = MQTTClient_create(&client, url, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) !=
        MQTTCLIENT_SUCCESS) {
        printf("Failed to create client, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto exit;
    }

    if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    conn_opts.keepAliveInterval = 10;  /* 10 seconds */
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    pubmsg.payload = (void *)imgContent;
    pubmsg.payloadlen = (int)imgFileSize;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    if ((rc = MQTTClient_subscribe(client, TOPIC, QOS)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to subscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }
    printf("subscribe, return code %d\n", rc);
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n",
        TOPIC,
        CLIENTID,
        QOS);

    if ((rc = MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish message, return code %d\n", rc);
        goto destroy_exit;
    }

    if ((rc = MQTTClient_waitForCompletion(client, token, TIMEOUT)) != MQTTCLIENT_SUCCESS) {
        printf("Data waiting timed out, return code %d\n", rc);
        goto destroy_exit;
    }

    if ((rc = MQTTClient_unsubscribe(client, TOPIC)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to unsubscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    if ((rc = MQTTClient_disconnect(client, TIMEOUT)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to disconnect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

destroy_exit:
    MQTTClient_destroy(&client);
exit:
    free(imgContent);
    return rc;
}

static int mqtt_secure_client_sendtext_test(char *url, size_t url_len, char *file_path, size_t file_path_len)
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
    if (url == NULL || file_path == NULL || url_len == 0 || file_path_len == 0) {
        printf("URL or file_path is empty, please enter it\n");
        return -1;
    }

    FILE* txtFile = fopen(file_path, "r");
    if (txtFile == NULL) {
        printf("failed to open text file\n");
        return EXIT_FAILURE;
    }

    (void)fseek(txtFile, 0, SEEK_END);
    long tempSize = ftell(txtFile);
    if (tempSize < 0) {
        printf("File size overflow or error\n");
        (void)fclose(txtFile);
        return EXIT_FAILURE;
    }
    size_t txtFileSize = (size_t)tempSize;
    (void)fseek(txtFile, 0, SEEK_SET);

    char* data = (char*)malloc(txtFileSize + 1);
    if (data == NULL) {
        printf("Failed to allocate memory for text content\n");
        (void)fclose(txtFile);
        return EXIT_FAILURE;
    }

    if (fread(data, 1, txtFileSize, txtFile) != txtFileSize) {
        printf("Failed to read text file\n");
        free(data);
        (void)fclose(txtFile);
        return EXIT_FAILURE;
    }

    data[txtFileSize] = '\0';
    (void)fclose(txtFile);

    if ((rc = MQTTClient_create(&client, url, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) !=
        MQTTCLIENT_SUCCESS) {
        printf("Failed to create client, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto exit;
    }

    conn_opts.ssl = &ssl_opts;

    if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    conn_opts.keepAliveInterval = 10;  /* 10 seconds */
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    pubmsg.payload = data;
    pubmsg.payloadlen = (int)txtFileSize;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    if ((rc = MQTTClient_subscribe(client, TOPIC, QOS)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to subscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }
    printf("subscribe, return code %d\n", rc);
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n",
        TOPIC,
        CLIENTID,
        QOS);

    if ((rc = MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish message, return code %d\n", rc);
        goto destroy_exit;
    }

    if ((rc = MQTTClient_waitForCompletion(client, token, TIMEOUT)) != MQTTCLIENT_SUCCESS) {
        printf("Data waiting timed out, return code %d\n", rc);
        goto destroy_exit;
    }

    if ((rc = MQTTClient_unsubscribe(client, TOPIC)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to unsubscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    if ((rc = MQTTClient_disconnect(client, TIMEOUT)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to disconnect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

destroy_exit:
    MQTTClient_destroy(&client);
exit:
    free(data);
    return rc;
}

static int mqtt_secure_client_sendbin_test(char *url, size_t url_len, char *file_path, size_t file_path_len)
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
    if (url == NULL || file_path == NULL || url_len == 0 || file_path_len == 0) {
        printf("URL or file_path is empty, please enter it\n");
        return -1;
    }

    FILE* imgFile = fopen(file_path, "rb");
    if (imgFile == NULL) {
        printf("Failed to open file\n");
        return EXIT_FAILURE;
    }

    (void)fseek(imgFile, 0, SEEK_END);
    long tempSize = ftell(imgFile);
    if (tempSize < 0) {
        printf("File size overflow or error\n");
        (void)fclose(imgFile);
        return EXIT_FAILURE;
    }
    size_t imgFileSize = (size_t)tempSize;
    (void)fseek(imgFile, 0, SEEK_SET);

    unsigned char* imgContent = (unsigned char*)malloc(imgFileSize);
    if (imgContent == NULL) {
        printf("Failed to allocate memory for image content\n");
        (void)fclose(imgFile);
        return EXIT_FAILURE;
    }

    if (fread(imgContent, 1, imgFileSize, imgFile) != imgFileSize) {
        printf("Failed to read image file\n");
        free(imgContent);
        (void)fclose(imgFile);
        return EXIT_FAILURE;
    }

    (void)fclose(imgFile);

    if ((rc = MQTTClient_create(&client, url, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) !=
        MQTTCLIENT_SUCCESS) {
        printf("Failed to create client, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto exit;
    }

    conn_opts.ssl = &ssl_opts;

    if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    conn_opts.keepAliveInterval = 10;  /* 10 seconds */
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    pubmsg.payload = (void *)imgContent;
    pubmsg.payloadlen = (int)imgFileSize;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    if ((rc = MQTTClient_subscribe(client, TOPIC, QOS)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to subscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }
    printf("subscribe, return code %d\n", rc);
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n",
        TOPIC,
        CLIENTID,
        QOS);

    if ((rc = MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to publish message, return code %d\n", rc);
        goto destroy_exit;
    }

    if ((rc = MQTTClient_waitForCompletion(client, token, TIMEOUT)) != MQTTCLIENT_SUCCESS) {
        printf("Data waiting timed out, return code %d\n", rc);
        goto destroy_exit;
    }

    if ((rc = MQTTClient_unsubscribe(client, TOPIC)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to unsubscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    if ((rc = MQTTClient_disconnect(client, TIMEOUT)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to disconnect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

destroy_exit:
    MQTTClient_destroy(&client);
exit:
    free(imgContent);
    return rc;
}

uint32_t NetATMqttSecureSendText(uint8_t argc, char *argv[])
{
    char *url = NULL;
    char *file_path = NULL;
    size_t url_len = 0;
    size_t file_path_len = 0;
    if (argc >= 1 && argv[0] != NULL) {
        url = strdup(argv[0]);
        if (url != NULL) {
            url_len = strlen(url);
        }
    }
    if (argc >= 2 && argv[1] != NULL) { /* 2 para num */
        file_path = strdup(argv[1]);
        if (file_path != NULL) {
            file_path_len = strlen(file_path);
        }
    }
    mqtt_secure_client_sendtext_test(url, url_len, file_path, file_path_len);

    if (url != NULL) {
        free(url);
    }
    if (file_path != NULL) {
        free(file_path);
    }
    return ERRCODE_SUCC;
}

uint32_t NetATMqttSecureSendBin(uint8_t argc, char *argv[])
{
    char *url = NULL;
    char *file_path = NULL;
    size_t url_len = 0;
    size_t file_path_len = 0;
    if (argc >= 1 && argv[0] != NULL) {
        url = strdup(argv[0]);
        if (url != NULL) {
            url_len = strlen(url);
        }
    }
    if (argc >= 2 && argv[1] != NULL) { /* 2 para num */
        file_path = strdup(argv[1]);
        if (file_path != NULL) {
            file_path_len = strlen(file_path);
        }
    }
    mqtt_secure_client_sendbin_test(url, url_len, file_path, file_path_len);

    if (url != NULL) {
        free(url);
    }
    if (file_path != NULL) {
        free(file_path);
    }
    return ERRCODE_SUCC;
}

uint32_t NetATMqttSendText(uint8_t argc, char *argv[])
{
    char *url = NULL;
    char *file_path = NULL;
    size_t url_len = 0;
    size_t file_path_len = 0;
    if (argc >= 1 && argv[0] != NULL) {
        url = strdup(argv[0]);
        if (url != NULL) {
            url_len = strlen(url);
        }
    }
    if (argc >= 2 && argv[1] != NULL) { /* 2 para num */
        file_path = strdup(argv[1]);
        if (file_path != NULL) {
            file_path_len = strlen(file_path);
        }
    }
    mqtt_client_sendtext_test(url, url_len, file_path, file_path_len);

    if (url != NULL) {
        free(url);
    }
    if (file_path != NULL) {
        free(file_path);
    }
    return ERRCODE_SUCC;
}

uint32_t NetATMqttSendBin(uint8_t argc, char *argv[])
{
    char *url = NULL;
    char *file_path = NULL;
    size_t url_len = 0;
    size_t file_path_len = 0;
    if (argc >= 1 && argv[0] != NULL) {
        url = strdup(argv[0]);
        if (url != NULL) {
            url_len = strlen(url);
        }
    }
    if (argc >= 2 && argv[1] != NULL) { /* 2 para num */
        file_path = strdup(argv[1]);
        if (file_path != NULL) {
            file_path_len = strlen(file_path);
        }
    }
    mqtt_client_sendbin_test(url, url_len, file_path, file_path_len);

    if (url != NULL) {
        free(url);
    }
    if (file_path != NULL) {
        free(file_path);
    }
    return ERRCODE_SUCC;
}
#endif

#ifdef AT_COMMAND_HTTP
static int http_request_test(char *url)
{
    HttpClient client = {0};
    HttpClientData client_data = {0};
    int ret;

    if (url == NULL) {
        printf("URL is empty, please enter it\n");
        return -1;
    }

    client_data.headerBuf = malloc(REQ_BUF_SIZE);
    if (client_data.headerBuf == NULL) {
        return -1;
    }
    client_data.headerBufLen = REQ_BUF_SIZE;

    client_data.responseBuf = malloc(RSP_BUF_SIZE);
    if (client_data.responseBuf == NULL) {
        free(client_data.headerBuf);
        return -1;
    }
    client_data.responseBufLen = RSP_BUF_SIZE;

    printf("http request: %s\r\n", url);

    ret = HttpClientGetRequest(&client, url, &client_data);
    if (ret >= 0 && client.responseCode == HTTP_STATUS_OK) {
        ret = 0;
        printf("http request test ok\n");
    } else {
        ret = -1;
        printf("http responsecode:%d\n", client.responseCode);
        printf("http request test fail\n");
    }
    free(client_data.headerBuf);
    free(client_data.responseBuf);
    return ret;
}

static int http_set_anything_header(char *url)
{
    HttpClient client = {0};
    HttpClientData client_data = {0};

    char* customer_header = strdup("Accept: */*\r\n");
    if (customer_header == NULL) {
        return -1;
    }
    int ret;
    if (url == NULL) {
        printf("URL is empty, please enter it\n");
        return -1;
    }

    client_data.headerBuf = malloc(REQ_BUF_SIZE);
    if (client_data.headerBuf == NULL) {
        return -1;
    }
    client_data.headerBufLen = REQ_BUF_SIZE;

    client_data.responseBuf = malloc(RSP_BUF_SIZE);
    if (client_data.responseBuf == NULL) {
        free(client_data.headerBuf);
        return -1;
    }
    client_data.responseBufLen = RSP_BUF_SIZE;

    printf("http request %s\r\n", url);

    HttpClientSetCustomHeader(&client, customer_header);
    ret = HttpClientGetRequest(&client, url, &client_data);
    if (ret >= 0 && client.responseCode == HTTP_STATUS_OK) {
        ret = 0;
        printf("http set anything header ok\n");
    } else {
        ret = -1;
        printf("http set anything header fail\n");
    }
    free(customer_header);
    free(client_data.headerBuf);
    free(client_data.responseBuf);
    return ret;
}

static int http_set_json_header(char *url)
{
    HttpClient client = {0};
    HttpClientData client_data = {0};

    char* customer_header = strdup("Accept: application/json\r\n");
    if (customer_header == NULL) {
        return -1;
    }
    int ret;
    if (url == NULL) {
        printf("URL is empty, please enter it\n");
        return -1;
    }

    client_data.headerBuf = malloc(REQ_BUF_SIZE);
    if (client_data.headerBuf == NULL) {
        return -1;
    }
    client_data.headerBufLen = REQ_BUF_SIZE;

    client_data.responseBuf = malloc(RSP_BUF_SIZE);
    if (client_data.responseBuf == NULL) {
        free(client_data.headerBuf);
        return -1;
    }
    client_data.responseBufLen = RSP_BUF_SIZE;

    printf("http request %s\r\n", url);

    HttpClientSetCustomHeader(&client, customer_header);
    ret = HttpClientGetRequest(&client, url, &client_data);
    if (ret >= 0 && client.responseCode == HTTP_STATUS_OK) {
        ret = 0;
        printf("http set json header ok\n");
    } else {
        ret = -1;
        printf("http set json header fail\n");
    }
    free(customer_header);
    free(client_data.headerBuf);
    free(client_data.responseBuf);
    return ret;
}

static int http_set_text_header(char *url)
{
    HttpClient client = {0};
    HttpClientData client_data = {0};

    char* customer_header = strdup("Accept: text/html\r\n");
    if (customer_header == NULL) {
        return -1;
    }
    int ret;
    if (url == NULL) {
        printf("URL is empty, please enter it\n");
        return -1;
    }

    client_data.headerBuf = malloc(REQ_BUF_SIZE);
    if (client_data.headerBuf == NULL) {
        return -1;
    }
    client_data.headerBufLen = REQ_BUF_SIZE;

    client_data.responseBuf = malloc(RSP_BUF_SIZE);
    if (client_data.responseBuf == NULL) {
        free(client_data.headerBuf);
        return -1;
    }
    client_data.responseBufLen = RSP_BUF_SIZE;

    printf("http request %s\r\n", url);

    HttpClientSetCustomHeader(&client, customer_header);
    ret = HttpClientGetRequest(&client, url, &client_data);
    if (ret >= 0 && client.responseCode == HTTP_STATUS_OK) {
        ret = 0;
        printf("http set text header ok\n");
    } else {
        ret = -1;
        printf("http set text header fail\n");
    }
    free(customer_header);
    free(client_data.headerBuf);
    free(client_data.responseBuf);
    return ret;
}

static int http_post_test(char *url, size_t url_len, char *postData, size_t postData_len)
{
    HttpClient client = {0};
    HttpClientData client_data = {0};
    int ret;

    if (url == NULL || url_len == 0 || postData == NULL || postData_len == 0) {
        printf("URL or message is empty, please enter it\n");
        return -1;
    }
    
    client_data.postBuf = strdup(postData);
    if (client_data.postBuf == NULL) {
        return -1;
    }
    client_data.postBufLen = postData_len;

    client_data.headerBuf = malloc(REQ_BUF_SIZE);
    if (client_data.headerBuf == NULL) {
        return -1;
    }
    client_data.headerBufLen = REQ_BUF_SIZE;

    client_data.responseBuf = malloc(RSP_BUF_SIZE);
    if (client_data.responseBuf == NULL) {
        free(client_data.headerBuf);
        return -1;
    }
    client_data.responseBufLen = RSP_BUF_SIZE;

    printf("http request %s\r\n", url);

    ret = HttpClientPostRequest(&client, url, &client_data);
    if (ret >= 0 && client.responseCode == HTTP_STATUS_OK) {
        ret = 0;
        printf("responsecode:%d\n", client.responseCode);
        printf("http post test ok\n");
    } else {
        ret = -1;
        printf("responsecode:%d\n", client.responseCode);
        printf("http post test fail\n");
    }
    free(client_data.postBuf);
    free(client_data.headerBuf);
    free(client_data.responseBuf);
    return ret;
}

static int http_put_test(char *url)
{
    HttpClient client = {0};
    HttpClientData client_data = {0};

    client_data.postBuf = strdup("{ \"name\": \"www\", \"age\": 30 }");
    if (client_data.postBuf == NULL) {
        return -1;
    }
    size_t length = strlen(client_data.postBuf);
    client_data.postBufLen = length;
    int ret;
    if (url == NULL) {
        printf("URL is empty, please enter it\n");
        return -1;
    }

    client_data.headerBuf = malloc(REQ_BUF_SIZE);
    if (client_data.headerBuf == NULL) {
        return -1;
    }
    client_data.headerBufLen = REQ_BUF_SIZE;

    client_data.responseBuf = malloc(RSP_BUF_SIZE);
    if (client_data.responseBuf == NULL) {
        free(client_data.headerBuf);
        return -1;
    }
    client_data.responseBufLen = RSP_BUF_SIZE;

    printf("http request %s\r\n", url);

    ret = HttpClientPutRequest(&client, url, &client_data);
    if (ret >= 0 && client.responseCode == HTTP_STATUS_OK) {
        ret = 0;
        printf("http put test ok\n");
    } else {
        ret = -1;
        printf("http put test fail\n");
    }
    free(client_data.postBuf);
    free(client_data.headerBuf);
    free(client_data.responseBuf);
    return ret;
}

static int http_delete_test(char *url)
{
    HttpClient client = {0};
    HttpClientData client_data = {0};

    int ret;
    if (url == NULL) {
        printf("URL is empty, please enter it\n");
        return -1;
    }

    client_data.headerBuf = malloc(REQ_BUF_SIZE);
    if (client_data.headerBuf == NULL) {
        return -1;
    }
    client_data.headerBufLen = REQ_BUF_SIZE;

    client_data.responseBuf = malloc(RSP_BUF_SIZE);
    if (client_data.responseBuf == NULL) {
        free(client_data.headerBuf);
        return -1;
    }
    client_data.responseBufLen = RSP_BUF_SIZE;

    printf("http request %s\r\n", url);

    ret = HttpClientDelete(&client, url, &client_data);
    if (ret >= 0 && client.responseCode == HTTP_STATUS_OK) {
        ret = 0;
        printf("http delete test ok\n");
    } else {
        ret = -1;
        printf("http delete test fail\n");
    }
    free(client_data.headerBuf);
    free(client_data.responseBuf);
    return ret;
}

uint32_t NetATHttpRequest(uint8_t argc, char *argv[])
{
    int ret;
    if (argc > 0) {
        ret = http_request_test((char *)argv[0]);
    } else {
        ret = http_request_test(NULL);
    }
    if (ret != 0) {
        printf("\n http test fail\n");
    } else {
        printf("\n http test ok\n");
    }
    return ERRCODE_SUCC;
}

uint32_t NetATHttpSetAnythingHeader(uint8_t argc, char *argv[])
{
    int ret;
    if (argc > 0) {
        ret = http_set_anything_header((char *)argv[0]);
    } else {
        ret = http_set_anything_header(NULL);
    }
    if (ret != 0) {
        printf("\n http set anything header test fail\n");
    } else {
        printf("\n http set anything header test ok\n");
    }
    return ERRCODE_SUCC;
}

uint32_t NetATHttpSetJsonHeader(uint8_t argc, char *argv[])
{
    int ret;
    if (argc > 0) {
        ret = http_set_json_header((char *)argv[0]);
    } else {
        ret = http_set_json_header(NULL);
    }
    if (ret != 0) {
        printf("\n http set json header test fail\n");
    } else {
        printf("\n http set json header test ok\n");
    }
    return ERRCODE_SUCC;
}

uint32_t NetATHttpSetTextHeader(uint8_t argc, char *argv[])
{
    int ret;
    if (argc > 0) {
        ret = http_set_text_header((char *)argv[0]);
    } else {
        ret = http_set_text_header(NULL);
    }
    if (ret != 0) {
        printf("\n http set text header test fail\n");
    } else {
        printf("\n http set text header test ok\n");
    }
    return ERRCODE_SUCC;
}

uint32_t NetATHttpPost(uint8_t argc, char *argv[])
{
    int ret;

    if (argc < 2) { // 2: param number
        printf("url or postbuf is empty, please enter it\n");
        return ERRCODE_FAIL;
    }
        
    ret = http_post_test(argv[0], strlen(argv[0]), argv[1], strlen(argv[1]));
    if (ret != 0) {
        printf("\n http post test fail\n");
    } else {
        printf("\n http post test ok\n");
    }

    return ERRCODE_SUCC;
}

uint32_t NetATHttpPut(uint8_t argc, char *argv[])
{
    int ret;
    if (argc > 0) {
        ret = http_put_test((char *)argv[0]);
    } else {
        ret = http_put_test(NULL);
    }
    if (ret != 0) {
        printf("\n http put test fail\n");
    } else {
        printf("\n http put test ok\n");
    }
    return ERRCODE_SUCC;
}

uint32_t NetATHttpDelete(uint8_t argc, char *argv[])
{
    int ret;
    if (argc > 0) {
        ret = http_delete_test((char *)argv[0]);
    } else {
        ret = http_delete_test(NULL);
    }
    if (ret != 0) {
        printf("\n http delete test fail\n");
    } else {
        printf("\n http delete test ok\n");
    }
    return ERRCODE_SUCC;
}
#endif

#ifdef AT_COMMAND_WEBSOCKET
static int ws_client_sendtext_test(char *url, size_t url_len, char *file_path, size_t file_path_len)
{
    int clientFd;
    int ret;
    g_test_open = false;
    g_test_end= false;
    if (url == NULL || file_path == NULL || url_len == 0 || file_path_len == 0) {
        printf("URL or file_path is empty, please enter it\n");
        return -1;
    }
    printf("url=%s, file_path=%s\n", url, file_path);

    clientFd = WebSocketClientConstructor(OnOpenWs, OnMessageWs, OnErrorWs, OnCloseWs);
    if (clientFd < 0) {
        printf("client fd fail=%d\n", clientFd);
        return -1;
    }

    ret = WebSocketClientConnect(clientFd, url, NULL);
    if (ret != 0) {
        printf("connect fail, ret=0x%x\n", ret);
        return ret;
    }

    FILE* txtFile = fopen(file_path, "r");
    if (txtFile == NULL) {
        printf("failed to open text file\n");
        return EXIT_FAILURE;
    }

    (void)fseek(txtFile, 0, SEEK_END);
    long tempSize = ftell(txtFile);
    if (tempSize < 0) {
        printf("File size overflow or error\n");
        (void)fclose(txtFile);
        return EXIT_FAILURE;
    }
    size_t txtFileSize = (size_t)tempSize;
    (void)fseek(txtFile, 0, SEEK_SET);

    char *data = (char*)malloc(txtFileSize + 1);
    if (data == NULL) {
        printf("Failed to allocate memory for text content\n");
        (void)fclose(txtFile);
        return EXIT_FAILURE;
    }

    if (fread(data, 1, txtFileSize, txtFile) != txtFileSize) {
        printf("Failed to read text file\n");
        free(data);
        (void)fclose(txtFile);
        return EXIT_FAILURE;
    }

    data[txtFileSize] = '\0';
    (void)fclose(txtFile);

    while (!g_test_open) {
        sleep(1); /* 1 seconds */
        if (g_test_end) {
            goto END;
        }
    }

    ret = WebSocketClientSendText(clientFd, data, (size_t)txtFileSize);
    if (ret != 0) {
        printf("WebSocketClientSendText fail, ret=0x%x\n", ret);
    }
    printf("send finish\r\n");

    sleep(5); /* 5 seconds */
END:
    ret = WebSocketClientClose(clientFd);
    if (ret != 0) {
        printf("close fail, ret=0x%x\n", ret);
    } else {
        printf("close ok\n");
    }
    free(data);
    return 0;
}

static int ws_client_sendbin_test(char *url, size_t url_len, char *file_path, size_t file_path_len)
{
    int clientFd;
    int ret;
    g_test_open = false;
    g_test_end = false;
    if (url == NULL || file_path == NULL || url_len == 0 || file_path_len == 0) {
        printf("URL or file_path is empty, please enter it\n");
        return -1;
    }
    printf("url=%s, file_path=%s\n", url, file_path);

    clientFd = WebSocketClientConstructor(OnOpenWs, OnMessageWs, OnErrorWs, OnCloseWs);
    if (clientFd < 0) {
        printf("client fd fail=%d\n", clientFd);
        return -1;
    }

    ret = WebSocketClientConnect(clientFd, url, NULL);
    if (ret != 0) {
        printf("connect fail, ret=0x%x\n", ret);
        return ret;
    }

    FILE* imgFile = fopen(file_path, "rb");
    if (imgFile == NULL) {
        printf("Failed to open image file\n");
        return EXIT_FAILURE;
    }

    (void)fseek(imgFile, 0, SEEK_END);
    long tempSize = ftell(imgFile);
    if (tempSize < 0) {
        printf("File size overflow or error\n");
        (void)fclose(imgFile);
        return EXIT_FAILURE;
    }
    size_t imgFileSize = (size_t)tempSize;
    (void)fseek(imgFile, 0, SEEK_SET);

    char* imgContent = (char*)malloc(imgFileSize);
    if (imgContent == NULL) {
        printf("Failed to allocate memory for image content\n");
        (void)fclose(imgFile);
        return EXIT_FAILURE;
    }

    if (fread(imgContent, 1, imgFileSize, imgFile) != imgFileSize) {
        printf("Failed to read image file\n");
        free(imgContent);
        (void)fclose(imgFile);
        return EXIT_FAILURE;
    }

    (void)fclose(imgFile);
    size_t length = (size_t)imgFileSize;

    while (!g_test_open) {
        sleep(1); /* 1 seconds */
        if (g_test_end) {
            goto END;
        }
    }

    ret = WebSocketClientSendBinStart(clientFd, imgContent, IMG_HEADER_SIZE);
    if (ret != 0) {
        printf("send header data fail, ret=%d\n", ret);
        free(imgContent);
        goto END;
    }

    ret = WebSocketClientSendBinContinue(clientFd, imgContent + IMG_HEADER_SIZE,
                                         length - IMG_HEADER_SIZE - IMG_FOOTER_SIZE);
    if (ret != 0) {
        printf("send content data failed! ret=%d\n", ret);
        free(imgContent);
        goto END;
    }

    ret = WebSocketClientSendBinEnd(clientFd, imgContent + length - IMG_FOOTER_SIZE, IMG_FOOTER_SIZE);
    if (ret != 0) {
        printf("send final data failed! ret=%d\n", ret);
        free(imgContent);
        goto END;
    }

    sleep(5); /* 5 seconds */
END:
    ret = WebSocketClientClose(clientFd);
    if (ret != 0) {
        printf("close fail, ret=0x%x\n", ret);
    } else {
        printf("close ok\n");
    }
    free(imgContent);
    return 0;
}

uint32_t NetATWebsocketSendText(uint8_t argc, char *argv[])
{
    char *url = NULL;
    char *file_path = NULL;
    size_t url_len = 0;
    size_t file_path_len = 0;
    if (argc >= 1 && argv[0] != NULL) {
        url = strdup(argv[0]);
        if (url != NULL) {
            url_len = strlen(url);
        }
    }
    if (argc >= 2 && argv[1] != NULL) { /* 2 para num */
        file_path = strdup(argv[1]);
        if (file_path != NULL) {
            file_path_len = strlen(file_path);
        }
    }
    ws_client_sendtext_test(url, url_len, file_path, file_path_len);
    if (url != NULL) {
        free(url);
    }
    if (file_path != NULL) {
        free(file_path);
    }
    return ERRCODE_SUCC;
}

uint32_t NetATWebsocketSendBin(uint8_t argc, char *argv[])
{
    char *url = NULL;
    char *file_path = NULL;
    size_t url_len = 0;
    size_t file_path_len = 0;
    if (argc >= 1 && argv[0] != NULL) {
        url = strdup(argv[0]);
        if (url != NULL) {
            url_len = strlen(url);
        }
    }
    if (argc >= 2 && argv[1] != NULL) { /* 2 para num */
        file_path = strdup(argv[1]);
        if (file_path != NULL) {
            file_path_len = strlen(file_path);
        }
    }
    ws_client_sendbin_test(url, url_len, file_path, file_path_len);
    if (url != NULL) {
        free(url);
    }
    if (file_path != NULL) {
        free(file_path);
    }
    return ERRCODE_SUCC;
}
#endif

void OnNetConnectStateChanged(const uint8_t networkType, const uint8_t netConnectState)
{
    printf("network type:%d state changed to %d\n", networkType, netConnectState);
}

int RegisterConnectState(uint8_t networkType, NetConnectionCallback callback)
{
    int ret;
    if (callback == NULL) {
        printf("Invalid callback\n");
        return -1;
    }

    if (networkType != NETWORK_TYPE_BLUETOOTH && networkType != NETWORK_TYPE_BLUETOOTH_LE
        && networkType != NETWORK_TYPE_BLUETOOTH_PAN && networkType != NETWORK_TYPE_CELL_LTE_CAT1) {
        printf("Not supported networktype\n");
        return -1;
    }
    ret = RegisterConnectListener(networkType, callback);
    if (ret != 0) {
        printf("Failed to register networktype:%d connection listener\n", networkType);
    }
    printf("Registered listener for network type %d\n", networkType);
    return ret;
}

int UnRegisterConnectState(uint8_t networkType, NetConnectionCallback callback)
{
    if (callback == NULL) {
        printf("Invalid callback\n");
        return -1;
    }

    if (networkType != NETWORK_TYPE_BLUETOOTH && networkType != NETWORK_TYPE_BLUETOOTH_LE
        && networkType != NETWORK_TYPE_BLUETOOTH_PAN && networkType != NETWORK_TYPE_CELL_LTE_CAT1) {
        printf("Not supported networktype\n");
        return -1;
    }
    UnregisterConnectListener(networkType, callback);
    printf("Unregistered listener for network type %d\n", networkType);
    return 0;
}

void GetNetConnectStateTest(uint8_t networkType)
{
    uint8_t state = GetNetConnectState(networkType);
    printf("Network type: %d, Connection state: %d\n", networkType, state);
}

uint32_t NetATGetConnectState(uint8_t argc, char *argv[])
{
    uint8_t networktype = NETWORK_TYPE_UNKNOWN;
    int ret;

    if (argc > 0) {
        char *endptr;
        networktype = (uint8_t)strtoul(argv[0], &endptr, BASE_DECIMAL);
        if (*endptr != '\0') {
            printf("Invalid network type parameter\n");
            return ERRCODE_FAIL;
        }
    }

    GetNetConnectStateTest(networktype);
    return ERRCODE_SUCC;
}

uint32_t NetATRegConnect(uint8_t argc, char *argv[])
{
    NetConnectionCallback callback = OnNetConnectStateChanged;
    uint8_t networktype = NETWORK_TYPE_UNKNOWN;
    int ret;

    if (argc > 0) {
        char *endptr;
        networktype = (uint8_t)strtoul(argv[0], &endptr, BASE_DECIMAL);
        if (*endptr != '\0') {
            printf("Invalid network type parameter\n");
            return ERRCODE_FAIL;
        }
    }

    ret = RegisterConnectState(networktype, callback);
    if (ret != 0) {
        printf("Register Connect State fail\n");
        return ERRCODE_FAIL;
    } else {
        printf("Register Connect State success\n");
        return ERRCODE_SUCC;
    }
}

uint32_t NetATUnRegConnect(uint8_t argc, char *argv[])
{
    NetConnectionCallback callback = OnNetConnectStateChanged;
    uint8_t networktype = NETWORK_TYPE_UNKNOWN;
    int ret;

    if (argc > 0) {
        char *endptr;
        networktype = (uint8_t)strtoul(argv[0], &endptr, BASE_DECIMAL);
        if (*endptr != '\0') {
            printf("Invalid network type parameter\n");
            return ERRCODE_FAIL;
        }
    }
    ret = UnRegisterConnectState(networktype, callback);
    if (ret != 0) {
        printf("UnRegister Connect State fail\n");
        return ERRCODE_FAIL;
    } else {
        printf("UnRegister Connect State success\n");
        return ERRCODE_SUCC;
    }
}
#endif

#define MULTICAST_PORT 12345
#define RECV_TIMEOUT   10

uint32_t NetATSendMultiCast(uint8_t argc, char *argv[])
{
    int ret;
    int sock;
    struct sockaddr_in multicast_addr;
    struct in_addr iface_ip;
    int ttl_value = 1;
    char *message = "Hello Multicast!";

    if (argc < 2) { // 2 : param number
        printf("you need input local interface IP and multicast address\n");
        return ERRCODE_FAIL;
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("create socket failed\n");
        return ERRCODE_FAIL;
    }

    inet_aton(argv[0], &iface_ip);
    ret = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &iface_ip, sizeof(iface_ip));
    if (ret < 0) {
        printf("set IP_MULTICAST_IF failed\n");
        goto EXIT;
    }

    ret = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl_value, sizeof(ttl_value));
    if (ret < 0) {
        printf("set IP_MULTICAST_TTL failed\n");
        goto EXIT;
    }

    (void)memset_s(&multicast_addr, sizeof(multicast_addr), 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(MULTICAST_PORT);
    multicast_addr.sin_addr.s_addr = inet_addr(argv[1]);
    
    ret = sendto(sock, message, strlen(message), 0, (struct sockaddr*)&multicast_addr, sizeof(multicast_addr));
    if (ret < 0) {
        printf("send multicast failed\n");
        goto EXIT;
    }
    
    printf("send multicast success\n");

EXIT:
    close(sock);
    return ERRCODE_SUCC;
}

uint32_t NetATRecvMultiCast(uint8_t argc, char *argv[])
{
    struct sockaddr_in local_addr;
    struct ip_mreq mreq;
    char buffer[128];

    if (argc < 2) { // 2 : param number
        printf("you need input local interface IP and multicast address\n");
        return ERRCODE_FAIL;
    }

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("create socket failed\n");
        return ERRCODE_FAIL;
    }

    struct timeval tv = {RECV_TIMEOUT, 0};
    int ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (ret < 0) {
        printf("set SO_RCVTIMEO failed\n");
        goto EXIT;
    }

    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    (void)memset_s(&local_addr, sizeof(local_addr), 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(MULTICAST_PORT);
    ret = bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr));
    if (ret < 0) {
        printf("bind to local_addr failed\n");
        goto EXIT;
    }

    mreq.imr_interface.s_addr = inet_addr(argv[0]);
    mreq.imr_multiaddr.s_addr = inet_addr(argv[1]);
    setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if (ret < 0) {
        printf("set IP_ADD_MEMBERSHIP failed\n");
        goto EXIT;
    }

    int len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, NULL, 0);
    if (len > 0) {
        buffer[len] = '\0';
        printf("Recv: %s\n", buffer);
    } else {
        printf("recvfrom failed\n");
    }

EXIT:
    setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    close(sock);
    return ERRCODE_SUCC;
}