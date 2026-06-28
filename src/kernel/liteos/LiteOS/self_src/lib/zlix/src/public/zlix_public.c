/**
 * @file zlix_public.c
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief \n
 * 提供zlix公共数据和操作
 * @author Anonym
 * @date 2023-03-23
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author     <th>Description
 * <tr><td>2023-04-19  <td>0.1.0    <td>Anonym     <td>提供zlix公共数据和操作
 * </table>
 *
 * *******************************************************************************************
 */
#include <stdlib.h>
#include "zlix_public_inner.h"

static const char *g_zlixVersion = "zlix 1.4.1";

/* zlix内存操作 */
ZlixMemFuncs g_zlixMemOper = {NULL, NULL};

/* 流压缩基础操作接口 */
ZlixStreamOperFuncs g_zlixStreamOprTbl[ZLIX_ALGO_BUTT] = {
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
};

const char *ZlixVersionGet(void)
{
    return g_zlixVersion;
}

void *ZlixDefaultLabelMallocFunc(uint32_t label, size_t size, const char *fileName, uint32_t line)
{
    (void)label;
    return g_zlixMemOper.zlixAlloc(size, fileName, line);
}

void ZlixDefaultLabelFreeFunc(uint32_t label, void *addr, const char *fileName, uint32_t line)
{
    (void)label;
    return g_zlixMemOper.zlixFree(addr, fileName, line);
}

ZlixLabelMemFuncs g_zlixLabelMemOper = {ZlixDefaultLabelMallocFunc, ZlixDefaultLabelFreeFunc};

void *ZlixMalloc(size_t size, const char *fileName, uint32_t line)
{
    (void)fileName;
    (void)line;

    return malloc(size);
}

void ZlixFree(void *addr, const char *fileName, uint32_t line)
{
    (void)fileName;
    (void)line;

    free(addr);
}

int32_t ZlixMemFuncsCheck(const ZlixConfigInfo *usrCfgInfo, ZlixMemFuncs *zlixMemOper)
{
    /* 内存管理配置 */
    if ((usrCfgInfo->zlixMalloc != NULL) && (usrCfgInfo->zlixFree != NULL)) {
        zlixMemOper->zlixAlloc = usrCfgInfo->zlixMalloc;
        zlixMemOper->zlixFree = usrCfgInfo->zlixFree;
    } else {
        zlixMemOper->zlixAlloc = ZlixMalloc;
        zlixMemOper->zlixFree = ZlixFree;
    }
    return ZLIX_OK;
}
