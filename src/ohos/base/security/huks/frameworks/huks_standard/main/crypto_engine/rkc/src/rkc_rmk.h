/*
 * Copyright (c) 2020-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RKC_RMK_H
#define RKC_RMK_H

#include "hks_rkc_rw.h"
#include "hks_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

// ohos begin
// After the upgrade to OH5.0, a true random number is transferred for rootkey encryption.
int32_t RkcGetRmkRawKey(const struct HksKsfDataRkc *ksfDataRkc, const struct HksKsfDataMk *ksfDataMk,
    struct HksBlob *rawKey);
// ohos end
#ifdef __cplusplus
}
#endif

#endif /* RKC_RMK_H */
