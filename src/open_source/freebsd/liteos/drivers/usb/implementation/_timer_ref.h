/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2009 Andrew Thompson
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2025. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* from sys/dev/usb/usbdi.h */
#ifndef _TIMER_REF_H
#define	_TIMER_REF_H

#include <los_swtmr.h>
#include <los_spinlock.h>

#define	DELAY(t)	usleep(t)
#define	msleep	LOS_Msleep

#ifndef MSEC_PER_SEC
#define	MSEC_PER_SEC	1000L
#endif

#ifndef NSEC_PER_USEC
#define	NSEC_PER_USEC	1000L
#endif

#ifndef NSEC_PER_MSEC
#define	NSEC_PER_MSEC	1000000L
#endif

#ifndef NSEC_PER_SEC
#define	NSEC_PER_SEC	1000000000L
#endif

typedef void (*timer_func)(unsigned long);

struct callout_data {
	timer_func func;
	void *arg;
};

struct usb_callout {
	uint16_t timer;
	uint16_t timer_flag;
	struct mtx *tm_mtx;
	struct callout_data callout_data;
	SPIN_LOCK_S lock;
};

void	usb_callout_init_mtx(struct usb_callout *c, struct mtx *mtx, int flag);
void	usb_callout_reset(struct usb_callout *c, int to_ticks, void (*func)(void *), void *arg);
void	usb_callout_stop(struct usb_callout *c);
void	usb_callout_drain(struct usb_callout *c);
void	usb_callout_pending(struct usb_callout *c);

#endif /* _TIMER_REF_H */
