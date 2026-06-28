/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: implementation for bch extend.
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

#include "bch_private.h"
#include "fs/fs.h"
#include "bch.h"
#include "disk.h"
#include "disk_pri.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

int bchlib_extend_setup(FAR struct bchlib_s *bch)
{
    los_part *part = NULL;
    FAR struct BchPrivate *private = NULL;

    bch->dirty    = FALSE;
    bch->unlinked = FALSE;

    part = los_part_find(bch->inode);
    if (part == NULL) {
        ferr("ERROR: los_part_find failed\n");
        return -ENODEV;
    }

    bch->nsectors = (blkcnt_t)part->sector_count;

    private = (FAR struct BchPrivate *)bch->extdata;
    private->sectStart = (blkcnt_t)part->sector_start;
    private->disk = get_disk(part->disk_id);

    return 0;
}

int bch_extend_write(FAR struct inode *inode, FAR struct bchlib_s *bch, FAR uint8_t *buffer,
                     blkcnt_t sectstart, unsigned int nsectors)
{
    (VOID)inode;
    FAR struct BchPrivate *private = (FAR struct BchPrivate *)bch->extdata;

    return los_disk_write(private->disk->disk_id, buffer, (UINT64)(sectstart + private->sectStart), nsectors);
}

int bch_extend_read(FAR struct inode *inode, FAR struct bchlib_s *bch, FAR uint8_t *buffer,
                    blkcnt_t sectstart, unsigned int nsectors)
{
    (VOID)inode;
    FAR struct BchPrivate *private = (FAR struct BchPrivate *)bch->extdata;

    return los_disk_read(private->disk->disk_id, buffer, (UINT64)(sectstart + private->sectStart),
                         nsectors);
}

int bch_extend_ioctl(FAR struct inode *inode, FAR struct bchlib_s *bch,
                     int cmd, unsigned long arg)
{
    int ret = -ENOTTY;
    los_disk *disk = NULL;
    FAR struct BchPrivate *private = (FAR struct BchPrivate *)bch->extdata;

    disk = private->disk;
    if (disk == NULL) {
        return -ENODEV;
    }

    DISK_LOCK(disk);
    if (disk->disk_status == STAT_INUSED) {
        ret = inode->u.i_bops->ioctl(inode, cmd, arg);
    }
    DISK_UNLOCK(disk);

    return ret;
}
