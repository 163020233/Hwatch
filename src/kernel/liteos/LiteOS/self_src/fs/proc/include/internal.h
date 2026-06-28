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

#ifndef _PROC_INTERNAL_H
#define _PROC_INTERNAL_H

#include "proc_fs_pri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_NON_LFS ((1UL << 31) - 1)

extern spinlock_t procfs_lock;
extern bool procfs_init;
typedef struct proc_dir_entry pfs_node;

int proc_match(unsigned int len, const char *name, struct proc_dir_entry *pde);
struct proc_dir_entry *proc_find_entry(const char *path);
void pde_put(struct proc_dir_entry *pde);

extern int proc_stat(const char *file, struct proc_stat *buf);
extern void proc_mounts_init(void);

/**
 * @ingroup  procfs
 * @brief open a proc node
 *
 * @par Description:
 * This API is used to open the  node by 'filename' and flags,
 *
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param  filename [IN] Type  #const char * the filename of the node to  be opened
 * @param  flags    [IN] Type  #int the flags of open's node
 *
 * @retval #NULL                open failed
 * @retval #NOT NULL            open successfully
 * @par Dependency:
 * <ul><li>proc_fs.h: the header file that contains the API declaration.</li></ul>
 * @see proc_close
 *
 * @since Huawei LiteOS V100R001C00
 */
extern struct proc_dir_entry *proc_open(const char *filename, int flags, ...);

/**
 * @ingroup  procfs
 * @brief close a proc node
 *
 * @par Description:
 * This API is used to close the node by pde
 *
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param  pde [IN] Type #struct proc_dir_entry * pointer of the node structure to be closed
 *
 * @retval #-1                close failed
 * @retval #0                 close successfully
 * @par Dependency:
 * <ul><li>proc_fs.h: the header file that contains the API declaration.</li></ul>
 * @see proc_open
 *
 * @since Huawei LiteOS V100R001C00
 */
extern int proc_close(struct proc_dir_entry *pde);

/**
 * @ingroup  procfs
 * @brief read a proc node
 *
 * @par Description:
 * This API is used to read the node by pde
 *
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param  pde     [IN] Type #struct proc_dir_entry * pointer of the node structure to be read
 * @param  buf     [IN] Type #void *  user-provided to save the data
 * @param  len     [IN] Type #size_t  the length of want to read
 *
 * @retval #-1                read failed
 * @retval #>0                Number of bytes read success
 * @par Dependency:
 * <ul><li>proc_fs.h: the header file that contains the API declaration.</li></ul>
 * @see proc_open
 *
 * @since Huawei LiteOS V100R001C00
 */
extern int proc_read(struct proc_dir_entry *pde, void *buf, size_t len);

/**
 * @ingroup  procfs
 * @brief write a proc node
 *
 * @par Description:
 * This API is used to write the node by pde
 *
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param  pde     [IN] Type #struct proc_dir_entry * pointer of the node structure to be written
 * @param  buf     [IN] Type #const void *    data to write
 * @param  len     [IN] Type #size_t    length of data to write
 *
 * @retval #-1                write failed
 * @retval #>0                Number of bytes write successfully
 * @par Dependency:
 * <ul><li>proc_fs.h: the header file that contains the API declaration.</li></ul>
 * @see proc_open
 *
 * @since Huawei LiteOS V100R001C00
 */
extern int proc_write(struct proc_dir_entry *pde, const void *buf, size_t len);

/**
 * @ingroup  procfs
 * @brief File migration
 *
 * @par Description:
 * This API is used to set the proc file migration
 *
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param  pde     [IN] Type #struct proc_dir_entry *   pointer of the node structure to be deviation
 * @param  offset  [IN] Type #loff_t    the number of deviation
 * @param  whence  [IN] Type #int       the begin of  deviation
 *
 * @retval #<0                deviation failed
 * @retval #>=0               deviation successfully
 * @par Dependency:
 * <ul><li>proc_fs.h: the header file that contains the API declaration.</li></ul>
 * @see proc_open
 *
 * @since Huawei LiteOS V100R001C00
 */
extern loff_t proc_lseek(struct proc_dir_entry *pde, loff_t offset, int whence);

/**
 * @ingroup  procfs
 * @brief directory migration
 *
 * @par Description:
 * This API is used to set the proc directory migration
 *
 * @attention
 * <ul>
 * <li>Only allow SEEK_SET to zero.</li>
 * </ul>
 *
 * @param  pde     [IN] Type #struct proc_dir_entry *   pointer of the node structure to be deviated
 * @param  pos     [IN] Type #off_t *      the number of deviation
 * @param  whence  [IN] Type #int       the begin of deviation
 *
 * @retval #EINVAL            deviation failed
 * @retval #ENOERR            deviation successfully
 * @par Dependency:
 * <ul><li>proc_fs.h: the header file that contains the API declaration.</li></ul>
 * @see proc_open
 *
 * @since Huawei LiteOS V100R001C00
 */
int proc_dirlseek(struct proc_dir_entry *pde, off_t *pos, int whence);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
