/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_LIST_ADAPTER_EXT_H
#define OHOS_ACELITE_LIST_ADAPTER_EXT_H

#include <string>
#include <vector>
#include "text_component.h"

namespace OHOS {
namespace ACELite {

class ListAdapterExt {
public:
    ListAdapterExt();

    ~ListAdapterExt();

    static void SetItemAdaptive(bool isItemAdaptive);

    static bool GetItemAdaptive();

    static void SetListValue(char *text);

    static void SetItemIndex(int16_t index);

    static int16_t GetItemIndex();

    static int16_t BuildItemAdaptive(Component *component, int16_t index);

    static void CleanVector();

    static void CleanString();

    static void CleanAll();

private:
    static int16_t CalculateItems(UILabel *uiLable, int16_t index);
    static bool isItemAdaptive_;
    static int16_t itemIndex_;
    static std::vector<std::string> strings_;
    static std::string all_;
};
}  // namespace ACELite
}  // namespace OHOS
#endif // OHOS_ACELITE_LIST_ADAPTER_EXT_H
