/**
 * @file zlix_encode.c
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief \n
 * 提供zlix压缩的关键接口，比如压缩初始化、去初始化、压缩、压缩重置以及获取压缩输出缓存区大小等关键接口
 * @author Anonym
 * @date 2021-10-22
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author     <th>Description
 * <tr><td>2021-10-22  <td>0.1.0    <td>Anonym     <td>压缩关键接口
 * </table>P
 *
 * *******************************************************************************************
 */
#include "drb_zlix.h"
#include "zlix_public_inner.h"

/* 压缩所需资源初始化 */
int32_t ZlixStreamEncodeInit(const ZlixConfigInfo *usrConfigInfo, ZlixStream *streamInfo)
{
    ZlixStreamOperFuncs *zlixStreamOprTbl;

    /* 入参判断 */
    if ((usrConfigInfo == NULL) || (streamInfo == NULL)) {
        return ZLIX_INPUT_PARA_INVALID;
    }

    /* 目前只支持Deflate算法，其余算法均会报错 */
    if (usrConfigInfo->method >= ZLIX_ZSTD) {
        return ZLIX_FEATURE_NO_SUPPORT;
    }

    /* 判断zlix流操作表中Deflate相应操作是否有进行过初始化 */
    zlixStreamOprTbl = ZlixStreamOperTblGet();
    if (zlixStreamOprTbl[usrConfigInfo->method].encodeResInit == NULL) {
        return ZLIX_FEATURE_NO_SUPPORT;
    }

    return zlixStreamOprTbl[usrConfigInfo->method].encodeResInit(usrConfigInfo, streamInfo);
}

int32_t ZlixStreamEncode2(int32_t flushType, ZlixStream *streamInfo, ZlixGzHeadInfo *gzHeadInfo)
{
    ZlixStreamOperFuncs *zlixStreamFuncTbl;
    /* 入参判断 */
    if ((streamInfo == NULL) || (streamInfo->method >= ZLIX_ALGO_BUTT)) {
        return ZLIX_INPUT_PARA_INVALID;
    }

    zlixStreamFuncTbl = ZlixStreamOperTblGet();
    if (zlixStreamFuncTbl[streamInfo->method].encodeStream == NULL) {
        return ZLIX_FEATURE_NO_SUPPORT;
    }
    return zlixStreamFuncTbl[streamInfo->method].encodeStream(flushType, streamInfo, gzHeadInfo);
}

int32_t ZlixStreamEncode(int32_t flushType, ZlixStream *streamInfo)
{
    return ZlixStreamEncode2(flushType, streamInfo, NULL);
}

int32_t ZlixStreamEncodeFini(ZlixStream *streamInfo)
{
    ZlixStreamOperFuncs *zlixStreamFuncTbl;
    /* 入参判断 */
    if ((streamInfo == NULL) || (streamInfo->method >= ZLIX_ALGO_BUTT)) {
        return ZLIX_INPUT_PARA_INVALID;
    }

    zlixStreamFuncTbl = ZlixStreamOperTblGet();
    if (zlixStreamFuncTbl[streamInfo->method].encodeResFini == NULL) {
        return ZLIX_FEATURE_NO_SUPPORT;
    }
    return zlixStreamFuncTbl[streamInfo->method].encodeResFini(streamInfo);
}
