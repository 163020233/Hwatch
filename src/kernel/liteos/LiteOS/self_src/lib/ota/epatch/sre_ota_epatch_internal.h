/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: epatch差分还原算法内部头文件
 * Author: LibCK项目组
 * Create: 2025-04-12
 * History: 2025-04-12 LibCK项目组
 */
#ifndef SRE_OTA_EPATCH_INTERNAL_H
#define SRE_OTA_EPATCH_INTERNAL_H

#include "drb_ota.h"
#include "drb_typedef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* epatch头文件信息 */
#define EPATCH_HEADER_LEN                  32U
#define EPATCH_HEADER_VERSION_LEN          8U
#define EPATCH_HEADER_CTRL_BLOCK_LEN       8U
#define EPATCH_HEADER_DIFF_BLOCK_LEN       8U
#define EPATCH_HEADER_NEW_IMAGE_LEN        8U

/* epatch控制块信息 */
#define EPATCH_CTRL_BLOCK_UNIT_DIFF_LEN    8U
#define EPATCH_CTRL_BLOCK_UNIT_EXTRA_LEN   8U
#define EPATCH_CTRL_BLOCK_UNIT_OFFSET_LEN  8U
#define EPATCH_CTRL_BLOCK_UNIT_SIZE        24U

/* epatch组成部分信息 */
#define EPATCH_COMPONENT_TYPE_CTRL_BLOCK   0U
#define EPATCH_COMPONENT_TYPE_DIFF_BLOCK   1U
#define EPATCH_COMPONENT_TYPE_EXTRA_BLOCK  2U
#define EPATCH_COMPONENT_TYPE_NUM          3U

/*
 * @ingroup sre_ota
 * epatch的header信息结构定义。
 *
 * epatch的header信息
 */
struct OsOtaEpatchHeader {
    /* 版本号 */
    char version[8];
    /* ctrl block大小 */
    size_t ctrlBlockSize;
    /* diff block大小 */
    size_t diffBlockSize;
    /* new镜像大小 */
    size_t newImageSize;
};

/*
 * @ingroup sre_ota
 * epatch的单个ctrl block信息结构定义。
 *
 * epatch的单个ctrl block信息
 */
struct OsOtaEpatchCtrlBlockUnit {
    /* diff数据大小 */
    size_t diffSize;
    /* extra数据大小 */
    size_t extraSize;
    /* old镜像的偏移量 */
    off_t oldOffset;
};

extern U32 OsOtaEpatch(const struct OtaPara *para, size_t *newImageSize);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* SRE_OTA_EPATCH_INTERNAL_H */
