/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map callabck implement.
 * Author: CompanyName
 * Create: 2026-01-07
 */

#include "map_callback_manager.h"
#include <algorithm>
#include <memory>
#include "cmsis_os2.h"

namespace OHOS {
namespace ACELite {

CallbackManager::CallbackManager() : nextId_(1)
{}

CallbackManager::~CallbackManager()
{
    std::unique_lock<std::mutex> lock(mutex_);
    callbacks_.clear();
    poiCallbacks_.clear();
    typeCallbacks_.clear();
}

uint32_t CallbackManager::AddCallback(MapCallback callback)
{
    std::unique_lock<std::mutex> lock(mutex_);
    uint32_t id = nextId_++;
    callbacks_[id] = callback;
    typeCallbacks_[id] = MAP_CALLBACK;
    return id;
}

uint32_t CallbackManager::AddPOICallback(POISearchCallback callback)
{
    std::unique_lock<std::mutex> lock(mutex_);
    uint32_t id = nextId_++;
    poiCallbacks_[id] = callback;
    typeCallbacks_[id] = MAP_POI_CALLBACK;
    return id;
}

void CallbackManager::ExecuteCallback(uint32_t id, const char *result)
{
    std::unique_lock<std::mutex> lock(mutex_);
    MapCallback callback = nullptr;

    auto it = callbacks_.find(id);
    if (it != callbacks_.end()) {
        callback = it->second;
        callbacks_.erase(it);
    }

    auto type = typeCallbacks_.find(id);
    if (type != typeCallbacks_.end()) {
        typeCallbacks_.erase(type);
    }

    if (callback) {
        callback(result);
    }

    CheckAndCleanup();
}

void CallbackManager::ExecutePOICallback(uint32_t id, const std::map<std::string, std::string> &result)
{
    std::unique_lock<std::mutex> lock(mutex_);
    POISearchCallback callback = nullptr;
    auto it = poiCallbacks_.find(id);
    if (it != poiCallbacks_.end()) {
        callback = it->second;
        poiCallbacks_.erase(it);
    }

    auto type = typeCallbacks_.find(id);
    if (type != typeCallbacks_.end()) {
        typeCallbacks_.erase(type);
    }
    if (callback) {
        callback(result);
    }

    CheckAndCleanup();
}

MapCallbackType CallbackManager::GetMapCallbackType(uint32_t id)
{
    MapCallbackType type = MapCallbackType::MAP_NONE;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto it = typeCallbacks_.find(id);
        if (it != typeCallbacks_.end()) {
            type = it->second;
        }
    }
    return type;
}

void CallbackManager::CheckAndCleanup()
{
    if (callbacks_.empty() && poiCallbacks_.empty() && typeCallbacks_.empty()) {
        nextId_ = 1;
    }
}

void CallbackManager::RemoveCallback(uint32_t id)
{
    std::unique_lock<std::mutex> lock(mutex_);
    callbacks_.erase(id);
    poiCallbacks_.erase(id);
    typeCallbacks_.erase(id);
}

}  // namespace ACELite
}  // namespace OHOS
