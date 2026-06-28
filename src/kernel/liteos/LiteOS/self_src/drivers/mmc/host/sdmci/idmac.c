/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: LiteOS MMC Himci Idma Implementation
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

#include "mmc_adapt.h"
#include "hci.h"
#include "hci_helper.h"
#include "mmcreg.h"
#include "sdmci.h"

/*
 * Descriptor architecture allows large blocks of
 * data transfer with minimum CPU intervention;
 * each descriptor can transfer up to 4KB of data in chained mode
 * and 8KB of data in dual-buffer mode
 */
#define	IDMAC_SLICE_MAX_SIZE	0x1000

#define	IDMAC_ALIAN_SIZE	(4)

typedef struct {
	unsigned long ctrl;
	unsigned long buf_size;
	unsigned long buf_addr;
	unsigned long next_addr;
}idmac_desc;

void
sdmci_idma_reset(const struct hci_softc *sc)
{
	hci_setl(sc, SDMMC_BMOD, SDMMC_BMOD_SWR);
	hci_setl(sc, SDMMC_CTRL, SDMMC_CTRL_RESET | SDMMC_CTRL_FIFO_RESET | SDMMC_CTRL_DMA_RESET);
	LOS_Udelay(1);
	hci_writel(sc, SDMMC_RINTSTS, ALL_INT_CLR);
}

void
sdmci_idma_start(const struct hci_softc *sc, u_char *desc, uint32_t size)
{
	hci_dma_clean_cache(desc, size);
	hci_dma_inv_cache(desc, size);
	/* Write PHY desc address to SDMMC_DBADDR */
	hci_writel(sc, SDMMC_DBADDR, (uintptr_t)(MMC_DMA_VADDR_TO_PADDR(desc)));
	hci_setl(sc, SDMMC_BMOD, SDMMC_BMOD_DE);
}

void
sdmci_idma_stop(const struct hci_softc *sc)
{
	hci_clearl(sc, SDMMC_BMOD, SDMMC_BMOD_DE);
}

static void
idamc_slice_mark_start(u_char *dma_desc)
{
	idmac_desc *id = (idmac_desc *)dma_desc;

	id->ctrl |= DES0_FS;
}

static void
idamc_slice_mark_end(u_char *dma_desc)
{
	idmac_desc *id = (idmac_desc *)dma_desc;

	id->ctrl = DES0_OWN | DES0_LD;
	id->next_addr = 0;
}

static void
idamc_slice_setup(u_char *dma_desc, const void *addr, uint32_t len)
{
	uintptr_t dma = MMC_VMM_TO_DMA_ADDR((uintptr_t)addr);
	idmac_desc *id = (idmac_desc *)dma_desc;

	id->ctrl = DES0_OWN | DES0_CH | DES0_DIC;
	/* Write DMA buffer address to des */
	id->buf_addr = dma;
	id->buf_size = len;
	/* Write Phy desc address to des */
	id->next_addr = (uintptr_t)(MMC_DMA_VADDR_TO_PADDR((void *)(id + 1)));
}

int
sdmci_sgs_idmac_finish(const idamc *dma, const struct mmc_data *data)
{
	size_t i;
	u_char *addr = NULL;
	uint32_t size, align_size;
	uint32_t len = data->len;

	if (!(dma->rw & MMC_DATA_READ)) {
		return (0);
	}

	for (i = 0; i < data->sg_count; i++) {
		size = data->sg[i].length;
		addr = (u_char *)data->sg[i].data;
		LOS_ASSERT(len >= size);
		len -= size;
		align_size = (size + CACHE_ALIGNED_SIZE - 1) & ~((CACHE_ALIGNED_SIZE - 1));
		hci_dma_inv_cache(addr, align_size);
	}

    return (0);
}

int
sdmci_idmac_finish(const idamc *dma, void *mem, uint32_t len)
{
	int status;
	uint32_t align_len = len;
	uint32_t size = len;
	uintptr_t addr = (uintptr_t)mem;

	if (!(dma->rw & MMC_DATA_READ)) {
		return (0);
	}

	if (addr & (CACHE_ALIGNED_SIZE - 1)) {
		hci_dma_inv_cache(dma->head_buff, dma->buf_size);
		status = memcpy_s((void *)addr, len, dma->head_buff, dma->head_consume);
		if (status != EOK) {
			return (-ENOMEM);
		}
		align_len = len - dma->head_consume;
		size = len - dma->head_consume;
		addr += dma->head_consume;
		if (size == 0) {
			return (0);
		}
	}

	if ((addr + size) & (CACHE_ALIGNED_SIZE - 1)) {
		align_len = size - dma->tail_consume;
		hci_dma_inv_cache(dma->tail_buff, dma->buf_size);
		status = memcpy_s((void *)(addr + align_len), dma->tail_consume, dma->tail_buff, dma->tail_consume);
		if (status != EOK) {
			return (-ENOMEM);
		}
	}
	if (align_len) {
		hci_dma_inv_cache((void *)addr, align_len);
	}

	return (0);
}

static void
sdmci_dma_cache_setup(const idamc *dma, void *buff, uint32_t size)
{
	if (dma->rw & MMC_DATA_WRITE) {
		hci_dma_clean_cache(buff, size);
	} else {
		hci_dma_inv_cache(buff, size);
	}
}

/*
 * Setup the memory which is not aligned,
 * it has to be the first IDMAC block.
 */
static uint32_t
sdmci_idamc_head_setup(idamc *dma, const void *mem, uint32_t len)
{
	int status;
	uintptr_t addr = (uintptr_t)mem;
	u_char *desc = dma->desc;

	if (len <= CACHE_ALIGNED_SIZE) {
		dma->head_consume = len;
	} else {
		dma->head_consume = DIMAC_ADDR_ALIGN_OFF(addr);
	}

	if (dma->rw & MMC_DATA_WRITE) {
		status = memcpy_s(dma->head_buff, dma->buf_size, mem, dma->head_consume);
		if (status != EOK) {
			dma->head_consume = 0;
			return (0);
		}
	}

	idamc_slice_setup(desc, dma->head_buff, dma->head_consume);
	sdmci_dma_cache_setup(dma, dma->head_buff, dma->buf_size);
	return (dma->head_consume);
}

/*
 * Setup the memory which end addr is not aligned,
 * it has to be the last IDMAC block.
 */
static uint32_t
sdmci_idamc_tail_setup(idamc *dma, u_char *dma_desc, const void *mem, uint32_t len)
{
	int status;

	dma->tail_consume = len;

	if (dma->rw & MMC_DATA_WRITE) {
		status = memcpy_s(dma->tail_buff, dma->buf_size, mem, dma->tail_consume);
		if (status != EOK) {
			dma->tail_consume = 0;
			return (0);
		}
	}

	idamc_slice_setup(dma_desc, dma->tail_buff, dma->tail_consume);
	sdmci_dma_cache_setup(dma, dma->tail_buff, dma->buf_size);
	return (dma->tail_consume);
}

/*
 * Return the gap of the DMA engine which can not process at a time.
 * IDMAC spec told us it has limits on 4 bytes alignment.
 */
uint32_t
sdmci_idmac_align_size(const idamc *dma)
{
	(void)dma;
	return (IDMAC_ALIAN_SIZE);
}

int
sdmci_handle_single_sg(idamc *dma, const struct mmc_data *data)
{
	uint32_t head_size;
	uint32_t tail_size;
	uint32_t align_size;
	uint32_t size;
	u_char *desc = dma->desc;
	u_char *addr = (u_char *)data->data;
	uint32_t len = data->len;
	const uint32_t chunk = IDMAC_SLICE_MAX_SIZE;

	dma->rw = data->flags;
	size = len;

	if ((uintptr_t)addr & (CACHE_ALIGNED_SIZE - 1)) {
		head_size = sdmci_idamc_head_setup(dma, addr, len);
		if (head_size == 0) {
			return (-EINVAL);
		} else if (head_size == len) {
			idamc_slice_mark_start(dma->desc);
			idamc_slice_mark_end(dma->desc);
			return (0);
		}

		desc += dma->slice;
		addr += head_size;
		size = len - head_size;
	}

	align_size = size & (~(CACHE_ALIGNED_SIZE - 1));
	if (align_size != 0) {
		sdmci_dma_cache_setup(dma, addr, align_size);
	}

	while (1) {
		if (size > chunk) {
			idamc_slice_setup(desc, addr, chunk);
			size -= chunk;
			desc += dma->slice;
			addr += chunk;
		} else {
			align_size = size & (~(CACHE_ALIGNED_SIZE - 1));
			tail_size = size - align_size;
			if (align_size != 0) {
				idamc_slice_setup(desc, addr, align_size);
				if (tail_size != 0) {
					desc += dma->slice;
					addr += align_size;
				}
			}
			if (tail_size != 0) {
				size = sdmci_idamc_tail_setup(dma, desc, addr, tail_size);
				if (size != tail_size) {
					return (-EINVAL);
				}
			}
			idamc_slice_mark_end(desc);
			break;
		}
	}
	idamc_slice_mark_start(dma->desc);
	return (0);
}

int
sdmci_handle_multiple_sgs(idamc *dma, const struct mmc_data *data)
{
	size_t i;
	uint32_t size, align_size;
	u_char *addr = NULL;
	u_char *desc = dma->desc;
	uint32_t len = data->len;
	const uint32_t chunk = IDMAC_SLICE_MAX_SIZE;

	dma->rw = data->flags;

	for (i = 0; i < data->sg_count; i++) {
		size = data->sg[i].length;
		addr = (u_char *)data->sg[i].data;
		LOS_ASSERT(len >= size);
		len -= size;

		align_size = (size + CACHE_ALIGNED_SIZE - 1) & ~((CACHE_ALIGNED_SIZE - 1));
		sdmci_dma_cache_setup(dma, addr, align_size);
		while (1) {
			if (size > chunk) {
				idamc_slice_setup(desc, addr, chunk);
				size -= chunk;
				desc += dma->slice;
				addr += chunk;
			} else {
				idamc_slice_setup(desc, addr, size);
				break;
			}
		}
		if (len) {
			desc += dma->slice;
		}

	}
	idamc_slice_mark_end(desc);
	idamc_slice_mark_start(dma->desc);

	return (0);
}

int
sdmci_idmac_setup(idamc *dma, const struct mmc_data *data)
{
	const uint32_t chunk = IDMAC_SLICE_MAX_SIZE;

	if (chunk * dma->nslices < data->len || data->len == 0) {
		return (-ENOMEM);
	}

	if (MMC_DATA_SG_MODE(data->flags)) {
		return sdmci_handle_multiple_sgs(dma, data);
	} else {
		return sdmci_handle_single_sg(dma, data);
	}
}

int
sdmci_idmac_create(idamc *dma, uint32_t nslices)
{
	uint32_t slice = sizeof(idmac_desc);
	uint32_t size = nslices * slice;
	uint32_t buf_size = CACHE_ALIGNED_SIZE;

	size = ALIGN(size, CACHE_ALIGNED_SIZE);
	/* Desc virtual address */
	dma->desc = (u_char *)mmc_dma_mem_alloc(NULL, size, CACHE_ALIGNED_SIZE, DMA_CACHE);
	if (dma->desc == NULL) {
		return (-ENOMEM);
	}

	dma->head_buff = memalign(CACHE_ALIGNED_SIZE, buf_size);
	if (dma->head_buff == NULL) {
		mmc_dma_mem_free(dma->desc);
		dma->desc = NULL;
		return (-ENOMEM);
	}

	(void)memset_s(dma->head_buff, buf_size, 0, buf_size);

	dma->tail_buff = memalign(CACHE_ALIGNED_SIZE, buf_size);
	if (dma->tail_buff == NULL) {
		mmc_dma_mem_free(dma->desc);
		dma->desc = NULL;
		free(dma->head_buff);
		dma->head_buff = NULL;
		return (-ENOMEM);
	}

	(void)memset_s(dma->tail_buff, buf_size, 0, buf_size);

	dma->slice = slice;
	dma->nslices = nslices;
	dma->desc_size = size;
	dma->buf_size = buf_size;
	return (0);
}

void
sdmci_idmac_delete(idamc *dma)
{
	if (dma->desc != NULL) {
		mmc_dma_mem_free(dma->desc);
		dma->desc = NULL;
	}

	if (dma->head_buff != NULL) {
		free(dma->head_buff);
		dma->head_buff = NULL;
	}

	if (dma->tail_buff != NULL) {
		free(dma->tail_buff);
		dma->tail_buff = NULL;
	}
}
