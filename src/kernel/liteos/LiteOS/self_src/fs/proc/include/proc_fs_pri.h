/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LiteOS PROC FS Inside Headfile
 * Author: Huawei LiteOS Team
 * Create: 2021-07-16
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

#ifndef _PROC_FS_PRI_H
#define _PROC_FS_PRI_H
#include <sys/types.h>
#include <sys/stat.h>
#include "proc_fs.h"

#ifdef LOSCFG_FS_PROC

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LIMIT_SIZE 0x100000
#define PROC_ERROR (-1)

/* 64bit hashes as llseek() offset (for directories) */
#define FMODE_64BITHASH   ((fmode_t)0x400)
/* 32bit hashes as llseek() offset (for directories) */
#define FMODE_32BITHASH   ((fmode_t)0x200)
/* File is opened using open(.., 3, ..) and is writable only for ioctls
 *	 (specially hack for floppy.c)
 */
#define FMODE_WRITE_IOCTL ((fmode_t)0x100)
/* File is opened with O_EXCL (only set for block devices) */
#define FMODE_EXCL        ((fmode_t)0x80)
/* File is opened with O_NDELAY (only set for block devices) */
#define FMODE_NDELAY      ((fmode_t)0x40)
/* File is opened for execution with sys_execve / sys_uselib */
#define FMODE_EXEC        ((fmode_t)0x20)
/* file can be accessed using pwrite */
#define FMODE_PWRITE      ((fmode_t)0x10)
/* file can be accessed using pread */
#define FMODE_PREAD       ((fmode_t)0x8)
/* file is seekable */
#define FMODE_LSEEK       ((fmode_t)0x4)
/* file is open for writing */
#define FMODE_WRITE       ((fmode_t)0x2)
/* file is open for reading */
#define FMODE_READ        ((fmode_t)0x1)

struct proc_stat {
	mode_t st_mode;
	struct proc_dir_entry *pPDE;
	char name[MAX_NAMELEN];
};

#define S_IALLUGO (S_ISUID | S_ISGID | S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOSCFG_FS_PROC */
#endif /* _PROC_FS_PRI_H */
