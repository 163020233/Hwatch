/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2015-2019. All rights reserved.
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

#include "sys/statfs.h"
#include "stdlib.h"
#include "fcntl.h"
#include "internal.h"
#include "los_tables.h"

#ifdef LOSCFG_FS_PROC

static char *
procfs_changepath(const char *relpath, int namelen)
{
	char *path = (char *)relpath;

	if (*relpath != 0) {
		path = path - (namelen + 1); /* "/proc/xxx" need left avertence "/proc/" */
	} else {
		path = path - namelen;
	}

	return path;
}

int
vfs_procfs_open(struct file *filep, const char *relpath, int oflags, mode_t mode)
{
	struct proc_dir_entry *pde = NULL;

	if ((unsigned int)oflags & O_DIRECTORY) {
		return -EACCES;
	}
	pde = proc_open(filep->f_path, oflags);
	if (pde == NULL) {
		if ((unsigned int)oflags & O_CREAT) {
			return -EPERM;
		}
		return -ENOENT;
	}
	if (((unsigned int)oflags & O_CREAT) && ((unsigned int)oflags & O_EXCL)) {
		(void)proc_close(pde);
		return -EEXIST;
	}
	if (S_ISDIR(pde->mode)) {
		(void)proc_close(pde);
		return -EISDIR;
	}
	filep->f_priv = (void *)pde;
	filep->f_pos = 0;

	return 0;
}

int
vfs_procfs_close(struct file *filep)
{
	return proc_close((struct proc_dir_entry *)filep->f_priv);
}

ssize_t
vfs_procfs_read(struct file *filep, FAR char *buffer, size_t buflen)
{
	ssize_t size;
	struct proc_dir_entry *pde = (struct proc_dir_entry *)filep->f_priv;

	size = (ssize_t)proc_read(pde, (void *)buffer, buflen);
	filep->f_pos = pde->pf->f_pos;

	return size;
}

ssize_t
vfs_procfs_write(struct file *filep, const char *buffer, size_t buflen)
{
	ssize_t size;
	struct proc_dir_entry *pde = (struct proc_dir_entry *)filep->f_priv;

	size = (ssize_t)proc_write(pde, (void *)buffer, buflen);
	filep->f_pos = pde->pf->f_pos;

	return size;
}

off_t
vfs_procfs_lseek(struct file *filep, off_t offset, int whence)
{
	loff_t off;
	struct proc_dir_entry *pde = (struct proc_dir_entry *)filep->f_priv;

	off = proc_lseek(pde, (loff_t)offset, whence);
	filep->f_pos = pde->pf->f_pos;

	return (off_t)off;
}

loff_t
vfs_procfs_lseek64(struct file *filep, loff_t offset, int whence)
{
	loff_t off;
	struct proc_dir_entry *pde = (struct proc_dir_entry *)filep->f_priv;

	off = proc_lseek(pde, offset, whence);
	filep->f_pos = pde->pf->f_pos;

	return off;
}

int
vfs_procfs_ioctl(struct file *filep, int cmd, unsigned long arg)
{
	return -ENOSYS;
}

int
vfs_procfs_sync(struct file *filep)
{
	return -ENOSYS;
}

int
vfs_procfs_dup(const struct file *oldp, struct file *newp)
{
	return -ENOSYS;
}

int
vfs_procfs_opendir(struct inode *mountpt, const char *relpath, struct fs_dirent_s **dir)
{
	struct proc_dir_entry *pde = NULL;
	char *path = NULL;
	int oflags = O_APPEND;

	*dir = malloc(sizeof(struct fs_dirent_s));
	if (*dir == NULL) {
		return -ENOMEM;
	}

	path = procfs_changepath(relpath, PROCFS_MOUNT_POINT_SIZE);
	pde = proc_open(path, oflags);
	if (pde == NULL) {
		free(*dir);
		return -ENOENT;
	}
	if (S_ISREG(pde->mode)) {
		free(*dir);
		(void)proc_close(pde);
		return -ENOTDIR;
	}

	(*dir)->fd_data = pde;

	return 0;
}

int
vfs_procfs_closedir(struct inode *mountpt, struct fs_dirent_s *dir)
{
	int ret;

	ret = proc_close((struct proc_dir_entry *)dir->fd_data);
	free(dir);
	return ret;
}

int
vfs_procfs_readdir(struct inode *mountpt, struct fs_dirent_s *dir, struct dirent *d)
{
	int result;
	char *buffer = NULL;
	int buflen = MAX_NAMELEN;
	unsigned int min_size;
	unsigned int dst_name_size;
	struct proc_dir_entry *pde = dir->fd_data;

	buffer = (char *)malloc(sizeof(char) * MAX_NAMELEN);
	if (buffer == NULL) {
		PRINT_ERR("malloc failed\n");
		return -ENOMEM;
	}
	(void)memset_s(buffer, MAX_NAMELEN, 0, MAX_NAMELEN);

	result = proc_read(pde, (void *)buffer, buflen);
	if (result != ENOERR) {
		free(buffer);
		return result;
	}
	d->d_off = pde->pf->f_pos - 1;
	dst_name_size = sizeof(d->d_name);
	min_size = dst_name_size < MAX_NAMELEN ? dst_name_size : MAX_NAMELEN;
	result = strncpy_s(d->d_name, dst_name_size, buffer, min_size);
	if (result != EOK) {
		free(buffer);
		return -ENAMETOOLONG;
	}
	d->d_name[dst_name_size - 1] = '\0';
	free(buffer);

	return ENOERR;
}

int
vfs_procfs_rewinddir(struct inode *mountpt, struct fs_dirent_s *dir)
{
	int ret;
	off_t pos = 0;

	ret = proc_dirlseek((struct proc_dir_entry *)dir->fd_data, &pos, SEEK_SET);
	if (ret != ENOERR) {
		return -ret;
	}

	return 0;
}

int
vfs_procfs_bind(struct inode *blkdriver, const void *data, FAR void **handle, const char *relpath)
{
	int len, length;

	len = strlen(relpath) + 1;
	length = strlen(PROCFS_MOUNT_POINT) + 1;
	if ((len == length) && !strncmp(relpath, PROCFS_MOUNT_POINT, length)) {
		spin_lock_init(&procfs_lock);
		procfs_init = true;
		return ENOERR;
	}

	return -EPERM;
}

int
vfs_procfs_unbind(void *handle, struct inode **blkdriver)
{
	return -EPERM;
}

int
vfs_procfs_statfs(struct inode *mountpt, struct statfs *buf)
{
	(void)memset_s(buf, sizeof(struct statfs), 0, sizeof(struct statfs));
	buf->f_type = PROCFS_MAGIC;

	return OK;
}

int
vfs_procfs_unlink(struct inode *mountpt, const char *relpath)
{
	struct proc_dir_entry *pde = NULL;
	char *path = NULL;

	path = procfs_changepath(relpath, PROCFS_MOUNT_POINT_SIZE);
	pde = proc_find_entry(path);
	if (pde == NULL) {
		return -ENOENT;
	}
	if (S_ISDIR(pde->mode)) {
		return -EISDIR;
	}

	return -EACCES;
}

int
vfs_procfs_mkdir(struct inode *mountpt, const char *relpath, mode_t mode)
{
	struct proc_dir_entry *pde = NULL;
	char *path = NULL;

	path = procfs_changepath(relpath, PROCFS_MOUNT_POINT_SIZE);
	pde = proc_find_entry(path);
	if (pde == NULL) {
		return -ENOENT;
	}

	return -EEXIST;
}

int
vfs_procfs_rmdir(struct inode *mountpt, const char *relpath)
{
	struct proc_dir_entry *pde = NULL;
	char *path = NULL;

	path = procfs_changepath(relpath, PROCFS_MOUNT_POINT_SIZE);
	pde = proc_find_entry(path);
	if (pde == NULL) {
		return -ENOENT;
	}

	return -EACCES;
}

int
vfs_procfs_rename(struct inode *mountpt, const char *oldrelpath, const char *newrelpath)
{
	return -ENOSYS;
}

int
vfs_procfs_stat(struct inode *mountpt, const char *relpath, struct stat *buf)
{
	int result;
	struct proc_stat statbuf;
	char *path = procfs_changepath(relpath, PROCFS_MOUNT_POINT_SIZE);

	result = proc_stat(path, &statbuf);
	if (result != ENOERR) {
		return -result;
	}
	(void)memset_s(buf, sizeof(struct stat), 0, sizeof(struct stat));
	buf->st_mode = statbuf.st_mode;

	return 0;
}

const struct mountpt_operations procfs_operations = {
	vfs_procfs_open,		/* open */
	vfs_procfs_close,		/* close */
	vfs_procfs_read,		/* read */
	vfs_procfs_write,		/* write */
	vfs_procfs_lseek,		/* seek */
	vfs_procfs_ioctl,		/* ioctl */
	NULL,					/* mmap */
	NULL,					/* truncate */
	vfs_procfs_sync,		/* sync */
	vfs_procfs_dup,			/* dup */
	NULL,				/* fstat */
	NULL,				/* fchstat */
	vfs_procfs_opendir,		/* opendir */
	vfs_procfs_closedir,		/* closedir */
	vfs_procfs_readdir,		/* readdir */
	vfs_procfs_rewinddir,		/* rewinddir */
	vfs_procfs_bind,		/* bind */
	vfs_procfs_unbind,		/* unbind */
	vfs_procfs_statfs,		/* statfs */
	NULL,				/* virstatfs */
	vfs_procfs_unlink,		/* unlink */
	vfs_procfs_mkdir,		/* mkdir */
	vfs_procfs_rmdir,		/* rmdir */
	vfs_procfs_rename,		/* rename */
	vfs_procfs_stat,		/* stat */
	NULL,				/* chstat */
	NULL,				/* for utime */
	NULL,				/* chattr */
	vfs_procfs_lseek64,		/* seek64 */
	NULL,				/* getlabel */
	NULL,				/* fallocate */
	NULL,				/* fallocate64 */
	NULL,				/* truncate64 */
	NULL				/* fscheck */
};

FSMAP_WOW_ENTRY(procfs_fsmap, "procfs", procfs_operations, FALSE, FALSE);

#endif
