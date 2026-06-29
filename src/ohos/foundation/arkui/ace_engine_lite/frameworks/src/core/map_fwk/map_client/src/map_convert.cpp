/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map convert implement.
 * Author: CompanyName
 * Create: 2026-01-07
 */
#include <string>
#include "ace_log.h"
#include "map_convert.h"
#include "ace_mem_base.h"

namespace OHOS {
namespace ACELite {

map_track_navigation_listener MapConverter::map_track_listener_;

MapConverter &MapConverter::GetInstance()
{
    static MapConverter instance;
    return instance;
}

mal_pixel_mode_t MapConverter::convert_pixel_mode(map_pixel_mode mode)
{
    switch (mode) {
        case MAP_PIXEL_MODE_GREY:
            return MAL_PIXEL_MODE_GREY;
        case MAP_PIXEL_MODE_RGB_332:
            return MAL_PIXEL_MODE_RGB_332;
        case MAP_PIXEL_MODE_RGB_565:
            return MAL_PIXEL_MODE_RGB_565;
        case MAP_PIXEL_MODE_RGB_888:
            return MAL_PIXEL_MODE_RGB_888;
        case MAP_PIXEL_MODE_ARGB_8888:
            return MAL_PIXEL_MODE_ARGB_8888;
        case MAP_PIXEL_MODE_RGBA_8888:
            return MAL_PIXEL_MODE_RGBA_8888;
        case MAP_PIXEL_MODE_BGR_233:
            return MAL_PIXEL_MODE_BGR_233;
        case MAP_PIXEL_MODE_BGR_565:
            return MAL_PIXEL_MODE_BGR_565;
        case MAP_PIXEL_MODE_BGR_888:
            return MAL_PIXEL_MODE_BGR_888;
        case MAP_PIXEL_MODE_BGRA_8888:
            return MAL_PIXEL_MODE_BGRA_8888;
        case MAP_PIXEL_MODE_ABGR_8888:
            return MAL_PIXEL_MODE_ABGR_8888;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "convert_pixel_mode failed");
            return MAL_PIXEL_MODE_GREY;
    }
}

map_pixel_mode MapConverter::convert_pixel_mode(mal_pixel_mode_t mode)
{
    switch (mode) {
        case MAL_PIXEL_MODE_GREY:
            return MAP_PIXEL_MODE_GREY;
        case MAL_PIXEL_MODE_RGB_332:
            return MAP_PIXEL_MODE_RGB_332;
        case MAL_PIXEL_MODE_RGB_565:
            return MAP_PIXEL_MODE_RGB_565;
        case MAL_PIXEL_MODE_RGB_888:
            return MAP_PIXEL_MODE_RGB_888;
        case MAL_PIXEL_MODE_ARGB_8888:
            return MAP_PIXEL_MODE_ARGB_8888;
        case MAL_PIXEL_MODE_RGBA_8888:
            return MAP_PIXEL_MODE_RGBA_8888;
        case MAL_PIXEL_MODE_BGR_233:
            return MAP_PIXEL_MODE_BGR_233;
        case MAL_PIXEL_MODE_BGR_565:
            return MAP_PIXEL_MODE_BGR_565;
        case MAL_PIXEL_MODE_BGR_888:
            return MAP_PIXEL_MODE_BGR_888;
        case MAL_PIXEL_MODE_BGRA_8888:
            return MAP_PIXEL_MODE_BGRA_8888;
        case MAL_PIXEL_MODE_ABGR_8888:
            return MAP_PIXEL_MODE_ABGR_8888;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "convert_pixel_mode failed");
            return MAP_PIXEL_MODE_GREY;
    }
}

mal_align_style_t MapConverter::convert_align_style(map_align_style align)
{
    switch (align) {
        case MAP_ALIGN_STYLE_NONE:
            return MAL_ALIGN_STYLE_NONE;
        case MAP_ALIGN_STYLE_CENTER:
            return MAL_ALIGN_STYLE_CENTER;
        case MAP_ALIGN_STYLE_LEFT:
            return MAL_ALIGN_STYLE_LEFT;
        case MAP_ALIGN_STYLE_RIGHT:
            return MAL_ALIGN_STYLE_RIGHT;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "convert_align_style failed");
            return MAL_ALIGN_STYLE_NONE;
    }
}

map_align_style MapConverter::convert_align_style(mal_align_style_t align)
{
    switch (align) {
        case MAL_ALIGN_STYLE_NONE:
            return MAP_ALIGN_STYLE_NONE;
        case MAL_ALIGN_STYLE_CENTER:
            return MAP_ALIGN_STYLE_CENTER;
        case MAL_ALIGN_STYLE_LEFT:
            return MAP_ALIGN_STYLE_LEFT;
        case MAL_ALIGN_STYLE_RIGHT:
            return MAP_ALIGN_STYLE_RIGHT;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "convert_align_style failed");
            return MAP_ALIGN_STYLE_NONE;
    }
}

mal_font_weight_t MapConverter::convert_font_weight(map_font_weight weight)
{
    switch (weight) {
        case MAP_FONT_WEIGHT_NORMAL:
            return MAL_FONT_WEIGHT_NORMAL;
        case MAP_FONT_WEIGHT_THIN:
            return MAL_FONT_WEIGHT_THIN;
        case MAP_FONT_WEIGHT_BOLD:
            return MAL_FONT_WEIGHT_BOLD;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "convert_font_weight failed");
            return MAL_FONT_WEIGHT_NORMAL;
    }
}

map_font_weight MapConverter::convert_font_weight(mal_font_weight_t weight)
{
    switch (weight) {
        case MAL_FONT_WEIGHT_NORMAL:
            return MAP_FONT_WEIGHT_NORMAL;
        case MAL_FONT_WEIGHT_THIN:
            return MAP_FONT_WEIGHT_THIN;
        case MAL_FONT_WEIGHT_BOLD:
            return MAP_FONT_WEIGHT_BOLD;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "convert_font_weight failed");
            return MAP_FONT_WEIGHT_NORMAL;
    }
}

mal_fill_style_t MapConverter::convert_fill_style(map_fill_style fill)
{
    switch (fill) {
        case MAP_FILL_STYLE_DRAWING_ONLY:
            return MAL_FILL_STYLE_DRAWING_ONLY;
        case MAP_FILL_STYLE_DRAWING_AND_STROKE:
            return MAL_FILL_STYLE_DRAWING_AND_STROKE;
        case MAP_FILL_STYLE_STROKE_ONLY:
            return MAL_FILL_STYLE_STROKE_ONLY;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "convert_fill_style failed");
            return MAL_FILL_STYLE_DRAWING_ONLY;
    }
}

map_fill_style MapConverter::convert_fill_style(mal_fill_style_t fill)
{
    switch (fill) {
        case MAL_FILL_STYLE_DRAWING_ONLY:
            return MAP_FILL_STYLE_DRAWING_ONLY;
        case MAL_FILL_STYLE_DRAWING_AND_STROKE:
            return MAP_FILL_STYLE_DRAWING_AND_STROKE;
        case MAL_FILL_STYLE_STROKE_ONLY:
            return MAP_FILL_STYLE_STROKE_ONLY;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "convert_fill_style failed");
            return MAP_FILL_STYLE_DRAWING_ONLY;
    }
}

mal_image_format_t MapConverter::convert_image_format(map_image_format format)
{
    switch (format) {
        case MAP_IMAGE_FORMAT_JPEG:
            return MAL_IMAGE_FORMAT_JPEG;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "convert_image_format failed");
            return MAL_IMAGE_FORMAT_JPEG;
    }
}

map_image_format MapConverter::convert_image_format(mal_image_format_t format)
{
    switch (format) {
        case MAL_IMAGE_FORMAT_JPEG:
            return MAP_IMAGE_FORMAT_JPEG;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "convert_image_format failed");
            return MAP_IMAGE_FORMAT_JPEG;
    }
}

map_track_navi_event_type MapConverter::convert_track_navi_event_type_mode(mal_map_track_navi_event_type mode)
{
    switch (mode) {
        case MAL_MAP_TRACK_NAVI_EVENT_ARRIVED:
            return MAP_TRACK_NAVI_EVENT_ARRIVED;
        case MAL_MAP_TRACK_NAVI_EVENT_AROUNDING:
            return MAP_TRACK_NAVI_EVENT_AROUNDING;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "convert_track_navi_event_type_mode failed");
            return MAP_TRACK_NAVI_EVENT_ARRIVED;
    }
}

map_track_point_type MapConverter::convert_track_point_type_mode(mal_map_track_point_type mode)
{
    switch (mode) {
        case MAL_MAP_TRACK_POINT_START:
            return MAP_TRACK_POINT_START;
        case MAL_MAP_TRACK_POINT_PASS:
            return MAP_TRACK_POINT_PASS;
        case MAL_MAP_TRACK_POINT_TURN:
            return MAP_TRACK_POINT_TURN;
        case MAL_MAP_TRACK_POINT_FINISH:
            return MAP_TRACK_POINT_FINISH;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "convert_track_point_type_mode failed");
            return MAP_TRACK_POINT_START;
    }
}

mal_map_texture_data_t MapConverter::convert_texture(const map_texture_data *texture_data)
{
    mal_map_texture_data_t mal_data;
    mal_data.pixel_mode = (mal_pixel_mode_t)texture_data->pixel_mode;
    mal_data.buffer = texture_data->buffer;
    mal_data.buffer_size = texture_data->buffer_size;
    mal_data.width = texture_data->width;
    mal_data.height = texture_data->height;
    mal_data.stride = texture_data->stride;
    mal_data.pre_multiplied = texture_data->pre_multiplied;
    return mal_data;
}

map_overlay_type MapConverter::convert_map_overlay_type(mal_map_overlay_type type)
{
    switch (type) {
        case MAL_MAP_OVERLAY_TYPE_POINT:
            return MAP_OVERLAY_TYPE_POINT;
        case MAL_MAP_OVERLAY_TYPE_LINE:
            return MAP_OVERLAY_TYPE_LINE;
        case MAL_MAP_OVERLAY_TYPE_POLYGON:
            return MAP_OVERLAY_TYPE_POLYGON;
        case MAL_MAP_OVERLAY_TYPE_TRACK_NAVI:
            return MAP_OVERLAY_TYPE_TRACK_NAVI;
        case MAL_MAP_OVERLAY_TYPE_UNKNOWN:
            return MAP_OVERLAY_TYPE_UNKNOWN;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "convert_map_overlay_type failed");
            return MAP_OVERLAY_TYPE_UNKNOWN;
    }
}

mal_map_overlay_type MapConverter::convert_map_overlay_type(map_overlay_type type)
{
    switch (type) {
        case MAP_OVERLAY_TYPE_POINT:
            return MAL_MAP_OVERLAY_TYPE_POINT;
        case MAP_OVERLAY_TYPE_LINE:
            return MAL_MAP_OVERLAY_TYPE_LINE;
        case MAP_OVERLAY_TYPE_POLYGON:
            return MAL_MAP_OVERLAY_TYPE_POLYGON;
        case MAP_OVERLAY_TYPE_TRACK_NAVI:
            return MAL_MAP_OVERLAY_TYPE_TRACK_NAVI;
        case MAP_OVERLAY_TYPE_UNKNOWN:
            return MAL_MAP_OVERLAY_TYPE_UNKNOWN;
        default:
            HILOG_ERROR(HILOG_MODULE_ACE, "convert_map_overlay_type failed");
            return MAL_MAP_OVERLAY_TYPE_UNKNOWN;
    }
}

mal_map_base_overlay_t MapConverter::convert_base_overlay(map_base_overlay point_overlay)
{
    mal_map_base_overlay_t mal_overlay;
    mal_overlay.geometry_type = convert_map_overlay_type(point_overlay.geometry_type);
    mal_overlay.group_id = point_overlay.group_id;
    mal_overlay.guid = point_overlay.guid;
    mal_overlay.is_focus = point_overlay.is_focus;
    mal_overlay.map_id = point_overlay.map_id;
    mal_overlay.max_level = point_overlay.max_level;
    mal_overlay.min_level = point_overlay.min_level;
    mal_overlay.priority = point_overlay.priority;
    mal_overlay.visible = point_overlay.visible;
    return mal_overlay;
}

map_base_overlay MapConverter::convert_map_base_overlay(mal_map_base_overlay_t point_overlay)
{
    map_base_overlay mal_overlay;
    mal_overlay.geometry_type = convert_map_overlay_type(point_overlay.geometry_type);
    mal_overlay.group_id = point_overlay.group_id;
    mal_overlay.guid = point_overlay.guid;
    mal_overlay.is_focus = point_overlay.is_focus;
    mal_overlay.map_id = point_overlay.map_id;
    mal_overlay.max_level = point_overlay.max_level;
    mal_overlay.min_level = point_overlay.min_level;
    mal_overlay.priority = point_overlay.priority;
    mal_overlay.visible = point_overlay.visible;
    return mal_overlay;
}

mal_map_coord2d_t MapConverter::convert_coord2d(map_coord2d coord2d)
{
    mal_map_coord2d_t mal_coord2d;
    mal_coord2d.lat = coord2d.lat;
    mal_coord2d.lon = coord2d.lon;
    return mal_coord2d;
}

map_coord2d MapConverter::convert_map_coord2d(mal_map_coord2d_t coord2d)
{
    map_coord2d mal_coord2d;
    mal_coord2d.lat = coord2d.lat;
    mal_coord2d.lon = coord2d.lon;
    return mal_coord2d;
}

mal_point_64_t MapConverter::convert_point_64(map_point_64 point)
{
    mal_point_64_t mal_point;
    mal_point.x = point.x;
    mal_point.y = point.y;
    return mal_point;
}

mal_map_view_port_t MapConverter::convert_view_port(map_view_port view)
{
    mal_map_view_port_t mal_port;
    mal_port.height = view.height;
    mal_port.width = view.width;
    return mal_port;
}

mal_map_line_overlay_marker_t MapConverter::convert_line_overlay(map_line_overlay_marker map_overlay)
{
    mal_map_line_overlay_marker_t mal_overlay;
    mal_overlay.border_color = map_overlay.border_color;
    mal_overlay.border_width = map_overlay.border_width;
    mal_overlay.dash_offset = map_overlay.dash_offset;
    mal_overlay.dash_painted_length = map_overlay.dash_painted_length;
    mal_overlay.dash_unpainted_length = map_overlay.dash_unpainted_length;
    mal_overlay.line_color = map_overlay.line_color;
    mal_overlay.line_width = map_overlay.line_width;
    mal_overlay.use_dash = map_overlay.use_dash;
    return mal_overlay;
}

mal_map_point_overlay_marker_t MapConverter::convert_point_overlay_marker(map_point_overlay_marker marker)
{
    mal_map_point_overlay_marker_t mal_marker;
    mal_marker.bubble_texture = convert_point_texture_info(marker.bubble_texture);
    mal_marker.icon_texture = convert_point_texture_info(marker.icon_texture);
    mal_marker.name_texture = convert_point_texture_info(marker.name_texture);
    return mal_marker;
}

map_point_overlay_marker MapConverter::convert_map_point_overlay_marker(mal_map_point_overlay_marker_t marker)
{
    map_point_overlay_marker mal_marker;
    mal_marker.bubble_texture = convert_map_point_texture_info(marker.bubble_texture);
    mal_marker.icon_texture = convert_map_point_texture_info(marker.icon_texture);
    mal_marker.name_texture = convert_map_point_texture_info(marker.name_texture);
    return mal_marker;
}

map_point_texture_info MapConverter::convert_map_point_texture_info(mal_map_point_texture_info_t info)
{
    map_point_texture_info mal_texture_info;
    mal_texture_info.anchor_x = info.anchor_x;
    mal_texture_info.anchor_y = info.anchor_y;
    mal_texture_info.scale = info.scale;
    mal_texture_info.texture_id = info.texture_id;
    return mal_texture_info;
}

mal_map_point_texture_info_t MapConverter::convert_point_texture_info(map_point_texture_info info)
{
    mal_map_point_texture_info_t mal_texture_info;
    mal_texture_info.anchor_x = info.anchor_x;
    mal_texture_info.anchor_y = info.anchor_y;
    mal_texture_info.scale = info.scale;
    mal_texture_info.texture_id = info.texture_id;
    return mal_texture_info;
}

mal_point_t MapConverter::convert_point(const map_point *src)
{
    mal_point_t dst;
    dst.x = src->x;
    dst.y = src->y;
    return dst;
}

map_point MapConverter::convert_point(const mal_point_t *src)
{
    map_point dst;
    dst.x = src->x;
    dst.y = src->y;
    return dst;
}

mal_rect_area_t MapConverter::convert_rect_area(const map_rect_area *src)
{
    mal_rect_area_t dst;
    dst.x = src->x;
    dst.y = src->y;
    dst.width = src->width;
    dst.height = src->height;
    return dst;
}

map_rect_area MapConverter::convert_rect_area(const mal_rect_area_t *src)
{
    map_rect_area dst;
    dst.x = src->x;
    dst.y = src->y;
    dst.width = src->width;
    dst.height = src->height;
    return dst;
}

mal_render_context_t MapConverter::convert_render_context(const map_render_context *src)
{
    mal_render_context_t dst;
    dst.width = src->width;
    dst.height = src->height;
    dst.map_view_rect = convert_rect_area(&src->map_view_rect);
    return dst;
}

mal_text_style_t MapConverter::convert_text_style(const map_text_style *src)
{
    mal_text_style_t dst;
    dst.align = convert_align_style(src->align);
    dst.font_size = src->font_size;
    dst.font_weight = convert_font_weight(src->font_weight);
    return dst;
}

map_text_style MapConverter::convert_text_style(const mal_text_style_t *src)
{
    map_text_style dst;
    dst.align = convert_align_style(src->align);
    dst.font_size = src->font_size;
    dst.font_weight = convert_font_weight(src->font_weight);
    return dst;
}

mal_dash_style_t MapConverter::convert_dash_style(const map_dash_style *src)
{
    mal_dash_style_t dst;
    dst.painted_length = src->painted_length;
    dst.unpainted_length = src->unpainted_length;
    dst.offset = src->offset;
    return dst;
}

map_dash_style MapConverter::convert_dash_style(const mal_dash_style_t *src)
{
    map_dash_style dst;
    dst.painted_length = src->painted_length;
    dst.unpainted_length = src->unpainted_length;
    dst.offset = src->offset;
    return dst;
}

mal_paint_style_t MapConverter::convert_paint_style(const map_paint_style *src)
{
    mal_paint_style_t dst;
    dst.width = src->width;
    dst.color = src->color;
    dst.angle = src->angle;
    dst.text_style = convert_text_style(&src->text_style);
    dst.fill_style = convert_fill_style(src->fill_style);
    dst.dash_style = convert_dash_style(&src->dash_style);
    return dst;
}

map_paint_style MapConverter::convert_paint_style(const mal_paint_style_t *src)
{
    map_paint_style dst;
    dst.width = src->width;
    dst.color = src->color;
    dst.angle = src->angle;
    dst.text_style = convert_text_style(&src->text_style);
    dst.fill_style = convert_fill_style(src->fill_style);
    dst.dash_style = convert_dash_style(&src->dash_style);
    return dst;
}

mal_bitmap_t MapConverter::convert_bitmap(const map_bitmap *src)
{
    mal_bitmap_t dst;
    dst.pixel_mode = convert_pixel_mode(src->pixel_mode);
    dst.buffer = src->buffer;
    dst.buffer_size = src->buffer_size;
    dst.width = src->width;
    dst.height = src->height;
    dst.stride = src->stride;
    dst.pre_multiplied = src->pre_multiplied;
    return dst;
}

map_bitmap MapConverter::convert_bitmap(const mal_bitmap_t *src)
{
    map_bitmap dst;
    dst.pixel_mode = convert_pixel_mode(src->pixel_mode);
    dst.buffer = src->buffer;
    dst.buffer_size = src->buffer_size;
    dst.width = src->width;
    dst.height = src->height;
    dst.stride = src->stride;
    dst.pre_multiplied = src->pre_multiplied;
    return dst;
}

mal_image_buf_info_t MapConverter::convert_image_buf_info(const map_image_buf_info *src)
{
    mal_image_buf_info_t dst;
    dst.image_format = convert_image_format(src->image_format);
    dst.buffer = src->buffer;
    dst.buffer_size = src->buffer_size;
    return dst;
}

map_image_buf_info MapConverter::convert_image_buf_info(const mal_image_buf_info_t *src)
{
    map_image_buf_info dst;
    dst.image_format = convert_image_format(src->image_format);
    dst.buffer = src->buffer;
    dst.buffer_size = src->buffer_size;
    return dst;
}

void MapConverter::convert_render_adapter(const map_render_adapter *adapter)
{
    map_render_adapter_.begin_drawing = adapter->begin_drawing;
    map_render_adapter_.commit_drawing = adapter->commit_drawing;
    map_render_adapter_.draw_bitmap = adapter->draw_bitmap;
    map_render_adapter_.draw_color = adapter->draw_color;
    map_render_adapter_.draw_image_buffer = adapter->draw_image_buffer;
    map_render_adapter_.draw_point = adapter->draw_point;
    map_render_adapter_.draw_polygon = adapter->draw_polygon;
    map_render_adapter_.draw_polyline = adapter->draw_polyline;
    map_render_adapter_.draw_svg_bitmap = adapter->draw_svg_bitmap;
    map_render_adapter_.draw_text = adapter->draw_text;
    map_render_adapter_.measure_text = adapter->measure_text;
}

void MapConverter::convert_render_callback(const map_render_callback *render)
{
    map_render_callback_.on_point_begin_draw = render->on_point_begin_draw;
    map_render_callback_.on_point_end_draw = render->on_point_end_draw;
    map_render_callback_.on_line_begin_draw = render->on_line_begin_draw;
    map_render_callback_.on_line_end_draw = render->on_line_end_draw;
    map_render_callback_.on_polygon_begin_draw = render->on_polygon_begin_draw;
    map_render_callback_.on_polygon_end_draw = render->on_polygon_end_draw;
    map_render_callback_.on_tile_begin_draw = render->on_tile_begin_draw;
    map_render_callback_.on_tile_end_draw = render->on_tile_end_draw;
    map_render_callback_.on_poi_begin_draw = render->on_poi_begin_draw;
    map_render_callback_.on_poi_end_draw = render->on_poi_end_draw;
    map_render_callback_.on_img_bin_begin_draw = render->on_img_bin_begin_draw;
    map_render_callback_.on_img_bin_end_draw = render->on_img_bin_end_draw;
    map_render_callback_.on_img_bin_layer_begin = render->on_img_bin_layer_begin;
    map_render_callback_.on_img_bin_layer_end = render->on_img_bin_layer_end;
}

void MapConverter::convert_track_listener(const map_track_navigation_listener *listener)
{
    map_track_listener_.on_navi_event = listener->on_navi_event;
    map_track_listener_.on_track_pass_point = listener->on_track_pass_point;
    map_track_listener_.on_trajector_yaw = listener->on_trajector_yaw;
    map_track_listener_.on_trajector_yaw_end = listener->on_trajector_yaw_end;
}

bool MapConverter::ConvertTrackNaviOverlay(const map_track_navi_overlay *src, mal_map_track_navi_overlay_t *dst)
{
    if (!src || !dst) {
        return false;
    }

    errno_t ret = memset_s(dst, sizeof(mal_map_track_navi_overlay_t), 0, sizeof(mal_map_track_navi_overlay_t));
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ConvertTrackNaviOverlay memset_s failed, ret: %d", ret);
        return false;
    }
    // 1. 转换 base_overlay
    dst->base_overlay = convert_base_overlay(src->base_overlay);

    // 2. 转换 track_info
    if (src->track_info) {
        dst->track_info = (mal_map_track_info_t *)ace_malloc(sizeof(mal_map_track_info_t));
        if (dst->track_info) {
            ConvertTrackInfo(src->track_info, dst->track_info, true);
        }
    }

    // 3. 转换 start_marker 和 end_marker
    dst->start_marker = convert_point_overlay_marker(src->start_marker);
    dst->end_marker = convert_point_overlay_marker(src->end_marker);

    // 4. 转换 custom_point_point_overlay_id
    dst->custom_point_point_overlay_id = src->custom_point_point_overlay_id;

    // 5. 转换 plan_polyline_options
    ConvertPlanPolylineOptions(&src->plan_polyline_options, &dst->plan_polyline_options);

    // 6. 转换 real_polyline_options
    ConvertRealPolylineOptions(&src->real_polyline_options, &dst->real_polyline_options);

    // 7. 转换 yaw_options
    ConvertYawOptions(&src->yaw_options, &dst->yaw_options);

    ConvertNaviOptions(&src->navi_options, &dst->navi_options);

    return true;
}

void MapConverter::FreeMalTrackNaviOverlay(mal_map_track_navi_overlay_t *overlay)
{
    if (!overlay)
        return;

    if (overlay->track_info) {
        FreeMalTrackInfo(overlay->track_info);
        ace_free(overlay->track_info);
    }

    if (overlay->plan_polyline_options.point_flags) {
        ace_free(overlay->plan_polyline_options.point_flags);
    }

    if (overlay->navi_options.pass_point_icon) {
        ace_free(overlay->navi_options.pass_point_icon);
    }

    if (overlay->navi_options.turn_point_icon) {
        ace_free(overlay->navi_options.turn_point_icon);
    }

    if (overlay->navi_options.pass_point_around_threshold) {
        ace_free(overlay->navi_options.pass_point_around_threshold);
    }

    if (overlay->navi_options.turn_point_around_threshold) {
        ace_free(overlay->navi_options.turn_point_around_threshold);
    }
}

bool MapConverter::ConvertTrackInfo(const map_track_info *src, mal_map_track_info_t *dst, bool need_full)
{
    if (!src || !dst) {
        return false;
    }
    errno_t ret = memset_s(dst, sizeof(mal_map_track_info_t), 0, sizeof(mal_map_track_info_t));
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ConvertTrackInfo memset_s failed, ret: %d", ret);
        return false;
    }

    dst->name = src->name;
    dst->desc = src->desc;

    dst->point_size = src->point_size;
    dst->via_point_index_size = src->via_point_index_size;
    dst->turn_point_size = src->turn_point_size;

    if (src->points && src->point_size > 0) {
        dst->points = ConvertPointsArray(src->points, src->point_size);
    }

    if (src->via_point_indexes && src->via_point_index_size > 0) {
        dst->via_point_indexes = ConvertInt32Array(src->via_point_indexes, src->via_point_index_size);
    }

    if (need_full && src->turn_points && src->turn_point_size > 0) {
        dst->turn_points = ConvertTurnPointsArray(src->turn_points, src->turn_point_size);
    }

    return true;
}

void MapConverter::FreeMalTrackInfo(mal_map_track_info_t *info)
{
    if (!info)
        return;

    if (info->points) {
        ace_free(info->points);
    }

    if (info->via_point_indexes) {
        ace_free(info->via_point_indexes);
    }

    if (info->turn_points) {
        ace_free(info->turn_points);
    }
}

bool MapConverter::ConvertPlanPolylineOptions(
    const map_track_plan_polyline_options *src, mal_map_track_plan_polyline_options_t *dst)
{
    if (!src || !dst)
        return false;

    errno_t ret =
        memset_s(dst, sizeof(mal_map_track_plan_polyline_options_t), 0, sizeof(mal_map_track_plan_polyline_options_t));
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ConvertPlanPolylineOptions memset_s failed, ret: %d", ret);
        return false;
    }

    dst->plan_polyline_marker = convert_line_overlay(src->plan_polyline_marker);
    dst->arrow_marker = convert_point_overlay_marker(src->arrow_marker);
    dst->show_arrow = src->show_arrow;

    if (src->point_flags && src->point_flags_size > 0) {
        dst->point_flags = ConvertUint8Array(src->point_flags, src->point_flags_size);
        dst->point_flags_size = src->point_flags_size;
    }

    return true;
}

bool MapConverter::ConvertRealPolylineOptions(
    const map_track_real_polyline_options *src, mal_map_track_real_polyline_options_t *dst)
{
    if (!src || !dst)
        return false;

    errno_t ret =
        memset_s(dst, sizeof(mal_map_track_real_polyline_options_t), 0, sizeof(mal_map_track_real_polyline_options_t));
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ConvertRealPolylineOptions memset_s failed, ret: %d", ret);
        return false;
    }

    dst->real_polyline_marker = convert_line_overlay(src->real_polyline_marker);
    dst->arrow_marker = convert_point_overlay_marker(src->arrow_marker);
    dst->show_arrow = src->show_arrow;

    return true;
}

bool MapConverter::ConvertYawOptions(const map_track_yaw_options *src, mal_map_track_yaw_options_t *dst)
{
    if (!src || !dst)
        return false;

    errno_t ret = memset_s(dst, sizeof(mal_map_track_yaw_options_t), 0, sizeof(mal_map_track_yaw_options_t));
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ConvertYawOptions memset_s failed, ret: %d", ret);
        return false;
    }

    dst->yaw_threshold = src->yaw_threshold;
    dst->is_show_yaw_polyline = src->is_show_yaw_polyline;
    dst->yaw_polyline_marker = convert_line_overlay(src->yaw_polyline_marker);
    dst->arrow_marker = convert_point_overlay_marker(src->arrow_marker);
    dst->show_arrow = src->show_arrow;

    return true;
}

bool MapConverter::ConvertNaviOptions(const map_track_navi_options *src, mal_map_track_navi_options_t *dst)
{
    if (!src || !dst)
        return false;

    errno_t ret = memset_s(dst, sizeof(mal_map_track_navi_options_t), 0, sizeof(mal_map_track_navi_options_t));
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ConvertNaviOptions memset_s failed, ret: %d", ret);
        return false;
    }

    dst->point_correcttion_threshold = src->point_correcttion_threshold;
    dst->navi_event_distance_threshold = src->navi_event_distance_threshold;
    dst->navi_calc_sampling = src->navi_calc_sampling;
    dst->tracking_mode = src->tracking_mode;

    if (src->pass_point_icon && src->pass_point_icon_size > 0) {
        dst->pass_point_icon = ConvertPointMarkerArray(src->pass_point_icon, src->pass_point_icon_size);
        dst->pass_point_icon_size = src->pass_point_icon_size;
    }

    if (src->turn_point_icon && src->turn_point_icon_size > 0) {
        dst->turn_point_icon = ConvertPointMarkerArray(src->turn_point_icon, src->turn_point_icon_size);
        dst->turn_point_icon_size = src->turn_point_icon_size;
    }

    if (src->pass_point_around_threshold && src->pass_point_around_threshold_size > 0) {
        dst->pass_point_around_threshold =
            ConvertDoubleArray(src->pass_point_around_threshold, src->pass_point_around_threshold_size);
        dst->pass_point_around_threshold_size = src->pass_point_around_threshold_size;
    }

    if (src->turn_point_around_threshold && src->turn_point_around_threshold_size > 0) {
        dst->turn_point_around_threshold =
            ConvertDoubleArray(src->turn_point_around_threshold, src->turn_point_around_threshold_size);
        dst->turn_point_around_threshold_size = src->turn_point_around_threshold_size;
    }

    return true;
}

mal_map_coord2d_t *MapConverter::ConvertPointsArray(const map_coord2d *src, size_t size)
{
    if (!src || size == 0)
        return nullptr;

    mal_map_coord2d_t *dst = (mal_map_coord2d_t *)ace_malloc(size * sizeof(mal_map_coord2d_t));
    if (!dst)
        return nullptr;

    for (size_t i = 0; i < size; i++) {
        dst[i] = convert_coord2d(src[i]);
    }

    return dst;
}

int32_t *MapConverter::ConvertInt32Array(const int32_t *src, size_t size)
{
    if (!src || size == 0)
        return nullptr;

    int32_t *dst = (int32_t *)ace_malloc(size * sizeof(int32_t));
    if (!dst)
        return nullptr;

    errno_t ret = memcpy_s(dst, size * sizeof(int32_t), src, size * sizeof(int32_t));
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ConvertInt32Array memcpy_s failed, ret: %d", ret);
        return nullptr;
    }
    return dst;
}

uint8_t *MapConverter::ConvertUint8Array(const uint8_t *src, size_t size)
{
    if (!src || size == 0)
        return nullptr;

    uint8_t *dst = (uint8_t *)ace_malloc(size);
    if (!dst)
        return nullptr;

    errno_t ret = memcpy_s(dst, size, src, size);
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ConvertUint8Array memcpy_s failed, ret: %d", ret);
        return nullptr;
    }
    return dst;
}

double *MapConverter::ConvertDoubleArray(const double *src, size_t size)
{
    if (!src || size == 0)
        return nullptr;

    double *dst = (double *)ace_malloc(size * sizeof(double));
    if (!dst)
        return nullptr;

    errno_t ret = memcpy_s(dst, size * sizeof(double), src, size * sizeof(double));
    if (ret != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ConvertDoubleArray memcpy_s failed, ret: %d", ret);
        return nullptr;
    }
    return dst;
}

mal_map_track_turn_point_info_t *MapConverter::ConvertTurnPointsArray(const map_track_turn_point_info *src, size_t size)
{
    if (!src || size == 0)
        return nullptr;

    mal_map_track_turn_point_info_t *dst =
        (mal_map_track_turn_point_info_t *)ace_malloc(size * sizeof(mal_map_track_turn_point_info_t));
    if (!dst)
        return nullptr;

    for (size_t i = 0; i < size; i++) {
        dst[i].index = src[i].index;
        dst[i].bear = src[i].bear;
        dst[i].turn_message = src[i].turn_message;
        dst[i].turn_location_message = src[i].turn_location_message;
    }

    return dst;
}

mal_map_point_overlay_marker_t *MapConverter::ConvertPointMarkerArray(const map_point_overlay_marker *src, size_t size)
{
    if (!src || size == 0)
        return nullptr;

    mal_map_point_overlay_marker_t *dst =
        (mal_map_point_overlay_marker_t *)ace_malloc(size * sizeof(mal_map_point_overlay_marker_t));
    if (!dst)
        return nullptr;

    for (size_t i = 0; i < size; i++) {
        dst[i] = convert_point_overlay_marker(src[i]);
    }
    return dst;
}

void MapConverter::track_on_track_pass_point(int32_t index, mal_map_coord2d_t point)
{
    if (map_track_listener_.on_track_pass_point) {
        map_coord2d coord2d = convert_map_coord2d(point);
        map_track_listener_.on_track_pass_point(index, coord2d);
    }
}

void MapConverter::track_on_trajector_yaw(double distance)
{
    if (map_track_listener_.on_trajector_yaw) {
        map_track_listener_.on_trajector_yaw(distance);
    }
}

void MapConverter::track_on_trajector_yaw_end()
{
    if (map_track_listener_.on_trajector_yaw_end) {
        map_track_listener_.on_trajector_yaw_end();
    }
}

void MapConverter::track_on_navi_event(mal_map_track_navi_event_info_t *event)
{
    if (map_track_listener_.on_navi_event) {
        map_track_navi_event_info map_event;
        map_event.point_index = event->point_index;
        map_event.distance = event->distance;
        map_event.turn_message = event->turn_message;
        map_event.turn_location_message = event->turn_location_message;
        map_event.bear = event->bear;
        map_event.event_type = convert_track_navi_event_type_mode(event->event_type);
        map_event.point_type = convert_track_point_type_mode(event->point_type);
        map_track_listener_.on_navi_event(&map_event);
    }
}

}  // namespace ACELite
}  // namespace OHOS
