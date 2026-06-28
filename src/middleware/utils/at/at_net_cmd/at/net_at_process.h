/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides at cmd register func \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-20， Create file. \n
 */
#ifndef NET_AT_PROCESS_H
#define NET_AT_PROCESS_H

#include <stdint.h>
#include <stdio.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */
uint32_t NetATOpenBtPan(uint8_t argc, char *argv[]);
#ifdef AT_COMMAND_MQTT
uint32_t NetATMqttSecure(uint8_t argc, char *argv[]);
uint32_t NetATMqtt(uint8_t argc, char *argv[]);
#endif
#ifdef AT_COMMAND_HTTP
uint32_t NetATHttp(uint8_t argc, char *argv[]);
uint32_t NetATHttpDownLoad(uint8_t argc, char *argv[]);
#endif
#ifdef AT_COMMAND_WEBSOCKET
uint32_t NetATWebsocket(uint8_t argc, char *argv[]);
#endif
uint32_t NetATSwitch(uint8_t argc, char *argv[]);
uint32_t NetATPing(uint8_t argc, char *argv[]);
uint32_t NetATSntp(uint8_t argc, char *argv[]);
uint32_t NetATSendMultiCast(uint8_t argc, char *argv[]);
uint32_t NetATRecvMultiCast(uint8_t argc, char *argv[]);
#ifdef CHBA_SUPPORT
uint32_t NetATSLECHBA(uint8_t argc, char *argv[]);
#endif
uint32_t NetATIperf(uint8_t argc, char *argv[]);
uint32_t NetATIfconfig(uint8_t argc, char *argv[]);
uint32_t NetATGetDnsConfig(uint8_t argc, char *argv[]);
uint32_t NetATSetDnsConfig(uint8_t argc, char *argv[]);
uint32_t NetATTcpServer(uint8_t argc, char *argv[]);
uint32_t NetATTcpClient(uint8_t argc, char *argv[]);
uint32_t NetATUdpServer(uint8_t argc, char *argv[]);
uint32_t NetATUdpClient(uint8_t argc, char *argv[]);
#ifdef NET_AT_TEST_NETSTACK
uint32_t NetATSendAvc(uint8_t argc, char *argv[]);
uint32_t NetATRecvAvc(uint8_t argc, char *argv[]);
#ifdef AT_COMMAND_MQTT
uint32_t NetATMqttSecureSendText(uint8_t argc, char *argv[]);
uint32_t NetATMqttSecureSendBin(uint8_t argc, char *argv[]);
uint32_t NetATMqttSendText(uint8_t argc, char *argv[]);
uint32_t NetATMqttSendBin(uint8_t argc, char *argv[]);
#endif
#ifdef AT_COMMAND_HTTP
uint32_t NetATHttpRequest(uint8_t argc, char *argv[]);
uint32_t NetATHttpSetAnythingHeader(uint8_t argc, char *argv[]);
uint32_t NetATHttpSetJsonHeader(uint8_t argc, char *argv[]);
uint32_t NetATHttpSetTextHeader(uint8_t argc, char *argv[]);
uint32_t NetATHttpPost(uint8_t argc, char *argv[]);
uint32_t NetATHttpPut(uint8_t argc, char *argv[]);
uint32_t NetATHttpDelete(uint8_t argc, char *argv[]);
#endif
#ifdef AT_COMMAND_WEBSOCKET
uint32_t NetATWebsocketSendText(uint8_t argc, char *argv[]);
uint32_t NetATWebsocketSendBin(uint8_t argc, char *argv[]);
#endif
uint32_t NetATGetConnectState(uint8_t argc, char *argv[]);
uint32_t NetATRegConnect(uint8_t argc, char *argv[]);
uint32_t NetATUnRegConnect(uint8_t argc, char *argv[]);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
