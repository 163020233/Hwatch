/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: user api for audio effect
 * Author: @CompanyNameTag
 */

#ifndef __SOC_UAPI_AEF_H__
#define __SOC_UAPI_AEF_H__

#include <stdint.h>
#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @defgroup soc_uapi_aef soc_uapi_aef
 * @ingroup  audio
 * @{
 */

/**
 * @if Eng
 * @brief  Audio effect type.
 * @else
 * @brief  音效类型
 * @endif
 */
typedef enum {
    UAPI_AEF_TYPE_NULL,    /*!< @if Eng NULL audio effect.
                                @else   无效音效 @endif */
    UAPI_AEF_TYPE_SWS,     /*!< @if Eng SWS audio effect.
                                @else   SWS音效 @endif */
    UAPI_AEF_TYPE_SMARTPA, /*!< @if Eng Smart PA audio effect.
                                @else   Smart PA音效 @endif */
    UAPI_AEF_TYPE_WDRC,    /*!< @if Eng WDRC audio effect for hearing aid.
                               @else    助听WDRC音效 @endif */
    UAPI_AEF_TYPE_AVC,     /*!< @if Eng Auto volume control.
                                @else   自动音量控制 @endif */
    UAPI_AEF_TYPE_GEQ,     /*!< @if Eng GEQ audio effect.
                                @else   GEQ音效 @endif */
    UAPI_AEF_TYPE_DRC,     /*!< @if Eng Dynamic range control.
                                @else   音频动态范围控制 @endif */
    UAPI_AEF_TYPE_PEQ,     /*!< @if Eng PEQ audio effect.
                                @else   PEQ音效 @endif */
    UAPI_AEF_TYPE_AEF_PRE, /*!< @if Eng Pre-processing aef audio effect.
                                @else   AEF前处理音效 @endif */
    UAPI_AEF_TYPE_HVS,     /*!< @if Eng hvs lite audio effect.
                                @else   HVS lite音效 @endif */

    UAPI_AEF_TYPE_CUSTOMER = 0x80, /*!< @if Eng Customer audio effect.
                                        @else   客户自研音效 @endif */

    UAPI_AEF_TYPE_MAX
} uapi_aef_type;

/**
 * @if Eng
 * @brief  make audio effect command id.
 * @else
 * @brief  音效接口命令id生成方法
 * @endif
 */
#define uapi_aef_cmd(aef_type, cmd_id) (uint32_t)(((aef_type) << 16) | (cmd_id))

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SOC_UAPI_AEF_H__ */
