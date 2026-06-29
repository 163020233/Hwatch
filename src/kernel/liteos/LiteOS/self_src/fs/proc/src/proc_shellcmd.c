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

#include "string.h"
#include "fcntl.h"
#if defined(LOSCFG_SHELL) && defined(LOSCFG_FS_PROC)
#include "shell.h"
#include "shcmd.h"
#include "internal.h"
#include "proc_fs.h"

#define WRITEPROC_ARGC  3

int
osShellCmdWriteProc(int argc, char **argv)
{
	int i;
	int ret;
	const char *path = NULL;
	const char *value = NULL;
	unsigned int len;
	struct proc_dir_entry *handle = NULL;

	if (argc == WRITEPROC_ARGC) {
		value = argv[0];
		path = argv[2];
		len = strlen(value) + 1;  /* +1:add the \0 */
		if (strncmp(argv[1], ">>", strlen(">>")) == 0) {
			handle = proc_open(path, O_TRUNC);
			if (handle == NULL) {
				PRINT_ERR("No such file or directory\n");
				return PROC_ERROR;
			}

			ret = proc_write(handle, value, len);
			if (ret < 0) {
				(void)proc_close(handle);
				PRINT_ERR("write error\n");
				return PROC_ERROR;
			}
			for (i = 0; i < argc; i++) {
				PRINTK("%s%s", i > 0 ? " " : "", argv[i]);
			}
			PRINTK("\n");
			(void)proc_close(handle);
			return LOS_OK;
		}
	}
	PRINT_ERR("writeproc [data] [>>] [path]\n");
	return PROC_ERROR;
}

SHELLCMD_ENTRY(writeproc_shellcmd, CMD_TYPE_EX, "writeproc", XARGS, (CmdCallBackFunc)osShellCmdWriteProc);
#endif
