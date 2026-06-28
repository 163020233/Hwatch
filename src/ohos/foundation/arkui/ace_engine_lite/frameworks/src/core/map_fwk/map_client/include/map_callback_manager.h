/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map callabck.
 * Author: CompanyName
 * Create: 2026-01-07
 */
#ifndef MAP_CALLBACK_MANAGER_H
#define MAP_CALLBACK_MANAGER_H

#include <map>
#include "http/http_module_client.h"
#include "httpclient.h"

namespace OHOS {
namespace ACELite {

typedef std::function<void(const std::map<std::string, std::string> &)> POISearchCallback;

enum MapCallbackType { MAP_NONE, MAP_CALLBACK, MAP_POI_CALLBACK };

class CallbackManager {
public:
    CallbackManager();
    ~CallbackManager();

    // 存储回调并返回唯一ID
    uint32_t AddCallback(MapCallback callback);
    uint32_t AddPOICallback(POISearchCallback callback);

    // 获取并执行回调
    void ExecuteCallback(uint32_t id, const char *result);
    void ExecutePOICallback(uint32_t id, const std::map<std::string, std::string> &result);

    // 清理回调
    void RemoveCallback(uint32_t id);

    // 统一接收回调
    MapCallbackType GetMapCallbackType(uint32_t id);

private:
    void CheckAndCleanup();

    std::mutex mutex_;
    uint32_t nextId_ = 1;
    std::map<uint32_t, MapCallback> callbacks_;
    std::map<uint32_t, POISearchCallback> poiCallbacks_;
    std::map<uint32_t, MapCallbackType> typeCallbacks_;
};

}  // namespace ACELite
}  // namespace OHOS
#endif  // MAP_CALLBACK_MANAGER_H
