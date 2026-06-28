/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map interface implement.
 * Author: CompanyName
 * Create: 2026-01-07
 */

#include "map_interface_impl.h"
#include "ace_log.h"
#include "http/http_callback.h"
#include "cJSON.h"

namespace OHOS {
namespace ACELite {

const std::string MapInterfaceImpl::MAP_REQUEST_SCHEME = "http://";
const std::string MapInterfaceImpl::MAP_REQUEST_DOMAIN = "restapi.amap.com/";
const std::string MapInterfaceImpl::MAP_REQUEST_PATH_V3 = "v3/";
const std::string MapInterfaceImpl::MAP_REQUEST_PATH_V4 = "v4/";
const std::string MapInterfaceImpl::MAP_REQUEST_PATH_V5 = "v5/";

char *MapInterfaceImpl::GetUrl(const std::string &path)
{
    if (path.empty()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetUrl: path is empty");
        return nullptr;
    }

    char *buf = static_cast<char *>(ace_malloc(path.size() + 1));
    if (buf == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetUrl: malloc failed");
        return nullptr;
    }

    if (memcpy_s(buf, path.size() + 1, path.c_str(), path.size() + 1) != EOK) {
        ace_free(buf);
        HILOG_ERROR(HILOG_MODULE_ACE, "GetUrl: memcpy_s failed");
        return nullptr;
    }

    return buf;
}

bool MapInterfaceImpl::SendHttpRequest(HttpModuleClient *client, const char *url, uint32_t id, const char *filePath)
{
    if (client == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SendHttpRequest client is nullptr");
        return false;
    }
    HttpMessage message{};
    message.param.url = url;
    message.param.header = nullptr;
    message.requestId = OHOS::ACELite::HTTP_MAP_REQUESTINSTREAM;
    message.requestType = HTTP_GET;
    message.dataType = 0;
    message.mapCallbackId = id;
    message.param.filePath = filePath;
    osStatus_t ret = client->HttpMessagePut(&message);
    if (ret != osOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SendHttpRequest failed, ret=%d", ret);
        return false;
    }
    return true;
}

std::string MapInterfaceImpl::UrlEncode(const std::string& value)
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value) {
        // 保留字符：字母、数字、短横线、下划线、点、波浪号、逗号不编码
        if (std::isalnum(static_cast<unsigned char>(c)) ||
            c == '-' || c == '_' || c == '.' || c == '~' || c == ',') {
            escaped << c;
        } else if (c == ' ') {
            escaped << "%20";   // 也可编码为 '+'，但 %20 更通用
        } else {
            // 其他字符编码为 %XX
            escaped << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
        }
    }
    return escaped.str();
}

bool MapInterfaceImpl::ExecuteMapRequest(const std::map<std::string, std::string> &args, const std::string &requestPath,
    const std::string &version, HttpModuleClient *client, uint32_t id)
{
    if (args.empty()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Map request args is empty");
        return false;
    }

    if (client == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Map request clinet is nullptr");
        return false;
    }

    std::string requestUrl =
        MapInterfaceImpl::MAP_REQUEST_SCHEME + MapInterfaceImpl::MAP_REQUEST_DOMAIN + version + requestPath + "?";
    std::string argUrl;

    for (const auto &arg : args) {
        argUrl += UrlEncode(arg.first) + "=" + UrlEncode(arg.second) + "&";
    }

    if (argUrl.empty()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Map request args is error");
        return false;
    } else {
        argUrl.pop_back();
    }
    requestUrl += argUrl;

    char *url = GetUrl(requestUrl);
    if (url == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Get Url failed");
        return false;
    }

    bool success = SendHttpRequest(client, url, id, nullptr);
    if (!success) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SendHttpRequest error");
        ace_free(url);
        return false;
    }
    return true;
}

bool MapInterfaceImpl::ExecuteMapRequest(const char *url, HttpModuleClient *client, uint32_t id, const char *filePath)
{
    if (url == nullptr || client == nullptr || filePath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Map request url , client , filePath is empty");
        return false;
    }

    char *urlPath = GetUrl(url);
    char *path = GetUrl(filePath);

    if (urlPath == nullptr || path == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Get Url or filePath failed");
        return false;
    }

    bool success = SendHttpRequest(client, urlPath, id, path);
    if (!success) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SendHttpRequest error");
        ace_free(urlPath);
        ace_free(path);
        return false;
    }
    return true;
}

bool MapInterfaceImpl::MapPOISearchRequests(
    const std::map<std::string, std::string> &args, HttpModuleClient *client, uint32_t id)
{
    return ExecuteMapRequest(args, "place/text", MAP_REQUEST_PATH_V5, client, id);
}

bool MapInterfaceImpl::MapBikePathRequests(
    const std::map<std::string, std::string> &args, HttpModuleClient *client, uint32_t id)
{
    return ExecuteMapRequest(args, "direction/bicycling", MAP_REQUEST_PATH_V5, client, id);
}

bool MapInterfaceImpl::MapWalkPathRequests(
    const std::map<std::string, std::string> &args, HttpModuleClient *client, uint32_t id)
{
    return ExecuteMapRequest(args, "direction/walking", MAP_REQUEST_PATH_V5, client, id);
}

bool MapInterfaceImpl::GetMapOfflineMapListRequests(
    const std::map<std::string, std::string> &args, HttpModuleClient *client, uint32_t id)
{
    return ExecuteMapRequest(args, "rtos/offlinemap/city_pack_list", MAP_REQUEST_PATH_V5, client, id);
}

bool MapInterfaceImpl::DownOfflineMapRequests(
    const std::map<std::string, std::string> &args, HttpModuleClient *client, uint32_t id)
{
    return ExecuteMapRequest(args, "rtos/offlinemap/city_pack", MAP_REQUEST_PATH_V5, client, id);
}

bool MapInterfaceImpl::DownOfflineMapRequests(const char *url, HttpModuleClient *client, uint32_t id, const char *filePath)
{
    return ExecuteMapRequest(url, client, id, filePath);
}
}  // namespace ACELite
}  // namespace OHOS
