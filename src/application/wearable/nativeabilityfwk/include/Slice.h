/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Slice
 * Create: 2025-11
 */

#ifndef SLICE_H
#define SLICE_H
#include "components/ui_view_group.h"

namespace OHOS {
extern const uint16_t SLICE_MASK;
extern const uint32_t PAGE_OFFSET;
enum class PauseReason : int16_t {
    CHANGE_SLICE = 0,
    ENTER_JS,
    ENTER_AOD,
    SCREEN_OFF,
    SHOW_NOTIFY,
    SWITCH_PAGE,
    SWITCH_PAGE_WITH_ANIM,
};

class Slice {
public:
    Slice() = default;
    virtual ~Slice() = default;
    /**
     * @brief Called when the Slice is started and before transition animator starts if exits.
     *
     *        Its system implemention is in the AbilitySlice, you can define your own processing logic by overriding
     *        Prenseter::OnStart And View::OnStart
     */
    virtual void OnStart(void* data) = 0;
    /**
     * @brief Called when the slice will be visible and after transition animator ends if exits
     *
     *        Its system implemention is in the AbilitySlice, you can define your own processing logic by overriding
     *        Prenseter::OnResume
     */
    virtual void OnResume() = 0;
    /**
     * @brief Called when another slice will be started and before transition animator starts if exits
     *
     *        Its system implemention is in the AbilitySlice, you can define your own processing logic by overriding
     *        Prenseter::OnPause
     */
    virtual void OnPause(PauseReason reason) = 0;
    /**
     * @brief Called when the slice will be invisible and another slice will be visible
     *
     * @brief Called when the slice maybe become invisible
     *
     *        Its system implemention is in the AbilitySlice, you can define your own processing logic by overriding
     *        Prenseter::OnStop and View::OnStop
     */
    virtual void OnStop() = 0;
    virtual UIViewGroup* GetSliceContainer() = 0;

    Slice(const Slice &) = delete;
    Slice &operator=(const Slice &) = delete;
    Slice(Slice &&) = delete;
    Slice &operator=(Slice &&) = delete;
};
} // namespace OHOS

#endif // SLICE_H
