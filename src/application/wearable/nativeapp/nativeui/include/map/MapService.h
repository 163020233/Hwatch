/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: MapService - Network requests and JSON parsing
 * Created: 2026-01-14
 */

#ifndef MAP_SERVICE_H
#define MAP_SERVICE_H

#include <functional>
#include <string>
#include <vector>
#include <map>
#include "mal_common_defines.h"
#include "MapModel.h"
#include "map_client.h"
#include "cJSON.h"

namespace OHOS {
// ===== 离线地图数据结构 =====
struct OfflineRegion {
    std::string name;       // "上海市" / "虹口区"
    std::string level;      // "province" / "city" / "district"
    std::string adcode;     // "310000"
    int64_t size = 0;       // 字节
    std::vector<OfflineRegion> children;
};

struct OfflineMapDownloadInfo {
    std::string adcode;
    std::string version;
    std::vector<std::string> urls;
};

enum class OfflineDownloadStage {
    LIST,       // 获取目录
    AUTH,       // 鉴权获取 URL
    DOWNLOAD,   // 下载文件
};

// 回调类型
using OfflineMapListCallback     = std::function<void(bool success, OfflineRegion &&root)>;
using OfflineMapAuthCallback     = std::function<void(bool success, OfflineMapDownloadInfo &&info)>;

using PoiSearchCallback = std::function<void(bool success, std::vector<MapResultItem> &&results)>;
using RoutePlanCallback = std::function<void(bool success, std::vector<RouteInfo> &&routes)>;

class MapService {
public:
    static MapService *GetInstance();
    MapService(const MapService &) = delete;
    MapService &operator=(const MapService &) = delete;
    MapService(MapService &&) = delete;
    MapService &operator=(MapService &&) = delete;

    /**
     * @brief 异步 POI 搜索
     * @param keyword 搜索关键词（由 Page 传入）
     * @param callback 结果回调
     */
    void SearchPOI(const std::string &keyword, PoiSearchCallback callback);

    /**
     * @brief 异步骑行路线规划
     * @param origin "lon,lat"
     * @param destination "lon,lat"
     * @param callback 结果回调
     */
    void PlanBikeRoute(const std::string &origin, const std::string &destination, RoutePlanCallback callback);

    /**
    * @brief 获取离线地图目录（按省 adcode 查询）
    * @param adcode 省级 adcode，例如 "310000"
    * @param callback 结果回调（树形结构）
    */
    void GetOfflineMapList(const std::string &adcode, OfflineMapListCallback callback);

    /**
    * @brief 下载离线地图（鉴权 + 下载）
    * @param adcode 目标 adcode
    * @param callback 完成回调
    */
    void DownloadOfflineMap(const std::string &adcode);

    bool ExtractOfflineMap(const char *zipPath, const char *outDir);

    ACELite::MapClient *GetClient();

    void ClearClient();

private:
    MapService() : client_(nullptr) {}
    ~MapService();
    // JSON 解析（纯函数，不依赖任何 UI）
    static std::vector<MapResultItem> ParsePoiResult(const std::map<std::string, std::string> &result);
    static std::vector<RouteInfo> ParseRouteJson(const char *json);
    static void ParsePolylineString(const std::string &str, std::vector<ACELite::map_coord2d> &outPoints);
    static void LogJsonChunks(const char *json);
    static RouteStep ParseSingleStep(cJSON *item, std::vector<ACELite::map_coord2d> &polylinePoints, int stepIndex);
    static void ParseSteps(cJSON *path, RouteInfo &info);
    static RouteInfo ParseSinglePath(cJSON *path, int index);

    // 离线地图 JSON 解析
    static bool ParseOfflineMapList(const char *json, OfflineRegion &outRoot);
    static void ParseOfflineRegionNode(cJSON *node, OfflineRegion &out);
    void GetOfflineMapAuth(const std::string &adcode, OfflineMapAuthCallback callback);
    static bool ParseOfflineMapAuth(const char *json, OfflineMapDownloadInfo &outInfo);

    // 文件路径构造
    static std::string BuildOfflineMapFilePath(const std::string &adcode);
private:
    ACELite::MapClient *client_;
};

}  // namespace OHOS

#endif  // MAP_SERVICE_H
