/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_BUNDLE_MANAGER_SERVICE_H
#define OHOS_BUNDLE_MANAGER_SERVICE_H

#include <map>
#include <vector>
#include <serializer.h>

#ifdef LITEWEARABLE_SUPPORT_ENABLED
#include "bundle_install_msg.h"
#endif
#include "ability_service_interface.h"
#include "adapter.h"
#include "bundle_installer.h"
#include "bundle_info.h"
#include "bundle_map.h"
#include "cJSON.h"
#include "message.h"
#include "nocopyable.h"
#include "stdint.h"

namespace OHOS {
class ManagerService {
public:
    static ManagerService &GetInstance()
    {
        static ManagerService instance;
        return instance;
    }
// ohos begin
// bundle_framework_lite, hi3321_support
    bool IsExternalInstallMode() const;
    BundleInfo *QueryBundleInfo(const char *bundleName);
    void RemoveBundleInfo(const char *bundleName);
    std::string GetCodeDirPath() const;
    std::string GetDataDirPath() const;
    bool IsDebugMode() const;
    void AddBundleInfo(BundleInfo *info);
    bool UpdateBundleInfo(BundleInfo *info);
    void Install(const char *path, int32_t installLocation, InstallerCallback installerCallback);
    int32_t ReportInstallCallback(uint8_t errCode, uint8_t installState,
        uint8_t process, InstallerCallback installerCallback);
#ifdef OHOS_DEBUG
    uint8_t SetSignMode(bool enable);
    bool IsSignMode() const;
#endif
private:
    ManagerService();
    ~ManagerService();

    BundleInstallMsg *bundleInstallMsg_;

    std::map<int, std::string> sysUidMap_;
    std::map<int, std::string> sysVendorUidMap_;
    std::map<int, std::string> appUidMap_;
    BundleInstaller *installer_;
    BundleMap *bundleMap_;
    std::vector<SvcIdentity> svcIdentity_;
    bool IsExternalInstallMode_ { false };
    bool isDebugMode_ { false };
#ifdef OHOS_DEBUG
    bool isSignMode_ { true };
#endif

    DISALLOW_COPY_AND_MOVE(ManagerService);
};
// ohos end
}
#endif // OHOS_BUNDLE_MANAGER_SERVICE_H
