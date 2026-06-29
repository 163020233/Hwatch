/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux led solution \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */
#ifndef UX_BOX_H
#define UX_BOX_H


#include "stdint.h"
#include "errcode.h"
#include "ux_api_box.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    ux_api_box_cover_state_t cover_state;                        /* 记录盒盖状态 */
    ux_api_box_device_position_t device_position;                       /* 记录设备相对盒子的位置 */
} ux_box_info_t;

/**
 * @brief ux_box模块设置盒盖状态
 * @param ux_api_box_cover_state_t 盒盖状态，分为开盖、关盖、无效状态
 * @retval errcode_t 设置盒盖状态是否成功;ERRCODE_SUCC: 成功; 其他: 失败
 */
errcode_t ux_box_set_cover_status(ux_api_box_cover_state_t new_state);

/**
 * @brief ux_box模块设置设备相对盒子的位置
 * @param ux_api_box_device_position_t 设备相对盒子的位置，分为设备在盒子里、设备在盒子外面、无效位置
 * @retval errcode_t 设置设备相对盒子的位置是否成功;ERRCODE_SUCC: 成功; 其他: 失败
 */

errcode_t ux_box_set_device_position(ux_api_box_device_position_t new_position);

/**
 * 获取盒子盖子状态
 * @return 盒子盖子状态枚举值
 */
ux_api_box_cover_state_t ux_box_get_cover_status(void);

/**
 * 获取设备位置信息
 * @return 设备位置信息枚举值
 */
ux_api_box_device_position_t ux_box_get_device_position(void);

/**
 * @brief box_solution初始化
 * @param void
 * @retval 是否初始化成功;ERRCODE_SUCC: 成功;其他: 失败
 */
errcode_t ux_box_init(void);

/**
 * @brief box_solution去初始化
 * @param void
 * @retval 是否去初始化成功;ERRCODE_SUCC: 成功;其他: 失败
 */
errcode_t ux_box_deinit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
