/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: ui_view_ids_breath
 * Create: 2026-01
 */

#ifndef UI_VIEW_IDS_BREATH_H
#define UI_VIEW_IDS_BREATH_H

/*
 * 用途：Native应用内部组件ID，用于自适应layout或者可测试性需求
 * 命名规范： UIVIEW_模块名_控件类型_控件描述
 * 命名示例： UIVIEW_MONKEY_UILABEL_MONKEY
 * 取值规范： W模块ID_元素ID 模块ID与UX资源保持一致，取值本文件内部顺序排列
 * 取值示例： W001_001
 */

// W011: breath
#define UIVIEW_BREATH_UIBUTTON_TIMESET_BUTTON "W011_001" // 主界面-时间设置
#define UIVIEW_BREATH_UIBUTTON_RHYTHM_SET_BUTTON "W011_002" // 主界面-节奏设置
#define UIVIEW_BREATH_UIIMAGEVIEW_START_BUTTON "W011_003" // 主界面-开始
#define UIVIEW_BREATH_UIIMAGEVIEW_RHYTHM_OK_BUTTON "W011_004" // 节奏设置界面-确认
#define UIVIEW_BREATH_UIIMAGEVIEW_TIMESET_OK_BUTTON "W011_005" // 时间设置界面-确认
#define UIVIEW_BREATH_UIBUTTON_COMPLETE_BUTTON "W011_006" // 训练结果界面-完成
#define UIVIEW_BREATH_UIBUTTON_RESTART_BUTTON "W011_007" // 训练结果界面-再次训练

#endif
