/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: haid_statistic header file
 * Author: AuthorNameMagicTag
 * Create: 2023-03-30
 */

#ifndef HAID_STATISTIC_H
#define HAID_STATISTIC_H

#include "stdint.h"
#include "stdbool.h"
#include "ux_config.h"
#include "ux_feature_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HAID_MODE_STATISTIC_UNIT 4 // 一个助听模式4个字节表示内容
#define HAID_MODE_STATISTIC_BUFF 100 // 最大支持100组数据, 一组数据4字节
#define HAID_MODE_STATISTIC_TIME_MASK 0xFFFFFF
#define HAID_MODE_STATISTIC_MODE_SHIFT 24
#define MS_TO_MIN 60000

typedef struct {
    uint16_t idx;  /* 已记录助听模式个数 */
    uint8_t flag;  /* 标记是否已满 */
    bool is_valid; /* 标记是否初次使用 */
    uint32_t time[HAID_MODE_STATISTIC_BUFF]; /* 助听模式使用时长顺序记录 */
    uint32_t total_time[SCENE_MODE_MAX];  /* 助听模式使用时长汇总 */
} haid_mode_statistic_t;

void haid_statistic_init(void);
haid_mode_statistic_t *haid_get_mode_statistic(void);
void haid_write_mode_statistic_to_nv(void);
uint16_t haid_get_mode_statistic_cnt(void);
void haid_update_mode_statistic(uint8_t new_mode, bool is_need_statis);
void haid_update_total_time_statistic(uint8_t mode);
void haid_clear_mode_order_statistic(void);
void haid_clear_mode_sum_statistic(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
