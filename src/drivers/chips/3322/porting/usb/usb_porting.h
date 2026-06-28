/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides uart port template \n
 *
 * History: \n
 * 2022-06-06， Create file. \n
 */
#ifndef USB_PORT_H
#define USB_PORT_H
#include <stdint.h>
#include "stdlib.h"
#include "implementation/usb_init.h"
#ifdef SUPPORT_LWIP
#include "lwip/netifapi.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef SUPPORT_LWIP
typedef struct {
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;
} ip_info_t;
#endif

// for serial
#define USB_SERIAL_0 0  // for at
#define USB_SERIAL_1 1  // for hso
#define USB_SERIAL_COUNT_MAX 2
// register vbus irq
void registe_vbus_irq(void);
int uapi_usb_serial_write(uint32_t index, const void *buffer, uint16_t length);
#ifdef SUPPORT_LWIP
uint32_t uapi_usb_set_rndis_netif_info(ip_info_t info);
struct netif *lwip_get_usb_netif(void);
#endif
uint32_t uapi_usb_device_init(device_type dtype);
uint32_t uapi_usb_device_deinit(device_type dtype);


// host
uint32_t uapi_usb_host_init(void);
void uapi_usb_host_deinit(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
