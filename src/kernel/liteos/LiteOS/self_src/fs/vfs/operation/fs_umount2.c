/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: LiteOS Fs umount2 Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2024-7-4
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
#include <sys/mount.h>
#include <errno.h>
#include <nuttx/fs/fs.h>
#include "inode/inode.h"
#include "nuttx/spinlock.h"

INT32 CloseMountPointFiles(struct inode *mountptInode)
{
    struct filelist *list = NULL;
    INT32 i = 0;
    INT32 j = 0;
    struct fs_dirent_s *dir = NULL;
    struct file *filep = NULL;
    struct inode *node = NULL;
    uint32_t flags;

    list = nxsched_get_files_from_tcb(NULL);
    flags = spin_lock_irqsave_notrace(&list->fl_lock);
    do {
        do {
            filep = &list->fl_files[i][j];
            /* The opened directory. */
            if (((UINT32)filep->f_oflags & O_DIRECTORY) != 0) {
                dir = (struct fs_dirent_s *)filep->f_priv;
                node = dir != NULL ? dir->fd_root : NULL;
            } else { /* The opened file. */
                node = filep->f_inode;
            }

            if (node == mountptInode) {
                spin_unlock_irqrestore_notrace(&list->fl_lock, flags);
                (void)fsync(i * CONFIG_NFILE_DESCRIPTORS_PER_BLOCK + j);
                flags = spin_lock_irqsave_notrace(&list->fl_lock);
                inode_release(node);
            }
        } while (++j < CONFIG_NFILE_DESCRIPTORS_PER_BLOCK);
        j = 0;
    } while (++i < list->fl_rows);
    spin_unlock_irqrestore_notrace(&list->fl_lock, flags);
    return LOS_OK;
}

INT32 umount2(const CHAR *target, INT32 flags)
{
    struct inode *mountptInode = NULL;
    struct inode_search_s desc;
    char *fullpath = NULL;
    INT32 ret;

    ret = vfs_normalize_path((const char *)NULL, target, &fullpath);
    if (ret < 0) {
        set_errno(-ret);
        return ERROR;
    }

    SETUP_SEARCH(&desc, fullpath, false);
    ret = inode_find(&desc);
    if (ret < 0) {
        set_errno(ENOENT);
        ret = ERROR;
        goto errout_with_search;
    }

    mountptInode = desc.node;
    if (((UINT32)flags & MNT_FORCE) != 0) {
        ret = CloseMountPointFiles(mountptInode);
        if (ret != LOS_OK) {
            inode_release(mountptInode);
            set_errno(ENOENT);
            ret = ERROR;
            goto errout_with_search;
        }
    } else if (flags != 0) {
        inode_release(mountptInode);
        set_errno(EINVAL);
        ret = ERROR;
        goto errout_with_search;
    }
    inode_release(mountptInode);
    ret = umount(target);

errout_with_search:
    RELEASE_SEARCH(&desc);
    vfs_release_fullpath(fullpath);
    return ret;
}
