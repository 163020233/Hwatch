/* ---------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2023. All rights reserved.
 * Description: mtd list
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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

#include "linux/mtd/mtd_list.h"
#include <linux/module.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "los_mux.h"

STATIC linklist g_mtdHead = NULL;
STATIC UINT32 g_mtdListMux = 0xFFFFFFFF; /* 0xFFFFFFFF: Invalid mutex id */

struct mtd_info *get_mtd(const char *type)
{
    linklist node = NULL;
    struct mtd_info *mtd = NULL;

    if (type == NULL) {
        return NULL;
    }

    if (LOS_MuxPend(g_mtdListMux, LOS_WAIT_FOREVER) != LOS_OK) {
        return NULL;
    }

    node = g_mtdHead->next;
    while (node) {
        if (strcmp(type, node->type) == 0) {
            node->status++;
            mtd = node->mtd;
            break;
        }
        node = node->next;
    }

    (VOID)LOS_MuxPost(g_mtdListMux);

    return mtd;
}

int get_mtd_info(const char *type)
{
    linklist node = NULL;
    int ret = -1;

    if (type == NULL) {
        return ret;
    }

    if (LOS_MuxPend(g_mtdListMux, LOS_WAIT_FOREVER) != LOS_OK) {
        return ret;
    }

    node = g_mtdHead->next;
    while (node) {
        if (strcmp(type, node->type) == 0) {
            ret = 0;
            break;
        }
        node = node->next;
    }

    (VOID)LOS_MuxPost(g_mtdListMux);

    return ret;
}

int free_mtd(struct mtd_info *mtd)
{
    linklist node = NULL;
    int ret = -1;

    if (mtd == NULL) {
        return ret;
    }

    if (LOS_MuxPend(g_mtdListMux, LOS_WAIT_FOREVER) != LOS_OK) {
        return ret;
    }

    node = g_mtdHead->next;
    while (node) {
        if (node->mtd == mtd) {
            node->status--;
            ret = 0;
            break;
        }
        node = node->next;
    }

    (VOID)LOS_MuxPost(g_mtdListMux);

    return ret;
}

void add_mtd_list(char *type, struct mtd_info *mtd)
{
    linklist new = NULL;

    if ((type == NULL) || (mtd == NULL)) {
        return;
    }

    if (LOS_MuxPend(g_mtdListMux, LOS_WAIT_FOREVER) != LOS_OK) {
        return;
    }

    new = (linklist)zalloc(sizeof(Lnode));
    if (new == NULL) {
        goto OUT;
    }

    new->type = type;
    new->mtd = mtd;
    new->status = 0;
    new->next = g_mtdHead->next;
    g_mtdHead->next = new;

OUT:
    (VOID)LOS_MuxPost(g_mtdListMux);
}

int del_mtd_list(struct mtd_info *mtd)
{
    linklist node = NULL;
    linklist prev = NULL;
    int ret = -1;

    if (mtd == NULL) {
        return ret;
    }

    if (LOS_MuxPend(g_mtdListMux, LOS_WAIT_FOREVER) != LOS_OK) {
        return ret;
    }

    node = g_mtdHead->next;
    prev = g_mtdHead;

    while (node) {
        if (node->mtd == mtd) {
            if (node->status == 0) {
                prev->next = node->next;
                free(node);
                ret = 0;
            }
            break;
        }
        prev = node;
        node = node->next;
    }

    (VOID)LOS_MuxPost(g_mtdListMux);
    return ret;
}

static int mtd_init_list(void)
{
    UINT32 ret;

    g_mtdHead = (linklist)zalloc(sizeof(Lnode));
    if (!g_mtdHead) {
        return -1;
    }

    ret = LOS_MuxCreate(&g_mtdListMux);
    if (ret != LOS_OK) {
        free(g_mtdHead);
        g_mtdHead = NULL;
        return -1;
    }

    return 0;
}

arch_initcall(mtd_init_list);