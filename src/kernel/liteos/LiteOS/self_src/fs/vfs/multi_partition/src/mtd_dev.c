/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2023. All rights reserved.
 * Description: LiteOS FS MTD Device (mtdblk and mtdchar) Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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

#if defined(LOSCFG_FS_YAFFS) || defined(LOSCFG_FS_JFFS) || defined(LOSCFG_FS_ROMFS) || \
    defined(LOSCFG_FS_LITTLEFS) || defined(LOSCFG_FS_FAT_FTL)

#include "fs/fs.h"
#include "linux/mtd/mtd.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"
#include "los_mux.h"
#include "mtd_partition.h"

/*
 * mtd block driver dummy interface.
 * Block device registered with this block operations will not actually support
 * file operation. The block device is only provided for file systems to mount with.
 */

static int MtdblockOpenDummy(FAR struct inode *inode)
{
    (VOID)inode;
    return 0;
}

static int MtdblockCloseDummy(FAR struct inode *inode)
{
    (VOID)inode;
    return 0;
}

static ssize_t MtdblockReadDummy(FAR struct inode *inode, FAR unsigned char *buffer,
                                 unsigned long long startSector, unsigned int nsectors)
{
    (VOID)inode;
    (VOID)buffer;
    (VOID)startSector;
    (VOID)nsectors;

    return 0;
}

static ssize_t MtdblockWriteDummy(FAR struct inode *inode, FAR const unsigned char *buffer,
                                  unsigned long long startSector, unsigned int nsectors)
{
    (VOID)inode;
    (VOID)buffer;
    (VOID)startSector;
    (VOID)nsectors;

    return 0;
}

static int MtdblockGeometryDummy(FAR struct inode *inode, FAR struct geometry *geometry)
{
    if (geometry == NULL || inode == NULL) {
        return -EINVAL;
    }

    geometry->geo_mediachanged = false;
    geometry->geo_available = true;

    /* For an MTD device, the device capacity cannot be obtained. The default value is 0. */

    geometry->geo_sectorsize = 0;
    geometry->geo_nsectors = 0;

    return 0;
}

static int MtdblockIoctlDummy(FAR struct inode *inode, int cmd, unsigned long arg)
{
    (VOID)inode;
    (VOID)cmd;
    (VOID)arg;

    return 0;
}

const struct block_operations g_mtdblock_dummy_ops = {
    .open       = MtdblockOpenDummy,
    .close      = MtdblockCloseDummy,
    .read       = MtdblockReadDummy,
    .write      = MtdblockWriteDummy,
    .geometry   = MtdblockGeometryDummy,
    .ioctl      = MtdblockIoctlDummy,
    .unlink     = NULL
};

/* mtd char driver interface  */

static int MtdcharOpen(FAR struct file *filep)
{
    mtd_partition *partition = (mtd_partition *)(filep->f_inode->i_private);
    struct mtd_info *mtd = NULL;

    /* Already opened */
    if (partition->user_num != 0) {
        return -EBUSY;
    }

    (VOID)LOS_MuxPend(partition->lock, LOS_WAIT_FOREVER);

    mtd = (struct mtd_info *)(partition->mtd_info);
    filep->f_pos = partition->start_block * mtd->erasesize;
    partition->user_num = 1;

    (VOID)LOS_MuxPost(partition->lock);

    return ENOERR;
}

static int MtdcharClose(FAR struct file *filep)
{
    mtd_partition *partition = (mtd_partition *)(filep->f_inode->i_private);

    (VOID)LOS_MuxPend(partition->lock, LOS_WAIT_FOREVER);
    partition->user_num = 0;
    (VOID)LOS_MuxPost(partition->lock);

    return ENOERR;
}

static ssize_t MtdcharRead(FAR struct file *filep, FAR char *buffer, size_t buflen)
{
    mtd_partition *partition = (mtd_partition *)(filep->f_inode->i_private);
    struct mtd_info *mtd = NULL;
    size_t startAddr, endAddr, retLen;
    ssize_t ret = 0;
    off_t pos;

    (VOID)LOS_MuxPend(partition->lock, LOS_WAIT_FOREVER);

    mtd = (struct mtd_info *)(partition->mtd_info);
    startAddr = partition->start_block * mtd->erasesize;
    endAddr = (partition->end_block + 1) * mtd->erasesize;
    pos = (off_t)filep->f_pos;

    if (buflen == 0) {
        ret = 0;
        goto OUT;
    }

    if (((size_t)pos < startAddr) || ((size_t)pos > endAddr) || (((size_t)pos + buflen) > endAddr)) {
        ret = -1;
        goto OUT;
    }

    ret = mtd->read(mtd, pos, buflen, &retLen, buffer);
    if (ret != 0) {
        goto OUT;
    }

    filep->f_pos += (loff_t)retLen;
    ret = (ssize_t)retLen;

OUT:
    (VOID)LOS_MuxPost(partition->lock);
    return ret;
}

static ssize_t MtdcharWrite(FAR struct file *filep, FAR const char *buffer, size_t buflen)
{
    mtd_partition *partition = (mtd_partition *)(filep->f_inode->i_private);
    struct mtd_info *mtd = NULL;
    size_t startAddr, endAddr, retLen;
    ssize_t ret = 0;
    off_t pos;

    (VOID)LOS_MuxPend(partition->lock, LOS_WAIT_FOREVER);

    mtd = (struct mtd_info *)(partition->mtd_info);
    startAddr = partition->start_block * mtd->erasesize;
    endAddr = (partition->end_block + 1) * mtd->erasesize;
    pos = (off_t)filep->f_pos;

    if (buflen == 0) {
        ret = 0;
        goto OUT;
    }

    if (((size_t)pos < startAddr) || ((size_t)pos > endAddr) || (((size_t)pos + buflen) > endAddr)) {
        ret = -1;
        goto OUT;
    }

    ret = mtd->write(mtd, pos, buflen, &retLen, buffer);
    if (ret != 0) {
        goto OUT;
    }

    filep->f_pos += (loff_t)retLen;
    ret = (ssize_t)retLen;

OUT:
    (VOID)LOS_MuxPost(partition->lock);
    return ret;
}

static off_t MtdcharLseek(FAR struct file *filep, off_t offset, int whence)
{
    mtd_partition *partition = (mtd_partition *)(filep->f_inode->i_private);
    struct mtd_info *mtd = NULL;
    size_t startAddr, endAddr;
    off_t ret = -EINVAL;

    (void)LOS_MuxPend(partition->lock, LOS_WAIT_FOREVER);

    mtd = (struct mtd_info *)(partition->mtd_info);
    startAddr = partition->start_block * mtd->erasesize;
    endAddr = (partition->end_block + 1) * mtd->erasesize;

    switch (whence) {
        case SEEK_SET:
            if (offset < 0 || (size_t)offset >= endAddr - startAddr) {
                goto ERR;
            }
            filep->f_pos = (loff_t)((off_t)startAddr + offset);
            break;

        case SEEK_CUR:
            if ((size_t)(filep->f_pos + offset) < startAddr ||
                (size_t)(filep->f_pos + offset) >= endAddr) {
                goto ERR;
            }
            filep->f_pos += offset;
            break;

        case SEEK_END:
            if (offset > 0 || (size_t)(-offset) > endAddr - startAddr) {
                goto ERR;
            }
            filep->f_pos = (off_t)endAddr + offset;
            break;

        default:
            goto ERR;
    }

    ret = filep->f_pos;

ERR:
    (VOID)LOS_MuxPost(partition->lock);
    return ret;
}

static int MtdcharIoctl(FAR struct file *filep, int cmd, unsigned long arg)
{
    (VOID)filep;
    (VOID)cmd;
    (VOID)arg;

    return 0;
}

const struct file_operations_vfs g_mtdchar_fops = {
    .open  = MtdcharOpen,
    .close = MtdcharClose,
    .read  = MtdcharRead,
    .write = MtdcharWrite,
    .seek  = MtdcharLseek,
    .ioctl = MtdcharIoctl,
#ifndef CONFIG_DISABLE_POLL
    .poll  = NULL,
#endif
    .unlink = NULL
};

#endif