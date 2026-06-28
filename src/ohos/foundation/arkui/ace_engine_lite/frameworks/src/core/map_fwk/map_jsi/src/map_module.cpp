/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: map module implement.
 * Author: CompanyName
 * Create: 2026-03-09
 */

#include "ace_log.h"
#include "map_module.h"

namespace OHOS {
namespace ACELite {

JSIValue map_searchpoi_callback = nullptr;
JSIValue map_searchpoi_context = nullptr;
MapClient *MapModule::mapClient_ = nullptr;

const char *const MapModule::POI_NAME = "poi";
// 开发临时调用KEY
const char *const MapModule::API_KEY = "cebd61d505fb29c68e8c7cfd8b255e46";

static bool CheckIsJsValueDefined(JSIValue jsValue)
{
    if (jsValue == nullptr) {
        return false;
    }

    if (JSI::ValueIsUndefined(jsValue)) {
        return false;
    }

    return true;
}

static void ReleaseJsValue(JSIValue &jsValue)
{
    if (CheckIsJsValueDefined(jsValue)) {
        JSI::ReleaseValue(jsValue);
        jsValue = JSI::CreateUndefined();
    }
    return;
}

static void HandlePoiResult(const std::map<std::string, std::string> &result)
{
    JSIValue dataInfo = JSI::CreateObject();

    for (const auto &pair : result) {
        JSI::SetStringProperty(dataInfo, pair.first.c_str(), pair.second.c_str());
    }

    JSIValue callbackArgv[OHOS::ACELite::ARGC_ONE] = {dataInfo};

    if (map_searchpoi_callback == nullptr || map_searchpoi_context == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapModule::HandlePoiResult map_searchpoi_callback or map_searchpoi_context invalid");
        JSI::ReleaseValue(dataInfo);
        return;
    }

    JSI::CallFunction(map_searchpoi_callback, map_searchpoi_context, callbackArgv, ARGC_ONE);

    JSI::ReleaseValue(dataInfo);
}

JSIValue MapModule::SearchPoi(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    char *data = JSI::GetStringProperty(args[0], POI_NAME);
    if (!data) {
        HILOG_ERROR(HILOG_MODULE_ACE, "MapModule::SearchPoi args is invalid");
        return JSI::CreateBoolean(false);
    }

    ReleaseJsValue(map_searchpoi_context);
    ReleaseJsValue(map_searchpoi_callback);
    map_searchpoi_context = JSI::AcquireValue(thisVal);
    map_searchpoi_callback = JSI::GetNamedProperty(args[0], CB_CALLBACK);

    if (mapClient_ == nullptr) {
        mapClient_ = new MapClient(false);
    }

    std::map<std::string, std::string> params;

    params["key"] = API_KEY;
    params["keywords"] = std::string(data);
    params["page_size"] = "5";
    params["page_num"] = "1";
    mapClient_->SearchPOI(params, HandlePoiResult);

    ace_free(data);
    return JSI::CreateBoolean(true);
}

void MapModule::OnDestroy()
{
    HILOG_INFO(HILOG_MODULE_ACE, "MapModule::OnDestroy");
    if (mapClient_ != nullptr) {
        delete mapClient_;
        mapClient_ = nullptr;
    }
    ReleaseJsValue(map_searchpoi_callback);
    ReleaseJsValue(map_searchpoi_context);
    map_searchpoi_context = nullptr;
    map_searchpoi_callback = nullptr;
}

void InitMapModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "SearchPoi", MapModule::SearchPoi);
    JSI::SetOnDestroy(exports, MapModule::OnDestroy);
}

}  // namespace ACELite
}  // namespace OHOS
