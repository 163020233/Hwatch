/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: UIWidgetUtils - Common UI widget lifecycle utilities
 * Created: 2026-01-14
 */

#ifndef UI_WIDGET_UTILS_H
#define UI_WIDGET_UTILS_H

#include <memory>

namespace OHOS {
namespace UIWidgetUtils {

/**
 * @brief 从父容器中移除子控件并释放资源
 * @param parent 父容器（需提供 Remove 接口）
 * @param child  子控件（unique_ptr 管理）
 */
template <typename ParentT, typename WidgetT>
inline void RemoveChild(ParentT *parent, std::unique_ptr<WidgetT> &child)
{
    if (parent && child) {
        parent->Remove(child.get());
        child.reset();
    }
}

}  // namespace UIWidgetUtils
}  // namespace OHOS

#endif  // UI_WIDGET_UTILS_H