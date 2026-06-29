/*-
 * Copyright (c) 2008 Alexander Motin <mav@FreeBSD.org>
 * All rights reserved.
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
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

#ifndef SDHCI_PRIVATE_H
#define SDHCI_PRIVATE_H

#include "stdio.h"
#include "asm/io.h"
#include "errno.h"
#include "linux/scatterlist.h"
#include "linux/interrupt.h"
#include "los_mux.h"
#include "los_event.h"
#include "disk.h"
#include "mmcreg.h"
#include "mmc_adapt_os.h"
#include "hci.h"
#include "hci_helper.h"

#define	SDHCI_LOCK(_slot)		mmc_mtx_lock(&_slot->mtx)
#define	SDHCI_UNLOCK(_slot)		mmc_mtx_unlock(&_slot->mtx)
#define	SDHCI_LOCK_INIT(_slot)	\
    mmc_mtx_init(&_slot->mtx, "SD slot mtx", "sdhci", 0)
#define	SDHCI_LOCK_DESTROY(_slot)	mmc_mtx_destroy(&_slot->mtx);
#define	SDHCI_ASSERT_LOCKED(_slot)	mtx_assert(&_slot->mtx, MA_OWNED);
#define	SDHCI_ASSERT_UNLOCKED(_slot)	mtx_assert(&_slot->mtx, MA_NOTOWNED);

#define	SDHCI_BUS_LOCK(_slot)		mmc_mtx_lock(&_slot->bus_mtx)
#define	SDHCI_BUS_UNLOCK(_slot)		mmc_mtx_unlock(&_slot->bus_mtx)
#define	SDHCI_BUS_LOCK_INIT(_slot)	\
    mmc_mtx_init(&_slot->bus_mtx, NULL, NULL, 0)
#define	SDHCI_BUS_LOCK_DESTROY(_slot)	mmc_mtx_destroy(&_slot->bus_mtx);

#define SDHCI_EMMC_HW_RESET			0x534
#define SDHCI_AUTO_TUNING_CTRL			0x540
#define SDHCI_SW_TUNING_EN			0x00000010
#define SDHCI_AUTO_TUNING_STATUS		0x544

#define SDHCI_CENTER_PH_CODE_MASK		0x000000ff
#define SDHCI_PHASE_SEL_MASK			0xff

#define SDHCI_EMMC_CTRL				0x52c
#define SDHCI_EMMC_CTRL_EMMC			0x1
#define SDHCI_EMMC_CTRL_ENH_STROBE_EN		(1U << 8)

#define SDHCI_MULTI_CYCLE			0x54c
#define SDHCI_FOUND_EDGE			(0x1U << 11)
#define SDHCI_EDGE_DETECT_EN			(0x1U << 8)
#define SDHCI_DATA_DLY_EN			(0x1U << 3)
#define SDHCI_CMD_DLY_EN			(0x1U << 2)

int mmc_attach(device_t dev);
int mmc_detach(device_t dev);
int tune_edge(device_t brdev, device_t reqdev);

extern Atomic mmc_task_count;
extern uint32_t mmc_task_id;

#endif /* SDHCI_PRIVATE_H */
