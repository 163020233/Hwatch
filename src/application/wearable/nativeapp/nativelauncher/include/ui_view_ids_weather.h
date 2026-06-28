/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ui_view_ids_weather
 * Create: 2025-12
 */

#ifndef UI_VIEW_IDS_WEATHER_H
#define UI_VIEW_IDS_WEATHER_H

/*
 * 用途：Native应用内部组件ID，用于自适应layout或者可测试性需求
 * 命名规范： UIVIEW_模块名_控件类型_控件描述
 * 命名示例： UIVIEW_MONKEY_UILABEL_MONKEY
 * 取值规范： W模块ID_元素ID 模块ID与UX资源保持一致，取值本文件内部顺序排列
 * 取值示例： W001_001
 */

// W006: weather
#define UIVIEW_WEATHER_UIVIEWGROUP_MAIN "W006_001" // 天气首页
#define UIVIEW_WEATHER_UISWIPEVIEW_24HOUR "W006_002" // 天气24小时预报页面
#define UIVIEW_WEATHER_UIVIEWGROUP_8_24HOUR "W006_003" // 天气24小时预报页面，第一个8h
#define UIVIEW_WEATHER_UIVIEWGROUP_16_24HOUR "W006_004" // 天气24小时预报页面，第二个8h
#define UIVIEW_WEATHER_UIVIEWGROUP_24_24HOUR "W006_005" // 天气24小时预报页面，第三个8h
#define UIVIEW_WEATHER_UIVIEWGROUP_SUNRISE_MAIN "W006_006" // 天气日出日落页面，首页
#define UIVIEW_WEATHER_UIBUTTON_SUNRISE_LEFT "W006_007" // 天气日出日落页面，左箭头
#define UIVIEW_WEATHER_UIBUTTON_SUNRISE_RIGHT "W006_008" // 天气日出日落页面，右箭头
#define UIVIEW_WEATHER_UIVIEWGROUP_MOONRISE_MAIN "W006_009" // 天气月出月落页面，首页
#define UIVIEW_WEATHER_UIBUTTON_MOONRISE_LEFT "W006_010" // 天气月出月落页面，左箭头
#define UIVIEW_WEATHER_UIBUTTON_MOONRISE_RIGHT "W006_011" // 天气月出月落页面，右箭头
#define UIVIEW_WEATHER_UIVIEWGROUP_LUNARPHASE_MAIN "W006_012" // 天气月相页面，首页
#define UIVIEW_WEATHER_UIBUTTON_LUNARPHASE_LEFT "W006_013" // 天气月相页面，左箭头
#define UIVIEW_WEATHER_UIBUTTON_LUNARPHASE_RIGHT "W006_014" // 天气月相页面，右箭头


#endif
