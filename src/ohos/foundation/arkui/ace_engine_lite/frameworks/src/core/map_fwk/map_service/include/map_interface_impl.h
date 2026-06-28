/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map interface implement.
 * Author: CompanyName
 * Create: 2026-01-07
 */
#ifndef MAP_INTERFACE_IMPLEMENT_H
#define MAP_INTERFACE_IMPLEMENT_H

#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "httpclient.h"
#include "http/http_module_client.h"

namespace OHOS {
namespace ACELite {

class MapInterfaceImpl : public MemoryHeap {
public:
    static bool MapPOISearchRequests(
        const std::map<std::string, std::string> &args, HttpModuleClient *client, uint32_t id);

    static bool MapBikePathRequests(
        const std::map<std::string, std::string> &args, HttpModuleClient *client, uint32_t id);

    static bool MapWalkPathRequests(
        const std::map<std::string, std::string> &args, HttpModuleClient *client, uint32_t id);

    static bool GetPOISearch(const std::map<std::string, std::string> &args, HttpModuleClient *client, uint32_t id);

    static bool GetMapOfflineMapListRequests(
        const std::map<std::string, std::string> &args, HttpModuleClient *client, uint32_t id);

    static bool DownOfflineMapRequests(
        const std::map<std::string, std::string> &args, HttpModuleClient *client, uint32_t id);

    static bool DownOfflineMapRequests(const char *url, HttpModuleClient *client, uint32_t id, const char *filePath);

private:
    MapInterfaceImpl();
    ~MapInterfaceImpl();

    static char *GetUrl(const std::string &str);

    static std::string UrlEncode(const std::string& value);

    static bool SendHttpRequest(HttpModuleClient *client, const char *url, uint32_t id, const char *filePath);

    static bool ExecuteMapRequest(const std::map<std::string, std::string> &args, const std::string &requestPath,
        const std::string &apiVersion, HttpModuleClient *client, uint32_t id);

    static bool ExecuteMapRequest(const char *url, HttpModuleClient *client, uint32_t id, const char *filePath);

    static const std::string MAP_REQUEST_SCHEME;
    static const std::string MAP_REQUEST_DOMAIN;
    static const std::string MAP_REQUEST_PATH_V3;
    static const std::string MAP_REQUEST_PATH_V4;
    static const std::string MAP_REQUEST_PATH_V5;
};

}  // namespace ACELite
}  // namespace OHOS
#endif  // MAP_INTERFACE_IMPLEMENT_H
