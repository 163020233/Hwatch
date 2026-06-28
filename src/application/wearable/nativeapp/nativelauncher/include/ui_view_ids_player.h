/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ui_view_ids_player
 * Create: 2025-12
 */

#ifndef UI_VIEW_IDS_PLAYER_H
#define UI_VIEW_IDS_PLAYER_H

/*
 * 用途：Native应用内部组件ID，用于自适应layout或者可测试性需求
 * 命名规范： UIVIEW_模块名_控件类型_控件描述
 * 命名示例： UIVIEW_MONKEY_UILABEL_MONKEY
 * 取值规范： W模块ID_元素ID 模块ID与UX资源保持一致，取值本文件内部顺序排列
 * 取值示例： W001_001
 */

// W002: player
#define UIVIEW_PLAYER_UIIMAGEVIEW_NEXT "W002_001" // 下一首
#define UIVIEW_PLAYER_UIIMAGEVIEW_PRE "W002_002" // 上一首
#define UIVIEW_PLAYER_UICHECKBOX_PLAY_OR_PAUSE "W002_003" // 播放或暂停
#define UIVIEW_PLAYER_UIIMAGEVIEW_PLAYMODE "W002_004" // 播放模式
#define UIVIEW_PLAYER_UIIMAGEVIEW_VOICE "W002_005" // 调节音量
#define UIVIEW_PLAYER_UIIMAGEVIEW_PLAY_SRC "W002_006" // 播放源选择
#define UIVIEW_PLAYER_UIIMAGEVIEW_PULL_BUTTON "W002_007" // 上拉按钮
#define UIVIEW_PLAYER_UIBUTTON_A2DP_MUSIC "W002_008" // 蓝牙音乐

#endif
