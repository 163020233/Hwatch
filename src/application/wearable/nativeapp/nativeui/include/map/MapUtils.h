/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: MapUtils - Common map drawing utilities
 * Created: 2026-01-14
 */

#ifndef MAP_UTILS_H
#define MAP_UTILS_H

#include <vector>
#include "mal_common_defines.h"
#include "map_controller_interface.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"
#include "map_client.h"
#include "MapService.h"
#include "map_defines.h"

namespace OHOS {
namespace MapUtils {

static constexpr const char *BACK_BUTTON_ID = "BackButton";
static constexpr const char *ZOOM_IN_BUTTON_ID = "ZoomInButton";
static constexpr const char *ZOOM_OUT_BUTTON_ID = "ZoomOutButton";
static constexpr const char *POI_BUTTON_ID = "PoiButton";
static constexpr const char *DOWNLOAD_BUTTON_ID = "MapDownloadButton";
static constexpr const char *BACK_BUTTON_TEXT = "返回";
static constexpr const char *ZOOM_IN_BUTTON_TEXT = "+";
static constexpr const char *ZOOM_OUT_BUTTON_TEXT = "-";
static constexpr const char *POI_BUTTON_TEXT = "搜索";
static constexpr const char *DOWNLOAD_BUTTON_TEXT = "下载";
static constexpr uint16_t BUTTON_WIDTH = 80;
static constexpr uint16_t BUTTON_HEIGHT = 80;
static constexpr uint16_t BACK_BUTTON_WIDTH = 80;
static constexpr uint16_t BACK_BUTTON_HEIGHT = 36;
static constexpr uint16_t DOWNLOAD_BUTTON_WIDTH = 80;
static constexpr uint16_t DOWNLOAD_BUTTON_HEIGHT = 36;
static constexpr uint16_t BUTTON_RADIUS = 18;
static constexpr uint32_t NAVI_ARROW_SIZE = 60;
static constexpr uint32_t BGRA8888_BYTES_PER_PIXEL = 4;
static constexpr float DEFAULT_MAP_ZOOM = 12.0f;
static constexpr const char *NAVI_ARROW_PATH = "/user/data/map/arrow_circle.bin";

void CreateRoundButton(std::unique_ptr<UILabelButton> &button, int16_t posX, int16_t posY, const char *viewId,
    UIView::OnClickListener *listener, UIViewGroup *container, const char *text = "",
    uint16_t buttonWidth = BUTTON_WIDTH, uint16_t buttonHeight = BUTTON_HEIGHT);

// 线段样式
struct PolylineStyle {
    int32_t lineWidth = 6;
    int32_t lineColor = 0xFF4A90D9;
    int32_t borderWidth = 2;
    int32_t borderColor = 0xFF2A5FA0;
    bool useDash = false;
    int32_t dashPainted = 5;
    int32_t dashUnpainted = 8;
    int32_t dashOffset = 0;
};

/**
 * @brief 在地图上绘制 polyline 路线
 * @param controller    地图控制器
 * @param mapId         地图实例 ID
 * @param polyline      坐标点序列（GCJ02）
 * @param normalStyle   未走过部分的样式
 * @param focusStyle    已走过部分的样式（nullptr 则与 normalStyle 相同）
 * @param preIndex      已走过路线点的索引（默认 0 表示未走）
 * @return overlay guid，失败返回 -1
 */
int32_t DrawPolylineOverlay(IMapController *controller, int32_t mapId, const std::vector<ACELite::map_coord2d> &polyline,
    const PolylineStyle &normalStyle, const PolylineStyle *focusStyle = nullptr, uint32_t preIndex = 0);

/**
 * @brief 移除路线覆盖物
 * @param controller 地图控制器
 * @param mapId      地图实例 ID
 * @param overlayId  覆盖物 ID（移除后自动重置为 -1）
 */
void RemoveOverlay(IMapController *controller, int32_t mapId, int32_t& overlayId);

/**
 * @brief 设置地图中心并渲染
 */
void SetCenterAndRender(IMapController *controller, int32_t mapId, const ACELite::map_coord2d &center);

/**
 * - 调用者负责在不再使用时释放 texData.buffer(GrallocEngines::GetInstance()->FreeBuffer(buffer);)
 * - 文件大小必须等于 width * height * BGRA8888_BYTES_PER_PIXEL
 * - stride 固定为 BGRA8888_BYTES_PER_PIXEL
 */
bool LoadBGRA8888Texture(const char *filePath, ACELite::map_texture_data &texData, uint32_t width, uint32_t height);

int32_t SafeConvertToInt32(const char *str);

}  // namespace MapUtils
}  // namespace OHOS

#endif  // MAP_UTILS_H
