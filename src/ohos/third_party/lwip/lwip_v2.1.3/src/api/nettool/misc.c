/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: DNS and UDP\TCP API implementation
 * Create:2026
 */

#include "lwip/nettool/misc.h"
#include <stdlib.h>
#include "lwip/nettool/utility.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"

#define MISC_PRINT(fmt, args...) printf(fmt, ##args)
#define MISC_OK 0
#define MISC_FAIL (-1)
#define PRINT_CHUNK_SIZE 128

static void print_data_chunk(const char *data, size_t len)
{
    size_t i;
    for(i = 0; i < len; i += PRINT_CHUNK_SIZE) {
        size_t chunk = (len - i > PRINT_CHUNK_SIZE) ? PRINT_CHUNK_SIZE : (len - i);
        MISC_PRINT("%.*s", (int)chunk, data + i);
    }
    MISC_PRINT("\r\n");
}

#if LWIP_DNS
uint32_t os_shell_show_dns(void)
{
    ip_addr_t dns;
    err_t err;
    int i;
    uint32_t ret = MISC_FAIL;

    MISC_PRINT("+DNS:\r\n");

    for (i = 0; i < DNS_MAX_SERVERS; i++) {
        err = lwip_dns_getserver((u8_t)i, &dns);
        if (err == ERR_OK) {
            char buf[IPADDR_STRLEN_MAX];
            (void)ipaddr_ntoa_r(&dns, buf, IPADDR_STRLEN_MAX);
            MISC_PRINT("%s\r\n", buf);
            ret = MISC_OK;
        } else {
            MISC_PRINT("Failed to get DNS info for index %d\r\n", i);
        }
    }
    return ret;
}

static void dns_help_usage(void)
{
    MISC_PRINT("DNS help      - show help message\r\n");
    MISC_PRINT("Index: 1-%d (DNS server position)\r\n", DNS_MAX_SERVERS);
    MISC_PRINT("IP:    IPv4 or IPv6 address\r\n");
    MISC_PRINT("Example: 1,192.168.1.10\r\n");
}

uint32_t os_shell_dns(int argc, const char **argv)
{
    ip_addr_t dns;
    err_t err;
    int i;

    if (argv == NULL) {
        dns_help_usage();
        return MISC_FAIL;
    }

    // help参数
    if ((argc == 1) && (strcmp(argv[0], "help") == 0)) {
        dns_help_usage();
        return MISC_OK;
    }

    // 验证第一个参数是否为数字
    if (lwip_isdigitstr((s8_t *)argv[0]) != 0) {
        MISC_PRINT("dns error parameter:%s\r\n", argv[0]);
        dns_help_usage();
        return MISC_FAIL;
    }

    if (argc == 2) { /* 2 : current argc number */
        char *endptr = NULL;
        i = (int)strtol(argv[0], &endptr, 10); /* 10：decimal notation */
        if (endptr == argv[0]) {
            MISC_PRINT("Invalid number\r\n");
            return MISC_FAIL;
        }

        if ((i <= 0) || (i > DNS_MAX_SERVERS)) {
            dns_help_usage();
            return MISC_FAIL;
        }

    // 检测ipv6地址
    if (strchr(argv[1], ':') != NULL) {
        MISC_PRINT("IPv6 not supported\r\n");
        dns_help_usage();
        return MISC_FAIL;
    }

#if LWIP_IPV6
        if (ip6addr_aton(argv[1], ((ip6_addr_t *)&dns))) {
#if LWIP_IPV4 && LWIP_IPV6
            dns.type = IPADDR_TYPE_V6;
#endif
#ifdef LWIP_DNS_GLOBAL_ADDR
            if (!ip6_addr_isglobal((ip6_addr_t *)&dns)) {
                MISC_PRINT("ip address<%s> is wrong\r\n", argv[1]);
                dns_help_usage();
                return MISC_FAIL;
            }
#endif
        } else
#endif
        {
#if LWIP_IPV4
            ((ip4_addr_t *)&dns)->addr = ipaddr_addr(argv[1]);
            if (((ip4_addr_t *)&dns)->addr == IPADDR_NONE) {
                MISC_PRINT("ip address<%s> is wrong\r\n", argv[1]);
                dns_help_usage();
                return MISC_FAIL;
            }
#if LWIP_IPV4 && LWIP_IPV6
            dns.type = IPADDR_TYPE_V4;
#endif
#endif
        }

        err = lwip_dns_setserver((u8_t)(i - 1), &dns);
        if (err != ERR_OK) {
            MISC_PRINT("dns set failed\r\n");
            dns_help_usage();
            return MISC_FAIL;
        }

        return MISC_OK;
    } else {
        MISC_PRINT("input parameter error\r\n");
        dns_help_usage();
        return MISC_FAIL;
    }
}
#endif /* LWIP_DNS */

static void server_help_usage(void)
{
    MISC_PRINT("Usage: server <port_number>\r\n");
    MISC_PRINT("Example: server 8080\r\n");
}

static void client_help_usage(void)
{
    MISC_PRINT("Usage:input <server_ip> <server_port> <send_data>\r\n");
    MISC_PRINT("Example:192.168.1.10,8080,HELLO\r\n");
}

#ifdef LWIP_DEBUG_TCPSERVER
#define MAX_SIZE 1024
static void tcp_access(int sockfd)
{
    size_t n, i;
    ssize_t ret;
    char *msg = (char *)malloc(MAX_SIZE);
    if (msg == NULL) {
        MISC_PRINT("memory allocation fail\r\n");
        (void)lwip_close(sockfd);
        return;
    }

    while (1) {
        MISC_PRINT("waiting for receive data\r\n");
        (void)memset_s(msg, MAX_SIZE, 0, MAX_SIZE);
        ret = lwip_recv(sockfd, msg, MAX_SIZE - 1, 0);
        if (ret < 0) {
            goto END;
        } else if (ret == 0) {
            MISC_PRINT("client disconnect.\r\n");
            goto END;
        }

        msg[ret] = '\0';
        MISC_PRINT("Received from client %d bytes\r\n", ret);
        print_data_chunk(msg, ret);

        n = ret;
        for (i = 0; i < n; ++i) {
            if ((msg[i] >= 'a') && (msg[i] <= 'z')) {
                msg[i] = (char)(msg[i] + ('A' - 'a'));
            } else if ((msg[i] >= 'A') && (msg[i] <= 'Z')) {
                msg[i] = (char)(msg[i] + ('a' - 'A'));
            }
        }

        if (lwip_send(sockfd, msg, n, 0) < 0) {
            MISC_PRINT("send data to client failed!\r\n");
            continue;
        }
        MISC_PRINT("send %d bytes data to client\r\n", n);
        print_data_chunk(msg, n);
    }

END:
    (void)lwip_close(sockfd);
    free(msg);
}

uint32_t os_tcpserver(int argc, const char **argv)
{
    uint16_t port;
    int sockfd = -1;
    int ret;
    struct sockaddr_in seraddr;
    struct sockaddr_in cliaddr;
    uint32_t cliaddr_size = (uint32_t)sizeof(cliaddr);
    int reuse;

    if (argc != 1) { /* 1: parameters Number */
        MISC_PRINT("parameter input error\r\n");
        server_help_usage();
        return MISC_FAIL;
    }

    // help参数
    if (strcmp(argv[0], "help") == 0) { /* 0: parameter index */
        server_help_usage();
        return MISC_OK;
    }

    char *endptr = NULL;
    long int server_port = strtol(argv[0], &endptr, 10); /* 10：decimal notation */
    if (endptr == argv[0]) {
        MISC_PRINT("No digits were found\r\n");
        return MISC_FAIL;
    }

    /* Port 0 not supported , negative values not supported , max port limit is 65535 */
    if (server_port <= 0 || server_port > 65535) {
        MISC_PRINT("Invalid port\r\n");
        server_help_usage();
        return MISC_FAIL;
    }

    port = (uint16_t)server_port;

    sockfd = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    reuse = 1;
    if (lwip_setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) != 0) {
        MISC_PRINT("set SO_REUSEADDR failed\r\n");
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    }

    (void)memset_s(&seraddr, sizeof(seraddr), 0, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    seraddr.sin_port = htons(port);

    ret = lwip_bind(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr));
    if (ret < 0) {
        MISC_PRINT("bind ip and port failed ret = %d", ret);
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    } else {
        MISC_PRINT(
            "Server successfully bound to IP: %s and port: %d\n", inet_ntoa(seraddr.sin_addr), ntohs(seraddr.sin_port));
    }

    ret = lwip_listen(sockfd, 5); /* 5 : max tcp listen backlog queue */
    if (ret < 0) {
        MISC_PRINT("listen failed ret = %d\r\n", ret);
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    } else {
        MISC_PRINT("Starts listening to data\r\n");
    }
    while (1) {
        MISC_PRINT("waiting for accept\r\n");
        (void)memset_s(&cliaddr, sizeof(struct sockaddr_in), 0, sizeof(struct sockaddr_in));
        ret = lwip_accept(sockfd, (struct sockaddr *)&cliaddr, &cliaddr_size);
        if (ret < 0) {
            MISC_PRINT("Accept failed ret = %d\r\n", ret);
            (void)lwip_close(sockfd);
            return MISC_FAIL;
        }

        tcp_access(ret);
    }
    return MISC_OK;  // Hits Only If Accept Fails
}

uint32_t os_tcpclient(int argc, const char **argv)
{
    // help参数
    if (argc == 1 && strcmp(argv[0], "help") == 0) { /* 0: parameter index  1: parameter number */
        client_help_usage();
        return MISC_OK;
    }
    if (argc != 3) { /* 3: parameters Number */
        MISC_PRINT("parameter input is incorrect.\r\n");
        client_help_usage();
        return MISC_FAIL;
    }

    const char *server_ip = argv[0]; /* 0: parameter index */
    const char *send_data = argv[2]; /* 2: parameter index */
    size_t send_data_len = strlen(send_data);
    if (send_data_len >= MAX_SIZE) {
        MISC_PRINT("The data length exceeds the maximum(1023 bytes) limit.\r\n");
        return MISC_FAIL;
    }

    char *endptr = NULL;
    long int server_port = strtol(argv[1], &endptr, 10); /* 10：decimal notation */
    if (endptr == argv[1]) {
        MISC_PRINT("No digits were found\r\n");
        return MISC_FAIL;
    }

    /* Port 0 not supported , negative values not supported , max port limit is 65535 */
    if ((server_port <= 0) || (server_port > 65535)) {
        MISC_PRINT("\rInvalid Port\r\n");
        client_help_usage();
        return MISC_FAIL;
    }

    int sockfd = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        MISC_PRINT("socket creation failed\r\n");
        return MISC_FAIL;
    }

    struct sockaddr_in server_addr;
    memset_s(&server_addr, sizeof(struct sockaddr_in), 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (lwip_inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        MISC_PRINT("Invalid server IP address format\r\n");
        return MISC_FAIL;
    }

    // 连接服务器
    if (lwip_connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        MISC_PRINT("connect failed\r\n");
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    }
    MISC_PRINT("connected to server ip: %s port:%d\r\n", server_ip, server_port);

    // 发送数据
    int sent_bytes = lwip_send(sockfd, send_data, strlen(send_data), 0);
    if (sent_bytes < 0) {
        MISC_PRINT("send data failed\r\n");
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    } else {
        MISC_PRINT("send %d bytes success\r\n", sent_bytes);
        print_data_chunk(send_data, strlen(send_data));
    }

    // 接收服务器回显数据
    char *recv_buffer = (char *)malloc(MAX_SIZE);
    if (recv_buffer == NULL) {
        MISC_PRINT("memory allocation failed\r\n");
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    }

    int recv_bytes = lwip_recv(sockfd, recv_buffer, MAX_SIZE - 1, 0);
    if (recv_bytes < 0) {
        MISC_PRINT("receive from server data failed\r\n");
        free(recv_buffer);
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    } else if (recv_bytes == 0) {
        MISC_PRINT("server closed\r\n");
        free(recv_buffer);
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    } else {
        recv_buffer[recv_bytes] = '\0';
        MISC_PRINT("recv from server %d bytes\r\n", recv_bytes);
        print_data_chunk(recv_buffer, recv_bytes);
    }

    free(recv_buffer);
    (void)lwip_close(sockfd);

    return MISC_OK;
}
#endif /* LWIP_DEBUG_TCPSERVER */

#ifdef LWIP_DEBUG_UDPSERVER
uint32_t udpserver(int argc, const char **argv)
{
    int sockfd = -1;
    int ret, fromlen;
    struct sockaddr_in seraddr;
    struct sockaddr_in cliaddr;
    size_t n, i;
    char *msg = NULL;
    uint16_t port;

    if (argv == NULL) {
        server_help_usage();
        return MISC_FAIL;
    }
    if (argc != 1) {
        MISC_PRINT("\rparameter input error\r\n");
        server_help_usage();
        return MISC_FAIL;
    }

    // help参数
    if (strcmp(argv[0], "help") == 0) {
        server_help_usage();
        return MISC_OK;
    }

    char *endptr = NULL;
    long int server_port = strtol(argv[0], &endptr, 10); /* 10：decimal notation */
    if (endptr == argv[0]) {
        MISC_PRINT("No digits were found\r\n");
        return MISC_FAIL;
    }

    /* Port 0 not supported , negative values not supported , max port limit is 65535 */
    if ((server_port <= 0) || (server_port > 65535)) {
        MISC_PRINT("\rInvalid Port\r\n");
        server_help_usage();
        return MISC_FAIL;
    }

    port = (uint16_t)server_port;
    MISC_PRINT("port:%d\r\n", port);

    sockfd = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    (void)memset_s(&seraddr, sizeof(seraddr), 0, sizeof(seraddr));

    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    seraddr.sin_port = htons(port);
    ret = lwip_bind(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr));
    if (ret < 0) {
        MISC_PRINT("bind ip and port failed:%d\r\n", ret);
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    } else {
        MISC_PRINT(
            "Server successfully bound to ip: %s and port: %d\n", inet_ntoa(seraddr.sin_addr), ntohs(seraddr.sin_port));
    }

    msg = (char *)malloc(MAX_SIZE);
    if (msg == NULL) {
        MISC_PRINT("memory allocation failed\r\n");
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    }

    while (1) {
        (void)memset_s(msg, MAX_SIZE, 0, MAX_SIZE);
        (void)memset_s(&cliaddr, sizeof(cliaddr), 0, sizeof(cliaddr));
        fromlen = sizeof(cliaddr);
        MISC_PRINT("start receiv data\r\n");
        ret = lwip_recvfrom(sockfd, msg, MAX_SIZE - 1, 0, (struct sockaddr *)&cliaddr, (socklen_t *)&fromlen);
        if (ret < 0) {
            if ((ret == EAGAIN) || (ret == EINTR)) {
                continue;
            } else {
                MISC_PRINT("receiv from client data fail\r\n");
                free(msg);
                break;
            }
        } else {
            MISC_PRINT("client ip:%s\r\n", inet_ntoa(cliaddr.sin_addr));
        }

        // 打印接收到的数据
        msg[ret] = '\0';
        MISC_PRINT("recv from client %d bytes data\r\n", ret);
        print_data_chunk(msg, ret);

        n = (size_t)ret;
        for (i = 0; i < n; ++i) {
            if (msg[i] >= 'a' && msg[i] <= 'z') {
                msg[i] = (char)(msg[i] + 'A' - 'a');
            } else if (msg[i] >= 'A' && msg[i] <= 'Z') {
                msg[i] = (char)(msg[i] + 'a' - 'A');
            }
        }
        ret = lwip_sendto(sockfd, msg, ret, 0, (struct sockaddr *)&cliaddr, (socklen_t)fromlen);
        if (ret <= 0) {
            free(msg);
            break;
        } else {
            MISC_PRINT("data has been returned to the client.\r\n");
            print_data_chunk(msg, ret);
        }
    }

    free(msg);
    (void)lwip_close(sockfd);
    return MISC_OK;
}

uint32_t udpclient(int argc, const char **argv)
{
    int sockfd = -1;
    struct sockaddr_in server_addr;
    char *sendline = NULL;
    char *recvline = NULL;
    int n, len;

    if (strcmp(argv[0], "help") == 0) {
        client_help_usage();
        return MISC_OK;
    }

    if (argc != 3) { /* 3: parameters Number */
        MISC_PRINT("parameter input error.\r\n");
        client_help_usage();
        return MISC_FAIL;
    }
    const char *server_ip = argv[0]; /* 0: parameter index */

    size_t send_data_len = strlen(argv[2]);
    if (send_data_len >= MAX_SIZE) {
        MISC_PRINT("The data length exceeds the maximum(1023 bytes) limit.\r\n");
        return MISC_FAIL;
    }

    char *endptr = NULL;
    long int server_port = strtol(argv[1], &endptr, 10); /* 10：decimal notation */
    if (endptr == argv[1]) {
        MISC_PRINT("No digits were found\r\n");
        return MISC_FAIL;
    }
    /* Port 0 not supported , negative values not supported , max port limit is 65535 */
    if ((server_port < 0) || (server_port > 65535)) {
        client_help_usage();
        return MISC_FAIL;
    }

    sockfd = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        MISC_PRINT("socket creation failed\r\n");
        return MISC_FAIL;
    }

    memset_s(&server_addr, sizeof(server_addr), 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    int result = lwip_inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
    if (result <= 0) {
        if (result == 0) {
            MISC_PRINT("Invalid IP address format: %s\r\n", server_ip);
        } else {
            MISC_PRINT("IP address conversion failed\r\n");
        }
        client_help_usage();
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    }
    MISC_PRINT("server ip: %s and port: %d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

    sendline = (char *)malloc(MAX_SIZE);
    if (sendline == NULL) {
        MISC_PRINT("memory allocation failed for send buffer\r\n");
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    }

    recvline = (char *)malloc(MAX_SIZE);
    if (recvline == NULL) {
        MISC_PRINT("memory allocation failed for receive buffer\r\n");
        free(sendline);
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    }
    
    strncpy_s(sendline, MAX_SIZE, argv[2], MAX_SIZE - 1); /* 2: parameter index */
    sendline[MAX_SIZE - 1] = '\0'; /* 1: 1 byte length */

    len = sizeof(server_addr);

    int ret = lwip_sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&server_addr, (socklen_t)len);
    if (ret < 0) {
        MISC_PRINT("send data failed\r\n");
        free(sendline);
        free(recvline);
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    } else {
        MISC_PRINT("send %d bytes data success\r\n", ret);
        print_data_chunk(sendline, strlen(sendline));
    }

    n = lwip_recvfrom(sockfd, recvline, MAX_SIZE - 1, 0, (struct sockaddr *)&server_addr, (socklen_t *)&len);
    if (n < 0) {
        MISC_PRINT("recv from server data failed\r\n");
        free(sendline);
        free(recvline);
        (void)lwip_close(sockfd);
        return MISC_FAIL;
    }

    recvline[n] = '\0';
    MISC_PRINT("recv from server %d bytes data\r\n", n);
    print_data_chunk(recvline, n);

    free(sendline);
    free(recvline);
    (void)lwip_close(sockfd);
    return MISC_OK;
}
#endif /* LWIP_DEBUG_UDPSERVER */
