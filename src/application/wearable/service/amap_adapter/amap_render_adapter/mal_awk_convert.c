/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: mal awk convert
 * Created: 2026-01-14
 */

#include "mal_awk_convert.h"
#include <stdlib.h>
#include <string.h>
#include "amap_system_adapter.h"
#include "mal_common_defines.h"
#include "mal_map_defines.h"
#include "securec.h"

void print_double_le_bytes(const char *label, double value)
{
    const uint8_t *bytes = (const uint8_t *)&value;

    if (label) {
        printf("%s: ", label);
    }

    // double 是 8 字节，小端序：低地址存最低有效字节
    for (int i = 0; i < 8; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");
}

void print_float_le_bytes(const char *label, float value)
{
    const uint8_t *bytes = (const uint8_t *)&value;

    if (label) {
        printf("%s: ", label);
    }

    // float 是 4 字节，小端序：低地址存最低有效字节
    for (int i = 0; i < 4; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");
}

// 辅助函数：深拷贝字符串（内部使用）
char *mal_strdup(const char *str);

// render部分数据转换(callback awk t0 mal)
mal_render_context_t awk_to_mal_render_context_t(const awk_render_context_t src)
{
    mal_rect_area_t map_view_rect = {.height = src.map_view_rect.height,
                                     .width = src.map_view_rect.width,
                                     .x = src.map_view_rect.x,
                                     .y = src.map_view_rect.y};

    mal_render_context_t dst = {.height = src.height, .width = src.width, .map_view_rect = map_view_rect};
    return dst;
}

mal_point_t awk_to_mal_point_t(const awk_point_t src)
{
    mal_point_t dst = {.x = src.x, .y = src.y};
    return dst;
}

// 指针类型不同，只能做深copy
void awk_to_mal_point_list_t(const awk_point_t *src, uint32_t point_size, mal_point_t *dst)
{
    if (src == NULL || dst == NULL || point_size == 0) {
        return;
    }

    // 因为 awk_point_t 和 mal_point_t 均为 { double x; double y; }，大小相同, 可直接copy
    size_t size = sizeof(mal_point_t) * point_size;
    memcpy_s(dst, size, src, size);
}

void awk_to_mal_paint_style_t(const awk_paint_style_t *src, mal_paint_style_t *dst)
{
    if (src == NULL || dst == NULL) {
        return;
    }

    dst->width = src->width;
    dst->color = src->color;
    dst->angle = src->angle;

    // text_style
    dst->text_style.align = (mal_align_style_t)src->text_style.align;
    dst->text_style.font_size = src->text_style.font_size;
    dst->text_style.font_weight = (mal_font_weight_t)src->text_style.font_weight;

    // fill_style
    dst->fill_style = (mal_fill_style_t)src->fill_style;

    // dash_style
    dst->dash_style.painted_length = src->dash_style.painted_length;
    dst->dash_style.unpainted_length = src->dash_style.unpainted_length;
    dst->dash_style.offset = src->dash_style.offset;
}

mal_rect_area_t awk_to_mal_rect_area_t(const awk_rect_area_t src)
{
    mal_rect_area_t dst = {.x = src.x, .y = src.y, .height = src.height, .width = src.width};
    return dst;
}

mal_bitmap_t awk_to_mal_bitmap_t(const awk_bitmap_t src)
{
    // buffer由调用方管理，只做浅拷贝指针
    mal_bitmap_t dst = {0};
    // 像素格式转换：枚举值一一对应，可直接强转
    dst.pixel_mode = (mal_pixel_mode_t)src.pixel_mode;
    // 浅拷贝 buffer 指针（不复制数据）
    dst.buffer = src.buffer;
    dst.buffer_size = src.buffer_size;
    // 其他字段直接赋值
    dst.width = src.width;
    dst.height = src.height;
    dst.stride = src.stride;
    dst.pre_multiplied = src.pre_multiplied;
    return dst;
}

void awk_to_mal_image_buf_info_t(const awk_image_buf_info_t *src, mal_image_buf_info_t *dst)
{
    // 浅拷贝 buffer 指针（不复制数据）
    dst->buffer = src->buffer;
    dst->buffer_size = src->buffer_size;
    // 枚举值一一对应，可直接强转
    dst->image_format = (mal_image_format_t)src->image_format;
}

// 地图部分数据转换(mal to awk & awk to mal)
awk_map_view_param_t mal_to_awk_map_view_param_t(mal_map_view_param_t src)
{
    awk_map_view_param_t dst = {0};
    dst.port.width = src.port.width;
    dst.port.height = src.port.height;
    dst.map_view_rect.x = src.map_view_rect.x;
    dst.map_view_rect.y = src.map_view_rect.y;
    dst.map_view_rect.width = src.map_view_rect.width;
    dst.map_view_rect.height = src.map_view_rect.height;
    return dst;
}

awk_map_view_port_t mal_to_awk_map_view_port_t(mal_map_view_port_t src)
{
    awk_map_view_port_t dst = {.height = src.height, .width = src.width};
    return dst;
}

awk_map_coord2d_t mal_to_awk_map_coord2d_t(mal_map_coord2d_t src)
{
    awk_map_coord2d_t dst = {.lat = src.lat, .lon = src.lon};
    return dst;
}

void awk_to_mal_map_coord2d_t(awk_map_coord2d_t *src, mal_map_coord2d_t *dst)
{
    if (src == NULL || dst == NULL) {
        return;
    }
    dst->lon = src->lon;
    dst->lat = src->lat;
}

void mal_to_awk_map_texture_data_t(const mal_map_texture_data_t *src, awk_map_texture_data_t *dst)
{
    if (!src || !dst) {
        return;
    }

    dst->pixel_mode = (awk_pixel_mode_t)src->pixel_mode;
    // 直接浅copy
    dst->buffer = src->buffer;
    dst->buffer_size = src->buffer_size;
    dst->width = src->width;
    dst->height = src->height;
    dst->stride = src->stride;
    dst->pre_multiplied = src->pre_multiplied;
}

static void copy_base_overlay_to_awk(const mal_map_base_overlay_t *src, awk_map_base_overlay_t *dst)
{
    dst->geometry_type = (awk_map_overlay_type)src->geometry_type;
    dst->group_id = src->group_id;
    dst->map_id = src->map_id;
    dst->priority = src->priority;
    dst->visible = src->visible;
    dst->is_focus = src->is_focus;
    dst->min_level = src->min_level;
    dst->max_level = src->max_level;
    dst->guid = src->guid;
}

static void copy_line_marker_to_awk(const mal_map_line_overlay_marker_t *src, awk_map_line_overlay_marker_t *dst)
{
    dst->line_width = src->line_width;
    dst->line_color = src->line_color;
    dst->border_width = src->border_width;
    dst->border_color = src->border_color;
    dst->dash_painted_length = src->dash_painted_length;
    dst->dash_unpainted_length = src->dash_unpainted_length;
    dst->dash_offset = src->dash_offset;
    dst->use_dash = src->use_dash;
}

static void copy_texture_to_awk(const mal_map_point_texture_info_t *src, awk_map_point_texture_info_t *dst)
{
    dst->texture_id = src->texture_id;
    dst->anchor_x = src->anchor_x;
    dst->anchor_y = src->anchor_y;
    dst->scale = src->scale;
}

static void copy_point_marker_to_awk(const mal_map_point_overlay_marker_t *src, awk_map_point_overlay_marker_t *dst)
{
    copy_texture_to_awk(&src->icon_texture, &dst->icon_texture);
    copy_texture_to_awk(&src->name_texture, &dst->name_texture);
    copy_texture_to_awk(&src->bubble_texture, &dst->bubble_texture);
}

static void copy_base_overlay_to_mal(const awk_map_base_overlay_t *src, mal_map_base_overlay_t *dst)
{
    dst->geometry_type = (mal_map_overlay_type)src->geometry_type;
    dst->group_id = src->group_id;
    dst->map_id = src->map_id;
    dst->priority = src->priority;
    dst->visible = src->visible;
    dst->is_focus = src->is_focus;
    dst->min_level = src->min_level;
    dst->max_level = src->max_level;
    dst->guid = src->guid;
}

static void copy_line_marker_to_mal(const awk_map_line_overlay_marker_t *src, mal_map_line_overlay_marker_t *dst)
{
    dst->line_width = src->line_width;
    dst->line_color = src->line_color;
    dst->border_width = src->border_width;
    dst->border_color = src->border_color;
    dst->dash_painted_length = src->dash_painted_length;
    dst->dash_unpainted_length = src->dash_unpainted_length;
    dst->dash_offset = src->dash_offset;
    dst->use_dash = src->use_dash;
}

static void copy_texture_to_mal(const awk_map_point_texture_info_t *src, mal_map_point_texture_info_t *dst)
{
    dst->texture_id = src->texture_id;
    dst->anchor_x = src->anchor_x;
    dst->anchor_y = src->anchor_y;
    dst->scale = src->scale;
}

static void copy_point_marker_to_mal(const awk_map_point_overlay_marker_t *src, mal_map_point_overlay_marker_t *dst)
{
    copy_texture_to_mal(&src->icon_texture, &dst->icon_texture);
    copy_texture_to_mal(&src->name_texture, &dst->name_texture);
    copy_texture_to_mal(&src->bubble_texture, &dst->bubble_texture);
}

void mal_to_awk_map_point_overlay_t(mal_map_point_overlay_t *src, awk_map_point_overlay_t *dst)
{
    if (!src || !dst) {
        return;
    }

    copy_base_overlay_to_awk(&src->base_overlay, &dst->base_overlay);

    dst->position.lon = src->position.lon;
    dst->position.lat = src->position.lat;

    dst->angle = src->angle;
    dst->icon_visible = src->icon_visible;
    dst->name_visible = src->name_visible;
    dst->bubble_visible = src->bubble_visible;

    copy_point_marker_to_awk(&src->normal_marker, &dst->normal_marker);
    copy_point_marker_to_awk(&src->focus_marker, &dst->focus_marker);
}

void awk_to_mal_map_point_overlay_t(awk_map_point_overlay_t *src, mal_map_point_overlay_t *dst)
{
    if (!src || !dst) {
        return;
    }

    copy_base_overlay_to_mal(&src->base_overlay, &dst->base_overlay);

    dst->position.lon = src->position.lon;
    dst->position.lat = src->position.lat;

    dst->angle = src->angle;
    dst->icon_visible = src->icon_visible;
    dst->name_visible = src->name_visible;
    dst->bubble_visible = src->bubble_visible;

    copy_point_marker_to_mal(&src->normal_marker, &dst->normal_marker);
    copy_point_marker_to_mal(&src->focus_marker, &dst->focus_marker);
}

void mal_to_awk_map_polyline_overlay_t(mal_map_polyline_overlay_t *src, awk_map_polyline_overlay_t *dst)
{
    if (!src || !dst) {
        return;
    }
    copy_base_overlay_to_awk(&src->base_overlay, &dst->base_overlay);

    // 浅拷贝：直接复用指针(数据大小一致，所以可以直接不深拷贝)
    dst->points = (awk_map_coord2d_t *)src->points;       // 类型强转，结构体布局相同
    dst->map_points = (awk_point_64_t *)src->map_points;  // 地图坐标点
    dst->point_size = src->point_size;
    dst->point_flags = src->point_flags;  // uint8_t* 兼容
    dst->point_flags_size = src->point_flags_size;

    dst->tolerance = src->tolerance;
    dst->max_keep_point_size = src->max_keep_point_size;

    copy_line_marker_to_awk(&src->normal_marker, &dst->normal_marker);
    copy_line_marker_to_awk(&src->focus_marker, &dst->focus_marker);
    copy_point_marker_to_awk(&src->arrow_marker, &dst->arrow_marker);
    dst->show_arrow = src->show_arrow;

    // 已走过路线 & 当前位置
    dst->pre_index = src->pre_index;
    dst->location.lat = src->location.lat;
    dst->location.lon = src->location.lon;
}

void awk_to_mal_map_polyline_overlay_t(awk_map_polyline_overlay_t *src, mal_map_polyline_overlay_t *dst)
{
    if (!src || !dst) {
        return;
    }
    copy_base_overlay_to_mal(&src->base_overlay, &dst->base_overlay);

    // 浅拷贝：直接复用指针（结构体布局相同，可安全强转）
    dst->points = (mal_map_coord2d_t *)src->points;
    dst->map_points = (mal_point_64_t *)src->map_points;
    dst->point_size = src->point_size;
    dst->point_flags = src->point_flags;
    dst->point_flags_size = src->point_flags_size;
    dst->tolerance = src->tolerance;
    dst->max_keep_point_size = src->max_keep_point_size;

    copy_line_marker_to_mal(&src->normal_marker, &dst->normal_marker);
    copy_line_marker_to_mal(&src->focus_marker, &dst->focus_marker);
    copy_point_marker_to_mal(&src->arrow_marker, &dst->arrow_marker);
    dst->show_arrow = src->show_arrow;

    // 已走过路线 & 当前位置
    dst->pre_index = src->pre_index;
    dst->location.lat = src->location.lat;
    dst->location.lon = src->location.lon;
}

void mal_to_awk_map_polygon_overlay_t(mal_map_polygon_overlay_t *src, awk_map_polygon_overlay_t *dst)
{
    if (!src || !dst) {
        return;
    }

    // base_overlay
    copy_base_overlay_to_awk(&src->base_overlay, &dst->base_overlay);

    // 浅拷贝指针（关键！）
    dst->points = (awk_map_coord2d_t *)src->points;  // 安全：结构体布局相同
    dst->point_size = src->point_size;
    dst->color = src->color;
}

void awk_to_mal_map_polygon_overlay_t(awk_map_polygon_overlay_t *src, mal_map_polygon_overlay_t *dst)
{
    if (!src || !dst) {
        return;
    }

    // base_overlay
    copy_base_overlay_to_mal(&src->base_overlay, &dst->base_overlay);

    // 浅拷贝指针
    dst->points = (mal_map_coord2d_t *)src->points;
    dst->point_size = src->point_size;
    dst->color = src->color;
}

void mal_to_awk_map_base_overlay_t(const mal_map_base_overlay_t *src, awk_map_base_overlay_t *dst)
{
    if (!src || !dst) {
        return;
    }
    // 清零目标
    memset_s(dst, sizeof(*dst), 0, sizeof(*dst));

    // 正确字段逐项复制（注意：geometry_type 需转换）
    dst->group_id = src->group_id;
    dst->map_id = src->map_id;
    dst->priority = src->priority;  // 绘制优先级
    dst->visible = src->visible;
    dst->is_focus = src->is_focus;
    dst->min_level = src->min_level;
    dst->max_level = src->max_level;
    dst->guid = src->guid;

    // 枚举类型转换
    switch (src->geometry_type) {
        case MAL_MAP_OVERLAY_TYPE_POINT:
            dst->geometry_type = AWK_MAP_OVERLAY_TYPE_POINT;
            break;
        case MAL_MAP_OVERLAY_TYPE_LINE:
            dst->geometry_type = AWK_MAP_OVERLAY_TYPE_LINE;
            break;
        case MAL_MAP_OVERLAY_TYPE_POLYGON:
            dst->geometry_type = AWK_MAP_OVERLAY_TYPE_POLYGON;
            break;
        case MAL_MAP_OVERLAY_TYPE_TRACK_NAVI:
            dst->geometry_type = AWK_MAP_OVERLAY_TYPE_TRACK_NAVI;
            break;
        default:
            dst->geometry_type = AWK_MAP_OVERLAY_TYPE_UNKNOWN;
            return;
    }
}

awk_map_render_callback_t mal_to_awk_map_render_callback_t(mal_map_render_callback_t src)
{
    awk_map_render_callback_t dst = {0};
    // 点覆盖物
    dst.on_point_begin_draw = src.on_point_begin_draw;
    dst.on_point_end_draw = src.on_point_end_draw;

    // 线覆盖物
    dst.on_line_begin_draw = src.on_line_begin_draw;
    dst.on_line_end_draw = src.on_line_end_draw;

    // 面覆盖物
    dst.on_polygon_begin_draw = src.on_polygon_begin_draw;
    dst.on_polygon_end_draw = src.on_polygon_end_draw;

    // Tile 绘制
    dst.on_tile_begin_draw = src.on_tile_begin_draw;
    dst.on_tile_end_draw = src.on_tile_end_draw;

    // POI 绘制
    dst.on_poi_begin_draw = src.on_poi_begin_draw;
    dst.on_poi_end_draw = src.on_poi_end_draw;

    // === MAL 没有的字段：设为 NULL 后续再增加 ===
    dst.on_tile_begin_download = NULL;
    dst.on_poi_begin_download = NULL;
    dst.on_tile_end_download = NULL;
    dst.on_poi_end_download = NULL;
    return dst;
}

void awk_to_mal_map_posture_t(awk_map_posture_t *src, mal_map_posture_t *dst)
{
    if (src == NULL || dst == NULL) {
        return;
    }

    // 转换 map_center
    dst->map_center.lon = src->map_center.lon;
    dst->map_center.lat = src->map_center.lat;

    // 转换 view_port
    dst->view_port.width = src->view_port.width;
    dst->view_port.height = src->view_port.height;

    // 转换 drawing_port
    dst->drawing_port.width = src->drawing_port.width;
    dst->drawing_port.height = src->drawing_port.height;

    // 转换 anchor
    dst->anchor.anchor_x = src->anchor.anchor_x;
    dst->anchor.anchor_y = src->anchor.anchor_y;

    // 转换 map_view_rect
    dst->map_view_rect.x = src->map_view_rect.x;
    dst->map_view_rect.y = src->map_view_rect.y;
    dst->map_view_rect.width = src->map_view_rect.width;
    dst->map_view_rect.height = src->map_view_rect.height;

    // 基本类型直接赋值
    dst->scale = src->scale;
    dst->level = src->level;
    dst->roll_angle = src->roll_angle;
}

static int safe_mal_strdup(const char *src, char **dst)
{
    *dst = mal_strdup(src);
    return (*dst == NULL) ? -1 : 0;
}

static int awk_to_mal_copy_points(const awk_map_track_info_t *src, mal_map_track_info_t *dst)
{
    if (!src->points || src->point_size <= 0) {
        return 0;
    }
    dst->point_size = src->point_size;
    dst->points = (mal_map_coord2d_t *)amap_mem_malloc(dst->point_size * sizeof(mal_map_coord2d_t));
    if (!dst->points) {
        return -1;
    }
    for (size_t i = 0; i < dst->point_size; ++i) {
        awk_to_mal_map_coord2d_t(&src->points[i], &dst->points[i]);
    }
    return 0;
}

static int awk_to_mal_copy_via_indexes(const awk_map_track_info_t *src, mal_map_track_info_t *dst)
{
    if (!src->via_point_indexes || src->via_point_index_size <= 0) {
        return 0;
    }
    dst->via_point_index_size = src->via_point_index_size;
    size_t size = dst->via_point_index_size * sizeof(int32_t);
    dst->via_point_indexes = (int32_t *)amap_mem_malloc(size);
    if (!dst->via_point_indexes) {
        return -1;
    }
    memcpy_s(dst->via_point_indexes, size, src->via_point_indexes, size);
    return 0;
}

static int awk_to_mal_copy_turn_points(const awk_map_track_info_t *src, mal_map_track_info_t *dst)
{
    if (!src->turn_points || src->turn_point_size <= 0) {
        return 0;
    }
    dst->turn_point_size = src->turn_point_size;
    dst->turn_points = (mal_map_track_turn_point_info_t *)amap_mem_malloc(dst->turn_point_size *
                                                                          sizeof(mal_map_track_turn_point_info_t));
    if (!dst->turn_points) {
        return -1;
    }
    for (size_t i = 0; i < dst->turn_point_size; ++i) {
        awk_to_mal_map_track_turn_point_info_t(&src->turn_points[i], &dst->turn_points[i]);
    }
    return 0;
}

static void mal_map_track_info_cleanup(mal_map_track_info_t *dst)
{
    if (dst->name) {
        amap_mem_free((void *)dst->name);
    }
    if (dst->desc) {
        amap_mem_free((void *)dst->desc);
    }
    if (dst->points) {
        amap_mem_free(dst->points);
    }
    if (dst->via_point_indexes) {
        amap_mem_free(dst->via_point_indexes);
    }
    if (dst->turn_points) {
        for (size_t i = 0; i < dst->turn_point_size; i++) {
            if (dst->turn_points[i].turn_message) {
                amap_mem_free(dst->turn_points[i].turn_message);
            }
            if (dst->turn_points[i].turn_location_message) {
                amap_mem_free(dst->turn_points[i].turn_location_message);
            }
        }
        amap_mem_free(dst->turn_points);
    }
    memset_s(dst, sizeof(*dst), 0, sizeof(*dst));
}

// 添加代码的部分
// 1.track info 相关转换
void awk_to_mal_map_track_info_t(const awk_map_track_info_t *src, mal_map_track_info_t *dst)
{
    if (!src || !dst) {
        return;
    }
    memset_s(dst, sizeof(*dst), 0, sizeof(*dst));

    if (safe_mal_strdup(src->name, &dst->name) != 0 || safe_mal_strdup(src->desc, &dst->desc) != 0 ||
        awk_to_mal_copy_points(src, dst) != 0 || awk_to_mal_copy_via_indexes(src, dst) != 0 ||
        awk_to_mal_copy_turn_points(src, dst) != 0) {
        mal_map_track_info_cleanup(dst);
        printf("Failed to convert awk_map_track_info_t to mal_map_track_info_t");
        return;
    }
}

/**
 * @brief MAL → AWK：将 mal_map_track_info_t 数据复制到已分配的 awk_map_track_info_t 中
 * @param src 源数据（MAL 层）
 * @param dst 目标缓冲区（AWK 层，必须已分配）
 * @note  字符串为浅拷贝（指针赋值），生命周期由 src 管理；数组为深拷贝
 */
void mal_to_awk_map_track_info_t(const mal_map_track_info_t *src, awk_map_track_info_t *dst)
{
    if (!src || !dst) {
        return;
    }
    memset_s(dst, sizeof(*dst), 0, sizeof(*dst));

    // 1. 拷贝字符串（浅拷贝：只赋指针）
    dst->name = src->name;
    dst->desc = src->desc;

    // 2. 拷贝坐标点数组（深拷贝）
    if (src->points && src->point_size > 0) {
        dst->point_size = src->point_size;
        dst->points = (awk_map_coord2d_t *)amap_mem_malloc(dst->point_size * sizeof(awk_map_coord2d_t));
        if (dst->points) {
            for (size_t i = 0; i < dst->point_size; ++i) {
                dst->points[i] = mal_to_awk_map_coord2d_t(src->points[i]);
            }
        } else {
            dst->points = NULL;
            dst->point_size = 0;
        }
    }

    // 3. 拷贝途径点索引数组
    if (src->via_point_indexes && src->via_point_index_size > 0) {
        dst->via_point_index_size = src->via_point_index_size;
        size_t size = dst->via_point_index_size * sizeof(int32_t);
        dst->via_point_indexes = (int32_t *)amap_mem_malloc(size);
        if (dst->via_point_indexes) {
            memcpy_s(dst->via_point_indexes, size, src->via_point_indexes, size);
        } else {
            dst->via_point_indexes = NULL;
            dst->via_point_index_size = 0;
        }
    }

    // 4. 拷贝转向点数组（深拷贝 + 子结构体转换）
    if (src->turn_points && src->turn_point_size > 0) {
        dst->turn_point_size = src->turn_point_size;
        dst->turn_points = (awk_map_track_turn_point_info_t *)amap_mem_malloc(dst->turn_point_size *
                                                                              sizeof(awk_map_track_turn_point_info_t));
        if (dst->turn_points) {
            for (size_t i = 0; i < dst->turn_point_size; ++i) {
                mal_to_awk_map_track_turn_point_info_t(&src->turn_points[i], &dst->turn_points[i]);
            }
        } else {
            dst->turn_points = NULL;
            dst->turn_point_size = 0;
        }
    }
}
// 2. turn point info 转换
void awk_to_mal_map_track_turn_point_info_t(const awk_map_track_turn_point_info_t *src,
                                            mal_map_track_turn_point_info_t *dst)
{
    if (!src || !dst) {
        return;
    }
    dst->index = src->index;
    dst->bear = src->bear;
    dst->turn_message = mal_strdup(src->turn_message);
    dst->turn_location_message = mal_strdup(src->turn_location_message);
}
void mal_to_awk_map_track_turn_point_info_t(const mal_map_track_turn_point_info_t *src,
                                            awk_map_track_turn_point_info_t *dst)
{
    if (!src || !dst) {
        return;
    }
    dst->index = src->index;
    dst->bear = src->bear;
    dst->turn_message = src->turn_message;
    dst->turn_location_message = src->turn_location_message;
}
// 3.navi event info 转换
/**
 * @brief AWK → MAL：转换导航事件信息（深拷贝字符串）
 * @param src 源数据（AWK 层）
 * @param dst 目标缓冲区（MAL 层）
 */
void awk_to_mal_map_track_navi_event_info_t(const awk_map_track_navi_event_info_t *src,
                                            mal_map_track_navi_event_info_t *dst)
{
    if (!src || !dst) {
        return;
    }
    dst->event_type = (mal_map_track_navi_event_type)src->event_type;
    dst->point_type = (mal_map_track_point_type)src->point_type;
    dst->point_index = src->point_index;
    dst->distance = src->distance;
    dst->turn_message = mal_strdup(src->turn_message);
    dst->turn_location_message = mal_strdup(src->turn_location_message);
    dst->bear = src->bear;
}
/**
 * @brief MAL → AWK：转换导航事件信息（浅拷贝字符串指针）
 * @param src 源数据（MAL 层）
 * @param dst 目标缓冲区（AWK 层）
 */
void mal_to_awk_map_track_navi_event_info_t(const mal_map_track_navi_event_info_t *src,
                                            awk_map_track_navi_event_info_t *dst)
{
    if (!src || !dst) {
        return;
    }
    dst->event_type = (awk_map_track_navi_event_type)src->event_type;
    dst->point_type = (awk_map_track_point_type)src->point_type;
    dst->point_index = src->point_index;
    dst->distance = src->distance;
    dst->turn_message = src->turn_message;
    dst->turn_location_message = src->turn_location_message;
    dst->bear = src->bear;
}
// 4. track navi overlay 转换
/**
 * @brief AWK → MAL：转换轨迹导航覆盖物（深拷贝 track_info，其他字段逐项复制）
 * @param src 源数据（AWK 层）
 * @param dst 目标缓冲区（MAL 层，必须已分配）
 */
void awk_to_mal_map_track_navi_overlay_t(awk_map_track_navi_overlay_t *src, mal_map_track_navi_overlay_t *dst)
{
    if (!src || !dst) {
        return;
    }
    copy_base_overlay_to_mal(&src->base_overlay, &dst->base_overlay);

    dst->track_info = (mal_map_track_info_t *)src->track_info;

    awk_to_mal_map_point_overlay_marker_t(&src->start_marker, &dst->start_marker);
    awk_to_mal_map_point_overlay_marker_t(&src->end_marker, &dst->end_marker);
    dst->custom_point_point_overlay_id = src->custom_point_point_overlay_id;
    awk_to_mal_map_track_plan_polyline_options_t(&src->plan_polyline_options, &dst->plan_polyline_options);
    awk_to_mal_map_track_real_polyline_options_t(&src->real_polyline_options, &dst->real_polyline_options);
    awk_to_mal_map_track_yaw_options_t(&src->yaw_options, &dst->yaw_options);

    awk_to_mal_map_track_navi_options_t(&src->navi_options, &dst->navi_options);
}
/**
 * @brief MAL → AWK：转换轨迹导航覆盖物（浅拷贝字符串，动态数组另分配）
 * @param src 源数据（MAL 层）
 * @param dst 目标缓冲区（AWK 层，必须已分配）
 */
void mal_to_awk_map_track_navi_overlay_t(mal_map_track_navi_overlay_t *src, awk_map_track_navi_overlay_t *dst)
{
    if (!src || !dst) {
        return;
    }
    copy_base_overlay_to_awk(&src->base_overlay, &dst->base_overlay);

    dst->track_info = (awk_map_track_info_t *)src->track_info;

    mal_to_awk_map_point_overlay_marker_t(&src->start_marker, &dst->start_marker);
    mal_to_awk_map_point_overlay_marker_t(&src->end_marker, &dst->end_marker);
    dst->custom_point_point_overlay_id = src->custom_point_point_overlay_id;
    mal_to_awk_map_track_plan_polyline_options_t(&src->plan_polyline_options, &dst->plan_polyline_options);
    mal_to_awk_map_track_real_polyline_options_t(&src->real_polyline_options, &dst->real_polyline_options);
    mal_to_awk_map_track_yaw_options_t(&src->yaw_options, &dst->yaw_options);

    mal_to_awk_map_track_navi_options_t(&src->navi_options, &dst->navi_options);
}
// 5. options 结构体转换
/**
 * @brief AWK → MAL：转换 track navigation options（深拷贝所有动态数组）
 * @param src 源数据（AWK 层）
 * @param dst 目标缓冲区（MAL 层，必须已分配）
 */
void awk_to_mal_map_track_navi_options_t(const awk_map_track_navi_options_t *src, mal_map_track_navi_options_t *dst)
{
    if (!src || !dst) {
        return;
    }

    memset_s(dst, sizeof(*dst), 0, sizeof(*dst));

    // 1. 基本字段直接赋值
    dst->point_correcttion_threshold = src->point_correcttion_threshold;
    dst->navi_event_distance_threshold = src->navi_event_distance_threshold;
    dst->navi_calc_sampling = src->navi_calc_sampling;
    dst->tracking_mode = src->tracking_mode;

    // 2. pass_point_icon 数组
    if (src->pass_point_icon && src->pass_point_icon_size > 0) {
        dst->pass_point_icon_size = src->pass_point_icon_size;
        dst->pass_point_icon = (mal_map_point_overlay_marker_t *)amap_mem_malloc(
            dst->pass_point_icon_size * sizeof(mal_map_point_overlay_marker_t));
        if (dst->pass_point_icon) {
            for (size_t i = 0; i < dst->pass_point_icon_size; ++i) {
                awk_to_mal_map_point_overlay_marker_t(&src->pass_point_icon[i], &dst->pass_point_icon[i]);
            }
        }
    }

    // 3. turn_point_icon 数组
    if (src->turn_point_icon && src->turn_point_icon_size > 0) {
        dst->turn_point_icon_size = src->turn_point_icon_size;
        dst->turn_point_icon = (mal_map_point_overlay_marker_t *)amap_mem_malloc(
            dst->turn_point_icon_size * sizeof(mal_map_point_overlay_marker_t));
        if (dst->turn_point_icon) {
            for (size_t i = 0; i < dst->turn_point_icon_size; ++i) {
                awk_to_mal_map_point_overlay_marker_t(&src->turn_point_icon[i], &dst->turn_point_icon[i]);
            }
        }
    }

    // 4. pass_point_around_threshold（double 数组）
    if (src->pass_point_around_threshold && src->pass_point_around_threshold_size > 0) {
        dst->pass_point_around_threshold_size = src->pass_point_around_threshold_size;
        size_t size = dst->pass_point_around_threshold_size * sizeof(double);
        dst->pass_point_around_threshold = (double *)amap_mem_malloc(size);
        if (dst->pass_point_around_threshold) {
            memcpy_s(dst->pass_point_around_threshold, size, src->pass_point_around_threshold, size);
        }
    }

    // 5. turn_point_around_threshold
    if (src->turn_point_around_threshold && src->turn_point_around_threshold_size > 0) {
        dst->turn_point_around_threshold_size = src->turn_point_around_threshold_size;
        size_t size = dst->turn_point_around_threshold_size * sizeof(double);
        dst->turn_point_around_threshold = (double *)amap_mem_malloc(size);
        if (dst->turn_point_around_threshold) {
            memcpy_s(dst->turn_point_around_threshold, size, src->turn_point_around_threshold, size);
        }
    }
}
/**
 * @brief MAL → AWK：转换 track navigation options（浅拷贝指针，不分配新内存）
 * @param src 源数据（MAL 层）
 * @param dst 目标缓冲区（AWK 层，必须已分配）
 */
void mal_to_awk_map_track_navi_options_t(const mal_map_track_navi_options_t *src, awk_map_track_navi_options_t *dst)
{
    if (!src || !dst) {
        return;
    }

    // 清零目标结构体
    memset_s(dst, sizeof(*dst), 0, sizeof(*dst));

    // 1. 基本字段
    dst->point_correcttion_threshold = src->point_correcttion_threshold;
    dst->navi_event_distance_threshold = src->navi_event_distance_threshold;
    dst->navi_calc_sampling = src->navi_calc_sampling;

    // 2. pass_point_icon：需要深拷贝每个 marker
    if (src->pass_point_icon && src->pass_point_icon_size > 0) {
        dst->pass_point_icon_size = src->pass_point_icon_size;
        dst->pass_point_icon = (awk_map_point_overlay_marker_t *)amap_mem_malloc(
            dst->pass_point_icon_size * sizeof(awk_map_point_overlay_marker_t));
        if (dst->pass_point_icon) {
            for (size_t i = 0; i < dst->pass_point_icon_size; ++i) {
                mal_to_awk_map_point_overlay_marker_t(&src->pass_point_icon[i], &dst->pass_point_icon[i]);
            }
        } else {
            dst->pass_point_icon = NULL;
            dst->pass_point_icon_size = 0;
        }
    }

    // 3. turn_point_icon
    if (src->turn_point_icon && src->turn_point_icon_size > 0) {
        dst->turn_point_icon_size = src->turn_point_icon_size;
        dst->turn_point_icon = (awk_map_point_overlay_marker_t *)amap_mem_malloc(
            dst->turn_point_icon_size * sizeof(awk_map_point_overlay_marker_t));
        if (dst->turn_point_icon) {
            for (size_t i = 0; i < dst->turn_point_icon_size; ++i) {
                mal_to_awk_map_point_overlay_marker_t(&src->turn_point_icon[i], &dst->turn_point_icon[i]);
            }
        } else {
            dst->turn_point_icon = NULL;
            dst->turn_point_icon_size = 0;
        }
    }

    // 4. around thresholds
    dst->pass_point_around_threshold = src->pass_point_around_threshold;
    dst->pass_point_around_threshold_size = src->pass_point_around_threshold_size;
    dst->turn_point_around_threshold = src->turn_point_around_threshold;
    dst->turn_point_around_threshold_size = src->turn_point_around_threshold_size;
}
/**
 * @brief AWK → MAL：转换偏航参数选项
 * @param src 源数据（AWK 层）
 * @param dst 目标缓冲区（MAL 层，必须已分配）
 */
void awk_to_mal_map_track_yaw_options_t(const awk_map_track_yaw_options_t *src, mal_map_track_yaw_options_t *dst)
{
    if (!src || !dst) {
        return;
    }

    memset_s(dst, sizeof(*dst), 0, sizeof(*dst));

    // 1. 基本字段直接赋值
    dst->yaw_threshold = src->yaw_threshold;
    dst->is_show_yaw_polyline = src->is_show_yaw_polyline;
    dst->show_arrow = src->show_arrow;

    // 2. 纹理标记转换
    awk_to_mal_map_line_overlay_marker_t(&src->yaw_polyline_marker, &dst->yaw_polyline_marker);
    awk_to_mal_map_point_overlay_marker_t(&src->arrow_marker, &dst->arrow_marker);
}
/**
 * @brief MAL → AWK：转换偏航参数选项
 * @param src 源数据（MAL 层）
 * @param dst 目标缓冲区（AWK 层，必须已分配）
 */
void mal_to_awk_map_track_yaw_options_t(const mal_map_track_yaw_options_t *src, awk_map_track_yaw_options_t *dst)
{
    if (!src || !dst) {
        return;
    }

    memset_s(dst, sizeof(*dst), 0, sizeof(*dst));

    // 1. 基本字段直接赋值
    dst->yaw_threshold = src->yaw_threshold;
    dst->is_show_yaw_polyline = src->is_show_yaw_polyline;
    dst->show_arrow = src->show_arrow;

    // 2. 纹理标记转换
    mal_to_awk_map_line_overlay_marker_t(&src->yaw_polyline_marker, &dst->yaw_polyline_marker);
    mal_to_awk_map_point_overlay_marker_t(&src->arrow_marker, &dst->arrow_marker);
}

/**
 * @brief AWK → MAL：转换实走轨迹线选项
 * @param src 源数据（AWK 层）
 * @param dst 目标缓冲区（MAL 层，必须已分配）
 */
void awk_to_mal_map_track_real_polyline_options_t(const awk_map_track_real_polyline_options_t *src,
                                                  mal_map_track_real_polyline_options_t *dst)
{
    if (!src || !dst) {
        return;
    }

    memset_s(dst, sizeof(*dst), 0, sizeof(*dst));

    // 1. 基本字段
    dst->show_arrow = src->show_arrow;

    // 2. 线覆盖物 marker
    awk_to_mal_map_line_overlay_marker_t(&src->real_polyline_marker, &dst->real_polyline_marker);

    // 3. 鱼骨箭头 marker
    awk_to_mal_map_point_overlay_marker_t(&src->arrow_marker, &dst->arrow_marker);
}
/**
 * @brief MAL → AWK：转换实走轨迹线选项
 * @param src 源数据（MAL 层）
 * @param dst 目标缓冲区（AWK 层，必须已分配）
 */
void mal_to_awk_map_track_real_polyline_options_t(const mal_map_track_real_polyline_options_t *src,
                                                  awk_map_track_real_polyline_options_t *dst)
{
    if (!src || !dst) {
        return;
    }

    // 清零目标结构体
    memset_s(dst, sizeof(*dst), 0, sizeof(*dst));

    // 1. 基本字段
    dst->show_arrow = src->show_arrow;

    // 2. 线覆盖物 marker
    mal_to_awk_map_line_overlay_marker_t(&src->real_polyline_marker, &dst->real_polyline_marker);

    // 3. 鱼骨箭头 marker
    mal_to_awk_map_point_overlay_marker_t(&src->arrow_marker, &dst->arrow_marker);
}
/**
 * @brief AWK → MAL：转换规划轨迹线选项（深拷贝 point_flags 数组）
 * @param src 源数据（AWK 层）
 * @param dst 目标缓冲区（MAL 层，必须已分配）
 */
void awk_to_mal_map_track_plan_polyline_options_t(const awk_map_track_plan_polyline_options_t *src,
                                                  mal_map_track_plan_polyline_options_t *dst)
{
    if (!src || !dst) {
        return;
    }

    // 清零目标结构体
    memset_s(dst, sizeof(*dst), 0, sizeof(*dst));

    // 1. 基本字段
    dst->show_arrow = src->show_arrow;

    // 2. 线 marker
    awk_to_mal_map_line_overlay_marker_t(&src->plan_polyline_marker, &dst->plan_polyline_marker);

    // 3. 箭头 marker
    awk_to_mal_map_point_overlay_marker_t(&src->arrow_marker, &dst->arrow_marker);

    // 4. point_flags 数组：需要深拷贝
    if (src->point_flags && src->point_flags_size > 0) {
        dst->point_flags_size = src->point_flags_size;
        size_t size = dst->point_flags_size * sizeof(uint8_t);
        dst->point_flags = (uint8_t *)amap_mem_malloc(size);
        if (dst->point_flags) {
            memcpy_s(dst->point_flags, size, src->point_flags, size);
        } else {
            dst->point_flags = NULL;
            dst->point_flags_size = 0;
        }
    }
}
/**
 * @brief MAL → AWK：转换规划轨迹线选项（浅拷贝 point_flags 指针）
 * @param src 源数据（MAL 层）
 * @param dst 目标缓冲区（AWK 层，必须已分配）
 */
void mal_to_awk_map_track_plan_polyline_options_t(const mal_map_track_plan_polyline_options_t *src,
                                                  awk_map_track_plan_polyline_options_t *dst)
{
    if (!src || !dst) {
        return;
    }

    // 清零目标结构体
    memset_s(dst, sizeof(*dst), 0, sizeof(*dst));

    // 1. 基本字段
    dst->show_arrow = src->show_arrow;

    // 2. 线 marker
    mal_to_awk_map_line_overlay_marker_t(&src->plan_polyline_marker, &dst->plan_polyline_marker);

    // 3. 箭头 marker
    mal_to_awk_map_point_overlay_marker_t(&src->arrow_marker, &dst->arrow_marker);

    // 4. point_flags：浅拷贝指针（AWK 层只读）
    dst->point_flags = src->point_flags;
    dst->point_flags_size = src->point_flags_size;
}

void awk_to_mal_map_point_overlay_marker_t(const awk_map_point_overlay_marker_t *src,
                                           mal_map_point_overlay_marker_t *dst)
{
    // 转换icon纹理信息
    dst->icon_texture.texture_id = src->icon_texture.texture_id;
    dst->icon_texture.anchor_x = src->icon_texture.anchor_x;
    dst->icon_texture.anchor_y = src->icon_texture.anchor_y;
    dst->icon_texture.scale = src->icon_texture.scale;

    // 转换name纹理信息
    dst->name_texture.texture_id = src->name_texture.texture_id;
    dst->name_texture.anchor_x = src->name_texture.anchor_x;
    dst->name_texture.anchor_y = src->name_texture.anchor_y;
    dst->name_texture.scale = src->name_texture.scale;

    // 转换气泡纹理信息
    dst->bubble_texture.texture_id = src->bubble_texture.texture_id;
    dst->bubble_texture.anchor_x = src->bubble_texture.anchor_x;
    dst->bubble_texture.anchor_y = src->bubble_texture.anchor_y;
    dst->bubble_texture.scale = src->bubble_texture.scale;
}
void mal_to_awk_map_point_overlay_marker_t(const mal_map_point_overlay_marker_t *src,
                                           awk_map_point_overlay_marker_t *dst)
{
    // 转换icon纹理信息
    dst->icon_texture.texture_id = src->icon_texture.texture_id;
    dst->icon_texture.anchor_x = src->icon_texture.anchor_x;
    dst->icon_texture.anchor_y = src->icon_texture.anchor_y;
    dst->icon_texture.scale = src->icon_texture.scale;

    // 转换name纹理信息
    dst->name_texture.texture_id = src->name_texture.texture_id;
    dst->name_texture.anchor_x = src->name_texture.anchor_x;
    dst->name_texture.anchor_y = src->name_texture.anchor_y;
    dst->name_texture.scale = src->name_texture.scale;

    // 转换气泡纹理信息
    dst->bubble_texture.texture_id = src->bubble_texture.texture_id;
    dst->bubble_texture.anchor_x = src->bubble_texture.anchor_x;
    dst->bubble_texture.anchor_y = src->bubble_texture.anchor_y;
    dst->bubble_texture.scale = src->bubble_texture.scale;
}
void awk_to_mal_map_line_overlay_marker_t(const awk_map_line_overlay_marker_t *src, mal_map_line_overlay_marker_t *dst)
{
    if (!src || !dst) {
        return;
    }

    dst->line_width = src->line_width;
    dst->line_color = src->line_color;
    dst->border_width = src->border_width;
    dst->border_color = src->border_color;
    dst->dash_painted_length = src->dash_painted_length;
    dst->dash_unpainted_length = src->dash_unpainted_length;
    dst->dash_offset = src->dash_offset;
    dst->use_dash = src->use_dash;
}

void mal_to_awk_map_line_overlay_marker_t(const mal_map_line_overlay_marker_t *src, awk_map_line_overlay_marker_t *dst)
{
    if (!src || !dst) {
        return;
    }

    // 清零目标
    memset_s(dst, sizeof(*dst), 0, sizeof(*dst));

    // 字段反向映射 + 类型转换
    dst->line_width = src->line_width;
    dst->line_color = src->line_color;
    dst->border_width = src->border_width;
    dst->border_color = src->border_color;
    dst->dash_painted_length = src->dash_painted_length;
    dst->dash_unpainted_length = src->dash_unpainted_length;
    dst->dash_offset = src->dash_offset;
    dst->use_dash = src->use_dash;
}

/** * @brief 释放由 awk_to_mal_map_track_info_t 分配的 mal_map_track_info_t 内存
 * @param info 指向要释放的 track_info 结构
 */
void awk_to_mal_map_track_info_free(mal_map_track_info_t *info)
{
    if (!info) {
        return;
    }

    if (info->name)
        amap_mem_free((void *)info->name);
    if (info->desc)
        amap_mem_free((void *)info->desc);
    if (info->points)
        amap_mem_free(info->points);
    if (info->via_point_indexes)
        amap_mem_free(info->via_point_indexes);

    if (info->turn_points) {
        for (size_t i = 0; i < info->turn_point_size; i++) {
            if (info->turn_points[i].turn_message)
                amap_mem_free(info->turn_points[i].turn_message);
            if (info->turn_points[i].turn_location_message)
                amap_mem_free(info->turn_points[i].turn_location_message);
        }
        amap_mem_free(info->turn_points);
    }
}

char *mal_strdup(const char *str)
{
    if (!str) {
        return NULL;
    }

    size_t len = strlen(str) + 1;
    char *copy = (char *)amap_mem_malloc(len);
    if (copy) {
        memcpy_s(copy, len, str, len);
    }
    return copy;
}