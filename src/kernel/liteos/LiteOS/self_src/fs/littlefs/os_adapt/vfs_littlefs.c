/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: LiteOS FS Littlefs Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2020-12-30
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

#include "vfs_littlefs.h"
#include "los_typedef.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "stdlib.h"
#include "los_mux.h"
#include "sys/statfs.h"
#include "fs/fs.h"
#include "los_fs.h"
#include "los_tables.h"
#include "los_atomic.h"
#include "linux/mtd/mtd.h"
#include "linux/mtd/mtd_list.h"
#include "lfs.h"
#include "mtd_partition.h"

#ifdef LOSCFG_FS_LITTLEFS

struct mtd_info *lfs_mtd = NULL;
struct inode *lfs_block[CONFIG_MTD_PATTITION_NUM];
static const char lfs_root_path[] = ".";

struct lfs_table {
    struct lfs_config cfg;
    lfs_t *fs;
    lfs_size_t mutex;      /* Mutex for thread safe */
    lfs_size_t start_addr; /* Start address of the littlefs mtd partition */
};

struct lfs_table *lfs_part[CONFIG_MTD_PATTITION_NUM];

static int ret_to_errno(int result) {
    int err = 0;
    switch (result) {
        case LFS_ERR_OK:
            err = ENOERR;
            break;
        case LFS_ERR_IO:
            err = -EIO;
            break;
        case LFS_ERR_NOENT:
            err = -ENOENT;
            break;
        case LFS_ERR_EXIST:
            err = -EEXIST;
            break;
        case LFS_ERR_NOTDIR:
            err = -ENOTDIR;
            break;
        case LFS_ERR_ISDIR:
            err = -EISDIR;
            break;
        case LFS_ERR_NOTEMPTY:
            err = -ENOTEMPTY;
            break;
        case LFS_ERR_BADF:
            err = -EBADF;
            break;
        case LFS_ERR_INVAL:
            err = -EINVAL;
            break;
        case LFS_ERR_NOSPC:
            err = -ENOSPC;
            break;
        case LFS_ERR_NOMEM:
            err = -ENOMEM;
            break;
        case LFS_ERR_CORRUPT:
            err = -ELIBBAD;
            break;
        default:
            err = -EIO;
            break;
    }
    return err;
}

static const char *lfs_path_convert(const char *relpath)
{
    return (relpath[0] == '\0') ? lfs_root_path : relpath;    // littlefs2.11版本已经不允许传入空字符串""来作为根目录"/"的别名
}

static int lfs_mutex_take(const struct lfs_config *c) {
    struct lfs_table *part = (struct lfs_table *)c->context;
    if (!FS_OPERATE_IN_EXC) {
        if (LOS_MuxPend(part->mutex, (uint32_t)LFS_WAITING_FOREVER)) {
            PRINT_ERR("lfs_mutex_take LOS_MuxPend failed\n");
            return -1;
        }
    }
    return 0;
}

static int lfs_mutex_release(const struct lfs_config *c) {
    struct lfs_table *part = (struct lfs_table *)c->context;
    if (!FS_OPERATE_IN_EXC) {
        if (LOS_MuxPost(part->mutex)) {
            PRINT_ERR("lfs_mutex_release LOS_MuxPost failed\n");
            return -1;
        }
    }
    return 0;
}

static int lfs_mutex_create(struct lfs_config *c) {
    struct lfs_table *part = (struct lfs_table *)c->context;

    if (LOS_MuxCreate(&part->mutex) != LOS_OK) {
        PRINT_ERR("lfs_mutex_create LOS_MuxCreate failed\n");
        return -1;
    }
    return 0;
}

static void lfs_mutex_delete(struct lfs_config *c) {
    struct lfs_table *part = (struct lfs_table *)c->context;

    (void)LOS_MuxDelete(part->mutex);
}

static int lfs_flash_read(const struct lfs_config *cfg, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size) {
    int ret;
    uint64_t addr;
    lfs_size_t realsize;
    struct lfs_table *part = (struct lfs_table *)cfg->context;

    if (buffer == NULL || size == 0) {
        return -EINVAL;
    }
    addr = part->start_addr + block * cfg->block_size + off;
    ret = lfs_mtd->read(lfs_mtd, addr, size, &realsize, (char *)buffer);
    if (ret || realsize != size) {
        return -1;
    }
    return LFS_ERR_OK;
}

static int lfs_flash_write(const struct lfs_config *cfg, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size) {
    int ret;
    uint64_t addr;
    lfs_size_t realsize;
    struct lfs_table *part = (struct lfs_table *)cfg->context;

    if (buffer == NULL || size == 0) {
        return -EINVAL;
    }
    addr = part->start_addr + block * cfg->block_size + off;
    ret = lfs_mtd->write(lfs_mtd, addr, size, &realsize, buffer);
    if (ret || realsize != size) {
        return -1;
    }

    return LFS_ERR_OK;
}

static int lfs_flash_erase(const struct lfs_config *cfg, lfs_block_t block) {
    int ret;
    struct erase_info instr = {0};
    struct lfs_table *part = (struct lfs_table *)cfg->context;

    instr.mtd = lfs_mtd;
    instr.addr = part->start_addr + block * cfg->block_size;
    instr.len = cfg->block_size;

    ret = lfs_mtd->erase(lfs_mtd, &instr);
    if (ret) {
        return -1;
    }

    return LFS_ERR_OK;
}

static int lfs_flash_sync(const struct lfs_config *cfg) {
    (void)cfg;
    return LFS_ERR_OK;
}

static void lfs_config_init(struct lfs_table *part, const mtd_partition *p) {
    uint32_t block_count = p->end_block - p->start_block + 1;
    struct lfs_config *cfg = &part->cfg;

    part->start_addr = p->start_block * lfs_mtd->erasesize;
    cfg->block_size = lfs_mtd->erasesize;
    cfg->block_count = block_count;
    cfg->context = part;                 // for acquiring member of struct lfs_table
    cfg->read  = lfs_flash_read;         // read data form spi flash
    cfg->prog  = lfs_flash_write;        // write data to spi flash
    cfg->erase = lfs_flash_erase;        // erase by per
    cfg->sync  = lfs_flash_sync;
    cfg->lock  = lfs_mutex_take;
    cfg->unlock = lfs_mutex_release;
    cfg->read_size = lfs_mtd->writesize;
    cfg->prog_size = lfs_mtd->writesize;
    cfg->cache_size = LFS_CACHE_SIZE;
    cfg->lookahead_size = LFS_LOOKAHEAD_SIZE;
    cfg->block_cycles = LFS_BLOCK_CYCLES;
}

static void lfs_partition_free(struct lfs_table *part) {
    free(part->fs);
    part->fs = NULL;
    free(part);
}

static int vfs_lfs_bind(FAR struct inode *blkdriver,
        FAR const void *data, FAR void **handle,
        FAR const char *relpath) {
    int ret = LFS_ERR_OK;

    mtd_partition *p = (mtd_partition *)blkdriver->i_private;
    if (p == NULL) {
        return -EPERM;
    }
    uint32_t partition_num = p->patitionnum;

    lfs_mtd = (struct mtd_info *)(p->mtd_info);
    if (lfs_mtd == NULL) {
        return -EPERM;
    }
    if (lfs_mtd->type != MTD_NORFLASH) {
        return -EINVAL;
    }

    lfs_part[partition_num] = (struct lfs_table *)malloc(sizeof(struct lfs_table));
    if (lfs_part[partition_num] == NULL) {
        return -ENOMEM;
    }
    lfs_part[partition_num]->fs = (lfs_t *)malloc(sizeof(lfs_t));
    if (lfs_part[partition_num]->fs == NULL) {
        free(lfs_part[partition_num]);
        lfs_part[partition_num] = NULL;
        return -ENOMEM;
    }
    memset(&lfs_part[partition_num]->cfg, 0, sizeof(struct lfs_config));
    lfs_config_init(lfs_part[partition_num], p);

    if (lfs_mutex_create(&lfs_part[partition_num]->cfg) != 0) {
        ret = -EINVAL;
        goto err_free;
    }

    memset(lfs_part[partition_num]->fs, 0, sizeof(lfs_t));
    ret = lfs_mount(lfs_part[partition_num]->fs, &lfs_part[partition_num]->cfg);
    if (ret == LFS_ERR_CORRUPT) {
        PRINT_INFO("formatting littlefs...\n");
        ret = lfs_format(lfs_part[partition_num]->fs, &lfs_part[partition_num]->cfg);
        if (ret != LFS_ERR_OK) {
            PRINT_ERR("littlefs format failed! %d\n", ret);
            goto err;
        }
        ret = lfs_mount(lfs_part[partition_num]->fs, &lfs_part[partition_num]->cfg);
    }
    if (ret != LFS_ERR_OK) {
        PRINT_ERR("littlefs mount failed! %d\n", ret);
        goto err;
    }

    PRINT_INFO("littlefs mount done!\n");

    *handle = (void *)p;
    lfs_block[partition_num] = blkdriver;
    return LFS_ERR_OK;

err:
    lfs_mutex_delete(&lfs_part[partition_num]->cfg);
err_free:
    lfs_partition_free(lfs_part[partition_num]);
    lfs_part[partition_num] = NULL;
    return ret_to_errno(ret);
}

static int vfs_lfs_unbind(FAR void *handle, FAR struct inode **blkdriver) {
    int ret;
    mtd_partition *p = (mtd_partition *)handle;
    if (p == NULL) {
        return -EINVAL;
    }
    uint32_t partition_num = p->patitionnum;
    if (lfs_part[partition_num] == NULL || lfs_part[partition_num]->fs == NULL) {
        return -EINVAL;
    }

    ret = lfs_unmount(lfs_part[partition_num]->fs);
    if (ret < 0) {
        PRINT_ERR("littlefs_unmount error, %d\n", ret);
        return ret_to_errno(ret);
    }

    lfs_mutex_delete(&lfs_part[partition_num]->cfg);

    lfs_partition_free(lfs_part[partition_num]);
    lfs_part[partition_num] = NULL;
    free(p->mountpoint_name);
    p->mountpoint_name = NULL;

    *blkdriver = lfs_block[partition_num];
    return LFS_ERR_OK;
}

#define LITTLEFS_SUPER_MAGIC 0x0a732923
static int vfs_lfs_statfs(FAR struct inode *mountpt, FAR struct statfs *buf)
{
    mtd_partition *p = (mtd_partition *)mountpt->i_private;
    memset_s(buf, sizeof(*buf), 0, sizeof(*buf));
    buf->f_type = LITTLEFS_SUPER_MAGIC;
    buf->f_namelen = LFS_NAME_MAX;
    buf->f_bsize = lfs_mtd->erasesize;
    buf->f_blocks = p->end_block + 1 - p->start_block;
    uint32_t used_blocks = lfs_fs_size(lfs_part[p->patitionnum]->fs);
    buf->f_bavail = buf->f_bfree = buf->f_blocks - used_blocks;
    return 0;
}

static int lfs_flags_get(int oflags) {
    int flags = 0;
    switch (oflags & O_ACCMODE) {
        case O_RDONLY:
            flags |= LFS_O_RDONLY;
            break;
        case O_WRONLY:
            flags |= LFS_O_WRONLY;
            break;
        case O_RDWR:
            flags |= LFS_O_RDWR;
            break;
        default:
            break;
    }

    if (oflags & O_CREAT) {
        flags |= LFS_O_CREAT;
    }

    if ((oflags & O_EXCL)) {
        flags |= LFS_O_EXCL;
    }

    if (oflags & O_TRUNC) {
        flags |= LFS_O_TRUNC;
    }

    if (oflags & O_APPEND) {
        flags |= LFS_O_APPEND;
    }

    return flags;
}

static Atomic s_open_handles_count = 0;
static int vfs_lfs_open(FAR struct file *filep,
        FAR const char *relpath,
        int oflags, mode_t mode) {
    int ret;
    const char *name = NULL;
    lfs_file_t *file = NULL;

    mtd_partition *p = (mtd_partition *)filep->f_inode->i_private;

    if (LOS_AtomicRead(&s_open_handles_count) >= LFS_MAX_OPEN_HANDLES) {
        PRINT_ERR("open_handles=%d over limit.\n", s_open_handles_count);
        return -EMFILE;
    }

    name = relpath;
    if (name[0] == '/' && name[1] == 0) {
        name = lfs_root_path;
    } else if (name[0] == '/') {
        name++;
    }
    if (filep->f_inode->mountflags == MS_RDONLY && oflags != O_RDONLY) {
        return -EACCES;
    }

    if (oflags & O_DIRECTORY) {
        return -EACCES;
    }

    file = (lfs_file_t *)malloc(sizeof(lfs_file_t));
    if (file == NULL) {
        return -ENOMEM;
    }

    ret = lfs_file_open(lfs_part[p->patitionnum]->fs,
            file, name, lfs_flags_get(oflags));
    if (ret) {
        free(file);
        return ret_to_errno(ret);
    }

    LOS_AtomicInc(&s_open_handles_count);

    // save this pointer, it will be used when calling read(),write(),
    // flush(), lseek(), and will be free when calling close()
    filep->f_priv = file;
    filep->f_pos = file->pos;
    return LFS_ERR_OK;
}


static int vfs_lfs_close(FAR struct file *filep) {
    int ret;
    lfs_file_t *file = NULL;
    mtd_partition *p = (mtd_partition *)filep->f_inode->i_private;

    LFS_IS_NOT_NULL(filep->f_priv != NULL);
    file = (lfs_file_t *)(filep->f_priv);

    ret = lfs_file_close(lfs_part[p->patitionnum]->fs, file);
    if (ret) {
        return ret_to_errno(ret);
    }

    LOS_AtomicDec(&s_open_handles_count);

    free(file);
    filep->f_priv = NULL;
    return LFS_ERR_OK;
}

static ssize_t vfs_lfs_read(FAR struct file *filep,
        FAR char *buffer, size_t buflen) {
    lfs_ssize_t readsize;
    lfs_file_t *file = NULL;
    mtd_partition *p = (mtd_partition *)filep->f_inode->i_private;

    LFS_IS_NOT_NULL(filep->f_priv != NULL);
    file = (lfs_file_t *)(filep->f_priv);

    readsize = lfs_file_read(lfs_part[p->patitionnum]->fs,
            file, (void *)buffer, (lfs_size_t)buflen);
    if (readsize < 0) {
        return ret_to_errno(readsize);
    }

    filep->f_pos= (loff_t)file->pos;
    return (ssize_t)readsize;
}

static ssize_t vfs_lfs_write(FAR struct file *filep,
        FAR const char *buffer,
        size_t buflen) {
    lfs_file_t *file = NULL;
    lfs_ssize_t writesize;
    mtd_partition *p = (mtd_partition *)filep->f_inode->i_private;

    if (filep->f_inode->mountflags == MS_RDONLY) {
        return -EACCES;
    }

    LFS_IS_NOT_NULL(filep->f_priv != NULL);
    file = (lfs_file_t *)(filep->f_priv);

    writesize = lfs_file_write(lfs_part[p->patitionnum]->fs,
            file, (const void *)buffer, (lfs_size_t)buflen);

    if (writesize < 0) {
        return ret_to_errno(writesize);
    }
    filep->f_pos = (loff_t)file->pos;
    return (ssize_t)writesize;
}

static off_t vfs_lfs_seek(FAR struct file *filep, off_t offset, int whence) {
    lfs_file_t *file = NULL;
    lfs_soff_t off;
    LFS_IS_NOT_NULL(filep->f_priv != NULL);
    file = (lfs_file_t *)(filep->f_priv);
    mtd_partition *p = (mtd_partition *)filep->f_inode->i_private;

    off = lfs_file_seek(lfs_part[p->patitionnum]->fs, file, (lfs_soff_t)offset, whence);
    if (off < 0) {
        return ret_to_errno(off);
    }
    // update file position
    filep->f_pos = (loff_t)off;
    return (off_t)off;
}

static int vfs_lfs_sync(FAR struct file *filep) {
    lfs_file_t *file = (lfs_file_t *)(filep->f_priv);
    mtd_partition *p = (mtd_partition *)filep->f_inode->i_private;
    return lfs_file_sync(lfs_part[p->patitionnum]->fs, file);
}

static loff_t vfs_lfs_seek64(FAR struct file *filep,
        loff_t offset, int whence) {
    return (loff_t)vfs_lfs_seek(filep, (off_t)offset, whence);
}

static int vfs_lfs_opendir(FAR struct inode *mountpt,
        FAR const char *relpath,
        FAR struct fs_dirent_s **dir) {
    int ret;
    lfs_dir_t *lfs_d = NULL;
    mtd_partition *p = (mtd_partition *)mountpt->i_private;

    lfs_d = (lfs_dir_t *)malloc(sizeof(lfs_dir_t));
    if (lfs_d == NULL) {
        return -ENOMEM;
    }
    if (relpath[0] == '/' && relpath[1] == 0) {
        relpath = lfs_root_path;
    } else if (relpath[0] == '/') {
        relpath++;
    }

    *dir = malloc(sizeof(struct fs_dirent_s));
    if (*dir == NULL) {
        free(lfs_d);
        return -ENOMEM;
    }

    // open dir
    ret = lfs_dir_open(lfs_part[p->patitionnum]->fs, lfs_d, lfs_path_convert(relpath));
    if (ret) {
        free(*dir);
        free(lfs_d);
        return ret_to_errno(ret);
    }

    lfs_d->pos = LFS_FILE_OFFSET;
    (*dir)->fd_data = lfs_d;
    return LFS_ERR_OK;
}

static int vfs_lfs_closedir(FAR struct inode *mountpt,
        FAR struct fs_dirent_s *dir) {
    int ret;
    lfs_dir_t *lfs_d = NULL;

    LFS_IS_NOT_NULL(dir->fd_data != NULL);
    lfs_d = (lfs_dir_t *)dir->fd_data;
    mtd_partition *p = (mtd_partition *)mountpt->i_private;

    ret = lfs_dir_close(lfs_part[p->patitionnum]->fs, lfs_d);
    if (ret) {
        return ret_to_errno(ret);
    }

    free(lfs_d);
    dir->fd_data = NULL;
    free(dir);

    return LFS_ERR_OK;
}

static int vfs_lfs_readdir(FAR struct inode *mountpt,
        FAR struct fs_dirent_s *dir, struct dirent *d) {
    int ret;
    unsigned int dst_name_size;
    struct lfs_info info;
    lfs_dir_t *lfs_d = NULL;
    lfs_d = (lfs_dir_t *)dir->fd_data;
    mtd_partition *p = (mtd_partition *)mountpt->i_private;

    memset(&info, 0, sizeof(struct lfs_info));

    ret = lfs_dir_read(lfs_part[p->patitionnum]->fs, lfs_d, &info);
    if (ret == false) {
        return ret_to_errno((int)LFS_ERR_NOENT);
    } else if (ret != true) {
        return ret_to_errno(ret);
    }

    switch (info.type) {
    case LFS_TYPE_REG:
        d->d_type = DT_REG;
        break;
    case LFS_TYPE_DIR:
        d->d_type = DT_DIR;
        break;
    default:
        // unknown file type
        return -EPERM;
    }

    d->d_ino = lfs_d->id;
    d->d_reclen = info.size;
    d->d_off = lfs_d->pos - LFS_REDUCED_VALUE;
    dst_name_size = sizeof(d->d_name);
    if (strncpy_s(d->d_name, dst_name_size,
            info.name, dst_name_size - 1) != EOK) {
        return -ENAMETOOLONG;
    }
    d->d_name[dst_name_size - 1] = '\0';

    return LFS_ERR_OK;
}

static int vfs_lfs_rewinddir(FAR struct inode *mountpt,
        FAR struct fs_dirent_s *dir) {
    int ret;
    lfs_dir_t *lfs_d = NULL;
    lfs_d = (lfs_dir_t *)dir->fd_data;
    mtd_partition *p = (mtd_partition *)mountpt->i_private;

    ret = lfs_dir_rewind(lfs_part[p->patitionnum]->fs, lfs_d);
    if (ret) {
        return ret_to_errno(ret);
    }

    lfs_d->pos = LFS_FILE_OFFSET;
    dir->fd_data = lfs_d;
    return LFS_ERR_OK;
}

static int vfs_lfs_mkdir(FAR struct inode *mountpt,
        FAR const char *relpath, mode_t mode) {
    int ret;
    mtd_partition *p = (mtd_partition *)mountpt->i_private;

    if (relpath[0] == '/') {
        relpath++;
    }
    if (mountpt->mountflags == MS_RDONLY) {
        return -EACCES;
    }

    ret = lfs_mkdir(lfs_part[p->patitionnum]->fs, lfs_path_convert(relpath));

    if (ret) {
        return ret_to_errno(ret);
    }
    return LFS_ERR_OK;
}


static int vfs_lfs_rmdir(FAR struct inode *mountpt,
        FAR const char *relpath) {
    int ret;
    struct lfs_info info;
    mtd_partition *p = (mtd_partition *)mountpt->i_private;

    // deal path
    if (relpath[0] == '/') {
        relpath++;
    } else if (relpath[0] == '\0') {
        return -EBUSY;
    }
    if (mountpt->mountflags == MS_RDONLY) {
        return -EACCES;
    }

    ret = lfs_stat(lfs_part[p->patitionnum]->fs, lfs_path_convert(relpath), &info);
    if (ret) {
        return ret_to_errno(ret);
    }

    switch (info.type) {
    case LFS_TYPE_REG:
        ret = -ENOTDIR;
        break;
    case LFS_TYPE_DIR:
        ret = lfs_remove(lfs_part[p->patitionnum]->fs, lfs_path_convert(relpath));
        break;
    default:
        // unknown file type
        ret = -1;
        break;
    }

    if (ret) {
        return ret_to_errno(ret);
    }
    return LFS_ERR_OK;
}

static int vfs_lfs_unlink(FAR struct inode *mountpt,
        FAR const char *relpath) {
    int ret;
    struct lfs_info info;
    mtd_partition *p = (mtd_partition *)mountpt->i_private;

    if (relpath[0] == '/') {
        relpath++;
    }
    if (mountpt->mountflags == MS_RDONLY) {
        return -EACCES;
    }

    // judge file type, dir is to be delete by rmdir, others by unlink
    ret = lfs_stat(lfs_part[p->patitionnum]->fs, lfs_path_convert(relpath), &info);
    if (ret) {
        return ret_to_errno(ret);
    }
    switch (info.type) {
    case LFS_TYPE_REG:
        ret = lfs_remove(lfs_part[p->patitionnum]->fs, relpath);
        break;
    case LFS_TYPE_DIR:
        ret = -EISDIR;
        break;
    default:
        // unknown file type
        return -1;
    }

    if (ret) {
        return ret_to_errno(ret);
    }
    return LFS_ERR_OK;
}

static int vfs_lfs_rename(FAR struct inode *mountpt,
        FAR const char *oldrelpath,
        FAR const char *newrelpath) {
    int ret;
    unsigned int oldrelpath_len;
    unsigned int newrelpath_len;
    unsigned int cmp_len;

    mtd_partition *p = (mtd_partition *)mountpt->i_private;
    if (*oldrelpath == '/') {
        oldrelpath += 1;
    }
    if (*newrelpath == '/') {
        newrelpath += 1;
    }
    if (mountpt->mountflags == MS_RDONLY) {
        return -EACCES;
    }

    oldrelpath_len = strlen(oldrelpath);
    newrelpath_len = strlen(newrelpath);
    cmp_len = (oldrelpath_len <= newrelpath_len) ? oldrelpath_len : newrelpath_len;
    if (!strncmp(oldrelpath, newrelpath, cmp_len) &&
            (oldrelpath[cmp_len] == '/' || newrelpath[cmp_len] == '/')) {
        return -EPERM;
    }

    ret = lfs_rename(lfs_part[p->patitionnum]->fs, lfs_path_convert(oldrelpath), lfs_path_convert(newrelpath));
    if (ret) {
        return ret_to_errno(ret);
    }
    return LFS_ERR_OK;
}

static int vfs_lfs_stat(FAR struct inode *mountpt,
        FAR const char *relpath,
        FAR struct stat *buf) {
    int ret;
    struct lfs_info info;
    mtd_partition *p = (mtd_partition *)mountpt->i_private;

    if (relpath[0] == '/') {
        relpath++;
    }
    ret = lfs_stat(lfs_part[p->patitionnum]->fs, lfs_path_convert(relpath), &info);
    if (ret) {
        return ret_to_errno(ret);
    }

    // convert to vfs stat structure
    buf->st_mode = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH;
    switch (info.type) {
    case LFS_TYPE_REG:
        buf->st_mode |= S_IFREG;
        buf->st_size = (off_t)info.size;
        break;

    case LFS_TYPE_DIR:
        buf->st_mode |= S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
        buf->st_size = 0;
        break;

    default:
        buf->st_mode = DT_UNKNOWN;
        break;
    }

    return LFS_ERR_OK;
}

int vfs_lfs_format(void)
{
    for (int partition_num = 0; partition_num < CONFIG_MTD_PATTITION_NUM; ++partition_num) {
        if (lfs_part[partition_num] == NULL) {
            continue;
        }
        PRINT_INFO("formatting littlefs. partition_num=%d\n", partition_num);
        int ret = lfs_unmount(lfs_part[partition_num]->fs);
        if (ret != LFS_ERR_OK) {
            PRINT_ERR("littlefs unmount failed! %d\n", ret);
            return ret;
        }
        ret = lfs_format(lfs_part[partition_num]->fs, &lfs_part[partition_num]->cfg);
        if (ret != LFS_ERR_OK) {
            PRINT_ERR("littlefs format failed! %d\n", ret);
            return ret;
        }
        ret = lfs_mount(lfs_part[partition_num]->fs, &lfs_part[partition_num]->cfg);
        if (ret != 0) {
            PRINT_ERR("littlefs mount failed! %d\n", ret);
            return ret;
        }
    }
    return LFS_ERR_OK;
}

const struct mountpt_operations lfs_operations = {
    vfs_lfs_open,      /* open */
    vfs_lfs_close,     /* close */
    vfs_lfs_read,      /* read */
    vfs_lfs_write,     /* write */
    vfs_lfs_seek,      /* seek */
    NULL,              /* ioctl */
    NULL,              /* mmap */
    NULL,              /* truncate */

    vfs_lfs_sync,      /* sync */
    NULL,              /* dup */
    NULL,              /* fstat */
    NULL,              /* fchstat */
    vfs_lfs_opendir,   /* opendir */
    vfs_lfs_closedir,  /* closedir */
    vfs_lfs_readdir,   /* readdir */

    vfs_lfs_rewinddir, /* rewinddir */

    vfs_lfs_bind,      /* bind */
    vfs_lfs_unbind,    /* unbind */
    vfs_lfs_statfs,    /* statfs */
    NULL,              /* virstatfs */
    vfs_lfs_unlink,    /* unlinke */
    vfs_lfs_mkdir,     /* mkdir */
    vfs_lfs_rmdir,     /* rmdir */
    vfs_lfs_rename,    /* rename */
    vfs_lfs_stat,      /* stat */
    NULL,              /* chstat */
    NULL,              /* for utime */
    NULL,              /* chattr */
    vfs_lfs_seek64,    /* seek64 */
    NULL,              /* getlabel */
    NULL,              /* fallocate Contiguous blocks */
    NULL,              /* fallocate64 */
    NULL,              /* truncate64 */
    NULL               /* fscheck */
};

FSMAP_ENTRY(littlefs_fsmap, "littlefs", lfs_operations, TRUE, TRUE);

#endif
