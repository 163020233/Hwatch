/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: LiteOS MMC Sdhci Adma2 Implementation
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

/*
 * 64-bit ADMA2 Descriptor Line (32-bit Addressing Mode)
 * |---------------------------------------------------------------------------------
 * | 32-Bit Address | 16-Bit Length | 10-Bit Length |                Attribute
 * |---------------------------------------------------------------------------------
 * | 63       32    | 31         16 |  15        06 |  05    04   03   02  01   00
 * |---------------------------------------------------------------------------------
 * |   xxxx_xxxxh   | xxxxh         | xxxxxxxxxxb   |  Act2  Act1 Act0 Int End  Valid
 * |---------------------------------------------------------------------------------
 * |                |     Lower     |  Upper        |
 * |---------------------------------------------------------------------------------
 *
 * |----------------------------------------------------
 * | Valid |   Indicates Validity of a Descriptor Line |
 * | End   |   End of Descriptor                       |
 * | Int   |   Force to generate ADMA Interrupt        |
 * |----------------------------------------------------
 *
 * |----------------------------------------------------
 * | Act2 Act1 Act0  Symbol  Comment         Operation
 * |------------------------------------------------------------------------------------------
 * | 0    0    0     Nop     No Operation    Do not execute current line and go to next line
 * |------------------------------------------------------------------------------------------
 * | 0    1    0     rsv     Reserved        Same as Nop. Do not execute current line and go to next line
 * |------------------------------------------------------------------------------------------
 * | 1    0    0     Tran    Transfer Data   Transfer data of one descriptor line
 * |------------------------------------------------------------------------------------------
 * | 1    1    0     Link    Link Descriptor Link to another descriptor
 * |------------------------------------------------------------------------------------------
 */

#include "errno.h"
#include "kal_base.h"
#include "sdhci.h"
#include "adma2.h"

#define ADMA_VALID (1U << 0)
#define ADMA_END (1U << 1)
#define ADMA_INT (1U << 2)

#define ADMA_ACT0 (1U << 3)
#define ADMA_ACT1 (1U << 4)
#define ADMA_ACT2 (1U << 5)

#define ADMA_LEN_UPPER_BIT (1U << 6)           /* Make sure the ADMA_SLICE_MAX_SIZE is 64KiB */
#define ADMA_LEN_LOWER(x) (((x) & 0xffff) << 16) /* 16-Bit Length, 31~16 */
#define ADMA_LOWER_TRAN_VAL (ADMA_VALID | ADMA_ACT2)
#define ADMA_UPPER_TRAN_VAL (ADMA_VALID | ADMA_ACT2 | ADMA_LEN_UPPER_BIT)
#define ADMA_SLICE_MAX_SIZE (0x10000) /* 64K at a time maximum */
#ifdef LOSCFG_DRIVERS_MMC_HISDEMMC
#define ADMA_DESC_LINE_SZ 16
#else
#define ADMA_DESC_LINE_SZ 8
#endif
#define ADMA_BOUNCE_NSLICES 1

/*
 * dma_desc: memory address of adma2 slice
 * addr: data memory address to be transfer
 * len: data length to be transfer.
 * note: len is zero means that this slice is the end slice.
 */
static void sdhci_adma2_slice_setup(void *dma_desc, uintptr_t addr, uint32_t len)
{
    uint32_t *desc = (uint32_t *)dma_desc;
    uint32_t pay_load = len > ADMA_SLICE_MAX_SIZE ? ADMA_SLICE_MAX_SIZE : len;

    if (len & ADMA_SLICE_MAX_SIZE) {
        desc[0] = ADMA_UPPER_TRAN_VAL;
    } else if (len == 0) {
        desc[0] = ADMA_VALID | ADMA_END;
    } else {
        desc[0] = ADMA_LOWER_TRAN_VAL;
    }

    desc[0] |= ADMA_LEN_LOWER(pay_load);
    desc[1] |= ((uint32_t)addr);
}

/*
 * mark the adma2 slice the end slice.
 * instead of use the len = 0 in sdhci_adma2_slice_setup, you should check ADMA2_ATTR_MASK
 */
static inline void sdhci_adma2_end(void *dma_desc)
{
    uint32_t *desc = (uint32_t *)dma_desc;
    desc[0] |= ADMA_END;
}

static void sdmci_dma_cache_setup(void *buff, uint32_t size, uint32_t write_flag)
{
#ifdef LOSCFG_DRIVERS_MMC_DMA_CACHE_DISABLE
    (void)buff;
    (void)size;
    (void)write_flag;
#else
    if (write_flag & MMC_DATA_WRITE)
        hci_dma_clean_cache(buff, size);
    else
        hci_dma_inv_cache(buff, size);
#endif
}

static uint32_t sdhci_adma2_align_setup(void *dma_desc, aligned_buf_s *align_buf, uintptr_t addr, uint32_t len,
                                        uint32_t write_flag)
{
#define IDAMC_ADMA2_ALIGN CACHE_ALIGNED_SIZE
#define IDAMC_ADMA2_MASK (IDAMC_ADMA2_ALIGN - 1)
#define DIMAC_ADDR_ALIGN_OFF(addr) ((IDAMC_ADMA2_ALIGN - ((addr) & IDAMC_ADMA2_MASK)) & IDAMC_ADMA2_MASK)

    align_buf->unaligned_flag = 1;
    if (len <= CACHE_ALIGNED_SIZE) {
        align_buf->buf_size = len;
    } else {
        align_buf->buf_size = DIMAC_ADDR_ALIGN_OFF(addr);
    }
    if (memset_s(align_buf->ptr, align_buf->buf_size_max, 0, align_buf->buf_size_max) != EOK) {
        return 0;
    }
    if (write_flag & MMC_DATA_WRITE) {
        if (memcpy_s(align_buf->ptr, align_buf->buf_size_max, (void *)addr, align_buf->buf_size) != EOK) {
            return 0;
        }
    }

    sdhci_adma2_slice_setup(dma_desc, (uintptr_t)(align_buf->ptr), align_buf->buf_size);
    sdmci_dma_cache_setup(align_buf->ptr, align_buf->buf_size_max, write_flag);

    return align_buf->buf_size;
}

static int32_t sdhci_handle_single_sg(adma2_s *dma, const struct mmc_data *data)
{
    uint32_t size, loop, consume, flags, len;
    uint32_t unalign_len, align_len;
    uintptr_t mem, addr;
    unsigned char *desc = dma->desc;
    uint32_t len_temp = data->len;
    aligned_buf_s *align_buf;

    len = data->len;
    flags = data->flags;
    addr = (uintptr_t)data->data;

    (void)memset_s((void *)(dma->desc), dma->length, 0, dma->length);
    dma->aligned_buf_start.unaligned_flag = 0;
    dma->aligned_buf_start.buf_size = 0;
    dma->aligned_buf_end.unaligned_flag = 0;
    dma->aligned_buf_end.buf_size = 0;

    /* Use bounce slice to carry the external alignment  */
    mem = addr;
    align_buf = &dma->aligned_buf_start;
    if (addr & (CACHE_ALIGNED_SIZE - 1)) {
        size = sdhci_adma2_align_setup(desc, align_buf, addr, len, flags);

        if (size == 0) {
            return (-EINVAL);
        } else {
            desc += dma->slice;
            if (size == len) {
                goto OUT;
            }
            mem += size;
            len_temp -= size;
        }
    }

    loop = len_temp / ADMA_SLICE_MAX_SIZE;
    consume = len_temp % ADMA_SLICE_MAX_SIZE;
    unalign_len = consume & (CACHE_ALIGNED_SIZE - 1);
    align_len = consume - unalign_len;
    if (len_temp - unalign_len != 0) {
        sdmci_dma_cache_setup((void *)mem, len_temp - unalign_len, flags);
    }
    for (uint32_t i = 1; i <= loop; i += 1) {
        sdhci_adma2_slice_setup((void *)desc, mem, ADMA_SLICE_MAX_SIZE);
        mem += ADMA_SLICE_MAX_SIZE;
        desc += dma->slice;
    }

    if (align_len != 0) {
        sdhci_adma2_slice_setup((void *)desc, mem, align_len);
        desc += dma->slice;
        mem += align_len;
    }

    align_buf = &dma->aligned_buf_end;
    if (unalign_len != 0) {
        size = sdhci_adma2_align_setup(desc, align_buf, mem, unalign_len, flags);
        if (size != unalign_len) {
            return (-EINVAL);
        }
        desc += dma->slice;
    }

OUT:
    if (dma->attr & ADMA2_ATTR_MASK) {
        desc -= dma->slice;
        sdhci_adma2_end(desc);
    } else {
        sdhci_adma2_slice_setup((void *)desc, 0, 0);
    }

    hci_dma_clean_cache((void *)dma->desc, dma->length);
    return (0);
}

int sdhci_read_req_sgs_process(const struct mmc_data *data)
{
    size_t i;
    uint32_t size, align_size;
    uint32_t len = data->len;
    (void)len;

    if (!(data->flags & MMC_DATA_READ)) {
        return (0);
    }

    for (i = 0; i < data->sg_count; i++) {
        size = data->sg[i].length;
        LOS_ASSERT(len >= size);
        len -= size;
        align_size = (size + CACHE_ALIGNED_SIZE - 1) & ~((CACHE_ALIGNED_SIZE - 1));
        hci_dma_inv_cache(data->sg[i].data, align_size);
    }

    return (0);
}

static int32_t sdhci_handle_multiple_sgs(adma2_s *dma, const struct mmc_data *data)
{
    size_t i;
    uint32_t size, align_size;
    uintptr_t addr;
    unsigned char *desc = dma->desc;
    uint32_t len = data->len;
    const uint32_t chunk = ADMA_SLICE_MAX_SIZE;

    (void)memset_s((void *)(dma->desc), dma->length, 0, dma->length);

    for (i = 0; i < data->sg_count; i++) {
        size = data->sg[i].length;
        addr = (uintptr_t)data->sg[i].data;
        LOS_ASSERT(len >= size);
        len -= size;

        align_size = (size + CACHE_ALIGNED_SIZE - 1) & ~((CACHE_ALIGNED_SIZE - 1));
        sdmci_dma_cache_setup((void *)addr, align_size, data->flags);
        while (1) {
            if (size > chunk) {
                sdhci_adma2_slice_setup(desc, addr, chunk);
                size -= chunk;
                desc += dma->slice;
                addr += chunk;
            } else {
                /* One sg setup done. */
                sdhci_adma2_slice_setup(desc, addr, size);
                break;
            }
        }
        if (len) {
            desc += dma->slice;
        }
    }

    if (dma->attr & ADMA2_ATTR_MASK) {
        /* We can remark the last data slice as end slice. */
        sdhci_adma2_end(desc);
    } else {
        desc += dma->slice;
        sdhci_adma2_slice_setup((void *)desc, 0, 0);
    }

    hci_dma_clean_cache((void *)dma->desc, dma->length);
    return (0);
}

/* The same as sdhci_adma_table_pre */
int32_t sdhci_adma2_tab_setup(adma2_s *dma, const struct mmc_data *data)
{
    if (ADMA_SLICE_MAX_SIZE * dma->num_slices < data->len) {
        mmc_err("too many data(%u) request at a time!\n", data->len);
        return (-ENOMEM);
    }
    if (MMC_DATA_SG_MODE(data->flags)) {
        return sdhci_handle_multiple_sgs(dma, data);
    } else {
        return sdhci_handle_single_sg(dma, data);
    }
}

uint32_t sdhci_adma2_create(adma2_s *dma, uint32_t num_slices, uint32_t attr)
{
    uint32_t size;
    uint32_t slice;

    slice = ADMA_DESC_LINE_SZ;

    if (attr & ADMA2_ATTR_MASK) {
        size = (num_slices + ADMA_BOUNCE_NSLICES) * ADMA_DESC_LINE_SZ;
    } else {
        /* Need additional slice to carry the end */
        size = (num_slices + ADMA_BOUNCE_NSLICES + 1) * ADMA_DESC_LINE_SZ;
    }

    size = ALIGN(size, CACHE_ALIGNED_SIZE);
    dma->desc = memalign(CACHE_ALIGNED_SIZE, size);
    if (dma->desc == NULL) {
        PRINT("allocate ADMA memory failed!\n");
        return (ENOMEM);
    }

    dma->slice = slice;
    dma->num_slices = num_slices;
    dma->length = size;
    dma->attr = attr & ADMA2_ATTR_MASK;

    dma->aligned_buf_start.buf_size_max = CACHE_ALIGNED_SIZE;
    dma->aligned_buf_start.ptr = memalign(CACHE_ALIGNED_SIZE, dma->aligned_buf_start.buf_size_max);
    if (dma->aligned_buf_start.ptr == NULL) {
        free(dma->desc);
        dma->desc = NULL;
        PRINT("allocate ADMA alignedBuffer memory failed!\n");
        return (ENOMEM);
    }

    dma->aligned_buf_end.buf_size_max = CACHE_ALIGNED_SIZE;
    dma->aligned_buf_end.ptr = memalign(CACHE_ALIGNED_SIZE, dma->aligned_buf_end.buf_size_max);
    if (dma->aligned_buf_end.ptr == NULL) {
        free(dma->aligned_buf_start.ptr);
        dma->aligned_buf_start.ptr = NULL;
        free(dma->desc);
        dma->desc = NULL;
        PRINT("allocate ADMA alignedBuffer memory failed!\n");
        return (ENOMEM);
    }

    return (0);
}

void sdhci_adma2_delete(adma2_s *dma)
{
    if (dma->desc != NULL) {
        free(dma->desc);
        dma->desc = NULL;
    }
    if (dma->aligned_buf_start.ptr != NULL) {
        free(dma->aligned_buf_start.ptr);
        dma->aligned_buf_start.ptr = NULL;
    }
    if (dma->aligned_buf_end.ptr != NULL) {
        free(dma->aligned_buf_end.ptr);
        dma->aligned_buf_end.ptr = NULL;
    }
}