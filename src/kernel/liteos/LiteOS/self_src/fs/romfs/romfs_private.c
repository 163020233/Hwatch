/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: implementation for romfs extend.
 * Author: Huawei LiteOS Team
 * Create: 2022-07-15
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "romfs_private.h"
#include "fs_romfs.h"
#include "nuttx/config.h"
#include "nuttx/mtd/mtd.h"
#include "nuttx/fs/fs.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

#ifdef ROMFS_MOUNT_ON_FLASH
ssize_t romfs_extend_read(const struct romfs_mountpt_s *rm, uint8_t *buffer,
                          uint32_t sector, unsigned int nsectors)
{
    ssize_t ret = -ENODEV;
    size_t readLen;
    struct RomfsPrivate *priv = NULL;
    uint32_t flashOffset;

    priv = (struct RomfsPrivate *)(rm->extdata);
    flashOffset = SEC_OFFSET(priv, rm, sector);
    if (priv->mtd == NULL) {
        ferr("rm->mtd is NULL\n");
        return ret;
    }

    ret = priv->mtd->read(priv->mtd, (unsigned long)flashOffset,
                          nsectors * rm->rm_hwsectorsize, &readLen, (char *)buffer);
    if (ret) {
        ferr("Error: romfs_mtd_read failed \n");
        return ret;
    }
    return nsectors;
}

INT32 romfs_extend_hwconfigure(struct romfs_mountpt_s *rm, struct inode *inode)
{
    struct RomfsPrivate *priv = NULL;
    mtd_partition *p = NULL;

    p = (mtd_partition *)inode->i_private;
    priv = (struct RomfsPrivate *)(rm->extdata);

    priv->mtd = (struct mtd_info *)(p->mtd_info);
    priv->startAddr = p->start_block * priv->mtd->erasesize;
    rm->rm_hwsectorsize = FLASH_SECTOR_SIZE;
    rm->rm_hwnsectors = (p->end_block - p->start_block + 1) * priv->mtd->erasesize / FLASH_SECTOR_SIZE;

    return OK;
}

void romfs_extend_free(struct inode *inode)
{
    mtd_partition *part = NULL;

    part = (mtd_partition *)inode->i_private;
    if (part != NULL) {
        free(part->mountpoint_name);
        part->mountpoint_name = NULL;
    }
}

FSMAP_ENTRY(romfs_fsmap, "romfs", g_romfs_operations, TRUE, TRUE);

#else

ssize_t romfs_io_read(int pt, uint32_t sector, uint32_t count, void *data)
{
    ssize_t ret;
    los_part *part = NULL;

    part = get_part(pt);
    if (part == NULL || part->dev == NULL) {
        return -ENODEV;
    }
    sector += part->sector_start;

    ret = los_part_read(pt, data, sector, count);
    if (ret < 0) {
        ferr("ERROR:romfs_io_read failed\n");
        return ret;
    }

    return count;
}

ssize_t romfs_extend_read(const struct romfs_mountpt_s *rm, uint8_t *buffer,
                          uint32_t sector, unsigned int nsectors)
{
    ssize_t nsectorsRead;
    struct RomfsPrivate *priv = NULL;

    priv = (struct RomfsPrivate *)(rm->extdata);
    nsectorsRead = romfs_io_read(priv->part->part_id, sector, nsectors, buffer);

    return nsectorsRead;
}

INT32 romfs_extend_hwconfigure(struct romfs_mountpt_s *rm, struct inode *inode)
{
    struct RomfsPrivate *priv = NULL;
    los_part *part = NULL;

    priv = (struct RomfsPrivate *)(rm->extdata);
    part = los_part_find(inode);
    if (part == NULL) {
        return -ENODEV;
    }
    priv->part = part;

    return OK;
}

void romfs_extend_free(struct inode *inode) {}

FSMAP_ENTRY(romfs_fsmap, "romfs", g_romfs_operations, FALSE, TRUE);

#endif
