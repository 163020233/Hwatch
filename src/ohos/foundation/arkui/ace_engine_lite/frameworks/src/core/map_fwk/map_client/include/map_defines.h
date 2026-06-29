/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map defines.
 * Author: CompanyName
 * Create: 2026-02-27
 */

// 地图数据模型定义
#ifndef MAP_DEFINES_H
#define MAP_DEFINES_H
#include "map_common_defines.h"
#include "map_render_adapter.h"

namespace OHOS {
namespace ACELite {

// UIKIT初始化环境
typedef struct {
    // 待根据实现情况增加上层要传递的参数，看是直接由适配层写死，还是怎么处理
    map_pixel_mode tile_pixel_mode;
    map_render_adapter render_adapter;    //!< 绘制相关代理
    map_render_callback render_callback;  // 地图绘制生命周期回调
} map_context;

/**
 * @brief 视口尺寸结构
 */
typedef struct {
    int32_t width;   //!< 视口宽度，单位：像素
    int32_t height;  //!< 视口高度，单位：像素
} map_view_port;

/**
 * @brief 画布点坐标int64_t
 */
typedef struct {
    int64_t x;
    int64_t y;
} map_point_64;

/**
 * @brief 地图view的锚点
 *
 */
typedef struct {
    float anchor_x;  //!< 锚点x位置，取值[0.0,1.0]
    float anchor_y;  //!< 锚点y位置，取值[0.0,1.0]
} map_view_anchor;

/**
 * @brief 地图姿态数据结构
 */
typedef struct {
    map_coord2d map_center;      //!< 地图中心点
    map_view_port view_port;     //!< 视口尺寸
    map_view_port drawing_port;  //!< 绘制的视口尺寸
    map_view_anchor anchor;
    float scale;       //!< 缩放比例
    float level;       //!< 地图比例尺, 范围：3~20
    float roll_angle;  //!< 地图旋转角, [0-360)，正北为0，顺时针为正
    map_rect_area map_view_rect;
} map_posture;

/**
 * @brief 地图Overlay几何图元类型
 */
typedef enum {
    MAP_OVERLAY_TYPE_POINT,       //!< 点类型
    MAP_OVERLAY_TYPE_LINE,        //!< 线类型
    MAP_OVERLAY_TYPE_POLYGON,     //!< 面类型
    MAP_OVERLAY_TYPE_TRACK_NAVI,  //!< 轨迹导航
    MAP_OVERLAY_TYPE_UNKNOWN      //!< 未设定
} map_overlay_type;

typedef enum {
    MAP_TILE_STYLE_STANDARD_GRID = 0,  //!< 使用1x的栅格地图
    MAP_TILE_SATELLITE = 1,            //!< 使用1x的卫星地图
    MAP_TILE_STYLE_GRID_AND_POI = 2,   //!< 使用1x的无poi文案的底图，配上动态绘制的poi文字
    MAP_TILE_STYLE_VECTOR = 3,         //!< 使用矢量地图
    MAP_TILE_STYLE_ROAD_AND_POI = 4,   //!< 路网加上poi，配上动态绘制的poi文字
    MAP_TILE_STYLE_ROAD_GRID = 5,      //!< 路网栅格图
    MAP_TILE_STYLE_IMAGE_BINARY = 6,   //!< 使用图片二进制数据
} map_tile_style;

/**
 * @brief 地图mapview参数
 */
typedef struct {
    map_view_port port;  //!< 视口宽高
    map_rect_area map_view_rect;
} map_view_param;

// 视图创建参数，一次性提交地图view创建所需内容，避免多个函数调用
typedef struct {
    // input
    map_view_param view_param;
    map_coord2d map_center;
    float zoom;
} map_view_create_param;

/**
 * @brief 纹理数据结构
 */
typedef map_bitmap map_texture_data;

/**
 * @brief 点纹理信息
 */
typedef struct {
    int32_t texture_id;  //!< 纹理id
    float anchor_x;      //!< 锚点x位置，取值[0.0,1.0]
    float anchor_y;      //!< 锚点y位置，取值[0.0,1.0]
    float scale;         //!< 缩放比例
} map_point_texture_info;

/**
 * @brief 点纹理信息
 */
typedef struct {
    map_point_texture_info icon_texture;    //!< icon纹理
    map_point_texture_info name_texture;    //!< name纹理
    map_point_texture_info bubble_texture;  //!< 气泡纹理
} map_point_overlay_marker;

/**
 * @brief 地图覆盖物基类
 */
typedef struct {
    map_overlay_type geometry_type;
    int32_t group_id;  //!< 组id，同一个组且类型相同的覆盖物的焦点态是互斥的
    int32_t map_id;    //!< overlay所属的地图实例id
    int32_t priority;  //!< 绘制优先级
    bool visible;      //!< 是否可见
    bool is_focus;     //!< 是否存于焦点态
    float min_level;   //!< 可显示的最小地图级别
    float max_level;   //!< 可显示的最大地图级别

    int32_t guid;
} map_base_overlay;

/**
 * @brief 地图点覆盖物
 */
typedef struct {
    map_base_overlay base_overlay;
    map_coord2d position;  //!< 点的经纬度坐标
    float angle;           //!< 旋转角度，取值[0-360) ，正东为0，逆时针为正
    map_point_overlay_marker
        normal_marker;  //!< 普通状态下的纹理信息，当base_overlay.is_focus=false时，必须设置，否则会无法显示
    map_point_overlay_marker
        focus_marker;  //!< 焦点状态下的纹理信息，当base_overlay.is_focus=true时，必须设置，否则会无法显示
    bool icon_visible;    //!< icon图标是否可见
    bool name_visible;    //!< name图标是否可见
    bool bubble_visible;  //!< 气泡图标是否可见
} map_point_overlay;

/**
 * @brief   polyline 纹理资源
 * @remark  polyline overlay item显示2条压盖在一起的线：外部描边线、内部填充线
 *          绘制的时候先绘制外部描边线、后绘制内部填充线
 */
typedef struct {
    int32_t line_width;  //!< 线的宽度，单位：像素
    int32_t line_color;  //!< 线的颜色，ARGB

    int32_t border_width;  //!< 边框的宽度，单位：像素
    int32_t border_color;  //!< 边框的颜色，ARGB

    int32_t dash_painted_length;    //!< 绘制长度
    int32_t dash_unpainted_length;  //!< 空白长度
    int32_t dash_offset;            //!< 起始位置

    bool use_dash;  //!< 是否使用虚线
} map_line_overlay_marker;

/**
 * 关键点标识数据标识 标志位1Byte
 */
typedef enum {
    MAP_POINT_FLAG_DEFAULT = 0x00,       //!< 默认值
    MAP_POINT_FLAG_NORMAL_POINT = 0x01,  //!< 普通点
    MAP_POINT_FLAG_KEY_POINT = 0x02,     //!< 关键点
} map_point_flag;

/**
 * @brief 地图线覆盖物
 */
typedef struct {
    map_base_overlay base_overlay;
    map_coord2d *points;         //!< 点序列经纬度坐标
    map_point_64 *map_points;    //!< 点序列地图坐标
    size_t point_size;           //!< 点的数量
    uint8_t *point_flags;        //!< 关键点标识数据(二维坐标点标识数据), 值见枚举map_point_flag
    size_t point_flags_size;     //!< 关键点标识的数量
    int32_t tolerance;           //!< 点偏离直线的容忍度（单位：像素），默认是10像素
    size_t max_keep_point_size;  //!< 点经过抽稀后保留的最大数量
    map_line_overlay_marker
        normal_marker;  //!< 普通状态下的纹理信息，当base_overlay.is_focus=false时，必须设置，否则会无法显示
    map_line_overlay_marker
        focus_marker;  //!< 焦点状态下的纹理信息，当base_overlay.is_focus=true时，必须设置，否则会无法显示
    map_point_overlay_marker arrow_marker;  //!< 鱼骨箭头纹理信息
    bool show_arrow;                        //!< 是否显示鱼骨箭头
    uint32_t pre_index;                     //!< 已走过路线点的索引
    map_coord2d location;                   //!< 当前位置
} map_polyline_overlay;

/**
 * @brief 地图线面覆盖物
 */
typedef struct {
    map_base_overlay base_overlay;
    map_coord2d *points;  //!< 面的点序列经纬度坐标
    size_t point_size;    //!< 点的数量
    int32_t color;        //!< 填充颜色, ARGB
} map_polygon_overlay;

/**
 * @brief 边缘插入
 *
 */
typedef struct {
    int32_t top;
    int32_t left;
    int32_t bottom;
    int32_t right;
} map_edge_insets;

/**
 * @brief 导航点边界结构体
 *
 */
typedef struct {
    map_coord2d north_east;  //!< 东北角坐标
    map_coord2d south_west;  //!< 西南角坐标
} map_navi_point_bounds;

typedef struct {
    char *keytag;         // poi标识，用于确认poi信息类型
    char *tel;            // poi的联系电话
    char *opentime_week;  // poi营业时间描述
    char *business_area;  // poi所属商圈
} map_business;

typedef struct {
    map_coord2d entr_location;  // 入口经纬度坐标
} map_navi;

typedef struct {
    char *name;             // poi名称
    char *id;               // poi唯一标识
    char *type;             // poi所属类型
    char *pname;            // poi所属省份
    char *cityname;         // poi所属城市
    char *adname;           // poi所属区县
    char *address;          // poi详细地址
    map_coord2d point;      // 经纬度
    map_navi navi;          // 导航位置相关信息
    map_business business;  // poi商业信息
} map_poi_info;

typedef struct {
    map_poi_info *poi_info;  // 一组poi的信息
    int count;               // 获取poi数量
    int infocode;            // 状态码
} map_poi;

/************** tacker navigation *******************/

/**
 * @brief 转向信息结构
 *
 */
typedef struct {
    int32_t index;                //!< 转向点在轨迹点上的索引
    float bear;                   //!< 转向角度
    char *turn_message;           //!< 转向信息
    char *turn_location_message;  //!< 转向位置信息
} map_track_turn_point_info;

/**
 * @brief 轨迹信息结构
 *
 */
typedef struct {
    char *name;                              //!< 轨迹名称
    char *desc;                              //!< 轨迹描述
    map_coord2d *points;                     //!< 轨迹点的集合
    size_t point_size;                       //!< 轨迹点的数量
    int32_t *via_point_indexes;              //!< 途径点在轨迹点上的索引集合
    size_t via_point_index_size;             //!< 途径点在轨迹点上的索引数量
    map_track_turn_point_info *turn_points;  //!< 转向点集合
    size_t turn_point_size;                  //!< 转向点数量
} map_track_info;

typedef struct {
    map_track_info **tracks;
    int32_t count;
} map_track_info_count;
/**
 * @brief 导航事件类型
 *
 */
typedef enum {
    /**
     * @brief 轨迹导航到达事件
     *
     */
    MAP_TRACK_NAVI_EVENT_ARRIVED,
    /**
     * @brief 轨迹导航到达周边事件
     *
     */
    MAP_TRACK_NAVI_EVENT_AROUNDING
} map_track_navi_event_type;

/**
 * @brief 导航点类型
 *
 */
typedef enum {
    /**
     * @brief 轨迹导航点类型,起点
     *
     */
    MAP_TRACK_POINT_START,
    /**
     * @brief 轨迹导航点类型,途经点
     *
     */
    MAP_TRACK_POINT_PASS,
    /**
     * @brief 轨迹导航点类型,转向点
     *
     */
    MAP_TRACK_POINT_TURN,
    /**
     * @brief 轨迹导航点类型,终点
     *
     */
    MAP_TRACK_POINT_FINISH
} map_track_point_type;

/**
 * @brief 导航事件信息
 *
 */
typedef struct {
    map_track_navi_event_type event_type;  //!< 到达事件类型
    map_track_point_type point_type;       //!< 导航点类型
    int32_t point_index;                   //!< 点的索引
    double distance;                       //!< 距离
    char *turn_message;                    //!< 转向信息
    char *turn_location_message;           //!< 转向位置信息
    float bear;                            //!< 转向角度
} map_track_navi_event_info;

/**
 * @brief 轨迹导航回调监听
 *
 */
typedef struct {
    /**
     * @brief 循迹导航走过点的回调
     *
     */
    void (*on_track_pass_point)(int32_t index, map_coord2d point);

    /**
     * @brief 偏航回调
     *
     */
    void (*on_trajector_yaw)(double distance);

    /**
     * @brief 轨迹导航偏航结束
     *
     */
    void (*on_trajector_yaw_end)();

    /**
     * @brief 导航事件回调
     * @param {map_track_navi_event_info} *event
     * @return {*}
     */
    void (*on_navi_event)(map_track_navi_event_info *event);
} map_track_navigation_listener;

/**
 * @brief 轨迹信息结构
 *
 */
typedef struct {
    float point_correcttion_threshold;    //!< 轨迹纠偏阈值,小于阈值使用轨迹上的点
    float navi_event_distance_threshold;  //!< 导航事件距离阈值,小于阈值触发,接近，到达都生效
    map_point_overlay_marker *pass_point_icon;  //!< 途经点图标
    size_t pass_point_icon_size;                //!< 途经点图标数量
    map_point_overlay_marker *turn_point_icon;  //!< 转向点图标
    size_t turn_point_icon_size;                //!< 转向点图标数量
    double *pass_point_around_threshold;        //!< 途经点接近通知阈值
    size_t pass_point_around_threshold_size;    //!< 途经点接近通知阈值数量
    double *turn_point_around_threshold;        //!< 转向点接近通知阈值
    size_t turn_point_around_threshold_size;    //!< 转向点接近通知阈值数量
    int32_t navi_calc_sampling;                 //!< 导航计算采样窗口点数量
    uint32_t tracking_mode;
} map_track_navi_options;

/**
 * @brief 偏航相关参数
 *
 */
typedef struct {
    double yaw_threshold;                         //!< 偏航阈值
    bool is_show_yaw_polyline;                    //!< 是否显示偏航线
    map_line_overlay_marker yaw_polyline_marker;  //!< 偏航线覆盖物纹理信息
    map_point_overlay_marker arrow_marker;        //!< 鱼骨箭头纹理信息
    bool show_arrow;                              //!< 是否显示鱼骨箭头
} map_track_yaw_options;

/**
 * @brief 实走轨迹参数
 *
 */
typedef struct {
    map_line_overlay_marker real_polyline_marker;  //!< 实际线覆盖物纹理信息
    map_point_overlay_marker arrow_marker;         //!< 鱼骨箭头纹理信息
    bool show_arrow;                               //!< 是否显示鱼骨箭头
} map_track_real_polyline_options;

/**
 * @brief 规划轨迹参数
 *
 */
typedef struct {
    map_line_overlay_marker plan_polyline_marker;  //!< 规划线覆盖物纹理信息
    map_point_overlay_marker arrow_marker;         //!< 鱼骨箭头纹理信息
    bool show_arrow;                               //!< 是否显示鱼骨箭头
    uint8_t *point_flags;     //!< 关键点标识数据(二维坐标点标识数据), 值见枚举map_point_flag
    size_t point_flags_size;  //!< 关键点标识的数量
} map_track_plan_polyline_options;

/**
 * @brief 循迹导航覆盖物
 *
 */
typedef struct {
    map_base_overlay base_overlay;                          //!< base_overlay
    map_track_info *track_info;                             //!< 轨迹信息
    map_point_overlay_marker start_marker;                  //!< 起点的纹理信息
    map_point_overlay_marker end_marker;                    //!< 终点的纹理信息
    int32_t custom_point_point_overlay_id;                  //!< 自定义点图标覆盖物id
    map_track_plan_polyline_options plan_polyline_options;  //!< 规划线覆盖物纹理信息
    map_track_real_polyline_options real_polyline_options;  //!< 实际线覆盖物纹理信息
    map_track_yaw_options yaw_options;                      //!< 偏航参数
    map_track_navigation_listener listener;                 //!< 轨迹导航监听
    map_track_navi_options navi_options;                    //!< 轨迹导航参数
} map_track_navi_overlay;

}  // namespace ACELite
}  // namespace OHOS
#endif  // MAP_DEFINES_H
