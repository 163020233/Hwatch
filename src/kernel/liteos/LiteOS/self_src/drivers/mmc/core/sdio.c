/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: LiteOS MMC Sdio Implementation
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

#include "inode/inode.h"
#include "mmc_type.h"
#include "bridge.h"
#include "mmcbrvar.h"
#include "mmcvar.h"
#include "mmc_adapt.h"
#include "sdio_reg.h"
#include "sdio_ops.h"
#include "sdio.h"
#include "block.h"

#define	SDIO_IO_BUST	(32 * 1024)

#ifndef SDIO_DEBUG_LOG_DISABLE
bool sdio_debug = true;
#else
bool sdio_debug = false;
#endif

static uint32_t sdio_card_no = 0;

uint32_t *
get_sdio_card_no(void)
{
	return &sdio_card_no;
}

uint32_t
get_sdio_card_no_bits(void)
{
	return sizeof(sdio_card_no) << 3;
}

static const char *sdio_class_info[] =
{
	"None",
	"UART",
	"Bluetooth Type-A",
	"Bluetooth Type-B",
	"GPS",
	"Camera",
	"PHS",
	"WLAN",
	"Embedded ATA",
	"Bluetooth Type-A AMP",
};

int
get_sdio_card_cnt(void)
{
	uint32_t max_num = get_sdio_card_no_bits();
	for (uint32_t i = 0; i < max_num; i++) {
		if (mmc_isclr(&sdio_card_no, i)) {
			mmc_setbit(&sdio_card_no, i);
			return ((int)i);
		}
	}
	mmc_err("sdio number run out!\n");
	return (-1);
}

void
clear_sdio_card_cnt(uint32_t bit)
{
	uint32_t max_num = get_sdio_card_no_bits();

	if (bit < max_num) {
		mmc_clrbit(&sdio_card_no, bit);
	} else {
		mmc_err("sdio number error!\n");
	}
}

static void
sdio_delete_func(struct sdio_softc *sc)
{
	uint32_t i;
	struct sdio_func *func;

	for (i = 0; i < sc->sdio_func_num; i++) {
		func = sc->sdio_funcs[i];
		if (func != NULL) {
			free(func);
			sc->sdio_funcs[i] = NULL;
		}
	}
}

static int
sdio_get_fun_class(struct sdio_func *func)
{
	int ret;
	uint8_t val;
	uint32_t func_addr_base = FUNC_ADDR_BASE(func->func_num);
	device_t mmcbus = func->sc->mmcbus;
	device_t dev = func->sc->dev;

	if (mmc_get_quirks(dev) & MMC_QUIRK_NON_STD_FUNC) {
		func->func_class = 0;
		return (0);
	}

	ret = sdio_rw_direct_ext(mmcbus, 0, func_addr_base, false, &val);
	if (ret) {
		return (ret);
	}

	if ((val & 0x0F) > 0x0E) {
		ret = sdio_rw_direct_ext(mmcbus, 0, (func_addr_base + SD_IO_FBR_EXT_IF), false, &val);
		if (ret) {
			return (ret);
		}
	}
	func->func_class = val;
	dev_logt(dev, "func_class:0x%x.\n", val);
	return (ret);
}

static int
sdio_init_funcs(struct sdio_softc *sc)
{
	uint32_t i;
	struct sdio_func *func;
	device_t child = NULL;

	for (i = 1; i <= sc->sdio_func_num; i++) {
		func = zalloc(sizeof(struct sdio_func));
		if (func == NULL) {
			return (-ENOMEM);
		}
		func->sc = sc;
		func->func_num = i;
		func->cur_blk_size = MMC_CARDBUS_BLOCK_SIZE;
		child = sc->dev;

		if (mmc_get_quirks(child) & MMC_QUIRK_NON_STD_FUNC) {
			func->manufacturer_id = (uint16_t)mmc_get_cis_manid(child);
			func->device_id = (uint16_t)mmc_get_cis_proid(child);
		} else {
			if (sdio_get_fun_class(func) != 0) {
				free(func);
				func = NULL;
				return (-EIO);
			}
			if (sdio_func_read_cis(func->sc->mmcbus, func, device_get_ivars(child)) != 0) {
				free(func);
				func = NULL;
				return (-EIO);
			}
			if (func->manufacturer_id == 0) {
				func->manufacturer_id = (uint16_t)mmc_get_cis_manid(child);
				func->device_id = (uint16_t)mmc_get_cis_proid(child);
			}
		}
		sc->sdio_funcs[i - 1] = func;
		if (sdio_debug) {
			device_printf(child, "SDIO card vendor=0x%x,device=0x%x\n",
				func->manufacturer_id, func->device_id);
		}
	}
	return (0);
}

static int
mmc_init_func(struct sdio_softc *sdio_sc)
{
	uint32_t ocr = mmcbr_get_ocr(sdio_sc->mmcbus);
	int ret;
	sdio_sc->sdio_func_num = (ocr >> 28) & 7;

	ret = sdio_init_funcs(sdio_sc);
	if (ret != 0) {
		sdio_delete_func(sdio_sc);
	}
	return (ret);
}

void
sdio_dettach(device_t dev)
{
	struct sdio_softc *sc = device_get_softc(dev);
	int ret = unregister_driver(sc->drv_node_name);
	if (ret != 0) {
		mmc_err("%s unregister_driver error!\n", __FUNCTION__);
	}
	clear_sdio_card_cnt(sc->drv_no);
	free(sc->buf);
	sc->buf = NULL;
	sdio_delete_func(sc);
}

int
sdio_attach(device_t dev)
{
	device_t mmcbus = NULL;
	int ret;
	int drv_num;
	struct sdio_softc *sc = device_get_softc(dev);
	uint32_t sz;
	struct sdio_func *func = NULL;

	sc->dev = dev;
	sc->mmcbus = mmcbus = device_get_parent(dev);
	sc->max_data = (uint32_t)mmc_get_max_data(dev);
	sc->rca = (uint16_t)mmc_get_rca(dev);

	MMCBUS_ACQUIRE_BUS(mmcbus, dev);
	ret = mmc_init_func(sc);
	MMCBUS_RELEASE_BUS(mmcbus, dev);
	if (ret) {
		return (-EIO);
	}

	sc->align = mmcbr_get_dma_alignment(device_get_parent(dev));
	if (sc->align > 0) { /* We don't need allocate memory in case of slot type SDHCI */
		sc->align -= 1;
		sc->buf_size = SDIO_IO_BUST;
		sc->buf = memalign(CACHE_ALIGNED_SIZE, sc->buf_size);
		if (sc->buf == NULL) {
			dev_loge(dev, "allocate memory for buffer fail!\n");
			ret = -ENOMEM;
			goto free_func;
		}
	}
	drv_num = get_sdio_card_cnt();
	if (drv_num < 0) {
		ret = -EINVAL;
		goto free_block;
	}
	sc->drv_no = (uint32_t)drv_num;

	/* Sdio don't need data */
	sz = sizeof(sc->drv_node_name);
	ret = snprintf_s(sc->drv_node_name, sz, sz - 1, "/dev/sdio%0u", sc->drv_no);
	if (ret <= 0) {
		mmc_err("snprintf_s err=%d, card_idx=%u\n", ret, sc->drv_no);
		ret = -ENOMEM;
		goto free_part_cnt;
	}

	(void)register_driver(sc->drv_node_name, NULL, 0666, sc);
	sz = (uint32_t)mmcbr_get_clock(sc->mmcbus);

	if (sdio_debug) {
		for (uint32_t i = 1; i <= sc->sdio_func_num; i++) {
			func = sc->sdio_funcs[i - 1];
			dprintf("%s-Fun%u: vendor_id 0x%04X, device_id 0x%04X, class %s,"
				" max_block_size %u bytes, %u.%01uMHz/%dbit.\n",
				sc->drv_node_name, i, func->manufacturer_id, func->device_id,
				(func->func_class < ARRAY_SIZE(sdio_class_info)) ?
				sdio_class_info[func->func_class] : "Reserved",
				func->max_blk_size, sz / 1000000, (sz / 100000) % 10,
				bus_width_value(dev));
		}
	}
	return (0);

free_part_cnt:
	clear_sdio_card_cnt(sc->drv_no);
free_block:
	free(sc->buf);
	sc->buf = NULL;
free_func:
	sdio_delete_func(sc);
	return (ret);
}
