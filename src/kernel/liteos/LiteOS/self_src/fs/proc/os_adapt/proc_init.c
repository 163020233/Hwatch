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
#include "sys/mount.h"
#include "linux/seq_file.h"
#include "proc_fs.h"

#ifdef LOSCFG_FS_PROC

static int
show(FAR const char *mountpoint, FAR struct statfs *statbuf, FAR void *arg)
{
	struct seq_file *seqfile = (struct seq_file *)arg;
	char *type = NULL;
	char *name = NULL;

	switch (statbuf->f_type) {
	case PROCFS_MAGIC:
		type = "proc";
		name = "proc";
		break;
	case JFFS2_SUPER_MAGIC:
		type = "jffs";
		name = "jffs";
		break;
	case YAFFS_MAGIC:
		type = "yaffs";
		name = "yaffs";
		break;
	case NFS_SUPER_MAGIC:
		type = "nfs";
		name = "nfs";
		break;
	case RAMFS_MAGIC:
		type = "ramfs";
		name = "ramfs";
		break;
	case MSDOS_SUPER_MAGIC:
		type = "vfat";
		name = "fat";
		break;
	case ROMFS_MAGIC:
		type = "romfs";
		name = "romfs";
		break;
	default:
		return 0;
	}
	(void)seq_printf(seqfile, "%s %s %s\n", name, mountpoint, type);

	return 0;
}

static int
mounts_proc_show(struct seq_file *m, void *v)
{
	foreach_mountpoint_t handler = show;
	(void)foreach_mountpoint(handler, (FAR void *)m);

	return 0;
}

static int
mounts_proc_open(struct inode *inode_ptr, struct proc_file *pf)
{
	return single_open(pf, mounts_proc_show, NULL);
}

static const struct proc_file_operations mounts_proc_fops = {
	.open		= mounts_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write		= NULL,
};

void
proc_mounts_init(void)
{
	struct proc_dir_entry *pde = create_proc_entry("mounts", 0, NULL);
	if (pde == NULL) {
		PRINT_ERR("creat mounts error!\n");
		return;
	}

	pde->proc_fops = &mounts_proc_fops;
}

void
proc_fs_init(void)
{
	int ret;

	ret = mount(NULL, PROCFS_MOUNT_POINT, "procfs", 0, NULL);
	if (ret) {
		PRINT_ERR("mount procfs err %d\n", ret);
		return;
	}
	PRINTK("Mount procfs finished.\n");
	proc_mounts_init();
}
#endif
