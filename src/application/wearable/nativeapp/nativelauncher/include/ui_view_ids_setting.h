/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: ui_view_ids_setting
 * Create: 2026-02
 */

#ifndef UI_VIEW_IDS_SETTING_H
#define UI_VIEW_IDS_SETTING_H

/*
 * 用途：Native应用内部组件ID，用于自适应layout或者可测试性需求
 * 命名规范： UIVIEW_模块名_控件类型_控件描述
 * 命名示例： UIVIEW_MONKEY_UILABEL_MONKEY
 * 取值规范： W模块ID_元素ID 模块ID与UX资源保持一致，取值本文件内部顺序排列
 * 取值示例： W001_001
 */

// W017: setting
#define UIVIEW_SETTING_UILABELBUTTON_RESEARCH_BUTTON "W017_001" // 主界面-蓝牙界面-重新搜索
#define UIVIEW_SETTING_UISIMPLELIST_OPTION_LIST "W017_002" // 主界面-蓝牙界面-选项列表(包含：连接到新手机，二维码连接)
#define UIVIEW_SETTING_UISIMPLELIST_BT_LIST "W017_003" // 主界面-蓝牙界面-蓝牙设备列表
#define UIVIEW_SETTING_UIIMAGEVIEW_CONNECT_BLUE_FAIL_CONFIRM_BUTTON "W017_004" // 主界面-蓝牙界面-蓝牙连接失败界面-确认
#define UIVIEW_SETTING_UIIMAGEVIEW_CONNECT_BLUE_SUCC_CONFIRM_BUTTON "W017_005" // 主界面-蓝牙界面-蓝牙连接成功界面-确认
#define UIVIEW_SETTING_UIIMAGEVIEW_CONNECT_OPEN_CONFIRM_BUTTON "W017_006" // 主界面-蓝牙界面-连接到新手机界面-确认
#define UIVIEW_SETTING_UIIMAGEVIEW_CONNECT_NEWPHONE_CONFIRM_BUTTON "W017_007" // 主界面-蓝牙界面-连接到新手机界面-确认操作
#define UIVIEW_SETTING_UIIMAGEVIEW_CONNECT_NEWPHONE_CANCEL_BUTTON "W017_008" // 主界面-蓝牙界面-连接到新手机界面-取消操作
#define UIVIEW_SETTING_UIIMAGEVIEW_RECONNECT_BUTTON "W017_009" // 主界面-蓝牙界面-连接蓝牙设备界面-连接操作(重新连接，断开连接)
#define UIVIEW_SETTING_UIIMAGEVIEW_UNPAIR_BUTTON "W017_0010" // 主界面-蓝牙界面-连接蓝牙设备界面-取消配对
#define UIVIEW_SETTING_UIIMAGEVIEW_RECONNECT_CONFIRM_BUTTON "W017_011" // 主界面-蓝牙界面-连接蓝牙设备界面-确认连接操作
#define UIVIEW_SETTING_UIIMAGEVIEW_RECONNECT_CANCEL_BUTTON "W017_012" // 主界面-蓝牙界面-连接蓝牙设备界面-取消连接操作
#define UIVIEW_SETTING_UIIMAGEVIEW_UNPAIR_CONFIRM_BUTTON "W017_013" // 主界面-蓝牙界面-连接蓝牙设备界面-确认取消配对操作
#define UIVIEW_SETTING_UIIMAGEVIEW_UNPAIR_CANCEL_BUTTON "W017_014" // 主界面-蓝牙界面-连接蓝牙设备界面-取消取消配对操作
#define UIVIEW_SETTING_UIBUTTON_DISPLAY_BRI_ADD_BUTTON "W017_015" // 主界面-显示与亮度界面-调节亮度界面-增加亮度
#define UIVIEW_SETTING_UIBUTTON_DISPLAY_BRI_SUB_BUTTON "W017_016" // 主界面-显示与亮度界面-调节亮度界面-减小亮度
#define UIVIEW_SETTING_UIBUTTON_DISPLAY_SCREEN_OFF_CONFIRM_BUTTON "W017_017" // 主界面-显示与亮度界面-熄屏时间界面-确认
#define UIVIEW_SETTING_UIBUTTON_DISPLAY_STEADY_ON_CONFIRM_BUTTON "W017_018" // 主界面-显示与亮度界面-长亮时刻界面-确认
#define UIVIEW_SETTING_UIBUTTON_CARD_ADD_BUTTON "W017_019" // 主界面-卡片界面-卡片管理-添加卡片
#define UIVIEW_SETTING_UIBUTTON_CARD_DEL_BUTTON "W017_020" // 主界面-卡片界面-卡片管理-删除卡片
#define UIVIEW_SETTING_UISLIDER_SYSTEM_SLIDER "W017_021" // 主界面-系统和升级界面-系统操作界面(关机，重启，恢复出厂设置)-滑块
#define UIVIEW_SETTING_UIBUTTON_SYSTEM_CANCEL_BUTTON "W017_022" // 主界面-系统和升级界面-系统操作界面-取消操作

#endif
