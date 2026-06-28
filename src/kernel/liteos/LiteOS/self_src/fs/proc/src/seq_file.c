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

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "errno.h"
#include "limits.h"
#include "linux/seq_file.h"
#ifdef LOSCFG_FS_PROC
#include "proc_fs_pri.h"

static int
reallocate_larger_buf(struct seq_file *seqfilep, size_t oldcount)
{
	char *largerbuf = NULL;
	int  err;

	/* check error! */
	if ((seqfilep == NULL) || (seqfilep->buf == NULL)) {
		return PROC_ERROR;
	}

	/* limited the size of the seq file, small than 1M. */
	if (seqfilep->size >= LIMIT_SIZE) {
		/* error! */
		goto reallocate_error;
	}

	largerbuf = (char *)malloc(seqfilep->size <<= 1);
	if (largerbuf == NULL) {
		/* error! */
		goto reallocate_error;
	}
	(void)memset_s(largerbuf, seqfilep->size, 0, seqfilep->size);

	/* get the data before the latest vsprintf. */
	err = memcpy_s(largerbuf, seqfilep->size, seqfilep->buf, oldcount);
	if (err != EOK) {
		free(largerbuf);
		goto reallocate_error;
	}
	seqfilep->count = oldcount;

	/* free the old small buf. */
	free(seqfilep->buf);
	/* point to the new larger buf. */
	seqfilep->buf = largerbuf;

	return 0;

reallocate_error:
	free(seqfilep->buf);
	seqfilep->buf = NULL;
	return PROC_ERROR;
}

int
seq_open(struct proc_file *procfilep, const struct seq_operations *op)
{
	struct seq_file *seqfilep = NULL;

	if ((procfilep == NULL) || (op == NULL) || (op->show == NULL)) {
		return PROC_ERROR;
	}

	if (procfilep->private_data != NULL) {
		return PROC_ERROR;
	}

	seqfilep = (struct seq_file *)malloc(sizeof(struct seq_file));
	if (seqfilep == NULL) {
		return PROC_ERROR;
	}
	(void)memset_s(seqfilep, sizeof(struct seq_file), 0, sizeof(struct seq_file));

	seqfilep->op = op;
	procfilep->private_data = seqfilep;

	return 0;
}

ssize_t
seq_read(struct proc_file *procfilep, char *buf, size_t readsize, loff_t *ppos)
{
	struct seq_file *seqfilep = NULL;
	size_t realcopysize = 0;
	int ret;

	/* check error! */
	if ((procfilep == NULL) || (procfilep->private_data == NULL) || (buf == NULL) || (ppos == NULL) || (*ppos < 0)) {
		goto read_error;
	}

	seqfilep = (struct seq_file *)procfilep->private_data;

	/* make sure only show once. */
	if ((seqfilep->op->show != NULL) && (seqfilep->buf == NULL)) {
		/* call the show function to build the seq file. */
		if (seqfilep->op->show(seqfilep, NULL) != 0) {
			goto read_error;
		}
	}

	/* if the count is 0, means never call the seq_printf in the show function, no need to malloc the buf. */
	if ((*ppos >= seqfilep->count) || (readsize == 0)) {
		/* there's no data at or after the file tail. */
		realcopysize = 0;
	} else {
		/* because of the return type is signed int, so the realcopysize can't bigger than INT_MAX. */
		realcopysize = min((seqfilep->count - *ppos), min(readsize, INT_MAX));
		ret = memcpy_s(buf, readsize, seqfilep->buf + *ppos, realcopysize);
		if (ret != EOK) {
			goto read_error;
		}

        seqfilep->read_pos = (off_t)(*ppos + realcopysize);
		procfilep->f_pos = seqfilep->read_pos;
	}

	return (ssize_t)realcopysize;

read_error:
	return PROC_ERROR;
}

loff_t
seq_lseek(struct proc_file *procfilep, loff_t offset, int origin)
{
	struct seq_file *seqfilep = NULL;
	loff_t absoluteoffset;

	/* check error. */
	if ((procfilep == NULL) || (procfilep->private_data == NULL)) {
		return PROC_ERROR;
	}

	seqfilep = (struct seq_file *)procfilep->private_data;

	switch (origin) {
	case SEEK_CUR:
		absoluteoffset = offset + seqfilep->read_pos;
		break;

	case SEEK_SET:
		absoluteoffset = offset;
		break;

	default:
		absoluteoffset = PROC_ERROR;
		break;
	}

	if (absoluteoffset >= 0) {
		seqfilep->read_pos = absoluteoffset;
		procfilep->f_pos = seqfilep->read_pos;

		return absoluteoffset;
	}

	return -EINVAL;
}

int
seq_release(struct inode *inodep, struct proc_file *procfilep)
{
	struct seq_file *seqfilep = NULL;

	if ((procfilep == NULL) || (procfilep->private_data == NULL)) {
		return PROC_ERROR;
	}

	seqfilep = (struct seq_file *)procfilep->private_data;

	/* if never call the seq_printf in the show function, the buf is NULL. */
	if (seqfilep->buf != NULL) {
		free(seqfilep->buf);
		seqfilep->buf = NULL;
	}
	free(seqfilep);

	procfilep->private_data = NULL;

	return 0;
}

int
seq_printf(struct seq_file *seqfilep, const char *f, ...)
{
	int buflen;
	size_t oldcount;
	va_list arglist;
	bool needreprintf = FALSE; /* if need re-printf. */

	/* check error! */
	if (seqfilep == NULL) {
		return PROC_ERROR;
	}

	/* memory alloc the buf. */
	if (seqfilep->buf == NULL) {
		seqfilep->size = PAGE_SIZE;
		seqfilep->buf = (char *)malloc(seqfilep->size);
		if (seqfilep->buf == NULL) {
			/* no memory, error! */
			return PROC_ERROR;
		}
		(void)memset_s(seqfilep->buf, seqfilep->size, 0, seqfilep->size);
		seqfilep->count = 0;
	}

	do {
		oldcount = seqfilep->count;

		/* write the new data. */
		va_start(arglist, f);
		buflen = vsnprintf(seqfilep->buf + seqfilep->count, seqfilep->size - seqfilep->count, f, arglist);
		va_end(arglist);
		if (buflen < 0) {
			free(seqfilep->buf);
			seqfilep->buf = NULL;
			break;
		}

        if (seqfilep->count + (unsigned int)buflen < seqfilep->size) {
			/* succeed write. */
            seqfilep->count += (unsigned int)buflen;
			return 0;
		}

		/* need re-printf the data after we get a larger buf. */
		needreprintf = TRUE;

		/* means the buf is too small, we need a larger one. */
		if (reallocate_larger_buf(seqfilep, oldcount) != 0) {
			/* error! */
			break;
		}
	} while (needreprintf);

	return PROC_ERROR;
}

int
single_open(struct proc_file *procfilep, int (*show)(struct seq_file *, void *), void *data)
{
	struct seq_operations *seqoperationp = NULL;
	struct seq_file *seqfilep = NULL;

	if ((procfilep == NULL) || (show == NULL) || (procfilep->private_data != NULL)) {
		return PROC_ERROR;
	}

	seqoperationp = (struct seq_operations *)malloc(sizeof(struct seq_operations));
	if (seqoperationp == NULL) {
		return PROC_ERROR;
	}
	(void)memset_s(seqoperationp, sizeof(struct seq_operations), 0, sizeof(struct seq_operations));

	seqoperationp->start = NULL;
	seqoperationp->stop  = NULL;
	seqoperationp->next  = NULL;
	seqoperationp->show  = show;

	if (seq_open(procfilep, seqoperationp) != 0) {
		free(seqoperationp);
		return PROC_ERROR;
	}

	seqfilep = (struct seq_file *)procfilep->private_data;
	seqfilep->private = data;

	return 0;
}

int
single_release(struct inode *inodep, struct proc_file *procfilep)
{
	const struct seq_operations *seqoperationp = NULL;
	int reval;

	if ((procfilep == NULL) || (procfilep->private_data == NULL)) {
		return PROC_ERROR;
	}

	seqoperationp = ((struct seq_file *)procfilep->private_data)->op;
	reval = seq_release(inodep, procfilep);

	free((void *)seqoperationp);

	return reval;
}
#endif
