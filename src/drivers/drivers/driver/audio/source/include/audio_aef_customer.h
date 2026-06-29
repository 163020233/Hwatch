/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: api for customer audio effect
 * Author: @CompanyNameTag
 */

#ifndef __AUDIO_AEF_CUSTOMER_H__
#define __AUDIO_AEF_CUSTOMER_H__

#include "audio_aef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* 输入buf，最多4块 */
#define AEF_CUSTOMER_INP_BUF_MAX 4
/* 输出buf，最多2块 */
#define AEF_CUSTOMER_OUT_BUF_MAX 2

/* 客户音效数据描述结构体 */
typedef struct {
    td_bool valid; /* buf可用标记，valid=0表示该buf中无数据 */
    td_u32 size;   /* buf大小 */
    td_s32 *addr;  /* buf地址 */
} customer_aef_buf;

/*
 * 客户音效输入输出数据结构体
 * 直播麦RX场景
 * 输入：
 * 4路T节点的数据
 * RX上3.5mm输入的数据 -- 待扩展
 *
 * 输出：
 * RX 3.5mm监听口的数据
 * UAC监听的数据
 */
typedef struct {
    customer_aef_buf inp[AEF_CUSTOMER_INP_BUF_MAX];
    customer_aef_buf out[AEF_CUSTOMER_OUT_BUF_MAX];
} customer_aef_data;

/**
 * @if Eng
 * @brief  customer effect command.
 * @else
 * @brief  客户音效接口命令
 * @endif
 */
enum {
    /*!< @if Eng Read and Write customer effect paramater command.
         @else   获取和设置客户音效参数命令 @endif */
    AEF_CUSTOMER_RW_PARAM_CMD = aef_cmd(AEF_TYPE_CUSTOMER, 0),

    /*!< @if Eng Get customer effect bypass command.
         @else   获取客户音效bypass命令 @endif */
    AEF_CUSTOMER_GET_BYPASS_CMD = aef_cmd(AEF_TYPE_CUSTOMER, 1),

    /*!< @if Eng Set customer effect bypass command.
         @else   设置客户音效bypass命令 @endif */
    AEF_CUSTOMER_SET_BYPASS_CMD = aef_cmd(AEF_TYPE_CUSTOMER, 2),
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AUDIO_AEF_CUSTOMER_H__ */
