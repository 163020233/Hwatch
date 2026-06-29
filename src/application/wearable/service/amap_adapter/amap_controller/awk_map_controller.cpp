
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: amap controller
 * Created: 2026-01-14
 */

#include "awk_map_controller.h"
#include <awk_defines.h>
#include <map/awk_map.h>
#include <awk.h>
#include <cstring>
#include <sys/stat.h>
#include "securec.h"
#include "map/awk_map_defines.h"
#include "amap_system_adapter.h"
#include "awk_mal_render_adapter.h"
#include "mal_awk_convert.h"
#include "mal_common_defines.h"
#include "map_config.h"
#include "amap_log_adapter.h"
#include "mal_map_defines.h"

static constexpr uint16_t DIR_MODE_DEFAULT = 0755;
static constexpr char PATH_SEPARATOR = '/';
static constexpr int32_t MAX_PATH_LEN = 256;
static constexpr double LNG_MIN = -180.0;
static constexpr double LNG_MAX = 180.0;
static constexpr double LAT_MIN = -90.0;
static constexpr double LAT_MAX = 90.0;
static constexpr float MAP_LEVEL_MIN = 3.0f;
static constexpr float MAP_LEVEL_MAX = 20.0f;

/**
 * @brief 地图SDK初始化错误码
 */
typedef enum {
    MAL_INIT_SUCCESS = 0,
    MAL_INIT_ERR_ALREADY_INITIALIZED = -1,  // 已经初始化，不能重复初始化
} mal_init_error_t;

typedef enum {
    MAL_MAP_ACTIVATE_SUCCESS = 0,  // 设备已激活
    MAL_MAP_ACTIVATE_IN_PROGRESS   // 激活请求已发起，等待回调
} mal_map_activate_result_t;

// 向上提供真实控制器，生命周期上调用方管理
extern "C" OHOS::IMapController *ProvideMapController()
{
    return new OHOS::AwkMapController();
}

namespace OHOS {

/**
 * 私有方法不对外暴露
 */
static mal_map_base_overlay_t *awk_to_mal_map_base_overlay_t(const awk_map_base_overlay_t *src)
{
    if (!src) {
        return nullptr;
    }
    switch (src->geometry_type) {
        case AWK_MAP_OVERLAY_TYPE_POINT: {
            mal_map_base_overlay_t *mal_overylay =
                (mal_map_base_overlay_t *)amap_mem_malloc(sizeof(mal_map_point_overlay_t));
            awk_to_mal_map_point_overlay_t((awk_map_point_overlay_t *)src, (mal_map_point_overlay_t *)mal_overylay);
            return mal_overylay;
        }
        case AWK_MAP_OVERLAY_TYPE_LINE: {
            mal_map_base_overlay_t *mal_overylay =
                (mal_map_base_overlay_t *)amap_mem_malloc(sizeof(mal_map_polyline_overlay_t));
            awk_to_mal_map_polyline_overlay_t((awk_map_polyline_overlay_t *)src,
                                              (mal_map_polyline_overlay_t *)mal_overylay);
            return mal_overylay;
        }
        case AWK_MAP_OVERLAY_TYPE_POLYGON: {
            mal_map_base_overlay_t *mal_overylay =
                (mal_map_base_overlay_t *)amap_mem_malloc(sizeof(mal_map_polygon_overlay_t));
            awk_to_mal_map_polygon_overlay_t((awk_map_polygon_overlay_t *)src,
                                             (mal_map_polygon_overlay_t *)mal_overylay);
            return mal_overylay;
        }
            //  新增：轨迹导航类型
        case AWK_MAP_OVERLAY_TYPE_TRACK_NAVI: {
            mal_map_base_overlay_t *mal_overlay =
                (mal_map_base_overlay_t *)amap_mem_malloc(sizeof(mal_map_track_navi_overlay_t));
            awk_to_mal_map_track_navi_overlay_t((awk_map_track_navi_overlay_t *)src,
                                                (mal_map_track_navi_overlay_t *)mal_overlay);
            return mal_overlay;
        }

        default:
            // 不支持的转换类型
            return nullptr;
    }
}

// 激活成功回调
static void OnDeviceActiveSuccess(const char *device_id, const char *license_id)
{
    AMAP_LOGI("Device activated, device_id: %s", device_id ? device_id : "null");
    AMAP_LOGI("Device activated, license_id: %s", license_id ? license_id : "null");
}

// 激活失败回调
static void OnDeviceActiveFail(int code, const char *msg)
{
    AMAP_LOGE("Device activation failed, code: %d, msg: %s", code, msg ? msg : "unknown");
}

static int32_t CreateDirIfNotExist(const char *path, const awk_file_adapter_t &adapter)
{
    if (adapter.file_dir_exists(path)) {
        return 0;
    }

    int32_t ret = adapter.file_mkdir(path, DIR_MODE_DEFAULT);
    if (ret != 0) {
        AMAP_LOGE("mkdir failed: %s, ret=%d", path, ret);
        return ret;
    }

    AMAP_LOGD("mkdir success: %s", path);
    return 0;
}

static int32_t EnsureDirRecursiveLite(const char *path, const awk_file_adapter_t &adapter)
{
    if (path == nullptr) {
        AMAP_LOGE("path is null");
        return -1;
    }

    char temp[MAX_PATH_LEN] = {0};

    int32_t len = (int32_t)strlen(path);
    if (len <= 0 || len >= MAX_PATH_LEN) {
        AMAP_LOGE("invalid path length: %d", len);
        return -1;
    }

    errno_t retSafe = memcpy_s(temp, sizeof(temp), path, len);
    if (retSafe != EOK) {
        AMAP_LOGE("memcpy_s failed, ret=%d", retSafe);
        return -1;
    }

    temp[len] = '\0';
    // 如果以 '/' 开头，从 root 开始
    int32_t start = 0;
    if (temp[0] == PATH_SEPARATOR) {
        start = 1;
    }

    for (int32_t i = start; i < len; i++) {
        if (temp[i] != PATH_SEPARATOR) {
            continue;
        }

        temp[i] = '\0';

        // 跳过空段
        if (temp[0] != '\0') {
            int32_t ret = CreateDirIfNotExist(temp, adapter);
            if (ret != 0) {
                return ret;
            }
        }

        temp[i] = PATH_SEPARATOR;
    }

    // 创建最后一段
    int32_t ret = CreateDirIfNotExist(temp, adapter);
    if (ret != 0) {
        return ret;
    }

    return 0;
}

static void InitBaseContext(awk_context_t &ctx)
{
    static constexpr int32_t TILE_MEM_CACHE_KB = 1536;

    ctx.device_id = const_cast<char *>("rtos-mainland-raster-10086");
    ctx.key = const_cast<char *>("cebd61d505fb29c68e8c7cfd8b255e46");
    ctx.offline_map_dir = const_cast<char *>(OFFLINE_MAP_DIR);
    ctx.root_dir = const_cast<char *>(MAP_ROOT_DIR);  // SDK内部文件夹根路径

    ctx.tile_disk_cache_max_size = 0;                     //!< tile磁盘缓存最大空间，单位: MB
    ctx.tile_mem_cache_max_size = TILE_MEM_CACHE_KB;      //!< tile内存缓存最大空间，单位：KB
    ctx.poi_tile_mem_cache_max_size = TILE_MEM_CACHE_KB;  //!< poi瓦片内存缓存最大空间，单位：KB

#ifdef AMAP_SVG_SUPPORT
    AMAP_LOGD("tile_style: VECTOR");
    ctx.tile_style = AWK_MAP_TILE_STYLE_VECTOR;
#else
    AMAP_LOGD("tile_style: IMAGE_BINARY");
    ctx.tile_style = AWK_MAP_TILE_STYLE_IMAGE_BINARY;
#endif

    ctx.tile_load_mode = AWK_MAP_TILE_LOAD_OFFLINE;
    ctx.tile_clip_load = false;
    ctx.tile_background_custom_draw = false;
}

static void InitAdapter(awk_context_t &ctx)
{
    // fast memory
    ctx.fast_memory_adapter.mem_malloc = amap_fast_mem_malloc;
    ctx.fast_memory_adapter.mem_free = amap_fast_mem_free;
    ctx.fast_memory_adapter.mem_free_type = amap_fast_mem_free_type;

    // memory
    ctx.memory_adapter.mem_malloc = amap_mem_malloc;
    ctx.memory_adapter.mem_free = amap_mem_free;
    ctx.memory_adapter.mem_calloc = amap_mem_calloc;
    ctx.memory_adapter.mem_realloc = amap_mem_realloc;

    // file
    ctx.file_adapter.file_open = amap_file_open;
    ctx.file_adapter.file_close = amap_file_close;
    ctx.file_adapter.file_read = amap_file_read;
    ctx.file_adapter.file_write = amap_file_write;
    ctx.file_adapter.file_mkdir = amap_file_mkdir;
    ctx.file_adapter.file_exists = amap_file_exists;
    ctx.file_adapter.file_remove = amap_file_remove;
    ctx.file_adapter.file_opendir = amap_file_opendir;
    ctx.file_adapter.file_closedir = amap_file_closedir;
    ctx.file_adapter.file_readdir = amap_file_readdir;
    ctx.file_adapter.file_seek = amap_file_seek;
    ctx.file_adapter.file_tell = amap_file_tell;
    ctx.file_adapter.file_flush = amap_file_flush;
    ctx.file_adapter.file_rmdir = amap_file_rmdir;
    ctx.file_adapter.file_dir_exists = amap_file_dir_exists;
    ctx.file_adapter.file_get_size = amap_file_get_size;
    ctx.file_adapter.file_get_last_access = amap_file_get_last_access;
    ctx.file_adapter.file_rename = amap_file_rename;
    ctx.file_adapter.file_unzip = amap_file_unzip;

    // network
    ctx.network_adapter.send = amap_send;
    ctx.network_adapter.cancel = amap_cancel;
}

static void InitRenderAdapter(awk_context_t &ctx, mal_context_t &mal_context)
{
    awk_render_adapter_init(mal_context.render_adapter, mal_context.map_render_callback);

    ctx.render_adapter.begin_drawing = awk_render_begin_drawing_adapter;
    ctx.render_adapter.commit_drawing = awk_render_commit_drawing_adapter;
    ctx.render_adapter.draw_point = awk_render_point_adapter;
    ctx.render_adapter.draw_polyline = awk_render_polyline_adapter;
    ctx.render_adapter.draw_polygon = awk_render_polygon_adapter;
    ctx.render_adapter.draw_image_buffer = awk_render_draw_image_buffer;
    ctx.render_adapter.draw_bitmap = awk_render_bitmap_adapter;
    ctx.render_adapter.draw_color = awk_render_color_adapter;
    ctx.render_adapter.draw_text = awk_render_draw_text_adapter;
    ctx.render_adapter.measure_text = awk_render_measure_text_adapter;
    ctx.render_adapter.draw_svg_bitmap = awk_render_draw_svg_bitmap_adapter;
}

static void InitSystemAdapter(awk_context_t &ctx)
{
    ctx.thread_adapter.get_thread_id = amap_get_thread_id;
    ctx.system_adapter.get_system_time = amap_get_system_time;
    ctx.system_adapter.log_printf = amap_log_printf;
}

AwkMapController::AwkMapController() : context_{} {}

AwkMapController::~AwkMapController() {}

/*********************
 *   初始化
 *********************/
int32_t AwkMapController::mal_map_init(mal_context_t mal_context)
{
    AMAP_LOGD("AwkMapController::mal_map_init");
    memset_s(&context_, sizeof(awk_context_t), 0, sizeof(awk_context_t));  // 务必先memset
    InitBaseContext(context_);
    InitAdapter(context_);
    InitRenderAdapter(context_, mal_context);
    InitSystemAdapter(context_);
    context_.tile_pixel_mode = (awk_pixel_mode_t)mal_context.tile_pixel_mode;  // 对齐一致可以强转
    AMAP_LOGD("tile_pixel_mode = %d", (int)mal_context.tile_pixel_mode);
    int32_t ret = EnsureDirRecursiveLite(MAP_ROOT_DIR, context_.file_adapter);
    if (ret != MAL_INIT_SUCCESS) {
        return ret;
    }

    ret = EnsureDirRecursiveLite(OFFLINE_MAP_DIR, context_.file_adapter);
    if (ret != MAL_INIT_SUCCESS) {
        return ret;
    }

    // ============ SDK 初始化 ============
    AMAP_LOGD("awk_init");
    ret = awk_init(&context_);
    AMAP_LOGD("awk_init ret = %d", ret);
    if (ret != MAL_INIT_SUCCESS && ret != MAL_INIT_ERR_ALREADY_INITIALIZED) {
        AMAP_LOGE("awk_init failed: %d", ret);
    }

    return ret;
}

int32_t AwkMapController::mal_map_uninit(void)
{
    awk_render_adapter_deinit();
    return awk_uninit();
}

/*********************
 *    地图操作
 *********************/
// 直接将多个接口归一以参数传递，避免UIKit分散调用
int32_t AwkMapController::mal_map_create_view(mal_map_view_create_param_t param)
{
    int32_t map_id = awk_map_create_view(mal_to_awk_map_view_param_t(param.view_param));  // 创建地图，返回地图id
    AMAP_LOGD("AwkMapController::mal_map_create_view map_id = %d", map_id);
    if (map_id < 0) {
        return map_id;
    }
    // 设置视口和层级
    AMAP_LOGD("param.map_center.lat = %f param.map_center.lon = %f", param.map_center.lat, param.map_center.lon);

    int32_t ret = awk_map_set_center(map_id, mal_to_awk_map_coord2d_t(param.map_center));
    if (ret != MAL_API_SUCCESS) {
        AMAP_LOGE("AwkMapController::awk_map_set_center failed, ret = %d", ret);
        return map_id;
    }
    ret = awk_map_set_level(map_id, param.zoom);
    if (ret != MAL_API_SUCCESS) {
        AMAP_LOGE("AwkMapController::awk_map_set_level failed, param.zoom = %f ret = %d", param.zoom, ret);
        return map_id;
    }

    awk_map_render_callback_t render_callback;
    // 回调需要适配转换结构数据，统一到mal层,
    // 待确认这部分是随context初始传入，还是走param传入合适。C没法动态代理只能适配写死
    memset_s(&render_callback, sizeof(awk_map_render_callback_t), 0, sizeof(awk_map_render_callback_t));
    render_callback.on_point_begin_draw = awk_map_on_point_begin_draw_callback;
    render_callback.on_point_end_draw = awk_map_on_point_end_draw_callback;
    render_callback.on_line_begin_draw = awk_map_on_line_begin_draw_callback,
    render_callback.on_line_end_draw = awk_map_on_line_end_draw_callback,
    render_callback.on_polygon_begin_draw = awk_map_on_polygon_begin_draw_callback,
    render_callback.on_polygon_end_draw = awk_map_on_polygon_end_draw_callback,
    render_callback.on_poi_begin_draw = awk_map_on_poi_begin_draw_callback,
    render_callback.on_poi_end_draw = awk_map_on_poi_end_draw_callback,
    render_callback.on_tile_begin_draw = awk_map_on_tile_begin_draw_callback;
    render_callback.on_tile_end_draw = awk_map_on_tile_end_draw_callback;
    render_callback.on_img_bin_begin_draw = awk_map_on_img_bin_begin_draw_callback;
    render_callback.on_img_bin_end_draw = awk_map_on_img_bin_end_draw_callback;

    // 在线地图加载接口暂时先未适配，待有需要增加
    // 设置渲染事件回调
    awk_map_set_render_callback(render_callback);
    return map_id;
}

int32_t AwkMapController::mal_map_activate_device()
{
    // ============ 激活参数 ============
    awk_device_activate_param_t activate_param;
    memset_s(&activate_param, sizeof(awk_device_activate_param_t), 0, sizeof(awk_device_activate_param_t));
    activate_param.area = const_cast<char *>("mainland");
    activate_param.country = const_cast<char *>("cn");
    activate_param.type = 0;
    activate_param.data_type = const_cast<char *>("vector");
    // ============ 检查激活状态 ============
    bool isContextInited = awk_check_device_activated(&activate_param, &context_);
    AMAP_LOGD("awk_check_device_activated isContextInited = %d", isContextInited);

    // ============ 设备激活 ============

    if (isContextInited) {
        return MAL_MAP_ACTIVATE_SUCCESS;
    }
    awk_device_active_callback callback;
    callback.awk_device_active_on_success = OnDeviceActiveSuccess;
    callback.awk_device_active_on_fail = OnDeviceActiveFail;

    AMAP_LOGD("Starting device activation...");
    awk_activate_device(&activate_param, callback);
    AMAP_LOGD("awk_activate_device end");
    return MAL_MAP_ACTIVATE_IN_PROGRESS;
}

int32_t AwkMapController::mal_map_destroy_view(uint32_t map_id)
{
    return awk_map_destroy_view(map_id);
}

int32_t AwkMapController::mal_map_do_render(void)
{
    AMAP_LOGD("start AwkMapController::mal_map_do_render");
    int32_t ret = awk_map_do_render();
    AMAP_LOGD("AwkMapController::mal_map_do_render with ret %d", ret);
    return ret;
}

int32_t AwkMapController::mal_map_pause_render(uint32_t map_id)
{
    return awk_map_pause_render(map_id);
}

int32_t AwkMapController::mal_map_resume_render(uint32_t map_id)
{
    return awk_map_resume_render(map_id);
}

int32_t AwkMapController::mal_map_add_texture(const mal_map_texture_data_t *texture_data)
{
    if (!texture_data) {
        return MAL_API_NULL_PARAM;
    }
    awk_map_texture_data_t awk_tex;
    mal_to_awk_map_texture_data_t(texture_data, &awk_tex);
    return awk_map_add_texture(&awk_tex);
}

int32_t AwkMapController::mal_map_update_texture(uint32_t texture_id, const mal_map_texture_data_t *texture_data)
{
    awk_map_texture_data_t awk_tex;
    mal_to_awk_map_texture_data_t(texture_data, &awk_tex);
    return awk_map_update_texture(texture_id, &awk_tex);
}

int32_t AwkMapController::mal_map_remove_texture(uint32_t texture_id)
{
    return awk_map_remove_texture(texture_id);
}

int32_t AwkMapController::mal_map_init_point_overlay(mal_map_point_overlay_t *point_overlay)
{
    if (!point_overlay) {
        return MAL_API_NULL_PARAM;
    }
    awk_map_point_overlay_t awk_overlay = {};
    mal_to_awk_map_point_overlay_t(point_overlay, &awk_overlay);
    int32_t result = awk_map_init_point_overlay(&awk_overlay);
    if (result != MAL_API_SUCCESS) {
        return result;
    }
    awk_to_mal_map_point_overlay_t(&awk_overlay, point_overlay);
    return result;
}

int32_t AwkMapController::mal_map_init_line_overlay(mal_map_polyline_overlay_t *line_overlay)
{
    if (!line_overlay) {
        return MAL_API_NULL_PARAM;
    }
    awk_map_polyline_overlay_t awk_line_overlay = {};
    mal_to_awk_map_polyline_overlay_t(line_overlay, &awk_line_overlay);
    int32_t result = awk_map_init_line_overlay(&awk_line_overlay);
    if (result != MAL_API_SUCCESS) {
        return result;
    }
    awk_to_mal_map_polyline_overlay_t(&awk_line_overlay, line_overlay);
    return result;
}

int32_t AwkMapController::mal_map_init_polygon_overlay(mal_map_polygon_overlay_t *polygon_overlay)
{
    if (!polygon_overlay) {
        return MAL_API_NULL_PARAM;
    }
    awk_map_polygon_overlay_t awk_polygon_overlay = {};
    mal_to_awk_map_polygon_overlay_t(polygon_overlay, &awk_polygon_overlay);
    int32_t result = awk_map_init_polygon_overlay(&awk_polygon_overlay);
    if (result != MAL_API_SUCCESS) {
        return result;
    }
    awk_to_mal_map_polygon_overlay_t(&awk_polygon_overlay, polygon_overlay);
    return result;
}

int32_t AwkMapController::mal_map_add_overlay(uint32_t map_id, mal_map_base_overlay_t *overlay)
{
    // 两个结构体的第一个成员的类型相同，那么该结构体的起始地址与其第一个成员的地址相同(C继承的标准手法)
    // 这里的mal_map_base_overlay_t并没有代表实际类型，实际可能是点、线、面
    if (!overlay) {
        return awk_map_add_overlay(map_id, nullptr);
    }
    switch (overlay->geometry_type) {
        case MAL_MAP_OVERLAY_TYPE_POINT: {
            awk_map_point_overlay_t awk_overlay = {};
            mal_to_awk_map_point_overlay_t((mal_map_point_overlay_t *)overlay, &awk_overlay);
            return awk_map_add_overlay(map_id, (awk_map_base_overlay_t *)&awk_overlay);
        }
        case MAL_MAP_OVERLAY_TYPE_LINE: {
            awk_map_polyline_overlay_t awk_line_overlay = {};
            mal_to_awk_map_polyline_overlay_t((mal_map_polyline_overlay_t *)overlay, &awk_line_overlay);
            return awk_map_add_overlay(map_id, (awk_map_base_overlay_t *)&awk_line_overlay);
        }
        case MAL_MAP_OVERLAY_TYPE_POLYGON: {
            awk_map_polygon_overlay_t awk_polygon_overlay = {};
            mal_to_awk_map_polygon_overlay_t((mal_map_polygon_overlay_t *)overlay, &awk_polygon_overlay);
            return awk_map_add_overlay(map_id, (awk_map_base_overlay_t *)&awk_polygon_overlay);
        }
        default:
            return awk_map_add_overlay(map_id, nullptr);
    }
}

int32_t AwkMapController::mal_map_update_overlay(uint32_t map_id, mal_map_base_overlay_t *overlay)
{
    if (!overlay) {
        return awk_map_update_overlay(map_id, nullptr);
    }
    switch (overlay->geometry_type) {
        case MAL_MAP_OVERLAY_TYPE_POINT: {
            awk_map_point_overlay_t awk_overlay = {};
            mal_to_awk_map_point_overlay_t((mal_map_point_overlay_t *)overlay, &awk_overlay);
            return awk_map_update_overlay(map_id, (awk_map_base_overlay_t *)&awk_overlay);
        }
        case MAL_MAP_OVERLAY_TYPE_LINE: {
            awk_map_polyline_overlay_t awk_line_overlay = {};
            mal_to_awk_map_polyline_overlay_t((mal_map_polyline_overlay_t *)overlay, &awk_line_overlay);
            return awk_map_update_overlay(map_id, (awk_map_base_overlay_t *)&awk_line_overlay);
        }
        case MAL_MAP_OVERLAY_TYPE_POLYGON: {
            awk_map_polygon_overlay_t awk_polygon_overlay = {};
            mal_to_awk_map_polygon_overlay_t((mal_map_polygon_overlay_t *)overlay, &awk_polygon_overlay);
            return awk_map_update_overlay(map_id, (awk_map_base_overlay_t *)&awk_polygon_overlay);
        }
        default:
            return awk_map_update_overlay(map_id, nullptr);
    }
}

int32_t AwkMapController::mal_map_remove_overlay(uint32_t map_id, uint32_t overlay_id)
{
    return awk_map_remove_overlay(map_id, overlay_id);
}

// 统一走内存copy，外部释放的规则，否则后续有新的SDK接入，结构不一致后，此处复制上层需要修改
const mal_map_base_overlay_t *AwkMapController::mal_map_find_overlay(uint32_t map_id, uint32_t overlay_id)
{
    // 如果后续结构不一致了，此处不能强转，所以是不是应
    const awk_map_base_overlay_t *awk_map_base_overlay = awk_map_find_overlay(map_id, overlay_id);
    if (!awk_map_base_overlay) {
        return nullptr;
    }
    return awk_to_mal_map_base_overlay_t(awk_map_base_overlay);
}

uint32_t AwkMapController::mal_map_get_overlay_count(uint32_t map_id)
{
    return awk_map_get_overlay_count(map_id);
}

const mal_map_base_overlay_t *AwkMapController::mal_map_get_overlay(uint32_t map_id, uint32_t index)
{
    // 返回指针生命周期由底层管理，调用者不free, 后续兼容建议让接口调用方释放内存，此处进行copy,
    // 其实正常应用调用者传入buffer
    const awk_map_base_overlay_t *awk_map_base_overlay = awk_map_get_overlay(map_id, index);
    return awk_to_mal_map_base_overlay_t(awk_map_base_overlay);
}

int32_t AwkMapController::mal_map_release_overlay(mal_map_base_overlay_t *overlay)
{
    if (!overlay) {
        return MAL_API_NULL_PARAM;
    }
    // 处理含动态内存的类型
    if (overlay->geometry_type == MAL_MAP_OVERLAY_TYPE_POLYGON) {
        mal_map_polygon_overlay_t *poly = (mal_map_polygon_overlay_t *)overlay;
        amap_mem_free(poly->points);  // 安全：即使 points == NULL，free 也安全
    }
    // 其他类型（POINT/LINE）无内部堆内存，直接 free 即可
    amap_mem_free(overlay);
    return MAL_API_SUCCESS;
}

int32_t AwkMapController::mal_map_set_view_port(uint32_t map_id, mal_map_view_port_t view_port)
{
    return awk_map_set_view_port(map_id, mal_to_awk_map_view_port_t(view_port));
}

int32_t AwkMapController::mal_map_set_render_callback(mal_map_render_callback_t callback)
{
    return awk_map_set_render_callback(mal_to_awk_map_render_callback_t(callback));
}

int32_t AwkMapController::mal_map_set_center(uint32_t map_id, mal_map_coord2d_t coord2d)
{
    return awk_map_set_center(map_id, mal_to_awk_map_coord2d_t(coord2d));
}

int32_t AwkMapController::mal_map_set_level(uint32_t map_id, float level)
{
    return awk_map_set_level(map_id, level);
}

int32_t AwkMapController::mal_map_set_roll_angle(uint32_t map_id, float roll_angle)
{
    return awk_map_set_roll_angle(map_id, roll_angle);
}

int32_t AwkMapController::mal_map_get_posture(uint32_t map_id, mal_map_posture_t *posture)
{
    awk_map_posture_t awk_map_posture{};
    int32_t ret = awk_map_get_posture(map_id, &awk_map_posture);
    awk_to_mal_map_posture_t(&awk_map_posture, posture);
    // awk_map_posture为栈变量无须清理内存
    return ret;
}

int32_t AwkMapController::mal_map_lonlat_to_xy(uint32_t map_id, mal_map_coord2d_t lonlat, int32_t *x, int32_t *y)
{
    return awk_map_lonlat_to_xy(map_id, mal_to_awk_map_coord2d_t(lonlat), x, y);
}

int32_t AwkMapController::mal_map_xy_to_lonlat(uint32_t map_id, int32_t x, int32_t y, mal_map_coord2d_t *lonlat)
{
    awk_map_coord2d_t awk_map_coord2d{};
    int32_t ret = awk_map_xy_to_lonlat(map_id, x, y, &awk_map_coord2d);
    awk_to_mal_map_coord2d_t(&awk_map_coord2d, lonlat);
    return ret;
}

int32_t AwkMapController::mal_map_touch_begin(uint32_t map_id, int32_t x, int32_t y)
{
    return awk_map_touch_begin(map_id, x, y);
}

int32_t AwkMapController::mal_map_touch_update(uint32_t map_id, int32_t x, int32_t y)
{
    return awk_map_touch_update(map_id, x, y);
}

int32_t AwkMapController::mal_map_touch_end(uint32_t map_id, int32_t x, int32_t y)
{
    return awk_map_touch_end(map_id, x, y);
}

mal_map_point_overlay_t **AwkMapController::mal_map_click_points(uint32_t map_id, int32_t x, int32_t y,
                                                                 int32_t *overlay_count)
{
    awk_map_point_overlay_t **awk_click_points = awk_map_click_points(map_id, x, y, overlay_count);
    if (!awk_click_points || !overlay_count || *overlay_count <= 0) {
        return nullptr;
    }
    mal_map_point_overlay_t **mal_list =
        static_cast<mal_map_point_overlay_t **>(amap_mem_malloc((*overlay_count) * sizeof(*mal_list)));
    if (!mal_list) {
        return nullptr;
    }

    for (int32_t i = 0; i < *overlay_count; ++i) {
        mal_list[i] = static_cast<mal_map_point_overlay_t *>(amap_mem_malloc(sizeof(mal_map_point_overlay_t)));
        if (!mal_list[i]) {
            // 已分配内存全部释放
            for (int32_t j = 0; j < i; ++j) {
                amap_mem_free(mal_list[j]);
            }
            amap_mem_free(mal_list);
            return nullptr;
        }
        awk_to_mal_map_point_overlay_t(awk_click_points[i], mal_list[i]);
    }

    // 释放awk_click_points
    awk_map_release_overlays((awk_map_base_overlay_t **)awk_click_points, *overlay_count);
    return mal_list;
}

int32_t AwkMapController::mal_map_release_overlays(mal_map_base_overlay_t **overlays, int32_t overlay_count)
{
    if (!overlays || overlay_count <= 0) {
        return 0;
    }
    for (int32_t i = 0; i < overlay_count; ++i) {
        if (overlays[i] == nullptr) {
            continue;
        }

        // 处理含动态内存的类型
        if (overlays[i]->geometry_type == MAL_MAP_OVERLAY_TYPE_POLYGON) {
            mal_map_polygon_overlay_t *poly = (mal_map_polygon_overlay_t *)overlays[i];
            amap_mem_free(poly->points);  // 安全：即使 points == NULL，free 也安全
        }
        // 其他类型（POINT/LINE）无内部堆内存，直接 free 即可
        amap_mem_free(overlays[i]);
    }
    amap_mem_free(overlays);
    return 0;  // 0 表示成功
}

/**
 * @brief 获取地图元素外接矩形边界和地图级别，方法的调用需要在主流程线程中，仅支持线/面/轨迹导航overlay
 * @param {uint32_t} map_id 地图实例id
 * @param {awk_map_base_overlay_t} overlay 参数（参数不为空仅返回该overlay参数，如果为空则返回地图中所有overlay的范围）
 * @param {awk_map_coord2d_t} north_east 返回的东北角坐标
 * @param {awk_map_coord2d_t} south_west 返回的西南角坐标
 * @param {float} level 返回的地图级别
 * @return {成功返回0, 失败返回error: -1表示没有初始化 -3初始化的线程和当前调用不一致 -2表示overlay类型错误}
 */
int32_t AwkMapController::mal_map_get_map_bounds(uint32_t map_id, mal_map_base_overlay_t *overlay,
                                                 mal_map_coord2d_t *north_east, mal_map_coord2d_t *south_west,
                                                 float *level)
{
    if (map_id == 0) {
        AMAP_LOGE("Invalid map_id: %u", map_id);
        return -1;
    }
    if (!north_east || !south_west || !level) {
        AMAP_LOGE("Output parameters cannot be nullptr");
        return -1;
    }
    awk_map_base_overlay_t *p_awk_base = nullptr;
    awk_map_base_overlay_t temp_awk_base;
    if (overlay) {
        mal_to_awk_map_base_overlay_t(overlay, &temp_awk_base);
        p_awk_base = &temp_awk_base;
    }
    awk_map_coord2d_t ne;
    awk_map_coord2d_t sw;
    float zoom_level = 0.0f;
    int32_t ret = awk_map_get_map_bounds(map_id, p_awk_base, &ne, &sw, &zoom_level);
    if (ret != 0) {
        AMAP_LOGE("Failed to get bounds from engine, error code: %d", ret);
        return ret;
    }
    north_east->lon = ne.lon;
    north_east->lat = ne.lat;
    south_west->lon = sw.lon;
    south_west->lat = sw.lat;
    *level = zoom_level;
    AMAP_LOGD("Bounds calculated: NE(%.6f, %.6f), SW(%.6f, %.6f), Level=%.2f", ne.lon, ne.lat, sw.lon, sw.lat,
             zoom_level);
    return ret;
}

/**
 * @brief 设置地图view的展示区域
 *
 * @param map_id {uint32_t} map_id 地图实例id
 * @param map_view_rect 区域rect
 * @return int32_t {成功返回0, 失败返回error: -1表示没有初始化 -3初始化的线程和当前调用不一致}
 */
int32_t AwkMapController::mal_map_set_map_view_rect(uint32_t map_id, mal_rect_area_t map_view_rect)
{
    if (map_id == 0) {
        AMAP_LOGE("Invalid map_id: %u", map_id);
        return -1;
    }
    if (map_view_rect.width <= 0 || map_view_rect.height <= 0) {
        AMAP_LOGE("Invalid rect: [x=%d, y=%d, width=%d, height=%d] - width and height must be positive", map_view_rect.x,
                 map_view_rect.y, map_view_rect.width, map_view_rect.height);
        return -1;
    }
    awk_rect_area_t rect = {
        .x = map_view_rect.x, .y = map_view_rect.y, .width = map_view_rect.width, .height = map_view_rect.height};
    int32_t ret = awk_map_set_map_view_rect(map_id, rect);
    if (ret != 0) {
        AMAP_LOGE("Failed to set map view rect for map_id=%u, error=%d", map_id, ret);
        return -1;
    }
    AMAP_LOGD("Map view rect set successfully: map_id=%u, rect=[x=%d,y=%d,width=%d,height=%d]", map_id, map_view_rect.x,
             map_view_rect.y, map_view_rect.width, map_view_rect.height);
    return 0;
}
/**
 * @brief 获取地图级别，方法的调用需要在主流程线程中
 * @param {uint32_t} map_id 地图实例id
 * @return {成功返回当前级别 失败返回error: -1表示没有初始化  -2 未找到该map_id对应的地图
 * -3初始化的线程和当前调用不一致}
 */
float AwkMapController::mal_map_get_level(uint32_t map_id)
{
    if (map_id == 0) {
        AMAP_LOGE("Invalid map_id: %u", map_id);
        return -2.0f;
    }
    awk_map_posture_t posture;
    int32_t ret = awk_map_get_posture(map_id, &posture);
    if (ret != 0) {
        AMAP_LOGE("Failed to get posture for map_id=%u, error=%d", map_id, ret);
        return -1.0f;
    }
    float level = posture.level;
    AMAP_LOGD("Got map level: map_id=%u, level=%.2f", map_id, level);
    return level;
}
/**
 * @brief 设置地图anchor
 *
 * @param map_id {uint32_t} map_id 地图实例id
 * @param anchor {mal_map_view_anchor_t} 地图view的锚点
 * @return int32_t {成功返回0, 失败返回error: -1表示没有初始化 -3初始化的线程和当前调用不一致}
 */
int32_t AwkMapController::mal_map_set_map_view_anchor(uint32_t map_id, mal_map_view_anchor_t anchor)
{
    if (map_id == 0) {
        AMAP_LOGE("Invalid map_id: %u", map_id);
        return -1;
    }
    if (anchor.anchor_x < 0.0f || anchor.anchor_x > 1.0f || anchor.anchor_y < 0.0f || anchor.anchor_y > 1.0f) {
        AMAP_LOGE("Invalid anchor: anchor_x=%.2f, anchor_y=%.2f - must be in [0.0, 1.0]", anchor.anchor_x,
                 anchor.anchor_y);
        return -1;
    }

    awk_map_view_anchor_t awk_anchor;
    awk_anchor.anchor_x = anchor.anchor_x;
    awk_anchor.anchor_y = anchor.anchor_y;

    int32_t ret = awk_map_set_map_view_anchor(map_id, awk_anchor);
    if (ret != 0) {
        AMAP_LOGE("Failed to set map view anchor for map_id=%u, error=%d", map_id, ret);
        return -1;
    }
    AMAP_LOGD("Map view anchor set successfully: map_id=%u, anchor_x=%.2f, anchor_y=%.2f", map_id, anchor.anchor_x,
             anchor.anchor_y);

    return 0;
}
/**
 * @brief 设置可见地区矩形区域
 * @param {uint32_t} map_id 地图实例id
 * @param {mal_navi_point_bounds_t} bounds  区域
 * @param {mal_edge_insets_t} insets  边缘插入
 * @return {成功返回0, 失败返回error: -1表示没有初始化 -3初始化的线程和当前调用不一致}
 */
int32_t AwkMapController::mal_map_set_visible_bounds(uint32_t map_id, mal_navi_point_bounds_t bounds,
                                                     mal_edge_insets_t insets)
{
    if (map_id == 0) {
        AMAP_LOGE("Invalid map_id: %u", map_id);
        return -1;
    }
    if (bounds.north_east.lon < bounds.south_west.lon || bounds.north_east.lat < bounds.south_west.lat) {
        AMAP_LOGE("Invalid bounds: NE(%.6f,%.6f), SW(%.6f,%.6f) - not a valid geographic rectangle",
                 bounds.north_east.lon, bounds.north_east.lat, bounds.south_west.lon, bounds.south_west.lat);
        return -1;
    }

    awk_navi_point_bounds_t awk_bounds = {.north_east = {.lon = bounds.north_east.lon, .lat = bounds.north_east.lat},
                                          .south_west = {.lon = bounds.south_west.lon, .lat = bounds.south_west.lat}};

    awk_edge_insets_t awk_insets = {
        .top = insets.top, .left = insets.left, .bottom = insets.bottom, .right = insets.right};

    int32_t ret = awk_map_set_visible_bounds(map_id, awk_bounds, awk_insets);
    if (ret != 0) {
        AMAP_LOGE("Failed to set visible bounds, error=%d", ret);
        return -1;
    }

    AMAP_LOGD("Visible bounds set successfully: map_id=%u, "
             "NE(%.6f,%.6f), SW(%.6f,%.6f)",
             map_id, bounds.north_east.lon, bounds.north_east.lat, bounds.south_west.lon, bounds.south_west.lat);

    return 0;
}

/*********************
 *    循迹导航 参见mal_map_navi.c
 *********************/

/*********************
 *    在线地图缓存
 *********************/
/**
 * @brief 在线请求并进行缓存瓦片，SDK内部会判断是否已经存在磁盘缓存和离线数据，如果存在缓存则不会走在线请求
 * @param {mal_rect_area_t} req_rect 需要缓存的屏幕区域
 * @param {mal_map_coord2d_t} *map_center 需要缓存的地图中心点
 * @param {float} map_level 需要缓存的地图级别
 * @param {int32_t} 请求的优先级，值请参考mal_http_priority
 * @return {返回大于0的值为请求id, 失败返回error: -1表示没有初始化 -3初始化的线程和当前调用不一致 -4表示参数非法}
 */
int32_t AwkMapController::mal_map_request_tiles(mal_rect_area_t req_rect, mal_map_coord2d_t map_center, float map_level,
                                                int32_t priority)
{
    if (req_rect.width <= 0 || req_rect.height <= 0) {
        AMAP_LOGE("Invalid req_rect: [x=%d,y=%d,w=%d,h=%d] - dimensions must be positive", req_rect.x, req_rect.y,
                 req_rect.width, req_rect.height);
        return -1;
    }
    if (map_center.lon < LNG_MIN || map_center.lon > LNG_MAX || map_center.lat < LAT_MIN || map_center.lat > LAT_MAX) {
        AMAP_LOGE("Invalid map_center: lon=%.6f, lat=%.6f", map_center.lon, map_center.lat);
        return -1;
    }
    if (map_level < MAP_LEVEL_MIN || map_level > MAP_LEVEL_MAX) {
        AMAP_LOGE("Invalid map_level: %.2f - must be in [3.0, 20.0]", map_level);
        return -1;
    }
    awk_rect_area_t awk_rect = {.x = req_rect.x, .y = req_rect.y, .width = req_rect.width, .height = req_rect.height};

    awk_map_coord2d_t awk_center = {.lon = map_center.lon, .lat = map_center.lat};

    int32_t request_id = awk_map_request_tiles(awk_rect, awk_center, map_level, priority);
    if (request_id <= 0) {
        AMAP_LOGE("awk_map_request_tiles failed, returned: %d", request_id);
        return -1;  // 底层未初始化或系统错误
    }

    AMAP_LOGD("Tile request success: id=%d, rect=[%d,%d,%d,%d], center=(%.6f,%.6f), level=%.2f", request_id, req_rect.x,
             req_rect.y, req_rect.width, req_rect.height, map_center.lon, map_center.lat, map_level);

    return request_id;  // 成功返回正数请求ID
}
/**
 * @brief 取消在线请求缓存瓦片
 * @param {int32_t} req_id 请求id
 * @return {成功返回0,  失败返回error: -1表示没有初始化 -3初始化的线程和当前调用不一致}
 */
int32_t AwkMapController::mal_map_cancel_request_tiles(int32_t req_id)
{
    if (req_id <= 0) {
        AMAP_LOGW("Invalid request ID: %d - ignored but treated as success", req_id);
        return 0;
    }
    int32_t ret = awk_map_cancel_request_tiles(req_id);
    if (ret != 0) {
        AMAP_LOGE("Failed to cancel tile request (req_id=%d), error=%d", req_id, ret);
        return -1;  // 映射到底层未初始化或其他系统错误
    }
    AMAP_LOGD("Tile request cancelled successfully: req_id=%d", req_id);
    return 0;  // 成功
}

/*********************
 *  工具
 *********************/
float AwkMapController::mal_map_calc_points_distance(double lon1, double lat1, double lon2, double lat2)
{
    if (lon1 < LNG_MIN || lon1 > LNG_MAX || lat1 < LAT_MIN || lat1 > LAT_MAX) {
        AMAP_LOGE("Invalid point 1: lon=%.6f, lat=%.6f", lon1, lat1);
        return 0.0;
    }
    if (lon2 < LNG_MIN || lon2 > LNG_MAX || lat2 < LAT_MIN || lat2 > LAT_MAX) {
        AMAP_LOGE("Invalid point 2: lon=%.6f, lat=%.6f", lon2, lat2);
        return 0.0;
    }

    float distance = static_cast<float>(awk_map_calc_points_distance(lon1, lat1, lon2, lat2));
    return distance;  // 单位：米
}
/**
 * @brief WGS84坐标转AMap坐标
 *
 * @param lng_wgs WGS84经度
 * @param lat_wgs WGS84纬度
 * @param lng_mgs AMap经度
 * @param lat_mgs AMap纬度
 */
void AwkMapController::mal_map_wgs84_to_mgs(double lng_wgs, double lat_wgs, double *lng_mgs, double *lat_mgs)
{
    if (lng_mgs == nullptr || lat_mgs == nullptr) {
        AMAP_LOGE("Output parameters 'lng_mgs' or 'lat_mgs' is nullptr");
        return;
    }
    if (lng_wgs < LNG_MIN || lng_wgs > LNG_MAX || lat_wgs < LAT_MIN || lat_wgs > LAT_MAX) {
        AMAP_LOGE("Invalid WGS84 coordinate: lng=%.6f, lat=%.6f", lng_wgs, lat_wgs);
        *lng_mgs = lng_wgs;
        *lat_mgs = lat_wgs;
        return;
    }
    awk_map_wgs84_to_mgs(lng_wgs, lat_wgs, lng_mgs, lat_mgs);
}

//---------------------------------循迹导航----------------------

/** * @brief 解析gpx */
mal_map_track_info_t *AwkMapController::mal_map_track_navi_parse_gpx(const char *gpx_data, size_t length)
{
    if (!gpx_data || length == 0) {
        AMAP_LOGE("GPX data is nullptr or length is 0");
        return nullptr;
    }
    awk_map_track_info_t *awk_map_track_info = awk_map_track_navi_parse_gpx(gpx_data, length);
    if (!awk_map_track_info) {
        AMAP_LOGE("Failed to parse GPX data");
        return nullptr;
    }
    mal_map_track_info_t *mal_map_track_info = (mal_map_track_info_t *)amap_mem_malloc(sizeof(mal_map_track_info_t));
    if (!mal_map_track_info) {
        AMAP_LOGE("Failed to allocate memory for mal_map_track_info_t");
        awk_map_track_navi_parse_track_info_free(awk_map_track_info);
        return nullptr;
    }
    memset_s(mal_map_track_info, sizeof(mal_map_track_info_t), 0, sizeof(mal_map_track_info_t));

    awk_to_mal_map_track_info_t(awk_map_track_info, mal_map_track_info);

    awk_map_track_navi_parse_track_info_free(awk_map_track_info);  // Assuming this frees the awk struct
    return mal_map_track_info;
}
char *AwkMapController::mal_poi_aos_json(const char *search_name)
{
    return nullptr;
}
char *AwkMapController::mal_route_aos_json(char *origin_coord, char *dest_coord, int mode)
{
    return nullptr;
}
/** * @brief 解析json */
mal_map_track_info_count_t *AwkMapController::mal_map_track_parser_json(const char *json_data, int mode)
{
    return nullptr;
}
/** * @brief 解析json */
mal_map_poi_t *AwkMapController::mal_poi_search_json(const char *json_str)
{
    return nullptr;
}
// 在使用完poi信息之后释放mal_map_poi_t缓存
void AwkMapController::mal_map_poi_mem_free(mal_map_poi_t *mal_map_poi_info)
{
    for (int i = 0; i < mal_map_poi_info->count; i++) {
        mal_map_poi_info_t *poi = &(mal_map_poi_info->poi_info[i]);
        amap_mem_free(poi->name);
        amap_mem_free(poi->id);
        amap_mem_free(poi->type);
        amap_mem_free(poi->pname);
        amap_mem_free(poi->cityname);
        amap_mem_free(poi->adname);
        amap_mem_free(poi->address);
        // 释放business子结构体
        amap_mem_free(poi->business.keytag);
        amap_mem_free(poi->business.tel);
        amap_mem_free(poi->business.opentime_week);
        amap_mem_free(poi->business.business_area);
    }
    amap_mem_free(mal_map_poi_info->poi_info);
    amap_mem_free(mal_map_poi_info);
}
void AwkMapController::mal_map_track_navi_parse_track_info_count_free(mal_map_track_info_count_t *info)
{
    if (info == nullptr) {
        AMAP_LOGE("info is nullptr");
        return;
    }
    for (int32_t i = 0; i < info->count; i++) {
        mal_map_track_info_t *track = info->tracks[i];
        if (track == nullptr) {
            continue;
        }
        awk_to_mal_map_track_info_free(track);
        amap_mem_free(track);
    }
    amap_mem_free(info);
}
// 析构track info
void AwkMapController::mal_map_track_navi_parse_track_info_free(mal_map_track_info_t *track_info)
{
    if (!track_info) {
        return;
    }

    awk_to_mal_map_track_info_free(track_info);
    amap_mem_free(track_info);
}
/** * @brief 初始化循迹导航覆盖物 */
int32_t AwkMapController::mal_map_init_track_navi_overlay(mal_map_track_navi_overlay_t *overlay)
{
    if (!overlay) {
        AMAP_LOGE("Overlay parameter is nullptr");
        return -1;
    }

    awk_map_track_navi_overlay_t *awk_overlay = (awk_map_track_navi_overlay_t *)overlay;

    int32_t ret = awk_map_init_track_navi_overlay(awk_overlay);
    if (ret != 0) {
        AMAP_LOGE("Failed to initialize awk track navi overlay, ret=%d", ret);
    }
    AMAP_LOGI("Track navigation overlay initialized in-place at %p", (void *)overlay);
    return ret;
}

/**
 * @brief 添加轨迹导航覆盖物
 *
 * @param map_id 地图view id
 * @param overlay 覆盖物
 * @return int32_t {成功返回0, 失败返回error: -1表示没有初始化,
 -2表示没有找到guid对应的覆盖物，-3表示传入的overlay参数为NULL或者geometry_type类型错误 -4初始化的线程和当前调用不一致
 -5未开放轨迹导航功能}
 */
int32_t AwkMapController::mal_map_track_navi_add_overlay(uint32_t map_id, mal_map_track_navi_overlay_t *overlay)
{
    if (!overlay) {
        AMAP_LOGE("Overlay parameter is nullptr");
        return -1;
    }
    // todo 是否墙砖？
    awk_map_track_navi_overlay_t *awk_overlay = (awk_map_track_navi_overlay_t *)overlay;

    int32_t ret = awk_map_track_navi_add_overlay(map_id, awk_overlay);
    if (ret != 0) {
        AMAP_LOGE("Failed to add track navi overlay to map engine, error code: %d", ret);
    }
    return ret;
}
/**
 * @brief 移除轨迹导航覆盖物
 *
 * @param map_id 地图view id
 * @param overlay_id 覆盖物id
 * @return int32_t {成功返回0, 失败返回error: -1表示没有初始化, -2表示没有找到guid对应的覆盖物
 * -3初始化的线程和当前调用不一致 -4未开放轨迹导航功能}
 */
int32_t AwkMapController::mal_map_track_navi_remove_overlay(uint32_t map_id, uint32_t overlay_id)
{
    if (map_id == 0) {
        AMAP_LOGE("Invalid map_id: %u", map_id);
        return -1;
    }
    if (overlay_id == 0) {
        AMAP_LOGE("Invalid overlay_id (guid): %u", overlay_id);
        return -1;
    }

    int32_t ret = awk_map_track_navi_remove_overlay(map_id, overlay_id);
    if (ret != 0) {
        AMAP_LOGE("Failed to remove track navi overlay: map_id=%u, guid=%u, error=%d", map_id, overlay_id, ret);
    } else {
        AMAP_LOGI("Track navigation overlay removed successfully: map_id=%u, guid=%u", map_id, overlay_id);
    }

    return ret;
}
/**
 * @brief 更新真实点
 *
 * @param map_id 地图view id
 * @param point 位置点
 * @param bear 角度
 * @param overlay 覆盖物
 */
void AwkMapController::mal_map_track_navi_update_real_point(uint32_t map_id, mal_map_coord2d_t point, float bear,
                                                            mal_map_track_navi_overlay_t *overlay)
{
    if (!overlay) {
        AMAP_LOGE("Overlay is nullptr");
        return;
    }
    if (map_id == 0) {
        AMAP_LOGE("Invalid map_id: %u", map_id);
        return;
    }

    awk_map_track_navi_overlay_t *awk_overlay = (awk_map_track_navi_overlay_t *)overlay;

    awk_map_coord2d_t awk_point = {.lon = point.lon, .lat = point.lat};

    awk_map_track_navi_update_real_point(map_id, awk_point, bear, awk_overlay);
    AMAP_LOGD("Updated real point: lon=%.6f, lat=%.6f, bear=%.2f°, map_id=%u", point.lon, point.lat, bear, map_id);
}
/**
 * @brief 获取距离,获取最近的一个类型点距离
 *
 * @param map_id 地图view id
 * @param point 位置点
 * @param point_type 点的类型
 * @param overlay 覆盖物
 * @return double
 */
double AwkMapController::mal_map_track_navi_get_distance(uint32_t map_id, mal_map_coord2d_t point,
                                                         mal_map_track_point_type point_type,
                                                         mal_map_track_navi_overlay_t *overlay)
{
    // --- 参数校验 ---
    if (!overlay) {
        AMAP_LOGE("Overlay is nullptr");
        return -1.0;
    }
    if (map_id == 0) {
        AMAP_LOGE("Invalid map_id: %u", map_id);
        return -1.0;
    }

    awk_map_track_navi_overlay_t *awk_overlay = (awk_map_track_navi_overlay_t *)overlay;

    awk_map_track_point_type awk_point_type;
    switch (point_type) {
        case MAL_MAP_TRACK_POINT_START:
            awk_point_type = AWK_MAP_TRACK_POINT_START;
            break;
        case MAL_MAP_TRACK_POINT_PASS:
            awk_point_type = AWK_MAP_TRACK_POINT_PASS;
            break;
        case MAL_MAP_TRACK_POINT_TURN:
            awk_point_type = AWK_MAP_TRACK_POINT_TURN;
            break;
        case MAL_MAP_TRACK_POINT_FINISH:
            awk_point_type = AWK_MAP_TRACK_POINT_FINISH;
            break;
        default:
            AMAP_LOGE("Unknown point type: %d", point_type);
            return -1.0;
    }

    awk_map_coord2d_t awk_point = {.lon = point.lon, .lat = point.lat};

    double distance = awk_map_track_navi_get_distance(map_id, awk_point, awk_point_type, awk_overlay);
    AMAP_LOGD("Get distance to type=%d: %.2f meters (map_id=%u)", point_type, distance, map_id);
    return distance;
}

}  // namespace OHOS