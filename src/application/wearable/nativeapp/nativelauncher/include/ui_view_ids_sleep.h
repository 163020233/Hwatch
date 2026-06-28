/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ui_view_ids_sleep
 * Create: 2025-12
 */

#ifndef UI_VIEW_IDS_SLEEP_H
#define UI_VIEW_IDS_SLEEP_H

/*
 * 用途：Native应用内部组件ID，用于自适应layout或者可测试性需求
 * 命名规范： UIVIEW_模块名_控件类型_控件描述
 * 命名示例： UIVIEW_MONKEY_UILABEL_MONKEY
 * 取值规范： W模块ID_元素ID 模块ID与UX资源保持一致，取值本文件内部顺序排列
 * 取值示例： W001_001
 */

// W008: sleep
#define UIVIEW_SLEEP_UISCROLLVIEW_MAIN "W008_001" // 睡眠首页
#define UIVIEW_SLEEP_UILABELBUTTON_SET_TARGET_TIME "W008_002" // 睡眠-设置目标
#define UIVIEW_SLEEP_UILABELBUTTON_START_SLEEP "W008_003" // 睡眠-设置目标-开始睡眠
#define UIVIEW_SLEEP_UIPICKER_SET_HOUR "W008_004" // 睡眠-设置目标-设置小时
#define UIVIEW_SLEEP_UIPICKER_SET_MINUTE "W008_005" // 睡眠-设置目标-设置分钟
#define UIVIEW_SLEEP_UISCROLLVIEW_SLEEP_DETAILS "W008_006" // 睡眠详情

#endif
