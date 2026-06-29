/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: NonOS MMC Block Private Header File
 * Author: Huawei LiteOS Team
 * Create: 2021-08-12
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

#ifndef _DEV_MMC_BLOCK_PRIVATE_H
#define _DEV_MMC_BLOCK_PRIVATE_H

#include "sdhci.h"
#include "tcxo.h"

#define MMC_CARDBUS_BLOCK_SIZE 512

#define BIO_READ 0x01    /* Read I/O data */
#define BIO_WRITE 0x02   /* Write I/O data */
#define BIO_DELETE 0x03  /* TRIM or free blocks, i.e. mark as unused */
#define BIO_GETATTR 0x04 /* Get GEOM attributes of object */
#define BIO_FLUSH 0x05   /* Commit outstanding I/O now */
#define BIO_CMD0 0x06    /* Available for local hacks */
#define BIO_CMD1 0x07    /* Available for local hacks */
#define BIO_CMD2 0x08    /* Available for local hacks */
#define BIO_ZONE 0x09    /* Zone command */
#define BIO_SPEEDUP 0x0a /* Upper layers face shortage */

/* bio_flags */
#define BIO_ERROR 0x01   /* An error occurred processing this bio. */
#define BIO_DONE 0x02    /* This bio is finished. */
#define BIO_ONQUEUE 0x04 /* This bio is in a queue & not yet taken. */

#define MMC_READ_FLAG 1
#define MMC_WRITE_FLAG 2
#define MMC_DELETE_FLAG 3

#define atomic_read(v) LOS_AtomicRead(v)

static inline void mmc_delay_ms(uint32_t ms)
{
    uapi_tcxo_delay_ms(ms);
}

ssize_t mmcsd_exit(struct mmcsd_part *part);
void deinit_part(struct mmcsd_part *part);
int mmcsd_part_ops_register(struct mmcsd_part *part, uint32_t card_mode);

#endif