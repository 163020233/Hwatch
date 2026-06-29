/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ui_view_ids_stopwatch
 * Create: 2025-12
 */

#ifndef UI_VIEW_IDS_STOPWATCH_H
#define UI_VIEW_IDS_STOPWATCH_H

/*
 * 用途：Native应用内部组件ID，用于自适应layout或者可测试性需求
 * 命名规范： UIVIEW_模块名_控件类型_控件描述
 * 命名示例： UIVIEW_MONKEY_UILABEL_MONKEY
 * 取值规范： W模块ID_元素ID 模块ID与UX资源保持一致，取值本文件内部顺序排列
 * 取值示例： W001_001
 */

// W004: stopwatch
#define UIVIEW_STOPWATCH_UILABELBUTTON_START_PAUSE "W004_001" // 开始计时或暂停计时
#define UIVIEW_STOPWATCH_UILABELBUTTON_RESET_RECORD "W004_002" // 重置计时或分段计时
#define UIVIEW_STOPWATCH_UISIMPLELIST_RECORD_LIST "W004_003" // 计时列表

#endif
