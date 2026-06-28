/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2001 Dag-Erling Smørgrav
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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
 */
#include "sys/cdefs.h"

#include <stdio.h>
#include "internal.h"

#ifdef   LOSCFG_FS_PROC

DEFINE_SPINLOCK(procfs_lock);
bool procfs_init = false;

static struct proc_file g_proc_pf = {
	.f_pos		   = 0,
};

static struct proc_dir_entry g_proc_root_dir_entry = {
	.namelen	= 5,
	.mode		= S_IFDIR | S_IRUSR | S_IRGRP | S_IROTH,
	.count		= ATOMIC_INIT(1),
	.proc_fops	= NULL,
	.parent 	= &g_proc_root_dir_entry,
	.name		= "/proc",
	.subdir 	= NULL,
	.next		= NULL,
	.pf 		= &g_proc_pf,
};

static loff_t
proc_default_lseek(struct proc_file *procfile, loff_t off, int orig)
{
	loff_t ret = -EINVAL;
	switch (orig) {
	case SEEK_CUR:
		off += procfile->f_pos;
	/* fall-through */
	case SEEK_SET:
		if ((off < 0) || (off > MAX_NON_LFS)) {
			break;
		}
		procfile->f_pos = ret = off;
	}
	return ret;
}

static ssize_t
__proc_file_read(struct proc_file *procfile, char *buf, size_t nbytes, loff_t *pos)
{
	ssize_t ret = 0;
	ssize_t size = 0;
	pfs_node *pn = NULL;
	unsigned long long p;

	if (pos == NULL) {
		p = 0;
	} else {
		p = *pos;
	}

	if (p > MAX_NON_LFS) {
		return 0;
	}

	if (nbytes > MAX_NON_LFS - p) {
		nbytes = MAX_NON_LFS - p;
	}

	pn = procfile->pPDE;
	for (; nbytes > 0;) {
		if (pn->read_proc != NULL) {
			size = (ssize_t)pn->read_proc(procfile, (void *)buf);
		}
		if (size == 0) {  /* end of file */
			break;
		}
		if (size < 0) { /* error */
			if (ret == 0) {
				ret = size;
			}
			break;
		}
		nbytes -= size;
		buf += size;
		ret += size;

		if (nbytes < (size_t)size) {
			break;
		}
	}
	return ret;
}

static ssize_t
proc_default_read(struct proc_file *procfile, char *buf, size_t nbytes, loff_t *pos)
{
	return __proc_file_read(procfile, buf, nbytes, pos);
}

static ssize_t
proc_default_write(struct proc_file *procfile, const char *buffer, size_t count, loff_t *pos)
{
	pfs_node *pn = (pfs_node *)(procfile->pPDE);

	if (pn->write_proc != NULL) {
		return pn->write_proc(procfile, buffer, count, pos);
	}
	return -EIO;
}

static struct proc_file_operations g_proc_default_operations = {
	.llseek 	= proc_default_lseek,
	.read		= proc_default_read,
	.write		= proc_default_write,
};

int
proc_match(unsigned int len, const char *name, pfs_node *pn)
{
	if (len != pn->namelen)
		return 0;
	return !strncmp(name, pn->name, len);
}

static pfs_node *
pfs_find_node(pfs_node *parent, const char *name)
{
	pfs_node *pn = NULL;
	int length;

	if ((parent == NULL) || (name == NULL)) {
		return pn;
	}
	length = strlen(name);

	for (pn = parent->subdir; pn != NULL; pn = pn->next) {
		if ((length == pn->namelen) && strcmp(pn->name, name) == 0) {
			break;
		}
	}

	return pn;
}

/*
 * descrition: find the file's handle
 * path: the file of fullpath
 * return: the file of handle
 * add by ll
 */
pfs_node *
proc_find_entry(const char *path)
{
	pfs_node *pn = NULL;
	int isfoundsub;
	const char *next = NULL;
	unsigned int len;
	int leveltotal = 0;
	int levelcount = 0;
	const char *p = NULL;
	const char *name = path;

	while ((p = strchr(name, '/')) != NULL) {
		leveltotal++;
		name = p;
		name++;
	}
	if (leveltotal < 1) {
		return pn;
	}

	spin_lock(&procfs_lock);

	pn = &g_proc_root_dir_entry;

	while ((pn != NULL) && (levelcount < leveltotal)) {
		levelcount++;
		isfoundsub = 0;
		while (pn != NULL) {
			next = strchr(path, '/');
			if (next == NULL) {
				while (pn != NULL) {
					if (strcmp(path, pn->name) == 0) {
						spin_unlock(&procfs_lock);
						return pn;
					}
					pn = pn->next;
				}
				pn = NULL;
				spin_unlock(&procfs_lock);
				return pn;
			}

			len = next - path;
			if (pn == &g_proc_root_dir_entry) {
				if (levelcount == leveltotal) {
					spin_unlock(&procfs_lock);
					return pn;
				}
				len = g_proc_root_dir_entry.namelen;
			}
			if (proc_match(len, path, pn)) {
				isfoundsub = 1;
				path += len + 1;
				break;
			}

			pn = pn->next;
		}

		if ((isfoundsub == 1) && (pn != NULL)) {
			pn = pn->subdir;
		} else {
			pn = NULL;
			spin_unlock(&procfs_lock);
			return pn;
		}
	}
	spin_unlock(&procfs_lock);
	return NULL;
}

static int
check_proc_name(const char *name, pfs_node **parent, const char **lastname)
{
	pfs_node *pn = *parent;
	const char *segment = name;
	const char *restname = NULL;
	int length;

	if (pn == NULL) {
		pn = &g_proc_root_dir_entry;
	}

	spin_lock(&procfs_lock);

	restname = strchr(segment, '/');
	for (; restname != NULL; restname = strchr(segment, '/')) {
		length = restname - segment;
		for (pn = pn->subdir; pn != NULL; pn = pn->next) {
			if (proc_match(length, segment, pn))
				break;
		}
		if (pn == NULL) {
			PRINT_ERR(" Error!No such name '%s'\n", name);
			spin_unlock(&procfs_lock);
			return -ENOENT;
		}
		segment = restname;
		segment++;
	}
	*lastname = segment;
	*parent = pn;
	spin_unlock(&procfs_lock);

	return 0;
}

static pfs_node *
pfs_alloc_node(pfs_node **parent, const char *name, mode_t mode)
{
	pfs_node *pn = NULL;
	const char *lastname = NULL;
	int ret;

	if ((name == NULL) || (strlen(name) == 0) || (procfs_init == false)) {
		return pn;
	}

	if (check_proc_name(name, parent, &lastname) != 0) {
		return pn;
	}

	if (strlen(lastname) >= MAX_NAMELEN) {
		return pn;
	}

	if ((S_ISDIR((*parent)->mode) == 0) || (strchr(lastname, '/'))) {
		return pn;
	}

	pn = (pfs_node *)malloc(sizeof(pfs_node));
	if (pn == NULL) {
		return NULL;
	}

	if ((mode & S_IALLUGO) == 0) {
		mode |= S_IRUSR | S_IRGRP | S_IROTH;
	}

	(void)memset_s(pn, sizeof(pfs_node), 0, sizeof(pfs_node));
	pn->namelen = strlen(lastname);
	pn->mode = mode;
	ret = memcpy_s(pn->name, sizeof(pn->name), lastname, strlen(lastname) + 1);
	if (ret != EOK) {
		free(pn);
		return NULL;
	}

	pn->pf = (struct proc_file *)malloc(sizeof(struct proc_file));
	if (pn->pf == NULL) {
		free(pn);
		return NULL;
	}
	(void)memset_s(pn->pf, sizeof(struct proc_file), 0, sizeof(struct proc_file));
	pn->pf->pPDE = pn;
	ret = memcpy_s(pn->pf->name, sizeof(pn->pf->name), pn->name, pn->namelen + 1);
	if (ret != EOK) {
		free(pn->pf);
		free(pn);
		return NULL;
	}

	atomic_set(&pn->count, 1);
	spin_lock_init(&pn->pde_unload_lock);
	return pn;
}

static int
pfs_add_node(pfs_node *parent, pfs_node *pn)
{
	pfs_node *temp = NULL;

	if (parent == NULL) {
		PRINT_ERR("%s(): parent is NULL", __FUNCTION__);
		return -EINVAL;
	}

	if (pn->parent != NULL) {
		PRINT_ERR("%s(): node already has a parent", __FUNCTION__);
		return -EINVAL;
	}

	if (S_ISDIR(parent->mode) == 0) {
		PRINT_ERR("%s(): parent is not a directory", __FUNCTION__);
		return -EINVAL;
	}

	if ((S_ISREG(pn->mode)) && (pn->proc_fops == NULL)) {
		pn->proc_fops = &g_proc_default_operations;
	}

	spin_lock(&procfs_lock);

	temp = pfs_find_node(parent, pn->name);
	if (temp != NULL) {
		PRINT_ERR("Error!proc_dir_entry '%s/%s' already registered\n", parent->name, pn->name);
		spin_unlock(&procfs_lock);
		return -EEXIST;
	}

	pn->parent = parent;
	pn->next = parent->subdir;
	parent->subdir = pn;

	spin_unlock(&procfs_lock);

	return 0;
}

static void
pfs_detach_node(pfs_node *pn)
{
	pfs_node *parent = pn->parent;
	pfs_node **iter = NULL;

	if (parent == NULL) {
		PRINT_ERR("%s(): node has no parent", __FUNCTION__);
		return;
	}

	iter = &parent->subdir;
	while (*iter != NULL) {
		if (*iter == pn) {
			*iter = pn->next;
			break;
		}
		iter = &(*iter)->next;
	}
	pn->parent = NULL;
}

static pfs_node *
pfs_create_dir(pfs_node *parent, const char *name,
	const struct proc_file_operations *proc_fops, mode_t mode)
{
	pfs_node *pn = NULL;
	int ret;

	pn = pfs_alloc_node(&parent, name, S_IFDIR | mode);
	if (pn == NULL) {
		return pn;
	}
	pn->proc_fops = proc_fops;
	ret = pfs_add_node(parent, pn);
	if (ret != 0) {
		free(pn->pf);
		free(pn);
		return NULL;
	}

	return pn;
}

static pfs_node *
pfs_create_file(pfs_node *parent, const char *name,
	const struct proc_file_operations *proc_fops, mode_t mode)
{
	pfs_node *pn = NULL;
	int ret;

	pn = pfs_alloc_node(&parent, name, S_IFREG | mode);
	if (pn == NULL) {
		return pn;
	}

	pn->proc_fops = proc_fops;
	ret = pfs_add_node(parent, pn);
	if (ret != 0) {
		free(pn->pf);
		free(pn);
		return NULL;
	}

	return pn;
}

struct proc_dir_entry *
create_proc_entry(const char *name, mode_t mode, struct proc_dir_entry *parent)
{
	pfs_node *pde = NULL;

	if (S_ISDIR(mode)) {
		pde = pfs_create_dir(parent, name, NULL, mode);
	} else {
		pde = pfs_create_file(parent, name, NULL, mode);
	}
	return pde;
}

static void
free_proc_entry(pfs_node *pn)
{
	if (pn == NULL) {
		return;
	}
	if ((pn->pf != NULL) && (pn->pf->private_data != NULL)) {
		free(pn->pf->private_data);
		pn->pf->private_data = NULL;
	}
	if (pn->pf != NULL) {
		free(pn->pf);
		pn->pf = NULL;
	}
	free(pn);
}

void
pde_put(pfs_node *pn)
{
	if (atomic_dec_and_test(&pn->count))
		free_proc_entry(pn);
}

static void
__remove_proc_entry(pfs_node *pn)
{
	if (pn == NULL) {
		return;
	}
	__remove_proc_entry(pn->next);
	__remove_proc_entry(pn->subdir);

	pde_put(pn);
}

void
remove_proc_entry(const char *name, struct proc_dir_entry *parent)
{
	pfs_node *pn = NULL;
	const char *lastname = name;

	if ((name == NULL) || (strlen(name) == 0) || (procfs_init == false)) {
		return;
	}

	if (check_proc_name(name, &parent, &lastname) != 0) {
		return;
	}

	if (strlen(lastname) > MAX_NAMELEN) {
		return;
	}

	spin_lock(&procfs_lock);

	pn = pfs_find_node(parent, lastname);
	if (pn == NULL) {
		PRINT_ERR("Error:name '%s' not found!\n", name);
		spin_unlock(&procfs_lock);
		return;
	}
	pfs_detach_node(pn);

	spin_unlock(&procfs_lock);

	__remove_proc_entry(pn->subdir);
	pde_put(pn);
}

struct proc_dir_entry *
proc_mkdir_mode(const char *name, mode_t mode, struct proc_dir_entry *parent)
{
	return pfs_create_dir(parent, name, NULL, mode);
}

struct proc_dir_entry *
proc_mkdir(const char *name, struct proc_dir_entry *parent)
{
	return pfs_create_dir(parent, name, NULL, 0);
}

struct proc_dir_entry *
proc_create_data(const char *name, mode_t mode, struct proc_dir_entry *parent,
	const struct proc_file_operations *proc_fops, void *data)
{
	pfs_node *pde = create_proc_entry(name, mode, parent);
	if (pde != NULL) {
		if (proc_fops != NULL) {
			pde->proc_fops = proc_fops;
		}
		pde->data = data;
	}
	return pde;
}

struct proc_dir_entry *
proc_create(const char *name, mode_t mode, struct proc_dir_entry *parent,
	const struct proc_file_operations *proc_fops)
{
	return proc_create_data(name, mode, parent, proc_fops, NULL);
}

static pfs_node *
__proc_open(const char *filename, int flags, ...)
{
	pfs_node *pn = proc_find_entry(filename);
	if (pn == NULL) {
		return NULL;
	}

	if (S_ISREG(pn->mode) && (pn->count != 1)) {
		return NULL;
	}

	pn->flags = (unsigned int)(pn->flags) | (unsigned int)flags;
	atomic_set(&pn->count, 2);
	if (S_ISREG(pn->mode) && (pn->proc_fops != NULL) && (pn->proc_fops->open != NULL)) {
		(void)pn->proc_fops->open((struct inode *)pn, pn->pf);
	}
	if (S_ISDIR(pn->mode)) {
		pn->pdir_current = pn->subdir;
		pn->pf->f_pos = 0;
	}

	return pn;
}

struct proc_dir_entry *
proc_open(const char *filename, int flags, ...)
{
	return __proc_open(filename, flags);
}

int
proc_close(struct proc_dir_entry *pde)
{
	int result = 0;

	if (pde == NULL) {
		return -EPERM;
	}
	pde->pf->f_pos = 0;
	atomic_set(&pde->count, 1);
	if (S_ISDIR(pde->mode)) {
		pde->pdir_current = pde->subdir;
	}
	if ((pde->proc_fops != NULL) && (pde->proc_fops->release != NULL)) {
		result = pde->proc_fops->release((struct inode *)pde,pde->pf);
		pde->pf->private_data = NULL;
	}
	if (pde->parent == NULL)
		free_proc_entry(pde);
	return result;
}

static int
__proc_readdir(pfs_node *pn, void *buf, size_t len)
{
	char *buff = (char *)buf;
	int ret;

	if (pn->pdir_current == NULL) {
		*buff = '\0';
		return -ENOENT;
	}
	ret = memcpy_s(buff, len, pn->pdir_current->name, pn->pdir_current->namelen);
	if (ret != EOK) {
		return -ENAMETOOLONG;
	}

	pn->pdir_current = pn->pdir_current->next;
	pn->pf->f_pos++;
	return ENOERR;
}

static int
__proc_read(pfs_node *pn, void *buf, size_t len)
{
	int result = -EPERM;

	if ((pn->proc_fops != NULL) && (pn->proc_fops->read != NULL)) {
		result = pn->proc_fops->read(pn->pf, (char *)buf, len, &(pn->pf->f_pos));
	}

	return result;
}

int
proc_read(struct proc_dir_entry *pde, void *buf, size_t len)
{
	int result = -EPERM;

	if (pde == NULL) {
		return result;
	}
	if (S_ISREG(pde->mode)) {
		result = __proc_read(pde, buf, len);
	} else if (S_ISDIR(pde->mode)) {
		result = __proc_readdir(pde,buf,len);
	}
	return result;
}

static int
__proc_write(pfs_node *pn, const void *buf, size_t len)
{
	int result = -EPERM;

	if (S_ISDIR(pn->mode)) {
		return -EISDIR;
	}
	if ((pn->proc_fops != NULL) && (pn->proc_fops->write != NULL)) {
		result = pn->proc_fops->write(pn->pf, (const char *)buf, len, &(pn->pf->f_pos));
	}
	return result;
}

int
proc_write(struct proc_dir_entry *pde, const void *buf, size_t len)
{
	int result = -EPERM;

	if (pde == NULL) {
		return result;
	}
	spin_lock(&procfs_lock);

	result = __proc_write(pde, buf, len);

	spin_unlock(&procfs_lock);

	return result;
}

loff_t
proc_lseek(struct proc_dir_entry *pde, loff_t offset, int whence)
{
	loff_t result = 0;

	if ((pde != NULL) && (pde->proc_fops != NULL) && (pde->proc_fops->llseek != NULL)) {
		result = pde->proc_fops->llseek(pde->pf,offset,whence);
	}
	return result;
}

int
proc_dirlseek(struct proc_dir_entry *pde, off_t *pos, int whence)
{
	/* Only allow SEEK_SET to zero */
	if ((whence != SEEK_SET) || (*pos != 0)) {
		return EINVAL;
	}
	pde->pdir_current = pde->subdir;
	pde->pf->f_pos = 0;
	return ENOERR;
}

int
proc_stat(const char *file, struct proc_stat *buf)
{
	pfs_node *pn = NULL;
	int len = sizeof(buf->name);
	int ret;

	pn = proc_find_entry(file);
	if (pn == NULL) {
		return ENOENT;
	}

	ret = strncpy_s(buf->name, len, pn->name, len-1);
	if (ret != EOK) {
		return ENAMETOOLONG;
	}

	buf->name[len-1] = '\0';
	buf->st_mode = pn->mode;
	buf->pPDE = pn;

	return 0;
}
#endif
