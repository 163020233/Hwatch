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

#ifndef OHOS_HAP_VERIFY_H
#define OHOS_HAP_VERIFY_H

#include "app_verify_pub.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

#include "stdint.h"

#include <string>
#include <vector>

namespace OHOS {
// ohos begin
// bundle_framework_lite, hi3321_support
struct HapSignatureInfo {
    std::string appId;
    std::string provisionBundleName;
    std::vector<std::string> restrictedPermissions;
};
// ohos end
class HapSignVerify {
public:
// ohos begin
// bundle_framework_lite, hi3321_support
    static uint8_t VerifySignature(const std::string &hapFilepath, HapSignatureInfo &signatureInfo);
// ohos end
private:
    HapSignVerify() = default;
    ~HapSignVerify() = default;
    static uint8_t SwitchErrorCode(int32_t errorCode);
};
} // namespace OHOS
#endif // OHOS_HAP_VERIFY_H