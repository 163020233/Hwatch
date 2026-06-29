/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: implementation for ramfs extend.
 * Author: Huawei LiteOS Team
 * Create: 2022-08-06
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

#include "nuttx/fs/fs.h"
#include "ramfs_private.h"

static spinlock_t g_tmpfsAllocUnitLock;
static BOOL g_isTmpfsLockInit = FALSE;
static UINT32 g_ramfsAllocUnit = 0;
static struct tmpfs_s g_tmpfsSuperBlock = {0};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

void tmpfs_write_unit_lock_init(void)
{
    spin_lock_init(&g_tmpfsAllocUnitLock);
    g_isTmpfsLockInit = TRUE;
}

void tmpfs_write_unit_lock_deinit(void)
{
    g_isTmpfsLockInit = FALSE;
}

uint32_t tmpfs_get_alloc_unit(void)
{
    return g_ramfsAllocUnit;
}

void los_set_ramfs_unit(off_t size)
{
    if (g_isTmpfsLockInit && size >= 0) {
        spin_lock(&g_tmpfsAllocUnitLock);
        g_ramfsAllocUnit = (UINT32)size;
        spin_unlock(&g_tmpfsAllocUnitLock);
    }
}

static time_t TmpfsTimestamp(void)
{
    struct timeval tv;

    (void)gettimeofday(&tv, (struct timezone *)NULL);

    return (time_t)(tv.tv_sec);
}

void tmpfs_set_time(struct tmpfs_time_extend *time, int timeToBeSet)
{
    if (((unsigned int)timeToBeSet) & TMPFS_CTIME) {
        time->ctime = TmpfsTimestamp();
    }

    if (((unsigned int)timeToBeSet) & TMPFS_MTIME) {
        time->mtime = TmpfsTimestamp();
    }

    if (((unsigned int)timeToBeSet) & TMPFS_ATIME) {
        time->atime = TmpfsTimestamp();
    }
}

bool tmpfs_check_namelen(const char *oldRelPath, const char *newRelPath)
{
    unsigned int oldRelPathLen, newRelPathLen, cmpNameLen;

    oldRelPathLen = strlen(oldRelPath);
    newRelPathLen = strlen(newRelPath);

    cmpNameLen = (oldRelPathLen <= newRelPathLen) ? oldRelPathLen : newRelPathLen;
    if (!cmpNameLen || ((!strncmp(oldRelPath, newRelPath, cmpNameLen)) &&
            (oldRelPath[cmpNameLen] == '/' || newRelPath[cmpNameLen] == '/'))) {
        return FALSE;
    }
    return TRUE;
}

bool tmpfs_nxrmutex_init_verify(void *obj, FAR mutex_t *mutex)
{
    if (nxmutex_init(mutex) != 0) {
        PRINT_ERR("%s %d, nxmutex_init failed!\n", __FUNCTION__, __LINE__);
        kmm_free(obj);
        return FALSE;
    }
    return TRUE;
}

bool tmpfs_fpos_and_buflen_verify(struct file *filep, struct tmpfs_file_s *tfo, size_t bufLen)
{
    if (filep->f_pos >= tfo->tfo_size || bufLen == 0) {
        return FALSE;
    }
    return TRUE;
}


void tmpfs_get_file_links(struct stat *buf, struct tmpfs_file_s *tfo)
{
    buf->st_nlink = tfo->tfo_refs - 1;
}

void tmpfs_get_directory_links(struct stat *buf, struct tmpfs_directory_s *tdo)
{
    buf->st_nlink = tdo->tdo_nentries + tdo->tdo_refs;
}

struct tmpfs_s *get_tmpfs_superblock(void)
{
    return &g_tmpfsSuperBlock;
}

FSMAP_ENTRY(ramfs_fsmap, "ramfs", g_tmpfs_operations, FALSE, FALSE);
