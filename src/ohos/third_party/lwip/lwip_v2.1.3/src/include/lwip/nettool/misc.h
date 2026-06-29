/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: DNS and UDP\TCP API definition
 * Create:2026
 */

#ifndef LWIP_OTHERS_H
#define LWIP_OTHERS_H
#include "lwip/opt.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LWIP_DNS
uint32_t os_shell_dns(int argc, const char **argv);
uint32_t os_shell_show_dns(void);
#endif /* LWIP_DNS */

#ifdef LWIP_DEBUG_TCPSERVER
uint32_t os_tcpserver(int argc, const char **argv);
uint32_t os_tcpclient(int argc, const char **argv);
#endif /* LWIP_DEBUG_TCPSERVER */

#ifdef LWIP_DEBUG_UDPSERVER
uint32_t udpserver(int argc, const char **argv);
uint32_t udpclient(int argc, const char **argv);
#endif /* LWIP_DEBUG_UDPSERVER */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_OTHERS_H */
