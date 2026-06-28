/**
 * @file zlix_decode.c
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief \n
 * 提供zlix解压缩的关键对外接口，比如压缩初始化、去初始化、压缩、压缩重置以及获取压缩输出缓存区大小等关键接口
 * @author Anonym
 * @date 2021-10-22
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2021-10-22  <td>0.1.0    <td>Anonym    <td>解压缩关键接口
 * </table>
 *
 * *******************************************************************************************
 */
#include "drb_zlix.h"
#include "zlix_public_inner.h"

int32_t ZlixStreamDecodeInit(const ZlixConfigInfo *usrCfgInfo, ZlixStream *streamInfo)
{
    int method;
    ZlixStreamOperFuncs *zlixStreamOprTbl;

    /* 入参判断 */
    if ((usrCfgInfo == NULL) || (streamInfo == NULL)) {
        return ZLIX_INPUT_PARA_INVALID;
    }

    if (usrCfgInfo->method >= ZLIX_ALGO_BUTT) {
        return ZLIX_FEATURE_NO_SUPPORT;
    }

    method = usrCfgInfo->method;
    zlixStreamOprTbl = ZlixStreamOperTblGet();
    if (zlixStreamOprTbl[method].decodeResInit == NULL) {
        return ZLIX_FEATURE_NO_SUPPORT;
    }

    return zlixStreamOprTbl[method].decodeResInit(usrCfgInfo, streamInfo);
}

int32_t ZlixStreamDecode(int32_t flushType, ZlixStream *streamInfo)
{
    ZlixAlgo method;
    ZlixStreamOperFuncs *zlixStreamFuncTbl;
    /* 入参判断 */
    if ((streamInfo == NULL) || (streamInfo->method >= ZLIX_ALGO_BUTT)) {
        return ZLIX_INPUT_PARA_INVALID;
    }

    method = streamInfo->method;
    zlixStreamFuncTbl = ZlixStreamOperTblGet();
    if (zlixStreamFuncTbl[method].decodeStream == NULL) {
        return ZLIX_FEATURE_NO_SUPPORT;
    }
    return zlixStreamFuncTbl[method].decodeStream(flushType, streamInfo);
}

int32_t ZlixStreamDecodeFini(ZlixStream *streamInfo)
{
    ZlixAlgo method;
    ZlixStreamOperFuncs *zlixStreamFuncTbl;
    /* 入参判断 */
    if ((streamInfo == NULL) || (streamInfo->method >= ZLIX_ALGO_BUTT)) {
        return ZLIX_INPUT_PARA_INVALID;
    }

    method = streamInfo->method;
    zlixStreamFuncTbl = ZlixStreamOperTblGet();
    if (zlixStreamFuncTbl[method].decodeResFini == NULL) {
        return ZLIX_FEATURE_NO_SUPPORT;
    }
    return zlixStreamFuncTbl[method].decodeResFini(streamInfo);
}
