/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: tws manager interface
 * Author: Media Software Group
 * Create: 2023-04-08
 */

#ifndef TWS_MANAGER_H
#define TWS_MANAGER_H
#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @if Eng
 * @brief  audio TWS mode
 * @else
 * @brief  真无线耳机模式
 * @endif
 */
typedef enum {
    TWS_MODE_NON_TWS,    /*!< @if Eng single-ear mode.
                                       @else   单耳模式 @endif */
 
    TWS_MODE_FORWARD,    /*!< @if Eng TWS mode is forward.
                                       @else   转发模式 @endif */
 
    TWS_MODE_LISTEN,   /*!< @if Eng TWS mode is listen.
                                       @else   协同模式 @endif */
 
    TWS_MODE_DUAL_TRANSMIT, /*!< @if Eng TWS mode is dual transmit.
                                            @else   双发模式 @endif */
 
    TWS_MODE_MAX,
    TWS_MODE_BUTT = 0xFFFFFFFF,
} TwsMode;
 
/**
 * @if Eng
 * @brief  audio TWS role
 * @else
 * @brief  真无线耳机角色
 * @endif
 */
typedef enum {
    TWS_ROLE_MASTER,   /*!< @if Eng TWS role is master.
                                       @else 真无线耳机主耳 @endif */
 
    TWS_ROLE_SLAVE,     /*!< @if Eng TWS role is slave.
                                       @else 真无线耳机副耳 @endif */
 
    TWS_ROLE_FREEMAN,  /*!< @if Eng TWS role is freeman.
                                       @else 真无线耳机自由播放 @endif */
 
    TWS_ROLE_MAX,
    TWS_ROLE_BUTT = 0xFFFFFFFF,
} TwsRole;

/**
 * @if Eng
 * @brief  audio TWS channel
 * @else
 * @brief  真无线耳机单耳模式
 * @endif
 */
typedef enum {
    TWS_CHANNEL_STEREO_MODE,   /*!< @if Eng Stereo Channel Mode.
                                       @else 立体声声道模式 @endif */
 
    TWS_CHANNEL_MONO_MODE,    /*!< @if Eng Mono mode.
                                       @else 单声道模式 @endif */
    TWS_CHANNEL_MAX = 0xFFFFFFFF,
} TwsChannelMode;

/**
 * @if Eng
 * @brief  audio TWS channel
 * @else
 * @brief  真无线耳机音频产品形态
 * @endif
 */
typedef enum {
    TWS_PRODUCT_FORM_LEFT,   /*!< @if Eng product form is only left channel.
                                             @else   产品形态为真无线耳机左耳 @endif */
    TWS_PRODUCT_FORM_RIGHT,  /*!< @if Eng product form is only right channel.
                                             @else   产品形态为真无线耳机右耳 @endif */
    TWS_PRODUCT_FORM_NON,    /*!< @if Eng product form is not tws.
                                             @else   产品形态不是真无线耳机 @endif */
    TWS_PRODUCT_FORM_MAX = 0xFFFFFFFF,
} TwsProductForm;

/**
 * @if Eng
 * @brief Set audio tws(True Wireless Stereo) mode.
 * @param [in] twsMode True wireless earphone working mode {@link TwsMode}.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief 设置真无线耳机工作模式。
  * @param [in] twsMode 真无线耳机工作模式  {@link TwsMode}。
 * @return 返回0成功，其他值失败。
 * @endif
 */
errcode_t TwsSetMode(uint32_t twsMode);

/**
 * @if Eng
 * @brief Set audio tws(True Wireless Stereo) role.
 * @param [in] twsRole True Wireless Earbuds Roles  {@link TwsRole}.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief 设置真无线耳机角色。
 * @param [in] twsRole 真无线耳机角色  {@link TwsRole}。
 * @return 返回0成功，其他值失败。
 * @endif
 */
errcode_t TwsSetRole(uint32_t twsRole);

/**
 * @if Eng
 * @brief Set audio tws(True Wireless Stereo) mono mode.
 * @param [in] twsChannelMode True Wireless Earbuds Single Ear Mode  {@link TwsChannelMode}.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief 设置真无线耳机单耳模式。
 * @param [in] twsChannelMode 真无线耳机单耳模式  {@link TwsChannelMode}。
 * @return 返回0成功，其他值失败。
 * @endif
 */
errcode_t TwsSetChannelMode(uint32_t twsChannelMode);
/**
 * @if Eng
 * @brief Set audio product form.
 * @param [in] twsProductForm Audio product form  {@link TwsProductForm}.
 * @return Returns <b>0</b> for success, and other values for failure.
 * @else
 * @brief 设置真无线耳机音频产品形态。
 * @param [in] twsProductForm 产品形态  {@link TwsProductForm}。
 * @return 返回0成功，其他值失败。
 * @endif
 */
errcode_t TwsSetProductForm(uint32_t twsProductForm);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* TWS_MANAGER */