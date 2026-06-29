/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ui_view_ids_timers
 * Create: 2025-12
 */

#ifndef UI_VIEW_IDS_TIMERS_H
#define UI_VIEW_IDS_TIMERS_H

/*
 * 用途：Native应用内部组件ID，用于自适应layout或者可测试性需求
 * 命名规范： UIVIEW_模块名_控件类型_控件描述
 * 命名示例： UIVIEW_MONKEY_UILABEL_MONKEY
 * 取值规范： W模块ID_元素ID 模块ID与UX资源保持一致，取值本文件内部顺序排列
 * 取值示例： W001_001
 */

// W005: timers
#define UIVIEW_TIMERS_UIIMAGEVIEW_ONE_MIN "W005_001" // 计时器首页1分钟
#define UIVIEW_TIMERS_UIIMAGEVIEW_THREE_MIN "W005_002" // 计时器首页3分钟
#define UIVIEW_TIMERS_UIIMAGEVIEW_FIFTEEN_MIN "W005_003" // 计时器首页15分钟
#define UIVIEW_TIMERS_UIIMAGEVIEW_THIRTY_MIN "W005_004" // 计时器首页30分钟
#define UIVIEW_TIMERS_UIIMAGEVIEW_CUSTOM_TIME "W005_005" // 计时器首页自定义时间
#define UIVIEW_TIMERS_UIPICKER_SET_HOUR "W005_006" // 计时器设置时间页面，设置时
#define UIVIEW_TIMERS_UIPICKER_SET_MIN "W005_007" // 计时器设置时间页面，设置分
#define UIVIEW_TIMERS_UIPICKER_SET_SEC "W005_008" // 计时器设置时间页面，设置秒
#define UIVIEW_TIMERS_UIIMAGEVIEW_START_COUNTDOWN "W005_009" // 计时器设置时间页面，选择时分秒后，开始倒计时
#define UIVIEW_TIMERS_UIIMAGEVIEW_START_OR_STOP "W005_010" // 计时器页面，开始计时或暂停计时
#define UIVIEW_TIMERS_UIIMAGEVIEW_RESET_OR_EXIT "W005_011" // 计时器页面，重置计时或退出计时


#endif
