/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Dentry Implementation
 * Author: Huawei LiteOS Team
 * Create: 2021-7-15
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

#include "dentry.h"
#include "limits.h"
#include "fs/fs.h"
#include "hash.h"

static HashTable *DentryHashTableGet(const LosDentry *dentry)
{
    /* Find the root dentry */
    while (!HASH_ROOT_PARENT_DENTRY(dentry)) {
        dentry = dentry->parent;
    }

    return HASH_TABLE_GET(dentry);
}

static INT32 DentryNameSet(LosDentry *dentry, const CHAR *name)
{
    CHAR *dst = NULL;
    UINT32 len;
    UINT32 nameLen;

    nameLen = strlen(name) + 1; /* 1: for terminated '\0' */
    if (nameLen <= DENTRY_SHORT_NAME_LEN) {
        dentry->autoName = NULL;
        dst = dentry->name;
        len = DENTRY_SHORT_NAME_LEN;
    } else {
        dentry->autoName = malloc(nameLen);
        if (dentry->autoName == NULL) {
            return -ENOMEM;
        }
        dst = dentry->autoName;
        len = nameLen;
    }

    if (memcpy_s(dst, len, name, nameLen) != EOK) {
        return -EINVAL;
    }

    return OK;
}

static INT32 DentryNameFree(VOID *data)
{
    LosDentry *dentry = (LosDentry *)data;

    if (dentry->autoName != NULL) {
        free(dentry->autoName);
        dentry->autoName = NULL;
    }

    return OK;
}

static const CHAR *DentryNameGet(const LosDentry *dentry)
{
    return (dentry->autoName == NULL) ? dentry->name : dentry->autoName;
}

static UINT32 DentryHashCaculate(const VOID *data)
{
    LosDentry *dentry = (LosDentry *)data;
    const CHAR *name = DentryNameGet(dentry);
    UINT32 len = strlen(name);
    UINT32 hash, i;

    /*
     * Use the parent directory as the SEED, so that we can handle
     * same file name in different directories.
     */
    hash = (UINT32)dentry->parent;

    for (i = 0; i < len; i++) {
        hash = 13 * hash + name[i]; /* 13: chosen odd prime, for diversity */
    }

    /* Get the remainder */
    hash &= ((1 << DENTRY_TABLE_SHIFT) - 1);

    return hash;
}

BOOL DentryHashCompare(const VOID *src, const VOID *dst)
{
    LosDentry *srcDentry = (LosDentry *)src;
    LosDentry *dstDentry = (LosDentry *)dst;

    if ((srcDentry == NULL) || (dstDentry == NULL)) {
        return FALSE;
    }

    /* Different directory */
    if (srcDentry->parent != dstDentry->parent) {
        return FALSE;
    }

    return (strcmp(DentryNameGet(srcDentry), DentryNameGet(dstDentry)) == 0) ? TRUE : FALSE;
}

LosDentry *DentryLookup(LosDentry *parent, CHAR *name)
{
    HashTable *table = NULL;
    LosDentry srcDentry;

    if ((parent == NULL) || (name == NULL)) {
        return NULL;
    }

    table = DentryHashTableGet(parent);

    /*
     * Construct a dentry with parent and name which is the seed and string for the hash function.
     * This will ensure that the dentry can be found if it's in hash table.
     */
    srcDentry.parent = parent;
    srcDentry.autoName = name;

    return (LosDentry *)HashLookup(table, &srcDentry);
}

LosDentry *DentryFind(LosDentry *root, const char *path)
{
    LosDentry *parent = NULL;
    LosDentry *dentry = NULL;
    char name[PATH_MAX];
    const char *n = path;
    int nameLen = 0;

    if ((root == NULL) || (path == NULL) || (*path == '\0')) {
        return NULL;
    }

    parent = root;
    for (;;) {
        /* Check if it is really a directory. */
        if (!S_ISDIR(parent->mode)) {
            return NULL;
        }

        /* Isolate the file/dir name out of the path. */
        while ((*n != '\0') && (*n != '/')) {
            name[nameLen] = *n;
            nameLen++;
            n++;
        }

        name[nameLen] = '\0';
        dentry = DentryLookup(parent, name);
        if (dentry == NULL) {
            return NULL;
        }

        while (*n == '/') {
            n++;
        }

        /* End of path. Return the dentry */
        if (*n == '\0') {
            return dentry;
        }

        nameLen = 0;
        parent = dentry;
    }

    return NULL;
}

LosDentry *DentryCreate(LosDentry *parent, CHAR *name)
{
    HashTable *table;
    LosDentry *dentry;

    if ((parent == NULL) || (name == NULL)) {
        return NULL;
    }

    if (DentryLookup(parent, name) != NULL) {
        /* Already existed */
        return NULL;
    }

    table = DentryHashTableGet(parent);
    dentry = (LosDentry *)HashAlloc(table);
    if (dentry == NULL) {
        return NULL;
    }

    if (DentryNameSet(dentry, name) != OK) {
        return NULL;
    }

    dentry->parent = parent;
    (VOID)HashAdd(table, dentry);

    return dentry;
}

VOID DentryDestroy(LosDentry *dentry)
{
    HashTable *table = NULL;

    if (dentry == NULL) {
        return;
    }

    table = DentryHashTableGet(dentry);
    (VOID)DentryNameFree(dentry);
    HashDelete(table, dentry);
}

LosDentry *DentryInit(CHAR *rootName)
{
    HashTable *table = NULL;
    LosDentry *rootDentry = NULL;
    UINTPTR rootParent;

    if (rootName == NULL) {
        return NULL;
    }

    HashParam param = {
        .entrySize = sizeof(LosDentry),
        .tableShift = DENTRY_TABLE_SHIFT,
        .entryShift = DENTRY_ENTRY_SHIFT,
        .hashCompare = DentryHashCompare,
        .hashCaculate = DentryHashCaculate
    };

    table = HashInit(&param);
    if (table == NULL) {
        return NULL;
    }

    /*
     * Since root has no parent, we use root's parent to store the hash table.
     * For sub directory, we will search the parent all the way up until we find
     * the hash table. There is also consideration for space of structure LosDentry.
     */
    rootParent = (UINTPTR)table | HASH_TABLE_FLAG;
    rootDentry = DentryCreate((LosDentry *)rootParent, rootName);
    if (rootDentry == NULL) {
        HashDeinit(table);
    }

    return rootDentry;
}

VOID DentryDeinit(const LosDentry *root)
{
    HashTable *table = NULL;

    if (root == NULL) {
        return;
    }

    table = DentryHashTableGet(root);
    (void)HashIterate(table, DentryNameFree);
    HashDeinit(table);
}

VOID DentryStatFill(const LosDentry *dentry, struct stat *buf)
{
    if ((dentry == NULL) || (buf == NULL)) {
        return;
    }
    buf->st_dev = 0;
    buf->st_size = dentry->size;
    buf->st_mode = dentry->mode;
    buf->st_blksize = dentry->blkSize;
    buf->st_blocks = (dentry->size + dentry->blkSize - 1) / dentry->blkSize;
    buf->st_atim = dentry->atime;
    buf->st_mtim = dentry->mtime;
    buf->st_ctim = dentry->ctime;
}

VOID DentryTimeUpdate(LosDentry *dentry, const struct timespec *ts, unsigned int flags)
{
    if ((dentry == NULL) || (ts == NULL)) {
        return;
    }

    if (flags & ATIME) {
        dentry->atime = *ts;
    }

    if (flags & MTIME) {
        dentry->mtime = *ts;
    }

    if (flags & CTIME) {
        dentry->ctime = *ts;
    }
}

VOID DentrySizeUpdate(LosDentry *dentry, unsigned long size)
{
    if (dentry == NULL) {
        return;
    }

    dentry->size = size;
}

VOID DentryBlockSizeUpdate(LosDentry *dentry, unsigned long blkSize)
{
    if (dentry == NULL) {
        return;
    }

    dentry->blkSize = blkSize;
}
