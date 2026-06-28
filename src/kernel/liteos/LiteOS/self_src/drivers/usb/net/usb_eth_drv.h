/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Usb ethernet HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

/* Ethernet driver structure */

#ifndef _ETH_DRV_H
#define	_ETH_DRV_H

#include "lwip/netif.h"

#define DUMP_LWIP_DATA 0
#if DUMP_LWIP_DATA
#define LWIP_DUMP_DATA_ONE_LINE_LENGTH 16
#define LWIP_DUMP_DATA_MAX_LENGTH 96
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct los_eth_driver {
    struct netif ac_if;
	void *driver_context;
};

struct los_eth_funs {
	void (*init)(struct los_eth_driver *drv, const unsigned char *mac_addr);
	void (*recv)(struct los_eth_driver *drv, int len);
	void (*send_complete)(struct los_eth_driver *drv, unsigned int key, int state);
};

struct eth_drv_sg {
	uintptr_t buf;
	UINT32 len;
};

#define	CYGNUM_IO_ETH_DRIVERS_SG_LIST_SIZE 32
#define	MAX_ETH_DRV_SG CYGNUM_IO_ETH_DRIVERS_SG_LIST_SIZE
#define	MAX_ETH_MSG 1540


struct eth_drv_sc {
	struct eth_hwr_funs *funs;
	void				*driver_private;
	const char			*dev_name;
	unsigned int		state;
};

typedef void (*eth_start)(struct los_eth_driver *sc, unsigned char *enaddr, int flags);
typedef void (*eth_stop)(struct los_eth_driver *sc);
typedef int (*eth_control)(struct los_eth_driver *sc, unsigned long key, void *data, int data_length);
typedef int (*eth_can_send)(struct los_eth_driver *sc);
typedef void (*eth_send)(struct los_eth_driver *sc, struct eth_drv_sg *sg_list,
			   int sg_len, int total_len, uintptr_t key);
typedef void (*eth_recv)(struct los_eth_driver *sc, struct eth_drv_sg *sg_list, int sg_len);
typedef void (*eth_deliver)(struct los_eth_driver *sc);
typedef void (*eth_poll)(struct los_eth_driver *sc);
typedef int (*eth_int_vector)(struct los_eth_driver *sc);

struct eth_hwr_funs {
	/* Initialize hardware (including startup) */
	eth_start start;
	/* Shut down hardware */
	eth_stop stop;
	/* Device control (ioctl pass-through) */
	eth_control control;
	/* Query - can a packet be sent? */
	eth_can_send can_send;
	/* Send a packet of data */
	eth_send send;
	/* Receive [unload] a packet of data */
	eth_recv recv;
	/*
	 * Deliver data to/from device from/to stack memory space
	 * (moves lots of memcpy()s out of DSRs into thread)
	 */
	eth_deliver deliver;
	/* Poll for interrupts/device service */
	eth_poll poll;
	/* Get interrupt information from hardware driver */
	eth_int_vector int_vector;
	/* Logical driver interface */
	const struct los_eth_funs *eth_drv;
	const struct los_eth_funs *eth_drv_old;
};

const struct los_eth_funs *get_eth_drv_funs_usb(void);

/* Control 'key's */
#define	ETH_DRV_SET_MAC_ADDRESS 0x0100

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ETH_DRV_H */
