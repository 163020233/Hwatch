/*-
 * Copyright (c) 2006 Bernd Walter.  All rights reserved.
 * Copyright (c) 2006 M. Warner Losh.  All rights reserved.
 * Copyright (c) 2017 Marius Strobl <marius@FreeBSD.org>
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *	  notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	  notice, this list of conditions and the following disclaimer in the
 *	  documentation and/or other materials provided with the distribution.
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
 *
 * Portions of this software may have been developed with reference to
 * the SD Simplified Specification.  The following disclaimer may apply:
 *
 * The following conditions apply to the release of the simplified
 * specification ("Simplified Specification") by the SD Card Association and
 * the SD Group. The Simplified Specification is a subset of the complete SD
 * Specification which is owned by the SD Card Association and the SD
 * Group. This Simplified Specification is provided on a non-confidential
 * basis subject to the disclaimers below. Any implementation of the
 * Simplified Specification may require a license from the SD Card
 * Association, SD Group, SD-3C LLC or other third parties.
 *
 * Disclaimers:
 *
 * The information contained in the Simplified Specification is presented only
 * as a standard specification for SD Cards and SD Host/Ancillary products and
 * is provided "AS-IS" without any representations or warranties of any
 * kind. No responsibility is assumed by the SD Group, SD-3C LLC or the SD
 * Card Association for any damages, any infringements of patents or other
 * right of the SD Group, SD-3C LLC, the SD Card Association or any third
 * parties, which may result from its use. No license is granted by
 * implication, estoppel or otherwise under any patent or other rights of the
 * SD Group, SD-3C LLC, the SD Card Association or any third party. Nothing
 * herein shall be construed as an obligation by the SD Group, the SD-3C LLC
 * or the SD Card Association to disclose or distribute any technical
 * information, know-how or other confidential information to any third party.
 */

#ifndef DEV_MMC_MMCSD_H
#define DEV_MMC_MMCSD_H

#include "emmc_type.h"

/* Class 4: Block oriented write commands */
#define MMC_SET_BLOCK_COUNT 23
#define MMC_WRITE_BLOCK 24
#define MMC_WRITE_MULTIPLE_BLOCK 25
#define MMC_PROGARM_CID 26
#define MMC_PROGRAM_CSD 27

#define MMCSD_TASK_STACK_SIZE 0xC00
#define MMCSD_TASK_PRIORITY 0x6

#define MMCSD_PART_NAMELEN 17

#define STATUS_REG_ERR_MASK 0xFDF92000U

/* SanDisk iNAND */
#define EXT_CSD_INAND_CMD38 113
#define EXT_CSD_INAND_CMD38_ERASE 0x00
#define EXT_CSD_INAND_CMD38_TRIM 0x01
#define EXT_CSD_INAND_CMD38_SECURE_ERASE 0x80
#define EXT_CSD_INAND_CMD38_SECURE_TRIM1 0x81
#define EXT_CSD_INAND_CMD38_SECURE_TRIM2 0x82

#define MMC_TYPE_HS_26_MAX 26000000
#define MMC_TYPE_HS_52_MAX 52000000
#define MMC_TYPE_DDR52_MAX 52000000
#define MMC_TYPE_HS200_HS400ES_MAX 200000000

/* Class 5: Erase commands */
#define SD_ERASE_WR_BLK_START 32
#define SD_ERASE_WR_BLK_END 33
/* 34 -- reserved old command */
#define MMC_ERASE_GROUP_START 35
#define MMC_ERASE_GROUP_END 36
/* 37 -- reserved old command */
#define MMC_ERASE 38
#define MMC_ERASE_ERASE 0x00000000
#define MMC_ERASE_TRIM 0x00000001
#define MMC_ERASE_FULE 0x00000002
#define MMC_ERASE_DISCARD 0x00000003
#define MMC_ERASE_SECURE_ERASE 0x80000000
#define MMC_ERASE_SECURE_TRIM1 0x80000001
#define MMC_ERASE_SECURE_TRIM2 0x80008000

void mmcsd_task_process(struct mmcsd_part *part, struct bio *bp);
int mmc_switch(device_t busdev, device_t dev, uint16_t rca, uint8_t set, uint8_t index, uint8_t value,
               unsigned int timeout, bool status);
const char *mmcsd_errmsg(int e);
int mmc_send_status(device_t busdev, device_t dev, uint16_t rca, uint32_t *status);
int mmc_wait_for_cmd(device_t busdev, device_t dev, struct mmc_command *cmd, int retries);
unsigned long mmcsd_delete(struct mmcsd_part *part, struct bio *bp);
unsigned long mmcsd_rw(struct mmcsd_part *part, struct bio *bp);
#endif
