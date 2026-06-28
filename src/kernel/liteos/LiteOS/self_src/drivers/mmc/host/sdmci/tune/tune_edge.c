/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: LiteOS MMC Himci Tune Edge Implementation
 * Author: Huawei LiteOS Team
 * Create: 2019-06-23
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

#include "stddef.h"
#include "mmc_adapt.h"
#include "mmcreg.h"
#include "mmc_subr_bus.h"
#include "mmc_private.h"
#include "hci.h"
#include "hci_helper.h"
#include "sdmci.h"
#include "mmc_adapt_os.h"
#include PLATFORM_MMC_HEADER_FILE

#define	SDMCI_PHASE_SCALE		8
#define	TUNE_EDGE_SIZE			0x100

#define	TUNE_RESULT_SUCCESS	1

typedef struct {
	uint32_t start;
	uint32_t end;
	uint32_t left;
	uint32_t right;
	uint32_t phase;
	uint32_t error;
} tune_result;

static const uint32_t tuning_blk_4bit[] = {
	0x00ff0fff, 0xccc3ccff, 0xffcc3cc3, 0xeffefffe,
	0xddffdfff, 0xfbfffbff, 0xff7fffbf, 0xefbdf777,
	0xf0fff0ff, 0x3cccfc0f, 0xcfcc33cc, 0xeeffefff,
	0xfdfffdff, 0xffbfffdf, 0xfff7ffbb, 0xde7b7ff7
};

static const uint32_t tuning_blk_8bit[] = {
	0xff00ffff, 0x0000ffff, 0xccccffff, 0xcccc33cc,
	0xcc3333cc, 0xffffcccc, 0xffffeeff, 0xffeeeeff,
	0xffddffff, 0xddddffff, 0xbbffffff, 0xbbffffff,
	0xffffffbb, 0xffffff77, 0x77ff7777, 0xffeeddbb,
	0x00ffffff, 0x00ffffff, 0xccffff00, 0xcc33cccc,
	0x3333cccc, 0xffcccccc, 0xffeeffff, 0xeeeeffff,
	0xddffffff, 0xddffffff, 0xffffffdd, 0xffffffbb,
	0xffffbbbb, 0xffff77ff, 0xff7777ff, 0xeeddbb77
};

static void
sdmci_set_sap_phase(const struct hci_softc *sc, uint32_t phase)
{
	uint32_t val = phase << CLK_SMPL_PHS_OFFSET;
	hci_mergel(sc, SDMMC_UHS_EXT, CLK_SMPL_PHS_MASK, val);
}

static int32_t
tune_get_cmd(const sdmci_slot *slot)
{
	const struct mmc_ios *ios = &slot->host.ios;
	int32_t opcode;

	opcode = MMC_SEND_TUNING_BLOCK;
	switch (ios->timing) {
	case bus_timing_mmc_hs400:
		dev_logi(slot->bus, "hs400 must be tuned in hs200 mode\n");
		return (-EINVAL);
	case bus_timing_mmc_hs200:
		opcode = MMC_SEND_TUNING_BLOCK_HS200;
		break;
	case bus_timing_uhs_ddr50:	/* FALLTHROUGH, no break */
	case bus_timing_uhs_sdr104:	/* FALLTHROUGH, no break */
	case bus_timing_uhs_sdr50:	/* FALLTHROUGH, no break */
	default:
		break;
	}
	return (opcode);
}

void
tune_wakeup(struct mmc_request *req)
{
	sdmci_slot *slot = (sdmci_slot *)req->done_data;
	(void)LOS_EventWrite(&slot->req_event, SDMCI_PEND_DTO);
}

int
sdmci_send_tuning(sdmci_slot *slot)
{
#define	HI_MCI_TUNINT_REQ_TIMEOUT	(LOSCFG_BASE_CORE_TICK_PER_SECOND * 10)
	int32_t             opcode = tune_get_cmd(slot);
	struct mmc_request  req = {NULL};
	struct mmc_command  cmd = {0};
	struct mmc_command  stop = {0};
	struct mmc_command  start = {0};
	struct mmc_data     data = {0};
	uint8_t             *blk = NULL;
	uint32_t            blk_size = 0;
	uint32_t            ret;
	struct mmc_ios      *ios = &slot->host.ios;
	uint8_t             buf[MMC_TUNING_LEN_HS200 + CACHE_ALIGNED_SIZE] = {0};
	uint32_t            mode = LOS_WAITMODE_AND | LOS_WAITMODE_CLR;
	struct hci_softc    *sc = device_get_softc(slot->bus);
	device_t            child = mmcdev_get_child(slot->dev);
	struct mmc_ivars    *ivar = device_get_ivars(child);
	/* 1000: logical suggestion for extreme scenes such as high temperature */
	uint32_t            retry_count = 1000;
	if (ios->bus_width == bus_width_8) {
		blk = (uint8_t *)tuning_blk_8bit;
		blk_size = sizeof(tuning_blk_8bit);
	} else if (ios->bus_width == bus_width_4) {
		blk = (uint8_t *)tuning_blk_4bit;
		blk_size = sizeof(tuning_blk_4bit);
	} else {
		return (-EINVAL);
	}

	if (opcode < 0)
		return (-EINVAL);

	start.opcode = opcode;
	start.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
	start.data = &data;

	data.data = (void *)LOS_Align((uintptr_t)buf, CACHE_ALIGNED_SIZE);
	data.len = blk_size;
	data.flags = MMC_DATA_READ;

	cmd.opcode = MMC_STOP_TRANSMISSION;
	cmd.flags = MMC_RSP_R1B | MMC_CMD_AC;

	stop.opcode = MMC_SEND_STATUS;
	stop.arg = (uint32_t)ivar->rca << 16;
	stop.flags = MMC_RSP_R1 | MMC_CMD_AC;
	req.bc = &start;

	req.cmd = &cmd;
	req.stop = &stop;
	req.done = tune_wakeup;
	req.done_data = slot;
	slot->req = &req;
	slot->flags = 0;

	sdmci_clock_enable(slot, false);
	/* Walkaround: logical suggestion for reliability for sdmci_clock_enable may timeout */
	while (retry_count > 0) {
		sdmci_idma_reset(sc);
		hci_writel(sc, SDMMC_RINTSTS, ALL_INT_CLR);
		tuning_reset_flag = 0;
		sdmci_clock_enable(slot, true);

		if (tuning_reset_flag != 1) {
			break;
		}
		retry_count--;
		if (retry_count == 0) {
			dev_loge(slot->bus, "sdmci_clock_enable failed\n");
			return -EINVAL;
		}
}

	sdmci_start(slot);
	ret = LOS_EventRead(&slot->req_event, SDMCI_PEND_DTO, mode, HI_MCI_TUNINT_REQ_TIMEOUT);
	if (ret == LOS_ERRNO_EVENT_READ_TIMEOUT) {
		return (MMC_ERR_NO_REP);
	} else {
		if (start.error != 0)
			return (start.error);
	}
	if (memcmp(blk, data.data, blk_size))
		return (-EIO);
	return (0);
}

static void
sdmci_edge_tuning_enable(const sdmci_slot *slot)
{
	struct hci_softc *sc = device_get_softc(slot->bus);
	hci_slot_ops *ops = slot->ops;

	ops->enable_drv_sample(sc, true);
	hci_setl(sc, SDMMC_TUNING_CTRL, HI_TUNING_ENABLE);
}

static void
sdmci_edge_tuning_disable(const sdmci_slot *slot)
{
	struct hci_softc *sc = device_get_softc(slot->bus);
	hci_slot_ops *ops = slot->ops;

	ops->enable_drv_sample(sc, false);
	hci_clearl(sc, SDMMC_TUNING_CTRL, HI_TUNING_ENABLE);
}

/*
 * The phase is an interval, there are only two cases,
 * (t->start <= a < b <= t->end), the interval is [a, b]
 * or [t->start, a] + [b, t->end].
 */
static void
tune_strict(sdmci_slot *slot, tune_result *t)
{
	uint32_t i;
	int tune_result = 0;
	uint32_t fail_index = 0;
	uint32_t failed[TUNE_EDGE_SIZE + 1];
	hci_slot_ops *ops = slot->ops;
	struct hci_softc *sc = device_get_softc(slot->bus);

	if (t->end - t->start > TUNE_EDGE_SIZE) {
		dev_loge(slot->bus, "tune edge %u,%u: overflow\n", t->end, t->start);
		return;
	}

	for (i = t->start; i <= t->end; i++) {
		ops->select_sample_phase(sc, i);
		tune_result = sdmci_send_tuning(slot);
		if (tune_result == 0)
			dev_logt(slot->bus, "success_idx %u\n", i);
		else {
			failed[fail_index++] = i;
			dev_logt(slot->bus, "fail_index %u\n", i);
		}
	}
	if (fail_index > 0) {
		t->left = failed[fail_index - 1] + t->phase;
		if (t->error == 0) {
			t->right = failed[0] + t->phase;
			t->error = 1;
		}
	}
}

static int
tune_edge_dll(sdmci_slot *slot, tune_result *t)
{
	uint32_t index;
	uint32_t phase_offset = 0;
	const uint32_t start_ele = 2;
	const uint32_t scales = SDMCI_PHASE_SCALE;
	hci_slot_ops *ops = slot->ops;
	struct hci_softc *sc = device_get_softc(slot->bus);
	uint32_t elements = ops->get_taps(sc) / scales;
	uint32_t totalphases = elements * scales;
	tune_result r = {0};

	if (totalphases == 0) {
		dev_logt(slot->bus, "phase is 0 for sap_dll master line tap is %u, phase scale is %u\n",
		    ops->get_taps(sc), scales);
		return (-1);
	}

	r.left = t->end * elements;
	r.right = t->start * elements;
	r.end = elements;
	r.start = start_ele;
	phase_offset = t->start * elements;
	r.error = 0;
	if (t->end >= t->start) {
		for (index = t->start; index < t->end; index++) {
			if (sdmci_card_state(slot) == MMC_CAP_CARD_ABSENT) {
				return (-EAGAIN);
			}
			sdmci_set_sap_phase(sc, index);
			r.phase = phase_offset;
			tune_strict(slot, &r);
			dev_logd(slot->bus, "left:%u right:%u\n", r.left, r.right);
			phase_offset += elements;
		}
	} else {
		for (index = t->start; index < scales; index++) {
			if (sdmci_card_state(slot) == MMC_CAP_CARD_ABSENT) {
				return (-EAGAIN);
			}
			sdmci_set_sap_phase(sc, index);
			r.phase = phase_offset;
			tune_strict(slot, &r);
			dev_logd(slot->bus, "left:%u right:%u\n", r.left, r.right);
			phase_offset += elements;
		}

		phase_offset = 0;
		for (index = 0; index < t->end; index++) {
			if (sdmci_card_state(slot) == MMC_CAP_CARD_ABSENT) {
				return (-EAGAIN);
			}
			sdmci_set_sap_phase(sc, index);
			r.phase = phase_offset;
			tune_strict(slot, &r);
			dev_logd(slot->bus, "left:%u right:%u\n", r.left, r.right);
			phase_offset += elements;
		}
	}

	if (r.right <= r.left) {
		r.right += totalphases;
	}

	phase_offset = ((r.left + r.right) / 2) % totalphases;
	index = (phase_offset / elements);

	r.phase = (phase_offset % elements);
	r.phase = ((r.phase > start_ele) ? r.phase : start_ele);

	slot->tune_phase = index;
	sdmci_set_sap_phase(sc, index);
	ops->select_sample_phase(sc, r.phase);

	dev_logd(slot->bus, "tuning phase:[%u / %u] [%u / %u] \n",
	    index, (scales - 1), r.phase, elements);

	return (0);
}

static void
tune_clean(const sdmci_slot *slot)
{
	sdmci_clock_enable(slot, false);
	mmc_delay_ms(1);
	sdmci_slot_reset(slot);
	mmc_delay_ms(1);
	sdmci_clock_enable(slot, true);
	mmc_delay_ms(1);
}

static int
tuning(sdmci_slot *slot, uint32_t parse)
{
	int status;
	uint32_t value;
	struct hci_softc *sc = device_get_softc(slot->bus);

	sdmci_set_sap_phase(sc, parse);

	status = sdmci_send_tuning(slot);
	if (status == 0) {
		value = hci_readl(sc, SDMMC_TUNING_CTRL);
		return ((value & HI_FOUND_EDGE) ? TUNE_RESULT_SUCCESS : 0);
	} else {
		return (TUNE_RESULT_SUCCESS);
	}
}

static int
tune_basic(sdmci_slot *slot, tune_result *t)
{
	uint32_t i;
	int tune_result;
	uint32_t end_edge = SDMCI_PHASE_SCALE;
	uint32_t start_edge = 0;
	int prev_result = 0;
	for (i = 0; i < SDMCI_PHASE_SCALE; i++) {
		if (sdmci_card_state(slot) == MMC_CAP_CARD_ABSENT)
			return (-EAGAIN);
		tune_result = tuning(slot, i);
		dev_logt(slot->bus, "tune_result %d\n", tune_result);

		if (prev_result && !tune_result) {
			end_edge = i;
		} else if (!prev_result && tune_result) {
			start_edge = i;
		}

		if ((start_edge != 0) && (end_edge != SDMCI_PHASE_SCALE))
			break;

		prev_result = tune_result;
	}

	if ((start_edge == 0) && (end_edge == SDMCI_PHASE_SCALE)) {
		return (-1);
	} else {
		t->left = start_edge;
		t->right = end_edge;
		dev_logd(slot->bus, "t->left :%u t->right %u\n", t->left, t->right);
		return (0);
	}
}

int
sdmci_tune_edge(device_t brdev, device_t reqdev)
{
	(void)reqdev;
	struct hci_softc *sc = device_get_softc(brdev);
	sdmci_slot *slot = (sdmci_slot *)sc->slot;
	int err;
	tune_result t = {0};

	if (!(slot->opt & SDMCI_TUNING_SUPPORTED)) {
		return (0);
	}
	SDMCI_LOCK(slot);
	slot->is_tuning = 1;
	/* Tell the host it's tuning failed by default */
	slot->opt &= ~SDMCI_TUNING_ENABLED;
	sdmci_edge_tuning_enable(slot);
	err = tune_basic(slot, &t);
	if (err != 0) {
		tune_clean(slot);
		slot->is_tuning = 0;
		SDMCI_UNLOCK(slot);
		dev_loge(slot->bus, "no valid phase shift! use default\n");
		return (-1);
	}
	dev_logt(slot->bus, "scan edges p2f:%u f2p:%u\n", t.left, t.right);
	t.phase = t.left + t.right;

	if (t.left > t.right) {
		t.phase = t.phase / 2 % SDMCI_PHASE_SCALE;
	} else {
		t.phase = (t.phase + SDMCI_PHASE_SCALE) / 2 % SDMCI_PHASE_SCALE;
	}
	dev_logd(slot->bus, "mix set temp-phase %u\n", t.phase);
	sdmci_set_sap_phase(sc, t.phase);
	(void)sdmci_send_tuning(slot);
	sdmci_edge_tuning_disable(slot);

	t.start = t.left;
	t.end = t.right;
	err = tune_edge_dll(slot, &t);
	tune_clean(slot);
	if (err == 0)
		(void)sdmci_send_tuning(slot);

	/* Tell the host it's tuning successful, we can do it in the future */
	slot->opt |= SDMCI_TUNING_ENABLED;
	slot->is_tuning = 0;
	SDMCI_UNLOCK(slot);
	return (0);
}

int
sdmci_retune(device_t brdev, device_t reqdev, bool reset)
{
	(void)reset;
	int status;
	sdmci_slot *slot = device_get_ivars(reqdev);
	slot->retune_req = 0;
	status = sdmci_tune_edge(brdev, reqdev);
	return (status);
}
