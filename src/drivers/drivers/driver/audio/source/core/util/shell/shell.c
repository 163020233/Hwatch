/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: shell.c
 * Author: audio
 */

#ifdef SAP_SHELL_SUPPORT
#include "shell.h"

/*********************************************************************
 * MACROS
 */
#define MEM_WRITE_ARGC  3
#define MEM_READ_ARGC   3

#define ADRESSS_OFFSET 4

static td_s32 mem_write_cmd(td_s32 argc, const td_char *argv[])
{
    td_u32 val;
    td_u32 addr;

    if ((argc != MEM_WRITE_ARGC) || (argv == TD_NULL)) {
        shell_log("cmd input is invalid, please run help cmd for details\n");
        return AUDIO_FAILURE;
    }
    if ((string2int(argv[1], &addr) != 0) || (string2int(argv[2], &val) != 0)) { /* 2 is index */
        shell_log("cmd input is invalid, please run help cmd for details\n");
        return AUDIO_FAILURE;
    }
    if (is_4byte_aligned(addr) != TD_TRUE) {
        shell_log("Invalid address\n");
        return AUDIO_FAILURE;
    }
    audio_reg_write(addr, val);
    shell_log("Writing %08x -> %08x\n", val, addr);

    return AUDIO_SUCCESS;
}
osshell_export_cmd(mem_write_cmd, "w4", "usage example: w4 0x2220F000 0xff");

static td_s32 mem_read_cmd(td_s32 argc, const td_char *argv[])
{
    td_u32 i;
    td_u32 len;
    td_s32 val;
    td_u32 addr;

    if ((argc != MEM_READ_ARGC) || (argv == TD_NULL)) {
        shell_log("cmd input is invalid, please run help cmd for details\n");
        return AUDIO_FAILURE;
    }
    if ((string2int(argv[1], &addr) != 0) || (string2int(argv[2], &len) != 0)) { /* 2 is index */
        shell_log("cmd input is invalid, please run help cmd for details\n");
        return AUDIO_FAILURE;
    }
    if (!is_4byte_aligned(addr)) {
        shell_log("Invalid address\n");
        return AUDIO_FAILURE;
    }

    for (i = 0; i < len; i++) {
        /* 4 output value in a row */
        if (i % 4 == 0) {
            shell_log("\n%08x =", addr);
        }
        audio_reg_read(addr, val);
        shell_log(" %08x", val);
        addr = addr + ADRESSS_OFFSET;
    }

    /* Without ending with '\n',the prtd_s32f function does not automatically
       flush the output stream until the cache is full. */
    shell_log("\n");

    return AUDIO_SUCCESS;
}
osshell_export_cmd(mem_read_cmd, "mem32", "usage example: mem32 0x2220F000 1");
#endif
