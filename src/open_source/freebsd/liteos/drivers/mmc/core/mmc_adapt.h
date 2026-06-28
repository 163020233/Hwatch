/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)param.h	8.3 (Berkeley) 4/4/95
 */

#ifndef DEV_MMC_ADAPT_H
#define DEV_MMC_ADAPT_H

#include "stdio.h"
#include "stdint.h"
#include "assert.h"
#include "mmc_adapt_os.h"
#include "los_printf.h"
#include "los_tick.h"
#include "los_config.h"
#include "los_sched_pri.h"
#include "mmc_type.h"

#define	BSD	199506		/* System version (year & month). */
#define BSD4_3	1
#define BSD4_4	1

/*
 * __FreeBSD_version numbers are documented in the Porter's Handbook.
 * If you bump the version for any reason, you should update the documentation
 * there.
 * Currently this lives here in the doc/ repository:
 *
 *	documentation/content/en/books/porters-handbook/versions/_index.adoc
 *
 * Encoding:	<major><two digit minor>Rxx
 *		'R' is in the range 0 to 4 if this is a release branch or
 *		X.0-CURRENT before releng/X.0 is created, otherwise 'R' is
 *		in the range 5 to 9.
 * Short hand: MMmmXXX
 *
 * __FreeBSD_version is bumped every time there's a change in the base system
 * that's noteworthy. A noteworthy change is any change which changes the
 * kernel's KBI in -CURRENT, one that changes some detail about the system that
 * external software (or the ports system) would want to know about, one that
 * adds a system call, one that adds or deletes a shipped library, a security
 * fix, or similar change not specifically noted here. Bumps should be limited
 * to one per day / a couple per week except for security fixes.
 *
 * The approved way to obtain this from a shell script is:
 *	awk '/^\#define[[:space:]]*__FreeBSD_version/ {print $3}'
 * Other methods to parse this file may work, but are not guaranteed against
 * future changes. The above script works back to FreeBSD 3.x when this macro
 * was introduced. This number is propagated to other places needing it that
 * cannot include sys/param.h and should only be updated here.
 */
#undef __FreeBSD_version
#define __FreeBSD_version 1403000

/*
 * __FreeBSD_kernel__ indicates that this system uses the kernel of FreeBSD,
 * which by definition is always true on FreeBSD. This macro is also defined
 * on other systems that use the kernel of FreeBSD, such as GNU/kFreeBSD.
 *
 * It is tempting to use this macro in userland code when we want to enable
 * kernel-specific routines, and in fact it's fine to do this in code that
 * is part of FreeBSD itself.  However, be aware that as presence of this
 * macro is still not widespread (e.g. older FreeBSD versions, 3rd party
 * compilers, etc), it is STRONGLY DISCOURAGED to check for this macro in
 * external applications without also checking for __FreeBSD__ as an
 * alternative.
 */
#undef __FreeBSD_kernel__
#define __FreeBSD_kernel__

extern MMC_MUTEX mmc_resource_mutex;

#define mmc_resource_mutex_init() 		mmc_mtx_init(&mmc_resource_mutex, NULL, NULL, 0)
#define mmc_resource_mutex_lock() 		mmc_mtx_lock(&mmc_resource_mutex)
#define mmc_resource_mutex_unlock() 		mmc_mtx_unlock(&mmc_resource_mutex)
#define mmc_resource_mutex_destroy() 		mmc_mtx_destroy(&mmc_resource_mutex)
#define is_mmc_resource_mutex_not_init()	MMC_MUTEX_IS_INVALID(mmc_resource_mutex)

#ifndef slot_printf
#define	slot_printf(slot, fmt, args...)	dprintf("%s-slot%u: " fmt, device_get_nameunit(slot->bus), slot->num, ##args)
#endif

#ifndef dev_loge
#define	dev_loge(dev, fmt, args...)	dprintf("[%s|ERROR] " fmt, device_get_nameunit(dev), ##args)
#endif

#ifndef dev_logw
#define	dev_logw(dev, fmt, args...)	dprintf("[%s|WARN] " fmt, device_get_nameunit(dev), ##args)
#endif

#ifndef mmc_printf
#define	mmc_printf(fmt, args...)	dprintf(fmt, ##args)
#endif

#define	mmc_trace(fmt, args...)	do {	\
		dprintf("%s:%d: ", __FUNCTION__, __LINE__);	\
		dprintf(fmt, ##args);	\
	} while (0)
#define	mmc_err(fmt, args...)	do {	\
		dprintf("[ERROR] %s:%d: ", __FUNCTION__, __LINE__);	\
		dprintf(fmt, ##args);	\
	} while (0)

#ifdef MMC_TRACE_LEVEL
#define	dev_logi(dev, fmt, args...)	\
    dprintf("[%s|INFO] " fmt, device_get_nameunit(dev), ##args)
#define	dev_logd(dev, fmt, args...)	\
    dprintf("[%s|%s@%s,%d] " fmt, device_get_nameunit(dev), __FUNCTION__, __FILE__, __LINE__, ##args)
#define	dev_logt(dev, fmt, args...)	\
    dprintf("[%s|%s@%s,%d] " fmt, device_get_nameunit(dev), __FUNCTION__, __FILE__, __LINE__, ##args)
#else
#define	dev_logi(dev, fmt, args...)
#define	dev_logd(dev, fmt, args...)
#define	dev_logt(dev, fmt, args...)
#endif

#define	DELAY(t)	LOS_Udelay(t)
#define	mmc_align(args, align)	((args) & ~((align) - 1))
static inline void mmc_delay_ms(uint32_t ms) {
	const uint32_t ms_delay = mmc_align(ms, (OS_SYS_MS_PER_SECOND / LOSCFG_BASE_CORE_TICK_PER_SECOND));
	const uint32_t rest_delay = ms - ms_delay;
#ifdef LOSCFG_DRIVERS_MMC_RW_IN_BOOT_AND_EXC
	if((!OS_SCHEDULER_ACTIVE) || OS_EXC_ACTIVE) {
		if (ms_delay)
			LOS_Mdelay(ms_delay);
		if (rest_delay) {
			LOS_Mdelay(rest_delay);
		}
		return;
	}
#endif
	if (ms_delay) {
		LOS_Msleep(ms_delay);
	}
	if (rest_delay) {
		LOS_Mdelay(rest_delay);
	}
}

#ifdef LOSCFG_DRIVERS_MMC_RW_IN_BOOT_AND_EXC
#ifndef MMC_PRE_SCAN_COUNT
#define MMC_PRE_SCAN_COUNT (50)
#endif
#ifndef MMC_SCAN_INTERVAL
#define MMC_SCAN_INTERVAL (50*1000)             /* us */
#endif
#define MMC_RELOAD_DELAY_IN_EXC (1500)					/* us */
#endif

#define	mmc_setbit(a, i)	*(uint32_t *)(a) |= 1U << (i)
#define	mmc_clrbit(a, i)	*(uint32_t *)(a) &= ~(1U << (i))
#define	mmc_isset(a, i)		((*(uint32_t *)(a) & (1U << (i))) != 0)
#define	mmc_isclr(a, i)		((*(uint32_t *)(a) & (1U << (i))) == 0)

int eventratecheck(struct timeval *lasttime, int *curpps, int maxpps);
#define ppsratecheck(t, c, m) eventratecheck(t, c, m)

device_t mmc_new_device(device_t busdev);
void mmc_del_device(device_t dev);
device_t mmcsd_new_device(device_t busdev);
void mmcsd_del_device(device_t dev);
device_t sdio_new_device(device_t busdev);
void sdio_del_device(device_t dev);
int mmc_device_del_child(device_t child);

int mmcbus_generic_attach(device_t dev);
device_t get_any_card(device_t busdev);
int bus_width_value(device_t dev);
int wait_card_ready(device_t dev, device_t mmcbus, bool in_exc);
#endif
