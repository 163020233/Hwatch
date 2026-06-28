/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: At net function \n
 *
 */

#include "at_net.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "at_cmd.h"
#include "at_net_cmd_register.h"
#include "at_net_cmd_table.h"
#include "debug_print.h"
#include "net_at_process.h"

#define AT_NET_FUNC_NUM (sizeof(g_at_net_cmd_parse_table) / sizeof(g_at_net_cmd_parse_table[0]))

void los_at_net_cmd_register(void)
{
    print_str("los_at_net_cmd_register EXCUTE\r\n");
    uapi_at_net_register_cmd(g_at_net_cmd_parse_table, AT_NET_FUNC_NUM);
}

static void space_to_comma(char *cmd)
{
    for (uint32_t i = 0; i < strlen(cmd); i++) {
        if (cmd[i] == ' ') {
            cmd[i] = ',';
        }
    }
}

static const at_net_cmd g_at_net_cmd[] = {
    {"NET_OPEN_BTPAN",           NetATOpenBtPan},
#ifdef AT_COMMAND_MQTT
    {"NETSTACK_MQTT",            NetATMqtt},
    {"NETSTACK_MQTTSECURE",      NetATMqttSecure},
#endif
#ifdef AT_COMMAND_HTTP
    {"NETSTACK_HTTP",            NetATHttp},
    {"NETSTACK_HTTPDOWNLOAD",       NetATHttpDownLoad},
#endif
#ifdef AT_COMMAND_WEBSOCKET
    {"NETSTACK_WEBSOCKET",       NetATWebsocket},
#endif
    {"NETSTACK_SWITCH",          NetATSwitch},
    {"NETSTACK_PING",            NetATPing},
    {"NETSTACK_SNTP",            NetATSntp},
    {"NETSTACK_SEND_MULTICAST",            NetATSendMultiCast},
    {"NETSTACK_RECV_MULTICAST",            NetATRecvMultiCast},
#ifdef CHBA_SUPPORT
    {"NETSTACK_SLE",             NetATSLECHBA},
#endif
    {"NETSTACK_IPERF",            NetATIperf},
    {"NETSTACK_IFCONFIG",         NetATIfconfig},
    {"NETSTACK_GETDNS",           NetATGetDnsConfig},
    {"NETSTACK_SETDNS",           NetATSetDnsConfig},
    {"NETSTACK_TCPSERVER",        NetATTcpServer},
    {"NETSTACK_TCPCLIENT",        NetATTcpClient},
    {"NETSTACK_UDPSERVER",        NetATUdpServer},
    {"NETSTACK_UDPCLIENT",        NetATUdpClient},
#ifdef NET_AT_TEST_NETSTACK
    {"NETSTACK_SENDACV",            NetATSendAvc},
    {"NETSTACK_RECVACV",            NetATRecvAvc},
#ifdef AT_COMMAND_MQTT
    {"NETSTACK_MQTTSENDTEXT",       NetATMqttSendText},
    {"NETSTACK_MQTTSENDBIN",        NetATMqttSendBin},
    {"NETSTACK_MQTTSESENDTEXT",     NetATMqttSecureSendText},
    {"NETSTACK_MQTTSESENDBIN",      NetATMqttSecureSendBin},
#endif
#ifdef AT_COMMAND_HTTP
    {"NETSTACK_HTTPREQUEST",        NetATHttpRequest},
    {"NETSTACK_HTTPSETANYHEADER",   NetATHttpSetAnythingHeader},
    {"NETSTACK_HTTPSETJSONHEADER",  NetATHttpSetJsonHeader},
    {"NETSTACK_HTTPSETTEXTHEADER",  NetATHttpSetTextHeader},
    {"NETSTACK_HTTPPOST",           NetATHttpPost},
    {"NETSTACK_HTTPPUT",            NetATHttpPut},
    {"NETSTACK_HTTPDELETE",         NetATHttpDelete},
#endif
#ifdef AT_COMMAND_WEBSOCKET
    {"NETSTACK_WEBSOCKETSENDTXT",   NetATWebsocketSendText},
    {"NETSTACK_WEBSOCKETSENDBIN",   NetATWebsocketSendBin},
#endif
#ifdef AT_COMMAND_NETCONNECT
    {"NETSTACK_GETNETSTATE",        NetATGetConnectState},
    {"NETSTACK_REGCONNECTSTATE",    NetATRegConnect},
    {"NETSTACK_UNREGCONNECTSTATE",  NetATUnRegConnect},
#endif
#endif
};

td_void at_net_process(int argc, char *argv[])
{
    if (argc < 1) {
        return;
    }

    const size_t numCommands = sizeof(g_at_net_cmd) / sizeof(g_at_net_cmd[0]);
    for (size_t i = 0; i < numCommands; ++i) {
        if (strcmp(argv[0], g_at_net_cmd[i].cmd) == 0) {
            g_at_net_cmd[i].handler(argc - 1, &argv[1]);
            return;
        }
    }

    PRINT("Unknown command: %s\n", argv[0]);
    return;
}

static td_s32 at_net_convert_bin_to_dec(td_s32 pbin)
{
    td_s32 result = 0;
    td_s32 temp = pbin;

    while (temp != 0) {
        result += temp % 2; // 2 div
        temp /= 2; // 2 div
    }

    return result;
}

at_ret_t at_net_sample_process(const net_args_t *arg)
{
    PRINT("net_at enter \r\n");
    td_s32 argc = at_net_convert_bin_to_dec(arg->para_map);
    char *argv[] = {arg->para1, arg->para2, arg->para3, arg->para4, arg->para5, arg->para6, arg->para7, arg->para8,
        arg->para9, arg->para10, arg->para11, arg->para12, arg->para13, arg->para14, arg->para15, arg->para16,
        arg->para17, arg->para18, arg->para19, arg->para20};
    PRINT("AT+NET=");
    for (td_s32 i = 0; i < argc; i++) {
        printf("%s,", argv[i]);
        if (strcmp(argv[i], " ") == 0) {
            argc = i;
            break;
        }
        space_to_comma(argv[i]);
    }
    printf("\n");

    at_net_process(argc, argv);

    return AT_RET_OK;
}